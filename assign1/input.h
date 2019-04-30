#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <sys/ioctl.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void *switch_function(void *data);
void *key_function(void *data);
void user_signal1(int sig);
int input_main(void);

