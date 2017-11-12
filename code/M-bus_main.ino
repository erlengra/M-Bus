/*********************************************************************
Code Author: Erlend Grande 
credits: Supporting libraries from Adafruit and Arduino.

This is the main code for communication, storage, processing and transmission between 
M-bus, ASM, arduinoMega and external modem.
Setup includes:
1# SD card serially through USART SPI connected
2# OLED  monitor connected through I2C
3# RS-232 COM against M-bus transciever
4# USART SPI COM against modem.
5# digital and analog Input from external power module (including power).   
*********************************************************************/

#include <mbusprocessingcard.h>


/*initiates all serial communication. It runs initialization of SD card with all info available
If the storage of the card is too little, an exeption is raised and user is notified. This should 
result in a memorydump before proceeding.
i2cscanner is scanning all ports on AtMega for the correct i2c address. it should return 0x3c, but is allowed
to be an arbitrary value. oled intro starts the oled driver and display the welcome pictures.*/
void setup()   {
  Wire.begin();                
  Serial.begin(9600);
  Serial1.begin(2400);
  while (!(Serial&&Serial1)) {
    ; // wait for serial ports to connect. Needed for native USB port only
  }
  sysconf(login,password,APN,proxy);
  OLED_intro();
  diagnostic();
}

/*main loop*/
void loop() {
  SWval = SWread();
  SWprint(toupper(SWval));
}


