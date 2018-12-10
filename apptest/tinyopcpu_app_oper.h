#pragma once
#include <windows.h>
#include <winioctl.h>
#include <winsvc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\tinyopcpu\tinyopcpu_packet.h"
#include "..\tinyopcpu\tinyopcpu_ioctl.h"


#ifdef __cplusplus
extern "C" {
#endif

	BOOL TinyOpCPU_Init();
	BOOL TinyOpCPU_Delete();
	ULONG TinyOpCPU_ReadIOPort8(USHORT port, UCHAR *data);
	ULONG TinyOpCPU_ReadIOPort16(USHORT port, USHORT *data);
	ULONG TinyOpCPU_ReadIOPort32(USHORT port, ULONG *data);
	ULONG TinyOpCPU_WriteIOPort8(USHORT port, ULONG data);
	ULONG TinyOpCPU_WriteIOPort16(USHORT port, ULONG data);
	ULONG TinyOpCPU_WriteIOPort32(USHORT port, ULONG data);
	ULONG TinyOpCPU_ReadMemory(ULONG AddressSpace, ULONGLONG Address, ULONG count, UCHAR *buf);
	ULONG TinyOpCPU_ReadPhysMemory(ULONGLONG physaddr, ULONG count, UCHAR *buf);
	ULONG TinyOpCPU_ReadVirtual(ULONGLONG virtaddr, ULONG count, UCHAR *buf);
#ifdef __cplusplus
}
#endif