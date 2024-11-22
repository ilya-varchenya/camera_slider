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

// encoder pins
#define CLK 10
#define DT 11
#define SW 12

// encoder constructor
Encoder enc1(CLK, DT, SW);

// LCD constructor
LiquidCrystal_I2C lcd(0x27, 20, 2);

// AccelStepper lib setup
float stepperMaxSpeed = 5000; 
float stepperAcceleration = 1500;
unsigned int stepperMinPulseWidth = 20;
// stepper 2 steps ammount
int stepsAmmount = 0;
// check if any motor is moving; lib method doesn't work for runSpeedToPosition()
bool isMoving = false;

int stepperTime = 5; // TODO: get this value from user
long pathB = 17000; // TODO: change after measuring full path
long pathA = 0;

// array of menu parameters with default values
long vals[4] = {pathB, stepperTime, stepperAcceleration, 0};
int8_t arrowPos = 0;  // arrow position



// stepper motors constructors
AccelStepper stepper1(AccelStepper::DRIVER, STEP_PIN_1, DIR_PIN_1);
AccelStepper stepper2(AccelStepper::DRIVER, STEP_PIN_2, DIR_PIN_2);
 
void setup() {  
  Serial.begin(115200);

  // stepper1.setMaxSpeed(stepperMaxSpeed);
  // stepper1.setAcceleration(stepperAcceleration);
  
  // stepper2.setMaxSpeed(stepperMaxSpeed);
  // stepper2.setAcceleration(stepperAcceleration);

  stepper1.setMinPulseWidth(stepperMinPulseWidth);
  stepper2.setMinPulseWidth(stepperMinPulseWidth);

  // disable drivers by default
  pinMode(EN_PIN_1, OUTPUT);
  pinMode(EN_PIN_2, OUTPUT);
  digitalWrite(EN_PIN_1, HIGH);
  digitalWrite(EN_PIN_2, HIGH);

  // TODO: create a method to move stepper 1 to initial position
  // move stepper 1 to initial position
  // moveToInitial();

  // set encoder
  enc1.setType(TYPE2);

  // set LCD
  lcd.init();
  lcd.backlight();
  
  // print menu
  printGUI();
}

void loop() {
  // TODO: finish functionality with different angles and distance on a rail
  if (isMoving) {
    // enable drivers
    digitalWrite(EN_PIN_1, LOW);
    digitalWrite(EN_PIN_2, LOW);
    moveCamera();
    if (stepper1.currentPosition() == vals[0] && stepper2.currentPosition() == stepsAmmount) {
      isMoving = false;
    } else {
      isMoving = true;
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
    stepper2SetParams();

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
  float speed = pathB / stepperTime;

  stepper1.moveTo(vals[0]);
  stepper1.setMaxSpeed(stepperMaxSpeed);
  stepper1.setSpeed(speed);
}

void stepper2SetParams() {
  /*                S 
          |        /|
          |       / |
          |      /  |
          |     / ^ |
          |    /  | |
          |   /betta|
   betta--|> /      |
          | / alfa  |
          A---------B
  */
  float AB = pathB;
  float BS = pathB/2;
  float alfa =  atan(BS / AB) * 57.2958;
  float betta = 90 - alfa;
  stepsAmmount = int(round(3200 / 360 * betta));
  float speed2 = stepsAmmount / stepperTime;
  
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
  // TODO: get params from user (AB, BC, BS, time)
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

// void moveToInitial() {
//   stepper1.setMaxSpeed(stepperMaxSpeed);
//   stepper1.setAcceleration(stepperAcceleration);
//   setPinsInverted(true, false, false); // invert direction
//   stepper1.moveTo(vals[0]);
//   stepper1.run()
//   setCurrentPosition(0);
//   setPinsInverted(false, false, false); // set clockwise direction
// }