#define EN_PIN_1   2
#define STEP_PIN_1 4
#define DIR_PIN_1  6

#define EN_PIN_2   3
#define STEP_PIN_2 5
#define DIR_PIN_2  7

#define DEG_PER_STEP 9

// EN_PIN_*, STEP_PIN_*, DIR_PIN_*
int Stepper1[] = {2, 4, 6};
int Stepper2[] = {3, 5, 7};

unsigned long startMillis;
unsigned long startMillisStep;

void setup() {
  Serial.begin(9600);
  // Prepare pins
  pinMode(Stepper1[0], OUTPUT);
  pinMode(Stepper1[1], OUTPUT);
  pinMode(Stepper2[0], OUTPUT);
  pinMode(Stepper2[1], OUTPUT);
  // disable drivers by default
  digitalWrite(Stepper1[0], HIGH);
  digitalWrite(Stepper2[0], HIGH);

  startMillis = millis();
  startMillisStep = millis();
}

void loop() {
  // move 1 motor
  // moveMotor(Stepper1, 180, 2, 1);
  moveMotor(Stepper2, 720, 2, 1);
  
  // delay(3000);
}


// 200 steps * 16 micro steps = 3200 steps
void moveMotor(int Stepper[], int angle, int time, int direction) {
  float period = (time * 1000 * 2 / (angle * DEG_PER_STEP));
  float finish_time = time * 1000;

  if (millis() - startMillis <= finish_time) {
    Serial.println("1");
    digitalWrite(Stepper[0], LOW);       // Enable driver
    digitalWrite(Stepper[2], direction); // Set direction: 0 - left, 1 - right
    if (millis() - startMillisStep >= period) {
      Serial.println("2");
      digitalWrite(Stepper[1], !digitalRead(Stepper[1]));  // Step
      startMillisStep = millis();
    }
  } else {
    Serial.println("3");
    digitalWrite(Stepper[0], HIGH);  // Disable driver
    delay(3000);
    startMillis = millis();
    startMillisStep = millis();
  }
}
