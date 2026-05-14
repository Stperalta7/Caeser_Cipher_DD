/**************************************************************
* Class::  CSC-415-04 Spring 2026
* Name::  Steven Peralta
* Student ID::  911003553
* GitHub-Name::  Stperalta7
* Project:: Assignment 6 - Device Driver
*
* File::  "enc_driver.h"
*
* Description::  This is the shared header file for the 
*                encryption driver.
*
**************************************************************/


#ifndef ENC_DRIVER_H
#define ENC_DRIVER_H
#define DEVICE_NAME "endev"
#define MODE_ENCRYPT 1
#define MODE_DECRYPT 2
#define IOCTL_SET_MODE _IOW('a', 'a', int)
#define IOCTL_SET_KEY _IOW('a', 'b', int)
#define BUFFER_SIZE 1024

#endif


