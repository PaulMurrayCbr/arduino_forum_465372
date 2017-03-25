

enum State {
  IDLE, PAUSE_BEFORE_ON, ON_DELAY, OPEN_DELAY, WAIT_BEAM, PAUSE_BEFORE_CLOSE, OFF_DELAY
} state = IDLE;

uint32_t start_time;

const uint32_t PAUSE_BEFORE_ON_ms = 10L * 1000L;
const uint32_t ON_DELAY_ms = 500L;
const uint32_t OPEN_DELAY_ms = 500L;
const uint32_t PAUSE_BEFORE_CLOSE_ms = 2L * 1000L;
const uint32_t OFF_DELAY_ms = 250L;


const byte onPin = 2;
const byte openPin = 3;
const byte beamPin = 4;

void activateButton(byte pin) {
  // to activate a button, we ground the pin
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void deactivateButton(byte pin) {
  // to deactivate a button, we float the pin
  pinMode(pin, INPUT);
  digitalWrite(pin, LOW);
}

void setup() {
  deactivateButton(onPin);
  deactivateButton(openPin);
  pinMode(beamPin, INPUT_PULLUP);

  state = IDLE;
  start_time = millis();
}

boolean beamIsBroken() {
  // the LED comes on when the beam is broken, I assume, so this
  // means that 'broken beam' grounds the output
  return digitalRead(beamPin) == LOW;
}


/**

  --wait for beam to break (IDLE)
  --beam breaks
  --wait 10 seconds  (PAUSE_BEFORE_ON)
  --send signal to ON
  --wait half a second  (ON_DELAY)
  --send signal to OPEN
  --wait half a second  (OPEN_DELAY)
  --release/turn off/float both signals
  --wait for beam to reconnect  (WAIT_BEAM)
  --wait another 2 seconds (PAUSE_BEFORE_CLOSE)
  --send ground signal to ON(which deactivates function)
  --wait 1/4 of a second (OFF_DELAY)
  --turn off ON
  --wait for beam to break again  (IDLE)

*/

void loop() {
  switch (state) {
    case IDLE:
      if (beamIsBroken()) {
        state = PAUSE_BEFORE_ON;
        start_time = millis();
      }
      break;

    case PAUSE_BEFORE_ON:
      if (millis() - start_time >= PAUSE_BEFORE_ON_ms) {
        activateButton(onPin);

        state = ON_DELAY;
        start_time = millis();
      }
      break;


    case ON_DELAY:
      if (millis() - start_time >= ON_DELAY_ms) {
        activateButton(openPin);

        state = OPEN_DELAY;
        start_time = millis();
      }
      break;

    case OPEN_DELAY:
      if (millis() - start_time >= OPEN_DELAY_ms) {
        deactivateButton(openPin);
        deactivateButton(onPin);

        state = WAIT_BEAM;
        start_time = millis();
      }
      break;

    case WAIT_BEAM:
      if (!beamIsBroken()) {
        state = PAUSE_BEFORE_CLOSE;
        start_time = millis();
      }
      break;


    case PAUSE_BEFORE_CLOSE:
      if (beamIsBroken()) {
        state = WAIT_BEAM;
        start_time = millis();
      }
      else  if (millis() - start_time >= PAUSE_BEFORE_CLOSE_ms) {
        activateButton(onPin);

        state = OFF_DELAY;
        start_time = millis();
      }
      break;


    case OFF_DELAY:
      if (millis() - start_time >= OFF_DELAY_ms) {
        deactivateButton(onPin);

        state = IDLE;
        start_time = millis();
      }
      break;
  }
}

