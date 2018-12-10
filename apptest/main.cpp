#include <iostream>
#include "tinyopcpu_app_oper.h"

using namespace std;

int main()
{
	unsigned char iodata;
	cout << "TinyOpCPU: init" << endl;
	cout << endl;
	TinyOpCPU_Init();
	cout << "TinyOpCPU£ºwrite IO 0x80 data 0x55" << endl;
	cout << endl;
	TinyOpCPU_WriteIOPort8(0x80,0x55);
	cout << "TinyOpCPU£ºreade IO 0x80 data ";
	cout << endl;
	TinyOpCPU_ReadIOPort8(0x80, &iodata);
	cout << iodata<<endl;
	cout <<  "TinyOpCPU£ºreade Physical Memory start address: 0x1000 length : 256 bytes " << endl;
	unsigned char *pysicalbuff = new unsigned char[256];
	TinyOpCPU_ReadPhysMemory(0x1000,256, pysicalbuff);
	cout << hex;
	for (int k = 0; k < 16; ++k) {
		cout << 0x1000 + k * 16<<": ";
		for (int kk = 0; kk < 16; ++kk) {
			cout << hex;
			cout.unsetf(ios::dec);
			cout.setf(ios::hex);
			cout <<0x0+pysicalbuff[k*16+kk] << "  ";
		}
		cout << endl;
	}
	cout << endl;
	/*
	cout << "TinyOpCPU£ºreade Virtual Memory start address: 0xffff900000000000 length : 256 bytes " << endl;
	unsigned char *virtualbuff = new unsigned char[256];
	TinyOpCPU_ReadVirtual(0xffff900000000000, 256, virtualbuff);
	for (int k = 0; k < 16; ++k) {
		cout << 0xffff900000000000 + k * 16 << ": ";
		for (int kk = 0; kk < 16; ++kk) {
			cout << 0x0 + virtualbuff[k * 16 + kk] << "  ";
		}
		cout << endl;
	}
	*/
	cout << endl;
	TinyOpCPU_Delete();
	system("pause");
	return 0;
}