#include "numberchains.h"

NumberChains::NumberChains(unsigned int input_integer)
{


}

void NumberChains::integer_mapping_to_array(int *arr, unsigned int input_integer, int len)
{
    int i = 0;
    for (i = (len-1); i >= 0; i--) {
        *(arr+i) = input_integer % 10;
        input_integer = input_integer / 10;
    }
}

unsigned int NumberChains::get_str_order_integer(unsigned int *input_str, unsigned int len, int order_type)
{
    int i = 0;
    int count = 0;
    int output = 0;

    if (order_type == ASCENDING)
        std::sort(input_str, input_str + len, std::less<int>());
    if (order_type == DESCENDING)
        std::sort(input_str, input_str + len, std::greater<int>());

    for (i = (len-1); i >= 0; i--) {
        output = output + *(input_str + i)*(pow(10, count++));
    }

    return output;
}

unsigned int NumberChains::get_str_digits(unsigned int input_integer) 
{
    int len = 0;

    while(input_integer > 0){
        input_integer = input_integer / 10;
        len = len+1;
    }

    return len; 
}
