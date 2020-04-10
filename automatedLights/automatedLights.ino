#include <Servo.h> // include servo library

Servo servos[3];
int lastArea = -1; // default area
int sensor[] = { 2, 3, 4, 5 }; // sensor pins
int pins[] = { 9, 10, 11, 12 }; // output pins
const int outputs = 4; // amount of outputs
int repeats[outputs], repeatTimes = 3; // amount of times to repeat writing value (helps if servos are struggling to turn light switch)
bool mode[outputs], lastMode[outputs]; // light mode, true = on, false = off
bool inverted = true; // are relays inverted, true = LOW means on
float timer = 0.5; // delay to turn off lights
unsigned long counterStart[outputs];

// Areas in the video: -1 = none, 0 = living room, 1 = toilet, 2 = entrance, 3 = pins[3]

void setup()
{
  // initialize sensors and set all values to 0
  for(int i = 0; i < outputs; i++)
  {
    pinMode(sensor[i], INPUT);
    mode[i] = false;
    lastMode[i] = mode[i];
    repeats[i] = 0;
    counterStart[i] = 0;
  }

  // initialize outputs
  servos[0].attach(pins[0]);
  servos[1].attach(pins[1]);
  servos[2].attach(pins[2]);
  pinMode(pins[3], OUTPUT);
  
  Serial.begin(9600);
}

void loop()
{
  // set all output modes to off for sensors with no input
  for(int i = 0; i < outputs; i++)
  {
    if(digitalRead(sensor[i]) == 0)
    {
      SetMode(i, false);
    }
  }

  // set modes based on last area occupied in case no sensor has input
  if(digitalRead(sensor[0]) == 0  && digitalRead(sensor[1]) == 0  && digitalRead(sensor[2]) == 0  && digitalRead(sensor[3]) == 0)
  {
    if(lastArea == -1 || lastArea == 2)
    {
      // turn all lights off
      for(int i = 0; i < outputs; i++)
      {
        SetMode(i, false);
      }
    }
    else if(lastArea == 0)
    {
      // only keep area 0 light on
      for(int i = 0; i < outputs; i++)
      {
        SetMode(i, false);
      }
      SetMode(0, true);
    }
    else if(lastArea == 1)
    {
      // only keep area 1 light on
      for(int i = 0; i < outputs; i++)
      {
        SetMode(i, false);
      }
      SetMode(1, true);
    }
    else if(lastArea == 3)
    {
      // only keep area 3 light on
      for(int i = 0; i < outputs; i++)
      {
        SetMode(i, false);
      }
      SetMode(3, true);
    }
  }

  // go through all outputs
  for(int i = 0; i < outputs; i++)
  {
    // set all output modes to on for sensors with input
    if(digitalRead(sensor[i]) == 1)
    {
      SetMode(i, true);
      lastArea = i; // set last area
    }
    
    if(mode[i] != lastMode[i] || repeats[i] < repeatTimes) // check if light mode has changed or writing hasn't been repeated too many times
    {
      lastMode[i] = mode[i]; // set last mode to current mode
      if(mode[i])
      {
        // turn lights on
        if(i >= 0 && i <= 2)
        {
          servos[i].write(80);
        }
        else if(i == 3)
        {
          digitalWrite(pins[3], !inverted);
        }
      }
      else
      {
        // turn lights off
        if(i >= 0 && i <= 2)
        {
          servos[i].write(0);
        }
        else if(i == 3)
        {
          digitalWrite(pins[3], inverted);
        }
      }
      repeats[i]++;
    }
    else if(i >= 0 && i <= 2)
    {
      servos[i].write(40); // set servos to neutral position
    }
  }

  // print data to Serial monitor
  Serial.print("Sensors: ");
  Serial.print(digitalRead(sensor[0]));
  Serial.print(" - ");
  Serial.print(digitalRead(sensor[1]));
  Serial.print(" - ");
  Serial.print(digitalRead(sensor[2]));
  Serial.print(" - ");
  Serial.print(digitalRead(sensor[3]));
  Serial.print(" === Last area: ");
  Serial.print(lastArea);
  Serial.print(" === Modes: ");
  Serial.print(mode[0]);
  Serial.print(" - ");
  Serial.print(mode[1]);
  Serial.print(" - ");
  Serial.print(mode[2]);
  Serial.print(" - ");
  Serial.print(mode[3]);
  Serial.print(" === Repeats: ");
  Serial.print(repeats[0]);
  Serial.print(" - ");
  Serial.print(repeats[1]);
  Serial.print(" - ");
  Serial.print(repeats[2]);
  Serial.print(" - ");
  Serial.println(repeats[3]);

  delay(50); // delay between loops (in milliseconds)
}

void SetMode(int n, bool lMode) // attempt to set area mode (true = on, false = off)
{
  if((millis() - counterStart[n] >= 1000 * timer || lMode == true) && mode[n] != lMode) // check if mode is different and turning on or enough time has passed to turn off
  {
    mode[n] = lMode;
    repeats[n] = 0; // reset repeats
  }
  else if(mode[n] == lMode)
  {
    counterStart[n] = millis(); // reset timer to turn off
  }
}
