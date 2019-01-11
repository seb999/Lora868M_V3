#ifndef ADXL_H
#define	ADXL_H

// ADLX Registers
#define ADXL_RESET 0x1f
#define ADXL_INTMAP2 0x2b
#define ADXL_POWER_CTL 0x2d
#define ADXL_WR_SPI 0x0A
#define ADXL_RD_SPI 0x0B

void InitADXL(void);
void ADXL_WriteReg( char reg, char cmd );
char ADXL_ReadReg( char reg );

#endif	