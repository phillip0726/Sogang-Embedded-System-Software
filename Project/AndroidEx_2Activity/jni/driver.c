#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "android/log.h"

#define TEXT_DRIVER "/dev/fpga_text_lcd"
#define DOT_DRIVER "/dev/fpga_dot"
#define FND_DRIVER "/dev/fpga_fnd"
#define SWITCH_DRIVER "/dev/fpga_push_switch"
#define MAX_BUTTON 9

#define LOG_TAG "NATIVE"
#define LOGV(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

unsigned char fpga_number[3][10] = {
	{0x0c,0x1c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x1e}, // 1
	{0x7e,0x7f,0x03,0x03,0x3f,0x7e,0x60,0x60,0x7f,0x7f}, // 2
	{0xfe,0x7f,0x03,0x03,0x7f,0x7f,0x03,0x03,0x7f,0x7e}, // 3
	{0,	  0,   0,	0,	 0,	  0,   0,	0,	 0,	  0} // blank
};
unsigned char quit = 0;

void text_init(int fd);

//text driver(MainActivity2)
JNIEXPORT jint JNICALL Java_com_example_androidex_MainActivity2_textdriveropen(JNIEnv *env, jobject this)
{
    int fd = open(TEXT_DRIVER, O_RDWR);
    if(fd < 0){
       LOGV("fd: %d /dev/fpga_text_lcd open error", fd);
       exit(-1);
    }
    return fd;
}
JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity2_textdriverwrite(JNIEnv *env, jobject this, jint fd, jstring val)
{
	text_init(fd);
	char txt[33];
	memset(txt,0,sizeof(txt));
	char *str = (*env)->GetStringUTFChars(env, val, 0);
	strncpy(txt,str,strlen(str));
	memset(txt+strlen(str),' ',32-strlen(str));
    write(fd, txt, 32);
}
JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity2_textdriverclose(JNIEnv *env, jobject this, jint fd)
{
    int ret = close(fd);
    LOGV("close text driver: %d", ret);
}
//text driver(MyService)
JNIEXPORT void JNICALL Java_com_example_androidex_MyService_textdriverwrite(JNIEnv *env, jobject this, jstring val)
{
	int fd = open(TEXT_DRIVER, O_RDWR);
	text_init(fd);
	char txt[33];
	memset(txt,0,sizeof(txt));
	char *str = (*env)->GetStringUTFChars(env, val, 0);
	strncpy(txt,str,strlen(str));
	memset(txt+strlen(str),' ',32-strlen(str));
    write(fd, txt, 32);
    int ret = close(fd);
}
// initialize text(MainActivity2, Myservice)
JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity2_initialtext(JNIEnv *env, jobject this)
{
	int ret;
	int fd = open(TEXT_DRIVER, O_RDWR);
	text_init(fd);
	ret = close(fd);
}
JNIEXPORT void JNICALL Java_com_example_androidex_MyService_initialtext(JNIEnv *env, jobject this)
{
	int ret;
	int fd = open(TEXT_DRIVER, O_RDWR);
	text_init(fd);
	ret = close(fd);
}
void text_init(int fd)
{
	char init[33];
	memset(init,' ',32);
	write(fd,init,32);
}



//dot driver(MainActivity)
JNIEXPORT jint JNICALL Java_com_example_androidex_MainActivity_dotdriveropen(JNIEnv *env, jobject this)
{
    int fd = open(DOT_DRIVER, O_RDWR);
    if(fd < 0){
       LOGV("fd: %d /dev/fpga_dot open error", fd);
       exit(-1);
    }
    return fd;
}
JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity_dotdriverwrite(JNIEnv *env, jobject this, jint fd, jint mode)
{
	LOGV("mode : %d", mode);
	write(fd, fpga_number[mode-1],sizeof(fpga_number[mode-1]));
}
JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity_dotdriverclose(JNIEnv *env, jobject this, jint fd)
{
    int ret = close(fd);
    LOGV("close dot driver: %d", ret);
}
JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity2_dotwrite(JNIEnv *env, jobject this, jint mode)
{
	int fd = open(DOT_DRIVER, O_RDWR);
	write(fd, fpga_number[mode-1],sizeof(fpga_number[mode-1]));
	int ret = close(fd);
}
JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity3_dotwrite(JNIEnv *env, jobject this, jint mode)
{
	int fd = open(DOT_DRIVER, O_RDWR);
	write(fd, fpga_number[mode-1],sizeof(fpga_number[mode-1]));
	int ret = close(fd);
}
JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity4_dotwrite(JNIEnv *env, jobject this, jint mode)
{
	int fd = open(DOT_DRIVER, O_RDWR);
	write(fd, fpga_number[mode-1],sizeof(fpga_number[mode-1]));
	int ret = close(fd);
}




// initialize dot matrix(MainActivity2,3,4)
JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity2_initialdot(JNIEnv *env, jobject this)
{
	int ret;
    int fd = open(DOT_DRIVER, O_RDWR);
    write(fd, fpga_number[3],sizeof(fpga_number[3]));
    ret = close(fd);
}
JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity3_initialdot(JNIEnv *env, jobject this)
{
	int ret;
    int fd = open(DOT_DRIVER, O_RDWR);
    write(fd, fpga_number[3],sizeof(fpga_number[3]));
    ret = close(fd);
}
JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity4_initialdot(JNIEnv *env, jobject this)
{
	int ret;
    int fd = open(DOT_DRIVER, O_RDWR);
    write(fd, fpga_number[3],sizeof(fpga_number[3]));
    ret = close(fd);
}




// fnd driver(MyService)
JNIEXPORT void JNICALL Java_com_example_androidex_MyService_fnddriverwrite(JNIEnv *env, jobject this, jint sec)
{
	int fd = open(FND_DRIVER, O_RDWR);
	int m, s;
	char data[4];
	s = sec%60; m = sec/60;
	data[0] = m/10; data[1] = m%10;
	data[2] = s/10; data[3] = s%10;
	write(fd,&data,4);
	close(fd);
}




// switch, dot driver(SwitchManager)
void user_signal(int sig)
{
	quit = 1;
}
JNIEXPORT jint JNICALL Java_com_example_androidex_SwitchManager_switchwrite(JNIEnv *env, jobject this)
{
	int fd = open(SWITCH_DRIVER, O_RDWR);
	int i,buff_size;
	int num = 0;
	unsigned char push_sw_buff[MAX_BUTTON];

	(void)signal(SIGINT, user_signal);

	buff_size = sizeof(push_sw_buff);
	read(fd, &push_sw_buff, buff_size);

	for(i=0; i<MAX_BUTTON; i++){
		if(push_sw_buff[i]==1){
			num = i-2;
			break;
		}
	}
	close(fd);
	return num;
}
JNIEXPORT void JNICALL Java_com_example_androidex_SwitchManager_dotwrite(JNIEnv *env, jobject this, jint mode)
{
	int fd = open(DOT_DRIVER, O_RDWR);
	write(fd, fpga_number[mode-1],sizeof(fpga_number[mode-1]));
	int ret = close(fd);
}
JNIEXPORT void JNICALL Java_com_example_androidex_SwitchManager_initialtext(JNIEnv *env, jobject this)
{
	int ret;
	int fd = open(TEXT_DRIVER, O_RDWR);
	text_init(fd);
	ret = close(fd);
}
