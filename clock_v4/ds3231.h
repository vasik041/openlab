/***************************************************************************/
/*
 * ds3231
 * LZ,2016
 */
/***************************************************************************/

//
// ds3231 registers
//
#define	SECONDS	0
#define	MINUTES	1
#define	HOURS	2
#define	DAY	3
#define	DATE	4
#define	MONTH	5
#define	YEAR	6
#define	ALARM1	7
#define	ALARM2	8
#define	CONTROL	0x0e
#define	CTRL_STAT 0x0f
#define	TEMP_L  0x12
#define	TEMP_H	0x11

int8_t clock_init(void);
void set_time(void);
void get_time(void);
uint16_t get_temp(void);

void get_date(void);
void set_date(void);

uint8_t b2bcd(uint8_t x);
uint8_t bcd2b(uint8_t x);
uint8_t is_dst(void);

/***************************************************************************/
