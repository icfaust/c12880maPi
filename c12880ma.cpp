#include<wiringPi.h>
#include<vector>
#include<iostream>
#include<fstream>

#include<c12880ma.h>


c12880ma::c12880ma(int vout, int st, int cl, tcl1543 ADC){
  video = vout;
  clock = cl;
  start = st;
  adc = ADC;

  pinMode(clock, OUTPUT);
  pinMode(start, OUTPUT);
  digitalWrite(start, 0);   // Set SPEC_ST Low
  
}


void c12880ma::clocktick(){
  digitalWrite(clock, 1);
  delayMicroseconds(delayTime);
  digitalWrite(clock, 0);
  delayMicroseconds(delayTime);
}


void c12880ma::read(int time){

  // Start clock cycle and set start pulse to signal start
  clocktick();
  clocktick();
  digitalWrite(start, 1);
  delayMicroseconds(delayTime);

  //Sample for a period of time
  for (int i = 0; i < 35; i++) {
    clocktick();
  }	
  delay(time);
  // Set SPEC_ST to low
  digitalWrite(start, 0);

  // Sample for a period of time
  for (int i = 0; i < 85; i++) {

    digitalWrite(clock, 1);
    //delayMicroseconds(delayTime);
    digitalWrite(clock, 0);
    //delayMicroseconds(delayTime);
  }

  // One more clock pulse before the actual read
  clocktick();

  // Read from SPEC_VIDEO
  for (int i = 0; i < 288; i++) {

    data[i] = adc.analogRead(video);
    clocktick();
  }
}


void printData(std::ofstream &myfile){
    for(int count = 0; count < SPEC_CHANNELS; count ++){
        myfile << data[count] << "," ;
    }

}


int main(int argc, char *argv[]) {
  int time = 15;
  if (argc > 1){
    time = std::stoi(argv[1]);
    std::cout << time << std::endl;
  }
  
  if (wiringPiSetup() < 0) {
    fprintf(stderr, "Unable to open serial device\n");
    return 1;
  }
  tcl1543 adc(26,27,28,29,30);
  c12880ma spectrometer(3, 21, 22, adc);

  std::ofstream myfile ("example.txt");

  spectrometer.read(time);

    for(int i=0; i<288; i++){
    data[i] = spectrometer.data[i];

  }
  printData(myfile);
  myfile.close();
}