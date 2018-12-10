#include "install.h"

BOOL IsWow64()
{
#ifdef AMD64
	return TRUE;
#else
	BOOL bIsWow64 = FALSE;
	typedef BOOLEAN(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE hProcess, PBOOL wow64Process);
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle("kernel32"), "IsWow64Process");
	if (fnIsWow64Process != NULL)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			return FALSE;
		}
	}
	return bIsWow64;
#endif
}

BOOL GetDriverFullPath(TCHAR* vdriverName , TCHAR* vDriveFullPath)
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

BOOL DriverFileIsExist(TCHAR* vDriveFullPath)
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
BOOL InstallDriver(_In_ LPCTSTR DriveServiceName, _In_ LPCTSTR ServiceExe)
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
BOOL UnstallDriver(_In_ LPCTSTR DriveServiceName)
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