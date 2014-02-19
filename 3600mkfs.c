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
#include <sys/types.h>
#include <sys/stat.h>

#include "3600fs.h"
#include "disk.h"
#include "vcb.h"
#include "dirent.h"
#include "fatent.h"

void printvcb(vcb myvcb);

void myformat(int size) {
  // Do not touch or move this function
  dcreate_connect();

  /* 3600: FILL IN CODE HERE.  YOU SHOULD INITIALIZE ANY ON-DISK
           STRUCTURES TO THEIR INITIAL VALUE, AS YOU ARE FORMATTING
           A BLANK DISK.  YOUR DISK SHOULD BE size BLOCKS IN SIZE. */

  /* 3600: AN EXAMPLE OF READING/WRITING TO THE DISK IS BELOW - YOU'LL
           WANT TO REPLACE THE CODE BELOW WITH SOMETHING MEANINGFUL. */

  // first, create a zero-ed out array of memory  
  char *tmp = (char *) malloc(BLOCKSIZE);
  memset(tmp, 0, BLOCKSIZE);

  // now, write that to every block
  for (int i=0; i<size; i++) 
    if (dwrite(i, tmp) < 0) 
      perror("Error while writing to disk");

  vcb myvcb;
  myvcb.blocksize = BLOCKSIZE;
  myvcb.magic     = maaaaagic;
  myvcb.de_start  = 1;
  myvcb.de_length = 100;

  int remaining_size = size - 101;
  int db_length  = 128 * remaining_size / 129;
  int fat_length = remaining_size - db_length;

  myvcb.fat_start  = 101;
  myvcb.fat_length = fat_length;
  myvcb.db_start   = myvcb.fat_start + myvcb.fat_length;

  printvcb(myvcb);

  char vcbtmp[BLOCKSIZE];
  memset(vcbtmp, 0, BLOCKSIZE);
  memcpy(vcbtmp, &myvcb, sizeof(vcb));
  
  dwrite(0, vcbtmp);

  dunconnect();
}

void printvcb(vcb myvcb) {
  printf("Printing vcb..\nblocksize: %d\nmagic: %d\nde_start: %d\nde_length: %d\nfat_start: %d\nfat_length: %d\ndb_start: %d\n", myvcb.blocksize,
	 myvcb.magic, myvcb.de_start, myvcb.de_length, 
	 myvcb.fat_start, myvcb.fat_length, myvcb.db_start);
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
