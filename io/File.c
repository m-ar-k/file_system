#define vdisk_path "../disk/vdisk"
#define BLOCK_SIZE 512
#define NUM_BLOCKS 4096
#define NUM_INODES 255
#define INODE_SIZE 32
#define BLOCK_OFFSET 20
#define DIRECTORY_REF_OFFSET 16

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "../disk/disk.h"

int parentRef;
int parentNode;
int parentInode;
int parentCheckpoint;
// Check a bit by refernce in free block vector to see if it is occupied
// Returns 1 if free, 0 if not

// Creation

int* checkFreeBlockBuffer(unsigned char* freeBlocks, int numBlocks) {
    int* returnBlocks = (int*)malloc(numBlocks*sizeof(int*));
    int blocksFound = 0;
    
    int j=4095;
    int i = 10;
    
    // This first loop check to see how far the blocks have been written to see where the beginning of the free blocks are
    for(;j>10;j--) {
        int bit = i%8;
        int pos = floor(((double)j / (double)NUM_BLOCKS) * BLOCK_SIZE); // Determine byte containing bit
        int vect = (int)freeBlocks[pos];
        vect = (vect >> (8-(bit+1))) & 1;

        if(vect==0) {
            i = j + 1;
            //printf("II:    %d\n",i);
            break;
        }
    }
    
    for (; i<4096; i++) {
        int bit = i%8;
        int pos = floor(((double)i / (double)NUM_BLOCKS) * BLOCK_SIZE); // Determine byte containing bit
        int vect = (int)freeBlocks[pos];
        vect = (vect >> (8-(bit+1))) & 1;

        if(vect==1) {
            returnBlocks[blocksFound] = i;
            blocksFound++;
        }
        if (blocksFound==numBlocks) break;
    }
    return returnBlocks;
}

void flipBlockBuffer(unsigned char* freeBlocks, int ref) {
    int bit = ref%8;
    int pos = floor(((double)ref / (double)NUM_BLOCKS)*BLOCK_SIZE);

    int vect = (int)freeBlocks[pos];
    bit = 8 - bit - 1;
    vect ^= 1UL << bit;
    memset(&freeBlocks[pos], vect, 1);
}

int createNewInode(int type, int* blocks, int blocknum, int size, unsigned char* inodeBuffer, unsigned char* writeBuffer, unsigned char* checkBuffer) {
 
    int i = 2;
    for (; i<255; i++) if ((int)*(checkBuffer+(i*4))==0) break;
    // Blocks not yet allocated
    memcpy(writeBuffer, &size, sizeof(int));
    memcpy(writeBuffer+4, &type, sizeof(int));
    //memset(&buffer6[(INODE_SIZE*(i%8))+sizeof(int)*2], 0, 24); // Set remaining memory to 0 since blocks have not yet been allocated
    //memcpy(buffer6+(INODE_SIZE*(i%8))+sizeof(int)*2, &try, 2); /* try is where first block location should be */
    for (int j=0; j<blocknum; j++) memcpy(writeBuffer+8+(j*2), &blocks[j], 2); // Write block locations
    
 
    return i; // The position in the inode map
}

void createNewDirectory(char* name, int type, int inode_num, int level, unsigned char* directoryBuffer) {
  
    // Create a new directory entry for the new file in the given directory location
    int len = strlen(name);
    unsigned char* file_name = calloc(31, 1);
    memcpy(file_name, name, len);
    file_name[len] = '\0'; // Necessary? Null termination of string name
   
    memcpy(directoryBuffer, &inode_num, 1); // 1st byte is inode number
    memcpy(directoryBuffer+1, file_name, 31); // Next 31 bytes are the name
    //writeBlock(blockWrite, i, buffer7);

    free(file_name);
    //free(buffer7);

}

// Update inode with block locations?
void createNewFlatFile(char* contents, char* name, int inodeRef, unsigned char* fileBuffer, int* blockLocations) { // pos is the first block allocated to start writing file data to
    /*
    FILE * fileWrite = fopen(vdisk_path, "rb+");
    unsigned char * buffer8;
    buffer8 = calloc(BLOCK_SIZE, 1);
    */

    int len = strlen(contents);
    int blocks = ceil((double)len/(double)BLOCK_SIZE);
    
    // Figure out filepath and/or which directory to put file in?
    // Update Inode with block locations
    if (blocks < 2) {
        /*
        int i=18;
        for(;i<NUM_BLOCKS;i++) if(checkFreeBlocks(i)==1) break;
        flipBlockVector(i);
        */
        strncpy((char*)fileBuffer, contents, len); 
        //writeBlock(fileWrite, i, buffer8);
    }
    else {
        /*
        int* block_locations = (int*)malloc(sizeof(int)*blocks); // Store found locations of free blocks
        int j=18; // Offset for reserved blocks
        for(int i=0; i<blocks; i++) { // Loop through data and write to blocks
            // Try to allocate all other blocks needed
            for(;j<NUM_BLOCKS;j++) if(checkFreeBlocks(j)==1) break;
            if (j == 128) printf("Error: failed to allocate enough blocks for new file\n"); // Return error if there is not enough space availible
            block_locations[i] = j;  
            j++;
        }
        */
        for (int i=0; i<blocks; i++) {
            //flipBlockVector(block_locations[i]); // Set found free location to occupied
            memset(fileBuffer+(i*BLOCK_SIZE), 0, BLOCK_SIZE);
            strncpy((char*)fileBuffer+(i*BLOCK_SIZE), contents+(BLOCK_SIZE*i), BLOCK_SIZE);
            //writeBlock(fileWrite, block_locations[i], buffer8);
        }
    }
    //free(buffer8);
    //fclose(fileWrite);
}

int findLevel(char* path) { // Return depth of path, if path does not start with '/' then return 0 which denotes an error
    int levels = 0;
    int i = 0;
    if (path[i]!='/') return 0;
    for (; i<strlen(path); i++) if (path[i] == '/') levels++;
    if (path[i-1]=='/') levels--;
    if (levels==0) levels = 1;
    return levels;
}

// Modification

void addToDirectory(int inodeNum, int directoryRef, int dirSize, char* filename, unsigned char* writeBuffer, unsigned char* checkBuffer, int* buffBlocks, int numBlocks, unsigned char* freeBlockBuffer) { //Inode for new file, parent directory data block, parent directory current size, name of file being added
    
    FILE * readDir = fopen(vdisk_path, "rb+");
    readBlock(readDir, directoryRef, writeBuffer);
    fclose(readDir);

    unsigned char* dirBuff = writeBuffer;
    //printf("%d\n", j);

    int i = 1;
    for (; i<16; i++) if ((int)*(dirBuff+(i*32))==0) break;
    memcpy(dirBuff+(i*32), &inodeNum, 1);
    strncpy((char*)dirBuff+(i*32)+1, filename, 31);
    //printf("%s\n",writeBuffer+(j*BLOCK_SIZE)+(i*32)+1);
    dirSize += 32;

    memcpy(checkBuffer+(inodeNum*4), &buffBlocks[numBlocks], 4); // Updating the inode map for new inode location, special case for root?
    flipBlockBuffer(freeBlockBuffer, directoryRef); // Free old directory block
   
}

int checkIfFileIsInDirectory(char* directory, char* name) { // Assuming that block for directory has already been read in
    for (int i = 1; i<16; i++) {
        if ((int)*(directory + (i*INODE_SIZE)) == 0) return 0;  // No more entries
        else if (strcmp(directory+(i*INODE_SIZE)+1, name)==0) return (int)*(directory+(i*INODE_SIZE)); // If a matching file is found, return its inode index number
    }
    return 0;
}

int traverseFileTreeCheck(FILE* fp, unsigned char* inodesBuffer, unsigned char* rootDir, char* path, char*filename, int filetype, int* buffBlocks, unsigned char* writeBuffer, unsigned char* checkBuffer) { // Inodes buffer = inode map cache
    unsigned char* tempDir = calloc(BLOCK_SIZE, 1);
    readBlock(fp, (int)*(checkBuffer+4), tempDir);
    parentRef = (int)*(tempDir+8); // location of root directory block
    free(tempDir);
    readBlock(fp, parentRef, rootDir);
    parentCheckpoint = 1; // root directory

    int foundFile = 0;
    int level = findLevel(path);
    char rath[strlen(path)+1];
    strcpy(rath, path);

    char* dir;
    dir = strtok(rath, "/");

    if(!dir) dir = filename;
    //printf("%s\n",dir); //debug
    if (level==1) parentNode = 1;
    
    for (int j=0; j<level; j++) {
        foundFile = checkIfFileIsInDirectory((char*)rootDir, dir); // Find the block reference to the next directory in the path
        //printf("%d\n",foundFile);
        
        if (foundFile == 0) {
            if(j < level-1) printf("Error: Parent folder does not exist");
            return 0;
        }
        else {
            memset(rootDir, 0, BLOCK_SIZE);
            //readBlock(fp, (int)*(inodesBuffer+((foundFile-1)*INODE_SIZE)+8), rootDir); // Assuming that next file is a directory, otherwise should check type flag at +4
                                                                                         // Maybe need to load in more inodes into checkbuffer (total of 8 blocks)
                                                                                         //ISSUE: Inode has not been allocated for this directory
            int dirPos = (int)*(inodesBuffer+(foundFile*4)); //Find location in inode checkpoint (*4 since each entry has 4 bytes)
            //int dirBlock = floor((double)dirPos/(double)512);
            //int dirBlockPos = dirPos%dirBlock; // Handling block location based on address to true location if there are multiple inodes per block
           
            //int nextDir = (int)*(rootDir+8);
            if (j==level-2) parentCheckpoint = foundFile;
            parentNode = foundFile;
            parentInode = dirPos;
            unsigned char* temp = calloc(BLOCK_SIZE, 1);
            readBlock(fp, dirPos, temp);
            memset(rootDir, 0, BLOCK_SIZE);
            readBlock(fp, *(temp+8), rootDir);
            parentRef = *(temp+8);
            free(temp);
            //parentRef = nextDir;
            if (j==level) printf("The file already exits\n");
            //parentRef = (int)*(inodesBuffer+((foundFile-1)*INODE_SIZE)+8);
            //printf("Parent: %d\n",(int)*(inodesBuffer+((foundFile-1)*INODE_SIZE)+8));
            }
            dir = strtok(NULL, "/");
        }
        return parentRef;
        //return parentRef;
        
}

char* findName(char* path, int level) {
    //char* name = strtok(path, "/");
    //printf("path: %s\t%lu\n",path,strlen(path));
    int i = 0;
    int found = 0;
    for(; i<strlen(path); i++){
        if(path[i]=='/') found++;
        if (found==level) {i++; break;}
    }
    //printf("i %lu\n",strlen(path)-i+1);
    char* name = calloc(sizeof(char*)*(strlen(path)-i+1), 1);
    int temp = i;
    for(int j=0;j<(strlen(path)-i); j++) {name[j] = path[temp]; temp++;}
    //printf("name %s\n",name);
    return name;
} 

int* newFile(FILE* fp, char* contents, char* path, int type, unsigned char* freeBlockBuffer, unsigned char* checkBuffer, unsigned char* dirBuffer, unsigned char* writeBuffer, int* blocksPositions, int fileBlocks) { //Need to add blocks are parameter for inode and inode as parameter for new directory, don't create either until blocks figured out
    //int parentRef = 0;

    readBlock(fp, 1, freeBlockBuffer);
    readBlock(fp, 2, checkBuffer);
    readBlock(fp, 3, checkBuffer+BLOCK_SIZE);
    readBlock(fp, 5, dirBuffer); // start with root directory

    int i = 2; // Skip root directory
    for (; i<255; i++) if((int)*(checkBuffer+(i*4))==0) break; //Check if there is a free inode

    char* name = findName(path, findLevel(path));

    traverseFileTreeCheck(fp, checkBuffer, dirBuffer, path, name, type, blocksPositions, writeBuffer, checkBuffer);
    
        int newInode = 0;
        if(type == 1) { // If directory does not exist create a new inode and directory
           
            //int* freeBlocks = checkFreeBlockBuffer(freeBlockBuffer, 1);
            blocksPositions = checkFreeBlockBuffer(freeBlockBuffer, 4);  //One for file and one for inode
            for (int k=0; k<fileBlocks+3; k++) flipBlockBuffer(freeBlockBuffer, blocksPositions[k]);
           
            // Pass in free block location to inode?
            // blockPositions[fileBlocks] is the location of the inode block
            newInode = createNewInode(1, blocksPositions, 1, 32, checkBuffer, writeBuffer+(fileBlocks*BLOCK_SIZE), checkBuffer);
            memcpy(checkBuffer+(newInode*4), &blocksPositions[fileBlocks], 4); // Set location in inode map
            createNewDirectory(name, 1, newInode, findLevel(path), writeBuffer);
        }
        else if (type == 0) { // If a new flat file is being created
        
            // Add blocks numbers to a list/array and then search through all of them in block storage space
            // see if block can be allocated for directory/file if it does not exist
            //int* freeBlocks = checkFreeBlockBuffer(freeBlockBuffer, fileBlocks);
            blocksPositions = checkFreeBlockBuffer(freeBlockBuffer, fileBlocks+3);
            for (int k=0; k<fileBlocks+3; k++) flipBlockBuffer(freeBlockBuffer, blocksPositions[k]);
            // Check if Inode can be created before creating file, create file and then create inode passing in block_locations[] (int pointer)?
            newInode = createNewInode(0, blocksPositions, fileBlocks, strlen(contents), checkBuffer, writeBuffer+(fileBlocks*BLOCK_SIZE), checkBuffer); 
            memcpy(checkBuffer+(newInode*4), &blocksPositions[fileBlocks], 4); // Set location in inode map

            createNewFlatFile(contents, name, newInode, writeBuffer, 0);
        }
        
        //printf("dir loc: %d\n", parentRef);
        addToDirectory(newInode, parentRef, 32, name, writeBuffer+((fileBlocks+1)*BLOCK_SIZE), checkBuffer, blocksPositions, fileBlocks, freeBlockBuffer); // Need to figure out how to updates directory sizes (in the inode)
        readBlock(fp, (int)*(checkBuffer+(parentNode*4)), writeBuffer+((fileBlocks+2)*BLOCK_SIZE)); // Read in old inode for updated directory
        memcpy(writeBuffer+((fileBlocks+2)*BLOCK_SIZE)+8, &blocksPositions[fileBlocks+1], 2); // Update inode of updated directory
        memcpy(checkBuffer+(parentNode*4), &blocksPositions[fileBlocks+2], 4); // Update the checkpoint location for new copy of inode

        //else addToDirectory(newInode, (int)*(checkBuffer+((exists-1)*INODE_SIZE)+8), (int)*(checkBuffer+((exists-1)*INODE_SIZE)), name); 
        
        //printf("\ndir containter: %st%d\n",name, (int)*(checkBuffer+((exists-1)*INODE_SIZE)+8));

        return blocksPositions;
}

void readDirectory(FILE* fp, unsigned char* checkBuffer, unsigned char* rootDir, char* path, int* buffBlocks, unsigned char* writeBuffer) {
    readBlock(fp, 2, checkBuffer);
    readBlock(fp, 3, checkBuffer+BLOCK_SIZE);
    readBlock(fp, 5, rootDir); // start with root directory

    char* name = findName(path, findLevel(path));
    traverseFileTreeCheck(fp, checkBuffer, rootDir, path, name, 1, buffBlocks, writeBuffer, checkBuffer);
    // Need condition if file DNE

    readBlock(fp, parentRef, rootDir);

    printf("%s\n",path);
    for(int i=1; i<16; i++) {
        if (*(rootDir+(32*i))==0) continue;
        printf("%d\t%s\n",*(rootDir+(32*i)),rootDir+(32*i)+1);
    }
}

void readFile(FILE* fp, unsigned char* checkBuffer, unsigned char* rootDir, char* path, int* buffBlocks, unsigned char* writeBuffer) {
    readBlock(fp, 2, checkBuffer);
    readBlock(fp, 3, checkBuffer+BLOCK_SIZE);
    readBlock(fp, 5, rootDir); // start with root directory

    char* name = findName(path, findLevel(path));
    traverseFileTreeCheck(fp, checkBuffer, rootDir, path, name, 1, buffBlocks, writeBuffer, checkBuffer);

    readBlock(fp, (int)*(checkBuffer+(parentNode*4)), rootDir);
    unsigned char* readBuffer = calloc(BLOCK_SIZE, 1);

    int i=0;
    for(; i<10; i++) {
        if ((int)*(rootDir+8+(2*i))==0) break;
        memset(readBuffer, 0, BLOCK_SIZE);
        readBlock(fp, (int)*(rootDir+8+(2*i)), readBuffer);
        printf("%s",readBuffer);
    }
    printf("\n");
}

int* deleteDirectory(FILE* fp, unsigned char* checkBuffer, unsigned char* freeBlockBuffer, unsigned char* rootDir, char* path, int* buffBlocks, unsigned char* writeBuffer) {
    readBlock(fp, 1, freeBlockBuffer);
    readBlock(fp, 2, checkBuffer);
    readBlock(fp, 3, checkBuffer+BLOCK_SIZE);
    readBlock(fp, 5, rootDir); // start with root directory

    char* name = findName(path, findLevel(path));
    traverseFileTreeCheck(fp, checkBuffer, rootDir, path, name, 1, buffBlocks, writeBuffer, checkBuffer);
    readBlock(fp, parentRef, rootDir);
    for (int i=1; i>16; i++) if (*(rootDir+(i*32))!=0) {printf("Error: Directory contains files\n"); return buffBlocks;} 
    flipBlockBuffer(freeBlockBuffer, parentRef);
    //flipBlockBuffer(freeBlockBuffer, parentNode); // Not freeing entire checkpoint block
    flipBlockBuffer(freeBlockBuffer, (int)*(checkBuffer+(parentNode*4))); //inode
    int clear = 0;

    ///
    buffBlocks = checkFreeBlockBuffer(freeBlockBuffer, 2); // One block for updated directory and one for its inode

    unsigned char* tempBuffer = calloc(BLOCK_SIZE, 1);
    memset(rootDir, 0, BLOCK_SIZE);
    readBlock(fp, (int)*(checkBuffer+(parentCheckpoint*4)), tempBuffer); // get the inode
    readBlock(fp, (int)*(tempBuffer+8), rootDir);

    int i = 1;
    for(;i<16;i++) if ((int)*(rootDir+(i*32))==parentNode) break;
    //printf("FOUND %d\n",(int)*(rootDir+(i*32)));
 
    memcpy(rootDir+(i*32), &clear, 1); // readDirectory only checks first byte, if 0 then it skips so setting 0 should free the directory entry
    memcpy(writeBuffer, rootDir, BLOCK_SIZE);
    
    // Updating parent directory inode
    memcpy(tempBuffer+8, &buffBlocks[0], 2);
    memcpy(writeBuffer+BLOCK_SIZE, tempBuffer, BLOCK_SIZE);

    flipBlockBuffer(freeBlockBuffer, (int)*(checkBuffer+(parentNode*4))); // free old inode
    memcpy(checkBuffer+(parentNode*4), &clear, 4); // Reset the inode checkpoint
    
    memcpy(checkBuffer+(parentCheckpoint*4), &buffBlocks[1], 4); // checkpoint update
   
    return buffBlocks;
}

int* deleteFile(FILE* fp, unsigned char* checkBuffer, unsigned char* freeBlockBuffer, unsigned char* rootDir, char* path, int* buffBlocks, unsigned char* writeBuffer) {
    readBlock(fp, 1, freeBlockBuffer);
    readBlock(fp, 2, checkBuffer);
    readBlock(fp, 3, checkBuffer+BLOCK_SIZE);
    readBlock(fp, 5, rootDir); // start with root directory
    unsigned char* tempBuffer = calloc(BLOCK_SIZE, 1);

    char* name = findName(path, findLevel(path));
    traverseFileTreeCheck(fp, checkBuffer, rootDir, path, name, 1, buffBlocks, writeBuffer, checkBuffer);
    readBlock(fp, *(checkBuffer+(parentNode*4)), rootDir);
    for(int i=0; i<10; i++){
        if (*(rootDir+8+(i*2))==0) break;
        flipBlockBuffer(freeBlockBuffer, *(rootDir+8+(i*2)));
    }

    buffBlocks = checkFreeBlockBuffer(freeBlockBuffer, 2); // One block for updated directory and one for its inode
    flipBlockBuffer(freeBlockBuffer, buffBlocks[0]);
    flipBlockBuffer(freeBlockBuffer, buffBlocks[1]);

    if (parentCheckpoint==0) parentCheckpoint = 1;
    
    // Updating parent directory
    memset(rootDir, 0, BLOCK_SIZE);
    readBlock(fp, (int)*(checkBuffer+(parentCheckpoint*4)), tempBuffer); // get the inode
    readBlock(fp, (int)*(tempBuffer+8), rootDir);

    int i = 1;
    for(;i<16;i++) if ((int)*(rootDir+(i*32))==parentNode) break;
   
    int clear = 0;
    memcpy(rootDir+(i*32), &clear, 1); // readDirectory only checks first byte, if 0 then it skips so setting 0 should free the directory entry
    //memset(rootDir+(i*32), 0, 32);
    memcpy(writeBuffer, rootDir, BLOCK_SIZE);
    
    // Updating parent directory inode
    memcpy(tempBuffer+8, &buffBlocks[0], 2);
    memcpy(writeBuffer+BLOCK_SIZE, tempBuffer, BLOCK_SIZE);
    
     
    flipBlockBuffer(freeBlockBuffer, (int)*(checkBuffer+(parentNode*4))); // free old inode
    memcpy(checkBuffer+(parentNode*4), &clear, 4); // Reset the inode checkpoint
    
    memcpy(checkBuffer+(parentCheckpoint*4), &buffBlocks[1], 4); // checkpoint update

    return buffBlocks;
}

int* updateFile(FILE* fp, unsigned char* checkBuffer, unsigned char* freeBlockBuffer, unsigned char* rootDir, char* path, int* buffBlocks, unsigned char* writeBuffer, char* contents) {
    readBlock(fp, 1, freeBlockBuffer);
    readBlock(fp, 2, checkBuffer);
    readBlock(fp, 3, checkBuffer+BLOCK_SIZE);
    readBlock(fp, 5, rootDir); // start with root directory
    unsigned char* oldBuffer = calloc(BLOCK_SIZE, 1);
    unsigned char* newBuffer = calloc(BLOCK_SIZE, 1);

    char* name = findName(path, findLevel(path));
    traverseFileTreeCheck(fp, checkBuffer, rootDir, path, name, 1, buffBlocks, writeBuffer, checkBuffer);
    int blocksRequired = ceil((double)strlen(contents)/(double)BLOCK_SIZE);
    int* newBlocks = (int*)malloc(sizeof(int*)*blocksRequired);

    readBlock(fp, (int)*(checkBuffer+(parentNode*4)), rootDir);
    int updatedBlock = 0;
    int sameBlock = 0;
    char* temp = calloc(BLOCK_SIZE, 1);

    for (int i=0; i<blocksRequired+1; i++) {
        //if (*(rootDir+8+(i*2))==0) break;
        readBlock(fp, (int)*(rootDir+8+(i*2)), oldBuffer);
        memcpy(temp, contents+(BLOCK_SIZE*i), BLOCK_SIZE);
        //printf("THINGS:  %s\n\n\n%s\n", oldBuffer,contents+(BLOCK_SIZE*i));
        if (strcmp((char*)oldBuffer, temp)==0) {
            sameBlock++;
            continue; // If current block is the same as updated version, it doesn't need to be changed
        }
        else { // If updated block is differnet, record it to the buffer
            strncpy((char*)writeBuffer+(updatedBlock*BLOCK_SIZE), contents+(BLOCK_SIZE*i), BLOCK_SIZE);
            newBlocks[updatedBlock] = i;//*(rootDir+8+(i*2)); // If any blocks in updated contents are the same as on disk, record them
            //flipBlockBuffer(freeBlockBuffer, *(rootDir+8+(i*2)));
            updatedBlock++;
        }
    }
    //printf("SAME BLOCK %d\t BLOCKS REQUIED: %d\tUPDATED BLCOKS%d\n",sameBlock, blocksRequired, updatedBlock);
 
    buffBlocks = checkFreeBlockBuffer(freeBlockBuffer, blocksRequired+1); // Allocate new blocks for each updated block plus the inode
    int i=sameBlock-1;
    if (i<0) i=0;
    for (;i<blocksRequired; i++) {
        flipBlockBuffer(freeBlockBuffer, *(rootDir+8+(newBlocks[i]*2))); // Free the old blocks that are being updated
        memcpy(rootDir+8+(newBlocks[i]*2), &buffBlocks[i], 2); // Update inode block location references
        flipBlockBuffer(freeBlockBuffer, buffBlocks[i]); 
    }
    flipBlockBuffer(freeBlockBuffer, buffBlocks[blocksRequired]);
   //for (int k=0; k<blocksRequired; k++) { // Indicate the newly allocated blocks as occupied
    int clear = 0;
    for (;i<10; i++) memcpy(rootDir+8+(i*2), &clear, 2); // Clear out old unused references when new file is smaller then the old

    memcpy(checkBuffer+(parentNode*4), &buffBlocks[blocksRequired], 4); // Update the inode checkpoint
    memcpy(writeBuffer+(blocksRequired*BLOCK_SIZE), rootDir, BLOCK_SIZE); // Copy updated inode to end of buffer
    //printf("Inode stats %d\n", (int)*(rootDir));
    free(oldBuffer);
    free(newBuffer);
    return buffBlocks;
}

void buffControl(char* arg, char* name, char* contents) {
   
    FILE * fp = fopen(vdisk_path, "rb+");
    unsigned char* freeBlockBuffer = calloc(BLOCK_SIZE, 1);
    unsigned char* checkBuffer = calloc(BLOCK_SIZE*2, 1); // Store all the inode references
    unsigned char* dirBuffer = calloc(BLOCK_SIZE, 1);
    unsigned char* writeBuffer;
    int* blocksPositions = NULL;
    int fileBlocks = 0;

    if (strcmp(arg, "mkdir")==0) {

        fileBlocks = ceil((double)strlen(contents)/(double)BLOCK_SIZE);
        writeBuffer = calloc(BLOCK_SIZE, fileBlocks+3); // One extra for inode, and another for the new directory version
        blocksPositions = newFile(fp, contents, name, 1, freeBlockBuffer, checkBuffer, dirBuffer, writeBuffer, blocksPositions, fileBlocks);

    }
    else if (strcmp(arg, "touch")==0) {
       
        fileBlocks = ceil((double)strlen(contents)/(double)BLOCK_SIZE);
        writeBuffer = calloc(BLOCK_SIZE, fileBlocks+3);
        blocksPositions = newFile(fp, contents, name, 0, freeBlockBuffer, checkBuffer, dirBuffer, writeBuffer, blocksPositions, fileBlocks);

    }

    else if (strcmp(arg, "ls")==0) {
        writeBuffer = calloc(BLOCK_SIZE, 1);
        readDirectory(fp, checkBuffer, dirBuffer, name, blocksPositions, writeBuffer);
        return; //need to close buffers and file
    }

    else if (strcmp(arg, "cat")==0) {
        writeBuffer = calloc(BLOCK_SIZE, 1);
        readFile(fp, checkBuffer, dirBuffer, name, blocksPositions, writeBuffer);
        return; //need to close buffers and file
    }

    else if (strcmp(arg, "rm -rf")==0) {
        writeBuffer = calloc(BLOCK_SIZE, 2);
        blocksPositions = deleteDirectory(fp, checkBuffer, freeBlockBuffer, dirBuffer, name, blocksPositions, writeBuffer);
        fileBlocks = -1;
    }

    else if (strcmp(arg, "rm")==0) {
        writeBuffer = calloc(BLOCK_SIZE, 2);
        blocksPositions = deleteFile(fp, checkBuffer, freeBlockBuffer, dirBuffer, name, blocksPositions, writeBuffer);
        fileBlocks = -1;
    }

    else if (strcmp(arg, "set")==0) { // Change this later to check if file already exists, run this if it does
        fileBlocks = ceil((double)strlen(contents)/(double)BLOCK_SIZE);
        writeBuffer = calloc(BLOCK_SIZE, fileBlocks+1);
        blocksPositions = updateFile(fp, checkBuffer, freeBlockBuffer, dirBuffer, name, blocksPositions, writeBuffer, contents);
        fileBlocks = fileBlocks-1;
    }

    // assign bytes for file and directory to corresponding inode and name to director
    writeBlock(fp, 1, freeBlockBuffer);
    writeBlock(fp, 2, checkBuffer);
    writeBlock(fp, 3, checkBuffer+BLOCK_SIZE);
    
    for(int i=0; i<fileBlocks+3; i++) {
        if (blocksPositions[i]<1) break;
        writeBlock(fp, blocksPositions[i], writeBuffer+(i*BLOCK_SIZE));
    }
    
    //for(int i=0; i<fileBlocks+3; i++) printf("Blcoks: %d\n", blocksPositions[i]);

    free(freeBlockBuffer);
    free(checkBuffer);
    free(dirBuffer);
    fclose(fp);
}

