#include<wiringPi.h>
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

#define SPEC_CHANNELS 350 // New Spec Channel
int data[SPEC_CHANNELS];

class c12880ma{
  public:

    tcl1543 adc;
    int video, clock, start;
    int delayTime = 1;
    int data[288]; 
    
    c12880ma(int vout, int st, int cl, tcl1543 ADC);
    void read(int time);

  private:
    void clocktick();


};

c12880ma::c12880ma(int vout, int st, int cl, tcl1543 ADC){
  video = vout;
  clock = cl;
  start = st;
  adc = ADC;

  pinMode(clock, OUTPUT);
  pinMode(start, OUTPUT);
  digitalWrite(clock, 1); // Set SPEC_CLK High
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
  digitalWrite(clock, 0);
  delayMicroseconds(delayTime);
  digitalWrite(clock, 1);
  delayMicroseconds(delayTime);
  digitalWrite(clock, 0);
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

  digitalWrite(clock, 1);
  delayMicroseconds(delayTime);
}


/*
 * This functions reads spectrometer data from SPEC_VIDEO
 * Look at the Timing Chart in the Datasheet for more info
 */
void readSpectrometer(tcl1543 adc) {

  int delayTime = 1; // delay time

  // Start clock cycle and set start pulse to signal start
  digitalWrite(SPEC_CLK, LOW);
  delayMicroseconds(delayTime);
  digitalWrite(SPEC_CLK, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(SPEC_CLK, LOW);
  digitalWrite(SPEC_ST, HIGH);
  delayMicroseconds(delayTime);

  //Sample for a period of time
  for (int i = 0; i < 35; i++) {

    digitalWrite(SPEC_CLK, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(SPEC_CLK, LOW);
    delayMicroseconds(delayTime);
  }	
  delay(15);
  // Set SPEC_ST to low
  digitalWrite(SPEC_ST, LOW);

  // Sample for a period of time
  for (int i = 0; i < 85; i++) {

    digitalWrite(SPEC_CLK, HIGH);
    //delayMicroseconds(delayTime);
    digitalWrite(SPEC_CLK, LOW);
    //delayMicroseconds(delayTime);
  }

  // One more clock pulse before the actual read
  digitalWrite(SPEC_CLK, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(SPEC_CLK, LOW);
  delayMicroseconds(delayTime);

  // Read from SPEC_VIDEO
  for (int i = 0; i < SPEC_CHANNELS; i++) {

    data[i] = adc.analogRead(SPEC_VIDEO);

    digitalWrite(SPEC_CLK, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(SPEC_CLK, LOW);
    delayMicroseconds(delayTime);
  }
  /*
  // Set SPEC_ST to high
  digitalWrite(SPEC_ST, HIGH);

  // Sample for a small amount of time
  for (int i = 0; i < 7; i++) {

    digitalWrite(SPEC_CLK, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(SPEC_CLK, LOW);
    delayMicroseconds(delayTime);
  }
*/
  digitalWrite(SPEC_CLK, HIGH);
  delayMicroseconds(delayTime);
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




int main2() {
  
  if (wiringPiSetup() < 0) {
    fprintf(stderr, "Unable to open serial device\n");
    return 1;
  }

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
