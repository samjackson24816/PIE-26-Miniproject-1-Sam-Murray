/*
Modes

1. All on
2. Flashing
3. Back and forth
4. SOS

Left: turning Left
Right: turning Right
(w potentiometer)

*/



int SWITCH_BUTTON = 8;
int LEDs[] = {5, 6, 7, 9, 10, 11, 12, 13};
int LED_NUM = 8;
int POT_PIN = A5;


// The potentiometer is a steering wheel. Near the middle means driving
// straight, turning it far enough either way asks for a turn signal, which
// takes priority over whichever mode is selected.
int POT_CENTER = 512;   // analogRead value with the knob centered
int POT_DEADZONE = 150; // how far the knob must move before a turn registers

// Steering positions
int STEER_STRAIGHT = 0;
int STEER_LEFT = 1;
int STEER_RIGHT = 2;


void setup() {
  pinMode(SWITCH_BUTTON, INPUT_PULLUP);
  pinMode(POT_PIN, INPUT);
  for (int led = 0; led < LED_NUM; led++) {
    pinMode(LEDs[led], OUTPUT);
  }

  Serial.begin(9600);
}



int mode = 0; // will be 0, 1, 2, 3, 4
int MODE_NUM = 5;


double timeCounter = 0.0; // time since last mode switch
double timeOffset = 0.0; // Helps us reset timeCounter
bool lastButtonState = HIGH;

unsigned long Press_Start_Time = 0;
unsigned long Long_Press_Ms = 3000;

bool Leds_Off = false;

int Last_Steering = STEER_STRAIGHT;

double steerOffset = 0.0; // Same idea as timeOffset, but for the turn signal

void loop() {
  int buttonState = digitalRead(SWITCH_BUTTON);
  if (lastButtonState == HIGH && buttonState == LOW) {

    Press_Start_Time = millis();

    Serial.println("BUTTON PRESSED");
}

// Button just released
// LOW -> HIGH
if (lastButtonState == LOW && buttonState == HIGH) {

    unsigned long pressTime =
        millis() - Press_Start_Time;

    Serial.print("BUTTON RELEASED, press time = ");
    Serial.println(pressTime);

    // Long press
    if (pressTime >= Long_Press_Ms) {

        Leds_Off = true;

        Serial.println("LONG PRESS -> OFF");
    }

    // Short press
    else {

        mode++;

        if (mode >= MODE_NUM) {
            mode = 0;
        }

        Leds_Off = false;

        timeOffset = millis() / 1000.0;

        Serial.print("SHORT PRESS -> MODE = ");
        Serial.println(mode);
    }
}


// Save current state for next loop
  lastButtonState = buttonState;
  timeCounter = (millis() / 1000.0) - timeOffset;



  // Restart the turn signal whenever the steering wheel moves, so the sweep
  // always begins at its first frame
  int steering = readSteering();

  if (steering != Last_Steering) {
    Last_Steering = steering;
    steerOffset = millis() / 1000.0;
  }

  double steerTime = (millis() / 1000.0) - steerOffset;


  bool ledVals[] = {false, false, false, false, false, false, false, false};


  if (steering == STEER_LEFT) {
    Serial.println("Turning left");
    signalLeft(ledVals, steerTime);
  } else if (steering == STEER_RIGHT) {
    Serial.println("Turning right");
    signalRight(ledVals, steerTime);
  } else {
    switch (mode) {
      case 0:
        Serial.println("Off");
        break;
      case 1:
        Serial.println("All on");
        modeAllOn(ledVals, timeCounter);
        break;
      case 2:
        Serial.println("Flashing");
        modeFlashing(ledVals, timeCounter);
        break;
      case 3:
        Serial.println("Back and forth");
        modeBackAndForth(ledVals, timeCounter);
        break;
      case 4:
        Serial.println("SOS");
        modeSOS(ledVals, timeCounter);
        break;
    }
  }

  // Long press overrides everything
  if (Leds_Off) {
    for (int i = 0; i < LED_NUM; i++) {
        ledVals[i] = false;
    }
}
  
  // Send the values to the LEDs
  for (int led = 0; led < LED_NUM; led++) {
    if (ledVals[led] == true) {
      digitalWrite(LEDs[led], HIGH);
    } else {
      digitalWrite(LEDs[led], LOW);
    }
  }

}


void modeAllOn(bool ledVals[], double time) {
  unsigned int sequence[] = {
    0b11111111
  };

  int sequenceLen = 1;

  sequenceTemplate(ledVals, time, sequence, sequenceLen, 1);
}


void modeFlashing(bool ledVals[], double time) {
  unsigned int sequence[] = {
    0b11111111,
    0b00000000
  };

  int sequenceLen = 2;

  sequenceTemplate(ledVals, time, sequence, sequenceLen, 1);
}


void modeBackAndForth(bool ledVals[], double time) {

  unsigned int sequence[] = {
    0b10000000,
    0b01000000,
    0b00100000,
    0b00010000,
    0b00001000,
    0b00000100,
    0b00000010,
    0b00000001,
    0b00000010,
    0b00000100,
    0b00001000,
    0b00010000,
    0b00100000,
    0b01000000
  };
  
  int sequenceLen = 14;

  sequenceTemplate(ledVals, time, sequence, sequenceLen, 1);
}


void modeSOS(bool ledVals[], double time) {

  unsigned int sequence[] = {
    0b11111111,
    0b00000000,
    0b11111111,
    0b00000000,
    0b11111111,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11111111,
    0b11111111,
    0b11111111,
    0b00000000,
    0b11111111,
    0b11111111,
    0b11111111,
    0b00000000,
    0b11111111,
    0b11111111,
    0b11111111,
    0b00000000,
    0b11111111,
    0b00000000,
    0b11111111,
    0b00000000,
    0b11111111,
    0b00000000,
    0b00000000,
    0b00000000
  };
  
  int sequenceLen = 28;

  sequenceTemplate(ledVals, time, sequence, sequenceLen, 0.1);
}


// The low end of the potentiometer is a left turn and the high end is a right
// turn. Swap the two returns if the knob feels backwards on your build.
int readSteering() {
  int potVal = analogRead(POT_PIN);

  if (potVal < POT_CENTER - POT_DEADZONE) {
    return STEER_LEFT;
  }

  if (potVal > POT_CENTER + POT_DEADZONE) {
    return STEER_RIGHT;
  }

  return STEER_STRAIGHT;
}


// Fills up towards LEDs[0] (pin 9), then blanks before repeating
void signalLeft(bool ledVals[], double time) {
  unsigned int sequence[] = {
    0b10000000,
    0b11000000,
    0b11100000,
    0b11110000,
    0b11111000,
    0b11111100,
    0b11111110,
    0b11111111,
    0b00000000
  };

  int sequenceLen = 9;

  sequenceTemplate(ledVals, time, sequence, sequenceLen, 1.5);
}


// Mirror of signalLeft: fills up towards LEDs[4] (pin 13)
void signalRight(bool ledVals[], double time) {
  unsigned int sequence[] = {
    0b00000001,
    0b00000011,
    0b00000111,
    0b00001111,
    0b00011111,
    0b00111111,
    0b01111111,
    0b11111111,
    0b00000000
  };

  int sequenceLen = 9;

  sequenceTemplate(ledVals, time, sequence, sequenceLen, 1.5);
}




// Sequence is binary numbers signifying which leds lit up, eg 00101 means 3rd and 5th are lit
void sequenceTemplate(bool ledVals[], double time, unsigned int sequence[], int sequenceLength, double periodPerSec) {

  // Rem increases from 0 to 1 repeatingly
  double rem = (time * periodPerSec) - floor(time * periodPerSec);

  // For example: to get a loop of 0 through 7 we can multiply by 8 and floor
  int index = floor(rem * sequenceLength);

  for (int i = 0; i < LED_NUM; i++) {
    ledVals[i] = false;

    unsigned int mask = 1 << i;

    unsigned int on = (sequence[index] & mask) >> i;

    if (on) {
      ledVals[i] = true;
    }
  }
}











