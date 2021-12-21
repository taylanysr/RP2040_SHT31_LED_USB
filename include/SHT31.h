#include "hardware/gpio.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define SHT31_ADDRESS 0x44

void SHT31_Reset();
float SHT31_ReadData();