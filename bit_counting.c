/* 
 * krazykrackpot@gmail.com
 * 
 *
 */

#include <stdio.h>

/*
 *  The technique is to clear the last set bit 
 *  and then move to the next set LSB.
 *
 *     E.g 7 (0111)
 *     1st pass  v(7) & (v - 1)(6) = 0111
 *                                   0110
 *                                  ======
 *                                   0110 (6)
 *
 *     2nd Pass v(6) * (v-1)(5)  =   0110
 *                                   0101
 *                                  ========
 *                                   0100  (4)
 *
 *     3rd Pass v(4) * (v-1)(3) =    0100
 *                                   0011
 *                                  ========
 *                                   0000 (0)
 *
 *     bit_count = 3
 * 
 *     E.g 8 (1000)
 *     1st pass v = 1000 & 0111  = 0 bit_count = 1
 *
 *     E.g 6 (0110)
 *     1st pass v = 0110 & 0101  = 0100 bit_count = 1
 *     2nd pass v = 0100 & 0011  = 0000 bit_count = 2
 *          
 */

unsigned int
count_set_bits (unsigned int v)
{
    unsigned int bit_count = 0;
 
    for (bit_count = 0; v; bit_count++) {
        /* clear the least significant bit set */
        v &= (v - 1);
    }
    return bit_count;
}


/*
 *  Brute force 
 *  iterate over all the bits
 *  
 *   E.g 8 (1000)
 *   1st Pass  v = 0100 & 1 = 0 bit_count = 0
 *   2nd Pass  v = 0010 & 1 = 0 bit_count = 0
 *   3rd Pass  v = 0001 & 1 = 1 bit_count = 1
 *   4th pass  v = 0000 - end of loop bit_count = 1
 *
 */
unsigned int 
count_set_bits_2 (unsigned int v)
{
    unsigned int bit_count = 0;
 
    for (bit_count = 0; v; v >>= 1) {
        bit_count += (v & 1);
    }
    return bit_count;
}


int main (int argc, char *argv[])
{
    unsigned int v = 0;

    while (scanf("%u", &v) != EOF) {
        printf("Method 1: Number %u: %u bits set\n", v, count_set_bits(v));
        printf("Method 2: Number %u: %u bits set\n", v, count_set_bits_2(v));
    }
    return 0;
}

