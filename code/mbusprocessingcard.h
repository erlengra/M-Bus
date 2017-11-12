#ifndef __MBUSPROCESSINGCARD_H__
#define __MBUSPROCESSINGCARD_H__

#define OLED_RESET 4
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16
#define bit9600Delay 100  
#define halfBit9600Delay 50
#define bit4800Delay 188
#define halfBit4800Delay 94
#define PINNUMBER "" // pinnumber for SIM card. Leave blank if SIM card has no pinnumber
#define GSM_packet_length 128

/*defining and initializing OLED*/
Adafruit_SSD1306 display(OLED_RESET);

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

// initialize the modem instances
GSM gsmAccess;
GSM_SMS sms;
GSMClient client;
GPRS gprsAccess;

// Error-handling messages
String oktext = "GPRS-OK";
String GPRSerror = "GPRS-ERROR";
String SDFailure = "SD-card-init-ERROR";
String restartBoard = "RESTART";
String FileGen = "FILE-OK";
String FileGenErr = "FILE-ERROR";
String FileOpenErr = "FILE_OPEN_ERROR";

char remoteNum[20];  // telephone number to send sms
char url[20]; // "url.com"
char path[1] = "/"; 
char urlproxy[20]; // "https://urlproxy.com"  webserver proxy address
char password[20]; // password for GSM network connection
char login[50]; // login-user for GSM network connection
char apn[50]; // apn-parameters for GSM connection
char proxy[100]; // proxy-router address

char i2c_scanner(void);
void OLED_intro(void);
void SD_cardinfo(void);
void sendDataPoint(int,int);
void dumpMemory();
void diagnostic();
void restart();
void sleepMode();
void normalMode();
void reducedMode();
void receiveCommand();
void sysConf(char,char,char,char);
int readSerial(char);
void GSMDiag(char,char,char,char);
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

#endif