#include "LightEffects.hpp"
#include "FS.h"

Ticker timer;

Waiter crawl_waiter;
Waiter dec_waiter;

void button_change_isr()
{
  bpressed = digitalRead(BUTTON) == HIGH ? false : true;
  bTime = 0;
}

void storeData(int *data, size_t size)
{
  if(canUseSPIFFS)
  {
    File p = SPIFFS.open("/data.txt", "w");
  
    char *towrite = (char*) malloc(size * sizeof(char));
  
    for(int i = 0; i < size; i++)
    {
      *(towrite + i) = char(*(data + i));
    }
    
    if(p)
    {
      p.println(towrite);
    }
  
    free(towrite);

    p.close();
  }
}

struct packet
{
  int *data;
  size_t size;
};

struct packet readData()
{
  struct packet r;

  r.data = NULL;
  r.size = 0;
  
  if(canUseSPIFFS)
  {
    File p = SPIFFS.open("/data.txt", "r");
    if(p)
    {
      r.data = (int*) malloc(sizeof(int) * 4);
      String rawdata = p.readStringUntil('\n');
      for(int i = 0; i < 4; i++)
      {
        if(i < rawdata.length())
          *(r.data + i) = int(rawdata[i]);
      }

      r.size = 4;
    }

    p.close();
  }

  return r;
}

void timerISR()
{
  static unsigned long int bpwCounter = 0;
  static unsigned int confirmCounter = 0;
  static unsigned long int buttonTriggered = 0, buttonLastSignal = 0;
  static bool isChecking = false;
  static unsigned int menu = 0;
  static bool enter = false;
  static unsigned int cdecay = 0;
  static bool oldbstate = false;
  struct packet pdata;
  
  bpwCounter++;

  if(bpressed != oldbstate)
  {
    Serial.print("Buttonstate: ");
    Serial.println(bpressed);
    oldbstate = bpressed;
  }

  if(crawlEstates)
  {
    if(!crawl_waiter.isStillWaiting())
    {
      currentEffect = (currentEffect + 1) % 3;
      crawl_waiter.wait(5000);
    }

    switch(currentEffect)
    {
      case 0:
        fire();
      break;
      case 1:
        train();
      break;
      case 2:
        setRGB(0, 0, 0);
      break;
    }
  }

  if(crawlFstates)
  {
    if(!crawl_waiter.isStillWaiting())
    {
      currentFanState = (currentFanState + 1) % 9;
      setRGB(0, 255, 0);
      setFan(fanstates[currentFanState]);
      if(currentFanState != 0)
        crawl_waiter.wait(7000);
      else
        crawl_waiter.wait(7000);
    }
    else if(crawl_waiter.getRemainingTime() < 1000)
      setRGB(0, 0, 0);
  }

  if(crawlFanDecay)
  {
    if(!crawl_waiter.isStillWaiting())
    {
      if(currentFanDecay < 100)
      {
        currentFanDecay++;
        fanMoves = true;
        setFan(100 - currentFanDecay);
        crawl_waiter.wait(333);
      }
      else if(currentFanDecay == 100)
      {
        crawl_waiter.wait(10000);
        fanMoves = false;
        setFan(0);
        currentFanDecay = 0;
      }
      else
        crawl_waiter.wait(1000);
    }
  }

  if(configured)
  {
    if(data[1] == 1)
    {
      if(!dec_waiter.isStillWaiting())
      {
        if(cdecay < 100)
          cdecay++;
        dec_waiter.wait(data[3] * 15 * 1000 * 60 / 100);
        Serial.print("Current Decay: ");
        Serial.println(cdecay);
      }
    }

    setFan(fanstates[data[0]] - (fanstates[data[0]] * cdecay / 100));
    
    if(cdecay != 100)
    {
      switch(data[2])
      {
        case 0:
          fire();
        break;
        case 1:
          train();
        break;
        case 2:
          setRGB(0, 0, 0);
        break;
      }
    }
    else
      setRGB(0, 0, 0);
  }

  if(bpressed && !isChecking)
  {
    bpwCounter = 0;
    isChecking = true;
    confirmCounter = 0;
    if(menu != 5)
      setRGB(255, 0, 0);
    else
      setRGB(0, 255, 255);
  }
  else if(bpressed && isChecking)
  {
    buttonLastSignal = bpwCounter;
    confirmCounter = 0;
  }
  else if(!bpressed && isChecking)
  {
    isChecking = false;
    
    long int offsetTime = buttonLastSignal - buttonTriggered;

    setRGB(0, 0, 0);
    if(offsetTime <= shortpress && offsetTime > btnjumpignore && !configured)
    {
      Serial.println("[Event] Shortpress");
      confirmCounter = 0;
      if(menu == 5)
      {
        data[menu - 2]++;
        enter = true;

        storeData(data, sizeof(data) / sizeof(int));
      }
      else if(menu != 0)
      {
        switch(menu)
        {
          case 2:
            data[menu - 2] = currentFanState;
            storeData(data, sizeof(data) / sizeof(int));
          break;
          case 3:
            data[menu - 2] = fanMoves ? 1 : 0;
            storeData(data, sizeof(data) / sizeof(int));
          break;
          case 4:
            data[menu - 2] = currentEffect;
            storeData(data, sizeof(data) / sizeof(int));
          break;
        }

        menu = 0;
        crawlFstates = false;
        crawlEstates = false;
        crawlFanDecay = false;
        setFan(0);
        setRGB(0, 0, 0);
        enter = true;
      }
      else
        timesbpressed++;
    }
    else if(offsetTime > shortpress && offsetTime < longpress)
    {
      Serial.println("[Event] reset (longpress)");
      timesbpressed = 0;
      confirmCounter = 0;
      setRGB(0, 0, 0);
      setFan(0);
      menu = 0;
      cdecay = 0;
      configured = false;
      crawlFstates = false;
      crawlEstates = false;
      crawlFanDecay = false;

      for(int i = 0; i < sizeof(data) / sizeof(int); i++)
      {
        data[i] = 0;
      }
    }

    buttonLastSignal = 0;
    buttonTriggered = 0;
    bpwCounter = 0;
  }
  
  if(timesbpressed > 0 || enter)
    confirmCounter++;
  
  if(!bpressed && confirmCounter > tvok && (timesbpressed > 0 || enter))
  {
    Serial.println("[Event] Virtual OK");
    confirmCounter = 0;
    enter = false;
    if(menu == 0)
    {
      menu = timesbpressed;
      timesbpressed = 0;
      switch(menu)
      {
         case 2:
          crawlFstates = true;
         break;
         case 3:
          crawlFanDecay = true;
          setRGB(255, 0, 255);
         break;
         case 4:
          crawlEstates = true;
         break;
         case 1:
          configured = true;
          crawlFstates = false;
          crawlEstates = false;
          menu = 0;
          cdecay = 0;
          setRGB(0, 0, 0);
          setFan(0);

          pdata = readData();

          if(pdata.size > 0)
          {
            for(int i = 0; i < pdata.size; i++)
            {
              data[i] = *(pdata.data + i);
            }
          }
         break;
         case 5:
          setRGB(0, 255, 255);
         break;
         default:
          menu = 0;
         break;
      }
    }
    else if(menu == 5)
      menu = 0;
  }
}

void setup()
{
  Serial.begin(9600);
  canUseSPIFFS = SPIFFS.begin();
  analogWriteFreq(18000);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(LDR, INPUT);
  pinMode(BUTTON, INPUT);
  pinMode(FANSPEED, OUTPUT);
  pinMode(FANDATA, INPUT);
  attachInterrupt(BUTTON, button_change_isr, CHANGE);
  timer.attach_ms(1, timerISR);
  setRGB(0, 0, 0);

  if(canUseSPIFFS)
  {
    struct packet pdata = readData();

    if(pdata.size > 0)
    {
      for(int i = 0; i < pdata.size; i++)
      {
        data[i] = *(pdata.data + i);
      }
    }
  }
}

void loop() {}
