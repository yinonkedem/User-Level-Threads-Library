//
// Created by yinon on 06/06/2024.
//
#include "uthreads.h"

#ifndef _TOOLS_H_
#define _TOOLS_H_

enum States{
    READY,
    RUNNING,
    BLOCKED,
    SLEEP,
    SLEEP_WITH_BLOCKED,
    TERMINATE
};
#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr);

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5


/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr);

#endif
#endif //_TOOLS_H_
