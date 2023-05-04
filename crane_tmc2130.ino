#include <Servo.h>
#include <TMCStepper.h>
#include <AccelStepper.h>


const int upButton = 2;
const int rightButton = 3;
const int downButton = 4;
const int leftButton = 5;
const int actionButton = 6;

const int cranePin = 49;
int cranePosition = 0;
Servo crane;

const int enPinX = 26;
const int dirPinX = 27;
const int stepPinX = 28;
const int csPinX = 29;

const int enPinY = 30;
const int dirPinY = 31;
const int stepPinY = 32;
const int csPinY = 33;

const int enPinZ = 34;
const int dirPinZ = 35;
const int stepPinZ = 36;
const int csPinZ = 37;

#define SW_MOSI 46
#define SW_MISO 47
#define SW_SCK 48
#define R_SENSE 0.11f
constexpr uint32_t stepsPerMm = 4000;

TMC2130Stepper driverX(csPinX, R_SENSE, SW_MOSI, SW_MISO, SW_SCK);
TMC2130Stepper driverY(csPinY, R_SENSE, SW_MOSI, SW_MISO, SW_SCK);
TMC2130Stepper driverZ(csPinZ, R_SENSE, SW_MOSI, SW_MISO, SW_SCK);

AccelStepper motorX = AccelStepper(motorX.DRIVER, stepPinX, dirPinX);
AccelStepper motorY = AccelStepper(motorY.DRIVER, stepPinY, dirPinY);
AccelStepper motorZ = AccelStepper(motorZ.DRIVER, stepPinZ, dirPinZ);


boolean isRunning = false;

void setup() {
  pinMode(upButton, INPUT);
  pinMode(rightButton, INPUT);
  pinMode(downButton, INPUT);
  pinMode(leftButton, INPUT);
  pinMode(actionButton, INPUT);
  digitalWrite(upButton, HIGH);
  digitalWrite(rightButton, HIGH);
  digitalWrite(downButton, HIGH);
  digitalWrite(leftButton, HIGH);
  digitalWrite(actionButton, HIGH);

  pinMode(enPinX, OUTPUT);
  pinMode(stepPinX, OUTPUT);
  pinMode(dirPinX, OUTPUT);
  digitalWrite(enPinX, LOW);

  setupMotors();
  crane.attach(cranePin);
  craneClose();
  delay(500);
  craneOpen();
}

void loop() {
  if (digitalRead(rightButton) == LOW) {
    motorX.move(100);
  }

  if (digitalRead(leftButton) == LOW) {
    motorX.move(-100);
  }

  if (digitalRead(upButton) == LOW) {
    motorY.move(100);
  }

  if (digitalRead(downButton) == LOW) {
    motorY.move(-100);  
  }

  motorX.run();
  motorY.run();
  motorZ.run();

  if (digitalRead(actionButton) == LOW) {
    craneAction();
  }
}

void craneClose()
{
  crane.write(56);
}

void craneOpen()
{
  crane.write(0);
}

void craneAction()
{
  motorX.stop();
  motorY.stop();

  motorZ.runToNewPosition(10000);
  delay(1000); // close crane here
  craneClose();
  delay(2000);
  motorZ.runToNewPosition(0);
  motorX.run();
  motorY.run();
  delay(1000);

  // bring X and Y to initial position
  motorX.runToNewPosition(0);
  motorY.runToNewPosition(0);

  delay(1000);
  craneOpen();
}


void setupMotors()
{
  SPI.begin();
  driverX.begin();
  driverX.rms_current(600);
  driverX.microsteps(4);
  driverX.en_pwm_mode(true);
  driverX.pwm_autoscale(true);
  motorX.setMaxSpeed(50 * stepsPerMm);
  motorX.setAcceleration(1000 * stepsPerMm);
  motorX.setEnablePin(enPinX);
  motorX.setPinsInverted(false, false, true);
  motorX.enableOutputs();

  driverY.begin();
  driverY.rms_current(600);
  driverY.microsteps(4);
  driverY.en_pwm_mode(true);
  driverY.pwm_autoscale(true);
  motorY.setMaxSpeed(50 * stepsPerMm);
  motorY.setAcceleration(1000 * stepsPerMm);
  motorY.setEnablePin(enPinY);
  motorY.setPinsInverted(false, false, true);
  motorY.enableOutputs();

  driverZ.begin();
  driverZ.rms_current(600);
  driverZ.microsteps(4);
  driverZ.en_pwm_mode(true);
  driverZ.pwm_autoscale(true);
  motorZ.setMaxSpeed(50 * stepsPerMm);
  motorZ.setAcceleration(1000 * stepsPerMm);
  motorZ.setEnablePin(enPinZ);
  motorZ.setPinsInverted(false, false, true);
  motorZ.enableOutputs();
}
