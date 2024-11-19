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

long path1B = 17000; // TODO: change after measuring full path
long path1A = 0;
long path2B = 1700; // TODO: change after measuring full path
long path2A = 0;

// array of menu parameters with default values
long vals[4] = {path1B, stepperMaxSpeed, stepperAcceleration, 0};
int8_t arrowPos = 0;  // arrow position
bool isMoving = false;


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
  // TODO: finish functionality with different angles and distance
  // on a rail
  if (isMoving) {
    // enable drivers
    digitalWrite(EN_PIN_1, LOW);
    digitalWrite(EN_PIN_2, LOW);
    moveCamera();
    isMoving = stepper1.isRunning() || stepper2.isRunning();
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
    stepper1.moveTo(vals[0]);
    stepper2SetParams();
    
    stepper1.setMaxSpeed(float(vals[1]));
    stepper1.setAcceleration(float(vals[2]));
    
    isMoving = true;
  }

  if (enc1.isTurn()) {  // при любом повороте
    // Serial.println("enc move");x
    int increment = 0;  // локальная переменная направления
    int incrementStep = 1; // локальная переменная шага инкремента
    
    // получаем направление    
    if (enc1.isRight()) increment = 1;
    if (enc1.isLeft()) increment = -1;
    
    arrowPos += increment;  // двигаем курсор   
    arrowPos = constrain(arrowPos, 0, 3); // ограничиваем

    increment = 0;  // обнуляем инкремент
    // параметр дистанции и времени инкрементируем на 5
    if (arrowPos == 0 || arrowPos == 1 || arrowPos == 2) {
      incrementStep = 100;
    } else {
      incrementStep = 10;
    }
    if (enc1.isRightH()) increment = 1 * incrementStep;
    if (enc1.isLeftH()) increment = -1 * incrementStep;
    // меняем параметры
    vals[arrowPos] += increment;

    printGUI();
  }
}

void stepper2SetParams() {
  float AB = path1B;
  float BS = path1B/2;
  float alfa =  atan(BS / AB) * 57.2958;
  float betta = 90 - alfa;
  int stepsAmmount = int(round(3200/360 * betta));
  
  stepper2.moveTo(stepsAmmount);
  // TODO: count stepper 2 velocity according to stepper 1 velocity
  stepper2.setMaxSpeed(float(vals[1]));
  stepper2.setAcceleration(float(vals[2]));
}

void moveCamera() {
  // lcd.clear();
  // lcd.setCursor(2, 0);
  // lcd.print("In progress...");

  stepper1.run();
  stepper2.run();


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