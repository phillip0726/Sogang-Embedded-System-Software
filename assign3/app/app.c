#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#include <linux/ioctl.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DEVICE_NAME "/dev/stopwatch"            // device name
#define MAJOR_NUM 242                           // device major number


/* 
   +----------------------------------------------------------------------------------------+
   | Method    : main                                                                       |
   | Returns   : negative (if device open fail) / 0 (default)                               |
   | parameter : void                                                                       |
   | perform   : Simply open and write the device                                           |
   +----------------------------------------------------------------------------------------+
*/
int main(void){
    int dev;
    unsigned char data[4] = {0,0,0,0};
    dev = open(DEVICE_NAME, O_RDWR);                // device open
    if(dev < 0){
        printf("/dev/stopwatch Error\n");
        exit(-1);
    }
    write(dev, &data, 4);                           // deivce write
    close(dev);
    return 0;
}
