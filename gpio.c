/* Device driver to blink the led which is connected with GPIO 60 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#define GPIO_LED 60

static dev_t dev;
static struct cdev cdev;
static struct class *cl;

/* Module permissions */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("DineshkumarSubramaniam");
MODULE_DESCRIPTION("Device driver for Blinking the LED in GPIO_60");


/* Open System Call */
/*
   struct inode *node -> describes a file-system object such as a file or a directory
   struct file *fun_file -> The file structure represents an open file
   */
static int my_open(struct inode *inode, struct file *file)
{
	return 0;
}


/* Close System Call */
/*
   struct inode *inode -> describes a file-system object such as a file or a directory
   struct file *file -> The file structure represents an open file
   */
static int my_release(struct inode *inode, struct file *file)
{
	return 0;
}


/*
   struct file *file -> file structure represents an open file
   const char *buf -> form of derefernce from user space
   size_t -> like an size of operator
   loff_t -> driver can read this value if it needs to know the current position in the file
   */
static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *pos)
{
	int i;
	/*Loop to blink the led for 10 times  */
	for (i = 0; i < 10; i++)
	{
		gpio_set_value(GPIO_LED, 1);
		msleep(2000);
		gpio_set_value(GPIO_LED, 0);
		msleep(2000);
	}

	return count;
}

/* Structure that defines the operations that the driver provides */
static struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_release,
	.write = my_write,
};

static int __init my_init(void)
{

	int ret;

	/* allocates a character device region and stores the starting device number and the number of devices in dev */
	ret = alloc_chrdev_region(&dev, 0, 1, "gpio_60");
	if (ret < 0) {
		pr_err("Failed to allocate char dev region\n");
		return ret;
	}

	/* Initializes the character device cdev with the file operations structure */
	cdev_init(&cdev, &my_fops);

	/* Notify the kernel abt the new device */
	cdev_add(&cdev, dev, 1);

        
	/* Create a device class named "mydev" and create a device node associated with the driver using device_create(). The device node is given a name of "gpio_60" */

        /* ithu vanthu new device class ah create panna use aavuthu, */
	/* Group devices with similar characteristics */
	/* owner, name of the class (describe the type of device) */
	cl = class_create(THIS_MODULE, "mydev");
	
	/* individual devices within a device class */
	/* Class of device , parent for device file,  major and minor number define panna use aakum, Device data ,Device name */
	device_create(cl, NULL, dev, NULL, "gpio_60");
        
	/* requesting for gpio pin */
	if (gpio_request(GPIO_LED, "led") < 0) 
	{
		pr_err("Failed to request GPIO %d\n", GPIO_LED);
		return -1;
	}
        
	/* Setting the direction for gpio */
	/* Pin number, Initial value */
	gpio_direction_output(GPIO_LED, 0);

	pr_info("Driver loaded successfully\n");

	return 0;
}

static void __exit my_exit(void)
{
	/* Setting the value to 0 before exiting */
	gpio_set_value(GPIO_LED, 0);

	/* free the used pin */
	gpio_free(GPIO_LED);

        /* Destroys the device file associated with the driver */
	device_destroy(cl, dev);
	
	/* Destroys the device class created for the driver */
	class_destroy(cl);
       
	/* Deletes the character device structure associated with the driver */
	cdev_del(&cdev);
	 
	/* Un-register the character device driver(Major and minor number) */
	unregister_chrdev_region(dev, 1);

	pr_info("Driver unloaded successfully\n");
}

module_init(my_init);
module_exit(my_exit);
