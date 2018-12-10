#ifndef __DRIVER_H__
#define __DRIVER_H__
#include <wdm.h>

#define PAGECODE code_seg("PAGE")  //能分页内存，可以交换到硬盘
#define NOPAGECODE code_seg()      //不能分页的内存， 常驻物理内存
#define INITCODE code_seg("INIT")  //初始化时候载入内存，然后可以从内存中卸掉
#define PAGEDATA data_seg("PAGE")
#define NOPAGEDATA data_seg()
#define INITDATA data_seg("INIT")

#define NT_DEVICE_NAME L"\\Device\\TinyOpCPU"			/*设备在内核中的挂载点*/
#define DOS_DEVICE_NAME L"\\DosDevices\\TinyOpCPU"		/*设备在内核中的名字或者\\?\\TinyOpCPU 在用户模式下\\.\\TinyOpCPU*/

typedef struct _DEVICE_EXTENSION
{
	PDEVICE_OBJECT self;
	PDEVICE_OBJECT nextStackDevice;
	UNICODE_STRING ustrDeviceName;	// 设备名
	UNICODE_STRING ustrSymLinkName;	// 符号链接名
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


NTSTATUS tinyOpCPU_Create(IN PDEVICE_OBJECT pdeviceObject, IN PIRP pIrp);
NTSTATUS tinyOpCPU_Close(IN PDEVICE_OBJECT pdeviceObject, IN PIRP pIrp);
NTSTATUS tinyOpCPU_DeviceControl(IN PDEVICE_OBJECT pdeviceObject, IN PIRP pIrp);
void tinyOpCPU_Unload(IN PDRIVER_OBJECT pdiverObject);
#endif