#ifndef _PER_GPIO_H
#define _PER_GPIO_H

#define PHY_BASE_ADDR 0x3F000000

#define GPFSEL1     (PHY_BASE_ADDR + 0x00200004)
#define GPSET0      (PHY_BASE_ADDR + 0x0020001C)
#define GPCLR0      (PHY_BASE_ADDR + 0x00200028)
#define GPPUD       (PHY_BASE_ADDR + 0x00200094)
#define GPPUDCLK0   (PHY_BASE_ADDR + 0x00200098)

#endif