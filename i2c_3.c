#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/slab.h>
#include<linux/i2c.h>
#include<linux/kernel.h>
#include<linux/delay.h>

#define DRIVER_NAME " LCD "
#define DRIVER_CLASS "lcd_class"

#define ENABLE 0x04
#define LCD_RS (1<<0)
#define LCD_RW 0x00
//
/* Defines for device identification */
  #define I2C_BUS_AVAILABLE 2 
  #define SLAVE_DEVICE_NAME "LCD_DISPLAY" //Device and Driver name
  #define LCD_SLAVE_ADDRESS 0x27 // lcd i2c address

static dev_t dev=0;
static struct class *dev_class;


uint8_t err;

static int lcd_driver_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int lcd_driver_remove(struct i2c_client *client);


static struct i2c_adapter *lcd_i2c_adapter=NULL;
static struct i2c_client *lcd_i2c_client=NULL;

/* LCD ID Table */

static const struct i2c_device_id lcd_driver_id[]={
        {SLAVE_DEVICE_NAME,0},
        { }
};

MODULE_DEVICE_TABLE(i2c,lcd_driver_id);


/* I2C ADD DRIVER */
static struct i2c_driver lcd_driver={
        .driver={
                .name  = SLAVE_DEVICE_NAME,
                .owner = THIS_MODULE,
        },
        .probe    = lcd_driver_probe,
        .remove   = lcd_driver_remove,
        .id_table = lcd_driver_id,
};

/* LCD I2C board Info */
static struct i2c_board_info lcd_i2c_board_info=
{
        I2C_BOARD_INFO(SLAVE_DEVICE_NAME,LCD_SLAVE_ADDRESS)
};


		



/* LCD WRITE FUNCTION */
static int I2C_write(unsigned char *data_t,unsigned int len)
{
	int ret;
	ret=i2c_master_send(lcd_i2c_client,data_t,len);
	return ret;
}



// STRING FUNCTION DEFINATION 

//void lcd_send_string(char *str)
/*static void lcd_send_string(struct i2c_client *client, const char *str)
{
	while(*str)
		lcd_send_data(client,*str++);
	printk(KERN_INFO"ENTER IN THE LCD STRING FUNCTION\n");
}*/


// COMMAND FUNCTION 
void lcd_send_cmd(char cmd)
{
	int ret;
	char data_u,data_l;
	uint8_t data_t[4];
	data_u=(cmd&0xf0);
	data_l=((cmd<<4)&0xf0);

	data_t[0]=data_u | 0x0c;  //BL=1,EN=1,R/W=0,RS=0
	data_t[1]=data_u | 0x08;  //BL=1,EN=0,R/W=0,RS=0
	data_t[2]=data_l | 0x0c;  //BL=1,EN=1,R/W=0,RS=0
	data_t[3]=data_l | 0x08;  //BL=1,EN=0,R/W=0,RS=0

	ret=I2C_write(data_t,4);

	//err= i2c_master_send( LCD_SLAVE_ADDRESS,data_t,4);
	printk(KERN_INFO"ENTER IN THE LCD_COMMAND FUNCTION\n");
}

// SEND FUNCTION 

void lcd_send_data(char data)

{
	int ret;
	char data_u,data_l;
	uint8_t data_t[4];

	data_u=(data&0xf0);
	data_l=((data<<4)&0xf0);

	data_t[0]=data_u | LCD_RS; //BL=1,EN=1,R/W=0,RS=1
	data_t[1]=data_u | LCD_RS;  //BL=1,EN=0,R/W=0,RS=1
        data_t[2]=data_l | LCD_RS;  //BL=1,EN=1,R/W=0,RS=1
        data_t[3]=data_l | LCD_RS;  //BL=1,EN=0,R/W=0,RS=1

	ret=I2C_write(data_t,4);

//	 err= i2c_master_send( LCD_SLAVE_ADDRESS,data_t,4);
	 printk(KERN_INFO" ENTER IN THE LCD SEND DATA FUNCTION\n");
}

// CLEAR FUNCTION 
void lcd_clear(void)
{
	lcd_send_cmd(0x01);
//	sleep(5000);
}

// CURSOR POSITION FUNCTION 
void lcd_put_cur(int row,int col)
{
	switch(row)
	{
		case 0:
			col |=0x80;
			break;
		case 1:
			col |=0xc0;
			break;
	}
	lcd_send_cmd(col);
}


int lcd_display_init(void)
{
// LCD DISPLAY INITIALIZATION

// 4Bit Initialization 

	msleep(40);      // wait for ->40ms    
	lcd_send_cmd(0x03);
	msleep(5);       // wait for ->41ms
	lcd_send_cmd(0x03);
	msleep(1);        // wait for 100micro sec
	lcd_send_cmd(0x03);
	msleep(5);
	lcd_send_cmd(0x20); // 4bit mode
	msleep(10000);

// Display Initialization 

	lcd_send_cmd(0x28); // Function set -->DL=0(4bit mode) ,N=1(2line display),F=0(5x8 characters)
	msleep(1000);
	lcd_send_cmd(0x08); // Disply ON/OFF control D=0,C=0,B=0 (Display OFF)
	msleep(1000);
	lcd_send_cmd(0x01); // Clear Display
	msleep(1000);
	lcd_send_cmd(0x06); // Entry Mode set I/D=1 (Increment cursor) and S=0 (no shift)
	msleep(1000);
	lcd_send_cmd(0x0c); // Display ON/OFF control --> D=1,C&B =0 (Cursor and Blink ,last two bits)
	msleep(1000);

	printk(KERN_INFO" LCD INITIALIZATION SUCCESSFULLY\n");

	return 0;
}

static void lcd_string(char *str, int len)
{
	unsigned int i;

	for(i=0;i<len-1;i++)
	{
		lcd_send_data(str[i]);
		printk(KERN_INFO "new string is %c\n",str[i]);
	}
	printk(KERN_INFO"lcd string ..\n");
}


/* INVOKE THE DEVICE USING PROBE FUNCTION */
static int lcd_driver_probe( struct i2c_client *client,const struct i2c_device_id *id)
{
	int len;
	char str[] = "world";
	printk(KERN_INFO"probe is invoked...\n");
	printk(KERN_INFO" LCD Driver Initialization\n ");
	lcd_display_init();
	len =sizeof(str);
	printk(KERN_INFO " the string size %d\n",len);
	printk(KERN_INFO "string name %s\n",str);
	lcd_string(str,len);
	return 0;
}


/* LCD Removed */
static int lcd_driver_remove(struct i2c_client *client)
{
	printk(KERN_INFO"LCD driver removed..\n");
	return 0;
}


 // Module initialization function
 static int __init i2c_driver_init(void)
{

        int ret = -1;

        //Allocating major number

        if((alloc_chrdev_region(&dev,0,1,"i2c_client_driver"))<0)

        {
                printk(KERN_INFO "cannot allocate the major number..\n");
                return -1;
        }
        printk(KERN_INFO"major =%d minor =%d..\n",MAJOR(dev),MINOR(dev));

        //creating struct class
        if((dev_class = class_create(THIS_MODULE,"i2c_client_class"))==NULL)
        {
                printk(KERN_INFO "cannot create the struct class..\n");

        }
        /*creating device */

        if((device_create(dev_class,NULL,dev,NULL,"i2c_client_dev")) ==NULL)
        {
                printk(KERN_INFO "cannot create the device..\n");}



        lcd_i2c_adapter=i2c_get_adapter(I2C_BUS_AVAILABLE);

        if(lcd_i2c_adapter !=NULL)
        {

        lcd_i2c_client = i2c_new_client_device(lcd_i2c_adapter, &lcd_i2c_board_info);

        if(lcd_i2c_client !=NULL)
        {
                i2c_add_driver(&lcd_driver);
                ret = 0;
        }
        i2c_put_adapter(lcd_i2c_adapter);
        }
	pr_info("Driver added\n");
        return ret;
}


// Module exit function
 static void __exit i2c_driver_exit(void)
{
        i2c_unregister_device(lcd_i2c_client);
        i2c_del_driver(&lcd_driver);
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        unregister_chrdev_region(dev,1);
        pr_info("Driver Removed!!!\n");

}
module_init(i2c_driver_init);
module_exit(i2c_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("I2C LCD Driver");


                                                           
                                                             


