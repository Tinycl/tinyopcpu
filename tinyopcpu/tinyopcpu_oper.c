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

NTSTATUS oper_read_memory(PDEVICE_EXTENSION pfdo, ULONG *incount,PACKET_MEM_READ *in, ULONG *outcount, UCHAR *out)
{
	//PUCHAR dst = (PUCHAR)&out->value[0]; PACKET_MEM_READ_REPLY
	UNREFERENCED_PARAMETER(pfdo);
	PUCHAR dst = out;
	ULONG datalen = in->count;
	switch (in->AddressSpace) {
		case 0: //physical memory
		{
			PUCHAR src;
			PHYSICAL_ADDRESS pa;
			pa.QuadPart = in->Address;
			src = MmMapIoSpace(pa, datalen, MmNonCached); //nocached
			if (src == NULL) {
				src = MmMapIoSpace(pa, datalen, MmCached); //cached
				if (src == NULL) {
					src = MmMapIoSpace(pa, datalen, MmWriteCombined);//wc
					if (src == NULL) {
						DbgPrint("TinyOpCPU: invalid parameter\n");
						return STATUS_INVALID_PARAMETER;
					}
				}
			}
			RtlCopyMemory(dst,src,datalen);
			MmUnmapIoSpace(src,datalen);
			*outcount = datalen;
			break;
		}
		case 1: //kernel memory
		{	
			#pragma code_seg() //nonpage
			//#pragma data_seg("PAGE") //nonpage
			PUCHAR src = (PUCHAR)in->Address;
			PMDL pmdl;
			RtlFillMemory(dst,datalen,0);
			pmdl = IoAllocateMdl(src,datalen,FALSE,FALSE,NULL);
			if (pmdl) {
				MmBuildMdlForNonPagedPool(pmdl);
				pmdl->MdlFlags = pmdl->MdlFlags | MDL_MAPPED_TO_SYSTEM_VA;
				RtlCopyMemory(dst, src, datalen);
				IoFreeMdl(pmdl);
				*outcount = datalen;
				break;
			}
			else {
				*outcount = 0;
				return STATUS_INVALID_PARAMETER;
			}
			

		}
		default:
			return STATUS_INVALID_PARAMETER;
	}
	return STATUS_SUCCESS;
}

NTSTATUS oper_read_msr(PDEVICE_EXTENSION pfdo, ULONG *incount, PACKET_CPU_MSR_READ *in, ULONG *outcount, PACKET_CPU_MSR_READ_REPLY *out)
{
	UNREFERENCED_PARAMETER(pfdo);
	if (in->cpu >= (unsigned)KeNumberProcessors) {
		return STATUS_INVALID_PARAMETER;
	}
	KeSetSystemAffinityThread((KAFFINITY)(1 << in->cpu));
	out->gp = 1;
	__try
	{
		out->data = __readmsr(in->msr);
		out->gp = 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		out->data = 0xDEADDEAD;
		out->gp = 1;
	}
	KeRevertToUserAffinityThread();
	*outcount = sizeof(PACKET_CPU_MSR_READ_REPLY);
	return STATUS_SUCCESS;

}

NTSTATUS oper_write_msr(PDEVICE_EXTENSION pfdo, ULONG *incount, PACKET_CPU_MSR_WRITE *in, ULONG *outcount, PACKET_CPU_MSR_WRITE_REPLY *out)
{
	UNREFERENCED_PARAMETER(pfdo);
	if (in->cpu >= (unsigned)KeNumberProcessors) {
		return STATUS_INVALID_PARAMETER;
	}
	KeSetSystemAffinityThread((KAFFINITY)(1 << in->cpu));
	out->gp = 1;
	__try
	{
		__writemsr(in->msr, in->data);
		out->gp = 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		out->data = 0xDEADDEAD;
		out->gp = 1;
	}
	__try
	{
		out->data = __readmsr(in->msr);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{

	}
	KeRevertToUserAffinityThread();
	*outcount = sizeof(PACKET_CPU_MSR_WRITE_REPLY);
	return STATUS_SUCCESS;
}