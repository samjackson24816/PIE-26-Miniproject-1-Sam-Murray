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
int LEDs[] = {9, 10, 11, 12, 13};
int LED_NUM = 5;
int POT_PIN = A5;

// The potentionmeter varaiables
int Center = 512;
int Dead_Zone = 120;
int Steer_Straight = 0;
int Steer_Left = 1;
int Steer_Right = 2;

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

bool lastSwitchButtonState = false;


int Last_Steering = Steer_Straight;
double Steer_Offset = 0.0;


void loop() {

  // It's an input pullup so the value is inverted
  int switchButtonVal = digitalRead(SWITCH_BUTTON);
  bool switchButtonState = true;
  if (switchButtonVal == HIGH) {
    switchButtonState = false;
  }

  // Detect when the button has just been pressed down
  bool buttonPressed = false;

  if (switchButtonState != lastSwitchButtonState) {
    
    lastSwitchButtonState = switchButtonState;

    if (switchButtonState == true) {
      buttonPressed = true;
      timeOffset = millis() / 1000.0;
    }
  }

  timeCounter = (millis() / 1000.0) - timeOffset;


  if (buttonPressed) {
    mode++;
    if (mode >= MODE_NUM) {
      mode = 0;
    }
  }

  
  int Steering = readSteering();

  int (Steering != lastSteering) {
    lastSteering = Steering;
    Steer_Offset = millis() / 1000.00 // seconds
  }

  double SteerTime = (milllis() / 1000.00) -Steer_Offset

  bool ledVals[] = {false, false, false, false, false};

  if (Steering == Steer_Left) {
    Serial.println("Turning left");
    signalLeft(ledVals, SteerTime);
  } else if (Steering == Steer_Right) {
    Serial.println("Turning right");
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

int readSteering() {
  int potVal = analogRead(POT_PIN);

  if (potVal < Center - Dead_Zone) {
    return Steer_Left;
  }

  if (potVal > Center + Dead_Zone) {
    return Steer_Right;
  }

  return Steer_Straight
}

// 
void signalLeft(bool ledVals[], double time) {
  unsigned int sequence[] = {
    0b10000,
    0b11000,
    0b11100,
    0b11110,
    0b11111,
    0b00000
  };

  int sequenceLen = 6;

  sequenceTemplate(ledVals, time, sequence, sequenceLen, 1.5);
}

void signalRight(bool ledVals[], double time) {
  unsigned int sequence[] = {
    0b00001,
    0b00011,
    0b00111,
    0b01111,
    0b11111,
    0b00000
  };

  int sequenceLen = 6;

  sequenceTemplate(ledVals, time, sequence, sequenceLen, 1.5);
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
    0b11111
  };

  int sequenceLen = 1;

  sequenceTemplate(ledVals, time, sequence, sequenceLen, 1);
}


void modeFlashing(bool ledVals[], double time) {
  unsigned int sequence[] = {
    0b11111,
    0b00000
  };

  int sequenceLen = 2;

  sequenceTemplate(ledVals, time, sequence, sequenceLen, 1);
}


void modeBackAndForth(bool ledVals[], double time) {

  int sequence[] = {
    0b10000,
    0b01000,
    0b00100,
    0b00010,
    0b00001,
    0b00010,
    0b00100,
    0b01000
  };
  
  int sequenceLen = 8;

  sequenceTemplate(ledVals, time, sequence, sequenceLen, 1);
}


void modeSOS(bool ledVals[], double time) {

  int sequence[] = {
    0b11111,
    0b00000,
    0b11111,
    0b00000,
    0b11111,
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b00000,
    0b11111,
    0b00000,
    0b11111,
    0b00000,
    0b11111,
    0b00000,
    0b00000,
    0b00000
  };
  
  int sequenceLen = 28;

  sequenceTemplate(ledVals, time, sequence, sequenceLen, 0.1);
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











