#include "driver.h"
#include "tinyopcpu_ioctl.h"
#include "tinyopcpu_oper.h"

#pragma code_seg("INIT")
NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING pRegistryPath)
{

	PDEVICE_OBJECT pdeviceObject;
	NTSTATUS ntstatus;
	UNICODE_STRING deviceName, dosName;
	PDEVICE_EXTENSION pfdo;
	UNREFERENCED_PARAMETER(pRegistryPath);

	RtlInitUnicodeString(&deviceName, NT_DEVICE_NAME);
	RtlInitUnicodeString(&dosName, DOS_DEVICE_NAME);
	ntstatus = IoCreateDevice(pDriverObject, sizeof(pfdo), &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pdeviceObject);
	if (!NT_SUCCESS(ntstatus))
	{
		return ntstatus;
	}
	RtlZeroMemory(pdeviceObject->DeviceExtension, sizeof(pfdo));
	pfdo = pdeviceObject->DeviceExtension;
	pfdo->self = pdeviceObject;
	pfdo->nextStackDevice = NULL;
	pdeviceObject->Flags |= DO_BUFFERED_IO;
	ntstatus = IoCreateSymbolicLink(&dosName, &deviceName);
	if (!NT_SUCCESS(ntstatus))
	{
		IoDeleteDevice(pdeviceObject);
		return ntstatus;
	}
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = tinyOpCPU_Create;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = tinyOpCPU_Close;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = tinyOpCPU_DeviceControl;
	pDriverObject->DriverUnload = tinyOpCPU_Unload;
	KdPrint(("TinyOpCPU driver entry\n"));
	return STATUS_SUCCESS;
}

#pragma  code_seg("PAGE")
__drv_dispatchType(IRP_MJ_CREATE) NTSTATUS tinyOpCPU_Create(IN PDEVICE_OBJECT pdeviceObject, IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pdeviceObject);
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(pIrp,IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

#pragma  code_seg("PAGE")
__drv_dispatchType(IRP_MJ_CLOSE) NTSTATUS tinyOpCPU_Close(IN PDEVICE_OBJECT pdeviceObject, IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pdeviceObject);
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

#pragma  code_seg("PAGE")
void tinyOpCPU_Unload(IN PDRIVER_OBJECT pdiverObject)
{
	UNICODE_STRING dosName;
	RtlInitUnicodeString(&dosName, DOS_DEVICE_NAME);
	IoDeleteSymbolicLink(&dosName);
	if (pdiverObject != NULL)
	{
		IoDeleteDevice(pdiverObject->DeviceObject);
	}
	
}

#pragma  code_seg("PAGE")
__drv_dispatchType(IRP_MJ_DEVICE_CONTROL) NTSTATUS tinyOpCPU_DeviceControl(IN PDEVICE_OBJECT pdeviceObject, IN PIRP pIrp)
{
	NTSTATUS ntstatus = STATUS_UNSUCCESSFUL;
	PIO_STACK_LOCATION irpStack;
	ULONG inBufferLength;
	ULONG outBufferLength;
	PVOID ioBuffer;
	ULONG uIoControlCode;
	PDEVICE_EXTENSION pfdo;

	DbgPrint("tiny IoToolDeviceControl \n");

	ioBuffer = pIrp->AssociatedIrp.SystemBuffer;  //用户空间内核空间共享缓冲区（作为输入输出）
	irpStack = IoGetCurrentIrpStackLocation(pIrp);
	inBufferLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;
	outBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
	uIoControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;
	pfdo = (PDEVICE_EXTENSION)pdeviceObject->DeviceExtension;
	switch (uIoControlCode)
	{
		case IOCTL_READ_IO_PORT:
		{
			oper_io_read(pfdo,&inBufferLength,ioBuffer,&outBufferLength,ioBuffer);
			ntstatus = STATUS_SUCCESS;
			break;
		}
		case IOCTL_WRITE_IO_PORT:
		{
			oper_io_write(pfdo, &inBufferLength, ioBuffer, &outBufferLength, ioBuffer);
			ntstatus = STATUS_SUCCESS;
			break;
		}

	}

	pIrp->IoStatus.Status = ntstatus;
	pIrp->IoStatus.Information = outBufferLength;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return ntstatus;
}
