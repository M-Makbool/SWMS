#include "Arduino.h"

class PID
{
  public:
    uint32_t lastTime;
    volatile double output;
    double lastErr;
    double errSum;
    double K[3];

    PID()
    {
      K[0] = 0.1;
      K[1] = 0.3;
      K[2] = 0.13;
    }

    double compute(long setPoint, long sensor, double minOut, double maxOut)
    {
      // Calculate time difference since last time executed
      uint32_t now = millis();
      double timeChange = (double)(now - lastTime);

      // Calculate error (P, I and D)
      double error = setPoint - sensor;

      errSum += error * timeChange;

      errSum = constrain(errSum, minOut, maxOut);

      double dErr = (error - lastErr) / timeChange;

      // Calculate the new output by adding all three elements together
      double newOutput = (K[0] * error + K[1] * errSum + K[2] * dErr);

      // If limit is specifyed, limit the output
      output = constrain(newOutput, minOut, maxOut);

      lastErr = error;
      lastTime = now;

      return (output);
    }
    void reset()
    {
      errSum = 0;
      lastErr = 0;
      lastTime = 0;
    }
};
