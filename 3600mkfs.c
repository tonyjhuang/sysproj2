/*
 * CS3600, Spring 2014
 * Project 2 Starter Code
 * (c) 2013 Alan Mislove
 *
 * This program is intended to format your disk file, and should be executed
 * BEFORE any attempt is made to mount your file system.  It will not, however
 * be called before every mount (you will call it manually when you format 
 * your disk file).
 */

#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "3600fs.h"
#include "disk.h"
#include "vcb.h"
#include "dirent.h"
#include "fatent.h"

void printvcb(vcb myvcb);
vcb *writevcb();
void writede(vcb *myvcb);
void writefat(vcb *myvcb);

void myformat(int size) {
  // Do not touch or move this function
  dcreate_connect();

  // Write FAT overhead.
  vcb *myvcb = writevcb(size);
  writede(myvcb);
  writefat(myvcb);
  
  // Create blank buffer for DataBlocks.
  char *tmp = (char *) malloc(BLOCKSIZE);
  memset(tmp, 0, BLOCKSIZE);

  // Write blank buffers to disk.
  for (int i=myvcb->db_start; i<size; i++) 
    if (dwrite(i, tmp) < 0) 
      perror("Error while writing to disk");
  free(myvcb);
  dunconnect();
}

// Store 128 invalid fatent structs in buffer, write to disk.
void writefat(vcb *myvcb) {
  fatent myfatent;
  myfatent.used = 0;

  char tmp[BLOCKSIZE];
  for(int i=0; i<BLOCKSIZE; i+=4) {
    memcpy(tmp+i, &myfatent, sizeof(myfatent));
  }
  printf("Writing FAT blocks to disk..\n");
  for(int i=myvcb->fat_start; i<myvcb->db_start; i++) {
    dwrite(i, tmp);
  }
}

// Write empty DirectoryEntries out to disk.
// TODO: Optimize so a single dirent doesn't take an entire block.
void writede(vcb *myvcb) {
  dirent mydirent;
  mydirent.valid = 0;
  char tmp[BLOCKSIZE];
  memset(tmp, 0, BLOCKSIZE);
  memcpy(tmp, &mydirent, sizeof(mydirent));

  printf("Writing empty dirent's to disk..\n");
  for(int i=1; i<myvcb->de_length+1; i++) {
    dwrite(i, tmp);
  }
}

// Write the VCB out to block 0.
vcb *writevcb(int size) {
  // Get time.
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);

  // Create vcb and write to disk.
  vcb *myvcb = (vcb *)  malloc(sizeof(vcb));
  myvcb->blocksize = BLOCKSIZE;
  myvcb->magic     = maaaaagic;
  myvcb->de_start  = 1;
  myvcb->de_length = 100;

  int remaining_size = size - 101;
  int db_length  = 128 * remaining_size / 129;
  int fat_length = remaining_size - db_length;

  myvcb->fat_start  = 101;
  myvcb->fat_length = fat_length;
  myvcb->db_start   = myvcb->fat_start + myvcb->fat_length;

  myvcb->consistent = 1;

  myvcb->user  = geteuid();
  myvcb->group = getegid();
  //TODO: myvcb->mode = ???;
  
  myvcb->access_time = now;
  myvcb->modify_time = now;
  myvcb->create_time = now;

  // Uncomment to debug vcb.
  //printvcb(*myvcb);

  char tmp[BLOCKSIZE];
  memset(tmp, 0, BLOCKSIZE);
  memcpy(tmp, myvcb, sizeof(vcb));
  
  printf("Writing vcb to block 0..\n");
  dwrite(0, tmp);
  return myvcb;
}

// Print out vcb to stdout.
void printvcb(vcb myvcb) {
  printf("Printing vcb..\nblocksize: %d\nmagic: %d\nde_start: %d\nde_length: %d\nfat_start: %d\nfat_length: %d\ndb_start: %d\nuser id: %d\ngroup id: %d\nmode: %d\n", myvcb.blocksize, myvcb.magic, myvcb.de_start, 
	 myvcb.de_length, myvcb.fat_start, myvcb.fat_length, 
	 myvcb.db_start, myvcb.user, myvcb.group, myvcb.mode);
}

int main(int argc, char** argv) {
  // Do not touch this function
  if (argc != 2) {
    printf("Invalid number of arguments \n");
    printf("usage: %s diskSizeInBlockSize\n", argv[0]);
    return 1;
  }

  unsigned long size = atoi(argv[1]);
  printf("Formatting the disk with size %lu \n", size);
  myformat(size);
}
