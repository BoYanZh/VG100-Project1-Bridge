#include <Servo.h>
#define GREEN_LIGHT_1_PIN 7
#define RED_LIGHT_1_PIN 6
#define GREEN_LIGHT_2_PIN 10
#define RED_LIGHT_2_PIN 9
#define INF_LEFT_PIN A3
#define INF_RIGHT_PIN A2
#define TRIG_MID_FRONT_PIN 8
#define ECHO_MID_FRONT_PIN A1
#define TRIG_MID_BEHIND_PIN 13
#define ECHO_MID_BEHIND_PIN A0
#define INF_MID_FRONT_PIN A3
#define INF_MID_BEHIND_PIN A2
#define LEFT_SERVO_PIN 12
#define RIGHT_SERVO_PIN 11
#define MOTOR_IN1_PIN 5
#define MOTOR_IN2_PIN 4
#define MOTOR_IN3_PIN 3
#define MOTOR_IN4_PIN 2

#define LEFT_SIDE -1
#define MID_FRONT_SIDE 0
#define RIGHT_SIDE 1
#define MID_BEHIND_SIDE 2

#define CAR_DISTANCE_MIN 0.5
#define CAR_DISTANCE_MAX 10

#define SHIP_DISTANCE_MIN 3
#define SHIP_DISTANCE_MAX 40

#define RED -1
#define GREEN 1

#define UP 0
#define DOWN 1

Servo LeftServo;
Servo RightServo;
volatile int car_from = 0;

float getDistance(int side) {
  int trigPin, echoPin;
  if (side == MID_FRONT_SIDE) {
    trigPin = TRIG_MID_FRONT_PIN;
    echoPin = ECHO_MID_FRONT_PIN;
  } else if (side == MID_BEHIND_SIDE) {
    trigPin = TRIG_MID_BEHIND_PIN;
    echoPin = ECHO_MID_BEHIND_PIN;
  }
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float cm = pulseIn(echoPin, HIGH) / 58.00;
  /*digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(2);
    digitalWrite(trigPin, LOW);
    float cm = int(pulseIn(echoPin, HIGH) / 58.0 * 100) / 100.0;*/
  return cm;
}

bool detectCar(int side) {
  // float myDistance = getDistance(side);
  // return myDistance <= CAR_DISTANCE_MAX && myDistance >= CAR_DISTANCE_MIN;
  int pin;
  if (side == LEFT_SIDE) {
    pin = INF_LEFT_PIN;
  } else if (side == RIGHT_SIDE) {
    pin = INF_RIGHT_PIN;
  }
  bool myRd = !digitalRead(pin);
  Serial.print(side);
  Serial.print(" ");
  Serial.println(myRd);
  return myRd;
}

bool detectShip(int side) {
  // float myDistance = getDistance(side);
  // return myDistance <= SHIP_DISTANCE_MAX && myDistance >= SHIP_DISTANCE_MIN;
  float myDistance = getDistance(side);
  Serial.println(myDistance);
  return myDistance <= SHIP_DISTANCE_MAX && myDistance >= SHIP_DISTANCE_MIN;
}

void changeLight(int lightColor) {
  if (lightColor == RED) {
    digitalWrite(RED_LIGHT_1_PIN, HIGH);
    digitalWrite(GREEN_LIGHT_1_PIN, LOW);
    digitalWrite(RED_LIGHT_2_PIN, HIGH);
    digitalWrite(GREEN_LIGHT_2_PIN, LOW);
  } else if (lightColor == GREEN) {
    digitalWrite(GREEN_LIGHT_1_PIN, HIGH);
    digitalWrite(RED_LIGHT_1_PIN, LOW);
    digitalWrite(GREEN_LIGHT_2_PIN, HIGH);
    digitalWrite(RED_LIGHT_2_PIN, LOW);
  } else if (lightColor == 0) {
    digitalWrite(GREEN_LIGHT_1_PIN, LOW);
    digitalWrite(RED_LIGHT_1_PIN, LOW);
    digitalWrite(GREEN_LIGHT_2_PIN, LOW);
    digitalWrite(RED_LIGHT_2_PIN, LOW);
  }
  Serial.println("light changes");
}

void moveBarrier(int direction) {
  int pos, needPos = 60;
  LeftServo.attach(LEFT_SERVO_PIN);
  RightServo.attach(RIGHT_SERVO_PIN);
  if (direction == UP) {
    LeftServo.write(40);
    RightServo.write(40);
    delay(200);
    LeftServo.write(60);
    RightServo.write(60);
    delay(200);
    LeftServo.write(80);
    RightServo.write(80);
    delay(200);
    LeftServo.write(95);
    RightServo.write(95);
  } else if (direction == DOWN) {
    LeftServo.write(80);
    RightServo.write(80);
    delay(200);
    LeftServo.write(60);
    RightServo.write(60);
    delay(200);
    LeftServo.write(40);
    RightServo.write(40);
    delay(200);
    LeftServo.write(20);
    RightServo.write(20);
    delay(200);
    RightServo.write(10);
  }
  delay(500);
  Serial.println("barrier moves");
}

void waitCarACome() {
  int leftCount = 0, rightCount = 0;
  while (true) {
    if (detectCar(LEFT_SIDE)) {
      ++leftCount;
      if (leftCount > 1) {
        car_from = LEFT_SIDE;
        break;
      }
    } else if (detectCar(RIGHT_SIDE)) {
      ++rightCount;
      if (rightCount > 1) {
        car_from = RIGHT_SIDE;
        break;
      }
    }
    else {
      leftCount = leftCount == 0 ? 0 : leftCount - 1;
      rightCount = rightCount == 0 ? 0 : rightCount - 1;
    }
    delay(100);
  }
  Serial.println("car A comes");
}

void waitShipCome() {
  int count = 0,comeCount = 0;
  while (true) {
    if (detectShip(MID_FRONT_SIDE)) {
      ++count;
      if (count > 0)
        break;
    } else
      count = count == 0 ? 0 : count - 1;
    delay(100);
  }
  Serial.println("ship comes");
}

void waitCarALeave() {
  int detectSide = car_from == LEFT_SIDE ? RIGHT_SIDE : LEFT_SIDE;
  int count = 0;
  while (true) {
    if (detectCar(detectSide)) {
      ++count;
      if (count > 1)
        break;
    } else
      count = count == 0 ? 0 : count - 1;
    delay(100);
  }
  Serial.println("car A leaves");
  delay(3000);
}

void moveBridge_old(int direction) {
  int i;
  if (direction == UP) {
    for (i = 0; i <= 255; i += 5) {
      analogWrite(MOTOR_IN1_PIN, i);
      analogWrite(MOTOR_IN2_PIN, 0);
      analogWrite(MOTOR_IN3_PIN, i);
      analogWrite(MOTOR_IN4_PIN, 0);
      delay(50);
    }
    delay(4000);
    for (i = 255; i >= 0; i -= 5) {
      analogWrite(MOTOR_IN1_PIN, i);
      analogWrite(MOTOR_IN2_PIN, 0);
      analogWrite(MOTOR_IN3_PIN, i);
      analogWrite(MOTOR_IN4_PIN, 0);
      delay(50);
    }
    /*digitalWrite(MOTOR_IN1_PIN, HIGH);
      digitalWrite(MOTOR_IN2_PIN, LOW);
      digitalWrite(MOTOR_IN3_PIN, HIGH);
      digitalWrite(MOTOR_IN4_PIN, LOW);
      delay(4000);
      digitalWrite(MOTOR_IN1_PIN, HIGH);
      digitalWrite(MOTOR_IN2_PIN, LOW);
      digitalWrite(MOTOR_IN3_PIN, HIGH);
      digitalWrite(MOTOR_IN4_PIN, LOW);*/
  } else if (direction == DOWN) {
    for (i = 0; i <= 255; i += 5) {
      analogWrite(MOTOR_IN1_PIN, 0);
      analogWrite(MOTOR_IN2_PIN, i);
      analogWrite(MOTOR_IN3_PIN, 0);
      analogWrite(MOTOR_IN4_PIN, i);
      delay(50);
    }
    delay(5000);
    for (i = 255; i >= 0; i -= 5) {
      analogWrite(MOTOR_IN1_PIN, 0);
      analogWrite(MOTOR_IN2_PIN, i);
      analogWrite(MOTOR_IN3_PIN, 0);
      analogWrite(MOTOR_IN4_PIN, i);
      delay(50);
    }
  }
  Serial.println("bridge moves");
}

void moveBridge(int direction) {
  if (direction == UP) {
    digitalWrite(MOTOR_IN1_PIN, HIGH);
    digitalWrite(MOTOR_IN2_PIN, LOW);
    delay(800);
    digitalWrite(MOTOR_IN3_PIN, HIGH);
    digitalWrite(MOTOR_IN4_PIN, LOW);
    delay(7600);
    digitalWrite(MOTOR_IN1_PIN, LOW);
    digitalWrite(MOTOR_IN2_PIN, LOW);
    delay(4000);
    digitalWrite(MOTOR_IN3_PIN, LOW);
    digitalWrite(MOTOR_IN4_PIN, LOW);
  } else if (direction == DOWN) {
    digitalWrite(MOTOR_IN4_PIN, HIGH);
    digitalWrite(MOTOR_IN3_PIN, LOW);
    delay(3000);
    digitalWrite(MOTOR_IN2_PIN, HIGH);
    digitalWrite(MOTOR_IN1_PIN, LOW);
    delay(6500);
    digitalWrite(MOTOR_IN4_PIN, LOW);
    digitalWrite(MOTOR_IN3_PIN, LOW);
    delay(1000);
    digitalWrite(MOTOR_IN2_PIN, LOW);
    digitalWrite(MOTOR_IN1_PIN, LOW);
  }
  Serial.println("bridge moves");
}

void waitShipLeave() {
  int count = 0, leaveCount = 0;
  while (true) {
    if (detectShip(MID_BEHIND_SIDE)) {
      ++count;
      if (count >= 2)
        break;
    } else
      count = count == 0 ? 0 : count + 1;
    ++leaveCount;
    delay(50);
    if (leaveCount > 200)
      break;
  }
  Serial.println("ship will leave");
  count = 0;
  while (true) {
    if (!detectShip(MID_BEHIND_SIDE)) {
      ++count;
      if (count >= 10)
        break;
    } else
      count = 0;
    delay(50);
  }
  delay(1000);
  Serial.println("ship leaves");
}

void setup() {
  Serial.begin(9600);
  pinMode(RED_LIGHT_1_PIN, OUTPUT);
  pinMode(GREEN_LIGHT_1_PIN, OUTPUT);
  pinMode(RED_LIGHT_2_PIN, OUTPUT);
  pinMode(GREEN_LIGHT_2_PIN, OUTPUT);
  pinMode(INF_LEFT_PIN, INPUT);
  pinMode(INF_RIGHT_PIN, INPUT);
  pinMode(TRIG_MID_FRONT_PIN, OUTPUT);
  pinMode(ECHO_MID_FRONT_PIN, INPUT);
  pinMode(TRIG_MID_BEHIND_PIN, OUTPUT);
  pinMode(ECHO_MID_BEHIND_PIN, INPUT);
  pinMode(MOTOR_IN1_PIN, OUTPUT);
  pinMode(MOTOR_IN2_PIN, OUTPUT);
  pinMode(MOTOR_IN3_PIN, OUTPUT);
  pinMode(MOTOR_IN4_PIN, OUTPUT);
  LeftServo.attach(LEFT_SERVO_PIN);
  LeftServo.write(95);
  RightServo.attach(RIGHT_SERVO_PIN);
  RightServo.write(95);
  delay(1000);
  LeftServo.attach(LEFT_SERVO_PIN);
  LeftServo.write(95);
  RightServo.attach(RIGHT_SERVO_PIN);
  RightServo.write(95);
  delay(1000);
  changeLight(GREEN);
  waitCarACome();
  changeLight(0);
  waitShipCome();
  moveBarrier(DOWN);
  delay(1000);
  changeLight(RED);
  //delay(1000);
  //changeLight(0);
  waitCarALeave();
  changeLight(RED);
  moveBridge(UP);
  changeLight(0);
  waitShipLeave();
  changeLight(RED);
  moveBridge(DOWN);
  changeLight(0);
  moveBarrier(UP);
  changeLight(GREEN);
}

void loop() {
}
