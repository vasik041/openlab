//===================================================================================

#include "main.h"
#include "port.h"
#include "sql.h"
#include "http.h"
#include "conio.h"
#include "port.h"
#include "calendar.h"


const char *sql1 =
"CREATE TABLE IF NOT EXISTS Meas ("
	"year INTEGER,"
	"month INTEGER,"
	"day INTEGER,"
	"hour INTEGER,"
	"minute INTEGER,"
	"week INTEGER,"
	"sensor_id INTEGER,"
	"value REAL,"
	"meas_id INTEGER PRIMARY KEY"
");";


//===================================================================================

typedef struct {
	float value;
	int sensor_id;
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int week;
} meas_t;


meas_t meas[10];
int n_meas = 0;


void flush()
{
char str[256] = {0};
int i;
    if(!n_meas) return;
    if(!dbx_query("begin transaction;")) {
	printf("flush - dbx_query failed\n");
    }
    for(i=0; i < n_meas; i++) {
    	snprintf(str,sizeof(str),"INSERT INTO Meas (year,month,day,hour,minute,week,sensor_id,value) VALUES "
		"(%d,%d,%d, %d,%d,%d, %d,%3.1f);",
		meas[i].year,meas[i].month,meas[i].day,
		meas[i].hour,meas[i].minute,meas[i].week,
		meas[i].sensor_id,meas[i].value);
	if(!dbx_query(str)) {
		printf("flush - dbx_query failed2\n");
	}
    }
    if(!dbx_query("commit;")) {
    	printf("flush - dbx_query failed3\n");
    }
    n_meas = 0;
}


void push(float val,int sid,int hour,int minute,int day,int month,int year)
{
int week = weekNumber(day,month,year);
    printf("sid: %d val: %3.1f %02d:%02d %02d-%02d-%04d wk %d\n",sid,val,hour,minute,day,month,year,week);

    meas[n_meas].value = val;
    meas[n_meas].sensor_id = sid;
    meas[n_meas].year = year;
    meas[n_meas].month = month;		//1..12
    meas[n_meas].day = day;		//1..31
    meas[n_meas].hour = hour;		//0..23
    meas[n_meas].minute = minute;	//0..59
    meas[n_meas].week = week;

    if(++n_meas >= NELEM(meas)) {
	flush();
    }
}


void test_data()
{
int d,m,h;
//    for(m=1; m <= 12; m++) {
	m = 11;
        for(d=14; d <= 20; d++) {
	    for(h=0; h < 24; h++) {
    		float temp = m+d+h;
		push(temp,20, h,30, d,m,2016);
	    }
	}
//    }
}


uint8_t check_data_cs(uint8_t *d)
{
uint8_t cs = (d[0] & 0x0f) ^ d[1] ^ d[2] ^ d[3];
    cs = (cs & 0x0f) ^ ((cs >> 4) & 0x0f) ^ 0x05;
    return ((d[0] & 0xf0) == (cs << 4)) ? 1 : 0;
}


//0123456789012345678901234567
//          1         2
//20:14 17-11-16 f1 60 18 26+

void add_meas(char *s)
{
float val = 0;
int year = 0;
int month = 0;
int day = 0;
int hour = 0;
int minute = 0;
int sid = 0;
int x = 0;
uint8_t d[4] = {0};

    printf(">%s\n",s);

    if(s[0] == 'o' && s[1] == 'k') return;
    if(s[0] == 'r' && s[1] == 'c' && s[2] == 'v' && s[3] == 'd') return;
    if(strlen(s) < 26) return;
    if(s[2] != ':' || s[5] != ' ' || s[8] != '-' || s[11] != '-') return;
    if(s[14] != ' ' || s[17] != ' ' || s[20] != ' ' || s[23] != ' ' || s[26] != '+') return;

    if(sscanf(s,"%d",&hour) != 1) return;
    if(sscanf(s+3,"%d",&minute) != 1) return;
    if(sscanf(s+6,"%d",&day) != 1) return;
    if(sscanf(s+9,"%d",&month) != 1) return;
    if(sscanf(s+12,"%d",&year) != 1) return;
    year += 2000;

    if(sscanf(s+15,"%02x",&x) != 1) return;
    d[0] = x & 0xff;
    if(sscanf(s+18,"%02x",&x) != 1) return;
    d[1] = x & 0xff;
    if(sscanf(s+21,"%02x",&x) != 1) return;
    d[2] = x & 0xff;
    if(sscanf(s+24,"%02x",&x) != 1) return;
    d[3] = x & 0xff;

    if(!check_data_cs(d)) return;

    sid = d[0] & 0x0f;
    if(sid == 1) {
	float bat = d[1];
	float temp = d[2];
	float hum = d[3];
	bat *= 100.;
	bat /= 256.;
	push(temp,0,hour,minute,day,month,year);
	push(hum, 1,hour,minute,day,month,year);
	push(bat, 2,hour,minute,day,month,year);
    } else if(sid == 2) {
	float temp = d[2] + 256*d[3];
	temp /= 10.;
	float bat = d[1];
	bat *= 100.;
	bat /= 256.;
	if(temp != 85.) push(temp,20,hour,minute,day,month,year);
	push(bat, 21,hour,minute,day,month,year);
    }

}

//===================================================================================

int main(int argc, char* argv[])
{
char str[32] = {0};
int pos = 0;
int cnt = 0;

	if(ser_open("/dev/ttyS2",B9600) != 0) {
	    printf("can\'t open COM port, error %d %s",errno,strerror(errno));
	    goto exit;
	}

	dbx_init();
	if(!dbx_query(sql1)) {
		printf("dbx_query failed\n");
		goto exit;
	}
//	test_data();
	start_server();

	for( ; ; ) {
		if(ser_rcvd() > 0) {
			int x = ser_recv();
//			printf("%c",(int)x);
			if(x != '\r' && x != '\n') {
				str[pos++] = x;
			}
			if(pos >= sizeof(str)-1 || x == '\r') {
				str[pos] = '\0';
				add_meas(str);
				pos = 0;
			}
		}
		if(++cnt > 500) {
			cnt = 0;
			ser_send('/');
		}
	        if(n_meas && !pos) {
		    flush();
		}
		usleep(1000);
	}
exit:
	stop_server();
	dbx_close();
	ser_close();
	return 0;
}

//===================================================================================
