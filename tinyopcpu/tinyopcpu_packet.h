#pragma once

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef BOOLEAN
	typedef unsigned char BOOLEAN;
#endif
#ifndef UCHAR
	typedef unsigned char UCHAR;
#endif
#ifndef CHAR
	typedef signed char CHAR;
#endif
#ifndef USHORT
	typedef unsigned short USHORT;
#endif
#ifndef SHORT
	typedef signed short SHORT;
#endif
#ifndef ULONG
	typedef unsigned long ULONG;
#endif
#ifndef LONG
	typedef signed long LONG;
#endif
/*
#ifndef ULONGLONG
	typedef unsigned long ULONGLONG;
#endif
#ifndef LONGLONG
	typedef signed long LONGLONG;
#endif
*/
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

#ifdef  __cplusplus
}
#endif