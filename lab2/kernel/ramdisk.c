#include "mini_uart.h"
#include "string_utils.h"

#define MAX_NUM_FILE  10
#define CPIO_HEADER_SIZE 110
#define RAMDISK_ADDRESS (char*)(0x20000000)

struct cpio_newc_header {
        char c_magic[6];
        char c_ino[8];
        char c_mode[8];        // File type
        char c_uid[8];
        char c_gid[8];
        char c_nlink[8];
        char c_mtime[8];
        char c_filesize[8];    // File size, NUL not included
        char c_devmajor[8];
        char c_devminor[8];
        char c_rdevmajor[8];
        char c_rdevminor[8];
        char c_namesize[8];    // Path size, NUL included
        char c_check[8];
};

struct file_list_struct {
        char type;
        struct cpio_newc_header *header_ptr;
        char *name_ptr;
        char *content_ptr;
};
struct file_list_struct file_list[MAX_NUM_FILE];
int parsed = 0;
int file_count = 0;

void parse(void)
{
        struct cpio_newc_header *header_ptr = 
                        (struct cpio_newc_header*)RAMDISK_ADDRESS;
        char *ptr = (char*)header_ptr + CPIO_HEADER_SIZE;
        /*
         * c_mode is 00000000 if it is the special record 
         * with pathname "TRAILER!!!" at the end of the archive
         */
        while ((header_ptr->c_mode)[7] != '0') {
                file_list[file_count].header_ptr = header_ptr;

                /*
                 * c_mode 000081B4=file 000041FD=directory
                 */
                if ((header_ptr->c_mode)[4] == '8') {
                        file_list[file_count].type = 'f';
                } else {
                        file_list[file_count].type = 'd';
                }

                file_list[file_count].name_ptr = ptr;
                int name_size = string_hex_to_int(header_ptr->c_namesize, 8);
                ptr += name_size;

                file_list[file_count].content_ptr = ptr;
                int file_size = string_hex_to_int(header_ptr->c_filesize, 8);
                ptr += file_size;
                /*
                 * Skips the NUL characters after file content
                 */
                while (*ptr == '\0') { ptr++; }

                header_ptr = (struct cpio_newc_header *)ptr;
                ptr += CPIO_HEADER_SIZE;
                file_count++;
        }
        parsed = 1;
}

void ramdisk_ls(void)
{
        if (!parsed) parse();

        for (int i = 0; i < file_count; i++) {
                uart_send_string(file_list[i].name_ptr);
                uart_endl();
        }
}

void ramdisk_cat(void)
{
        if (!parsed) parse();

        char filename[100];
        uart_send_string("Filename: ");
        uart_readline(filename, 100);

        int found = 0;
        for (int i = 0; i < file_count; i++) {
                if (!strcmp(filename, file_list[i].name_ptr)) {
                        if (file_list[i].type == 'f') {
                                uart_send_string(file_list[i].content_ptr);
                                uart_endl();
                        } else {
                                uart_send_string(filename);
                                uart_send_string(": Is a directory\r\n");
                        }
                        found = 1;
                        break;
                }
        }
        if (!found) {
                uart_send_string(filename);
                uart_send_string(": No such file or directory\r\n");
        }
}