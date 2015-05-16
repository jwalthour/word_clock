// John Walthour
// December 2011
// A gift for Erin Waldron

// Arduino interface pin numbers
int CLK_PIN = 3;
int DATA_PIN = 2;
int STR_PIN = 4;
int OE_PIN = 5;

int NUM_PINS = 32;

// Indices into button array
int HR_UP = 0;
int HR_DN = 1;
int MIN_UP = 2;
int MIN_DN = 3;
int TIME_SET_PIN[4] = {8, 9, 10, 11};

unsigned long DEBOUNCE_TIME_MS = 50; // time button must be depressed

void writeLeds(int pin[32])
{
  for(int i = NUM_PINS - 1; i >= 0; i--)
  {
    digitalWrite(DATA_PIN, pin[i]);
		digitalWrite(CLK_PIN, HIGH);
		digitalWrite(CLK_PIN, LOW);
  }
  digitalWrite(STR_PIN, HIGH);
  delay(2);
  digitalWrite(STR_PIN, LOW);
  delay(2);
}

void clearPins(int pin[32])
{
  for(int i = 0; i < NUM_PINS; i++)
  {
    pin[i] = LOW;
  }
}

void setup()
{
  pinMode(CLK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(STR_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);

  analogWrite(OE_PIN, 0);
  digitalWrite(STR_PIN, LOW);
  digitalWrite(CLK_PIN, LOW);
  digitalWrite(DATA_PIN, LOW);  
  
  for(int i = 0; i < 4; i++)
  {
    pinMode(TIME_SET_PIN[i], INPUT);
    digitalWrite(TIME_SET_PIN[i], HIGH);
  }

  int pin[32];
  clearPins(pin);
  writeLeds(pin);
}

// Takes in a time and outputs it to the LED array
void displayTime(int hour, int minute, int second)
{
  float fractional_minute = (float)minute + (float)second / 60.0;
  int hour_to_display = 12;
  
  // Turn on just "it is"
  int pin[NUM_PINS];
  clearPins(pin);
  pin[5] = HIGH;
  
  // Expressed as [hour] o'clock
  if(fractional_minute < 2.5 || fractional_minute > 57.5)
  {
    // turn on "o'clock"
    pin[27] = pin[28] = HIGH;
    
    // determine appropriate hour
    if(fractional_minute < 2.5)
    {
      hour_to_display = hour;
    }
    else // must be close to an hour
    {
      if(hour == 11) { hour_to_display = 0; }
      else { hour_to_display = hour + 1; }
    }
  }
  // Expressed as past the hour
  else if(fractional_minute < 35.0)
  {
    // turn on "past"
    pin[9] = HIGH;
    
    hour_to_display = hour;
    
    if(fractional_minute < 7.5)
    { pin[4] = HIGH; } // turn on "five"
    else if (fractional_minute < 12.5)
    { pin[3] = HIGH; } // turn on "ten"
    else if (fractional_minute < 17.5)
    { pin[1] = pin[2] = HIGH; } // turn on "quarter"
    else
    { pin[10] = pin[11] = HIGH; } // turn on "twenty"
    // There's no "half" on this clock.  Oops.
  }
  else
  // Expressed as "to" the hour
  {
    // turn on "to"
    pin[8] = HIGH;

    // next hour
    if(hour == 11) { hour_to_display = 0; }
    else { hour_to_display = hour + 1; }
    
    if(fractional_minute > 52.5)
    { pin[4] = HIGH; } // turn on "five"
    else if (fractional_minute > 47.5)
    { pin[3] = HIGH; } // turn on "ten"
    else if (fractional_minute > 42.5)
    { pin[1] = pin[2] = HIGH; } // turn on "quarter"
    else
    { pin[10] = pin[11] = HIGH; } // turn on "twenty"
    // There's no "half" on this clock.  Oops.
  }
  
  // Light up pin(s) for the hour
  switch(hour_to_display)
  {
    case 0: pin[29] = pin[30] = HIGH; break;  // 12
    case 1: pin[6] = HIGH; break;
    case 2: pin[16] = HIGH; break;
    case 3: pin[12] = HIGH; break;
    case 4: pin[20] = HIGH; break;
    case 5: pin[19] = HIGH; break;
    case 6: pin[18] = HIGH; break;
    case 7: pin[17] = HIGH; break;
    case 8: pin[26] = HIGH; break;
    case 9: pin[25] = HIGH; break;
    case 10: pin[24] = HIGH; break;
    case 11: pin[21] = pin[22] = HIGH; break;
  }
  writeLeds(pin);
}

// This is a 12 hour clock
unsigned long time = millis();
int second = 0;
int minute = 0;
int hour = 0; // 0 means 12

// Used for button debouncing
int reading[4] = {HIGH, HIGH, HIGH, HIGH};
int lastReading[4] = {HIGH, HIGH, HIGH, HIGH};
int buttonState[4] = {HIGH, HIGH, HIGH, HIGH};
int lastButtonState[4] = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastTransitionTime[4] = {0, 0, 0, 0};

////////////////////////////////////////////////////
//  Main Loop
////////////////////////////////////////////////////

void loop()
{  
  // Update time as necessary
  if(time > millis())
  { // handle rollover
    time = millis() + 296;// the spare time in a rollover
  }
  if(millis() - time >= 999)
  {
    // Seconds boundary, compute time
    time = millis();
    second++;
    if(second > 59)
    {
      second = 0;
      minute++;
      if(minute > 59)
      {
        minute = 0;
        hour++;
        if(hour > 11)
        {
          hour = 0;// 0 means 12
        }
      }
    }
    
    // Update display
    if(second == 10)
    {
      // fade out
      for(int i = 128; i >= 0; i--)
      {
        analogWrite(OE_PIN, i);
        delay(4);
      }
    }
    else if (second == 11)
    {
      // Light up "I love you"
      int pin[NUM_PINS];
      clearPins(pin);
      pin[13] = pin[14] = HIGH;
      writeLeds(pin);
      for(int i = 0; i < 255; i++)
      {
        analogWrite(OE_PIN, i);
        delay(2);
      }
    }
    else if (second == 20)
    {
      // Fade it out
      for(int i = 255; i >= 0; i--)
      {
        analogWrite(OE_PIN, i);
        delay(2);
      }
    }
    else if (second == 21)
    {
      // Write the real time
      displayTime(hour, minute, second);
      for(int i = 0; i < 128; i++)
      {
        analogWrite(OE_PIN, i);
        delay(4);
      }
    }
  }
  
  // Accept button input
  for(int i = 0; i < 4; i++)
  {
    reading[i] = digitalRead(TIME_SET_PIN[i]);
    if(millis() < lastTransitionTime[i])
    { // Handle rollover
      lastTransitionTime[i] = millis();
    }
    if(reading[i] != lastReading[i])
    { // Actual transition
      lastTransitionTime[i] = millis();
    }
    else if((millis() - lastTransitionTime[i]) > DEBOUNCE_TIME_MS)
    {
      buttonState[i] = reading[i];
    }
    lastReading[i] = reading[i];
  }

  if(buttonState[HR_UP] == LOW && lastButtonState[HR_UP] == HIGH)
  {
    hour++;
    if(hour > 11) { hour = 0; }
    displayTime(hour, minute, second);
    analogWrite(OE_PIN, 128);
  }
  else if(buttonState[HR_DN] == LOW && lastButtonState[HR_DN] == HIGH)
  {
    hour--;
    if(hour < 0) { hour = 11; }
    displayTime(hour, minute, second);
    analogWrite(OE_PIN, 128);
  }
  else if(buttonState[MIN_UP] == LOW && lastButtonState[MIN_UP] == HIGH)
  {
    minute += 5; 
    minute -= (minute % 5); // round to the nearest 5 mins
    if(minute >= 59) { minute = 0; }
    second = 0; // doesn't reset millis to make rollover easier
    displayTime(hour, minute, second);
    analogWrite(OE_PIN, 128);
  }
  else if(buttonState[MIN_DN] == LOW && lastButtonState[MIN_DN] == HIGH)
  {
    minute -= (minute % 5); // round to the nearest 5 mins
    minute -= 5; 
    if(minute < 0) { minute = 55; }
    second = 0; // doesn't reset millis to make rollover easier
    displayTime(hour, minute, second);
    analogWrite(OE_PIN, 128);
  }
  
  for(int i = 0; i < 4; i++)
  {
    lastButtonState[i] = buttonState[i];
  }
}



