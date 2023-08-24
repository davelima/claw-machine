#include <AccelStepper.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

AccelStepper Xaxis(1, 2, 5); // pin 2 = step, pin 5 = direction
AccelStepper Yaxis(1, 3, 6);
AccelStepper Zaxis(1, 4, 7);

const byte enablePin = 8;
constexpr uint32_t stepsPerMm = 4000;

const int XaxisRightEndstop = 9;
const int XaxisLeftEndstop = 43;
const int YaxisEndstop = 10;
const int YaxisNearEndstop = 11;

const int upButton = 24;
const int rightButton = 25;
const int downButton = 26;
const int leftButton = 27;

const int cranePin = 32;
int cranePosition = 0;
Servo crane;

const int actionButton = 50;

const int audioVolume = 30;

const int playerRx = A11;
const int playerTx = A10;

SoftwareSerial playerMP3Serial(playerRx, playerTx);
DFRobotDFPlayerMini playerMP3;

void setup()
{
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);

  Xaxis.setMaxSpeed(3000);
  Xaxis.setAcceleration(3000);
  Xaxis.setSpeed(3000);

  Yaxis.setMaxSpeed(3000);
  Yaxis.setAcceleration(3000);
  Yaxis.setSpeed(3000);
  Yaxis.setPinsInverted(true);

  Zaxis.setMaxSpeed(500);
  Zaxis.setAcceleration(500);
  Zaxis.setSpeed(500);

  crane.attach(cranePin);
  craneClose();
  delay(500);
  craneOpen();

  pinMode(upButton, INPUT);
  pinMode(rightButton, INPUT);
  pinMode(downButton, INPUT);
  pinMode(leftButton, INPUT);
  pinMode(YaxisEndstop, INPUT);
  pinMode(XaxisLeftEndstop, INPUT);
  pinMode(XaxisRightEndstop, INPUT);
  pinMode(YaxisNearEndstop, INPUT);

  digitalWrite(upButton, HIGH);
  digitalWrite(rightButton, HIGH);
  digitalWrite(downButton, HIGH);
  digitalWrite(leftButton, HIGH);
  digitalWrite(actionButton, HIGH);
  digitalWrite(YaxisEndstop, HIGH);
  digitalWrite(XaxisRightEndstop, HIGH);
  digitalWrite(XaxisLeftEndstop, HIGH);
  digitalWrite(YaxisNearEndstop, HIGH);

  playerMP3Serial.begin(9600);

  if (!playerMP3.begin(playerMP3Serial)) {
    while(true){
      delay(0);
    }
  }

  playerMP3.volume(audioVolume);
  playBgm();
}

void loop() {
  if (digitalRead(rightButton) == LOW && digitalRead(XaxisRightEndstop) == HIGH) {
    Xaxis.move(50);
  }

  if (digitalRead(leftButton) == LOW && digitalRead(XaxisLeftEndstop) == HIGH) {
    Xaxis.move(-50);
  }

  if (digitalRead(upButton) == LOW && digitalRead(YaxisEndstop) == HIGH) {
      Yaxis.move(50);
  }

  if (digitalRead(downButton) == LOW && digitalRead(YaxisNearEndstop) == HIGH) {
      Yaxis.move(-50);
  }

  if (digitalRead(actionButton) == LOW) {
    craneAction();
  }

  Xaxis.run();
  Yaxis.run();
  Zaxis.run();
}

void craneClose()
{
  crane.write(36);
}

void craneOpen()
{
  crane.write(100);
}

void craneAction()
{
  Xaxis.stop();
  Yaxis.stop();

  playerMP3.advertise(1);
  Zaxis.runToNewPosition(1280);
  delay(500); // close crane here
  craneClose();
  delay(2000);
  Zaxis.runToNewPosition(0);
  Xaxis.run();
  Yaxis.run();
  delay(1000);

  Xaxis.runToNewPosition(0);
  Yaxis.runToNewPosition(0);
  // bring X and Y to initial position
  while (digitalRead(YaxisNearEndstop) == HIGH) {
    Yaxis.runToNewPosition(Yaxis.currentPosition() - 50);
  }

  while (digitalRead(XaxisLeftEndstop) == HIGH) {
    Xaxis.runToNewPosition(Xaxis.currentPosition() - 50);
  }

  Yaxis.setCurrentPosition(0);
  Xaxis.setCurrentPosition(0);

  delay(1000);
  craneOpen();
}

void playBgm() {
  playerMP3.loopFolder(1);
}
