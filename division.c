/*
 *  Problem from Algorithm Design Manual
 *  by Skiena
 * 
 *  Problem: 1-28
 *
 */


#include <stdio.h>

void
division_by_subtraction (unsigned int num,
                         unsigned int div)
{
    unsigned int quotient = 0;

    printf("Dividing %u by %u", num, div);
    while (num >= div) {
        num = num - div;
        quotient++;
    }
    printf(" Quotient %u Remainder %u\n", quotient, num);
}


unsigned int
divide_recursion (unsigned int num,
                  unsigned int div)
{
    if (num < div) {
        return 0;
    } else {
        return (1 + divide_recursion(num-div, div));
    }
}


int main (int argc, char *argv[])
{
     unsigned int num = 0;
     unsigned int div = 0;

     unsigned int recursion_result = 0;

     while (scanf("%d %d", &num, &div) != EOF) {
        if (div == 0) {
            printf("You go do the divide-by-zero shithead\n");
            break;
        } else if (num < div) {
            printf("this is for integer division divider cannot"
                   " be greater than the number itself\n");
            break;
        }
        division_by_subtraction(num, div);
        /* recursion result - hmm we lose the remainder 
           we can retrieve it but it is trivial excercise */
        recursion_result = divide_recursion(num, div);
        printf("Dividing %u by %u Quotient %u", num, div, recursion_result);
     }
     return (0);
}
