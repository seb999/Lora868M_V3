#ifndef MISC_H
#define	MISC_H

void __delay_sec(int);
void blinkRed(int n);
void blinkGreen(int n);
void blinkOrange(int n);
void ReadUartCmd(char *Output);
void SendUartCmd(char *cmd);
void InitGPS(void);
void EUART_LORA(void);
void EUART_GPS(void);
void ChangeBaudRate57600(void);
char * NormalizeHex(char *hex);

#endif	/* MISC_H */

