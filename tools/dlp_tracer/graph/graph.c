
#include "koolplot.h"
#include "bmp2png.h"

extern void (*saveFunc)(HWND h);

char fnm[256] = {0};

void save(HWND hWnd)
{
char str[256] = {0};
    unlink("1.bmp");
    writeimagefile("1.bmp", 0, 0, INT_MAX, INT_MAX, false, hWnd);
    sprintf(str,"%s.png",fnm);
    bmp2png("1.bmp",str);
    unlink("1.bmp");
}


int main(int argc,char *argv[])
{
plotdata x;
plotdata y;
char title[256] = {0};
char str[256] = {0};
int i = 0;
double min_x = 0;
double max_x = 0;
double min_y = 0;
double max_y = 0;
int bInit = 0;
FILE *f = NULL;

    if(argc < 2 || argc > 3) {
	printf("Usage:\n");
	printf("graph infile.txt [infile.txt]\n\n");
	return 1;
    }
    saveFunc = save;
    setColor(x,y,CRIMSON);
    strcpy(fnm,argv[1]);

    for(i=1; i < argc; i++) {
        f = fopen(argv[i],"r");
	if(!f) {
	    printf("can\'t open %s\n",argv[i]);
	    return 1;
	}
	if(title[0]) {
	    strcat(title," / ");
	}
	strcat(title,argv[i]);
	if(i == 2) setColor(x,y,BLUE);
	while(fgets(str,sizeof(str),f)) {
	    int a = 0;
	    int b = 0;
	    sscanf(str,"%d %d",&a,&b);
            double xv = 20*(a-447);
            double yv = 20*(b-447);
	    xv /= 1024;
	    yv /= 1024;

	    if(!bInit) {
		min_x = max_x = xv;
		min_y = max_y = yv;
		bInit = 1;
	    }
	    if(yv > max_y) max_y = yv;
	    if(yv < min_y) min_y = yv;
	    if(xv > max_x) max_x = xv;
	    if(xv < min_x) min_x = xv;

	    point(x,y,xv,yv);
	}
	fclose(f);
	breakplot(x,y);
    }

    axesBotLeft (x, y, floor(min_x), floor(min_y));
    axesTopRight(x, y,  ceil(max_x),  ceil(max_y)); 
    plot(x, y, title);
    return 0;
}
