#include "output.h"

#define output_q 5678
#define MAX_DIGIT 4
#define FND_DEVICE "/dev/fpga_fnd"
#define FPGA_DOT_DEVICE "/dev/fpga_dot"
#define FPGA_TEXT_LCD_DEVICE "/dev/fpga_text_lcd"
#define FPGA_BASE_ADDRESS 0x08000000 // physical address
#define LED_ADDR 0x16
#define MAX_BUFF 32
#define LINE_BUFF 16


typedef struct{
    long msgtype;
    char switch_arr[10];
    int key_num;
    int main_mode;
    int hourMin;
    int output_switch;
    int switch_flag;
    int count;
    int dot_idx;
    unsigned char matrix[10];
    unsigned char string[33];
}msgbuf;


key_t key_id;

pthread_t switch_thread;
int switch_thr_id;

msgbuf mybuf;
int mode;

void show_fnd(int hourMin_int){
    int dev;
    unsigned char data[4];
    unsigned char retval;
    int i;
    int str_size;
    char hourMin[5];
    memset(data, 0, sizeof(data));
    
    sprintf(hourMin,"%04d",hourMin_int);
    str_size = strlen(hourMin);
    if(str_size>MAX_DIGIT){
        printf("Warning! 4 Digit\n");
        str_size = MAX_DIGIT;
    }
    for(i=0;i<str_size;i++){

        data[i] = hourMin[i] - 0x30;
    }
    dev = open(FND_DEVICE, O_RDWR);
    if(dev < 0){
        printf("Device open error : %s\n", FND_DEVICE);
        exit(1);
    }

    retval = write(dev, &data, 4);
    if(retval<0){
        printf("Write Error!\n");
        exit(1);
    }
    memset(data, 0, sizeof(data));
    
    retval = read(dev, &data ,4);
    if(retval < 0){
        printf("Read Error!\n");
        exit(1);
    }
    printf("Current FND Value : ");
    for(i=0;i<str_size;i++)
        printf("%d",data[i]);
    printf("\n");
    close(dev);
    return;
}

void show_led(int num){

    int fd,i;
    unsigned long *fpga_addr = 0;
    unsigned char *led_addr =0;
    unsigned char data;


    data = num;

    fd = open("/dev/mem", O_RDWR | O_SYNC); //memory device open
    if (fd < 0) {  //open fail check
        perror("/dev/mem open error");
        exit(1);
    }
    fpga_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, FPGA_BASE_ADDRESS);

    if (fpga_addr == MAP_FAILED) //mapping fail check
    {
        printf("mmap error!\n");
        close(fd);
        exit(1);
    }
    led_addr=(unsigned char*)((void*)fpga_addr+LED_ADDR);
    *led_addr=data; 
    data=0;
    data=*led_addr; //read led
    printf("Current LED VALUE : %d\n",data);
    munmap(led_addr, 4096); //mapping ÇØÁŠ
    close(fd);

}
void show_dot_matrix(unsigned char matrix[10]){
    int i;
    int dev;
    int str_size;
    

    dev = open(FPGA_DOT_DEVICE, O_WRONLY);
    if(dev < 0){
        printf("Device open error : %s\n",FPGA_DOT_DEVICE);
        exit(1);
    }
    str_size = 10;
    write(dev, matrix, str_size);
    close(dev);

}

void show_text(unsigned char arr[33]){
    int i;
    int dev;
    int str_size;
    int chk_size;
    unsigned char string[32];
    
    memset(string, 0, sizeof(string));
    dev = open(FPGA_TEXT_LCD_DEVICE, O_WRONLY);
    if(dev < 0){
        printf("Device open error : %s\n",FPGA_TEXT_LCD_DEVICE);
        return ;
    }
    str_size = strlen(arr);

    if(str_size >= 0){
        for(i=0;i<strlen(arr);i++){
            string[i] = arr[i];
        }
        for(i=strlen(arr);i<MAX_BUFF;i++){
            string[i] = ' ';
        }
    }
    write(dev, string, MAX_BUFF);
    close(dev);

}

// receive message type 3 (when switch button pressed)
void *output_msgtype3(){
    int rcvtype = 3;
    while(1){
        if(msgrcv(key_id, (void*)&mybuf, sizeof(msgbuf), rcvtype, 0)==-1){
            perror("msgrcv error : ");
            exit(0);
        }
        printf("Output Rev : 333\n");
        switch(mode)
        {
            case 0:
                printf("mybuf2 : %d\n",mybuf.hourMin);
                show_fnd(mybuf.hourMin);
                if(mybuf.switch_flag != -1){ 
                    show_led(mybuf.output_switch);
                }
                show_text(mybuf.string);
                break;
            case 1:

                show_fnd(mybuf.count);
                show_led(mybuf.output_switch);
                break;
            case 2:
                show_dot_matrix(mybuf.matrix);
                show_fnd(mybuf.count);
                //show_dot_matrix(mybuf.matrix);
                show_text(mybuf.string);
                break;
            case 3:
                show_dot_matrix(mybuf.matrix);
                show_fnd(mybuf.count);
                break;
            case 4:
                show_dot_matrix(mybuf.matrix);
                show_fnd(mybuf.count);
                show_text(mybuf.string);
                break;
        }
    }
}
// receive message type 4 (when key pressed)
void *output_msgtype4(){
    int rcvtype = 4;
    int i;
    while(1){
        if(msgrcv(key_id, (void*)&mybuf, sizeof(msgbuf), rcvtype, 0)==-1){
            perror("msgrcv error : ");
            exit(0);
        }
        mode = mybuf.main_mode;
        switch(mode)
        {
            case 0:
                show_fnd(mybuf.hourMin);
                show_led(mybuf.output_switch);
                show_text(mybuf.string);
                show_dot_matrix(mybuf.matrix);
                break;
            case 1:
                show_dot_matrix(mybuf.matrix);
                show_fnd(mybuf.count);
                show_led(mybuf.output_switch);
                show_text(mybuf.string);
                break;
            case 2:
                show_dot_matrix(mybuf.matrix);
                show_fnd(mybuf.count);
                show_led(mybuf.output_switch);
                show_text(mybuf.string);

                break;
            case 3:
                show_dot_matrix(mybuf.matrix);
                show_fnd(mybuf.count);
                show_led(mybuf.output_switch);
                show_text(mybuf.string);
                break;
            case 4:
                show_dot_matrix(mybuf.matrix);
                show_fnd(mybuf.count);
                show_led(mybuf.output_switch);
                show_text(mybuf.string);
                break;
        }
        printf("Output Rev : 444\n");
    }
}
void output_main(){

    pthread_t p_thread[2];
    int thr_id;

    key_id = msgget(output_q, IPC_CREAT|0666);

    if(key_id < 0){
        perror("msgget error : ");
        exit(0);
    }
    // create thread (receive message type 3)
    thr_id = pthread_create(&p_thread[0],NULL,output_msgtype3, NULL);
    if(thr_id < 0){
        perror("thread create error : ");
        exit(0);
    }
    // create thread (receive message type 4)
    thr_id = pthread_create(&p_thread[1], NULL, output_msgtype4, NULL);
    if(thr_id < 0){
        perror("thread create error : ");
        exit(0);
    }
    
    pthread_join(p_thread[0],(void **)NULL);
    pthread_join(p_thread[1],(void **)NULL);


}
