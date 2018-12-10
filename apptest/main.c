#include <stdlib.h>
#include <stdio.h>
#include "tinyopcpu_app_oper.h"


int main()
{
	unsigned short iodata;
	TinyOpCPU_Init();
	
	TinyOpCPU_WriteIOPort16(0x80,0x55aa);
	TinyOpCPU_ReadIOPort16(0x80, &iodata);


	TinyOpCPU_Delete();
	system("pause");
	return 0;
}