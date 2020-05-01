
/*
             define from  wiringPi.h                     define from Board
   DVK511 3.3V | | 5V               ->                 3.3V | | 5V 8/SDA | | 5V
   ->                  SDA | | 5V 9/SCL | | GND              -> SCL | | GND 7 |
   | 15/TX            ->                  IO7 | | TX GND | | 16/RX            ->
   GND | | RX 0 | | 1                ->                  IO0 | | IO1 2 | | GND
   ->                  IO2 | | GND 3 | | 4                -> IO3 | | IO4 VCC | |
   5                ->                  VCC | | IO5 MOSI/12 | | GND -> MOSI | |
   GND MISO/13 | | 6                ->                 MISO | | IO6 SCK/14 | |
   10/CE0           ->                  SCK | | CE0 GND | | 11/CE1           ->
   GND | | CE1 30/ID_SD| | 31/ID_SC         ->                ID_SD | | ID_SC
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

class tcl1543 {
private:
  unsigned int EOC;
  unsigned int Clock;
  unsigned int DataIn;
  unsigned int DataOut;
  unsigned int ChipSelect;
  bool flag;

  void clocktick(void);

public:
  tcl1543(unsigned int mEOC, unsigned int CL, unsigned int address,
          unsigned int dout, unsigned int CS);

  void sync(bool flag);
  unsigned int analogRead(unsigned int channel);

}

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
      std::cout << (int)digitalRead(DataOut) << ",";
      Wait2us;
      clocktick();
    }
    std::cout << std::endl;
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

  for (i = 0; i < 16;
       i++) { // input the channel to be read    Channel = Channel << 4;
    if (i < 4) {
      digitalWrite(DataIn, (unsigned char)(channel >> 7) & 0x01);
      channel <<= 1;
    }

    if (i < 10 && !digitalRead(DataOut))
      flag = true; // if the code does not list 10 1's move the sync
    Wait2us;
    clocktick();
  }

  for (i = 0; i < 16; i++) { // read data and set channel back to ref+
    if (i < 4)
      digitalWrite(DataIn, Chan[i]); // tell the next pass to return 10 1's
    if (i < 10) {
      output <<= 1;
      output |= digitalRead(DataOut, Ch);
    }
    Wait2us;
    clocktick();
  }
  digitalWrite(ChipSelect, 0);

  sync(); // rerun sync if there are dataout problems
  return output;
}

int analogRead(unsigned char Channel) {
  int ConvertValue;
  unsigned char i, Chan;
  unsigned char ConvertValueL, ConvertValueH;
  unsigned char delay;

  ConvertValueL = ConvertValueH = 0; // Initialize conversion results
  delay = 0;
  if (digitalRead(EOC)) {
    digitalWrite(Clock, 0);
    digitalWrite(ChipSelect, 1);
    Wait2us;
    digitalWrite(ChipSelect, 0);
    Wait2us;
    Channel = Channel << 4;
    for (i = 0; i < 4; i++) // Enter the channel number to be digitized
    {
      Chan = Channel;
      Chan = Chan >> 7;
      digitalWrite(DataIn, Chan & 0x01);
      Wait2us;
      digitalWrite(Clock, 1);
      digitalWrite(Clock, 0);
      Channel = Channel << 1;
    }
    for (i = 0; i < 6; i++) // Input conversion clock
    {
      digitalWrite(Clock, 1);
      digitalWrite(Clock, 0);
    }
    digitalWrite(ChipSelect, 1);
    // Start detection of conversion end flag, or conversion timeout error
    while ((!digitalRead(EOC)) && (delay < 10)) {
      Wait10us;
      delay++;
    }
    if (delay == 10) {
      return (0xFFFF); // Conversion timeout, return error code
    } else {
      Wait10us;
      digitalWrite(Clock, 0);
      digitalWrite(ChipSelect, 1);
      Wait1us;
      digitalWrite(ChipSelect, 0);
      Wait1us;
      for (i = 0; i < 2; i++) // Read the upper two bits
      {
        digitalWrite(Clock, 1);
        ConvertValueH <<= 1;
        if (digitalRead(DataOut))
          ConvertValueH |= 0x1;
        digitalWrite(Clock, 0);
        Wait1us;
      }
      for (i = 0; i < 8; i++) // Read the lower 8 bits
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
      return ConvertValue; // Return digitized value
    }
  }
}

int main() {
  int re;
  unsigned char d1, d2, d3, d4;

  if (wiringPiSetup() < 0) {
    fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
    return 1;
  }

  pinMode(EOC, INPUT);
  pullUpDnControl(EOC, PUD_UP);
  pinMode(DataOut, INPUT);
  pullUpDnControl(DataOut, PUD_UP);

  pinMode(Clock, OUTPUT);
  pinMode(DataIn, OUTPUT);
  pinMode(ChipSelect, OUTPUT);

  while (1) {
    /* if (digitalRead (LED) == 0)
       printf ("0\n") ;
     delay (500) ;*/
    printf("AD: %d%d%d%d \n", d1, d2, d3, d4);
    re = analogRead(0);
    d1 = re / 1000;
    d2 = re / 100 % 10;
    d3 = re / 10 % 10;
    d4 = re % 10;
    delay(100);
  }
}
