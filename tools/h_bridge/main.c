/*=========================================================================*/
/**
 * Pulse generator using atm328p/Timer1 (Arduino UNO + ARD LCD216)
 *
 * FE R&D group,2012-2014
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#include "main.h"

#include "display.h"
#include "keys.h"
#include "timer.h"

extern volatile uint32_t tau;	// in nanosecinds
extern volatile uint16_t duty;	// 0..999 (0..99.9%)
extern volatile uint16_t duty2; // 0..999 (0..99.9%)

volatile uint8_t sw;
volatile uint8_t f;
volatile uint8_t step_fq;
volatile uint8_t step_dt;

//=============================================================================

prog_uint32_t steps_fq[] = {
	1,10,100,1000,10000,100000,1000000 // ns
};

prog_uint32_t steps_dt[] = {
	1,10,100		// 0.1% 1% 10%
};


void keys()
{
uint8_t key = get_key();
uint32_t step;
	if(key == KEY_LEFT) {
		while(get_key());
		sw--;
		sw &= 3;
		f = 1;
	} else if(key == KEY_RIGHT) {
		while(get_key());
		sw++;
		sw &= 3;
		f = 1;
	} else if(key == KEY_UP || key == KEY_DOWN) {
		while(get_key());
		switch(sw) {
			case 0:
				step = pgm_read_dword(&steps_fq[step_fq]);
				if(key == KEY_UP) {
					if(tau+step < TAU_MAX) tau += step;
				} else {
					if(tau > TAU_MIN+step) tau -= step;
				}
				break;

			case 1:
				step_fq++;
				if(step_fq >= NELEM(steps_fq)) step_fq=0;
				break;

			case 2:
				step = pgm_read_dword(&steps_dt[step_dt]);
				if(key == KEY_UP) {
					if(duty+step < DT_MAX)  duty += step;
				} else {
					if(duty > step) duty -= step;
				}
				break;

			case 3:
				step_dt++;
				if(step_dt >= NELEM(steps_dt)) step_dt=0;
				break;
		}
		f = 1;
	}
}


//=============================================================================

volatile uint8_t drc;
volatile uint16_t w;


ISR (INT0_vect)
{
uint32_t step;
	for(w=0; w < 2000; w++);
	if((PIND & 4) != 0) return;	// int0
	drc = DRC;

	step = pgm_read_dword(&steps_dt[step_dt]);
	if(!drc) {
		if(duty+step < DT_MAX)  duty += step;
	} else {
		if(duty > step) duty -= step;
	}

	f = 1;
}

//=========================================================================

volatile uint8_t drc2;
volatile uint16_t w2;

ISR (INT1_vect)
{
uint32_t step;
	for(w2=0; w2 < 2000; w2++);
	if((PIND & 8) != 0) return;	// int1
	drc2 = DRC2;

	step = pgm_read_dword(&steps_fq[step_fq]);
	if(drc2) {
		if(tau+step < TAU_MAX) tau += step;
	} else {
		if(tau > TAU_MIN+step) tau -= step;
	}

	f = 1;
}

//=========================================================================

void put_tau(uint32_t t)
{
	if(t < 1000000) {
		putd(t/1000,3,0);
		putch('.');
		putd(t%1000,3,1);
		putch('u');
	} else {
		t /= 1000;
		putd(t/1000,3,0);
		putch('.');
		putd(t%1000,3,1);
		putch('m');
	}
	putch('s');
}


void put_step(uint32_t n)
{
	if(n >= 1000000) {
		putd(n/1000000,3,0);
		putch('m');
	} else if(n >= 1000) {
		putd(n/1000,3,0);
		putch('u');
	} else {
		putd(n,3,0);
		putch('n');
	}
	putch('s');
}


// print percents nn.n

void put_prc(uint16_t n)
{
	if(n > 999) n=999;
	putd(n/10,2,0);
	putch('.');
	putch('0'+n%10);
}


void show()
{
	clrscr(1);

	locxy(0,0);
	putch('T');
	put_tau(tau);

	locxy(11,0);
	put_step(pgm_read_dword(&steps_fq[step_fq]));

	locxy(0,1);
	putch('D'); putch(' ');
	put_prc(duty);
	putch('%');

	locxy(11,1);
	put_prc(pgm_read_dword(&steps_dt[step_dt]));
	putch('%');

	switch(sw) {
		case 0:	locxy(1,0); break;
		case 1:	locxy(11,0); break;
		case 2:	locxy(1,1); break;
		case 3:	locxy(10,1); break;
	}
}

//=========================================================================

void save_params()
{
	eeprom_write_dword((uint32_t*)0,tau);
	eeprom_write_word( (uint16_t*)4,duty) ;
	eeprom_write_byte( (uint8_t*)6,step_fq);
	eeprom_write_byte( (uint8_t*)7,step_dt);
}

//=========================================================================

void load_params()
{
	tau   = eeprom_read_dword((uint32_t*)0);
	duty  = eeprom_read_word( (uint16_t*)4);
	step_fq = eeprom_read_byte((uint8_t*)6);
	step_dt = eeprom_read_byte((uint8_t*)7);

	if(tau >= TAU_MAX) tau = TAU_MAX;
	if(tau < TAU_MIN) tau = TAU_MIN; 

	if(duty == 0) duty=1;
	if(duty > DT_MAX) duty=333;

	if(step_fq >= NELEM(steps_fq)) step_fq=0;
	if(step_dt >= NELEM(steps_dt)) step_dt=0;
}

//=========================================================================

int main(void)
{
uint8_t savet;
	DDRD = 0xf0;	// LCD data PD4-7
	DDRC = 0x18;    // LCD E,RS
	DDRB = 0x06;	// Timer out

	DIDR0 = 1;	// ADC0
	ADCSRA = 0x87;	// ADC enable
        ADMUX = 0x40;	// Vcc as Aref

	EIMSK = 3;	// int0,1
	EICRA = 0x0a;	// int0,1 falling edge

	display_init();
	clrscr(1);
	load_params();
	setup_timer1();
	sw = 0;
	f = 1;
	duty2 = 500;
	sei();

	for(savet=0; ; ) {
		LED1;	delay(100);
		LED0;	delay(100);
		keys();
		if(f) {
			f = 0;
			show();
			setup_timer1();
			savet = 25;	// 5sec timeout
		}
		if(savet) {
			if(--savet == 0) save_params();
		}
	}

	return 0;
}

//=========================================================================

