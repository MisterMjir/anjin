#include "edisk.h"
#include <stdio.h>

FILE *disk;

void eDisk_Init(int idk)
{
	disk = fopen("res/mafia_love/images.img", "rb");
}

void eDisk_ReadBlock(uint8_t *buffer, uint32_t sector)
{
	fseek(disk, sector * 512, SEEK_SET);
	if (fread(buffer, 1, 512, disk) != 512) {
		fclose(disk);
		fopen("res/mafia_love/images.img", "rb");
	}
}
