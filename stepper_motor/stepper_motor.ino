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

int vals[4] = {0, 0, 0, 0}; // array of menu parameters
int8_t arrowPos = 0;  // arrow position
bool isMoving = false;

// stepper motors constructors
AccelStepper stepper1(AccelStepper::DRIVER, STEP_PIN_1, DIR_PIN_1);
AccelStepper stepper2(AccelStepper::DRIVER, STEP_PIN_2, DIR_PIN_2);
 
void setup() {  
  Serial.begin(115200);

  stepper1.setMaxSpeed(500);
  stepper1.setAcceleration(50);
  
  stepper2.setMaxSpeed(500);
  stepper2.setAcceleration(50);

  stepper1.setMinPulseWidth(200);
  stepper2.setMinPulseWidth(200);

  // disable drivers by default
  pinMode(EN_PIN_1, OUTPUT);
  pinMode(EN_PIN_2, OUTPUT);
  digitalWrite(EN_PIN_1, HIGH);
  digitalWrite(EN_PIN_2, HIGH);

  // set encoder
  enc1.setType(TYPE2);

  // set LCD
  lcd.init();
  lcd.backlight();
  
  // print menu
  printGUI();
}

void loop() {
  // if (!stepper1.isRunning() && !stepper2.isRunning()) {
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
    stepper1.moveTo(vals[2]);
    stepper2.moveTo(vals[2]);
    isMoving = true;
  }

  if (enc1.isTurn()) {  // при любом повороте
    Serial.println("enc move");
    int increment = 0;  // локальная переменная направления
    int incrementStep = 1; // локальная переменная шага инкремента
    
    // получаем направление    
    if (enc1.isRight()) increment = 1;
    if (enc1.isLeft()) increment = -1;
    
    arrowPos += increment;  // двигаем курсор   
    arrowPos = constrain(arrowPos, 0, 3); // ограничиваем

    increment = 0;  // обнуляем инкремент
    // параметр дистанции и времени инкрементируем на 5
    if (arrowPos == 2 || arrowPos == 3) {
      incrementStep = 100;
    } else {
      incrementStep = 1;
    }
    if (enc1.isRightH()) increment = 1 * incrementStep;
    if (enc1.isLeftH()) increment = -1 * incrementStep;
    // меняем параметры
    vals[arrowPos] += increment;
    
    printGUI();
  }
}

void moveCamera() {
  // lcd.clear();
  // lcd.setCursor(2, 0);
  // lcd.print("In progress...");
  
  // Change direction at the limits
  // if (stepper1.distanceToGo() == 0)
  //     stepper1.moveTo(-stepper1.currentPosition());
  // if (stepper2.distanceToGo() == 0)
  //     stepper2.moveTo(-stepper2.currentPosition());
  stepper1.run();
  stepper2.run();

  // lcd.clear();
  // lcd.setCursor(2, 0);
  // lcd.print("Finished");
  // lcd.clear();
  // printGUI();
}

void printGUI() {
  lcd.setCursor(0, 0); lcd.print("Start:"); lcd.print(vals[0]);
  lcd.setCursor(8, 0); lcd.print("Stop:"); lcd.print(vals[1]);
  lcd.setCursor(0, 1); lcd.print("Dist:"); lcd.print(vals[2]);
  lcd.setCursor(8, 1); lcd.print("Time:"); lcd.print(vals[3]);
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