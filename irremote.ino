/* dump.ino Example sketch for IRLib2
   Illustrates how to receive an IR signal, decode it and print
   information about it to the serial monitor.
*/

#define IRLIB_TRACE

//This includes everything. Not generally recommended.
//It's better to include only the parts of library you really need.
//But for this example it's quick and easy. See "comboDump" example
//for a more efficient way.
#include "IRLibAll.h"
#include <IRLib_HashRaw.h>    //Only use raw sender

IRrecvPCI myReceiver(2); //create receiver and pass pin number
IRdecode myDecoder;   //create decoder
#define GND 4
#define VCC 5

IRsend mySender;
IRsendRaw myRawSender;
#define RAW_DATA_LEN 68
const PROGMEM uint16_t rawData[RAW_DATA_LEN] = {
  4626, 4334, 722, 1506, 714, 1518, 734, 1498, 
  722, 394, 710, 406, 714, 422, 690, 426, 
  678, 438, 690, 1542, 694, 1534, 698, 1518, 
  726, 406, 698, 418, 678, 438, 690, 426, 
  678, 438, 674, 442, 666, 450, 662, 1570, 
  674, 442, 670, 446, 670, 446, 666, 450, 
  666, 450, 662, 1570, 666, 1566, 662, 454, 
  670, 1562, 674, 1558, 690, 1538, 682, 1538, 
  722, 1506, 730, 1000};

#define INPUT_SIZE 400
#define RAW_DATA_BUF_MAX_LEN 100
uint16_t rawDataBuf[RAW_DATA_BUF_MAX_LEN];
uint16_t rawDataBufLen;
char input[INPUT_SIZE + 1]; // add space for '\0'
    
void setup() {
  pinMode(GND, OUTPUT);
  pinMode(VCC, OUTPUT);
  digitalWrite(GND, LOW);
  digitalWrite(VCC, HIGH);
  Serial.begin(115200);
  myReceiver.enableIRIn(); // Start the receiver
  Serial.println(F("Ready to receive IR signals"));
  for (uint8_t i = 1; i < LAST_PROTOCOL+1; i++) { Serial.print(i); Serial.print(F(" = ")); Serial.println(Pnames(i)); }
  Serial.println();
}

void loop() {
  //Continue looping until you get a complete signal received
  if (myReceiver.getResults()) {
    Serial.println(F("received"));
    Serial.println(F("-------------------------------------"));
    Serial.println(F("---decode---"));
    myDecoder.decode();           //Decode it
    myDecoder.dumpResults(false);  //Now print results. Use false for less detail
    Serial.println(F("Do a cut-and-paste of the following lines into the "));
    Serial.println(F("designated location in rawSend.ino"));
    Serial.print(F("\n#define RAW_DATA_LEN "));
    Serial.println(recvGlobal.recvLength, DEC);
    Serial.print(F("const PROGMEM uint16_t rawData[RAW_DATA_LEN] = {\n\t"));
    for(bufIndex_t i=1; i<recvGlobal.recvLength; i++) {
      Serial.print(recvGlobal.recvBuffer[i], DEC);
      Serial.print(F(", "));
      if( (i % 8)==0) Serial.print(F("\n\t"));
    }
    Serial.println(F("1000};"));//Add arbitrary trailing space
    Serial.println();
    Serial.print(F("raw,"));
    for(bufIndex_t i=1; i<recvGlobal.recvLength-1; i++) {
      Serial.print(recvGlobal.recvBuffer[i], DEC);
      Serial.print(F(","));
    }
    Serial.println(recvGlobal.recvBuffer[recvGlobal.recvLength-1]);
    //Serial.println(F("1000"));//Add arbitrary trailing space
    myReceiver.enableIRIn();      //Restart receiver
  }
  if (Serial.available()) {
    Serial.println(F("serial"));
    myReceiver.disableIRIn();

    uint16_t size = Serial.readBytesUntil('\n', input, INPUT_SIZE);
    input[size] = 0; //add terminator
    //get rid of extra carriage returns and line feeds (CR = '\r' 0x0D, LF = '\n' 0x0A)
    for (uint16_t i = 0; i < size; i++) if (input[i] == '\r' || input[i] == '\n') input[i] = '\0';
    Serial.print("\""); Serial.print(input); Serial.println("\""); 
    
    char* command = strtok(input, ",");
    Serial.println(command);
    uint8_t p = atoi(command);

    uint16_t cnt = 0;
    char* valueStr = strtok(0, ",");
    uint16_t hex = strtol(valueStr, NULL, 16);
    while (valueStr != 0)  {
      rawDataBuf[cnt++] = atoi(valueStr);
      valueStr = strtok(0, ",");
    }
    rawDataBufLen = cnt;
    //for (uint16_t i=0; i<rawDataBufLen; i++) Serial.println(rawDataBuf[i]);

    if (p > 0 && p < LAST_PROTOCOL+1) {
      Serial.println(F("protocol!"));
      Serial.println(Pnames(p));
      //mySender.send(SONY,0xa8bca, 20);//Sony DVD power A8BCA, 20 bits
      //uint32_t cmd = rawDataBuf[0];
      //uint32_t samsung = 0xE0E00000 | (cmd << 8) | (cmd^255);
      uint32_t samsung = 0xE0E00000 | (hex << 8) | (hex^255);
      Serial.println(samsung, HEX);
      mySender.send(p, samsung, 32);
      Serial.println(F("Sent signal."));
    }

    if (strcasecmp(command, "raw") == 0 || p == 0) {
      //printf("\nraw\n");
      Serial.println(F("\nraw\n"));
      myRawSender.send(rawDataBuf, rawDataBufLen, 38);
      Serial.println(F("Sent signal."));
    }

    if (strcasecmp(command, "test") == 0) {
      Serial.println(F("\ntest\n"));
      //myRawSender.send(rawData, RAW_DATA_LEN, 38);
      mySender.send(13, 0xE0E0D02F, 32);
      Serial.println(F("Sent signal."));
    }

    while (Serial.read() != -1);
    myReceiver.enableIRIn();
    Serial.println(F("jeba"));

  }
  
}
