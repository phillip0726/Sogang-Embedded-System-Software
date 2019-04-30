#include "input.h"
#include "output.h"

#define input_q 1234
#define output_q 5678
#define mode_num 5
#define MAX_TEXT 32
#define MAX_COL 7
#define MAX_ROW 10

typedef struct{
    /* msgtype 1 : input -> main (switch)   */
    /* msgtype 2 : input -> main (key)      */
    /* msgtype 3 : main  -> output (switch) */
    /* msgtype 4 : main  -> output (key)    */
    long msgtype;
    char switch_arr[10];
    int key_num;
    int main_mode;
    int hourMin;                // mode1 fnd number
    int output_switch;          // led number
    int switch_flag;            
    int count;                  // mode2,3,4 fnd number
    int dot_idx;
    unsigned char matrix[MAX_ROW];   // mode4 dot matrix
    unsigned char string[32];   // mode1,2,3,4 text lcd string
}msgbuf;


/*    Overall Variable    */
int mode = 0;                       // default mode = 0
int switch_num;
pid_t input_pid, output_pid;
////////////////////////////


/*    Clock Global Variable   */
time_t rawtime;
struct tm* timeinfo;
int switch_flag = 1;                // switch flag  : clock set, switch flag -1 : clock modify
int hourMin;                        // hour+Min (HHMM)
int output_switch = 32;             // led number (0~255)
int kkk = 1;                        // variable for counting every 60 sec
////////////////////////////////

/*     Count Global Variable     */
int counter_count   = 0;                      //count number
int notation[4]     = {10, 8, 4, 2};        
int notation_led[4] = {64, 32, 16, 128};    // if notation=10, then turn on LED number 1
int notation_idx    = 0;                       // default notation = 10
///////////////////////////////////

/*      Text Global Variable     */
int text_count           = 0;
int dot_idx              = 0;
int before_switch        = 0;
unsigned char string[33] = "";
unsigned char fpga_number[3][MAX_ROW] = {
    {0x1c,0x36,0x63,0x63,0x63,0x7f,0x7f,0x63,0x63,0x63}, //A
    {0x0c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x3f,0x3f}, //1
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00} //0
};

unsigned char text_lcd_1[10] = {'0','1','2','3','4','5','6','7','8','9'};
unsigned char text_lcd_A[10][3] = {
    {0,0,0},
    {'.','Q','Z'}, {'A','B','C'}, {'D','E','F'},
    {'G','H','I'}, {'J','K','L'}, {'M','N','O'},
    {'P','R','S'}, {'T','U','V'}, {'W','X','Y'}
};

///////////////////////////////////

/*       Draw Global Variable    */

int draw_count  = 0;
int cursor_flag = 1;
int x           = 0;
int y           = 0;
unsigned char draw_matrix[MAX_ROW][MAX_COL] = {                    
    {0, 0, 0, 0, 0, 0, 0},     
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0}
};
unsigned char draw[MAX_ROW] = {                              
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
unsigned int matrix[MAX_ROW][MAX_COL] = {                      // matrix for dot light turn on/off            
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0}
};
unsigned char tmp[MAX_ROW];

///////////////////////////////////

/*           My Function         */
unsigned int ddong_matrix[MAX_ROW][MAX_COL] = {            // ddong location
    {0, 0, 0, 0, 0, 0, 0},     
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0}
};
unsigned char ddong[MAX_ROW];
unsigned int new_line[MAX_COL] = {0, 0, 0, 0, 0, 0, 0}; // new line
char manual[33] = "Start : 5       Left:4 Right:6";
int score       = 0;                                          // increase #1 when dot matrix moves one step downside
int level       = 3;                                          // game level (amount of obstacle)
int level_flag  = 1;
int ddong_x     = MAX_ROW - 1;                               // game start position x
int ddong_y     = MAX_COL / 2;                               // game start position y
int start_flag  = 0;                                        // start_flag = 1 => game start
///////////////////////////////////

key_t input_key_id;
key_t output_key_id;

msgbuf mybuf;

/* move dot matrix downside */
void shift_down(){
    int i, j;
    for(i = MAX_ROW - 1 ;i > 0;i--){
        for(j=0;j<MAX_COL;j++){
            ddong_matrix[i][j] = ddong_matrix[i-1][j];
        }
    }
    
    for(i=0;i<MAX_COL;i++){
        ddong_matrix[0][i] = new_line[i];
    }

}

/* create new dot matrix line randomly */
void create_random_line(){
    int i;
    int count = 0;
    int random;
    int func_level;
    srand(time(NULL));

    /* level balancing */
    if(level_flag == 1)
        func_level = level;
    else
        func_level = level - 1;
    /////////////////////

    for(i = 0;i < MAX_COL;i++){
        if(count < func_level){
            random = rand()%2;
            if(random == 1){
                count++;
            }
            new_line[i] = random;
        }
        else{
            new_line[i] = 0;
        }
    }
}

/* convert ddong matrix to ddong(Dot matrix format) */
void convert_ddong(){
    int i, j;
    for(i=0;i<MAX_ROW;i++){
        ddong[i] = ddong_matrix[i][0] + ddong_matrix[i][1]*2 + ddong_matrix[i][2]*4
                    +ddong_matrix[i][3]*8 + ddong_matrix[i][4]*16 + ddong_matrix[i][5]*32 + ddong_matrix[i][6]*64;
    }
}

/* convert matrix to tmp(Dot matrix format) */
void convert_tmp(){

    int i, j;
    for(i=0;i<10;i++){
        tmp[i] = matrix[i][0]+matrix[i][1]*2+matrix[i][2]*4+matrix[i][3]*8
                        +matrix[i][4]*16+matrix[i][5]*32+matrix[i][6]*64;
    }
}

/* convert draw_matrix to draw(Dot matrix format) */
void convert_draw(){

    int i, j;
    for(i=0;i<10;i++){
        draw[i] = matrix[i][0]+matrix[i][1]*2+matrix[i][2]*4+matrix[i][3]*8
                        +matrix[i][4]*16+matrix[i][5]*32+matrix[i][6]*64;
    }

}

/* convert to decimal number */
int convert_to_10(int not, int num){
    int result = 0;
    int tmp = num;
    int i = 1;
    while(1){
        if(tmp == 0)
            break;
        result += (tmp % 10) * i;
        tmp = tmp/10;
        i *= not;
    }
    return result;
}

/* convert decimal number to designated notation number */
int convert_to_notation(int not, int num)
{
    int tmp;
    int result = 0;
    int i= 1;
    tmp = num;
    while(1){
        if(tmp == 0)
            break;
        result += i * (tmp % not);
        tmp /= not;
        i *=10;
    }
    return result % 1000;
}

/* clean matrix, draw_matrix */
void matrix_empty(){
    int i, j;
    for(i=0;i<10;i++){
        for(j=0;j<7;j++){
            draw_matrix[i][j] = 0;
            matrix[i][j] = 0;
        }
    }
    convert_draw();
    convert_tmp();
}

/* reverse matrix, draw_matrix */
void matrix_reversed(){
    int i,j;
    for(i=0;i<10;i++){
        for(j=0;j<7;j++){
            draw_matrix[i][j] = 1 - draw_matrix[i][j];
            matrix[i][j] = draw_matrix[i][j];
        }
    }
    convert_draw();
    convert_tmp();
}

/* thread : receive message type 1 */
void *input_msgtype1(){
    int i;
    int rcvtype = 1;                // receive message type
    int sndtype = 3;                // send message type
    int switch_num;
    int switch_count = 0;
    char next_char;
    msgbuf mybuf2;

    /* initializing struct */
    memset(mybuf2.matrix,0x00,10);
    memset(mybuf2.string, 0x00, 33);
    memcpy(mybuf2.matrix,"",0);
    memcpy(mybuf2.string, "",0);
    /////////////////////////

    while(1){
        // message receive
        if(msgrcv(input_key_id, (void*)&mybuf2, sizeof(msgbuf), rcvtype, 0)==-1){
            perror("msgrcv error : ");
            exit(0);
        }

        // convert switch number string to integer
        switch_num = atoi(mybuf2.switch_arr);

        switch(mode)
        {
            case 0:
                memset(string, 0, sizeof(string));
                memset(draw_matrix, 0, sizeof(draw_matrix));
                memset(draw, 0, sizeof(draw));
                memcpy(mybuf2.string, string, sizeof(string));
                memcpy(mybuf2.matrix, draw, sizeof(draw));
                if(switch_num == 1) switch_flag = switch_flag * (-1);
                // set new clock time in modified mode
                if(switch_flag == 1) {
                    mybuf2.hourMin = hourMin;
                    mybuf2.output_switch = 128;
                    mybuf2.msgtype = sndtype;
                    mybuf2.main_mode = mode;
                    mybuf2.switch_flag = switch_flag;
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }
                }
                // clock modify mode
                else{
                    printf("Clock modify mode\n");
                    if(switch_num == 3){
                        // increase 1 hour
                        hourMin = (((hourMin/100)+1)%24)*100 + hourMin%100;
                        mybuf2.hourMin = hourMin;
                        mybuf2.msgtype = sndtype;
                        mybuf2.main_mode = mode;
                        mybuf2.switch_flag = switch_flag;
                        if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                            perror("msgsnd error : ");
                            exit(0);
                        }
                    }
                    else if(switch_num == 4){
                        // increase 1 minutes
                        hourMin += 1;
                        if(hourMin % 100 >= 60){
                            hourMin = ((hourMin/100)+1)*100;
                        }
                        if(hourMin / 100 >= 24){
                            hourMin = ((hourMin/100)-24)*100 + hourMin%100;
                        }
                        mybuf2.hourMin = hourMin;
                        mybuf2.msgtype = sndtype;
                        mybuf2.main_mode = mode;
                        mybuf2.switch_flag = switch_flag;
                        if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                            perror("msgsnd error : ");
                            exit(0);
                        }
                    }
                    else if(switch_num == 2){
                        // reset time to local time
                        time(&rawtime);
                        timeinfo = localtime(&rawtime);
                        hourMin = 100*(timeinfo->tm_hour)+timeinfo->tm_min;
                        mybuf2.hourMin = hourMin;
                        mybuf2.msgtype = sndtype;
                        mybuf2.main_mode = mode;
                        mybuf2.switch_flag = switch_flag;
                        if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                            perror("msgsnd error : ");
                            exit(0);
                        }
                    }
                }
                break;

            case 1:
                if(switch_num == 1){
                    // change notation ( 10 -> 8 -> 4 -> 2 -> 10 -> ... )
                    counter_count = convert_to_10(notation[notation_idx], counter_count);
                    notation_idx = (notation_idx+1) % 4;

                    counter_count = convert_to_notation(notation[notation_idx], counter_count);
                    mybuf2.count = counter_count;
                    mybuf2.msgtype = sndtype;
                    mybuf2.main_mode = mode;
                    mybuf2.output_switch = notation_led[notation_idx];
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }
                }
                else if(switch_num == 2){
                    // increase 100(notation)
                    // first : convert to decimal number
                    counter_count = convert_to_10(notation[notation_idx], counter_count);
                    // second : count = count + notation * notation
                    counter_count = counter_count + notation[notation_idx]* notation[notation_idx];
                    // third : convert to designated notation number
                    counter_count = convert_to_notation(notation[notation_idx], counter_count);

                    mybuf2.count = counter_count;
                    mybuf2.msgtype = sndtype;
                    mybuf2.main_mode = mode;
                    mybuf2.output_switch = notation_led[notation_idx];
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }


                }
                else if(switch_num == 3){
                    // increase 10(notation)
                    // first : convert to decimal number
                    counter_count = convert_to_10(notation[notation_idx], counter_count);
                    // second : count = count + notation
                    counter_count = counter_count + notation[notation_idx];
                    // third : convert to designated notation number
                    counter_count = convert_to_notation(notation[notation_idx], counter_count);

                    mybuf2.count = counter_count;
                    mybuf2.msgtype = sndtype;
                    mybuf2.main_mode = mode;
                    mybuf2.output_switch = notation_led[notation_idx];
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }

                }
                else if(switch_num == 4){
                    // increase 1(notation)
                    // first : convert to decimal number
                    counter_count = convert_to_10(notation[notation_idx], counter_count);
                    // second : count = count + 1
                    counter_count = counter_count + 1;
                    // third : convert to designated notation number
                    counter_count = convert_to_notation(notation[notation_idx], counter_count);

                    mybuf2.count = counter_count;
                    mybuf2.msgtype = sndtype;
                    mybuf2.main_mode = mode;
                    mybuf2.output_switch = notation_led[notation_idx];
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }
                }
                break;

            case 2:
                // convert dot matrix "A" to "1", "1" to "A"
                if(switch_num == 56){
                    text_count = (text_count+1) % 10000;
                    dot_idx = (dot_idx+1) % 2;
                    mybuf2.count = text_count;
                    mybuf2.msgtype = sndtype;
                    mybuf2.main_mode = mode;
                    memcpy(mybuf2.matrix,fpga_number[dot_idx],10);
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }
                }

                else if(switch_num == 89){                  // append blank char to string
                    switch_count = 0;
                    before_switch = -1;
                    text_count = (text_count+1)%10000;
                    next_char = ' ';
                    if(strlen(string) < MAX_TEXT){          // append to string
                        string[strlen(string)] = next_char;
                    }
                    else{                                   // if TEXT_LCD is full
                        for(i=0;i<MAX_TEXT-1;i++)
                            string[i] = string[i+1];
                        string[i] = next_char;
                    }

                    mybuf2.count = text_count;
                    mybuf2.msgtype = sndtype;
                    mybuf2.main_mode = mode;
                    memcpy(mybuf2.string, string, sizeof(string));
                    memcpy(mybuf2.matrix,fpga_number[dot_idx],10);
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }
                }
                else if(switch_num == 23){                      // initializing TEXT_LCD
                    text_count = (text_count+1)%10000;
                    mybuf2.count = text_count;
                    memset(string, 0, sizeof(string));
                    mybuf2.msgtype = sndtype;
                    mybuf2.main_mode = mode;
                    memcpy(mybuf2.string, string, sizeof(string));
                    memcpy(mybuf2.matrix,fpga_number[dot_idx],10);
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }          
                }
                else if(switch_num >=1 && switch_num <=9){
                    text_count = (text_count+1)%10000;
                    if(dot_idx == 0){                               // Character mode
                        if(before_switch == 0){                     // when press switch button first time
                            before_switch = switch_num;
                            next_char = text_lcd_A[switch_num][switch_count];
                            string[0] = next_char;
                        }
                        else if(before_switch == switch_num){       // when press the same switch button
                            switch_count = (switch_count +1) % 3;
                            next_char = text_lcd_A[switch_num][switch_count];
                            string[strlen(string)-1] = next_char;
                        }
                        else{                                       // when press another switch button
                            switch_count = 0;
                            before_switch = switch_num;
                            next_char = text_lcd_A[switch_num][switch_count];
                            if(strlen(string) < MAX_TEXT){
                                string[strlen(string)] = next_char;
                            }
                            else{
                                for(i=0;i<MAX_TEXT-1;i++){
                                    string[i] = string[i+1];
                                }
                                string[i] = next_char;
                            }
                        }
                    }
                    else{                                               // Integer mode
                        next_char = text_lcd_1[switch_num];

                        if(strlen(string) < MAX_TEXT){                  // append to the string
                            string[strlen(string)] = next_char;
                        }
                        else{                                           // if TEXT_LCD is full
                            for(i=0;i<MAX_TEXT-1;i++){                  // move TEXT_LCD to the left (1 block)
                                string[i] = string[i+1];
                            }
                            string[i] = next_char;
                        }
                    }
                    mybuf2.count = text_count;
                    mybuf2.msgtype = sndtype;
                    mybuf2.main_mode = mode;
                    memcpy(mybuf2.string, string, sizeof(string));
                    memcpy(mybuf2.matrix,fpga_number[dot_idx],10);
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }     
                }
                break;

            case 3:                                        // mode 4
                draw_count = (draw_count+1) % 10000;
                mybuf2.count = draw_count;
                mybuf2.msgtype = sndtype;
                mybuf2.main_mode = mode;
                if(switch_num == 2){                        // move upward
                    matrix[x][6-y] = draw_matrix[x][6-y]; 
                    if(x != 0){                             // only x >= 0
                        x -= 1;
                        convert_draw();
                        mybuf2.output_switch = 0;
                        memset(string, 0, sizeof(string));
                        memcpy(mybuf2.string, string, sizeof(string));
                        memcpy(mybuf2.matrix, draw, sizeof(draw));
                        if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                            perror("msgsnd error : ");
                            exit(0);
                        }                
                    }

                }
                else if(switch_num == 4){                   // move left
                    matrix[x][6-y] = draw_matrix[x][6-y];
                    if(y != 0){                             // only y >= 0
                        y -= 1;
                    }
                    convert_draw();
                    mybuf2.output_switch = 0;
                    memset(string, 0, sizeof(string));
                    memcpy(mybuf2.string, string, sizeof(string));
                    memcpy(mybuf2.matrix, draw, sizeof(draw));
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }
                }
                else if(switch_num == 6){                   // move right
                    matrix[x][6-y] = draw_matrix[x][6-y];   
                    if(y != MAX_COL -1){                    // only y <= MAX_COL 
                        y += 1;
                    }
                    convert_draw();
                    mybuf2.output_switch = 0;
                    memset(string, 0, sizeof(string));
                    memcpy(mybuf2.string, string, sizeof(string));
                    memcpy(mybuf2.matrix, draw, sizeof(draw));
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }
                }
                else if(switch_num == 8){                   // move downside
                    matrix[x][6-y] = draw_matrix[x][6-y];
                    if(x != MAX_ROW -1){                    // only y <= MAX_ROW
                        x += 1;
                    }
                    convert_draw();
                    mybuf2.output_switch = 0;
                    memset(string, 0, sizeof(string));
                    memcpy(mybuf2.string, string, sizeof(string));
                    memcpy(mybuf2.matrix, draw, sizeof(draw));
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }
                }
                else if(switch_num == 5){                   // set (x,y)
                    if(draw_matrix[x][6-y] == 0){
                        matrix[x][6-y] = 1;
                        draw_matrix[x][6-y] = 1;
                    }
                    else{
                        matrix[x][6-y] = 0;
                        draw_matrix[x][6-y] = 0;
                    }
                    convert_draw();
                    mybuf2.output_switch = 0;
                    memset(string, 0, sizeof(string));
                    memcpy(mybuf2.string, string, sizeof(string));
                    memcpy(mybuf2.matrix, draw, sizeof(draw));
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }

                }
                else if(switch_num == 1){                   // clear dot matrix, reset (x,y)
                    x=0;y=0;
                    matrix_empty();
                    mybuf2.output_switch = 0;
                    memset(string, 0,sizeof(string));
                    memcpy(mybuf2.string, string, sizeof(string));
                    memcpy(mybuf2.matrix, draw, sizeof(draw));
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }                 


                }
                else if(switch_num == 3){                   // hide cursor
                    cursor_flag = cursor_flag * (-1);
                    matrix[x][6-y] = draw_matrix[x][6-y];
                    convert_draw();
                    mybuf2.output_switch = 0;
                    memset(string, 0, sizeof(string));
                    memcpy(mybuf2.string, string, sizeof(string));
                    memcpy(mybuf2.matrix, draw, sizeof(draw));
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }


                }
                else if(switch_num == 7){                   // only clear dot matrix, do not reset (x,y)
                    matrix_empty();
                    mybuf2.output_switch = 0;
                    memset(string, 0,sizeof(string));
                    memcpy(mybuf2.string, string, sizeof(string));
                    memcpy(mybuf2.matrix, draw, sizeof(draw));
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }

                }
                else if(switch_num == 9){                   // reverse matrix
                    matrix_reversed();
                    mybuf2.output_switch = 0;
                    memset(string, 0,sizeof(string));
                    memcpy(mybuf2.string, string, sizeof(string));
                    memcpy(mybuf2.matrix, draw, sizeof(draw));
                    if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                        perror("msgsnd error : ");
                        exit(0);
                    }

                }
                break;

            case 4:                                 // mode 5
                printf("Mode : 4\n");
                mybuf2.count = score;
                mybuf2.msgtype = sndtype;
                mybuf2.main_mode = mode;
                mybuf2.output_switch = 0;
                memcpy(mybuf2.string, manual, sizeof(manual));

                if(switch_num == 5){                // press switch button 5, then start game
                    start_flag = 1;
                }
                if(start_flag == 1){           // start game


                    if(switch_num == 4){            // move left 
                        if(ddong_y < MAX_COL -1){
                            ddong_y = ddong_y + 1;
                        }

                    }
                    else if(switch_num == 6){       // move right
                        if(ddong_y > 0) {
                            ddong_y = ddong_y - 1;
                        }

                    }
                    if(ddong_matrix[ddong_x][ddong_y] == 1)         // if my position == obstacle position, then game stop
                    {
                        start_flag = 0;
                        memset(string, 0, sizeof(string));
                        convert_ddong();
                        memcpy(mybuf2.matrix, ddong, sizeof(ddong));
                        if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                            perror("msgsnd error : ");
                            exit(0);
                        }
                    }
                    else{
                        ddong_matrix[ddong_x][ddong_y] = 1;
                        convert_ddong();
                        ddong_matrix[ddong_x][ddong_y] = 0;
                        memcpy(mybuf2.matrix, ddong, sizeof(ddong));
                        if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                            perror("msgsnd error : ");
                            exit(0);
                        }
                    }

                }
                break;
        }

    }

}
void *input_msgtype2(){
    int rcvtype = 2;
    int sndtype = 4;
    int i;

    msgbuf mybuf2;
    memset(mybuf2.matrix, 0x00,sizeof(mybuf2.matrix));
    memset(mybuf2.string, 0, sizeof(mybuf2.string));

    memcpy(mybuf2.string, "",0);

    while(1){
        // receive message type 2
        if(msgrcv(input_key_id, (void*)&mybuf2, sizeof(msgbuf), rcvtype, 0)==-1){
            perror("msgrcv error : ");
            exit(0);
        }
        if(mybuf2.key_num == 115) mode = (mode+1) % mode_num;                   // mode + 1
        else if(mybuf2.key_num == 114) mode = (mode+mode_num-1) % mode_num;     // mode - 1
        else if(mybuf2.key_num == 158){                                         // program exit
            show_fnd(0);
            show_led(0);
            memset(string, 0, sizeof(string));
            show_text(string);
            memset(draw, 0, sizeof(draw));
            show_dot_matrix(draw);
            kill(input_pid,9);
            kill(output_pid,9);
            msgctl(input_key_id, IPC_RMID,0);
            msgctl(output_key_id, IPC_RMID, 0);
            exit(0);
        }
        printf("Main mode : %d\n",mode);
        switch(mode)
        {
            case 0:         // mode 1
                score      = 0;
                start_flag = 0;
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                hourMin = 100*(timeinfo->tm_hour)+timeinfo->tm_min;         // calculate localtime
                mybuf2.hourMin = hourMin;
                mybuf2.output_switch = 128;
                //memset(string, 0,sizeof(string));
                memset(mybuf2.string, 0 ,sizeof(string));
                memcpy(mybuf2.matrix, fpga_number[2], 10);
                memcpy(mybuf2.string, string,sizeof(string));
                break;

            case 1:         // mode 2
                counter_count = 0;
                mybuf2.count = counter_count;
                memset(string, 0,sizeof(string));
                memcpy(mybuf2.matrix, fpga_number[2],10);
                memcpy(mybuf2.string, string, sizeof(string));
                mybuf2.output_switch = 64;
                break;

            case 2:         // mode 3
                text_count    = 0;
                dot_idx       = 0;
                before_switch = 0;
                memset(string, 0,sizeof(string));
                mybuf2.count = text_count;
                memcpy(mybuf2.matrix, fpga_number[dot_idx],10);
                memcpy(mybuf2.string, string, sizeof(string));
                mybuf2.output_switch = 0;
                break;

            case 3:         // mode 4
                score = 0;
                start_flag = 0;
                draw_count = 0;
                x          = 0;
                y          = 0;
                mybuf2.count = draw_count;
                mybuf2.output_switch = 0;
                memset(string, 0,sizeof(string));
                memset(draw, 0, sizeof(draw));
                memset(draw_matrix, 0, sizeof(draw_matrix));
                memset(matrix, 0, sizeof(matrix));
                memcpy(mybuf2.matrix, draw,sizeof(draw));
                memcpy(mybuf2.string, string, sizeof(string));
                break;
            
            case 4:
                score                = 0;
                start_flag           = 0;
                mybuf2.count         = 0;
                mybuf2.output_switch = 0;
                memset(ddong_matrix, 0, sizeof(ddong_matrix));
                convert_ddong();

                memcpy(mybuf2.string, manual, sizeof(manual));
                memcpy(mybuf2.matrix, ddong, sizeof(ddong));
                break;
        }
        mybuf2.msgtype = sndtype;
        mybuf2.main_mode = mode;
        if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
            perror("msgsnd error : ");
            exit(0);
        }

    }
}

void *switch_3_4(){
    msgbuf mybuf2;
    while(1){
        if(mode == 0 && switch_flag == -1){     // mode 1 clock modify mode
            usleep(1000);                       // prevent message type 3 being sent before message type 4
            kkk = 1;
            mybuf2.msgtype = 3;
            mybuf2.main_mode = mode;
            mybuf2.output_switch = output_switch;
            memset(string, 0, sizeof(string));
            memcpy(mybuf2.string,string, sizeof(string));
            memcpy(mybuf2.matrix, draw, sizeof(draw));
            if(output_switch == 32)
                output_switch = 16;
            else
                output_switch = 32;

            mybuf2.hourMin = hourMin;
            if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                perror("msgsnd error : ");
                exit(0);
            }
            sleep(1);
        }
        
        else if(mode == 0 && switch_flag == 1){     // mode 1 change time every one minute
            usleep(1000);
            mybuf2.msgtype = 3;
            mybuf2.main_mode = mode;
            mybuf2.hourMin = hourMin;
            mybuf2.output_switch = 128;
            memset(string, 0, sizeof(string));
            memcpy(mybuf2.string, string, sizeof(string));
            if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                perror("msgsnd error : ");
                exit(0);
            }
            if(kkk == 60){                          // increase 1 minute
                hourMin += 1;
                if(hourMin % 100 >= 60){
                    hourMin = ((hourMin/100)+1)*100;
                }
                if(hourMin / 100 >= 24){
                    hourMin = ((hourMin/100)-24)*100 + hourMin%100;
                }
                kkk = 1;
            }     
            kkk++;         
            sleep(1);
        }
        else if(mode == 3 && cursor_flag == 1){

            usleep(1000);
            mybuf2.msgtype = 3;
            mybuf2.main_mode = mode;
            mybuf2.count = draw_count;
            mybuf2.output_switch = 0;
            memset(string, 0,sizeof(string));
             
            if(draw_matrix[x][6-y] == 0 && cursor_flag == 1){           // if (x,y)=0 and when cursor appear
                if(matrix[x][6-y] == 0){
                    matrix[x][6-y] = 1;
                }
                else{
                    matrix[x][6-y] = 0;
                }
                convert_tmp();                                          // blink (x,y)
                memcpy(mybuf2.matrix, tmp, sizeof(tmp));  
            }
            else if(draw_matrix[x][6-y] != 0){                          // when (x,y)=1, not blink
                convert_draw();
                memcpy(mybuf2.matrix, draw, sizeof(draw));
            }
            memcpy(mybuf2.string, string, sizeof(string));

            if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                perror("msgsnd error : ");
                exit(0);
            }
            sleep(1);
            
        }
        else if(mode == 4 && start_flag == 1){          // when game start, move dot matrix downside and blink my position
            usleep(1000);
            level_flag = level_flag * (-1);
            mybuf2.msgtype = 3;
            mybuf2.main_mode = mode;
            mybuf2.output_switch = 0;
            mybuf2.count = score;
            create_random_line();
            shift_down();
            if(ddong_matrix[ddong_x][ddong_y] == 0){
                ddong_matrix[ddong_x][ddong_y] = 1;
                convert_ddong();
                ddong_matrix[ddong_x][ddong_y] = 0;
            }
            else{                                       // if my position == obstacle position
                start_flag = 0;
                convert_ddong();
            }
            memcpy(mybuf2.string, manual, sizeof(manual));
            memcpy(mybuf2.matrix, ddong, sizeof(ddong));

            if(msgsnd(output_key_id, (void *)&mybuf2, sizeof(mybuf2), IPC_NOWAIT)==-1){
                perror("msgsnd error : ");
                exit(0);
            }
            sleep(1);
            score++;
        }

    }
}
int main(int argc, char **argv){
    pthread_t p_thread[4];
    int thr_id;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    hourMin = 100*(timeinfo -> tm_hour)+timeinfo -> tm_min;

    input_pid = fork();
    if(input_pid < 0){
        printf("Input fork failed\n");
    }
    // Input Process
    else if(input_pid == 0){
        input_main();
    }

    else{
        output_pid = fork();

        if(output_pid < 0){
            printf("Output fork failed\n");
        }
        // Output Processs
        else if(output_pid == 0){
            output_main();
        }
        // Main Process
        else{
            input_key_id = msgget(input_q,IPC_CREAT|0666);
            output_key_id = msgget((key_t)output_q, IPC_CREAT|0666);
                    
            if(input_key_id < 0 || output_key_id < 0){
                perror("msgget error : ");
                exit(0);
            }

            /* Initialize default mode and send message */
            mybuf.msgtype = 4;
            mybuf.main_mode = 0;
            mybuf.hourMin = hourMin;
            mybuf.output_switch = 128;
            memset(string, 0, sizeof(string));
            memcpy(mybuf.string, string,sizeof(string));
            memset(draw_matrix, 0, sizeof(draw_matrix));
            memset(draw, 0, sizeof(draw));
            memset(matrix, 0 ,sizeof(matrix));
            memcpy(mybuf.matrix, draw, sizeof(draw));
            if(msgsnd(output_key_id, (void *)&mybuf, sizeof(mybuf), IPC_NOWAIT)==-1){
                perror("msgsnd error : ");
                exit(0);
            }
            //////////////////////////////////////////////

            // Create thread (send message to output when main receive message type 1)
            thr_id = pthread_create(&p_thread[0],NULL,input_msgtype1, NULL);
            if(thr_id < 0){
                perror("thread create error : ");
                exit(0);
            }
            // Create thread (send message to output when main receive message type 2)
            thr_id = pthread_create(&p_thread[1],NULL,input_msgtype2, NULL);
            if(thr_id < 0){
                perror("thread create error : ");
                exit(0);
            }
            // Create thread (send message to output every second)
            thr_id = pthread_create(&p_thread[2],NULL,switch_3_4, NULL);
            if(thr_id < 0){
                perror("thread create error : ");
                exit(0);
            }
            pthread_join(p_thread[0], (void **)NULL);
            pthread_join(p_thread[1], (void **)NULL);
            pthread_join(p_thread[2], (void **)NULL);
        
           }
    }
    // wait input, output process
    wait(NULL);
    wait(NULL);

	return 0;
}
