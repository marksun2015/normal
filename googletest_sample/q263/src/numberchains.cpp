#include "numberchains.h"

NumberChains::NumberChains(unsigned int input_integer)
{
    unsigned int len = 0; 
    unsigned int descending_number = 0;
    unsigned int ascending_number = 0;
    int chain_len = 0, difference = 0;

    std::cout << "Original number was " << input_integer << std::endl;
    
    while (1){
        len = get_str_digits(input_integer);
        unsigned int *buffer = new unsigned int[len];

        mapping_int_to_array(buffer, input_integer, len);
        descending_number = get_str_order_integer(buffer, len, DESCENDING);
        ascending_number = get_str_order_integer(buffer, len, ASCENDING);
        difference = descending_number - ascending_number;
        std::cout << descending_number << " - " << ascending_number << " = " << difference << std::endl;
        delete buffer;
        chain_len++;

        if (input_integer != difference)
            input_integer = difference;
        else 
            break;
    }
    std::cout << "Chain length " << chain_len << std::endl;
}

void NumberChains::mapping_int_to_array(unsigned int *arr, unsigned int input_integer, int len)
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
