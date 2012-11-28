#include <stdio.h>
#include <stdbool.h>

#define TRUE 1
#define FALSE 0


bool
is_even(unsigned int num) 
{ 
    if (!(num%2)) {
        return TRUE;
    } else {
        return FALSE;
    }
} 

void
even_action (unsigned int *num)
{
    *num = *num/2;
}

void 
odd_action (unsigned int *num)
{
    *num = ((*num * 3) + 1);
}


void 
perform_action (unsigned int *num)
{
    if (is_even(*num)) {
        even_action(num);
    } else {
        odd_action(num);
    }
#ifdef debug
    printf("%u \n", *num);
#endif
}

unsigned int
generate_sequence (unsigned int num)
{
    /* Initialize to 1 to take into account the 
       endpoint i.e include the number itself */
    unsigned int count = 1;
    while (num > 1) {
        count++;
        perform_action(&num);
    }
    return count;
}

void swap (unsigned int *num1,
           unsigned int *num2)
{
     unsigned int temp = *num1;
     *num1 = *num2;
     *num2 = temp;
}


unsigned int 
get_sequence_difference (unsigned int num1,
                         unsigned int num2)
{
    unsigned int i = 0;
    unsigned int count = 0;
    unsigned int high_count = 0;

    /* if the user goofed-up clear his mess */
    if (num1 > num2) {
        swap (&num1, &num2);
    }

    /* get the sequence counts */
    for(i = num1; i < num2; i++) {
        count = generate_sequence(i);
        if (count >= high_count) {
            high_count = count;
        }        
    }
    return high_count;
}


int main (int argc, char *argv[])
{
    unsigned int num1  = 0;
    unsigned int num2  = 0;

    while (scanf("%u %u", &num1, &num2) != EOF) {
        printf("%u %u %u \n", num1, num2, 
               get_sequence_difference(num1, num2));
    }
    return (0);
}
