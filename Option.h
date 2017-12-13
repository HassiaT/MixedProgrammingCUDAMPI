#if !defined(_OPTION_H)
#define _OPTION_H

/* The structure DATAOPT represents the */
/* characteristics of an option */
/* An object of type Option has a private member "data" that is */
/* a pointer on its key characteristics */

class Option 
{
 private:
  struct _DATAOPT 
  {
    double SO = 110; /*current price of the underlying asset */
    double K; /* the strike price, randomly chosen */
    double dt = 1/365;/*using time in years */
    double type; /* 1.0 if option is CALL, 2.0 if option is PUT */
    double sigma= 0.2 ; /* the volatility */
    double r = 0.05; /* typical value =5%,*/
                     /* the free-risk interest rate */
    double P; /* the price of the option,to be calculated */
  } *data;
  
 public:
  
 
}


#endif /* !defined(_OPTION_H) */
