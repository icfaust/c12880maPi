#ifndef C12880MA_H
#define C12880MA_H


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

#endif /* C12880MA_H */