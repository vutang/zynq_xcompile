/*
* @Author: vutang
* @Date:   2019-06-19 18:45:25
* @Last Modified by:   vutang
* @Last Modified time: 2019-06-19 20:00:42
*/
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <asm/io.h>

#define IO_MEMBASE 0x22000000
#define IO_MEMSIZE 0x100000 /*1M*/

/* character device structures */  
static dev_t mem_ioremap_dev;  
static struct cdev mem_ioremap_cdev; 
static struct class *mem_ioremap_cl;
void __iomem	*mem_ioremap_base;

/* the file operations, i.e. all character device methods */  
static struct file_operations mem_ioremap_fops = {  
	// .open = mem_ioremap_open,  
	// .release = mem_ioremap_release,  
	// .mmap = mem_ioremap_mmap,  
	// .owner = THIS_MODULE,  
};  

static int __init mem_ioremap_init_kernel(void) 
{
	int ret;
	struct device *dev;

	printk("mem_ioremap_init_kernel\n");

	/* get the major number of the character device */  
	if ((ret = alloc_chrdev_region(&mem_ioremap_dev, 0, 1, "mem_ioremap")) < 0) {  
		printk(KERN_ERR "could not allocate major number for mem_ioremap\n");  
		goto out;  
	}

	/*Define at linux/device.h, register a class of device driver*/
	if ((mem_ioremap_cl = class_create(THIS_MODULE, "sfp_chardrv")) == NULL) {
		printk(KERN_ERR "class_create fail\n");
		goto out_alloc_region;
	}

	/* initialize the device structure and register the device with the kernel */  
	cdev_init(&mem_ioremap_cdev, &mem_ioremap_fops);  
	if ((ret = cdev_add(&mem_ioremap_cdev, mem_ioremap_dev, 1)) < 0) {  
		printk(KERN_ERR "could not allocate chrdev for mmap\n");  
		goto out_class_create;  
	} 

	dev = device_create(mem_ioremap_cl, NULL, mem_ioremap_dev, \
		NULL, "mem-ioremap");
	if (dev == NULL) {
		printk(KERN_ERR "device_create fail\n");
		goto out_cdev_add;
	}

	mem_ioremap_base = ioremap(IO_MEMBASE, IO_MEMSIZE);
	if (mem_ioremap_base == NULL) {
		printk(KERN_ERR "ioremap fail\n");
		goto out_device_create;
	}

	return ret;
out_device_create: 
	device_destroy(mem_ioremap_cl, mem_ioremap_dev);
out_cdev_add:
	cdev_del(&mem_ioremap_cdev);
out_class_create:
	class_destroy(mem_ioremap_cl);
out_alloc_region:
	unregister_chrdev_region(mem_ioremap_dev, 1);
out:
	return ret;
}

static void __exit mem_ioremap_exit_kernel(void) 
{
	printk("mem_ioremap_exit_kernel\n");
	device_destroy(mem_ioremap_cl, mem_ioremap_dev);
	cdev_del(&mem_ioremap_cdev);
	class_destroy(mem_ioremap_cl);
	unregister_chrdev_region(mem_ioremap_dev, 1);

	return;
}

module_init(mem_ioremap_init_kernel);  
module_exit(mem_ioremap_exit_kernel); 

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Vu Tang");
MODULE_DESCRIPTION("Kernel module for memory ioremap");
MODULE_VERSION("0.1");