

enum State {
IDLE, PAUSE_BEFORE_ON, ON_DELAY, OPEN_DELAY, WAIT_BEAM, PAUSE_BEFORE_CLOSE, OFF_DELAY
} state = IDLE;

uint32_t start_time; // the time at which we entered the current state
// we don't bother with start_time in IDLE state, but ON and OPEN are timed

const uint32_t ON_TIME = 500; // wait half a second for power up
const uint32_t OPEN_TIME = 2000; // wait for 2 seconds after the beam is closed again

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
  // looking at the diagram in reply #8, it seems that
  // the pin gets pulled low when the beam is broken

  // if you are adding the recommended 5.1k resistor externally, then you don't
  // need INPUT_PULLUP - because that 5.1k resistor is a pullup resistor.
  // but, if you are just connecting the output pin directly, then you do need it
 
  pinMode(beamPin, INPUT_PULLUP); // assuming the 5.1k resistor is not in the circuit
  // pinMode(beamPin, INPUT); // if that5.1k resistor is present
 
  state = IDLE;
}

boolean beamIsBroken() {
  // the LED comes on when the beam is broken, I assume, so this
  // means that 'broken beam' grounds the output
  return digitalRead(beamPin) == LOW;
}

void loop() {
  switch (state) {
    case IDLE:
      if (beamIsBroken()) {
        start_time = millis();
        activateButton(onPin);
        state = ON;
      }
      break;

    case PAUSE_BEFORE_ON:
      break;


    case ON_DELAY:
      if (millis() - start_time >= ON_TIME) {
        start_time = millis();
        // don't need to activate button A - it's already activated.
        activateButton(openPin);
        state = OPEN;
      }
      break;

    case OPEN_DELAY:
      if(beamIsBroken()) {
        // if the beam is broken, we reset the 2-second timeout to "now". This means that
        // the dor should remain open until two seconds after the cat has come through the door.
        start_time = millis();
      }
      else
      if (millis() - start_time >= OPEN_TIME) {
        // the beam has been unbroken for two seconds. Close the door.
        deactivateButton(openPin);
        deactivateButton(onPin);
        state = IDLE;
      }
      else {
        ; // do nothing and leave the door open
      }
      break;

      case WAIT_BEAM:
      break;
   
   
    case PAUSE_BEFORE_CLOSE:
      break;
   
   
    case OFF_DELAY:
      break;
  }
}

