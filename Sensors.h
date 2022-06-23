#include "Arduino.h"

#define numReading 7
#define SAZ 5 // Sensor array size
#define LED_C1 13
#define LL 36
#define L 34
#define C 35
#define R 32
#define RR 33

PID Bright;

const static uint8_t Sensors[SAZ] = {LL, L, C, R, RR};
int16_t Sensed[SAZ];

// variables for smoothing
uint8_t iindex = 0;
int Total[SAZ];
int16_t Readings[SAZ][numReading];
int16_t Maximum[SAZ], Minimum[SAZ], Threshold[SAZ];
int B;
bool toggle;


double correlation(int16_t *X, int16_t *Y)
{
  double MeanX, MeanY, sX, sY, sXY;
  MeanX = MeanY = sX = sY = sXY = 0;

  for (int i = 0; i < SAZ; i++)
  {
    MeanX += X[i];
    MeanY += Y[i];
  }
  MeanX /= SAZ;
  MeanY /= SAZ;

  for (int i = 0; i < SAZ; i++)
  {
    sX += (X[i] - MeanX) * (X[i] - MeanX);
    sY += (Y[i] - MeanY) * (Y[i] - MeanY);
    sXY += (X[i] - MeanX) * (Y[i] - MeanY);
  }
  return (sXY / sqrt(sX * sY));
}

void get_sensor_raw_value(int16_t *sensor_value, bool Toggle)
{
  if (Toggle)
  {
    int16_t LED_off_value[SAZ];
    analogWrite(LED_C1, 0);
    delay(20);

    LED_off_value[0] = analogRead(LL);
    LED_off_value[1] = analogRead(L);
    LED_off_value[2] = analogRead(C);
    LED_off_value[3] = analogRead(R);
    LED_off_value[4] = analogRead(RR);

    analogWrite(LED_C1, B);
    delay(20);

    sensor_value[0] = analogRead(LL) - LED_off_value[0];
    sensor_value[1] = analogRead(L) - LED_off_value[1];
    sensor_value[2] = analogRead(C) - LED_off_value[2];
    sensor_value[3] = analogRead(R) - LED_off_value[3];
    sensor_value[4] = analogRead(RR) - LED_off_value[4];

    analogWrite(LED_C1, 0);
  }
  else
  {
    analogWrite(LED_C1, B);

    sensor_value[0] = analogRead(LL);
    sensor_value[1] = analogRead(L);
    sensor_value[2] = analogRead(C);
    sensor_value[3] = analogRead(R);
    sensor_value[4] = analogRead(RR);
  }

  int avareg = 0;
  for (int i = 0; i < SAZ; i++)
    avareg += sensor_value[i];
  avareg /= SAZ;
  B = Bright.compute(300, avareg, 1, 255);
}

void get_sensor_smoothed_value(int16_t *sensor_value, bool mode)
{
  get_sensor_raw_value(sensor_value, mode);
  for (int i = 0; i < SAZ; i++)
  {
    Total[i] -= Readings[i][iindex];
    Readings[i][iindex] = sensor_value[i];
    Total[i] += Readings[i][iindex];
    sensor_value[i] = Total[i] / numReading;
  }
  iindex++;
  if (iindex >= numReading)
    iindex = 0;
}

void calibrate(unsigned long calTime, bool mode)
{
  int16_t Cal[SAZ];
  for (int i = 0; i < SAZ; i++)
  {
    Maximum[i] = 0;
    Minimum[i] = 10000;
    Threshold[i] = 0;
  }

  for (int i = 0; i < numReading; i++)
    get_sensor_smoothed_value(Cal, mode);

  unsigned long now = millis();
  while (millis() - now < calTime)
  {
    get_sensor_smoothed_value(Cal, mode);
    for (int i = 0; i < SAZ; i++)
    {
      if (Maximum[i] < Cal[i])
        Maximum[i] = Cal[i];
      if (Minimum[i] > Cal[i])
        Minimum[i] = Cal[i];
    }
  }
  for (int i = 0; i < SAZ; i++)
    Threshold[i] = (Minimum[i] + Maximum[i]) / 2;
}

int8_t get_robot_position(bool mode)
{
  int16_t Sens[SAZ], T[SAZ];
  int8_t Po = 0;
  double r[SAZ * 2];
  get_sensor_raw_value(Sens, mode);
  for (int i = 0; i < SAZ; i++)
    T[i] = Maximum[i];

  for (int i = 0; i < SAZ; i++)
  {
    T[i] = Minimum[i];
    r[2 * i] = correlation(Sens, T);
    T[i] = Maximum[i];
  }

  for (int i = 0; i < SAZ - 1; i++)
  {
    T[i] = Minimum[i];
    T[i + 1] = Minimum[i + 1];
    r[(2 * i) + 1] = correlation(Sens, T);
    T[i] = Maximum[i];
    T[i + 1] = Maximum[i + 1];
  }
  // r[-2] = correlation(Sens, Minimum);
  r[-1] = correlation(Sens, Minimum);
  for (int i = 1; i < (SAZ * 2) ; i++)
    if (r[i] > r[Po])
      Po = i;
  return Po - SAZ + 1;
}

void attachSensors()
{
  analogReadResolution(10);
  pinMode(LED_C1, OUTPUT);
  analogWrite(LED_C1, 0);
  pinMode(LL, INPUT);
  pinMode(L, INPUT);
  pinMode(C, INPUT);
  pinMode(R, INPUT);
  pinMode(RR, INPUT);
  Bright.reset();
}
