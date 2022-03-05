#ifndef NUMBERCHAINS_H
#define NUMBERCHAINS_H

#include <algorithm>
#include <math.h>
#include <string>

enum {
    ASCENDING,
    DESCENDING
};

class NumberChains {
    NumberChains(unsigned int input_integer);

    public:
        static void integer_mapping_to_array(int *arr, unsigned int input_integer, int len);
        static unsigned int get_str_order_integer(unsigned int *input_str, unsigned int len, int order_type);
        static unsigned int get_str_digits(unsigned int input_integer);
};

#endif //NUMBERCHAINS_H
