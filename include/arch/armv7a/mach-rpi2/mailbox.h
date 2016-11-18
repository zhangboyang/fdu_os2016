#ifndef _MACH_MAILBOX_H
#define _MACH_MAILBOX_H

#include <mach-platform.h>

#define MAIL_BASE 0xB880

// This bit is set in the status register if there is no space to write into the mailbox
#define MAIL_FULL 0x80000000
// This bit is set in the status register if there is nothing to read from the mailbox
#define MAIL_EMPTY 0x40000000

#define MAIL0_PEEK 0x10
#define MAIL0_READ 0x00
#define MAIL0_STATUS 0x18
#define MAIL0_SENDER 0x14
#define MAIL0_CONFIG 0x1C

#define MAIL1_PEEK 0x30
#define MAIL1_READ 0x20
#define MAIL1_STATUS 0x38
#define MAIL1_SENDER 0x34
#define MAIL1_CONFIG 0x3C

#define MAIL0_WRITE MAIL1_READ

#endif
