#include "SHT31.h"

void SHT31_Reset()
{   
    int8_t reg[2] = {0};
	reg[0] = 0x2C;
	reg[1] = 0x06;

    i2c_write_blocking(I2C_PORT, SHT31_ADDRESS, reg, sizeof(reg), false );
   
}
float SHT31_ReadData(int16_t *temp)
{
    uint8_t buffer[6];

    // read the temperture data
    
    int8_t reg[2] = {0};
	reg[0] = 0x2C;
	reg[1] = 0x06;
 
	
    i2c_write_blocking(I2C_PORT, SHT31_ADDRESS, reg, sizeof(reg), false );
    sleep_ms(500);
    i2c_read_blocking(I2C_PORT, SHT31_ADDRESS, buffer, sizeof(buffer), false );
    
    return (((buffer[0] * 256) + buffer[1]) * 175.0) / 65535.0  - 45.0;

    
}