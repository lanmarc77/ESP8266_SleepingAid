#ifndef GLOBALS_HPP
#define GLOBALS_HPP
#include "Waiter.hpp"
#include "Arduino.h"
#include <math.h>
#include <Ticker.h>

extern const int RED;
extern const int GREEN;
extern const int BLUE;
extern const int LDR;
extern const int BUTTON;
extern const int FANSPEED;
extern const int FANDATA;

extern const int btnjumpignore;
extern const int shortpress;
extern const int longpress;
extern const int tvok;


extern bool buttonState;
extern unsigned int timesbpressed;
extern bool vok;
extern bool configured;

extern unsigned int currentFanState;
extern unsigned int currentEffect;
extern unsigned int currentFanDecay;

extern unsigned int bTime;

extern bool crawlFstates;
extern bool crawlEstates;
extern bool crawlFanDecay;
extern bool fanMoves;
extern bool bpressed;
extern bool canUseSPIFFS;

extern const unsigned short fanstates[9];
extern int data[4];

extern void setRGB(int, int, int);

extern void setFan(unsigned short);

#endif
