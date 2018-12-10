#include "tinyopcpu_oper.h"

NTSTATUS oper_io_read(PDEVICE_EXTENSION pfdo, ULONG *incount, PACKET_IO_READ *in, ULONG *outcount, PACKET_IO_READ_REPLY *out)
{
	USHORT port = in->port;
	ULONG bitsize = in->bitsize;
	UNREFERENCED_PARAMETER(incount);
	UNREFERENCED_PARAMETER(pfdo);
	if (in->cpu >= (unsigned)KeNumberProcessors) {
		return STATUS_INVALID_PARAMETER;
	}
	KeSetSystemAffinityThread((KAFFINITY)1<<in->cpu);
	out->data.int64 = 0xDEADDEADDEADDEAD;
	switch (bitsize) {
	case 8:
		out->data.int8 = READ_PORT_UCHAR((PUCHAR)port);
		break;
	case 16:
		out->data.int16 = READ_PORT_USHORT((PUSHORT)port);
		break;
	case 32:
		out->data.int32 = READ_PORT_ULONG((PULONG)port);
		break;
	}
	KeRevertToUserAffinityThread();
	*outcount = sizeof(PACKET_IO_READ_REPLY);
	return STATUS_SUCCESS;
}


NTSTATUS oper_io_write(PDEVICE_EXTENSION pfdo, ULONG *incount, PACKET_IO_WRITE *in, ULONG *outcount, void *out)
{
	ULONG bitsize = in->bitsize;
	UNREFERENCED_PARAMETER(pfdo);
	UNREFERENCED_PARAMETER(incount);
	UNREFERENCED_PARAMETER(out);
	switch (bitsize) {
	case 8:
		WRITE_PORT_UCHAR((PUCHAR)in->port, in->value.int8);
		break;
	case 16:
		WRITE_PORT_USHORT((PUSHORT)in->port, in->value.int16);
		break;
	case 32:
		WRITE_PORT_ULONG((PULONG)in->port, in->value.int32);
		break;
	}
	*outcount = 0;			
	return(STATUS_SUCCESS);
}