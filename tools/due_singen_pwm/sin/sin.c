
#include <stdio.h>
#include <math.h>

#define MAX 256
#define PI 3.141592

int main(int argc,char *argv[])
{
double i,f,f2;
int ii,ff,ff2;
FILE *fo;
	
	fo = fopen("sin.h","w");
	for(i=0.; i < MAX; i += 1.) {
		f = 128. + 120. * sinl( (2.*PI * i) / MAX );
		ii = (int) i;
		ff = (int) f;
		
		if(ii % 16 == 0)
			fprintf(fo,"\n\t");
		fprintf(fo,"%3d,",ff);
	}
	fclose(fo);
}

