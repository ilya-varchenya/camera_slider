#define EN_PIN_1   2
#define STEP_PIN_1 4
#define DIR_PIN_1  6

#define EN_PIN_2   3
#define STEP_PIN_2 5
#define DIR_PIN_2  7

// EN_PIN_*, STEP_PIN_*, DIR_PIN_*
int Stepper1[] = {2, 4, 6};
int Stepper2[] = {3, 5, 7};

int stepState = LOW;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  // Prepare pins
  pinMode(Stepper1[0], OUTPUT);
  pinMode(Stepper1[1], OUTPUT);
  pinMode(Stepper2[0], OUTPUT);
  pinMode(Stepper2[1], OUTPUT);

  // Enable drivers
  digitalWrite(Stepper1[0], LOW);
  digitalWrite(Stepper2[0], LOW);
}

void loop() {
  moveMotor(Stepper1, 180, 2, 1);
  moveMotor(Stepper2, 180, 2, 1);
  delay(3000);
}

// 200 steps * 16 micto steps = 3200 steps
void moveMotor(int Stepper[], int angle, int time, int direction) {
  Serial.println(Stepper[1]);
  digitalWrite(Stepper[0], LOW);        // Enable driver
  digitalWrite(Stepper[2], direction); // Set direction: 0 - left, 1 - right

  int deg_per_step = 9;
  int steps = angle * deg_per_step;
  float delay_time = (time * 1000/ steps);

  for (int i = 1; i <= steps; i++) {
    digitalWrite(Stepper[1], HIGH);  // Step
    delay(delay_time);
    digitalWrite(Stepper[1], LOW);
    delay(delay_time);
  }

  digitalWrite(Stepper[0], HIGH);  // Disable driver
}
