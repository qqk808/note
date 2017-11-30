#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>

#include <linux/device.h>

#include <linux/ioctl.h>  
#include <linux/errno.h>  
#include <linux/types.h>  
#include <linux/fcntl.h>  
#include <linux/cdev.h>  
#include <linux/version.h>  
#include <linux/vmalloc.h>  
#include <linux/ctype.h>  
#include <linux/pagemap.h> 

#define USE_IMMEDIATE
#define HELLO_MAGIC 'k'
#define SPI_CMD1    _IO(HELLO_MAGIC,0x1a)
#define HELLO_CMD2    _IO(HELLO_MAGIC,0x1b)

typedef struct IOCT_BAG
{
	unsigned int cmd;
	unsigned int send_len;
	unsigned int rev_len;
	unsigned char data[4096];
}*IOCT_BAG_P;
struct IOCT_BAG iobag;



#define DEVICE_NAME "chardev"
static struct cdev chr_dev;
static dev_t ndev;
static struct class *chardev_class;
#define CHAR_LEN 1000
static char char_buff[CHAR_LEN]; 
//static int char_len=10;


static int chr_open(struct inode *nd,struct file *filp)
{
	//struct demo_dev *dev;
	int major =MAJOR(nd->i_rdev);
	int minor =MINOR(nd->i_rdev);
	printk(KERN_EMERG"char_open,major=%d,minor=%d\n",major,minor);
	printk("char_open:the inode i_cdev is %d\n",(unsigned int )nd->i_cdev); 
	printk("char_open:the inode chr_dev is %\n",(unsigned int )&chr_dev); 
    //dev = container_of(inode->i_cdev, struct n_dev, cdev);  
    //filp->private_data = dev;   
	return 0;
}
static ssize_t chr_read(struct file *f,char __user *u,size_t sz,loff_t *off)
{
	//int retlen; 
	printk(KERN_EMERG"In the chr_read() size:%d shift:%d!\n",(int)sz,(int)*off);
	if(sz+*off>=CHAR_LEN)
		return -EFAULT;
	if(copy_to_user( (char *)u, char_buff+*off, sz))
				return -EFAULT;
	return sz;
}

ssize_t  chr_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)  
{  
    ssize_t retval = -ENOMEM; 
	printk(KERN_EMERG"In the chr_write(1) size:%d shift:%d!\n",(int)count,(int)*f_pos); 
    if(count+*f_pos>=CHAR_LEN)
    	retval = -EFAULT; 
    if (copy_from_user(char_buff+*f_pos, buf, count)) {  
        retval = -EFAULT;  
        goto out;     
    }  

    retval = count;  
out:  
    return retval;  
}  
static long chr_ioctl( struct file *file, unsigned int cmd, unsigned long arg)
{    int temp = 0;
	//printk("char_ioctl:\n");
    switch(cmd)
    {
      case SPI_CMD1:
           {
        	   if (copy_from_user(&iobag, (unsigned char *)arg, sizeof(struct IOCT_BAG)))
        	   {
        		   return -EFAULT;
        	   }
        	  // printk("cmd is %d\n",iobag.cmd);
        	  // printk("sendlen is %d\n",iobag.send_len);
        	  // printk("revlen is %d\n",iobag.rev_len);
        	   int i;
        	   for(i=0;i<iobag.rev_len;i++)
        	   {
        		   	  iobag.data[i]=10+i;
        	   }
        	     if(copy_to_user( (unsigned char *)arg, &iobag, sizeof(struct IOCT_BAG)))
				return -EFAULT;
            break;
           }
      case HELLO_CMD2:
            {
            temp = 2;
            if(copy_to_user( (int *)arg, &temp, sizeof(int))) 
				return -EFAULT;
            break;
            }
    }
    //printk( KERN_NOTICE"ioctl CMD%d done!\n",temp);

return 0;

}

static int chr_release(struct inode *inode, struct file *filp)  
{     
    //demo_inc--; 
	printk( KERN_NOTICE"char_release!\n");   
    return 0;  
} 
struct file_operations chr_ops =
{
	.owner=THIS_MODULE,
	.open=chr_open,
	.read=chr_read,
	.write=chr_write,
	.unlocked_ioctl = chr_ioctl, 
	.release = chr_release,  
};
static int demo_init(void)
{
	int ret;
	printk(KERN_EMERG"demo_init\n");
	cdev_init(&chr_dev,&chr_ops);
	//ret = alloc_chrdev_region(&ndev,0,1,"chr_dev");
	ndev=MKDEV(248,0);
	ret = register_chrdev_region(ndev,1,DEVICE_NAME);
	if(ret<0)
	{
		printk("register chrdev region err!\n");
		return ret;
	}
		
	
	printk(KERN_EMERG"demo_init(1):major=%d,minor=%d\n",MAJOR(ndev),MINOR(ndev));
	ret= cdev_add(&chr_dev,ndev,1);
	if(ret<0)
		return ret;	
	chardev_class=class_create(THIS_MODULE,DEVICE_NAME);
		if(IS_ERR(chardev_class))
		{
			printk("Err class crt\n");
			return -1;
		}
		device_create(chardev_class,NULL,ndev,NULL,"%s",DEVICE_NAME);
	return 0;
}

static void  demo_exit(void)
{
	device_destroy(chardev_class,ndev);
	class_destroy(chardev_class);
	printk(KERN_EMERG"Removing chr_dev module ...\n");
	cdev_del(&chr_dev);
	unregister_chrdev_region(ndev,1);
}
module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("QQK@808");
MODULE_DESCRIPTION("A char device drive as an example");
