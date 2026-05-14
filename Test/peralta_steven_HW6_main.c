/**************************************************************
* Class::  CSC-415-04 Spring 2026
* Name::  Steven Peralta
* Student ID::  911003553
* GitHub-Name::  Stperalta7
* Project:: Assignment 6 – Device Driver
*
* File::  "test_app.c"
*
* Description::  This is the user-side source file to handle 
*                functions that let the user interact with the
*                kernel module.
*
**************************************************************/


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#include "../Module/enc_driver.h"



int main(int argc, char *argv[])
{
  int fd;
  char *mode_string = argv[1];
  int key = atoi(argv[2]);
  char *text = argv[3];
  char w_buff[1024];
  char r_buff[1024];

  if (argc != 4 )
  {
    printf("Error! Please use:\n");
    printf("./test_app <encrypt/decrypt> <key> <message>\n");
    return -1;
  }

  fd = open("/dev/endev", O_RDWR);


  if (fd < 0)
  {
    printf("Error! Could not open device.\n");
    return -1;
  }

  if (strcmp(mode_string, "encrypt") == 0 )
  {
    ioctl(fd, IOCTL_SET_MODE, MODE_ENCRYPT);
    printf("Encrypted: ");
  } // ENCRYPT
  else if (strcmp(mode_string, "decrypt") == 0 )
  {
    ioctl(fd, IOCTL_SET_MODE, MODE_DECRYPT);
    printf("Decrypted: ");
  } // DECRYPT
  else
  {
    printf("Error! Invalid mode\n");
    close( fd );
    return -1;
  }

  ioctl(fd, IOCTL_SET_KEY, key);
  strncpy(w_buff, text, BUFFER_SIZE - 1);
  w_buff[BUFFER_SIZE - 1] = '\0'; // manually null-terminate for safety

  write(fd, w_buff, strlen(w_buff));
  read(fd, r_buff, sizeof(r_buff));

  printf("%s\n", r_buff);

  close(fd);
  return 0;
}// end main

