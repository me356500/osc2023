#ifndef _MBOX_H
#define _MBOX_H

/* a properly aligned buffer */
extern volatile unsigned int mbox[36];

#define MBOX_REQUEST    0

/* channels */
#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8

/* tags */
#define MBOX_TAG_GETSERIAL      0x10004
#define MBOX_TAG_LAST           0

struct arm_info{
    unsigned int base_addr, size;
}typedef arm_info;

int mbox_call(unsigned char ch);
void get_board_revision(unsigned int *revision);
void get_arm_memory(arm_info *arm_mem);

#endif