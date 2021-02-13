#define vdisk_path "../disk/vdisk"
#define BLOCK_SIZE 512
#define NUM_BLOCKS 4096
#define NUM_INODES 128
#define INODE_SIZE 32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void writeBlock(FILE* disk, int blockNum, unsigned char* data) {
    fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET);
    fwrite(data, BLOCK_SIZE, 1, disk);
}

void readBlock(FILE* disk, int blockNum, unsigned char* buffer) {
    fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET);
    fread(buffer, BLOCK_SIZE, 1, disk);
}

void InitLLFS() {
    FILE* disk = fopen(vdisk_path, "wb+");
    char* init = calloc(BLOCK_SIZE * NUM_BLOCKS, 1);
    fwrite(init, BLOCK_SIZE * NUM_BLOCKS, 1, disk);
    free(init);
  
    unsigned char * buffer;
    buffer = (unsigned char *) calloc(BLOCK_SIZE, 1);
    int magic = 42;
    int blocks = NUM_BLOCKS;
    int inodes = NUM_INODES;
    memcpy(buffer, &magic, sizeof(magic)); // Write magic number in first 4 bytes
    memcpy(buffer + sizeof(int) * 1, &blocks, sizeof(int)); // Write number of blocks in next 4 bytes
    memcpy(buffer + sizeof(int) * 2, &inodes, sizeof(int)); // Write number of inodes in next 4 bytes
    writeBlock(disk, 0, buffer);    // Super block
    free(buffer);
    
    unsigned char * buffer2;
    buffer2 = (unsigned char *) calloc(BLOCK_SIZE, 1);
    memset(buffer2, 0, 1); // Indicating that the first 8 blocks are not availible for data
    memset(&buffer2[1], 31, 1); // Indicate last 2 unavailible blocks and first 6 free blocks
    memset(&buffer2[2], 255, 510); // Indicate free blocks
    writeBlock(disk, 1, buffer2); // Free block vector
    free(buffer2);

    // Initialize and write entry in inode map for root directory inode
    unsigned char * buffer4;
    buffer4 = calloc(BLOCK_SIZE, 1);
    int start = 1;
    int location = 4;//(10*BLOCK_SIZE)+1;
    memcpy(buffer4, &start, 4); //Fill the first location which is unused to skip it
    memcpy(buffer4 + 4, &location, 4);
    writeBlock(disk, 2, buffer4);
    memset(buffer4, 0, BLOCK_SIZE);
    writeBlock(disk, 3, buffer4);
    free(buffer4);

    // Initialize the root directory inode
    unsigned char * buffer5;
    buffer5 = calloc(BLOCK_SIZE, 1);
    int size = 32; // 32 since there is currently one entry for root directory
    memcpy(buffer5, &size, sizeof(int));
    int type = 1; // 1 denotes that this inode is for a directory
    memcpy(buffer5 + 4, &type, 4);
    // Next 20 bytes for 10 x 2 byte blocks of first data block locations
    int offset = 5; // Location of root directory block
    memcpy(buffer5 + 8, &offset, 2);

    // Then 2 bytes for single redirect and 2 for double redirect
    writeBlock(disk, 4, buffer5);
    
    // Writing root directory to first data block
    int inodeID = 1;
    char* name = "/";
    memset(buffer5, 0, BLOCK_SIZE);
    memcpy(buffer5, &inodeID, 1);
    strncpy((char*)buffer5+1, name, 31);
    writeBlock(disk, 5, buffer5);

    free(buffer5);
    fclose(disk);
    printf("vdisk has been formatted to LLFS\n");
}
