#define vdisk_path "../disk/vdisk"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../disk/disk.h"
#include "../io/File.h"

int main() {
    printf("Formatting the disk\n");
    InitLLFS(); // Format the file system

    printf("Making a new directory \'/Adobe\' in the root directory\n");
    buffControl("mkdir", "/Adobe", "sample dir");
    printf("\n");

    printf("Making a new directory \'/Cache\' in the root directory\n");
    buffControl("mkdir", "/Cache", "new directory");
    printf("\n");

    printf("Making a new file \'/Documents\' in the root directory with contents occupying more than 1 block\n");
    buffControl("touch", "/Documents", "One megabyte is about 1 million bytes (or about 1000 kilobytes). An MP3 audio file of a few minutes or a 10 million pixel image from a digital camera would typically take up few megabytes. The rule of thumb for MP3 audio is that 1 minute of audio takes up about 1 megabyte. Audio and image and video data typically stored in \"compressed\" form, MP3 being an example. We'll talk about how compression works later. A data CD disk stores about 700 MB. The audio on a CD is not compressed, which is why it takes so much more space than the MP3. The series of bits are represented as spiral path of tiny pits in the silver material in the disk. Imagine that each pit is interpreted as a 0, and the lack of a pit is a 1 as the spiral sequence is read. Fun fact: the whole spiral on a CD is over 5km long.");
    printf("\n");

    printf("Making a new file \'/Adobe/Photos\' in the directory \'/Adobe\'\n");
    buffControl("touch", "/Adobe/Photos", "My photos");
    printf("\n");

    printf("Making a new file \'/Adobe/sample.txt\' in the directory \'/Adobe\' with contents occupying more than 1 block\n");
    buffControl("touch", "/Adobe/sample.txt", "One megabyte is about 1 million bytes (or about 1000 kilobytes). An MP3 audio file of a few minutes or a 10 million pixel image from a digital camera would typically take up few megabytes. The rule of thumb for MP3 audio is that 1 minute of audio takes up about 1 megabyte. Audio and image and video data typically stored in \"compressed\" form, MP3 being an example. We'll talk about how compression works later. A data CD disk stores about 700 MB. The audio on a CD is not compressed, which is why it takes so much more space than the MP3. The series of bits are represented as spiral path of tiny pits in the silver material in the disk. Imagine that each pit is interpreted as a 0, and the lack of a pit is a 1 as the spiral sequence is read. Fun fact: the whole spiral on a CD is over 5km long.");
    printf("\n");

    printf("Reading the contents of the directory \'/Adobe\'\n");
    buffControl("ls", "/Adobe", "Adobe");
    printf("\n");

    printf("Reading a file \'/Adobe/Photos\' containing more than 1 block in the directory \'/Adobe\'\n");
    buffControl("cat", "/Adobe/sample.txt", "Things");
    printf("\n");

    printf("Updating a file \'/Adobe/sample.txt\' in the \'/Adobe\' directory where the contents are\n the same in the first block and overall contents are increased above 2 blocks\n");
    buffControl("set", "/Adobe/sample.txt", "One megabyte is about 1 million bytes (or about 1000 kilobytes). An MP3 audio file of a few minutes or a 10 million pixel image from a digital camera would typically take up few megabytes. The rule of thumb for MP3 audio is that 1 minute of audio takes up about 1 megabyte. Audio and image and video data typically stored in \"compressed\" form, MP3 being an example. We'll talk about how compression works later. A data CD disk stores about 700 MB. The audio on a CD is not compressed, which is why it takes so much more space than the MP3. The series of bits are represented as spiral path of tiny pits in the silver material in the disk. Imagine that each pit is interpreted as a 0, and the lack of a pit is a 1 as the spiral sequence is read. Fun fact: the whole spiral on a CD is over 5km long. ***********EXTRA**********fdausnclneciln i a 848larluhr kjak3hajwhr jk jk hwajk hak h3 cjahkal hl hakljk3kjaacn hrajkc h3jkcacak jkac khc akjal h akj hjk nh nahjk cjXXXXXXXXXXXXXOne megabyte is about 1 million bytes (or about 1000 kilobytes). An MP3 audio file of a few minutes or a 10 million pixel image from a digital camera would typically take up few megabytes. The rule of thumb for MP3 audio is that 1 minute of audio takes up about 1 megabyte. Audio and image and video data typically stored in \"compressed\" form, MP3 being an example. We'll talk about how compression works later. A data CD disk stores about 700 MB. The audio on a CD is not compressed, which is why it takes so much more space than the MP3. The series of bits are represented as spiral path of tiny pits in the silver material in the disk. Imagine that each pit is interpreted as a 0, and the lack of a pit is a 1 as the spiral sequence is read. Fun fact: the whole spiral on a CD is over 10km long.");
    printf("\n");

    printf("Reading a file \'/Adobe/sample.txt\' in the \'/Adobe\' directory\n");
    buffControl("cat", "/Adobe/sample.txt", "print");
    printf("\n");

    printf("Updating a file \'/Adobe/sample.txt\' in the \'/Adobe\' directory where contents are reduced to 1 block\n");
    buffControl("set", "/Adobe/sample.txt", "ONE");
    printf("\n");

    printf("Reading a file \'/Adobe/sample.txt\' in the \'/Adobe\' directory\n");
    buffControl("cat", "/Adobe/sample.txt", "print");
    printf("\n");

    printf("Deleting a file \'/Adobe/sample.txt\' in the \'/Adobe\' directory\n");
    buffControl("rm", "/Adobe/sample.txt", "stuff");
    //buffControl("rm", "/Adobe/Photos", "stuff");
    printf("\n");

    printf("Reading a directory \'/Adobe\' in the root directory\n");
    buffControl("ls", "/Adobe", "print");
    printf("\n");

    printf("Deleting a file \'/Adobe/Photos\' in the \'/Adobe\' directory\n");
    buffControl("rm", "/Adobe/Photos", "stuff");
    printf("\n");

    printf("Reading a directory \'/Adobe\' in the root directory\n");
    buffControl("ls", "/Adobe", "print");
    printf("\n");

    printf("Reading the root directory\n");
    buffControl("ls", "/", "contents");
    printf("\n");

    printf("Deleting the \'Cache\' directory from the root directory\n");
    buffControl("rm -rf", "/Cache", "Cache directory");
    printf("\n");

    printf("Reading the root directory\n");
    buffControl("ls", "/", "contents");
    printf("\n");

    printf("Creating a new directory \'Downloads\' in the root directory\n");
    buffControl("mkdir", "/Downloads", "new dir");
    printf("\n");

    printf("Reading the contents of the root directory\n");
    buffControl("ls", "/", "contents");
    printf("\n");

    printf("Creating a new directory \'/Downloads/Updates\' in the /Downlods directory\n");
    buffControl("mkdir", "/Downloads/Updates", "new dir");
    printf("\n");

    printf("Reading the contents of the /Downlods directory\n");
    buffControl("ls", "/Downloads", "contents");
    printf("\n");

    printf("Creating a new directory \'/Downloads/Updates/New\'\n");
    buffControl("mkdir", "/Downloads/Updates/New", "new dir");
    printf("\n");

    printf("Reading the contents of the \'/Downloads/Updates directory\'\n");
    buffControl("ls", "/Downloads/Updates", "contents");
    printf("\n");

    printf("Creating a new directory \'/Downloads/Updates/New/Temp\'\n");
    buffControl("mkdir", "/Downloads/Updates/New/Temp", "new dir");
    printf("\n");

    printf("Reading the contents of the \'/Downloads/Updates/New\' directory\n");
    buffControl("ls", "/Downloads/Updates/New", "contents");
    printf("\n");

    printf("Creating two data files \'test.c\' and \'lowlevel.txt\' in the lowest level directory\n");
    buffControl("touch", "/Downloads/Updates/New/Temp/test.c", "new file");
    buffControl("touch", "/Downloads/Updates/New/Temp/lowlevel.txt", "This is a file in the lowest level directory");
    printf("\n");

    printf("Reading the contents of the \'/Downloads/Updates/New/Temp\' directory\n");
    buffControl("ls", "/Downloads/Updates/New/Temp", "contents");
    printf("\n");

    printf("Reading the contents of the \'/Downloads/Updates/New/Temp/lowlevel.txt\' file\n");
    buffControl("cat", "/Downloads/Updates/New/Temp/lowlevel.txt", "contents");
    printf("\n");

    buffControl("rm", "/Downloads/Updates/New/Temp/lowlevel.txt", "removing lowest file");
    printf("\n");

    printf("Reading the contents of the \'/Downloads/Updates/New/Temp\' directory\n");
    buffControl("ls", "/Downloads/Updates/New/Temp", "contents");
    printf("\n");

    printf("Deleting the \'/Downloads/Updates/New/Temp/test.c\' data file\n");
    buffControl("rm", "/Downloads/Updates/New/Temp/test.c", "removing lowest file");
    printf("\n");

    printf("Reading the contents of the \'/Downloads/Updates/New/Temp\' directory after removing deleting\'test.c\'\n");
    buffControl("ls", "/Downloads/Updates/New/Temp", "contents");
    printf("\n");

    printf("Reading the contents of the \'/Downloads/Updates/New\' directory\n");
    buffControl("ls", "/Downloads/Updates/New", "contents");
    printf("\n");

    printf("Deleting the \'/Downloads/Updates/New/Temp\' directory\n");
    buffControl("rm -rf", "/Downloads/Updates/New/Temp", ".");
    printf("\n");

    printf("Reading the contents of the \'/Downloads/Updates\' directory\n");
    buffControl("ls", "/Downloads/Updates", "contents");
    printf("\n");

    printf("Deleting the \'/Downloads/Updates/New\' directory\n");
    buffControl("rm -rf", "/Downloads/Updates/New", ".");
    printf("\n");

    printf("Reading the contents of the \'/Downloads\' directory\n");
    buffControl("ls", "/Downloads", "contents");
    printf("\n");

    printf("Deleting the \'/Downloads/Updates\' directory\n");
    buffControl("rm -rf", "/Downloads/Updates", ".");
    printf("\n");

    printf("Reading the contents of the root directory\n");
    buffControl("ls", "/", "contents");
    printf("\n");

    printf("Deleting the \'/Downloads\' directory from the root directory\n");
    buffControl("rm -rf", "/Downloads", ".");
    printf("\n");

    printf("Reading the contents of the root directory\n");
    buffControl("ls", "/", "contents");
    
    return 0;
}