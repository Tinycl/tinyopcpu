#pragma once

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef BOOLEAN
	#define  BOOLEAN unsigned char 
#endif
#ifndef UCHAR
	#define  UCHAR unsigned char 
#endif
#ifndef CHAR
	#define CHAR signed char 
#endif
#ifndef USHORT
	#define USHORT unsigned short 
#endif
#ifndef SHORT
	#define SHORT signed short 
#endif
#ifndef ULONG
	#define  ULONG unsigned long
#endif
#ifndef LONG
	#define  LONG signed long
#endif

#ifdef _WIN64

	#ifndef ULONGLONG
		#define ULONGLONG unsigned long long
	#endif

	#ifndef LONGLONG
		#define LONGLONG signed long long
	#endif
#else
	#ifndef ULONGLONG
	#define ULONGLONG unsigned long
	#endif

	#ifndef LONGLONG
	#define LONGLONG signed long
#endif

#endif



//typedef unsigned long ULONGLONG;
#ifndef _DATAVALUE_
#define	_DATAVALUE_
	typedef union {
		UCHAR			int8;
		USHORT			int16;
		ULONG			int32;
		ULONGLONG		int64;
	} DATAVALUE;
#endif

	/*
	* IOCTL_IO_READ
	*/
	typedef struct {
		ULONG		cpu;
		ULONG		bitsize;
		USHORT		port;
	} PACKET_IO_READ;
	typedef struct {
		DATAVALUE	data;
	} PACKET_IO_READ_REPLY;

	/*
	* IOCTL_IO_WRITE
	*/
	typedef struct {
		ULONG		cpu;
		ULONG		bitsize;
		USHORT		port;
		DATAVALUE	value;
	} PACKET_IO_WRITE;

	/*
	* IOCTL_MEM_READ
	*/
	typedef struct {
		ULONG		AddressSpace;			//0=physical, 1=Kernel, 2=Other context
		ULONGLONG	Address;
		ULONG		count;
	} PACKET_MEM_READ;
	typedef struct {
		UCHAR		value[1];
	} PACKET_MEM_READ_REPLY;
#ifdef  __cplusplus
}
#endif