/* exemple de creation de device caractere MAJOR number */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/device.h>
#include <linux/cdev.h>

MODULE_LICENSE("Dual BSD/GPL");

static dev_t attend_first; // Global var. for the attend_first device number 
static struct cdev attend_dev; // Global var. for the character device structure
static struct class *attend_class; // Global var. for the device class
static int attend_major = 254; /* MAJOR Number */
static DECLARE_WAIT_QUEUE_HEAD(wq);
static int vav = 0;

static ssize_t attend_read (struct file *fp, char __user *buf, size_t nbc, loff_t *pos)
{
int minor;
   // minor = iminor(fp->f_dentry->d_inode);
   minor = MINOR(attend_first);
   printk(KERN_DEBUG "read sur attend%i !\n", minor);
   printk(KERN_DEBUG "processus %i (%s) bloque !\n",current->pid,current->comm);
   wait_event_interruptible(wq, vav != 0);
   vav = 0;
   printk(KERN_DEBUG "Debloquage %i (%s)\n", current->pid,current->comm);
   return 0; /* idem EOF */
}

static ssize_t attend_write (struct file *fp, const char __user *buf, size_t nbc, loff_t *pos)
{
int minor;
   // minor = iminor(fp->f_dentry->d_inode);
   minor = MINOR(attend_first);
   printk(KERN_DEBUG "write sur attend%i !\n", minor);
   printk(KERN_DEBUG "Processus %i (%s) debloque le lecteur !\n",
         current->pid, current->comm);
   vav = 1;
   wake_up_interruptible(&wq);
   return nbc; /* retourne le nb de car ecrits */
}

static struct file_operations attend_fops = {
	.owner = THIS_MODULE,
	.read = attend_read,
	.write = attend_write,
};

static int __init attend_init(void)
{
int res;
   // res = register_chrdev(attend_major,  "attend", &attend_fops);
   attend_first = MKDEV(attend_major, 0);
   res = alloc_chrdev_region(&attend_first, 0, 1, "attend");
   if (res < 0) return res;
   if ((attend_class = class_create(THIS_MODULE, "attend")) == NULL) {
    unregister_chrdev_region(attend_first, 1);
    return -1;
   }
   if (device_create(attend_class, NULL, attend_first, NULL, "attend")==NULL) {
    class_destroy(attend_class);
    unregister_chrdev_region(attend_first, 1);
    return -1;
   }
   cdev_init(&attend_dev, &attend_fops);
   if (cdev_add(&attend_dev, attend_first, 1) == -1) {
    device_destroy(attend_class, attend_first);
    class_destroy(attend_class);
    unregister_chrdev_region(attend_first, 1);
    return -1;
   }
   /* c'est bon */
   if (attend_major == 0) attend_major = MAJOR(attend_first);
   printk(KERN_INFO "attend : Major %d !",attend_major);
   return 0;
}

void attend_start(void)
{
   attend_init();
}

#ifdef MODULE
static void __exit attend_exit(void)
{
  // unregister_chrdev(attend_major, "attend");
  cdev_del(&attend_dev);
  device_destroy(attend_class, attend_first);
  class_destroy(attend_class);
  unregister_chrdev_region(attend_first, 1);
}
module_exit(attend_exit);
#endif

module_init(attend_init);


