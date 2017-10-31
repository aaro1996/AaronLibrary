#ifndef _AARON_VECTOR_H_
#define _AARON_VECTOR_H_

typedef struct 
{
	void** buffer;
	int vector_size;
	int (*_comparison_)(void* item_1, void* item_2);  // returns negative if 1 < 2, 0 if 1 = 2, positive if 1 > 2
} aaron_vector;


#endif