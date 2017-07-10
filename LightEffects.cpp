#include "LightEffects.hpp"

extern Waiter leff_waiter = Waiter();

void fire()
{
  const static int ir = 255;
  const static int ig = 80;
  
  const static int dir = 230;
  const static int dig = 30;
  
  static int cr = 0;
  static int cg = 0;
  static int cb = 0;
  
  static int action = 0;

  if(!leff_waiter.isStillWaiting())
  {
    action = random(0, 2);
        
    if(action == 0) // Heller
    {
      cr += random(0, ir - cr);
      cg += random(0, ig - cg);
    }
    else if(action == 1) // dunkler
    {
      cr -= random(0, cr - dir);
      cg -= random(0, cg - dig);
    }
    else if(action == 2)
    {
      cr = random(0, 40);
      cg = random(0, 10);
    }
      
    setRGB(cr, cg, cb);
    leff_waiter.wait(random(60, 150));
  }
}

void train()
{
  const static int bhfProb = 7;
  const static int pollerFehlerProb = 20;
  static int zyklen = 0;
  static int zyklenBeendet = 0;
  static double grmult = 1, bmult = 0;
  static bool bahnhof = false;
  static int v = 1;
  static int mode = 2;
  static int blinkCounter = 0;
  static int state2 = 0;

  if(!leff_waiter.isStillWaiting())
  {
    switch(state2)
     {
      case 0:
        mode = random(0, bhfProb);
        v = 15; //random(3, 7);
        bahnhof = (mode == 0);
        zyklenBeendet = 0;
        if(bahnhof)
        {
          zyklen = random(60 / v, 120 / v);
          grmult = 1;
          bmult = 0;
        }
        else
        {
          zyklen = random(40 / v, 90 / v);
          grmult = 0.7;
          bmult = 1;
        }
        state2 = (state2 + 1) % 4;
      break;
      case 1:
        if(blinkCounter < (255 / v))
        {
          setRGB(blinkCounter * v * grmult, blinkCounter * v * grmult, blinkCounter * v * bmult);
          
          if(bahnhof)
            leff_waiter.wait(250 / (255 / v));
          else
            leff_waiter.wait(500 / (255 / v));
          blinkCounter++;
        }
        else
          state2 = (state2 + 1) % 4;
      break;
      case 2:
        if(blinkCounter > 0)
        {
           setRGB(blinkCounter * v * grmult, blinkCounter * v * grmult, blinkCounter * v * bmult);
      
           if(bahnhof)
             leff_waiter.wait(250 / (255 / v));
           else
             leff_waiter.wait(500 / (255 / v));
           blinkCounter--;
        }
        else
          state2 = (state2 + 1) % 4;
      break;
      case 3:
        if(!bahnhof)
          leff_waiter.wait((500 + random(0, 250)) / v);
         
        if(zyklenBeendet < zyklen)
        {
          zyklenBeendet++;
          state2 = 1;
        }
        else
          state2 = (state2 + 1) % 4;
      break;
     }
  }
}
