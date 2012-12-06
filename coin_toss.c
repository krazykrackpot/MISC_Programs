/*
 * krazykrackpot@gmail.com
 */


#include <stdio.h>


void
flip_a_coin (void)
{
    unsigned int toss = 0;

    /* Initialize the random seed 
     * Use the time as it is guaranteed to 
     * be different each time the function is 
     * evoked
     */
    srand(time(NULL));
   
    toss = rand() % 2;

    if (toss) {
        printf("Heads \n");
    } else {
        printf("Tails \n");    
    }
}



int main (int argc, char *argv[])
{
    unsigned int i = 0;
    unsigned int num_of_tosses = 0;
  
    while (scanf("%u", &num_of_tosses) != EOF) {
        for (i = 0; i < num_of_tosses; i++) {
            flip_a_coin();
            /* Introduced a sleep because without it the time
             * doesn't change sufficiently for srand to be 
             * seeded differently
             * A sleep will allow the time to change and srand
             * to be seeded differently
             */
            sleep(5);
        }
    }
}
