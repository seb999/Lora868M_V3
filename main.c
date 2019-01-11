#include "mcc_generated_files/mcc.h"
#include "main.h"
#include "misc.h"
#include "adxl.h"
#include <string.h>
#include <stdio.h>

bool isGpsAccess = false;
bool isLoraAccess = false;
bool isMotionStopped = false;
        
char lora[40]="";
char gps[40]="";
char hex[10]="";
char gpsHex[30]="";

char * token;
char * latitude;
char * longitude;
char * latitudeDec;
char * longitudeDec;

int counter = 0;
int counterMotion = 0;
int debugger = 0;

void main(void)
{
    SYSTEM_Initialize();

    INTERRUPT_GlobalInterruptDisable();
    
    SWDTEN = 1;
    CLRWDT();  
    
    InitADXL();
    
    //EUART_GPS();
    //InitGPS();
    
    ChangeBaudRate57600();
    
    EUART_LORA(); //remove this one after debug
    __delay_ms(100); 
    
    blinkGreen(3);
    blinkRed(3);    
     // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    
    //TImer0 interruption Off
    //INTCONbits.TMR0IE = 0;
    while (1)
    {
        CLRWDT();
    }
}

void Inter_adxl()
{
    blinkGreen(3);
   //wake up pic from sleep mode by generating interruption
}

void Inter_timer()
{
    counter++;
    if(counter==300){
        blinkRed(1);
        counter = 0;
        
        if(!isMotionStopped){
            
//            EUART_GPS();
//            if(!ReadGPS()) return;
            
            EUART_LORA();
            for(int i=0;i<=4;i++)
            {
                //LoraDebug();
                if(LoraSendData())
                {
                    break;                  
                }
            }
            counterMotion = 0; //If data sent with success we idle 20" and go in sleep
            isMotionStopped = true;
            //LoraDebug();  
        }
        

        if(isMotionStopped){
            counterMotion++;
        }
        
        if(counterMotion == 10)
        {
            isMotionStopped = false;
            INTCONbits.TMR0IE = 0; // Timer off
            SWDTEN = 0;            // WatchDog Off
            SLEEP();               // Sleep mode 
            INTCONbits.TMR0IE = 1;
            SWDTEN = 1; 
        }
        
        //remove after debug used to setup the device
//        LORA_RESET_SetLow();
//        __delay_ms(200);
//        LORA_RESET_SetHigh();
//        __delay_ms(200);
//        SendUartCmd("mac set appeui 70B3D57ED0007651\r\n"); 
//        ReadUartCmd(lora);
//        SendUartCmd("mac set appkey 9FA338A47B91F3690A48DAFEC7CEF807\r\n"); 
//        ReadUartCmd(lora);
//        SendUartCmd("mac save\r\n"); 
//        ReadUartCmd(lora);
    } 
}

//----------GPS---LORA---METHODS------------
bool ReadGPS()
{  
    CLRWDT();
    SendUartCmd("$PUBX,40,GLL,1,1,0,0,0\r\n"); //GPS ON
    ReadUartCmd(gps);
    ReadUartCmd(gps);//we read twice if empty string
    SendUartCmd("$PUBX,40,GLL,1,0,0,0,0\r\n");// GPS OFF
    
    token = strtok (gps," ,");//READ root code
    if(strcmp(token,"$GPGLL") != 0){ 
        isGpsAccess = false;
        return false;
    }
   
    latitude = strtok (NULL,"."); //Latitude
    latitudeDec = strtok (NULL,","); //Latitude decimal .
    
    token = strtok (NULL,","); //READ 'N' or exit
    if(strcmp(token,"N") != 0){
        isGpsAccess = false;
        return false;
    }
    
    longitude = strtok (NULL,"."); //Longitude 
    longitudeDec = strtok (NULL,","); //Longitude decimal
    
    //Convert to hex and concatenate
    strcpy(gpsHex,"");
      
    sprintf(hex, "%X", atoi(latitude));
    strcat(gpsHex,NormalizeHex(hex)); 
    sprintf(hex, "%X", atoi(latitudeDec));
    strcat(gpsHex,NormalizeHex(hex)); 
    sprintf(hex, "%X", atoi(longitude));
    strcat(gpsHex,NormalizeHex(hex)); 
    sprintf(hex, "%X", atoi(longitudeDec));
    strcat(gpsHex,NormalizeHex(hex)); 
    SendUartCmd(gpsHex);
    isGpsAccess = true;
    return true;
}

bool LoraCheckNetwork(){
    LORA_RESET_SetLow();
    __delay_ms(200);
    LORA_RESET_SetHigh();
     __delay_ms(200);
    SendUartCmd("sys reset\r\n"); 
    ReadUartCmd(lora);  //read serial number
    
    SendUartCmd("mac join otaa\r\n");
    ReadUartCmd(lora);  //read ok
    ReadUartCmd(lora);  //read denied/accepted
   
    token = strtok(lora," \r\n");
    if(strcmp(token,"accepted") != 0){
        blinkOrange(1);
        isLoraAccess = false;
        return false;
    }
    return true;
}

bool LoraSendData(){
    LORA_RESET_SetLow();
    __delay_ms(200);
    LORA_RESET_SetHigh();
     __delay_ms(200);
    SendUartCmd("sys reset\r\n"); 
    ReadUartCmd(lora);  //read serial number
    
    CLRWDT();//watch dog can reset before end of routine
    SendUartCmd("mac set pwridx 1\r\n"); //Set power
    ReadUartCmd(lora);  //read ok
    
    SendUartCmd("mac join otaa\r\n");
    ReadUartCmd(lora);  //read ok
    ReadUartCmd(lora);  //read denied or accepted
   
    token = strtok(lora," \r\n");
    if(strcmp(token,"accepted") != 0){
        blinkRed(2);
        isLoraAccess = false;
        return false;
    }
    CLRWDT();   
    //send GPS position
    char str[50];
    strcpy(str,"mac tx uncnf 9 ");
    strcat(str, gpsHex);
    strcat(str, "\r\n");
    SendUartCmd(str);
    ReadUartCmd(lora);  //read ok 
    ReadUartCmd(lora);   //read mac_tx_ok or not
    
    token = strtok(lora," \r\n");
    if(strcmp(token,"mac_tx_ok") != 0){
        SendUartCmd("send data failed");
        blinkRed(2);
        isLoraAccess = false;
        return false;
    }
    blinkGreen(3);
    isLoraAccess = true;
    isMotionStopped = true;
    counterMotion = 0;
    return true;
}

bool LoraDebug(){
    LORA_RESET_SetLow();
    __delay_ms(200);
    LORA_RESET_SetHigh();
     __delay_ms(200);
    SendUartCmd("sys reset\r\n"); 
    ReadUartCmd(lora);  //read serial number
    
    CLRWDT();//watch dog can reset before end of routine
    SendUartCmd("mac join otaa\r\n");
    ReadUartCmd(lora);  //read ok
    ReadUartCmd(lora);  //read denied or accepted
   
    token = strtok(lora," \r\n");
    if(strcmp(token,"accepted") != 0){
        blinkRed(2);
        return false;
    }
    CLRWDT();   
    
    SendUartCmd("mac tx uncnf 9 17206EB1070C01E9\r\n");
    ReadUartCmd(lora);  //read ok 
    ReadUartCmd(lora);   //read mac_tx_ok or not
    
    token = strtok(lora," \r\n");
    if(strcmp(token,"mac_tx_ok") != 0){
        SendUartCmd("send data failed");
        blinkRed(2);
        return false;
    }
    blinkGreen(3);
    SendUartCmd("GPS data sent\n"); 
    return true;
}
