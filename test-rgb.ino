
#define RGB_RED D0
#define RGB_GREEN D1
#define RGB_BLUE D2

void setup() {
  // put your setup code here, to run once:
  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);
}

void loop() {
  int red = 0;
  int green = 255;
  int blue = 0;
  // put your main code here, to run repeatedly:
   analogWrite(RGB_RED, red);
   analogWrite(RGB_GREEN, green);
   analogWrite(RGB_BLUE, blue);
}