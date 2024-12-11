#include <AccelStepper.h>
#include <LiquidCrystal_I2C.h>
#include "GyverEncoder.h"

// stepper pins
#define EN_PIN_1   2
#define STEP_PIN_1 4
#define DIR_PIN_1  6

#define EN_PIN_2   3
#define STEP_PIN_2 5
#define DIR_PIN_2  7

#define INIT_BTN 9

// encoder pins
#define CLK 10
#define DT 11
#define SW 12

// encoder constructor
Encoder enc1(CLK, DT, SW);

// LCD constructor
LiquidCrystal_I2C lcd(0x27, 20, 2);

// AccelStepper lib setup
float stepperMaxSpeed = 10000; 
float stepperAcceleration = 1500;
unsigned int stepperMinPulseWidth = 20;
// stepper 2 steps ammount
int stepsAmmount = 0;
// check if any motor is moving; lib method doesn't work for runSpeedToPosition()
bool isMoving = false;

int stepperTime = 15; // TODO: get this value from user
long pathC = 20000;
long pathB = 10000; // TODO: change after measuring full path
long pathA = 0;
long pathBS = 10000; // TODO: change after measuring full path

// array of menu parameters with default values
long vals[4] = {pathC, stepperTime, stepperAcceleration, 0};
int8_t arrowPos = 0;  // arrow position

// stepper motors constructors
AccelStepper stepper1(AccelStepper::DRIVER, STEP_PIN_1, DIR_PIN_1);
AccelStepper stepper2(AccelStepper::DRIVER, STEP_PIN_2, DIR_PIN_2);

// can be removed after fixin speed counting formula
int countStepper2Call = 0;

bool isInitialised = false;

void setup() {  
  Serial.begin(115200);

  stepper1.setMinPulseWidth(stepperMinPulseWidth);
  stepper2.setMinPulseWidth(stepperMinPulseWidth);

  // disable drivers by default
  pinMode(EN_PIN_1, OUTPUT);
  pinMode(EN_PIN_2, OUTPUT);
  digitalWrite(EN_PIN_1, HIGH);
  digitalWrite(EN_PIN_2, HIGH);

  // initial button
  pinMode(INIT_BTN, INPUT);

  // set encoder
  enc1.setType(TYPE2);

  // set LCD
  lcd.init();
  lcd.backlight();

  // set initial params for stepper 1
  // stepper1.moveTo(pathC * (-1));
  stepper1.moveTo(-40000);
  stepper1.setMaxSpeed(stepperMaxSpeed);
  stepper1.setSpeed(stepperMaxSpeed * (-1));
  
  // print menu
  printGUI();
}

void loop() {
  // move stepper1 to start position after Arduino boot
  if (!isInitialised) {
    if (digitalRead(INIT_BTN) == LOW) {
      // enable driver
      digitalWrite(EN_PIN_1, LOW);
      stepper1.runSpeedToPosition();
    } else {
      // disable driver
      digitalWrite(EN_PIN_1, HIGH);
      isInitialised = true;
      stepper1.setCurrentPosition(0);
    }
  }

  if (isMoving) {
    // enable drivers
    digitalWrite(EN_PIN_1, LOW);
    digitalWrite(EN_PIN_2, LOW);
    moveCamera();
    if (stepper1.currentPosition() == pathC) {
        isMoving = false;
    }
    if (stepper2.currentPosition() == stepsAmmount && countStepper2Call < 2) {    
      // calling set params method for second part
      stepper2.setCurrentPosition(0);
      stepper2SetParams(pathC - pathB);
    }
  } else {
    // disable drivers
    digitalWrite(EN_PIN_1, HIGH);
    digitalWrite(EN_PIN_2, HIGH);
    getParamsFromMenu();
  }
}

void getParamsFromMenu() {
  enc1.tick();
  if (enc1.isDouble()) {
    stepper1SetParams();
    stepper2SetParams(pathB);

    isMoving = true;
  }

  if (enc1.isTurn()) {
    int increment = 0;  // local variable of direction
    int incrementStep = 1;
    
    // getting direction
    if (enc1.isRight()) increment = 1;
    if (enc1.isLeft()) increment = -1;
    
    arrowPos += increment;  // moving cursor
    arrowPos = constrain(arrowPos, 0, 3); // measure by cursor

    increment = 0;
    // different increment 
    if (arrowPos == 0 || arrowPos == 1 || arrowPos == 2) {
      incrementStep = 100;
    } else {
      incrementStep = 10;
    }
    if (enc1.isRightH()) increment = 1 * incrementStep;
    if (enc1.isLeftH()) increment = -1 * incrementStep;
    // set new values
    vals[arrowPos] += increment;

    printGUI();
  }
}

void stepper1SetParams() {
  float speed = float(pathC) / stepperTime;

  Serial.println("Stepper 1:");
  Serial.print("Time: ");
  Serial.println(stepperTime);
  Serial.print("Path: ");
  Serial.println(pathC);
  Serial.print("Speed: ");
  Serial.println(speed);
  Serial.println("\\\\\\\\\\\\\\");

  stepper1.moveTo(pathC);
  stepper1.setMaxSpeed(stepperMaxSpeed);
  stepper1.setSpeed(speed);
}

void stepper2SetParams(long path) {
  countStepper2Call++;
  /*                S 
          |        /|\
          |       / | \
          |      /  |  \
          |     / ^ |   \
          |    /  | |    \
          |   /betta|     \
   betta--|> /      |      \
          | / alfa  |       \
          A---------B--------C
  */

  // TODO: fix the formula
  float alfa =  atan(float(pathBS) / path) * 57.2958;
  float betta = 90 - alfa;
  stepsAmmount = int(round((3200.0 / 360.0) * betta));
  float speed2 = stepsAmmount / (stepperTime * (float(path) / pathC));
  Serial.println("Stepper 2:");
  Serial.print("Time: ");
  Serial.println(stepperTime * (float(path) / pathC));
  Serial.print("Path: ");
  Serial.println(stepsAmmount);
  Serial.print("Speed: ");
  Serial.println(speed2);
  Serial.println("\\\\\\\\\\\\\\");
  
  // invert direction
  stepper2.setPinsInverted(true, false, false);
  stepper2.moveTo(stepsAmmount);
  stepper2.setMaxSpeed(stepperMaxSpeed);
  stepper2.setSpeed(speed2);
}

void moveCamera() {
  // lcd.clear();
  // lcd.setCursor(2, 0);
  // lcd.print("In progress...");

  // stepper1.run();
  // stepper2.run();
  stepper1.runSpeedToPosition();
  stepper2.runSpeedToPosition();


  // lcd.clear();
  // lcd.setCursor(2, 0);
  // lcd.print("Finished");
  // lcd.clear();
  // printGUI();
}

void printGUI() {
  // TODO: get params from user (pathB, BC, pathBS, time)
  lcd.setCursor(0, 0); lcd.print("Dist:"); lcd.print(vals[0]);
  lcd.setCursor(8, 0); lcd.print("maxS:"); lcd.print(vals[1]);
  lcd.setCursor(0, 1); lcd.print("accl:"); lcd.print(vals[2]);
  lcd.setCursor(8, 1); lcd.print("bla:"); lcd.print(vals[3]);
  // выводим стрелку
  switch (arrowPos) {
    case 0: lcd.setCursor(4, 0);
      break;
    case 1: lcd.setCursor(12, 0);
      break;
    case 2: lcd.setCursor(4, 1);
      break;
    case 3: lcd.setCursor(12, 1);
      break;
  }
  lcd.write(126);   // вывести стрелку
}

