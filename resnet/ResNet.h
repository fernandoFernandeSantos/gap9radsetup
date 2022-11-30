#ifndef __ResNet_H__
#define __ResNet_H__

#define __PREFIX(x) ResNet ## x
// Include basic GAP builtins defined in the Autotiler
#include "Gap.h"

#ifdef __EMUL__
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/param.h>
#include <string.h>
#endif

extern AT_HYPERFLASH_EXT_ADDR_TYPE ResNet_L3_Flash;
extern AT_HYPERFLASH_EXT_ADDR_TYPE ResNet_L3_PrivilegedFlash;
#endif