#ifndef __INSTALL_H__
#define __INSTALL_H__

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strsafe.h>


BOOL IsWow64();
BOOL GetDriverFullPath(TCHAR* vdriverName, TCHAR* vDriveFullPath);
BOOL DriverFileIsExist(TCHAR* vDriveFullPath);
BOOL InstallDriver(_In_ LPCTSTR DriveServiceName, _In_ LPCTSTR ServiceExe);
BOOL UnstallDriver(_In_ LPCTSTR DriveServiceName);
#endif