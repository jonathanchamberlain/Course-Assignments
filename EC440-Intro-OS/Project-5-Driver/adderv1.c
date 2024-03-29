/*
 *  adder.c: Creates a read/write char device that keeps track of the 
 *  total sum since the driver was first initialized
 *
 *  Code is based on the chardev.c example from Chapter 4 of
 *  Linux Kernel Module Programming Guide by Salzman, Burian, Pomerantz
 *
 *  modifications written by Jonathan Chamberlain for EC 440
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>	/* for put_user */

/*  
 *  Prototypes - this would normally go in a .h file
 */
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "adder"	/* Dev name as it appears in /proc/devices   */
#define BUF_LEN 80		/* Max length of the message from the device */

/* 
 * Global variables are declared as static, so are global within the file. 
 */

static int Major = 200;		/* Major number assigned to our device driver
                                   Hardcoded to 200 for project 5 */
// static int Device_Open = 0;	/* Is device open?  
				/* Used to prevent multiple access to device */
static int Read_Open = 0;       /* Used to prevent mulitple read access to device */
static int Write_Open = 0;      /* Used to prevent multiple write access to device */
static char msg[BUF_LEN];	/* The msg the device will give when asked */
static char *msg_Ptr;
static int  Sum = 0;            /* The sum of all numbers since driver module 
                                    inserted to kernel */
static int Term = 0;            /* The next term in the sum */
static int isNeg = 0;           /* Flag to denote negative terms */
static int getTerm = 0;         /* Flag to denote getting term */
static int isUpdated = 1;       /* Flag to denote whether Sum needs to be 
                                     recalculated for read */

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

/*
 * This function is called when the module is loaded
 */
int init_module(void)
{
        // Register hardcoded to use Major number 200 for Project 5 grading
        int reg = register_chrdev(Major, DEVICE_NAME, &fops);

	if (reg < 0) {
	  printk(KERN_ALERT "Registering char device failed with %d\n", reg);
	  return reg;
	}

	printk(KERN_INFO "I was succesfully registered; to talk to the driver\n");
	printk(KERN_INFO "create a dev file with 'mknod /dev/%s c %d 0'.\n",DEVICE_NAME, Major);
	printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
	printk(KERN_INFO "the device file.\n");
	printk(KERN_INFO "Remove the device file and module when done.\n");

	return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void)
{
	/* 
	 * Unregister the device 
	 */
  //	int ret = unregister_chrdev(Major, DEVICE_NAME);
        unregister_chrdev(Major, DEVICE_NAME);
  //	if (ret < 0)
  //		printk(KERN_ALERT "Error in unregister_chrdev: %d\n", ret);
}

/*
 * Methods
 */

/* 
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file)
{
        /*
	if (Device_Open)
		return -EBUSY;

	Device_Open++;
        */
        int rmode = file->f_mode & 1;
        int wmode = file->f_mode & 2;

        if (rmode && Read_Open)
          printk(KERN_INFO "Only one process may read from adder at a time.\n");
          return -EBUSY;

        if (wmode && Write_Open)
          printk(KERN_INFO "Only one process may write to adder at a time.\n");
          return -EBUSY;

        if (rmode)
          Read_Open++;
        if (wmode)
          Write_Open++;
        
	try_module_get(THIS_MODULE);

	returun SUCCESS;
}

/* 
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
	// Device_Open--;		/* We're now ready for our next caller */
        int rmode = file->f_mode & 1;
        int wmode = file->f_mode & 2;

        if (rmode)
          Read_Open--;
        if (wmode)
          Write_Open--;

	/* 
	 * Decrement the usage count, or else once you opened the file, you'll
	 * never get get rid of the module. 
	 */
	module_put(THIS_MODULE);

	return 0;
}

/* 
 * Called when a process, which already opened the dev file, attempts to
 * read from it. (e.g. cat dev/adder)
 */
static ssize_t device_read(struct file *filp,	/* see include/linux/fs.h   */
			   char *buffer,	/* buffer to fill with data */
			   size_t length,	/* length of the buffer     */
			   loff_t * offset)
{
	/*
	 * Number of bytes actually written to the buffer 
	 */
	int bytes_read = 0;

        /*
         * Generate message based on Sum as of read call
         * isUpdated is used as a flag to signal EOF if Sum has not
         * changed since last call, to prevent cat from triggering an 
         * infinite loop
         */
         if (isUpdated) {
           sprintf(msg,"%d\n",Sum);
           msg_Ptr = msg;
           isUpdated = 0;
         } else {
           // flip isUpdated back to 1 so that next read call gets sum
           // ensure consistent behavior if calling cat multiple times in row
           isUpdated = 1; 
           return 0;
         }

	/*
	 * If we're at the end of the message, 
	 * return 0 signifying end of file 
	 */
	if (*msg_Ptr == 0)
		return 0;

	/* 
	 * Actually put the data into the buffer 
	 */
	while (length && *msg_Ptr) {

		/* 
		 * The buffer is in the user data segment, not the kernel 
		 * segment so "*" assignment won't work.  We have to use 
		 * put_user which copies data from the kernel data segment to
		 * the user data segment. 
		 */
		put_user(*(msg_Ptr++), buffer++);

		length--;
		bytes_read++;
	}

	/* 
	 * Most read functions return the number of bytes put into the buffer
	 */
	return bytes_read;
}

/*  
 * Called when a process writes to dev file (e.g. echo 9 > /dev/adder) 
 */
static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
        int bytes_read;
        char next;
        for (bytes_read = 0; bytes_read < len; bytes_read++) {
          next = buff[bytes_read];
          if (next == '-' && !getTerm) {
            // Negative found, not currently accumulating
            getTerm = 1;
            isNeg = 1;
          } else if (next >= '0' && next <= '9') {
             // valid digit found
             if (!getTerm) getTerm = 1;
             Term = (10 * Term) + (next - '0');
          } else if (next != '\n' && next != '\r' && next != '\t' && next != ' ' && next != '\0') {
            // non integer character found, including '-' while accumulating - trash current term
            printk(KERN_ALERT "Sorry, this operation only supports integer input\n");
            isNeg = 0;
            getTerm = 0;
            Term = 0;
            return -EINVAL;
          } else if (getTerm) {
            // accumulating and reached whitespace or string terminator (\0)
            if (isNeg) {
              Sum -= Term;
            } else {
              Sum += Term;
            }
            isUpdated = 1;
            isNeg = 0;
            getTerm = 0;
            Term = 0;
          }  // end if-else
        } // end for loop
        return bytes_read;
}
