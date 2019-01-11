#ifndef MAIN_H
#define	MAIN_H

//#include <xc.h> // include processor files - each processor file is guarded.  

void Sleep();
void WakeUp();

void Inter_sw1();
void Inter_adxl();
void Inter_timer();

bool ReadGPS();
bool LoraSendData();
bool LoraCheckNetwork();
bool LoraDebug();


#endif	/* XC_HEADER_TEMPLATE_H */

