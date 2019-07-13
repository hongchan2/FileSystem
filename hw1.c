#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "hw1.h"

#define PERMS 0644

const char* PATH = "hw1.txt";
const char A = '1';
const char F = '0';

void searchEmptySpace(int fd, int size);
int searchAllocateSpace(int fd);
void checkBlock(int fd, int blockSize);
void checkBlockForeword(int fd, int blockSize);
int checkBlockBackword(int fd, int blockSize);

void InsertData(char* key, int keySize, char* pBuf, int bufSize){
    char *pMem = NULL;
    int pMemSize = 5 + keySize + bufSize;
    ssize_t wsize = 0;

    pMem = (char*)malloc(pMemSize);
    pMem[0] = A; // '1'
    pMem[1] = pMemSize;
    pMem[2] = keySize;
    pMem[3] = bufSize;
    strncpy(pMem + 4, key, keySize);
    strncpy(pMem + (4 + keySize), pBuf, bufSize);
    pMem[pMemSize - 1] = pMemSize;

    int fd = open(PATH, O_CREAT | O_RDWR, PERMS);
    if(fd == -1){
        perror("InsertData open ERROR!");
        exit(-1);
    }
    searchEmptySpace(fd, pMemSize); // search empty space

    wsize = write(fd, pMem, pMemSize); // write data
    if(wsize == -1){
        perror("InsertData write ERROR!");
        exit(-1);
    }
    free(pMem);
    pMem = NULL;
    close(fd);
}

int getDataByKey(char* key, int keySize, char* pBuf, int bufSize){
    ssize_t rsize = 0;
    char* blockKey = NULL;
    char* newBlock = NULL;
    int blockKeySize = 0;
    int blockBufSize = 0;
    int blockSize = 0;
    int fd = 0;

    fd = open(PATH, O_RDONLY);
    if(fd == -1){
        perror("getDataByKey open ERROR!");
        exit(-1);
    }

    while(1){
        blockSize = searchAllocateSpace(fd); // check allocated or free block
        //printf("blockSize : %d\n", blockSize);
        if(blockSize == 0) // eof
            break;

        rsize = read(fd, &blockKeySize, 1); // get blockKeySize
        if(rsize == -1){
            perror("getDataByKey blockKeySize read ERROR!");
            exit(-1);
        }

        rsize = read(fd, &blockBufSize, 1); // get blcokBufSize
        if(rsize == -1){
            perror("getDataByKey blockBufSize read ERROR!");
            exit(-1);
        }

        blockKey = (char*)malloc(blockKeySize);
        rsize = read(fd, blockKey, blockKeySize); // get blockKey

        //printf("blockKeySize : %d\n", blockKeySize);
        //printf("blockBufSize : %d\n", blockBufSize);
        //printf("blockKey : %s \n\n", blockKey);

        if(strcmp(key, blockKey) == 0){
            // store data and return data size
            rsize = read(fd, pBuf, blockBufSize); // store data to pBuf
            if(rsize == -1){
                perror("getDataByKey pBuf read ERROR!");
                exit(-1);
            }
            free(blockKey);
            blockKey = NULL;
            close(fd);
            return blockBufSize; // return stored data size
        }
        else{
            // jump to next block
            lseek(fd, blockBufSize + 1, SEEK_CUR);
        }

    }
    free(blockKey);
    blockKey = NULL;
    close(fd);
    return -1;
}

void RemoveDataByKey(char* key, int keySize){
    int fd = 0;
    int blockSize = 0;
    ssize_t rsize = 0;
    ssize_t wsize = 0;
    int blockKeySize = 0;
    int blockBufSize = 0;
    char* blockKey = NULL;
    const char freeMark = F;

    fd = open(PATH, O_RDWR);
    if(fd == -1){
        perror("RemoveDataByKey open ERROR!");
        exit(-1);
    }

    while(1){
        blockSize = searchAllocateSpace(fd); // check allocated or free block
        if(blockSize == 0) // eof
            break;

        rsize = read(fd, &blockKeySize, 1); // get block key size
        if(rsize == -1){
            perror("RemoveDataByKey blockKeySize read ERROR!");
            exit(-1);
        }

        rsize = read(fd, &blockBufSize, 1); // get blcok buf size
        if(rsize == -1){
            perror("RemoveDataByKey blockBufSize read ERROR!");
            exit(-1);
        }

        blockKey = (char*)malloc(blockKeySize);
        rsize = read(fd, blockKey, blockKeySize); // get block key

        //printf("blockKeySize : %d\n", blockKeySize);
        //printf("blockBufSize : %d\n", blockBufSize);
        //printf("blockKey : %s \n\n", blockKey);

        if(strcmp(key, blockKey) == 0){
            // Remove block (A -> F)
            lseek(fd, -(4 + keySize), SEEK_CUR);
            wsize = write(fd, &freeMark, 1);
            if(wsize == -1){
                perror("RemoveDataByKey write ERROR!");
                exit(-1);
            }
            //printf("A -> F SUCCESS!\n");
            // check next and previous block
            checkBlock(fd, blockSize);
            break;
        }
        else{
            // jump to next block
            lseek(fd, blockBufSize + 1, SEEK_CUR);
        }
    }
    free(blockKey);
    blockKey = NULL;
    close(fd);
}

void searchEmptySpace(int fd, int size){
    ssize_t rsize = 0;
    ssize_t wsize = 0;
    int content = 0;
    int blockSize = 0;
    int newBlockSize = 0;
    const char freeMark = F;

    off_t offset = 0;

    while(1){
        rsize = read(fd, &content, 1);
        if(rsize == 0){ // eof
            //printf("End of File. insert End! \n");
            offset = lseek(fd, 0, SEEK_END);
            break;
        }
        if(rsize == -1){
            perror("searchEmptySpace content read ERROR!");
            exit(-1);
        }

        rsize = read(fd, &blockSize, 1); // get block size
            if(rsize == -1){
                perror("searchEmptySpace bsize read ERROR!");
                exit(-1);
        }
        lseek(fd, -2, SEEK_CUR);

        if((content == F) && (blockSize >= size)){
            // find free block and enough space
            //printf("set next block free and size\n");
            newBlockSize = blockSize - size;
            if(blockSize == size)
                break;
            lseek(fd, size, SEEK_CUR);
            // set next block free
            wsize = write(fd, &freeMark, 1);
            if(wsize == -1){
                perror("searchEmptySpace freeMark write ERROR!");
                exit(-1);
            }
            // write new block size
            wsize = write(fd, &newBlockSize, 1);
            if(wsize == -1){
                perror("searchEmptySpace newBlockSize write ERROR!");
                exit(-1);
            }
            lseek(fd, newBlockSize - 3, SEEK_CUR);
            // write new block size
            wsize = write(fd, &newBlockSize, 1);
            if(wsize == -1){
                perror("searchEmptySpace newBlockSize write ERROR!");
                exit(-1);
            }

            // back offset
            lseek(fd, -blockSize, SEEK_CUR);
            break;
        }
        else{
            // not a empty space
            // jump to next block 
            lseek(fd, blockSize, SEEK_CUR);
        }
    }
}

int searchAllocateSpace(int fd){
    ssize_t rsize = 0;
    int content = 0;
    int blockSize = 0;

    while(1){
        rsize = read(fd, &content, 1); // get block content
        if(rsize == 0){ // eof
            return 0;
        }
        if(rsize == -1){
        perror("searchAllocateSpace content read ERROR!");
            exit(-1);
        }
        rsize = read(fd, &blockSize, 1); // get block size
        if(rsize == -1){
            perror("searchAllocateSpace bsize read ERROR!");
            exit(-1);
        }

        if(content == A){
            //printf("This is Allocate space! return blockSize\n");
            // this block is allcated block
            return blockSize;
        }
        else{
            //printf("content : %d\n", content);
            //printf("This is free space! lseek next block\n");
            // this block is free block
            lseek(fd, blockSize - 2, SEEK_CUR);
        }
    }
    return 0;
}

void checkBlock(int fd, int blockSize){
    int currentBlockSize = checkBlockBackword(fd, blockSize);
    checkBlockForeword(fd, currentBlockSize);
}

void checkBlockForeword(int fd, int blockSize){
    int currentBlockSize = blockSize;
    ssize_t rsize = 0;
    ssize_t wsize = 0;
    int preContent = 0;
    int preBlockSize = 0;
    int totalBlockSize = 0;

    rsize = read(fd, &preBlockSize, 1); // get previous block size
    if(rsize == -1){
        perror("checkBlockForeword preBlockSize read ERROR!");
        exit(-1);
    }

    lseek(fd, -preBlockSize, SEEK_CUR);
    rsize = read(fd, &preContent, 1); // get previous content
    if(rsize == -1){
        perror("checkBlockForeword preContent read ERROR!");
        exit(-1);
    }

    if(preContent == F){
        //printf("previous block is free block!\nstart add block!\n");
        // previous block is free block
        totalBlockSize = currentBlockSize + preBlockSize;
        // write totalBlockSize at previous block
        wsize = write(fd, &totalBlockSize, 1);
        if(wsize == -1){
            perror("checkBlockForeword totalBlockSize write ERROR!");
            exit(-1);
        }
        lseek(fd, totalBlockSize - 3, SEEK_CUR);
        // write totalBlocSize at current block
        wsize = write(fd, &totalBlockSize, 1);
        if(wsize == -1){
            perror("checkBlockForeword totalBlockSize write ERROR!");
            exit(-1);
        }
        //printf("FINSHED add previous block\n");
        return;
    }
    else if(preContent == A){
        // previous block is allocated block
        //printf("previous block is allocated block.\n");
        return;
    }
    else{
        //printf("Can't found checkBlockforeword\n");
        return;
    }
}

int checkBlockBackword(int fd, int blockSize){
    int currentBlockSize = blockSize;
    int nextBlockSize = 0;
    int totalBlockSize = 0;
    ssize_t rsize = 0;
    ssize_t wsize = 0;
    int nextContent = 0;
    
    lseek(fd, currentBlockSize - 1, SEEK_CUR);
    rsize = read(fd, &nextContent, 1); // get nextContent
    if(rsize == -1){
        perror("checkBlockBackword nextContent read ERROR!");
        exit(-1);
    }

    if(nextContent == F){
        //printf("next block is free block!\nstart add block!\n");
        // next block is free block
        rsize = read(fd, &nextBlockSize, 1); // get next block size
        if(rsize == -1){
            perror("checkBlockBackword nextBlockSize read ERROR!");
            exit(-1);
        }
        totalBlockSize = currentBlockSize + nextBlockSize;
        lseek(fd, -(currentBlockSize + 1), SEEK_CUR);

        // write total block size at current block
        wsize = write(fd, &totalBlockSize, 1);
        if(wsize == -1){
            perror("checkBlockBackword totalBlockSize write ERROR!");
            exit(-1);
        }
        lseek(fd, totalBlockSize - 3, SEEK_CUR);

        // write total block size at next block
        wsize = write(fd, &totalBlockSize, 1);
        if(wsize == -1){
            perror("checkBlockBackword totalBlockSize write ERROR!");
            exit(-1);
        }
        lseek(fd, -(totalBlockSize + 1), SEEK_CUR);
        //printf("FINSHED add backword block\n");
        return totalBlockSize;
    }
    else if(nextContent == A){
        // next block is allocated block
        // change offset to check foreword block
        lseek(fd, -(currentBlockSize + 2), SEEK_CUR);
        //printf("next block is allocated block.\n");
        return currentBlockSize;
    }
    else{
        //printf("Can't found checkBlockBackword\n");
        return currentBlockSize;
    }
}
