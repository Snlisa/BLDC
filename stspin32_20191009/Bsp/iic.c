#include "iic.h"
#include "bsp.h"

/* 写入一个命令 */
void i2c_write_command(uint8_t byte)
{
	HAL_I2C_Mem_Write(&hi2c1 ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,&byte,1,0x100);	/* 0x00为命令 */	
}


/* 写入一个数据 */
void i2c_write_data(uint8_t byte)
{
	HAL_I2C_Mem_Write(&hi2c1 ,0x78,0x40,I2C_MEMADD_SIZE_8BIT,&byte,1,0x100);	/* 0x40为数据 */
}

