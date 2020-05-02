
/* 
             define from  wiringPi.h                     define from Board DVK511
                 3.3V | | 5V               ->                 3.3V | | 5V
                8/SDA | | 5V               ->                  SDA | | 5V
                9/SCL | | GND              ->                  SCL | | GND
                    7 | | 15/TX            ->                  IO7 | | TX
                  GND | | 16/RX            ->                  GND | | RX
                    0 | | 1                ->                  IO0 | | IO1
                    2 | | GND              ->                  IO2 | | GND
                    3 | | 4                ->                  IO3 | | IO4
                  VCC | | 5                ->                  VCC | | IO5
              MOSI/12 | | GND              ->                 MOSI | | GND
              MISO/13 | | 6                ->                 MISO | | IO6
               SCK/14 | | 10/CE0           ->                  SCK | | CE0
                  GND | | 11/CE1           ->                  GND | | CE1
              30/ID_SD| | 31/ID_SC         ->                ID_SD | | ID_SC
                    21| | GND              ->                  SDA | | 5V
                    22| | 26               ->                  SCL | | GND
                    23| | GND              ->                  IO7 | | TX
                    24| | 27               ->                  GND | | RX
                    25| | 28               ->                  IO0 | | IO1
                   GND| | 29               ->                  IO2 | | GND
                  */


#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <wiringPi.h>
#include <tcl1543.h>

tcl1543::tcl1543(unsigned int mEOC, unsigned int CL, unsigned int address,
                 unsigned int dout, unsigned int CS) {
  EOC = mEOC;
  Clock = CL;
  DataIn = address;
  DataOut = dout;
  ChipSelect = CS;
  flag = true;

  pinMode(EOC, INPUT);
  pullUpDnControl(EOC, PUD_UP);
  pinMode(DataOut, INPUT);
  pullUpDnControl(DataOut, PUD_UP);

  pinMode(Clock, OUTPUT);
  pinMode(DataIn, OUTPUT);
  pinMode(ChipSelect, OUTPUT);

  sync();
}

void tcl1543::clocktick() {
  digitalWrite(Clock, 1);
  Wait2us;
  digitalWrite(Clock, 0);
  Wait2us;
}

void tcl1543::sync(void) {
  bool flag1 = false;
  unsigned char i, j;
  unsigned char Chan[4] = {1, 1, 0, 1};

  while (flag) {
    j = 16;
    digitalWrite(Clock, 0);
    digitalWrite(ChipSelect, 1);
    Wait2us;
    digitalWrite(ChipSelect, 0);
    Wait2us;

    for (i = 0; i < j; i++) {
      if (i < 4)
        digitalWrite(DataIn, Chan[i]); // tell the next pass to return 10 1's
      if (i < 10 && !digitalRead(DataOut) && flag1)
        j = 15; // if the code does not list 10 1's move the sync

      Wait2us;
      clocktick();
    }

    if (j == 16 && flag1)
      flag = false; // if not in sync, do it again, but only do this starting
                    // the second time around

    flag1 = true; // its has been successfully delayed
    digitalWrite(ChipSelect, 0);
  }
}

unsigned int tcl1543::analogRead(unsigned int channel) {
  unsigned int output = 0;
  unsigned char i;
  unsigned char Chan[4] = {1, 1, 0, 1};

  channel <<= 4;

  digitalWrite(Clock, 0);
  digitalWrite(ChipSelect, 1);
  Wait2us;
  digitalWrite(ChipSelect, 0);
  Wait2us;

  for (i = 0; i < 16; i++) { // input the channel to read
    if (i < 4) {
      digitalWrite(DataIn, (unsigned char)(channel >> 7) & 0x01);
      channel <<= 1;
    }

    if (i < 10 && !digitalRead(DataOut))
      flag = true; // if the code does not list 10 1's it is out of sync
    Wait2us;
    clocktick();
  }

  for (i = 0; i < 16; i++) { // read data and set channel back to ref+
    if (i < 4)
      digitalWrite(DataIn, Chan[i]); // tell the next pass to return 10 1's
    if (i < 10) {
      output <<= 1;
      output |= digitalRead(DataOut);
    }
    Wait2us;
    clocktick();
  }
  digitalWrite(ChipSelect, 0);

  //sync(); // rerun sync if there are dataout problems
  Wait2us;
  return output;
}

/*
int main() {
  int re, cntr = 0;
  unsigned char d1, d2, d3, d4;
  
  if (wiringPiSetup() < 0) {
    fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
    return 1;
  }

  tcl1543 adc(26,27,28,29,30);

  while (1) {
    /* if (digitalRead (LED) == 0)
       printf ("0\n") ;
     delay (500) ;*//*
    printf("%d AD: %d%d%d%d \n",cntr, d1, d2, d3, d4);
    re = adc.analogRead(11);

    d1 = re / 1000;
    d2 = re / 100 % 10;
    d3 = re / 10 % 10;
    d4 = re % 10;
    cntr++;
  }
}*/
