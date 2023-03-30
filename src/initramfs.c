#include "initramfs.h"
#include "uart.h"
#include "utils.h"
char *cpio_addr;
struct initramfs _initramfs;
void init_initramfs(struct initramfs *fs) 
{
    fs->addr = cpio_addr;
    fs->ls = _cpio_ls;
    fs->cat = _cpio_cat;
}

void _cpio_ls(struct initramfs *self, char *path) 
{
    char *ptr = self->addr;

    if (strncmp(ptr, "070701", 6) != 0) {
        uart_write_string("Not a valid New ASCII Format Cpio archive file\n");
        return;
    }
    // ptr += 6;
    unsigned file_size, path_size, header_path_size;
    char *file_path;
    struct cpio_newc_header *header;
    while (1) {
        header = ptr;
        
        file_size = hex2unsigned(&(header->c_filesize));
        file_path = ptr + sizeof(struct cpio_newc_header);

        if (strncmp(file_path, "TRAILER!!!", 10) == 0) {
            //end of archive
            break;
        }

        path_size = hex2unsigned(&(header->c_namesize));
        header_path_size = sizeof(struct cpio_newc_header) + path_size;
        if (strstartswith(file_path, path)) {
            uart_write_string(file_path);
            uart_write_string("\n");
        }
        // skip the file content
        /*
         This expression adds 3 to the size of an integer to round 
         it up to the nearest multiple of 4 bytes,
         then applies the mask ~3 to set the last two bits to 0.
        */
        ptr += ((file_size + 3) & ~3) + ((header_path_size + 3) & ~3);
    }
}

struct cpio_newc_header *_cpio_find_file(struct initramfs *self, char *path)
{
    char *ptr = self->addr;

    if (strncmp(ptr, "070701", 6) != 0) {
        uart_write_string("Not a valid New ASCII Format Cpio archive file\n");
        return;
    }
    unsigned file_size, path_size, header_path_size;
    char *file_path;
    struct cpio_newc_header *header;
    while (1) {
        header = ptr;
        
        file_path = ptr + sizeof(struct cpio_newc_header);
        if (strcmp(file_path, path) == 0) {
            return header;
        }

        if (strncmp(file_path, "TRAILER!!!", 10) == 0) {
            //end of archive
            break;
        }

        file_size = hex2unsigned(&(header->c_filesize));
        path_size = hex2unsigned(&(header->c_namesize));
        header_path_size = sizeof(struct cpio_newc_header) + path_size;
        // skip the file content
        /*
         This expression adds 3 to the size of an integer to round 
         it up to the nearest multiple of 4 bytes,
         then applies the mask ~3 to set the last two bits to 0.
        */
        ptr += ((file_size + 3) & ~3) + ((header_path_size + 3) & ~3);
    }
    return NULL;
}

void _cpio_cat(struct initramfs *self, char *path) 
{
    char *ptr = self->addr;

    if (strncmp(ptr, "070701", 6) != 0) {
        uart_write_string("Not a valid New ASCII Format Cpio archive file\n");
        return;
    }

    struct cpio_newc_header *header = _cpio_find_file(self, path);
    if (header == NULL) {
        uart_write_string("File Not Found!\n");
        return;
    }
    unsigned file_size, path_size, header_path_size;
    file_size = hex2unsigned(&(header->c_filesize));
    path_size = hex2unsigned(&(header->c_namesize));
    header_path_size = sizeof(struct cpio_newc_header) + path_size;
    header_path_size = ALIGN(header_path_size, 4);//((header_path_size + 3) & ~3);
    char *content_ptr = (char *)header + header_path_size;
    for (int i = 0; i < file_size; i++, content_ptr++) {
        uart_write(*content_ptr);
    }
    // uart_write_string("\n");
}