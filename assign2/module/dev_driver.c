/*
FILE : dev_driver.c
AUTH : Phillips
*/

#include "dev_driver.h"

#define IOM_DEVICE_MAJOR 242            // device major number
#define IOM_DEVICE_NAME "dev_driver"    // device name
#define IOM_FND_ADDRESS 0x08000004      // FND physical address
#define IOM_LED_ADDRESS 0x08000016      // LED physical address
#define IOM_DOT_ADDRESS 0x08000210      // DOT physical address
#define IOM_TEXT_ADDRESS 0x08000090     // TEXT physical address
#define TEXT_MAX_BUFFER 16              // TEXT_LCD single line max length

// define functions
ssize_t device_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what);
int device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);
int device_open(struct inode *minode, struct file *mfile);
int device_release(struct inode *minode, struct file *mfile);

// global variables
static int device_usage = 0;
static unsigned char *iom_fpga_fnd_addr;
static unsigned char *iom_fpga_led_addr;
static unsigned char *iom_fpga_dot_addr;
static unsigned char *iom_fpga_text_addr;

static struct struct_mydata {
    struct timer_list timer;
    int init;                   // initial repetition number. This variable cannot be changed.
    int pos;                    // FND position of number that is not zero
    int value;                  // FND number
    int interval;               // time interval
    int count;                  // repetition number count
    int up_text_flag;           // up text flag in TEXT_LCD. 1-> move text right, 0-> move text left
    int down_text_flag;         // down text flag in TEXT_LCD. 1-> move text right, 0-> move text left
};

struct struct_mydata mydata;
static struct file_operations iom_fops=
{
    .owner          =       THIS_MODULE,
    .open           =       device_open,
    .write          =       device_write,
    .unlocked_ioctl =       device_ioctl,
    .release        =       device_release,
};

int device_open(struct inode *minode, struct file *mfile){
    if(device_usage != 0) return -EBUSY;
    printk("Device open!\n");
    device_usage = 1;
    return 0;
}

int device_release(struct inode *minode, struct file *mfile) {
    device_usage = 0;
    return 0;
}

static void kernel_timer_blink(unsigned long timeout){          // callback function
    unsigned short int i;
    unsigned short int student_num_len;                 // strlen(student_num)
    unsigned short int student_name_len;                // strlen(student_name)
    unsigned short int student_num_buffer;              // 16 - strlen(student_num)
    unsigned short int student_name_buffer;             // 16 - strlen(student_name)
    unsigned short int fnd_value = 0;
    unsigned short led_value = 1;
    unsigned short int dot_value;

    unsigned char up_text[16];                          // TEXT_LCD upper string
    unsigned char down_text[16];                        // TEXT_LCD downward string
    unsigned short int text_value = 0;

    struct struct_mydata *p_data = (struct struct_mydata*)timeout;
    printk("kernel_timer_blink %d\n", p_data->count);

    /* Terminate Timer */
    if( p_data->count <= 0 ) {
        outw(0, (unsigned int)iom_fpga_fnd_addr);       // write FND 0
        outw(0, (unsigned int)iom_fpga_led_addr);       // turn off LED
        for(i = 0; i < 10; i++)                         // write an empty string to TEXT_LCD
            outw(0, (unsigned int)iom_fpga_dot_addr + i*2);
        for(i = 0; i < 32; i++)
            outw(' ', (unsigned int)iom_fpga_text_addr + i);
        return;
    }
    
    fnd_value = p_data->value;

    for(i = p_data->pos; i < 4; i++){                   // fnd_value = value * 10^(4-pos)
        fnd_value = fnd_value << 4;
    }
    outw(fnd_value,(unsigned int)iom_fpga_fnd_addr);    // write FND

    for(i = p_data->value; i < 8; i++){                 // led_value = value * 2^(8-pos)
        led_value = led_value << 1;
    }
    outw(led_value, (unsigned int)iom_fpga_led_addr);   // turn on LED
    
    for(i = 0; i < 10; i++){                            // draw dot matrix
        dot_value = fpga_number[p_data->value][i] & 0x7F;
        outw(dot_value, (unsigned int)iom_fpga_dot_addr+i*2);
    }

    for(i = 0; i < 16; i++){                            // write an empty string to TEXT_LCD (initiallize)
        memset(up_text, ' ', 16);
        memset(down_text, ' ', 16);
    }

    for(student_num_len = 0; student_num_len < 16; student_num_len++){      // get string length of upper text
       if(student_num[student_num_len] == '\0')
           break;
    }
    for(student_name_len = 0; student_name_len < 16; student_name_len++){   // get string length of downward text
        if(student_name[student_name_len] == '\0')
            break;
    }
    student_num_buffer = TEXT_MAX_BUFFER - student_num_len;                 // 16 - strlen(student num)
    student_name_buffer = TEXT_MAX_BUFFER - student_name_len;               // 16 - strlen(student name)

    if(p_data->up_text_flag)
        memcpy(up_text + ((p_data->init - p_data->count) % student_num_buffer), student_num, student_num_len);
    else
        memcpy(up_text + student_num_buffer - ((p_data->init - p_data->count) % student_num_buffer), student_num, student_num_len);

    if(p_data->down_text_flag)
        memcpy(down_text + ((p_data->init - p_data->count) % student_name_buffer), student_name, student_name_len);
    else
        memcpy(down_text + student_name_buffer - ((p_data->init - p_data->count) % student_name_buffer), student_name, student_name_len);

    // Upper case
    if((p_data->up_text_flag) && ((p_data->init - p_data->count + 1) % student_num_buffer == 0))    // if reach the right end
        p_data->up_text_flag = 0;
    else if((p_data->up_text_flag == 0) && ((p_data->init - p_data->count + 1) % student_num_buffer == 0)) // if reach the left end
        p_data->up_text_flag = 1;

    // Downward case
    if((p_data->down_text_flag) && ((p_data->init - p_data->count + 1) % student_name_buffer == 0)) // if reach the right end
        p_data->down_text_flag = 0;
    else if((p_data->down_text_flag == 0) && ((p_data->init - p_data->count + 1) % student_name_buffer == 0))  // if reach the left end
        p_data->down_text_flag = 1;

    /*              TEXT_LCD        
    * + - - - - - - - - - - - - - - - +
    * |         lenght : 16           |  -> up_text
    * + - - - - - - - - - - - - - - - +
    * |         lenght : 16           |  -> down_text
    * + - - - - - - - - - - - - - - - +
    */
    for( i = 0; i < 16; i++){               // write TEXT_LCD
        text_value = (up_text[i] & 0xFF) << 8 | (up_text[i + 1] & 0xFF);
        outw(text_value, (unsigned int)iom_fpga_text_addr + i);
        text_value = (down_text[i] & 0xFF) << 8 | (down_text[i + 1] & 0xFF);
        outw(text_value, (unsigned int)iom_fpga_text_addr + i + 16);
        i++;
    }


    if(p_data->value == 8){                             // increase pos
        p_data->pos = p_data->pos % 4 + 1;
    }
    p_data->value = p_data->value % 8 + 1;              // increase value
    p_data->count--;
    mydata.timer.expires = get_jiffies_64() + (p_data->interval * HZ / 10);
    mydata.timer.data = (unsigned long)&mydata;
    mydata.timer.function = kernel_timer_blink;

    add_timer(&mydata.timer);

}
ssize_t device_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what){
    const char *tmp = gdata;
    char kernel_timer_buff[4];

    if(copy_from_user(&kernel_timer_buff, tmp, length)){
        return -EFAULT;
    }
    mydata.init             = kernel_timer_buff[3];
    mydata.pos              = kernel_timer_buff[0];
    mydata.value            = kernel_timer_buff[1];
    mydata.interval         = kernel_timer_buff[2];
    mydata.count            = kernel_timer_buff[3];
    mydata.up_text_flag     = 1;
    mydata.down_text_flag   = 1;
    printk("pos : %d\n",mydata.pos);
    printk("value : %d\n", mydata.value);
    printk("interval : %d\n",mydata.interval);
    printk("timer count : %d\n", mydata.count);

    del_timer_sync(&mydata.timer);
    mydata.timer.expires = jiffies + (mydata.interval * HZ / 10);
    mydata.timer.data = (unsigned long)&mydata;
    mydata.timer.function = kernel_timer_blink;

    add_timer(&mydata.timer);
    return 1;
}   
int device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param){
    char *tmp;
    
    switch(_IOC_NR(ioctl_num)){
        case 0:                             // if write flag
            tmp = (char *)ioctl_param;
            device_write(file, tmp, 4, 0);
            break;
        default:
            break;
    }
    return 1;
}
int __init iom_device_init(void){
    int result;

    result = register_chrdev(IOM_DEVICE_MAJOR, IOM_DEVICE_NAME, &iom_fops);
    if(result < 0){
        printk(KERN_WARNING"Can't get any major\n");
        return result;
    }

    /* DEVICE ADDRESS mapping */
    
    iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
    iom_fpga_led_addr = ioremap(IOM_LED_ADDRESS, 0x1);
    iom_fpga_dot_addr = ioremap(IOM_DOT_ADDRESS, 0x10);
    iom_fpga_text_addr = ioremap(IOM_TEXT_ADDRESS, 0x32);
    
    ///////////////////////////

    init_timer(&(mydata.timer));
    printk("init module\n");
    return 0;

}

void __exit iom_device_exit(void){
    
    /*  UNMAPPING  */

    iounmap(iom_fpga_fnd_addr);
    iounmap(iom_fpga_led_addr);
    iounmap(iom_fpga_dot_addr);
    iounmap(iom_fpga_text_addr);
    
    /////////////////

    device_usage = 0;
    del_timer_sync(&mydata.timer);
    unregister_chrdev(IOM_DEVICE_MAJOR, IOM_DEVICE_NAME);
}


module_init(iom_device_init);
module_exit(iom_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Phillips");
