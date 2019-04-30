#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <termios.h>
#include <pthread.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/mman.h>

#include <linux/input.h>

void show_fnd(int hourMin_int);
void show_led(int num);
void show_dow_matrix(unsigned char matrix[10]);
void show_text(unsigned char arr[33]);
void *output_msgtype3();
void *output_msgtype4();
void output_main();
