#define EN_PIN_1   2
#define STEP_PIN_1 4
#define DIR_PIN_1  6

#define EN_PIN_2   3
#define STEP_PIN_2 5
#define DIR_PIN_2  7

// EN_PIN_*, STEP_PIN_*, DIR_PIN_*
int Stepper1[] = {2, 4, 6};
int Stepper2[] = {3, 5, 7};

void setup() {
  Serial.begin(9600);
  // Prepare pins
  pinMode(Stepper1[0], OUTPUT);
  pinMode(Stepper1[1], OUTPUT);
  pinMode(Stepper2[0], OUTPUT);
  pinMode(Stepper2[1], OUTPUT);
}

void loop() {
  // move 1 motor
  // moveMotor(Stepper1, 180, 2, 1);
  // moveMotor(Stepper2, 180, 2, 1);


  moveMotorS(Stepper1,
            180,
            1,
            Stepper2,
            180,
            0,
            2 // time in seconds
          );

  delay(3000);
}

void moveMotorS(int Stepper1[], int angle1, int direction1, int Stepper2[], int angle2, int direction2, int time) {
  digitalWrite(Stepper1[0], LOW);         // Enable driver 1
  digitalWrite(Stepper2[0], LOW);         // Enable driver 2
  digitalWrite(Stepper1[2], direction1);  // Set direction 1
  digitalWrite(Stepper2[2], direction2);  // Set direction 2
  

  int deg_per_step = 9;
  int steps1 = angle1 * deg_per_step;
  int steps2 = angle2 * deg_per_step;
  float delay_time1 = (time * 1000/ steps1);
  float delay_time2 = (time * 1000/ steps2);

  for (int i = 1; i <= steps; i++) {
    digitalWrite(Stepper[1], HIGH);  // Step
    delay(delay_time);
    digitalWrite(Stepper[1], LOW);
    delay(delay_time);
  }

  digitalWrite(Stepper[0], HIGH);  // Disable driver
}


// 200 steps * 16 micro steps = 3200 steps
void moveMotor(int Stepper[], int angle, int time, int direction) {
  digitalWrite(Stepper[0], LOW);       // Enable driver
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
