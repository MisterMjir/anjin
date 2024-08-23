#ifndef EDISK_H
#define EDISK_H

#include <stdint.h>

// Completely rewrite this to use a file, but still read in sectors like the original does

void eDisk_Init(int);
void eDisk_ReadBlock(uint8_t *buffer, uint32_t sector);

#endif