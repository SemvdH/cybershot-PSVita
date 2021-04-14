#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <stdio.h>
#include <vita2d.h>

#include "debugScreen.h"


#define printf psvDebugScreenPrintf

int main(int argc, char *argv[]) {
	vita2d_init();
	psvDebugScreenInit();
	printf("Hello, world!\n");
	
	sceKernelDelayThread(3*1000000); // Wait for 3 seconds
	sceKernelExitProcess(0);
	return 0;
}
