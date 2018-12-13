#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	#define FILE_ANY_ACCESS                 0
	#define FILE_SPECIAL_ACCESS    (FILE_ANY_ACCESS)
	#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
	#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe

	#define METHOD_BUFFERED                 0
	#define METHOD_IN_DIRECT                1
	#define METHOD_OUT_DIRECT               2
	#define METHOD_NEITHER                  3

	#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
		((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
	)


	#define TINYOPCPU_TYPE 50000
	#define IOCTL_READ_IO_PORT  CTL_CODE(TINYOPCPU_TYPE, 0x850, METHOD_BUFFERED, FILE_READ_ACCESS)
	#define IOCTL_WRITE_IO_PORT  CTL_CODE(TINYOPCPU_TYPE, 0x851, METHOD_BUFFERED, FILE_READ_ACCESS)
	#define IOCTL_READ_MEMORY  CTL_CODE(TINYOPCPU_TYPE, 0x852, METHOD_BUFFERED, FILE_READ_ACCESS)
	#define IOCTL_READ_MSR  CTL_CODE(TINYOPCPU_TYPE, 0x853, METHOD_BUFFERED, FILE_READ_ACCESS)
	#define IOCTL_WRITE_MSR  CTL_CODE(TINYOPCPU_TYPE, 0x854, METHOD_BUFFERED, FILE_READ_ACCESS)

#ifdef __cplusplus
}
#endif