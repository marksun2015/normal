#include "gtest/gtest.h"
#include "numberchains.h"

TEST(get_str_digits, test1) {
    //std::cout<<"test get_str_digits:123456-->6"<<std::endl;
    EXPECT_EQ (NumberChains::get_str_digits(123456),6); //通過
    //std::cout<<"test get_str_digits:1234567-->7"<<std::endl;
    EXPECT_EQ (NumberChains::get_str_digits(1234567),7); //通過
    //std::cout<<"test get_str_digits:1234567-->1 failed"<<std::endl;
    //EXPECT_EQ (NumberChains::get_str_digits(1234567),1); //failed
}

TEST(get_str_order_integer, test2) {
    unsigned int arr[] = {1,2,5,6,3,4};
    EXPECT_EQ (NumberChains::get_str_order_integer(arr, 6, ASCENDING),123456); //通過
    EXPECT_EQ (NumberChains::get_str_order_integer(arr, 6, DESCENDING),654321); //通過
}

TEST(integer_mapping_to_array, test3) {
    unsigned int *arr = new unsigned int[3];
    NumberChains::mapping_int_to_array(arr, 456, 3);
    EXPECT_EQ(*arr, 4); 
    EXPECT_EQ(*(arr+1), 5); 
    EXPECT_EQ(*(arr+2), 6); 
    delete arr;
}
