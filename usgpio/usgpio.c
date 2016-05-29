
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <asm/gpio.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

unsigned int pin = AT91_PIN_PB0; /*Pin selection*/


ssize_t user_gpio_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
    char level = at91_get_gpio_value(pin); /* Getting pin value */
    printk(KERN_ALERT "GPIO read %d\n",level);
    level += 'A';
    copy_to_user(buf,&level,1);
    if (*ppos == 0) {
	*ppos += 1;
        return 1;
    }
    return 0;
}


ssize_t user_gpio_write(struct file * file,const char *buf, size_t count, loff_t *loft)
{
char level = 0;
char *tmp = buf+count-1;
    copy_from_user(&level,tmp,1);
    printk(KERN_ALERT "GPIO write %d\n",level);
    level &= 1;
    at91_set_gpio_value(pin, level);
    return 1;
}


int user_gpio_open(struct inode *inode, struct file *filp) {
  /* Success */
  return 0;
}

int user_gpio_release(struct inode *inode, struct file *filp) {
  /* Success */
  return 0; 
}


static const struct file_operations user_gpio_fops = { /* Operations for controling the device */
        .owner             = THIS_MODULE,
        .open		   = user_gpio_open,
        .read              = user_gpio_read,
        .write             = user_gpio_write,
        .release           = user_gpio_release
};



/* Major number */
int gpio_major = 61;


static int modinit(void)  /* Module init */
{
    gpio_direction_output(pin,1);	// out
    printk(KERN_ALERT "Loading Socrates GPIO\n");
    return register_chrdev(gpio_major, "gpio",  &user_gpio_fops);
}


static void modexit(void) /* Module exit */
{
    printk(KERN_ALERT "Unloading Socrates GPIO\n");
    unregister_chrdev(gpio_major, "gpio");
}

module_init(modinit);
module_exit(modexit); 

