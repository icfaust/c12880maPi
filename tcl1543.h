
#ifndef TCL1543_H
#define TCL1543_H
#include <stdio.h>
#include <string.h>

#include <wiringPi.h>

#define Wait1us delayMicroseconds(1);
#define Wait2us delayMicroseconds(2);

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
  tcl1543(unsigned int, unsigned int, unsigned int,
          unsigned int, unsigned int);

  void sync(void);
  unsigned int analogRead(unsigned int);

};

#endif /* TCL1543_H */