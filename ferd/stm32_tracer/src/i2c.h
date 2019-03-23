
#define I2C_TIMEOUT_MAX 100000


void i2c_init();
int i2c_read(uint16_t addr);
int i2c_write(uint16_t addr, uint8_t data);

uint16_t i2c_read16(uint16_t addr);
void i2c_write16(uint16_t addr, uint16_t d);