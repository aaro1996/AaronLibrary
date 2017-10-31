#ifndef _AARON_TEST_H_
#define _AARON_TEST_H_

#include "aaron_lib.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv);
int test_print_hashmap(aaron_hashmap* hashmap);
int test_equality_check(void* a, void* b);
uint test_hash_function(void* a);
int hashmap_test();

#endif