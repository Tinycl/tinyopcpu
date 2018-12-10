#ifndef __TINYOPCPU_OPER_H__
#define __TINYOPCPU_OPER_H__
#include <wdm.h>
#include "driver.h"
#include "tinyopcpu_packet.h"



NTSTATUS oper_io_read(PDEVICE_EXTENSION pfdo, ULONG *incount, PACKET_IO_READ *in, ULONG *outcount, PACKET_IO_READ_REPLY *out);
NTSTATUS oper_io_write(PDEVICE_EXTENSION pfdo, ULONG *incount, PACKET_IO_WRITE *in, ULONG *outcount, void *out);

#endif