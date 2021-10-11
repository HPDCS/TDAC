#include "stdafx.h"

#include <math.h>
#include <stdlib.h>
#include "urand.h"
#include "generale.h"
#define  NORM     0x7fffffff

static double zipf_sum_data_items_vector[NUMBER_OF_DATA_ITEMS];
static double zipf_data_items_norm_const;
static double alpha=0;
static int first_execution=1;

// genera numeri uniformemente distribuiti tra 0 e max

double Random(unsigned *pseed)      /*  GENERATORE NUMERI PSEUDO CASUALI TRA 0 E 1 */
        
{
   	unsigned   temp;
   	double     temp1;

   	temp = (1220703125 * *pseed) & NORM;
   	*pseed = temp;

   	temp1 = temp;
   	temp1 *= 0.465613e-9;

   	return  temp1;

}

int random_number(unsigned *pseed,int max) {
	double val;
	while((   val = Random(pseed) )==1);
	int v= val*(max + 1);
	return v;
}



// genera numeri distribuiti esponenzialmente 
double Expent(unsigned *pseed, double ta)

{
   double   temp;
   unsigned temp1;

        temp1 = *pseed;
        temp = -ta*log(Random(&temp1));
	  
        *pseed = temp1;
	return(temp);
}

double zipf(int val, double alpha) {
	return 1.0 / pow((double) val, alpha);
}

double zipf_probability(int val, double new_alpha) {
// Compute normalization constant
	if (new_alpha != alpha || first_execution) {
		first_execution=0;
		alpha=new_alpha;
		zipf_data_items_norm_const=0;
		int i;
		for (i=1; i<=NUMBER_OF_DATA_ITEMS; i++) {
			zipf_data_items_norm_const = zipf_data_items_norm_const + zipf(i, alpha);
		}

		zipf_sum_data_items_vector[0]=zipf(i, alpha) * zipf_data_items_norm_const;
		for (i=1; i<NUMBER_OF_DATA_ITEMS; i++){
			zipf_sum_data_items_vector[i]=zipf_sum_data_items_vector[i-1]+ zipf(i, alpha) * zipf_data_items_norm_const;
		}
	}
	return (1.0 / pow((double) val, alpha))*1/zipf_data_items_norm_const;
}




int zipf_data_items(double new_alpha) {
	
	double z;                 // Uniform random number (0 < z < 1)
	int zipf_value;           // Computed exponential value to be returned
	int i=0;                  // Loop counter
	
	// Compute normalization constant
	if (new_alpha != alpha || first_execution){
		first_execution=0;
		alpha=new_alpha;
		zipf_data_items_norm_const=0;
		for (i=1; i<=NUMBER_OF_DATA_ITEMS; i++) {
			zipf_data_items_norm_const = zipf_data_items_norm_const + zipf(i, alpha);
		}
		  
		zipf_sum_data_items_vector[0]=zipf(i, alpha) * zipf_data_items_norm_const;
		for (i=1; i<NUMBER_OF_DATA_ITEMS; i++){
			zipf_sum_data_items_vector[i]=zipf_sum_data_items_vector[i-1]+ zipf(i, alpha) / zipf_data_items_norm_const;
		}	
	}
	
	// Pull a uniform random number (0 < z < 1)
	do {
	    z = rand();
	    z/=(double)(RAND_MAX-1);
	} while ((z == 0) || (z == 1));
	
	// Map z to the value
	for (i=0; i<NUMBER_OF_DATA_ITEMS; i++)
	    if (zipf_sum_data_items_vector[i] >= z) {
			zipf_value = i;
			break;
		}	
	return(zipf_value);
}
