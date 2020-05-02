#include<wiringPi>
#include<vector>
#include<iostream>
#include<fstream>

#include <tcl1543.h>

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
void readSpectrometer(tcl1543 *adc) {

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

    data[i] = adc->analogRead(SPEC_VIDEO);

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

  tcl1543 adc(26,27,28,29,30);

  std::ofstream myfile ("example.txt");

  digitalWrite(SPEC_CLK, HIGH); // Set SPEC_CLK High
  digitalWrite(SPEC_ST, LOW);   // Set SPEC_ST Low

  readSpectrometer(adc);
  printData(myfile);
  myfile.close();
}
