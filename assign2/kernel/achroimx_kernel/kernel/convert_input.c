#include <linux/kernel.h>

asmlinkage unsigned int convert_input(unsigned int interval, unsigned int num, unsigned int option) {
    unsigned int result;
    unsigned int start_pos;
    unsigned int start_value;

    if(option >= 1000){
        start_pos = 1;
        start_value = option/1000;
    }
    else if(option >= 100){
        start_pos = 2;
        start_value = option/100;
    }
    else if(option >= 10){
        start_pos = 3;
        start_value = option/10;
    }
    else{
        start_pos = 4;
        start_value = option;
    }
    result = (start_pos << 24) | (start_value << 16) | (interval << 8) | num;
	return result;
}
