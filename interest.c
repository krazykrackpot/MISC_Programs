/*
 *-------------------------------------------------------------------------
 * FINANCIAL CALCULATIONS 
 * interest.c - Sort FMD attributes
 *
 * to compile :  link with math library
 *    %gcc -o interest interest.c -lm  
 * 
 * October 2013, Aditya KUMAR (aditya.kr.jha@gmail.com)
 *
 * Copyright (c) 2010-2013 by Aditya KUMAR
 * All rights reserved.
 *
 *
 *-------------------------------------------------------------------------
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>   /* for pow(x,y) */

#define uint32_t unsigned int

#define FIN_DEBUG(fmt, ...) \
    do {\
        if (fin_debug_enabled) { \
            printf(fmt, ##__VA_ARGS__); \
        } \
    } while(0)

uint32_t fin_debug_enabled = 0;

typedef enum fin_cal_operations_t_ {
    FIN_CAL_INVALID = 0,
    FIN_CAL_SIMPLE_INTEREST, 
    FIN_CAL_COMPOUND_INTEREST,
    FIN_CAL_PV,
    FIN_CAL_FV,
    FIN_CAL_DISCOUNT_RATE,
    FIN_CAL_CONVERTED_DISCOUNT_RATE,
    FIN_CAL_PRINCIPLE_IN_PERIOD,
    FIN_CAL_INTEREST_IN_PERIOD,
    FIN_CAL_PERIODIC_PMT,
    FIN_CAL_PV_ANNUITY,
    FIN_CAL_AMMORTIZATION_SCHEDULE, 
    FIN_CAL_MAX
} fin_cal_operations_t;


double
fin_cal_simple_interest (double principle, 
                         double rate, double duration)
{
    /* SI = PTR/100 */
    return (principle * rate * duration)/100;
}
                         

/* PV of a future sum */
double
fin_cal_pv_of_fv (double fv, double discount_rate,
                  double num_period)
{
    /* convert from percentage */
    discount_rate = discount_rate/100;

    /* PV = FV/(1+i)pow(n) */
    return (fv/pow((1+ discount_rate), num_period));
}


/* FV of a present sum */
double
fin_cal_fv_of_pv (double pv, double discount_rate,
                  double num_period)
{
    double fv = 0;
    /* convert from percentage */
    discount_rate = discount_rate/100;

    /* FV = PV * (1 + i) pow (n) */
    fv = (pv * pow((1 + discount_rate), num_period));
    FIN_DEBUG("FV (PV: %lf Rate: %lf Period: %lf) : %lf \n",
           pv, discount_rate, num_period, fv); 
    return fv;
}


/* Compound interest rate */
double
fin_cal_discount_rate (double fv, double pv, double period)
{
    /* i = [(FV/PV)pow(1/n) - 1] */

    return (pow((fv/pv), (1/period)) - 1);
}


/* 
 * convert interest rate from one 
 * compounding basis to another 
 *
 * r2 = [(1+ r1/n1) pow (n1/n2) - 1] * n2
 * 
 * where r1 is interest rate with compounding frequency n1
 *       r2 is interest rate with compounding frequency n2
 *
 * IF the compounding period is annual , n2 = 1 ==> Effective Interest Rate
 *                                       Annual Percentage Rate (APR) 
 * 
 * WIKIPEDIA: Future Value
 * 
 */
double
fin_cal_converted_discount_rate (double r1, double n1, double n2)
{
    /* convert from percentage */
    r1 = r1/100;
    return (((pow((1 + (r1/n1)), (n1/n2)) - 1) * (n2)) * 100);
}


/*
 *
 */
double
fin_cal_effective_discount_rate (double r1, double n1, double n2)
{
    /* (1+r1) = (1 + r2) pow (n2/n1) */
    /* r1 = [ (1 + r2) pow (n2/n1) ] - 1 */

    r1 = r1/100;
    return  ((pow((1+r1), (n1/n2)) - 1) * 100);

}


double
fin_cal_periodic_pmt (double principle, double rate,
                    double period)
{

    /* Loans are normally an annuity
     * PV(annuity) = (C/r)(1- (1/(1+r)pow(N)
     * 
     *  C =  PV * r / (1 - (1/(1+r)powN) 
     */

    rate = rate/100;

    return (principle * rate)/(1-(1/pow((1 + rate), period)));

}


double
fin_cal_pv_annuity (double pmt, double rate, double period)
{
    /* PV = c/r (1 - (1/(1+r)powN)) */

    rate = rate/100;

    return ((pmt/rate) * (1 - (1/pow((1 + rate), period))));
}


double
fin_cal_pv_cumulate_pmt (double pmt,
                         double rate, uint32_t period)
{
    /* PV of payment in period N  = (PMT) *(1+r)pow(n) */
    /* cumulative payment = sum of all payments */

    uint32_t i = 0;
    double cumulative_pmt = 0;

    for (i = 0; i < period; i++) {
        cumulative_pmt += (pmt * pow((1 + rate/100), period));
    }
    FIN_DEBUG("Cumulative Payment is %lf \n", cumulative_pmt);
    return cumulative_pmt;
}


/* 
 * Calculate Interest and Principle components 
 * of the payment amounts between duration n1 and n2
 */
double
fin_cal_principle_in_period (double principle, double rate, double pmt,
                             uint32_t n1, uint32_t n2, uint32_t period)
{
    /* 
     * calculate PV(N1) of loan 
     * calculate PV(N2) of loan
     * Principle paid in period (N1, N2 ) = PV(N1) - PV(N2)
     */
   
    /* PV(N1) is FV(principle) at rate */
    double pv_n1 = 0;
    double pv_n2 = 0;

    /* pv_n1 is the PV of annuity of PMT for (period - n1) duration */
    pv_n1 = fin_cal_pv_annuity(pmt, rate, period - n1);
    FIN_DEBUG("Value of Loan at period %u : %lf \n", n1, pv_n1);
    
    /* similarly for PV(N2) */
    pv_n2 = fin_cal_pv_annuity(pmt, rate, period - n2);
    FIN_DEBUG("Value of Loan at period %u : %lf \n", n2, pv_n2);
  
    return (pv_n1 - pv_n2);
}


double
fin_cal_interest_in_period (double principle, double rate, double pmt, 
                            uint32_t n1, uint32_t n2, uint32_t period)
{
    double total_payment = 0;
    double principle_component = 0;    

    total_payment = pmt * (n2 - n1);

    principle_component = fin_cal_principle_in_period(principle, rate, pmt, 
                                                      n1, n2, period);
   
    FIN_DEBUG("Total payment (period %u - %u) %lf, principle component: %lf \n",
            n1, n2, total_payment, principle_component);
 
    return (total_payment - principle_component);
}

void
fin_display_ammortization_schedule (double principle, double rate, double pmt,
                                    uint32_t period)
{   
    uint32_t i = 0;

    for (i = 0; i < period; i++) {
        printf("Period %u : Principle :%lf Interest %lf \n",
               i+1, 
               fin_cal_principle_in_period(principle, rate, pmt, i, i+1, period),
               fin_cal_interest_in_period(principle, rate, pmt, i, i+1, period));
    }
}


void
fin_display_menu (void)
{
    printf("###################################################### \n");
    printf("#     FINANCE CHEATSHEET                             # \n");
    printf("###################################################### \n");
    printf("[1] Calculate Simple Interest \n");
    printf("[2] Calculate Compound Interest \n");
    printf("[3] Calculate Present Value (PV) \n");
    printf("[4] Calculate Future Value (FV) \n");
    printf("[5] Calculate Discount Rate \n");
    printf("[6] Calculate (Period/frequency) Converted Interest Rate \n");
    printf("[7] Calculate Principle between period N1 & N2 \n");
    printf("[8] Calculate Interest between period N1 & N2 \n");
    printf("[9] Calculate Periodic Payment (Annuity) \n");
    printf("[10] Calculate PV of Annuity \n");
    printf("[11] Calculate Ammortization Schedule \n");
}


void
fin_print_help_cal_simple_interest (void)
{
    printf("###################################################### \n");
    printf("#    Calculates SI                                   # \n");
    printf("#    SI = (principle * Duration * Rate) / 100        # \n");
    printf("###################################################### \n");
}

void
fin_handle_cal_simple_interest (void)
{
    double principle = 0;
    double      rate = 0;
    double  duration = 0;

    /* Print HELP - help the user */
    fin_print_help_cal_simple_interest();

    printf("Principle : ");
    scanf("%lf", &principle);
    printf("Rate : ");
    scanf("%lf", &rate);
    printf("Duration : ");
    scanf("%lf", &duration); 

    printf("Simple Interest : %lf \n", 
            fin_cal_simple_interest(principle, rate, duration));
}

void
fin_handle_cal_converted_discount_rate (void)
{
    double r1 = 0;
    double r2 = 0;
    double n1 = 0;
    double n2 = 0;

    /* Print HELP */
 
    printf("Discount Rate 1: ");
    scanf("%lf", &r1);
    printf("Compounding Frequency 1: ");
    scanf("%lf", &n1);
    printf("Compounding Frequency 2: ");
    scanf("%lf", &n2);

    printf("Discount Rate 2: %lf %% \n",
           fin_cal_converted_discount_rate(r1, n1, n2));

    printf("Discount Rate 2: %lf %% \n",
           fin_cal_effective_discount_rate(r1, n1 , n2));
}


void
fin_handle_cal_principle_in_period (void)
{
    double principle = 0;
    double      rate = 0;
    double       pmt = 0;
    uint32_t     n1 = 0;
    uint32_t     n2 = 0;
    uint32_t period = 0;

    printf("Principle : ");
    scanf("%lf", &principle);
    printf("Rate : ");
    scanf("%lf", &rate);
    printf("Payment: ");
    scanf("%lf", &pmt);
    printf("Duration of Loan : ");
    scanf("%u", &period);
    printf("Period1 : ");
    scanf("%u", &n1);
    printf("Period2 : ");
    scanf("%u", &n2);

    printf("Loan Details: (Principle : %lf, rate : %lf \n",
            principle, rate);

    printf("Principle Component in payment between period"
           "%u and %u is %lf \n", n1, n2, 
           fin_cal_principle_in_period(principle, rate, pmt, n1, n2, period));
    return;
}


void
fin_handle_cal_interest_in_period (void)
{
    /* Interest = Total payment - Principle component */
    /* Interest = EMI*(n2-n1) - principle component */

    double principle = 0;
    double      rate = 0;
    double       pmt = 0;
    uint32_t     n1 = 0;
    uint32_t     n2 = 0;
    uint32_t period = 0;

    printf("Principle : ");
    scanf("%lf", &principle);
    printf("Rate : ");
    scanf("%lf", &rate);
    printf("Payment: ");
    scanf("%lf", &pmt);
    printf("Duration of Loan : ");
    scanf("%u", &period);
    printf("Period1 : ");
    scanf("%u", &n1);
    printf("Period2 : ");
    scanf("%u", &n2);

    printf("Loan Details: (Principle : %lf, rate : %lf \n",
            principle, rate);

    printf("Interest Component in payment between period"
           "%u and %u is %lf \n", n1, n2,
           fin_cal_interest_in_period(principle, rate, pmt, n1, n2, period));
    return;
}


void
fin_handle_cal_periodic_pmt (void)
{
    double principle = 0; 
    double      rate = 0;
    double    period = 0;

    printf("Principle : "); 
    scanf("%lf", &principle); 
    printf("Rate : "); 
    scanf("%lf", &rate);
    printf("Period : ");
    scanf("%lf", &period);

    printf("Loan Details: (Principle : %lf, rate : %lf period: %lf \n",
           principle, rate, period);

    printf("Periodic Payment (PMT) amount : %lf \n", 
           fin_cal_periodic_pmt(principle, rate, period));

}


void
fin_handle_cal_pv_annuity (void)
{

    double       pmt = 0;
    double      rate = 0;
    double    period = 0;

    printf("Periodic Payment(PMT) : ");
    scanf("%lf", &pmt);
    printf("Rate : ");
    scanf("%lf", &rate);
    printf("Period (NPER) : ");
    scanf("%lf", &period);

    printf("Loan Details: (Periodic Payment: %lf, rate : %lf period: %lf \n",
           pmt, rate, period);

    printf("PV of annuity : %lf \n",
           fin_cal_pv_annuity(pmt, rate, period));

}


void
fin_handle_display_ammortization_schedule (void)
{
    double principle = 0;
    double       pmt = 0;
    double      rate = 0;
    double    period = 0;

    printf("Loan Amount (principle): ");
    scanf("%lf", &principle);
    printf("Periodic Payment(PMT) : ");
    scanf("%lf", &pmt);
    printf("Rate : ");
    scanf("%lf", &rate);
    printf("Period (NPER) : ");
    scanf("%lf", &period);

    printf("Loan Details: (Principle %lf Periodic Payment: %lf,"
           "rate : %lf period: %lf) \n",
           principle, pmt, rate, period);

    fin_display_ammortization_schedule(principle, rate, pmt, period);
}


void
fin_handle_user_choice (uint32_t choice)
{
    if (0 == choice) {
        printf("Life is all about making choices - You made a Bad One");
    }

    switch(choice) {
    case FIN_CAL_SIMPLE_INTEREST:
        fin_handle_cal_simple_interest();
    break;
    case FIN_CAL_COMPOUND_INTEREST:
    break;
    case FIN_CAL_PV:
    break;
    case FIN_CAL_FV:
    break;
    case FIN_CAL_DISCOUNT_RATE:
    break;
    case FIN_CAL_CONVERTED_DISCOUNT_RATE:
        fin_handle_cal_converted_discount_rate();
    break;
    case FIN_CAL_PRINCIPLE_IN_PERIOD:
        fin_handle_cal_principle_in_period();
    break;
    case FIN_CAL_INTEREST_IN_PERIOD:
        fin_handle_cal_interest_in_period();
    break;
    case FIN_CAL_PERIODIC_PMT:
        fin_handle_cal_periodic_pmt();
    break;
    case FIN_CAL_PV_ANNUITY:
        fin_handle_cal_pv_annuity();
    break;
    case FIN_CAL_AMMORTIZATION_SCHEDULE:
        fin_handle_display_ammortization_schedule();
    default:
        return; 
    }
}

int main (int argc, char** argv)
{
    uint32_t choice = 0; 

    /* Display the choices */
    fin_display_menu(); 
 
    scanf("%u", &choice);

    fin_handle_user_choice(choice);
    
    return 0;
}
