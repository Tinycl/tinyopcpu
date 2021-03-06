#ifndef __TINYOPCPU_OPER_H__
#define __TINYOPCPU_OPER_H__
#include <wdm.h>
#include <intrin.h>
#include "driver.h"
#include "tinyopcpu_packet.h"



NTSTATUS oper_io_read(PDEVICE_EXTENSION pfdo, ULONG *incount, PACKET_IO_READ *in, ULONG *outcount, PACKET_IO_READ_REPLY *out);
NTSTATUS oper_io_write(PDEVICE_EXTENSION pfdo, ULONG *incount, PACKET_IO_WRITE *in, ULONG *outcount, void *out);
NTSTATUS oper_read_memory(PDEVICE_EXTENSION pfdo, ULONG *incount, PACKET_MEM_READ *in, ULONG *outcount, UCHAR *out);
NTSTATUS oper_read_msr(PDEVICE_EXTENSION pfdo, ULONG *incount, PACKET_CPU_MSR_READ *in, ULONG *outcount, PACKET_CPU_MSR_READ_REPLY *out);
NTSTATUS oper_write_msr(PDEVICE_EXTENSION pfdo, ULONG *incount, PACKET_CPU_MSR_WRITE *in, ULONG *outcount, PACKET_CPU_MSR_WRITE_REPLY *out);

#endif