/*
FILE : stopwatch.c
AUTH : Phillips
*/

#include "stopwatch.h"

#define IOM_DEVICE_MAJOR 242            // device major number
#define IOM_DEVICE_NAME "stopwatch"     // device name
#define IOM_FND_ADDRESS 0x08000004      // FND physical address

// define functions
ssize_t device_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what);
int device_open(struct inode *minode, struct file *mfile);
int device_release(struct inode *minode, struct file *mfile);
static void stopwatch_callback(unsigned long timeout);
static void stop_callback(unsigned long timeout);

irqreturn_t inter_handler1(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler2(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler3(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler4(int irq, void* dev_id, struct pt_regs* reg);


// global variables
static int inter_major = 0, inter_minor = 0;            // interrupt majon, minor number
static int device_usage = 0;                          
static unsigned char *iom_fpga_fnd_addr;                // FND deivce address mapping
static int result;
static dev_t inter_dev;
static struct cdev inter_cdev;

int isStart;                                            // if stopwatch start -> true, default -> false
int isPause;                                            // if stopwatch executed -> false, paused -> true
int isPressed;                                          // if volume- button pressed -> true, released -> false
unsigned long msec;                                     // micro second

struct timer_list stopwatch;                            // stopwatch timer
struct timer_list stop;                                 // stop timer
char fnd_str[4];                                        // FND value

wait_queue_head_t my_queue;

static struct file_operations iom_fops={
    .owner          =       THIS_MODULE,
    .open           =       device_open,
    .write          =       device_write,
    .release        =       device_release,
};

/* 
   +----------------------------------------------------------------------------------------+
   | Method    : inter_handler1                                                             |
   | Returns   : IRQ_HANDLED                                                                |
   | parameter : irq, dev_id, reg                                                           |
   | perform   : Interrupt handler for "home" button                                        |
   |             If you press "home" button, stopwatch timer will be add in timer list.     |
   +----------------------------------------------------------------------------------------+
*/
irqreturn_t inter_handler1(int irq, void* dev_id, struct pt_regs* reg) {
    
    if(isStart == false){                                       // if you want to start stopwatch
        printk(KERN_ALERT "Stopwatch start!\n");
        isStart = true;
        stopwatch.expires = jiffies + (msec * HZ / 100);
        stopwatch.data = NULL;
        stopwatch.function = stopwatch_callback;
        add_timer(&stopwatch);                                  // add stopwatch timer to timer list
    }
    else{
        printk(KERN_ALERT "Stopwatch already start!\n");
    }
    
    return IRQ_HANDLED;
}

/* 
   +----------------------------------------------------------------------------------------+
   | Method    : inter_handler2                                                             |
   | Returns   : IRQ_HANDLED                                                                |
   | parameter : irq, dev_id, reg                                                           |
   | perform   : Interrupt handler for "back" button                                        |
   |             If you press "back" button, stopwatch timer will be paused.                |
   +----------------------------------------------------------------------------------------+
*/
irqreturn_t inter_handler2(int irq, void* dev_id, struct pt_regs* reg) {
    if(isStart == true){                                        // if stopwatch is running
        if(!isPause){                                           // if you want stopwatch to stop
            printk(KERN_ALERT "Stopwatch pause!\n");
            isPause = true;
            msec = stopwatch.expires - jiffies;
            del_timer(&stopwatch);
        }
        else{                                                   // if you want to run stopwatch again,
            printk(KERN_ALERT "Stopwatch goes on!\n");
            isPause = false;
            stopwatch.expires = jiffies + (msec * HZ / 100);
            stopwatch.data = NULL;
            stopwatch.function = stopwatch_callback;
            add_timer(&stopwatch);                              // add stopwatch timer to timer list       
        }
    }
    else{
        printk(KERN_ALERT "Stopwatch is not started!\n");
    }
    return IRQ_HANDLED;
}

/* 
   +----------------------------------------------------------------------------------------+
   | Method    : inter_handler3                                                             |
   | Returns   : IRQ_HANDLED                                                                |
   | parameter : irq, dev_id, reg                                                           |
   | perform   : Interrupt handler for "volume +" button                                    |
   |             If you press "volume +" button, stopwatch will be reset.                   |
   |             Reset FND value.                                                           |
   +----------------------------------------------------------------------------------------+
*/
irqreturn_t inter_handler3(int irq, void* dev_id, struct pt_regs* reg) {
    printk(KERN_ALERT "Stopwatch reset!\n");
    
    memset(fnd_str, 0, 4);
    outw(0, (unsigned int)iom_fpga_fnd_addr);                   // Print 0000 in FND device
    
    msec = 0;
    if(isStart == true && isPause == false){                    // if stopwatch module is running, delete the timer and start a new timer.
        del_timer(&stopwatch); 
        stopwatch.expires = jiffies + (msec * HZ / 100);
        stopwatch.data = NULL;
        stopwatch.function = stopwatch_callback;
        add_timer(&stopwatch);                                  // add stopwatch timer to timer list      
    }

    return IRQ_HANDLED;
}

/* 
   +---------------------------------------------------------------------------------------------+
   | Method    : inter_handler4                                                                  |
   | Returns   : IRQ_HANDLED                                                                     |
   | parameter : irq, dev_id, reg                                                                |
   | perform   : Interrupt handler for "volume -" button                                         |
   |             If you press "volume -" button for 3 seconds, add stop timer to the timer list. |
   |             If you release the button, stop timer will be deleted.                          |
   +---------------------------------------------------------------------------------------------+
*/
irqreturn_t inter_handler4(int irq, void* dev_id, struct pt_regs* reg) {
    if(isPressed == false){                                     // if you press button
        printk("stop button pressed\n");
        isPressed = true;

        stop.expires = get_jiffies_64() + (3 * HZ);
        stop.data = NULL;
        stop.function = stop_callback;
        add_timer(&stop);                                       // add stop timer to timer list
    }
    else{                                                       // if you release button
        printk("stop button released\n");
        isPressed = false;
        del_timer(&stop);                                       // delete timer
    }
    return IRQ_HANDLED;
}

/* 
   +----------------------------------------------------------------------------------------+
   | Method    : device_open                                                                |
   | Returns   : -EBUSY (if the device is already, used) / 0 (default)                      |
   | parameter : minode, mfile                                                              |
   | perform   : Mapping the device's interrupt number to the interrupt handler.            |
   |             home, back, volume + has IRQF_TRIGGER_RISING option.                       |
   |             volume - has IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING option             |
   +----------------------------------------------------------------------------------------+
*/
int device_open(struct inode *minode, struct file *mfile){
    int ret;
    int irq;

    if(device_usage != 0) return -EBUSY;
    printk("Device open!\n");
    device_usage = 1;

    init_waitqueue_head(&my_queue);

    // interrupt Home button
    gpio_direction_input(IMX_GPIO_NR(1,11));
    irq = gpio_to_irq(IMX_GPIO_NR(1,11));
    printk(KERN_ALERT "IRQ Number : %d\n", irq);
    ret = request_irq(irq, inter_handler1, IRQF_TRIGGER_RISING, "home", 0);

    // interrupt Back button
    gpio_direction_input(IMX_GPIO_NR(1,12));
    irq = gpio_to_irq(IMX_GPIO_NR(1,12));
    printk(KERN_ALERT "IRQ Number : %d\n", irq);
    ret = request_irq(irq, inter_handler2, IRQF_TRIGGER_RISING, "back", 0);

    // interrupt volume up button
    gpio_direction_input(IMX_GPIO_NR(2,15));
    irq = gpio_to_irq(IMX_GPIO_NR(2,15));
    printk(KERN_ALERT "IRQ Number : %d\n", irq);
    ret = request_irq(irq, inter_handler3, IRQF_TRIGGER_RISING, "volup", 0);

    // interrupt volume down button
    gpio_direction_input(IMX_GPIO_NR(5,14));
    irq = gpio_to_irq(IMX_GPIO_NR(5,14));
    printk(KERN_ALERT "IRQ Number : %d\n", irq);
    ret = request_irq(irq, inter_handler4, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "voldown", 0);

    isStart = false;
    isPressed = false;
    return 0;
}

/* 
   +----------------------------------------------------------------------------------------+
   | Method    : device_release                                                             |
   | Returns   : 0                                                                          |
   | parameter : minode, mfile                                                              |
   | perform   : Reset device_usage to 0.                                                   |
   |             Remove the interrupt handler registered as the request_irq() function.     |
   +----------------------------------------------------------------------------------------+
*/
int device_release(struct inode *minode, struct file *mfile) {
    device_usage = 0;

    free_irq(gpio_to_irq(IMX_GPIO_NR(1, 11)), NULL);
    free_irq(gpio_to_irq(IMX_GPIO_NR(1, 12)), NULL);
    free_irq(gpio_to_irq(IMX_GPIO_NR(2, 15)), NULL);
    free_irq(gpio_to_irq(IMX_GPIO_NR(5, 14)), NULL);

    printk(KERN_ALERT "Release Module\n");
    return 0;
}

/* 
   +----------------------------------------------------------------------------------------+
   | Method    : stop_callback                                                              |
   | Returns   : void                                                                       |
   | parameter : timeout                                                                    |
   | perform   : Callback function for stop timer.                                          |
   |             Print 0000 on FND device.                                                  |
   |             If stopwatch timer is in the timer list, delete the timer.                 |
   |             Wake up the prcess.                                                        |
   +----------------------------------------------------------------------------------------+
*/
static void stop_callback(unsigned long timeout){

    printk(KERN_ALERT "Exit!\n");
    
    outw(0, (unsigned int)iom_fpga_fnd_addr);           // Print 0000 in FND device
    if(isStart == true) del_timer(&stopwatch);          // if the stopwatch is running, delete timer from timer list
    
    __wake_up(&my_queue, 1, 1, NULL);

}

/* 
   +----------------------------------------------------------------------------------------+
   | Method    : stopwatch_callback                                                         |
   | Returns   : void                                                                       |
   | parameter : timeout                                                                    |
   | perform   : Print time on FND device.                                                  |
   |             Add a timer that runs after 1 second to the timer list.                    |
   +----------------------------------------------------------------------------------------+
*/
static void stopwatch_callback(unsigned long timeout){
    unsigned short int value_short;

    printk("stopwatch_timer %d%d%d%d\n", fnd_str[0], fnd_str[1], fnd_str[2], fnd_str[3]);
    value_short = fnd_str[0] << 12
                | fnd_str[1] << 8
                | fnd_str[2] << 4
                | fnd_str[3];

    outw(value_short, (unsigned int)iom_fpga_fnd_addr);
    
    fnd_str[3] += 1;
    if(fnd_str[3] == 10) { fnd_str[2] += 1; fnd_str[3] = 0; }
    if(fnd_str[2] == 6)  { fnd_str[1] += 1; fnd_str[2] = 0; }
    if(fnd_str[1] == 10) { fnd_str[0] += 1; fnd_str[1] = 0; }
    if(fnd_str[0] == 6)  { fnd_str[0] = 0; }

    stopwatch.expires = get_jiffies_64() + (1 * HZ);
    stopwatch.data = NULL;
    stopwatch.function = stopwatch_callback;

    add_timer(&stopwatch);
}

/* 
   +----------------------------------------------------------------------------------------+
   | Method    : device_write                                                               |
   | Returns   : length                                                                     |
   | parameter : inode, gdata, length, off_what                                             |
   | perform   : Initialize global variables and sleep the process.                         |
   +----------------------------------------------------------------------------------------+
*/
ssize_t device_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what){

    /* initialize global variables */
    isStart = false;
    isPressed = false;
    isPause = false;
    msec = 0;
    memset(fnd_str, 0, 4); 
    interruptible_sleep_on(&my_queue);              // sleep process
    return length;  
}   

/* 
   +----------------------------------------------------------------------------------------+
   | Method    : inter_register_cdev                                                        |
   | Returns   : negative integer (if error) / 0 (default)                                  |
   | parameter : void                                                                       |
   | perform   : Get major, minor number.                                                   |
   +----------------------------------------------------------------------------------------+
*/
static int inter_register_cdev(void)
{
    int error;
    if(inter_major) {
        inter_dev = MKDEV(inter_major, inter_minor);
        error = register_chrdev_region(inter_dev, 1, "inter");
    }else{
        error = alloc_chrdev_region(&inter_dev, inter_minor, 1, "inter");
        inter_major = MAJOR(inter_dev);
    }
    if(error<0) {
        printk(KERN_WARNING "inter: can't get major %d\n", inter_major);
        return result;
    }
    printk(KERN_ALERT "major number = %d\n", inter_major);
    cdev_init(&inter_cdev, &iom_fops);
    inter_cdev.owner = THIS_MODULE;
    inter_cdev.ops = &iom_fops;
    error = cdev_add(&inter_cdev, inter_dev, 1);
    if(error)
    {
        printk(KERN_NOTICE "inter Register Error %d\n", error);
    }
    return 0;
}

/* 
   +----------------------------------------------------------------------------------------+
   | Method    : iom_device_init                                                            |
   | Returns   : negative integer (if error) / 0 (default)                                  |
   | parameter : void                                                                       |
   | perform   : Initialize I/O devices and timer. Mapping the deive address.               |
   +----------------------------------------------------------------------------------------+
*/
int __init iom_device_init(void){

    result = register_chrdev(IOM_DEVICE_MAJOR, IOM_DEVICE_NAME, &iom_fops);
    if(result < 0){
        printk(KERN_WARNING"Can't get any major\n");
        return result;
    }

    if((result = inter_register_cdev()) < 0)
        return result;

    /* DEVICE ADDRESS mapping */
    
    iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
    
    ///////////////////////////
    
    init_timer(&stopwatch);
    init_timer(&stop);
    printk("init module\n");
    return 0;

}

/* 
   +----------------------------------------------------------------------------------------+
   | Method    : iom_device_exit                                                            |
   | Returns   : void                                                                       |
   | parameter : void                                                                       |
   | perform   :Unmapping the deivce. Release the interrupt number.                         |
   +----------------------------------------------------------------------------------------+
*/
void __exit iom_device_exit(void){
    
    /*  UNMAPPING  */

    iounmap(iom_fpga_fnd_addr);
     
    /////////////////

    device_usage = 0;
    cdev_del(&inter_cdev);
    unregister_chrdev_region(inter_dev, 1);
    del_timer_sync(&stopwatch);
    unregister_chrdev(IOM_DEVICE_MAJOR, IOM_DEVICE_NAME);
}


module_init(iom_device_init);
module_exit(iom_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Phillips");
