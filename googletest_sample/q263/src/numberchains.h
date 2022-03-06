#ifndef NUMBERCHAINS_H
#define NUMBERCHAINS_H

//#include <functional>
#include <algorithm>
#include <math.h>
#include <string>
#include <iostream>
#include <memory>

enum {
    ASCENDING,
    DESCENDING
};

class NumberChains {
    public:
        NumberChains(unsigned int input_integer);
        static void mapping_int_to_array(unsigned int *arr, unsigned int input_integer, int len);
        static unsigned int get_str_order_integer(unsigned int *input_str, unsigned int len, int order_type);
        static unsigned int get_str_digits(unsigned int input_integer);
};

#endif //NUMBERCHAINS_H
