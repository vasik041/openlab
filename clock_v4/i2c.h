//=========================================================================
//
// I2C
// LZ,2010
//
//=========================================================================

#define	rxACK		0
#define	rxNO_ACK	1

/*=========================================================================*/

void	wx(void);

void	e2p_err(uint8_t err_code,uint8_t err_place);
void	bstart(void);
void	bstop(void);
uint8_t	bitin(void);
void	bitout(uint8_t b);
uint8_t	rx(uint8_t ack);
void	tx(uint8_t d);

void	wr_clk(uint8_t adr,uint8_t d);
uint8_t	rd_clk(uint8_t adr);
uint8_t rd_eeprom(uint16_t adr);
void	wr_eeprom(uint16_t adr,uint8_t d);

void	i2c_init(void);

//=========================================================================

