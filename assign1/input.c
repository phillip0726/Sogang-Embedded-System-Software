#include "input.h"

#define MAX_BUTTON 9        //switch button
#define BUFF_SIZE 64
#define KEY_RELEASE 0
#define KEY_PRESS 1
#define input_q 1234        //input message queue id
typedef struct{
    /* type 1 : switch event */
    /* type 2 : key event    */
    long msgtype;
    char switch_arr[10];
    int key_num;
}msgbuf;

unsigned char quit = 0;
key_t key_id;
msgbuf mybuf;
void user_signal1(int sig) 
{
    quit = 1;
}


void *switch_function(void *data)
{
    int i;
    int dev;
    int buff_size;
    char arr[10];
    char tmp[10];
    char num_buff[2];
    unsigned char push_sw_buff[MAX_BUTTON];

    dev = open("/dev/fpga_push_switch", O_RDWR);

    if (dev<0){
        printf("Device Open Error\n");
        close(dev);
        return ;
    }

    (void)signal(SIGINT, user_signal1);

    tmp[0] = '\0';
    buff_size=sizeof(push_sw_buff);
    printf("Press <ctrl+c> to quit. \n");
    while(!quit){
        usleep(400000);
        arr[0] = '\0';
        read(dev, &push_sw_buff, buff_size);
        
        for(i=0;i<MAX_BUTTON;i++) {
            if(push_sw_buff[i] == 1){
                sprintf(num_buff,"%d",i+1);
                strcat(arr,num_buff);                   // if press switch, append that number to the string
            }
        }
        if(strcmp(arr, tmp) != 0){                      // if not press and hold the switch
            strcpy(tmp, arr);
            strcpy(mybuf.switch_arr,arr);  
            if(strlen(arr) != 0){                       // if nothing has been pressed, don't send message
                printf("\n");
                mybuf.msgtype = 1;
                mybuf.key_num = -1;

                if(msgsnd(key_id, (void *)&mybuf, sizeof(msgbuf), IPC_NOWAIT)==-1){
                    perror("msgsnd error : ");
                    exit(0);
                }
                printf("Send switch : %s\n",arr);
            }

        }
    }
    close(dev);
}
void *key_function(void *data)
{
    struct input_event ev[BUFF_SIZE];
    int fd, rd, value, size = sizeof (struct input_event);
    char name[256] = "Unknown";

    char* device = "/dev/input/event0";
    if((fd = open (device, O_RDONLY)) == -1) {
        printf ("%s is not a vaild device.n", device);
    }

    while (1){
        if ((rd = read (fd, ev, size * BUFF_SIZE)) < size)
        {
            printf("read()");  
            return ;     
        }

        value = ev[0].value;

        if (value != ' ' && ev[1].value == 1 && ev[1].type == 1){ // Only read the key press event
            printf ("code%d\n", (ev[1].code));
        }
        if( value == KEY_PRESS ) {
            printf ("key press\n");
            printf ("Type[%d] Value[%d] Code[%d]\n", ev[0].type, ev[0].value, (ev[0].code));

        } else{
            if( value == KEY_RELEASE ) {    // Send message only when key is released
                printf ("key release\n");
                printf ("Type[%d] Value[%d] Code[%d]\n", ev[0].type, ev[0].value, (ev[0].code));
                strcpy(mybuf.switch_arr,"");
                mybuf.msgtype = 2;
                mybuf.key_num = ev[0].code;
                if(msgsnd(key_id, (void*)&mybuf,sizeof(msgbuf), IPC_NOWAIT)==-1){
                    perror("msgsnd error : ");
                    exit(0);
                }
            }
        }
    }

    return ;
}

int input_main(void)
{
    pthread_t p_thread[2];
    char p1[] = "thread_switch";
    char p2[] = "thread_key";
    int thr_id;
   
    memset(mybuf.switch_arr,0x00,10);
    memcpy(mybuf.switch_arr,"",0);

    key_id = msgget((key_t)input_q, IPC_CREAT|0666);
    if(key_id == -1){
        perror("msgget error : ");
        exit(0);
    }

    // Create thread (send message when switch event occured)
    thr_id = pthread_create(&p_thread[0],NULL,switch_function,(void*)p1);
    
    if(thr_id < 0){
        perror("thread create error : ");
        exit(0);
    }
    // Create thread (send message when key event occured)
    thr_id = pthread_create(&p_thread[1], NULL, key_function, (void*)p2);
    if(thr_id < 0){
        perror("thread create error : ");
        exit(0);
    }
    
    pthread_join(p_thread[0],(void **)NULL);
    pthread_join(p_thread[1],(void **)NULL);

    return 0;

}
