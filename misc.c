#include "misc.h"
#include <stdint.h>
#include <xc.h>
#include <stdbool.h>
#include <string.h>
//#include "mcc_generated_files/eusart.h"
#include "mcc_generated_files/mcc.h"


//----------OTHERS----------
void __delay_sec(int n){
    while(n--) __delay_ms(1000); 
}

void blinkRed(int n){
    while(n--){
        LED_RED_SetHigh();
        __delay_ms(75);
        LED_RED_SetLow();
        __delay_ms(75);
    }
}

void blinkGreen(int n){
    while(n--){
        LED_GREEN_SetHigh();
        __delay_ms(75);
        LED_GREEN_SetLow();
        __delay_ms(75);
    }
}

void blinkOrange(int n){
    while(n--){
        LED_ORANGE_SetHigh();
        __delay_ms(75);
        LED_ORANGE_SetLow();
        __delay_ms(75);
    }
}

//----------EUART PIN MANAGER----------
void EUART_GPS(void)
{   
    bool state = (unsigned char)GIE;
    GIE = 0;
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS
      
    RC4PPSbits.RC4PPS = 0x14;   //RC4->EUSART:TX;
    RXPPSbits.RXPPS = 0x15;   //RC3->EUSART:RX;
    RA4PPSbits.RA4PPS = 0x0;   //RA5->I/O
    
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
    GIE = state;
    __delay_ms(100);   
}

void EUART_LORA(void)
{
    bool state = (unsigned char)GIE;
    GIE = 0;
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS
    
    RA4PPSbits.RA4PPS = 0x14;   //RA4->EUSART:TX;
    RXPPSbits.RXPPS = 0x05;   //RA5->EUSART:RX;
    RC4PPSbits.RC4PPS = 0x0;   //RC4->I/O;
    PORTCbits.RC4 = 1; //important, keep it high for GPS, otherwise send error message

    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
    GIE = state;
    __delay_ms(100);
}

//----------EUART EXTENTION METHODS----------
void ReadUartCmd(char *output)
{
//    memset(*output, 0, sizeof(output)/sizeof(char));
    memset(output, 0, 20);
    int i = 0;
    char token;
    token = EUSART_Read();
    while(token!='\n'){
        output[i] = token;
        token = EUSART_Read();
        i++;
    }
}

void SendUartCmd(char *cmd)
{
    for(int i=0;cmd[i] !=  '\0' ;i++)
        EUSART_Write(cmd[i]);
}

//---------GPS COMMANDS---------

void InitGPS(){
    SendUartCmd("$PUBX,40,GLL,1,0,0,0,0\r\n");
    SendUartCmd("$PUBX,40,GSA,0,0,0,0\r\n");
    SendUartCmd("$PUBX,40,GSV,0,0,0,0\r\n");
    SendUartCmd("$PUBX,40,RMC,0,0,0,0\r\n");
    SendUartCmd("$PUBX,40,VTG,0,0,0,0\r\n");
    SendUartCmd("$PUBX,40,GGA,0,0,0,0\r\n");
    
    //Change GPS baud rate to 57600
    SendUartCmd("$PUBX,41,1,0007,0003,57600,0*2B\r\n");
    __delay_ms(100);
}

void ChangeBaudRate57600(){
     //Change PIC Baud Rate to 57600;
    SP1BRGL = 0x8A;
    SP1BRGH = 0x00;
    __delay_ms(100);  
}

//--------------Decimal / Hex ----------
char * NormalizeHex(char *hex){
    char norm[20]="";
    int i;
    for (i = 0; i < hex[i]; i++) {};
    if(i==1) strcpy(norm, "000");
    if(i==2) strcpy(norm, "00");
    if(i==3) strcpy(norm, "0");
    strcat(norm, hex);
    return norm;
}