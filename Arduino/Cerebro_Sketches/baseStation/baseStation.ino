/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MIT License

Copyright (c) 2015-2018 Andy S. Lustig

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//Documentation for this project can be found at https://karpova-lab.github.io/cerebro/

#include <Radio.h>  //https://github.com/LowPowerLab/RFM69
#include <SPI.h>
#include <EEPROM.h>
#include <avr/wdt.h>

#define CHANNEL_ADDRESS 0

const uint8_t VERSION = 0; //2018-11-14

const uint8_t LED = 13;
const uint8_t TRIGGER_PIN = 6;
const uint8_t STOP_PIN = 5;

uint8_t netID;

Radio radio(8,7); //slave select pin, interrupt pin
WaveformData newWaveform;
WaveformData currentWaveform;
IntegerPayload integerMessage;
Info cerebroInfo;

uint32_t  valsFromParse[7];
uint32_t  startTime = 0;
uint32_t  spamFilter = 0;
uint32_t  triggerClock = 0;
uint16_t  msgCount = 0;
bool      ignoreFilter = false;
bool      warningsOn = true;

void setup() {
  Serial1.begin(57600);
  delay(10);
  pinMode(LED, OUTPUT);

  EEPROM.get(CHANNEL_ADDRESS,netID);
  radio.radioSetup(1,false,netID); //nodeID, autopower off;
  pinMode(TRIGGER_PIN,INPUT);
  pinMode(STOP_PIN,INPUT);
  startTime = millis();
  newSession();
}

void loop() {
  ////////////Receive Message From Bcontrol
  if (digitalRead(TRIGGER_PIN)) {
    while(digitalRead(TRIGGER_PIN)){
      //wait until signal goes low
    }
    triggerCommandReceived();
  }
  //if we read a high signal on pin 6, send a stop command to cerebro
  if (digitalRead(STOP_PIN)) {
    while(digitalRead(STOP_PIN)){
      //wait until signal goes low
    }
    stopCommandReceived();
  }

  ///////////Receive Message From Xavier//////////////////
  if (Serial1.available()){
    char msg = Serial1.read();
    if (msg=='W'){  //parse data then send new waveform
      parseData();
      sendWaveformUpdate();
    }
    else if (msg=='S' || msg == 'L' || msg == 'R' ||  msg == 'l' ||  msg == 'r'){ //parse data then send radio message with integer
      parseData();
      delay(500);
      sendMsgAndVal(msg,valsFromParse[0]);
    }
    else if (msg=='T'){
      triggerCommandReceived();
    }
    else if (msg=='A'){
      stopCommandReceived();
    }
    else if(msg=='M'){
      requestMissed();
    }
    else if(msg=='N'){
      newSession();
    }
    else if (msg=='K'){
      parseData();
      uint8_t newNetworkID = valsFromParse[0];
      Serial1.print("\nChanging to channel:");
      Serial1.println(newNetworkID);
      EEPROM.update(CHANNEL_ADDRESS, newNetworkID);
      Serial1.println("Restarting...");
      wdt_enable(WDTO_15MS);  // turn on the WatchDog timer
      while(1){}              // do nothing and wait for the reset
    }
    else if(msg=='?'){
      Serial1.print("Channel: ");
      Serial1.println(netID);
      Serial1.print("Base Version,");Serial1.print(VERSION);newline();
    }
    else if (msg!='\n'){
      relayMsg(msg);
    }
  }

  ///////////Receive Message From Cerebro///////
  if (radio.receiveDone()){
    switch (radio.DATALEN) {
      case sizeof(cerebroInfo):
        if (radio.ACKRequested()){
          radio.sendACK();
        }
        cerebroInfo = *(Info*)radio.DATA;
        printCerebroInfo();
        break;
      case sizeof(currentWaveform):
        if (radio.ACKRequested()){
          radio.sendACK();
        }
        currentWaveform = *(WaveformData*)radio.DATA;
        spamFilter = currentWaveform.startDelay + currentWaveform.trainDur;
        warningsOn = false;
        printWaveform(currentWaveform,true);
        break;
      case  sizeof(integerMessage):
        if (radio.ACKRequested()){
          radio.sendACK();
        }
        integerMessage = *(IntegerPayload*)radio.DATA;
        if(integerMessage.variable == 'B'){
          printBattery(integerMessage.msgCount,integerMessage.value);
        }
        else if (integerMessage.variable == 'M'){
          Serial1.print("Total Missed,");Serial1.print(integerMessage.value);newline();
        }
        else if (integerMessage.variable =='m'){
          Serial1.print("Missed Message Index,");Serial1.print(integerMessage.value);newline();
        }
        else if (integerMessage.variable == 'Y'){
          Serial1.print("Cerebro turned on and connected,");Serial1.print(integerMessage.value);//print time it took to startup and send connection message.
        }
        break;
    }
  }
}

void parseData(){
  char msgData[30] = "";
  Serial1.readBytesUntil('\n',msgData,30);
  char* msgPointer;
  msgPointer = strtok(msgData,",");
  char i = 0;
  while (msgPointer!=NULL){
    valsFromParse[i] = atol(msgPointer);
    msgPointer = strtok(NULL,",");
    i++;
  }
}

void sendMsgAndVal(char msg,unsigned int val){
  integerMessage.variable = msg;
  integerMessage.value = val;
  Serial1.print("\nSending '"); Serial1.print(msg);Serial1.print("' ") ;Serial1.print(integerMessage.value);Serial1.print("...");
  if (radio.sendWithRetry(CEREBRO, (const void*)(&integerMessage), sizeof(integerMessage),5)){
    Serial1.print("data received");newline();
  }
  else{
    Serial1.print("*X&data send fail");newline();
  }
}

void newSession(){
  startTime = millis();
  msgCount = 0;
  Serial1.print("\n*BaseOn&Base Version,");Serial1.print(VERSION);newline();
  uint32_t theVals[1] = {netID};
  sendDataToXavier("Base Channel",theVals,1);
  Serial1.print("Base Channel,");Serial1.print(netID);newline();

  char msg = 'N';
  if (radio.sendWithRetry(CEREBRO, &msg, 1, 3)){
    // Serial1.print("Connected!");
  }
  else{
    Serial1.print("*X&Error communicating with Cerebro\n\n");
  }
}

void relayMsg(char msg){
  if (radio.sendWithRetry(CEREBRO, &msg, 1, 0)){  // 0 = only 1 attempt, no retries
  }
  else{
    Serial1.print("*X&");
    printTime();Serial1.print("Tried Sending ''");Serial1.print(msg);
    Serial1.print("'', ACK not received");newline();
  }
}

void printCerebroInfo(){
  uint32_t theVals[2] = {cerebroInfo.firmware, cerebroInfo.serialNumber};
  sendDataToXavier("Cerebro Info",theVals,2);
  Serial1.print("Cerebro Version,");Serial1.print(cerebroInfo.firmware);newline();
  Serial1.print("Serial Number,");Serial1.print(cerebroInfo.serialNumber);newline();
}


void printBattery(uint16_t batteryMsgCount, uint8_t batteryStatus){
  printTime();
  Serial1.print("[");Serial1.print(batteryMsgCount);Serial1.print("]");
  uint32_t theVals[] = {batteryStatus};
  uint8_t numElements = sizeof(theVals)/sizeof(uint32_t);
  sendDataToXavier("Battery",theVals,numElements);
  printToBaseMonitor("Battery",theVals,numElements);
}

void printWaveform(WaveformData wave, bool response){
  printTime();
  uint32_t theVals[] = {wave.startDelay, wave.posAmp, wave.posDur, wave.negAmp, wave.negDur,wave.freq, wave.trainDur};
  uint8_t numElements = sizeof(theVals)/sizeof(uint32_t);
  sendDataToXavier("Waveform",theVals,numElements);
  if(response){
    Serial1.print("[");Serial1.print(wave.msgCount);Serial1.print(']');
  }
  else{
    Serial1.print(msgCount);
  }
  printToBaseMonitor("Waveform",theVals,numElements);
}


void sendWaveformUpdate(){
  newWaveform.startDelay = valsFromParse[0];
  newWaveform.posAmp = valsFromParse[1];
  newWaveform.posDur = valsFromParse[2];
  newWaveform.negAmp = valsFromParse[3];
  newWaveform.negDur = valsFromParse[4];
  newWaveform.freq = valsFromParse[5];
  newWaveform.trainDur = valsFromParse[6];
  newWaveform.msgCount = msgCount++;
  printWaveform(newWaveform,false);
  if (radio.sendWithRetry(CEREBRO, (const void*)(&newWaveform), sizeof(newWaveform))){
    currentWaveform = newWaveform;
    ignoreFilter = true;    
  }
  else{
    Serial1.print("*X&Waveform Update Failed\n");
  }
}

void requestMissed(){
  msgCount++;
  integerMessage.variable = 'M';
  integerMessage.value = msgCount;
  printTime();Serial1.print(msgCount);comma();Serial1.print("Misses Requested");newline();
  if(radio.sendWithRetry(CEREBRO, (const void*)(&integerMessage), sizeof(integerMessage),3)){
    //
  }
  else{
    printTime();Serial1.print(msgCount);comma();Serial1.print("No Miss Request Ack");newline();
  }
}

void triggerCommandReceived(){
  uint32_t  tSinceTrigger = millis() - triggerClock;
  if (tSinceTrigger>spamFilter || ignoreFilter){
    msgCount++;
    integerMessage.variable = 'T';
    integerMessage.value = msgCount;
    printTime();Serial1.print(msgCount);comma();Serial1.print("Trigger");newline();
    radio.send(CEREBRO, (const void*)(&integerMessage), sizeof(integerMessage));
    triggerClock = millis();
  }
  else{
    msgCount++;
    integerMessage.variable = 'C';
    integerMessage.value = msgCount;
    printTime();Serial1.print(msgCount);comma();Serial1.print("Continue");newline();
    radio.send(CEREBRO, (const void*)(&integerMessage), sizeof(integerMessage));
    triggerClock = millis();
  }
  ignoreFilter = false;
}

void stopCommandReceived(){
  uint32_t  tSinceTrigger = millis() - triggerClock;
  if (tSinceTrigger<spamFilter){
    msgCount++;
    integerMessage.variable = 'A';
    integerMessage.value = msgCount;
    printTime();Serial1.print(msgCount);comma();Serial1.print("Abort");newline();
    if (radio.sendWithRetry(CEREBRO, (const void*)(&integerMessage), sizeof(integerMessage),3)){
      triggerClock = -spamFilter; //prevents back to back stop signals from being sent
    }
    else{
      printTime();Serial1.print(msgCount);comma();Serial1.print("No Abort Ack");newline();
    }
  }
  else{
    printTime();Serial1.print(msgCount);comma();Serial1.print("Spam Filtered Abort");newline();
  }
}

void sendDataToXavier(char* parameterName, uint32_t theValues[], uint8_t numValues){
  Serial1.print("*");Serial1.print(parameterName);tilda();
  Serial1.print(theValues[0]);
  for (uint8_t i  = 1; i < numValues; i++){
    tilda();
    Serial1.print(theValues[i]);
  }
  Serial1.print("&");
}

void printToBaseMonitor(char* parameterName, uint32_t theValues[], uint8_t numValues){
  Serial1.print(",");Serial1.print(parameterName);comma();
  Serial1.print(theValues[0]);
  for (uint8_t i  = 1; i < numValues; i++){
    dash();
    Serial1.print(theValues[i]);
  }
  newline();
}

void printTime(){
  uint32_t currentTime = millis()-startTime;
  Serial1.print(currentTime);comma();
}

void comma(){
  Serial1.print(",");
}

void tilda(){
  Serial1.print("~");
}

void dash(){
  Serial1.print("-");
}

void newline(){
  Serial1.print("\n");
}