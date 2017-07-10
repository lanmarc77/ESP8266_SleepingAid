#include "Globals.hpp"

const int RED = 15;
const int GREEN = 12;
const int BLUE = 13;
const int LDR = 0xA0;
const int BUTTON = 4;
const int FANSPEED = 16;
const int FANDATA = 14;

const int btnjumpignore = 50;
const int shortpress = 800;
const int longpress = 2000;
const int tvok = 2100;

bool buttonState = false;
unsigned int timesbpressed = 0;
bool vok = false;
bool configured = false;

unsigned int currentFanState = 0;
unsigned int currentEffect = 0;
unsigned int currentFanDecay = 0;

unsigned int bTime = 0;

bool crawlFstates = false;
bool crawlEstates = false;
bool crawlFanDecay = false;
bool fanMoves = false;
bool bpressed = false;
bool canUseSPIFFS = false;

const unsigned short fanstates[9] = { 8, 15, 20, 40, 60, 80, 100, 0, 0 };
int data[4] = { 5, 1, 1, 4 };

void setRGB(int r, int g, int b)
{
  analogWrite(RED, r * 4);
  analogWrite(GREEN, g * 4);
  analogWrite(BLUE, b * 4);
}

void setFan(unsigned short speed)
{
  analogWrite(FANSPEED, 1023 * speed / 100);
}
