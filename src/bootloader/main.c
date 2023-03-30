#include <mini_uart.h>
#include <string.h>
#include <builtin.h>
#include <utils.h>

#define BUFSIZE 0x100
char shell_buf[BUFSIZE];
char *dtp_pass;
extern char _kernel[];

void _load(void){
    uart_send_string("[*] Loading kernel...\r\n");

    unsigned int len;
    char *p = _kernel;

    len = uart_recv_uint();
    uart_send_string("[*] Receiving kernel with len:");
    uart_send_hex(len);
    uart_send_string("\r\n");

    // uart_send_hex(p);
    while(len--) {
        *p++= uart_recv();
    }

    uart_send_string("[*] Jumping to the kernel\r\n");
    delay(5000);
    // Jump to the kernel and execute
    typedef void (*func_ptr)(char*);
    func_ptr ptr = (func_ptr)_kernel;
    // uart_send_hex(ptr);
    ptr(dtp_pass);
}

void shell_interact(void){
    uart_send_string("# ");
    unsigned int cnt = uart_recv_line(shell_buf, BUFSIZE);
    uart_send_string("\r\n");
    if (!strcmp("help", shell_buf))
        _help(1);
    else if (!strcmp("hello", shell_buf)) 
        _hello();
    else if (!strcmp("reboot", shell_buf))
        _reboot();
    else if (!strcmp("hwinfo", shell_buf))
        _hwinfo();
    else if (!strcmp("load", shell_buf))
        _load();
    else {
        _echo(shell_buf);
        if(cnt)
            uart_send_string("\r\n");
    }
}

void bootloader_main(char *dtp){
    uart_init();
    uart_send_string("[*] Running the bootloader...\r\n");
    dtp_pass = dtp;

    while(1)
        shell_interact();
}