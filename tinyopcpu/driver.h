#ifndef __DRIVER_H__
#define __DRIVER_H__
#include <wdm.h>

#define PAGECODE code_seg("PAGE")  //�ܷ�ҳ�ڴ棬���Խ�����Ӳ��
#define NOPAGECODE code_seg()      //���ܷ�ҳ���ڴ棬 ��פ�����ڴ�
#define INITCODE code_seg("INIT")  //��ʼ��ʱ�������ڴ棬Ȼ����Դ��ڴ���ж��
#define PAGEDATA data_seg("PAGE")
#define NOPAGEDATA data_seg()
#define INITDATA data_seg("INIT")

#define NT_DEVICE_NAME L"\\Device\\TinyOpCPU"			/*�豸���ں��еĹ��ص�*/
#define DOS_DEVICE_NAME L"\\DosDevices\\TinyOpCPU"		/*�豸���ں��е����ֻ���\\?\\TinyOpCPU ���û�ģʽ��\\.\\TinyOpCPU*/

typedef struct _DEVICE_EXTENSION
{
	PDEVICE_OBJECT self;
	PDEVICE_OBJECT nextStackDevice;
	UNICODE_STRING ustrDeviceName;	// �豸��
	UNICODE_STRING ustrSymLinkName;	// ����������
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


NTSTATUS tinyOpCPU_Create(IN PDEVICE_OBJECT pdeviceObject, IN PIRP pIrp);
NTSTATUS tinyOpCPU_Close(IN PDEVICE_OBJECT pdeviceObject, IN PIRP pIrp);
NTSTATUS tinyOpCPU_DeviceControl(IN PDEVICE_OBJECT pdeviceObject, IN PIRP pIrp);
void tinyOpCPU_Unload(IN PDRIVER_OBJECT pdiverObject);
#endif