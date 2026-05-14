/**************************************************************
* Class::  CSC-415-04 Spring 2026
* Name::  Steven Peralta
* Student ID::  911003553
* GitHub-Name::  Stperalta7
* Project:: Assignment 6 - Device Driver
*
* File::  "encrypt_driver.c"
*
* Description::  This is the kernel file for the encryption
*                driver.
*
**************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#include "enc_driver.h"

// GLOBAL VARIABLES:
#define BUFFER_SIZE 1024

static char dev_buff[BUFFER_SIZE]; //Stores enc/dec text
static int major_num;  // driver interal ID
static int curr_mode = MODE_ENCRYPT; // Mode tracker
static int enc_key = 3; // Stores cypher shift amt
static size_t dat_size = 0; // size of valid data

// FUNCTION PROTOTYPES:
void c_cipher(char *text, int key, int enc);
static int device_open(struct inode *inodep, struct file *filep);
static int device_release(struct inode *inodep, struct file *filep);
static ssize_t device_write(struct file *filep, const char *buff, size_t len, loff_t *offset);
static ssize_t device_read(struct file *filep, char *buff, size_t len, loff_t *offset);
static long device_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);
static int __init endevInit( void );
static void __exit endevExit( void );

// NOTE: in this struct def, '=' is included so the system automatically initializes it
static struct file_operations file_ops =
{
  .open = device_open,
  .read = device_read,
  .write = device_write,
  .unlocked_ioctl = device_ioctl,
  .release = device_release,
};



void c_cipher(char *text, int key, int enc)
{
  int i; //Declare index outside of for-loop for better tracking

  for(i = 0; text[i] != '\0'; i++)
  {
    char c = text[i];
    if ( c >= 'a' && c <= 'z') // LOWERCASE LETTERS
    {
      if ( enc )
      {  c = (( c - 'a' + key ) % 26) + 'a';  } 
      // Mod by 26 b/c english alphabet
      else
      {  c = (( c - 'a' - key + 26) % 26) + 'a';  }
    }
    else if ( c >= 'A' && c <= 'Z') // UPPERCASE LETTERS
    {
      if ( enc )
      {  c = (( c - 'A' + key ) % 26 ) + 'A';  }
      else
      {  c = (( c - 'A' - key + 26 ) % 26 ) + 'A';  }
    }

    text[i] = c;
  }// end for loop

}// end function



static int device_open(struct inode *inodep, struct file *filep)
{
  printk(KERN_INFO "endev: Device opened.\n");
  return 0;

}// end function


static int device_release(struct inode *inodep, struct file *filep)
{
  printk(KERN_INFO "endev: Device closed.\n");
  return 0;

}// end function


static ssize_t device_write(struct file *filep, const char *buff, size_t len, loff_t *offset)
{
  printk(KERN_INFO "endev: Writing data...\n");

  if ( len > BUFFER_SIZE - 1)
  {  len = BUFFER_SIZE - 1;  } // Handles buffer overflow

  if (copy_from_user(dev_buff, buff, len))
  {  return -EFAULT;  } // error handling, bad mem access
  dat_size = len; // global var update
  dev_buff[len] = '\0'; // null terminate buffer for safety

  if (curr_mode == MODE_ENCRYPT)
  {  c_cipher(dev_buff, enc_key, 1);  }
  else
  {  c_cipher(dev_buff, enc_key, 0);  }

  printk(KERN_INFO "endev: Text Processed : %s\n", dev_buff);

  return len;
}// end function



static ssize_t device_read(struct file *filep, char *buff, size_t len, loff_t *offset)
{
  size_t to_read; // bytes to be read

  if (*offset >= dat_size )
  {  return 0; }

  printk(KERN_INFO "endev: Reading data...\n");

  to_read = min(len, dat_size - *offset);
  if (copy_to_user(buff, dev_buff + *offset, to_read))
  {  return -EFAULT; }

  *offset += to_read;
  return to_read;
}// end function




static long device_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
  if (arg < 0 || arg > 25)
  {  return -EINVAL;  } // error handles imvalid keys

  switch( cmd )
  {
    case IOCTL_SET_MODE:
      curr_mode = (int)arg;
      printk(KERN_INFO "endev: Mode has changed.\n");
      break;

    case IOCTL_SET_KEY:
      enc_key = (int)arg;
      printk(KERN_INFO "endev: Key has changed to: %d\n", enc_key);
      break;

    default:
      return -EINVAL; // else return invalid
  }

  return 0;
}// end function




static int __init endevInit( void )
{
  printk(KERN_INFO "endev: Initializing driver...\n");

  major_num = register_chrdev(0, "encrypt_driver", &file_ops);
  if (major_num < 0)
  {
    printk(KERN_ALERT "endev: Registration Failed.\n");
    return major_num;
  }

  printk(KERN_INFO "endev: Registered properly.\n");
  printk(KERN_INFO "Major number = %d\n", major_num);

  return 0;
}//end function



static void __exit endevExit( void )
{
  unregister_chrdev(major_num, DEVICE_NAME);
  printk(KERN_INFO "endev: Driver has been unloaded.\n");

}//end function


// BOILERPLATE & METADATA


module_init(endevInit);
module_exit(endevExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Steven Peralta");
MODULE_DESCRIPTION("Encryption device driver");
MODULE_VERSION("1.0");









