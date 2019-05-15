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

#define PARAM_NUM 4              // passing parameter number
#define CONVERT_INPUT 376        // my syscall number
#define DEVICE_NAME "/dev/dev_driver"
#define MAJOR_NUM 242
#define IOCTL_WRITE _IOW(MAJOR_NUM, 0, char *)

unsigned int start_interval;     // time interval
unsigned int start_num;          // repetition number
unsigned int start_option;       // start option
char gdata[4];                   // 4 bytes input stream


int main(int argc, char **argv){
    int i, tmp;
    int option_flag = 0;
    int dev;
    int result;
    if(argc != PARAM_NUM){
        printf("The parameter is insufficient\n");
        return 0;
    }

    start_interval = atoi(argv[1]);
    start_num      = atoi(argv[2]);
    start_option   = atoi(argv[3]);

    if(start_interval < 1 || start_interval > 100){             //  1 <= time interval <= 100
        printf("Time interval must be greater than zero and less than 100.\n");
        return 0;
    }
    if(start_num < 1 || start_num > 100){                       // 1 <= repetition number <= 100
        printf("Repetition number must be greater than zero and less than 100.\n");
        return 0;
    }
    if(start_option < 1 || start_option > 8000){                // 1 <= option <= 8000
        printf("Start option must be greater than zero and less than 8000.\n");
        return 0;
    }
    tmp = start_option;
    for(i = 4; i != 0; i--){
        if((tmp % 10) >8){
            printf("Each number in Start option can not exceed 8.\n");
            return 0;
        }
        if((tmp % 10) == 0)
            option_flag++;
        tmp = tmp / 10;
    }
    if(option_flag != 3){
        printf("Any number other than one of the four digits must be zero.\n");
        return 0;
    }
    if(atof(argv[3]) - start_option != 0){                  // if option is float
        printf("Start option must be Integer!\n");
        return 0;
    }
    /*                  <gdata>                   
    * +----------+----------+----------+----------+
    * |   pos    |   value  | interval |   count  |
    * +----------+----------+----------+----------+
    */
    result = syscall(CONVERT_INPUT, start_interval, start_num, start_option);
    gdata[0] = (result >> 24) & 0xFF;
    gdata[1] = (result >> 16) & 0xFF;
    gdata[2] = (result >> 8) & 0xFF;
    gdata[3] = (result) & 0xFF;
    dev = open(DEVICE_NAME, O_RDWR);
    if(dev < 0){
        printf("Error\n");
        exit(1);
    }
    result = ioctl(dev, IOCTL_WRITE, gdata);
    close(dev);
    return 0;
}
