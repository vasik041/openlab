
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ent.h"
#include "randtest.h"

extern double pochisq(const double ax, const int df);


char *attn(double chip)
{
    chip *= 100;

    if(chip < 1. || chip > 99.) return "!!!";
    else if(chip >= 1. && chip <= 5.) return "!!";
    else if(chip >= 95. && chip <= 99.) return "!!";
    else if(chip >= 5. && chip <= 10.) return "!";
    else if(chip >= 90. && chip <= 95.) return "!";
    return "";
}


void calc(uint8_t *buf, int len, int binary, double *csq)
{
long ccount[256];	      /* Bins to count occurrences of values */
long totalc = 0;	      /* Total character count */
double montepi=0, chip=0, scc=0, ent=0, mean=0, chisq=0;

    memset(ccount, 0, sizeof ccount);

    /* Initialise for calculations */
    rt_init(binary);

    /* Scan input file and count character occurrences */
    for(int i=0; i < len; i++) {
	unsigned char ocb = buf[i];
	totalc += (binary ? 8 : 1);

	if(binary) {
	    int b;
    	    unsigned char ob = ocb;
	    for (b = 0; b < 8; b++) {
		ccount[ob & 1]++;
		ob >>= 1;
	    }    
	} else {
	    ccount[ocb]++;
	}
	rt_add(&ocb, 1);
    }

    /* Complete calculation and return sequence metrics */
    rt_end(&ent, &chisq, &mean, &montepi, &scc);

    /* Calculate probability of observed distribution occurring from the results of the Chi-Square test */
    chip = pochisq(chisq, binary ? 1 : 255);

    /* Print calculated results */
    printf("%ld samples, entropy %f, chisq %1.2f, mean %1.4f, chip %1.2f %s pi %f scc %f\n",
				 totalc, ent, chisq, mean, chip*100, attn(chip), montepi, scc);

    *csq = chisq;
}



void test(void)
{
double chip,chisq;
int i;

    for(i=0; i < 50; i++) {
	chisq = 10. * i;
        chip = pochisq(chisq, 255);
        printf("chisq %1.2f, chip %1.2f\n", chisq, chip*100);
    }
}



