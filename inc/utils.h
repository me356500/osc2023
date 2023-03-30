#ifndef	_UTILS_H
#define	_UTILS_H

#define enable_interrupt(){           \
    asm volatile("msr DAIFClr, 0xf"); \
}

#define disable_interrupt(){           \
    asm volatile("msr DAIFSet, 0xf"); \
}

#define get_elem_idx(elem, array) \
    (((char *)elem - (char *)array) / sizeof(array[0]))

extern void delay ( unsigned long);
extern void put32 ( unsigned long, unsigned int );
extern unsigned int get32 ( unsigned long );

#endif  /*_UTILS_H */