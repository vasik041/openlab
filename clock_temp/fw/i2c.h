/*=========================================================================*/
/*
 * I2C operations
 * This is free and unencumbered software released into the public domain.
 * LZs,2005
 */
/*=========================================================================*/

void	wx(void);

void	e2p_err(UCHR err_code);
void	bstart(void);
void	bstop(void);
UCHR	bitin(void);
void	bitout(UCHR b);
UCHR	rx(UCHR ack);
void	tx(UCHR d);

void	wr_clk(UCHR adr,UCHR d);
UCHR	rd_clk(UCHR adr);

void	i2c_init(void);

/*=========================================================================*/

