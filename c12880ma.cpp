#include<wiringPi>
#include<vector>
#include<iostream>
#include<fstream>

///////
#include <errno.h>
#include <stdio.h>
//#include"tcl1543.h"
///////

/*
 * Macro Definitions
 */
#define SPEC_TRG 10
#define SPEC_ST 21
#define SPEC_CLK 22
#define SPEC_VIDEO 3
#define WHITE_LED 24
#define LASER_404 25

#define HIGH 1
#define LOW 0

#define SPEC_CHANNELS 288 // New Spec Channel
int data[SPEC_CHANNELS];

///////////////////////////////////////
#define EOC 26
#define Clock 27
#define DataIn 28
#define DataOut 29
#define ChipSelect 30

#define Wait1us delayMicroseconds(1);
#define Wait2us delayMicroseconds(2);
#define Wait4us                                                                \
  {                                                                            \
    Wait2us;                                                                   \
    Wait2us;                                                                   \
  }
#define Wait8us                                                                \
  {                                                                            \
    Wait4us;                                                                   \
    Wait4us;                                                                   \
  }
#define Wait10us                                                               \
  {                                                                            \
    Wait8us;                                                                   \
    Wait2us;                                                                   \
  }

int analogRead(unsigned char Channel) {
  int ConvertValue;
  unsigned char i, Chan;
  unsigned char ConvertValueL, ConvertValueH;
  unsigned char delay;

  ConvertValueL = ConvertValueH = 0; //Initialize conversion results
  delay = 0;
  if (digitalRead(EOC)) {
    digitalWrite(Clock, 0);
    digitalWrite(ChipSelect, 1);
    Wait2us;
    digitalWrite(ChipSelect, 0);
    Wait2us;
    Channel = Channel << 4;
    for (i = 0; i < 4; i++) //Enter the channel number to be digitized
    {
      Chan = Channel;
      Chan = Chan >> 7;
      digitalWrite(DataIn, Chan & 0x01);
      Wait2us;
      digitalWrite(Clock, 1);
      digitalWrite(Clock, 0);
      Channel = Channel << 1;
    }
    for (i = 0; i < 6; i++) //Input conversion clock
    {
      digitalWrite(Clock, 1);
      digitalWrite(Clock, 0);
    }
    digitalWrite(ChipSelect, 1);
    //Start detection of conversion end flag, or conversion timeout error
    while ((!digitalRead(EOC)) && (delay < 10)) {
      Wait10us;
      delay++;
    }
    if (delay == 10) {
      return (0xFFFF); //Conversion timeout, return error code
    } else {
      Wait10us;
      digitalWrite(Clock, 0);
      digitalWrite(ChipSelect, 1);
      Wait1us;
      digitalWrite(ChipSelect, 0);
      Wait1us;
      for (i = 0; i < 2; i++) //Read the upper two bits
      {
        digitalWrite(Clock, 1);
        ConvertValueH <<= 1;
        if (digitalRead(DataOut))
          ConvertValueH |= 0x1;
        digitalWrite(Clock, 0);
        Wait1us;
      }
      for (i = 0; i < 8; i++) //Read the lower 8 bits
      {
        digitalWrite(Clock, 1);
        ConvertValueL <<= 1;
        if (digitalRead(DataOut))
          ConvertValueL |= 0x1;
        digitalWrite(Clock, 0);
        Wait1us;
      }
      digitalWrite(ChipSelect, 1);
      ConvertValue = ConvertValueH;
      ConvertValue <<= 8;
      ConvertValue |= ConvertValueL;
      return ConvertValue; //Return digitized value
    }
  }
}




//////////////////////////////////////







/*c12880ma::c12880ma(int trig, int start, int clock, ADC t1543){
  this.trig = trig;
  this.clock = clock;
  this.start = start;
  this.video = t1543;
  
}

void c12880ma::read(){


}*/


/*
 * This functions reads spectrometer data from SPEC_VIDEO
 * Look at the Timing Chart in the Datasheet for more info
 */
void readSpectrometer() {

  int delayTime = 1; // delay time

  // Start clock cycle and set start pulse to signal start
  digitalWrite(SPEC_CLK, LOW);
  delayMicroseconds(delayTime);
  digitalWrite(SPEC_CLK, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(SPEC_CLK, LOW);
  digitalWrite(SPEC_ST, HIGH);
  delayMicroseconds(delayTime);

  // Sample for a period of time
  for (int i = 0; i < 15; i++) {

    digitalWrite(SPEC_CLK, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(SPEC_CLK, LOW);
    delayMicroseconds(delayTime);
  }

  // Set SPEC_ST to low
  digitalWrite(SPEC_ST, LOW);

  // Sample for a period of time
  for (int i = 0; i < 85; i++) {

    digitalWrite(SPEC_CLK, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(SPEC_CLK, LOW);
    delayMicroseconds(delayTime);
  }

  // One more clock pulse before the actual read
  digitalWrite(SPEC_CLK, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(SPEC_CLK, LOW);
  delayMicroseconds(delayTime);

  // Read from SPEC_VIDEO
  for (int i = 0; i < SPEC_CHANNELS; i++) {

    data[i] = analogRead(SPEC_VIDEO);

    digitalWrite(SPEC_CLK, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(SPEC_CLK, LOW);
    delayMicroseconds(delayTime);
  }

  // Set SPEC_ST to high
  digitalWrite(SPEC_ST, HIGH);

  // Sample for a small amount of time
  for (int i = 0; i < 7; i++) {

    digitalWrite(SPEC_CLK, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(SPEC_CLK, LOW);
    delayMicroseconds(delayTime);
  }

  digitalWrite(SPEC_CLK, HIGH);
  delayMicroseconds(delayTime);
}

void printData(std::ofstream myfile){
    for(int count = 0; count < SPEC_CHANNELS; count ++){
        myfile << data[count] << "," ;
    }

}

int main() {
  pinMode(SPEC_CLK, OUTPUT);
  pinMode(SPEC_ST, OUTPUT);
  pinMode(LASER_404, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);

/////FOR DIGITIZER
  pinMode(EOC, INPUT);
  pullUpDnControl(EOC, PUD_UP);
  pinMode(DataOut, INPUT);
  pullUpDnControl(DataOut, PUD_UP);

  pinMode(Clock, OUTPUT);
  pinMode(DataIn, OUTPUT);
  pinMode(ChipSelect, OUTPUT);
///////////

  std::ofstream myfile ("example.txt");

  digitalWrite(SPEC_CLK, HIGH); // Set SPEC_CLK High
  digitalWrite(SPEC_ST, LOW);   // Set SPEC_ST Low

  readSpectrometer();
  printData(myfile);
  myfile.close();
}
