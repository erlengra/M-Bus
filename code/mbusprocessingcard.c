#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ctype.h>
#include <avr/wdt.h>
#include <GSM.h>
#include <avr/pgmspace.h>

//#include <M-Bus>
//#include <commandList.h>

/*draws characters on OLED*/
void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }    
  display.display();
  delay(1);
}



/*animated text in a scrolling manner for OLED*/
void testscrolltext(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  display.println("M-BUS");
  display.display();
  delay(1);
 
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);    
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
}



/*sends from master to slave over RS-232*/
void SWprint(int data){
  byte mask;
  //startbit
  digitalWrite(tx,LOW);
  delayMicroseconds(bit9600Delay);
  for (mask = 0x01; mask>0; mask <<= 1) {
    if (data & mask){ // choose bit
     digitalWrite(tx,HIGH); // send 1
    }
    else{
     digitalWrite(tx,LOW); // send 0
    }
    delayMicroseconds(bit9600Delay);
  }
  //stop bit
  digitalWrite(tx, HIGH);
  delayMicroseconds(bit9600Delay);
}



/*reads from slave over RS-232*/
int SWread(){
  byte val = 0;
  while (digitalRead(rx));
  //wait for start bit
  if (digitalRead(rx) == LOW) {
    delayMicroseconds(halfBit9600Delay);
    for (int offset = 0; offset < 8; offset++) {
     delayMicroseconds(bit9600Delay);
     val |= digitalRead(rx) << offset;
    }
    //wait for stop bit + extra
    delayMicroseconds(bit9600Delay);
    delayMicroseconds(bit9600Delay);
    return val;
  }
}



/*scans the AtMega GPIO for the correct I2C address and returns the HEX value of that address*/
char i2c_scanner(){
  byte error, address;
  int nDevices;

  Serial.println("\nI2C Scanner");

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);

      nDevices++;
      return address;
    }
    else if (error==4) 
    {
      Serial.print("Unknown error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);           // wait 5 seconds for next scan
}



/*displays all relevant information about the SD card. Also used as a test to access the card*/
void SD_cardinfo(){

  Serial.print("\nInitializing SD card...");

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);


  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
  Serial.println("\nSD card initialized");
}



/*init function for OLED. Includes pictures and text*/
void OLED_intro(){
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  // draw scrolling text
  testscrolltext();
  delay(2000);
  display.clearDisplay();

  // text display tests
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(20,5);
  display.println("M-Bus");
  display.setTextColor(WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();
}



/*command: sendDataPoint. Upon recieved command, the function fetches datapoint from memor
and sends to modem for trasmission. Datapiont is determined by, date and timestamp*/


/*upon exeption or command issued, this function fetches all files on SD-card and sends this to 
selected reciever.*/
void dumpMemory(){
  Serial.print("Dumping memory...");
  File dataFile = SD.open("datalog.txt");

  // if the file is available, write to it:
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {

    Serial.println("error opening datalog.txt");
  }
}



/*selfcheck to confirm correct operational conditions. returns a set of values stored as a separate 
file on SD-card with timestamp. Runs each time a new loggFile is generated or on request. */
void diagnostic(){
  SD_cardinfo();
  i2c_scanner();
}



/*if th user issues the restart command, from diagnostic or from exeption, this clears all registers and restarts
all subsystems. Finally reboots the bootloader.*/
void restart(){
  wdt_init();
  asm volatile ("  jmp 0");
}



/* Sets parameters for sleepMode, extra low power consumption. disables
all functionality except storing from sensor. Is timer-based and deactvates modem for operation
is active for at least 1 minute*/
void sleepMode(){
}



/* no restrictions on operation.*/
void normalMode(){
}



/* Sets parameters for reduced mode of operation.
function for reduced power consumption. stores all values, but sends only every 10 sek. 
only "average" measurement is available as a processing value.*/
void reducedMode(){
}



/*functionality for handling incoming command from modem*/
void recieveCommand(){
  
}



/*reset function. Uses AVR watchdog timer to disable register update and resets PP*/
void wdt_init(void){
    MCUSR = 0;
    wdt_disable();

    return;
}


/* tool for extracting commands or text from terminal or any other serial medium.
uses a simple buffer to hold all receiving characters and waits until the last character
is the \n before returning the buffer-list
*/
int readSerial(char result[]){
  int i = 0;
  while(1)
  {
    while (Serial.available() > 0)
    {
      char inChar = Serial.read();
      if (inChar == '\n')
      {
        result[i] = '\0';
        return 0;
      }
      if(inChar!='\r')
      {
        result[i] = inChar;
        i++;
      }
    }
  }
}


/* Main function when establishing connection to a GSM/GPRS(TCP) network. Parameters are global and configured during 
SysConf. Contact network provider for more information.*/ 
void GSMDiag(char login, char password, char APN, char proxy){
  use_proxy = false;
  Serial.print("Connecting GSM network...");
  if(gsmAccess.begin(PINNUMBER)!=GSM_READY)
  {
    Serial.println(GPRSerror);
    while(true);
  }
  Serial.println(oktext);
  Serial.print("login with user: ");
  Serial.print(login);
  Serial.println("Password for login: ");
  Serial.print(password);
  Serial.println("APN address: ");
  Serial.print(APN);
  if (use_proxy){
    Serial.println("proxy-address in use: ");
    Serial.print(proxy);
  }
  Serial.println("Attaching to GPRS with native APN...");
  if(gprsAccess.attachGPRS(APN, login, password)!=GPRS_READY)
  {
    Serial.println(errortext);
  }
  else{
    Serial.println(oktext);
  }
}



void sysConf(char login, char password, char APN, char proxy){
  Serial.println("########################################################################");
  Serial.println("########################################################################");
  Serial.println("########################################################################");
  Serial.println("######################## SYSTEM CONFIGURATION ##########################");
  Serial.println("########################################################################");
  Serial.println("########################################################################");
  Serial.println("########################################################################");
  Serial.println("");
  Serial.println("Insert parameters for all listed parameters:");
  Serial.println("");
  Serial.println("GSM login: ");
  readSerial(login);
  Serial.print(login);
  Serial.println("");
  Serial.println("GSM APN: ");
  readSerial(APN);
  Serial.println(APN);
  Serial.println("");
  Serial.println("GSM password: ");
  readSerial(password);
  Serial.println(password);
  Serial.println("");
  Serial.println("PROXY address: ");
  readSerial(proxy);
  Serial.println(proxy);
  Serial.println("");
  Serial.println("");
}