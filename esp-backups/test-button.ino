#define BUTTON_PIN D5
int buttonState = 0;
int previousButtonState = 0;

void setup() {
  pinMode(BUTTON_PIN,INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  // Robojax.com push button tutorial
  buttonState = digitalRead(BUTTON_PIN);
  if(previousButtonState && !buttonState) {
    Serial.println("Button is pushed.");
  }

  previousButtonState = buttonState;
  delay(100);
}