
#include <stdlib.h>
#include <unistd.h>

#include "main.h"
#include "conio.h"
#include "filesource.h"
#include "view.h"
#include "ctrl.h"
#include "hex.h"



void usage(void)
{
	printf("stvw version 0.1\n");
	printf("Usage:\n");
	printf(" stvw file\n");
	printf("\n");
}



int main(int argc,char **argv)
{
int ch;
	if(argc < 2) {
		usage();
		return -1;
	}

	CFileSource fs;
	if(!fs.open(argv[1])) {
		printf("Can\'t open %s\n",argv[1]);
		return -1;
	}

	CScreen s;
	CHexModel m(fs);
	CView v(s,m);
	CCtrl c(v,m,fs);

    for(;;) {
    	c.refresh();
    	ch = s.getchr();
// 		s.printfxy(0,2,"%d",v.getElemPos(c.vs));
		if(ch == KEY_ESC) break;
		c.loop(ch);
    }

    return 0;
}



