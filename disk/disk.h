#ifndef DISK_H
#define DISK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void writeBlock(FILE* disk, int blockNum, unsigned char* data);

void readBlock(FILE* disk, int blockNum, unsigned char* buffer);

void InitLLFS();

#endif