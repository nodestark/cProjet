#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/slab.h>

static int major;

char *msg = "nodestark.com\n";
static int seek = 0;

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int mychardev_open(struct inode *inode, struct file *file) {

	if (seek > 0)
		return -EBUSY;

	printk("*****Some body is opening me at major %d  minor %d*****\n", imajor(inode), MINOR(inode->i_rdev));

	return 0;
}

int mychardev_release(struct inode *inode, struct file *file) {

	seek = 0;

	printk("MYCHARDEV: Device close\n");
	return 0;
}

ssize_t mychardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset) {

	int blksize = MIN(16, strlen(msg) - seek);

	if (blksize <= 0)
		return 0;

	copy_to_user(buf, msg + seek, blksize);

	seek += blksize;

	return blksize;
}

ssize_t mychardev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset) {

	int blksize = MIN(3, count);

	char *aux = (char*) kmalloc(blksize + 1, GFP_KERNEL);
//	memset(aux, 0, blksize + 1);

	copy_from_user(aux, buf, blksize);

	printk("%s", aux);

	kfree(aux);

	return blksize;
}

static struct file_operations fops = {
		.owner = THIS_MODULE,
		.read = mychardev_read,
		.write = mychardev_write,
		.open = mychardev_open,
		.release = mychardev_release };

static int __init esp8266_init_module(void) {

	printk(KERN_INFO "Hello world!\n");

	major = register_chrdev(0, "eva_dts", &fops);

//	cat /proc/devices
//	242 eva_dts
//	mknod /dev/eva_dts c 242 0

	printk("Yhaoo, now I have a major %d", major);

	return 0;
}

void __exit esp8266_exit_module(void){

	unregister_chrdev(major, "eva_dts");

    printk(KERN_INFO "Cleaning up module.\n");
}

module_init(esp8266_init_module);
module_exit(esp8266_exit_module);

MODULE_LICENSE("GPL");
