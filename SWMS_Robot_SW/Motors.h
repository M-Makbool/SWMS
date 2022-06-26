#include "Arduino.h"

#define MOTOR1_C1 19
#define MOTOR1_C2 21
#define MOTOR2_C1 22
#define MOTOR2_C2 23

int Position = 0;

PID Line;

int motor_offset[2] = {0, 0};
int Speed = 0;

void follow_line(int S)
{
  int E = Line.compute(0, Position, S * (-1), S);
  analogWrite(MOTOR2_C1, constrain(S - E, 0, S));
  analogWrite(MOTOR2_C2, 0);
  analogWrite(MOTOR1_C1, constrain(S + E, 0, S));
  analogWrite(MOTOR1_C2, 0);
}

int speed_handel(int8_t motor, int S)
{
  return map(S, 0, 255, motor_offset[motor - 1], 255);
}

void forword(int S)
{
  analogWrite(MOTOR2_C1, speed_handel(2, S));
  analogWrite(MOTOR2_C2, 0);
  analogWrite(MOTOR1_C1, speed_handel(1, S));
  analogWrite(MOTOR1_C2, 0);
}
void left(int S)
{
  analogWrite(MOTOR2_C2, S);
  analogWrite(MOTOR2_C1, 0);
  analogWrite(MOTOR1_C1, S);
  analogWrite(MOTOR1_C2, 0);
}
void right(int S)
{
  analogWrite(MOTOR2_C1, S);
  analogWrite(MOTOR2_C2, 0);
  analogWrite(MOTOR1_C2, S);
  analogWrite(MOTOR1_C1, 0);
}
void backword(int S)
{
  analogWrite(MOTOR2_C2, S);
  analogWrite(MOTOR2_C1, 0);
  analogWrite(MOTOR1_C2, S);
  analogWrite(MOTOR1_C1, 0);
}
void attachMotor()
{
  pinMode(MOTOR2_C1, OUTPUT);
  pinMode(MOTOR2_C2, OUTPUT);
  pinMode(MOTOR1_C1, OUTPUT);
  pinMode(MOTOR1_C2, OUTPUT);
  Line.reset();
  Line.K[0] = 5;
  Line.K[1] = 0.7;
  Line.K[2] = 0.7;
}
