
#include "main.h"
#include "data.h"
#include "sql.h"
#include "calendar.h"


typedef struct {
	float value[31];
	int index[31];
	int p;
	int n;
} dataset_t;


static int data_cb(void *pCtx, int argc, char **argv, char **azColName)
{
dataset_t *ds = (dataset_t *)pCtx;
	if(argc != 3) return -1;
	double sum = atof(argv[0]);
	double count = atof(argv[1]);
	int ix = atoi(argv[2]);
	if(ds->p < ds->n) {
		if(count != 0) {
			ds->value[ds->p] = (float)(sum / count);
			ds->index[ds->p] = ix;
			ds->p++;
		}
	}
	return 0;
}


int get_year(int sid,int year,dataset_t *ds)
{
char sql[256] = {0};
	snprintf(sql,sizeof(sql),"select sum(meas.value),count(meas.meas_id),month from meas "
			"where year = %d and sensor_id = %d group by month order by month;",
			year,sid);
	ds->n = 12;
	ds->p = 0;
	return dbx_query_cb(sql,ds,data_cb);
}


int get_month(int sid,int year,int month,dataset_t *ds)
{
char sql[256] = {0};

	snprintf(sql,sizeof(sql),"select sum(meas.value),count(meas.meas_id),day from meas "
			"where year = %d and month = %d and sensor_id = %d group by day "
			"order by day;",year,month,sid);
	ds->n = daysInMonth(month,year);
	ds->p = 0;
	return dbx_query_cb(sql,ds,data_cb);
}


int get_day(int sid,int year,int month,int day,dataset_t *ds)
{
char sql[256] = {0};

	snprintf(sql,sizeof(sql),"select sum(meas.value),count(meas.meas_id),hour from meas "
			"where year = %d and month = %d and day = %d and sensor_id = %d group by hour order by hour;",
			year,month,day,sid);
	ds->n = 24;
	ds->p = 0;
	return dbx_query_cb(sql,ds,data_cb);
}


//=============================================================================

void swap(dataset_t *ds,int i,int j)
{
    float tmp = ds->value[i];
    ds->value[i] = ds->value[j];
    ds->value[j] = tmp;

    int tmp2 = ds->index[i];
    ds->index[i] = ds->index[j];
    ds->index[j] = tmp2;
}


void quickSort(dataset_t *ds, int left, int right)
{
int i = left, j = right;
int tmp;
int pivot = ds->index[(left + right) / 2];

      while (i <= j) {
            while (ds->index[i] < pivot)
                  i++;
            while (ds->index[j] > pivot)
                  j--;
            if (i <= j) {
                  swap(ds,i,j);
                  i++;
                  j--;
            }
      }
      if (left < j)
            quickSort(ds, left, j);
      if (i < right)
            quickSort(ds, i, right);
}


static int data_cb_week(void *pCtx, int argc, char **argv, char **azColName)
{
dataset_t *ds = (dataset_t *)pCtx;
	if(argc != 5) return -1;
	double sum = atof(argv[0]);
	double count = atof(argv[1]);
	int d = atoi(argv[2]);
	int m = atoi(argv[3]);
	int y = atoi(argv[4]);
	if(ds->p < ds->n) {
		if(count != 0) {
			ds->value[ds->p] = (float)(sum / count);
			ds->index[ds->p] = dayOfWeek(d,m,y);
			if(ds->index[ds->p] == 0) ds->index[ds->p] = 7; // special handling for Sundays
			ds->p++;
		}
	}
	return 0;
}


int get_week(int sid,int year,int week,dataset_t *ds)
{
char sql[256] = {0};
	snprintf(sql,sizeof(sql),"select sum(meas.value),count(meas.meas_id),day,month,year from meas "
			"where week = %d and year = %d and sensor_id = %d group by day "
			"order by day;",week,year,sid);
	ds->n = 7;
	ds->p = 0;
	if(!dbx_query_cb(sql,ds,data_cb_week)) return 0;
	if(ds->p > 1)
	     quickSort(ds,0,ds->p-1);
	return 1;
}

//=============================================================================

typedef struct {
	char date[32][64];
	float value[32];
	int p;
	int n;
} dataset2_t;


static int data_cb2(void *pCtx, int argc, char **argv, char **azColName)
{
dataset2_t *ds = (dataset2_t *)pCtx;
	if(argc != 3) return -1;
	double value = atof(argv[0]);
	int hour = atof(argv[1]);
	int minute = atoi(argv[2]);
	if(ds->p < ds->n) {
		ds->value[ds->p] = (float)value;
		snprintf(ds->date[ds->p],sizeof(ds->date[ds->p]),"%02d:%02d",hour,minute);
		ds->p++;
	}
	return 0;
}


int get_data(int sid,int year,int month,int day,dataset2_t *ds)
{
char sql[256] = {0};
	snprintf(sql,sizeof(sql),"select meas.value,hour,minute from meas "
			"where year = %d and month = %d and day = %d and sensor_id = %d order by hour,minute;",
			year,month,day,sid);
	ds->n = 32;
	ds->p = 0;
	return dbx_query_cb(sql,ds,data_cb2);
}

//=============================================================================

const char *months[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

extern const char *days[];


int dump_vals(dataset_t *ds,char *s,int maxlen)
{
int i;
    *s = '\0';
    printf("dataset len = %d/%d\n",ds->n,ds->p);
    strcat(s,"{ \"tempData\": [");
    for(i=0; i < ds->p; i++) {
		char val[64] = {0};
		char name[32] = {0};
		if(ds->n == 12) {
			snprintf(name,sizeof(name),"%s",months[ds->index[i]-1]);
		} else if(ds->n == 7) {
			snprintf(name,sizeof(name),"%s",days[ds->index[i]]);
		} else {
			snprintf(name,sizeof(name),"%d",ds->index[i]);
		}
		snprintf(val,sizeof(val),"{ \"name\": \"%s\", \"temp\": \"%3.1f\"}%s",
							name,
							ds->value[i],
							(i == ds->n-1) ? "" : ",");
		if(strlen(s)+strlen(val)+4 < maxlen) {
		    strcat(s,val);
		}
    }
    strcat(s,"]}");
    return strlen(s);
}



int dump_vals2(dataset2_t *ds,char *s,int maxlen)
{
int i;
    *s = '\0';
    printf("dataset2 len = %d/%d\n",ds->n,ds->p);
    for(i=0; i < ds->p; i++) {
	char row[64] = {0};
	snprintf(row,sizeof(row),"%s %3.1f\n",ds->date[i],ds->value[i]);
	if(strlen(s)+strlen(row)+4 < maxlen) {
	    strcat(s,row);
	}
    }
    return strlen(s);
}

//=============================================================================
//  /sen/id/yyyy
//  /sen/id/yyyy/mm
//  /sen/id/yyyy/mm/dd
//  /sen/id/yyyy/wkNN
//  /sen/id/yyyy/mm/dd?text=true

int parse_rest(char *req,char *resp,int maxlen)
{
char *tmp = NULL;
char *end = NULL;
int rc = -1;
int year = 0;
int month = -1;
int day = -1;
int wk = -1;
dataset_t ds;
dataset2_t ds2;
int sensor_id = 0;
	tmp = strstr(req,"/sen/");
	if(!tmp) return -1;
	end = strchr(tmp,' ');
	if(end) *end = '\0';

	tmp += 5;
	sensor_id = atoi(tmp);
	printf("sensor_id = %d\n",sensor_id);
	tmp = strchr(tmp,'/');
	if(!tmp) return -1;
	tmp++;
	year = atoi(tmp);
	printf("year = %d\n",year);
	tmp = strchr(tmp,'/');
	if(tmp) {
		tmp++;
		if(tmp[0] == 'w' && tmp[1] == 'k') {
			wk = atoi(tmp+2);
			printf("week = %d\n",wk);
		} else {
			month = atoi(tmp);
			printf("month = %d\n",month);
			tmp = strchr(tmp,'/');
			if(tmp) {
				day = atoi(tmp+1);
				printf("day = %d\n",day);
				if(strstr(req,"?text=true")) {
				    rc = get_data(sensor_id,year,month,day,&ds2);
				    if(!rc) return -1;
				    return dump_vals2(&ds2,resp,maxlen);
				}
			}
		}
	}

	memset(&ds,0,sizeof(dataset_t));
	if(wk != -1) {
		rc = get_week(sensor_id,year,wk,&ds);
	} else if(month != -1 && day != -1) {
		rc = get_day(sensor_id,year,month,day,&ds);
	} else if(month != -1) {
		rc = get_month(sensor_id,year,month,&ds);
	} else {
		rc = get_year(sensor_id,year,&ds);
	}
	if(!rc) return -1;
	return dump_vals(&ds,resp,maxlen);
}

