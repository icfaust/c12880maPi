 
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

unsigned int ADCSelChannel(unsigned char Channel) {
  unsigned int ConvertValue;
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

int main() {
  unsigned int re;
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
  pinMode(24, OUTPUT);

  while (1) {
    /* if (digitalRead (LED) == 0)
       printf ("0\n") ;
     delay (500) ;*/
    digitalWrite(24, 1);
    printf("AD: %d%d%d%d \n", d1, d2, d3, d4);
    re = ADCSelChannel(0);
    d1 = re / 1000;
    d2 = re / 100 % 10;
    d3 = re / 10 % 10;
    d4 = re % 10;
    delay(100);
    digitialWrite(24, 0);
    delay(1000);
  }
}