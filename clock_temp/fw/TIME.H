/***************************************************************************/
/*
 * PCF8593 operations
 * This is free and unencumbered software released into the public domain.
 * LZs,2005
 */
/***************************************************************************/

void Set_Time(void);
void Get_Time(void);
void Clock_StartUp(void);

UCHR b2bcd(UCHR x);
UCHR bcd2b(UCHR x);

/***************************************************************************/
