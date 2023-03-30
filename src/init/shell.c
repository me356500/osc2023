#include "bcm2835/uart.h"
#include "init/fdt.h"
#include "init/ramfs.h"
#include "init/shell.h"
#include "string.h"

#define SHELL_ST_NORMAL 0x0U
#define SHELL_ST_CAT    0x1U

#define PM_MGIC 0x5A000000U
#define PM_RSTC PRPHRL(0x10001C)
#define PM_WDOG PRPHRL(0x100024)

#define SBUF_SIZE 0x100

#define sbuf_empty(x) ((x)->i == (x)->o)

struct sbuf {
    unsigned int i;
    unsigned int o;
    char b[SBUF_SIZE];
};

static inline void sbuf_push(struct sbuf * buf, char c) {
    buf->b[buf->i] = c;
    buf->i = (buf->i + 1) % SBUF_SIZE;
    if (sbuf_empty(buf)) {
        buf->o = (buf->o + 1) % SBUF_SIZE;
    }
}

static inline char sbuf_pop(struct sbuf * buf) {
    if (sbuf_empty(buf)) {
        return '\0';
    }
    char c = buf->b[buf->o];
    buf->o = (buf->o + 1) % SBUF_SIZE;
    return c;
}

static inline void sbuf_getline(struct sbuf * buf, char * line) {
    unsigned int i = 0;
    while (!sbuf_empty(buf)) {
        char c = sbuf_pop(buf);
        if (c == '\n') {
            break;
        }
        line[i++] = c;
    }
    line[i] = '\0';
}

static void print_dt_callback(struct fdt_node * node) {
    for (unsigned int i = 0; i < node->level; ++i) {
        uart_puts("  ");
    }
    uart_puts("- ");
    uart_puts(node->name);
    uart_send('\n');
    struct fdt_prop * prop = node->prpty;
    if (prop) {
        for (unsigned int i = 0; prop && i < node->level + 1; ++i) {
            uart_puts("  ");
        }
        uart_puts(prop->name);
        for (prop = node->prpty; prop; prop = prop->next) {
            uart_puts(", ");
            uart_puts(prop->name);
        }
        uart_send('\n');
    }
}

static unsigned int print_dt_cmp(struct fdt_node * node) {
    return 1;
}

static inline unsigned int shell_exec(char * cmd) {
    if (!strcmp(cmd, "")) {
        return SHELL_ST_NORMAL;
    } else if (!strcmp(cmd, "help")) {
        uart_puts("cat:     print a ramdisk file's content\n");
        uart_puts("dt:      print device tree\n");
        uart_puts("help:    print this help menu\n");
        uart_puts("hello:   print Hello World!\n");
        uart_puts("ls:      list files in the ramdisk\n");
        uart_puts("reboot:  reboot the device\n");
    } else if (!strcmp(cmd, "cat")) {
        uart_puts("filename: ");
        return SHELL_ST_CAT;
    } else if (!strcmp(cmd, "dt")) {
        fdt_traverse(print_dt_callback, print_dt_cmp);
    } else if (!strcmp(cmd, "hello")) {
        uart_puts("Hello World!\n");
    } else if (!strcmp(cmd, "ls")) {
        ramfs_ls();
    } else if (!strcmp(cmd, "reboot")) {
        uart_puts("rebooting...\n");
        *PM_RSTC = PM_MGIC | 0x20;
        *PM_WDOG = PM_MGIC | 100;
        do {/* wait for rebooting */} while (1);
    } else {
        uart_puts(cmd);
        uart_puts(": command not found\n");
    }
    return SHELL_ST_NORMAL;
}

static inline unsigned int shell_exec_state(char * line, unsigned int state) {
    switch (state) {
    case SHELL_ST_CAT:
        ramfs_cat(line);
        return SHELL_ST_NORMAL;
    }
    return SHELL_ST_NORMAL;
}

void shell_start() {
    struct sbuf buf;
    buf.i = buf.o = 0;

    unsigned int state = SHELL_ST_NORMAL;

    uart_puts("# ");
    do {
        char c = uart_recv();
        uart_send(c);
        sbuf_push(&buf, c);
        if (c == '\n') {
            char line[SBUF_SIZE];
            sbuf_getline(&buf, line);
            if (state == SHELL_ST_NORMAL) {
                state = shell_exec(line);
            } else {
                state = shell_exec_state(line, state);
            }
            if (state == SHELL_ST_NORMAL) {
                uart_puts("# ");
            }
        }
    } while (1);
}
