#include <avr/io.h>

int main(void)
{
    int32_t array[] = {1, 2, 3, 4};
    uint8_t array_length = 4;
    for (int32_t *curr_ptr = array + 1; curr_ptr < array + array_length; curr_ptr++)
    {
	    int32_t key = *curr_ptr;
	    int32_t *prev_ptr = curr_ptr - 1;
	    while (*prev_ptr > key && prev_ptr >= array && prev_ptr < array + array_length)
	    {
		    *(prev_ptr + 1) = *prev_ptr;
		    prev_ptr--;
	    }
	    *(prev_ptr + 1) = key;
    }
	while (1)
	{
		
	}
}

