#include "adxl.h"
#include "mcc_generated_files/mcc.h"  //we need _XTAL_FREQ

//Read SPI bus
char ADXL_ReadReg( char reg )
{
    ADXL_CS_SetLow();
    SPI_Exchange8bit(ADXL_RD_SPI);
    SPI_Exchange8bit(reg);
    CKP=0;
    char val = SPI_Exchange8bit(0x00);
    ADXL_CS_SetHigh();
    __delay_ms(100);
    return (val);
}

//Write SPI bus
void ADXL_WriteReg( char reg, char cmd )
{
    ADXL_CS_SetLow();
    SPI_Exchange8bit(ADXL_WR_SPI);
    SPI_Exchange8bit(reg);
    SPI_Exchange8bit(cmd);
    ADXL_CS_SetHigh();
    __delay_ms(100);
}

void InitADXL()
{
    ADXL_CS_SetHigh();
    char reg;
    
    // reset the adxl362
    ADXL_WriteReg(ADXL_RESET, 0x52);

    //Check Reset done
    reg = ADXL_ReadReg(0x0b);
    reg = ADXL_ReadReg(0x00);
    reg = ADXL_ReadReg(0x2c);

   // Activity
    ADXL_WriteReg(0x20,0xFA);
    ADXL_WriteReg(0x21,0x0);
    ADXL_WriteReg(0x22,0xA); //Time before going in awake mode (30 = 5 sec)
    
    // INACTIVITY
    ADXL_WriteReg(0x23,0x96);
    ADXL_WriteReg(0x24,0x0);
    ADXL_WriteReg(0x25,0x1);//Time before going in sleep mode 
   
    //Put in loops mode
    ADXL_WriteReg(0x27,0x3F); //in reference mode
    
    //awake mode
    ADXL_WriteReg(0x2B,0x40);
    
    //STart reading
    ADXL_WriteReg(0x2d,0x0A);
    
   // __delay_ms(500);
}
