#include "uart.h"
#include "utils.h"
#include "dtb.h"
extern char* dtb_ptr;


void main(char* arg) { 
	dtb_ptr = arg; //get dtb from bootloader
	traverse_device_tree(dtb_ptr, dtb_callback_initramfs);
	shell_init();
	char cmd[100];
	int status=0;
	while (1) {
		switch (status) {
			case 0:
				shell_input(cmd);
				status = 1;
				break;
			case 1:
				shell_command(cmd);
				status = 0;
				break;
		}
	}
}


