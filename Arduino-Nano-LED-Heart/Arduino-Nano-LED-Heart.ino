/*
  TECHVIORA - 16 LED HEART ❤️
  Arduino Nano + 16 Red LEDs + Push Button

  Features:
  1. Automatic pattern change every 15 seconds
  2. Button press = immediate next pattern
  3. 10 animation patterns
  4. Non-blocking animations
*/

const byte ledPins[16] = {
  2, 3, 4, 5, 6, 7, 8, 9,
  10, 11, 12, 13, A0, A1, A2, A3
};

const byte BUTTON_PIN = A4;
const byte LED_COUNT = 16;
const byte TOTAL_PATTERNS = 10;

const unsigned long PATTERN_DURATION = 15000UL;
const unsigned long DEBOUNCE_MS = 35UL;

byte currentPattern = 0;
byte stepIndex = 0;

bool lastReading = HIGH;
bool stableButton = HIGH;
bool flashState = false;

unsigned long debounceTimer = 0;
unsigned long patternStartTime = 0;
unsigned long animationTimer = 0;

void allOff() {
  for (byte i = 0; i < LED_COUNT; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

void allOn() {
  for (byte i = 0; i < LED_COUNT; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
}

void showOne(byte index) {
  allOff();
  digitalWrite(ledPins[index % LED_COUNT], HIGH);
}

void changePattern() {
  currentPattern = (currentPattern + 1) % TOTAL_PATTERNS;

  stepIndex = 0;
  flashState = false;

  patternStartTime = millis();
  animationTimer = millis();

  allOff();
}

void checkButton() {
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastReading) {
    debounceTimer = millis();
  }

  if (millis() - debounceTimer >= DEBOUNCE_MS) {
    if (reading != stableButton) {
      stableButton = reading;

      // Change pattern once when button is pressed.
      if (stableButton == LOW) {
        changePattern();
      }
    }
  }

  lastReading = reading;
}

void runPattern() {
  unsigned long now = millis();

  switch (currentPattern) {

    // Pattern 1: Full heart blink
    case 0:
      if (now - animationTimer >= 500) {
        animationTimer = now;
        flashState = !flashState;

        if (flashState) allOn();
        else allOff();
      }
      break;

    // Pattern 2: Double heartbeat
    case 1: {
      unsigned long t = now % 1200UL;

      if (t < 120 || (t >= 220 && t < 340)) {
        allOn();
      } else {
        allOff();
      }
      break;
    }

    // Pattern 3: Running forward
    case 2:
      if (now - animationTimer >= 100) {
        animationTimer = now;
        showOne(stepIndex);
        stepIndex = (stepIndex + 1) % LED_COUNT;
      }
      break;

    // Pattern 4: Running backward
    case 3:
      if (now - animationTimer >= 100) {
        animationTimer = now;
        showOne(LED_COUNT - 1 - stepIndex);
        stepIndex = (stepIndex + 1) % LED_COUNT;
      }
      break;

    // Pattern 5: Alternate LEDs
    case 4:
      if (now - animationTimer >= 350) {
        animationTimer = now;
        flashState = !flashState;

        for (byte i = 0; i < LED_COUNT; i++) {
          bool on = ((i % 2 == 0) == flashState);
          digitalWrite(ledPins[i], on ? HIGH : LOW);
        }
      }
      break;

    // Pattern 6: Left half / right half
    case 5:
      if (now - animationTimer >= 350) {
        animationTimer = now;
        flashState = !flashState;

        for (byte i = 0; i < LED_COUNT; i++) {
          bool on = ((i < 8) == flashState);
          digitalWrite(ledPins[i], on ? HIGH : LOW);
        }
      }
      break;

    // Pattern 7: Expanding groups
    case 6:
      if (now - animationTimer >= 180) {
        animationTimer = now;
        allOff();

        byte level = stepIndex % 5;

        for (byte i = 0; i < LED_COUNT; i++) {
          if ((i % 5) <= level) {
            digitalWrite(ledPins[i], HIGH);
          }
        }

        stepIndex = (stepIndex + 1) % 10;
      }
      break;

    // Pattern 8: Random sparkle
    case 7:
      if (now - animationTimer >= 120) {
        animationTimer = now;
        allOff();

        digitalWrite(ledPins[random(LED_COUNT)], HIGH);

        if (random(2)) {
          digitalWrite(ledPins[random(LED_COUNT)], HIGH);
        }
      }
      break;

    // Pattern 9: SOS-style flash
    case 8:
      if (now - animationTimer >= 250) {
        animationTimer = now;
        flashState = !flashState;

        if (flashState) allOn();
        else allOff();
      }
      break;

    // Pattern 10: Fast celebration flash
    case 9:
      if (now - animationTimer >= 150) {
        animationTimer = now;
        flashState = !flashState;

        if (flashState) allOn();
        else allOff();
      }
      break;
  }
}

void setup() {
  for (byte i = 0; i < LED_COUNT; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  randomSeed(analogRead(A5));

  allOff();

  // Start Pattern 1 immediately.
  patternStartTime = millis();
  animationTimer = millis();
}

void loop() {
  checkButton();

  // Automatic change every 15 seconds.
  if (millis() - patternStartTime >= PATTERN_DURATION) {
    changePattern();
  }

  runPattern();
}