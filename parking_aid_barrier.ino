#include <Servo.h>

Servo myServo;

const int trigPinLeft = 8;
const int echoPinLeft = 9;
const int buzzerPin = 10;

const int trigPinRight = 2;
const int echoPinRight = 3;

const int redPin = 12;
const int greenPin = 13;
const int bluePin = 11;

long durationLeft, cmLeft;
long durationRight, cmRight;
long min_cm = 0;

int safetyDistance = 45;
int longDistance = 30;
int mediumDistance = 15;
int lowDistance = 5;
long interval = 100000000;
long blink_interval = -1;

unsigned long previousMillis = 0;
unsigned long previousBlinkMillis = 0;

int blink_mode = 0;

int currentServoAngle = 0;

void setup() {
  myServo.attach(7);
  myServo.write(0);
  pinMode(trigPinLeft, OUTPUT);
  pinMode(echoPinLeft, INPUT);

  pinMode(trigPinRight, OUTPUT);
  pinMode(echoPinRight, INPUT);

  pinMode(buzzerPin, OUTPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  Serial.begin(9600);
}

void moveServoGradually(int targetAngle, int delayBetweenSteps) {
  
  if (targetAngle > currentServoAngle) {
    for (int angle = currentServoAngle; angle <= targetAngle; angle++) {
      myServo.write(angle);
      delay(delayBetweenSteps);
    }
    currentServoAngle = 90;
    myServo.write(90);
  } else {
    for (int angle = currentServoAngle; angle >= targetAngle; angle--) {
      myServo.write(angle);
      delay(delayBetweenSteps);
    }
    myServo.write(0);
    currentServoAngle = 0;
  }
  
}

void loop() {
  digitalWrite(trigPinLeft, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPinLeft, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinLeft, LOW);

  pinMode(echoPinLeft, INPUT);
  durationLeft = pulseIn(echoPinLeft, HIGH);
  cmLeft = (durationLeft / 2) / 29.1;

  digitalWrite(trigPinRight, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPinRight, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinRight, LOW);

  pinMode(echoPinRight, INPUT);
  durationRight = pulseIn(echoPinRight, HIGH);
  cmRight = (durationRight / 2) / 29.1;

  if (cmLeft <= cmRight) {
    min_cm = cmLeft;
  } else {
    min_cm = cmRight;
  }

  unsigned long currentMillis = millis();
  if (min_cm >= safetyDistance) {
    interval = 100000000;
    blink_interval = -1;
  } else if (min_cm >= longDistance && min_cm < safetyDistance) {
    interval = 750;
    blink_interval = 375;
  } else if (min_cm >= mediumDistance && min_cm < longDistance) {
    interval = 350;
    blink_interval = 175;
  } else if (min_cm >= lowDistance && min_cm < mediumDistance) {
    interval = 150;
    blink_interval = 75;
  } else if (min_cm < lowDistance) {
    interval = 0;
    blink_interval = 0;
  }

  if (interval == 0) {
    tone(buzzerPin, 1000);
  } else if (interval == 100000000) {
    noTone(buzzerPin);
  } else {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      tone(buzzerPin, 10);
      delay(50);
      noTone(buzzerPin);
    }
  }
  if (blink_interval != 0) {
    moveServoGradually(0, 10); 
  }
  if (blink_interval == 0) {
    digitalWrite(redPin, HIGH);
    moveServoGradually(90, 10); 
    delay(5000);
  } else if (blink_interval == -1) {
    digitalWrite(redPin, LOW);
    moveServoGradually(0, 10);
  } else if (currentMillis - previousBlinkMillis >= blink_interval) {
    previousBlinkMillis = currentMillis;
    if (blink_mode == 1) {
      digitalWrite(redPin, HIGH);
      blink_mode = 0;
    } else if (blink_mode == 0) {
      digitalWrite(redPin, LOW);
      blink_mode = 1;
    }
  }

  Serial.print("Left: ");
  Serial.print(cmLeft);
  Serial.print("cm || ");
  Serial.print("Right: ");
  Serial.print(cmRight);
  Serial.print("cm || ");
  Serial.print("MINIM: ");
  Serial.print(min_cm);
  Serial.print("cm || ");
  Serial.print("interval: ");
  Serial.print(interval);
  Serial.print(" || Blink Mode: ");
  Serial.print(blink_mode);
  Serial.println("");

  delay(250);
}
