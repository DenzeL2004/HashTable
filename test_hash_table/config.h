#ifndef _CONFIG_H_
#define _CONFIG_H_

const char Separate_symbol = ',';

const char *Name_result_file = "temp/result_distribution.csv";

const char *Test_input_file  = "resource/test.txt";

const size_t Hash_table_capacity = 109ul;

const size_t Step_print = 1;

const size_t Count_query = 1000000ul;

//#define TEST_DISTRIBUTION

#define TEST_FIND

//#define RANDOM_ELEMENT_FIND

#endif