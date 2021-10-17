#include <TMCStepper.h>

#define EN_PIN_X           2
#define DIR_PIN_X          3
#define STEP_PIN_X         4
#define CS_PIN_X           5

#define EN_PIN_Y           6
#define DIR_PIN_Y          7
#define STEP_PIN_Y         8
#define CS_PIN_Y           9

#define EN_PIN_Z           A2
#define DIR_PIN_Z          A3
#define STEP_PIN_Z         A4
#define CS_PIN_Z           A5

#define SW_MOSI          11 // Software Master Out Slave In (MOSI)
#define SW_MISO          12 // Software Master In Slave Out (MISO)
#define SW_SCK           13 // Software Slave Clock (SCK)
#define DRIVER_ADDRESS 0b00 // TMC2209 Driver address according to MS1 and MS2

#define R_SENSE 0.11f

constexpr uint32_t steps_per_mm = 4000;

TMC2130Stepper driverX(CS_PIN_X, R_SENSE);                           // Hardware SPI
TMC2130Stepper driverY(CS_PIN_Y, R_SENSE);                           // Hardware SPI
TMC2130Stepper driverZ(CS_PIN_Z, R_SENSE);                           // Hardware SPI

#include <AccelStepper.h>
AccelStepper motorX = AccelStepper(motorX.DRIVER, STEP_PIN_X, DIR_PIN_X);
AccelStepper motorY = AccelStepper(motorY.DRIVER, STEP_PIN_Y, DIR_PIN_Y);
AccelStepper motorZ = AccelStepper(motorZ.DRIVER, STEP_PIN_Z, DIR_PIN_Z);

const int joystickX = A0;
const int joystickY = A1;
int joystickXState = 0;
int joystickYState = 0;

const int buttonPin = 10;
int buttonState = 0;

#include <Servo.h>
const int cranePin = 1;
Servo crane;

int cranePosition = 0;

void setup() {
  pinMode(EN_PIN_X, OUTPUT);
  pinMode(STEP_PIN_X, OUTPUT);
  pinMode(DIR_PIN_X, OUTPUT);
  digitalWrite(EN_PIN_X, LOW);

  pinMode(EN_PIN_Y, OUTPUT);
  pinMode(STEP_PIN_Y, OUTPUT);
  pinMode(DIR_PIN_Y, OUTPUT);
  digitalWrite(EN_PIN_Y, LOW);

  pinMode(EN_PIN_Z, OUTPUT);
  pinMode(STEP_PIN_Z, OUTPUT);
  pinMode(DIR_PIN_Z, OUTPUT);
  digitalWrite(EN_PIN_Z, LOW);

  SPI.begin();
  driverX.begin();
  //driverX.toff(5);
  driverX.rms_current(600);
  driverX.microsteps(4);
  driverX.en_pwm_mode(true);
  driverX.pwm_autoscale(true);

  driverY.begin();
  //driverY.toff(5);
  driverY.rms_current(600);
  driverY.microsteps(4);
  driverY.en_pwm_mode(true);
  driverY.pwm_autoscale(true);

  driverZ.begin();
  //driverZ.toff(5);
  driverZ.rms_current(600);
  driverZ.microsteps(4);
  driverZ.en_pwm_mode(true);
  driverZ.pwm_autoscale(true);

  motorX.setMaxSpeed(50 * steps_per_mm); // 100mm/s @ 80 steps/mm
  motorX.setAcceleration(1000 * steps_per_mm); // 2000mm/s^2
  motorX.setEnablePin(EN_PIN_X);
  motorX.setPinsInverted(false, false, true);
  motorX.enableOutputs();

  motorY.setMaxSpeed(50 * steps_per_mm); // 100mm/s @ 80 steps/mm
  motorY.setAcceleration(1000 * steps_per_mm); // 2000mm/s^2
  motorY.setEnablePin(EN_PIN_Y);
  motorY.setPinsInverted(false, false, true);
  motorY.enableOutputs();

  motorZ.setMaxSpeed(50 * steps_per_mm); // 100mm/s @ 80 steps/mm
  motorZ.setAcceleration(1000 * steps_per_mm); // 2000mm/s^2
  motorZ.setEnablePin(EN_PIN_Z);
  motorZ.setPinsInverted(false, false, true);
  motorZ.enableOutputs();

  crane.attach(cranePin);

  craneOpen();
}

void loop() {
  joystickXState = analogRead(joystickX);
  joystickYState = analogRead(joystickY);

  int currentPositionX = motorX.currentPosition();
  int currentPositionY = motorY.currentPosition();
  if (joystickXState > 1000 && currentPositionX < 5000) {
    motorX.move(100);
  }
  
  if (joystickXState < 50 && currentPositionX > 0) {
    motorX.move(-100);
  }

  if (joystickYState > 1000 && currentPositionY < 5000) {
    motorY.move(100);
  }

  if (joystickYState < 50 && currentPositionY > 0) {
    motorY.move(-100);
  }

  motorX.run();
  motorY.run();
  motorZ.run();

  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
     craneAction();
  }
}



void craneClose()
{
  int current = crane.read();
  for(cranePosition = current; cranePosition < 60; cranePosition++) {
    crane.write(cranePosition);
    delay(5);
  }
}

void craneOpen()
{
  int current = crane.read();
  for(cranePosition = current; cranePosition > 0; cranePosition--) {
    crane.write(cranePosition);
    delay(5);
  }
}


void craneAction()
{
  motorX.stop();
  motorY.stop();
  motorZ.run();
  for (int position = 0; position <= 3000; position++) {
    motorZ.move(100);
  }
  delay(4); // close crane here
  craneClose();
  for (int position = 3000; position > 0; position--) {
    motorZ.move(-100);
  }
  delay(4);
  motorX.run();
  motorY.run();
  motorZ.stop();

  // bring X and Y to initial position
  motorX.move(0);
  motorY.move(0);
  
  craneOpen();
}
