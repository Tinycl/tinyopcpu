#include "tinyopcpu_app_oper.h"

#define USER_DEVICE_NAME L"\\\\.\\TinyOpCPU"

TCHAR*  TinyOpCPU_ServiceName = L"tinyopcpu";
TCHAR*  TinyOpCPU_DriverName = L"tinyopcpu.sys";
TCHAR	TinyOpCPU_DriveFullPathName[MAX_PATH] = { 0 };
HANDLE	TinyOpCPU_Handle = NULL;

BOOL TinyOpCPU_GetDriverFullPath(TCHAR* vdriverName, TCHAR* vDriveFullPath)
{
	TCHAR vdrivePathTemp[MAX_PATH];
	int vindex = 0;
	/*.sys full path is exe's directory  */
	GetModuleFileName(NULL, vdrivePathTemp, MAX_PATH);	//exe path
	/**************exe dir + driver.sys************/
	for (vindex = MAX_PATH - 1; vindex > 0; vindex--)
	{
		if (vdrivePathTemp[vindex] == L'\\')
		{
			break;
		}
	}
	for (int i = 0; i <= vindex; i++)
	{
		vDriveFullPath[i] = vdrivePathTemp[i];
	}

	for (int i = 0; vdriverName[i] != L'\0'; i++)
	{
		vDriveFullPath[vindex + 1 + i] = vdriverName[i];
	}
	return TRUE;
}

BOOL TinyOpCPU_DriverFileIsExist(TCHAR* vDriveFullPath)
{
	HANDLE fileHandle;
	if ((fileHandle = CreateFile(vDriveFullPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
	{
		printf("driver %s is not load. \n", vDriveFullPath);
		return FALSE;
	}
	if (fileHandle)
	{
		CloseHandle(fileHandle);
	}
	return TRUE;
}

//安装驱动，就是用驱动的二进制文件创建一个xxx服务，并启动这个xxx服务
BOOL TinyOpCPU_InstallDriver(_In_ LPCTSTR DriveServiceName, _In_ LPCTSTR ServiceExe)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	DWORD err;

	if (!DriveServiceName || !ServiceExe)
	{
		printf("Invalid Driver or Service provided to ManageDriver() \n");
		return FALSE;
	}
	//打开服务管理器
	schSCManager = OpenSCManager(NULL,		//local machine
		NULL,								//local database
		SC_MANAGER_ALL_ACCESS);				//access required

	if (!schSCManager)
	{
		printf("Open SC Manager failed! error is %d \n", GetLastError());
		return FALSE;
	}

	//创建服务 DriveServiceName
	schService = CreateService(schSCManager,	//handle of service control manager database
		DriveServiceName,								//address of name of service to start
		DriveServiceName,								//address of display name
		SERVICE_ALL_ACCESS,						//type of access to service
		SERVICE_KERNEL_DRIVER,					//type of service
		SERVICE_DEMAND_START,					//when to start service
		SERVICE_ERROR_NORMAL,					//severity if service fails to start
		ServiceExe,								//address of name  of binary file  .sys full path
		NULL,									//service does not belong to a group
		NULL,									//no tag requested
		NULL,									//no dependency names
		NULL,									//use LocalSystem account
		NULL									//no password for service account
	);
	if (schService == NULL)
	{
		err = GetLastError();
		if (err = ERROR_SERVICE_EXISTS)
		{
			return TRUE;
		}
		else
		{
			printf("createservice failed! error is %d \n", GetLastError());
			return FALSE;
		}
	}

	//打开服务 DriveServiceName
	schService = OpenService(schSCManager,
		DriveServiceName,
		SERVICE_ALL_ACCESS
	);
	if (schService == NULL)
	{
		printf("openservice is failed! error is %d \n", GetLastError());
		return FALSE;
	}
	//启动服务 DriveServiceName
	if (!StartService(schService,	//service identifier
		0,							//number of arguments
		NULL						//pointer to arguments
	))
	{
		err = GetLastError();
		if (err == ERROR_SERVICE_ALREADY_RUNNING)
		{
			return TRUE;
		}
		else
		{
			printf("StartService is failed! error is %d \n", GetLastError());
			return FALSE;
		}
	}
	if (schService)
	{
		CloseServiceHandle(schService);
	}
	return TRUE;
}

//卸载驱动就是停止和删除xxx服务
BOOL TinyOpCPU_UnstallDriver(_In_ LPCTSTR DriveServiceName)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	BOOL err = TRUE;
	SERVICE_STATUS serviceStatus;
	if (!DriveServiceName)
	{
		printf("Invalid Driver provided to ManageDriver() \n");
		return FALSE;
	}
	//打开服务管理器
	schSCManager = OpenSCManager(NULL,		//local machine
		NULL,								//local database
		SC_MANAGER_ALL_ACCESS);				//access required
	if (!schSCManager)
	{
		printf("Open SC Manager failed! error is %d \n", GetLastError());
		return FALSE;
	}
	//打开服务 DriveServiceName
	schService = OpenService(schSCManager, DriveServiceName, SERVICE_ALL_ACCESS);
	if (schService == NULL)
	{
		printf("openservice is failed! error is %d \n", GetLastError());
		return FALSE;
	}
	//停止服务 DriveServiceName
	if (ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus))
	{
		err = TRUE;
	}
	else
	{
		printf("ControlService is failed! error is %d \n", GetLastError());
		err = FALSE;
	}
	//打开服务 DriveServiceName
	schService = OpenService(schSCManager, DriveServiceName, SERVICE_ALL_ACCESS);
	if (schService == NULL)
	{
		printf("openservice is failed! error is %d \n", GetLastError());
		return FALSE;
	}
	//删除服务 DriveServiceName
	if (DeleteService(schService))
	{
		err = TRUE;
	}
	else
	{
		printf("DeleteService is failed! error is %d \n", GetLastError());
		err = FALSE;
	}
	if (schService)
	{
		CloseServiceHandle(schService);
	}
	return err;
}

BOOL TinyOpCPU_Init()
{
	GetDriverFullPath(TinyOpCPU_DriverName, TinyOpCPU_DriveFullPathName);
	TinyOpCPU_Handle = CreateFile(USER_DEVICE_NAME,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		TRUNCATE_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (TinyOpCPU_Handle == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			printf("CreateFile is failed! error is %d \n", GetLastError());
			return FALSE;
		}
		if (!DriverFileIsExist(TinyOpCPU_DriveFullPathName))
		{
			return FALSE;
		}
		if (!InstallDriver(TinyOpCPU_ServiceName, TinyOpCPU_DriveFullPathName))
		{
			if (!UnstallDriver(TinyOpCPU_ServiceName))
			{
				return FALSE;
			}
			return FALSE;
		}

		TinyOpCPU_Handle = CreateFile(USER_DEVICE_NAME,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (TinyOpCPU_Handle == INVALID_HANDLE_VALUE)
		{
			printf("CreateFile is failed! error is %d", GetLastError());
			if (!UnstallDriver(TinyOpCPU_ServiceName))
			{
				return FALSE;
			}
			return FALSE;
		}
	}
	return TRUE;
}

BOOL TinyOpCPU_Delete()
{
	if (TinyOpCPU_Handle != NULL)
	{
		CloseHandle(TinyOpCPU_Handle);
	}
	if (!UnstallDriver(TinyOpCPU_ServiceName))
	{
		return FALSE;
	}
	return TRUE;
}
ULONG TinyOpCPU_ReadIOPort(ULONG cpu, ULONG bitsize, USHORT port, DATAVALUE *data)
{
	BOOL error;
	DWORD ByteReturned;
	PACKET_IO_READ req;
	PACKET_IO_READ_REPLY reply;
	req.bitsize = bitsize;
	req.cpu = cpu;
	req.port = port;
	error = DeviceIoControl(TinyOpCPU_Handle,
							IOCTL_READ_IO_PORT,
							&req,
							sizeof(req),
							&reply,
							sizeof(reply),
							&ByteReturned,
							NULL);
	switch (bitsize) {
	case 8:
		data->int8 = reply.data.int8;
		break;
	case 16:
		data->int16 = reply.data.int16;
		break;
	case 32:
		data->int32 = reply.data.int32;
		break;
	}
	return error;
}

ULONG TinyOpCPU_ReadIOPort8(USHORT port, UCHAR *data)
{
	return TinyOpCPU_ReadIOPort(0, 8, port, (DATAVALUE*)data);
}
ULONG TinyOpCPU_ReadIOPort16(USHORT port, USHORT *data)
{
	return TinyOpCPU_ReadIOPort(0, 16, port, (DATAVALUE*)data);
}
ULONG TinyOpCPU_ReadIOPort32(USHORT port, ULONG *data)
{
	return TinyOpCPU_ReadIOPort(0, 32, port, (DATAVALUE*)data);
}

ULONG TinyOpCPU_WriteIOPort(ULONG cpu, ULONG bitsize, USHORT port, ULONG data)
{
	BOOL error;
	DWORD ByteReturned;
	PACKET_IO_WRITE req;
	PACKET_IO_READ_REPLY reply;
	req.bitsize = bitsize;
	req.cpu = cpu;
	req.port = port;
	req.value.int32 = data;
	error = DeviceIoControl(TinyOpCPU_Handle,
		IOCTL_WRITE_IO_PORT,
		&req,
		sizeof(req),
		&reply,
		sizeof(reply),
		&ByteReturned,
		NULL);
	return error;
}

ULONG TinyOpCPU_WriteIOPort8(USHORT port, ULONG data)
{
	return TinyOpCPU_WriteIOPort(0, 8, port, data);
}
ULONG TinyOpCPU_WriteIOPort16(USHORT port, ULONG data)
{
	return TinyOpCPU_WriteIOPort(0, 16, port, data);
}
ULONG TinyOpCPU_WriteIOPort32(USHORT port, ULONG data)
{
	return TinyOpCPU_WriteIOPort(0, 32, port, data);
}

ULONG TinyOpCPU_ReadMemory(ULONG AddressSpace, ULONGLONG Address, ULONG count, UCHAR *buf)
{
	BOOL error;
	DWORD ByteReturned;
	PACKET_MEM_READ req;
	req.Address = Address;
	req.AddressSpace = AddressSpace;
	req.count = count;
	error = DeviceIoControl(TinyOpCPU_Handle,
							IOCTL_READ_MEMORY,
							&req,
							sizeof(PACKET_MEM_READ),
							buf,
							count,
							&ByteReturned,
							NULL);
	if (error == 0) {
		error = GetLastError();
	}
	/*
	unsigned char aa[256] = {0};
	for (int k = 0; k < count; ++k) {
		aa[k] = buf[k];
	}
	*/
	return error;
}

ULONG TinyOpCPU_ReadPhysMemory(ULONGLONG physaddr, ULONG count, UCHAR *buf)
{
	return(TinyOpCPU_ReadMemory(0, physaddr, count, buf));
}
ULONG TinyOpCPU_ReadVirtual(ULONGLONG virtaddr, ULONG count, UCHAR *buf)
{
	return(TinyOpCPU_ReadMemory(1, virtaddr, count, buf));
}