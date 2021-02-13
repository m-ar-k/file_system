#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

int* checkFreeBlockBuffer(unsigned char* freeBlocks, int numBlocks);

void flipBlockBuffer(unsigned char* freeBlocks, int ref);

int createNewInode(int type, int* blocks, int blocknum, int size, unsigned char* inodeBuffer, unsigned char* writeBuffer, unsigned char* checkBuffer);

void createNewDirectory(char* name, int type, int inode_num, int level, unsigned char* directoryBuffer, int numBlocks);

void createNewFlatFile(char* contents, char* name, int inodeRef, unsigned char* fileBuffer, int* blockLocations);

int findLevel(char* path);

void addToDirectory(int inodeNum, int directoryRef, int dirSize, char* filename, unsigned char* writeBuffer, unsigned char* checkBuffer, int* buffBlocks, unsigned char* freeBlocksBuffer);

int checkIfFileIsInDirectory(unsigned char* directory, char* name);

int traverseFileTreeCheck(FILE* fp, unsigned char* inodesBuffer, unsigned char* rootDir, char* path, char*filename, int filetype, int* buffBlocks, unsigned char* writeBuffer, unsigned char* checkBuffer);

char* findName(char* path, int level);

int* newFile(FILE* fp, char* contents, char* path, int type, unsigned char* freeBlockBuffer, unsigned char* checkBuffer, unsigned char* dirBuffer, unsigned char* writeBuffer, int* blocksPositions, int fileBlocks);

void readDirectory(FILE* fp, unsigned char* checkBuffer, unsigned char* rootDir, char* path, int* buffBlocks, unsigned char* writeBuffer);

void readFile(FILE* fp, unsigned char* checkBuffer, unsigned char* rootDir, char* path, int* buffBlocks, unsigned char* writeBuffer);

int* deleteDirectory(FILE* fp, unsigned char* checkBuffer, unsigned char* freeBlockBuffer, unsigned char* rootDir, char* path, int* buffBlocks, unsigned char* writeBuffer);

int* deleteFile(FILE* fp, unsigned char* checkBuffer, unsigned char* freeBlockBuffer, unsigned char* rootDir, char* path, int* buffBlocks, unsigned char* writeBuffer);

int* updateFile(FILE* fp, unsigned char* checkBuffer, unsigned char* freeBlockBuffer, unsigned char* rootDir, char* path, int* buffBlocks, unsigned char* writeBuffer, char* contents);

void buffControl(char* arg, char* name, char* contents);

#endif