#include "Arduino.h"
#include "BluetoothSerial.h"

#ifdef BT
#define com SerialBT
BluetoothSerial SerialBT;
#else
#define com Serial
#endif

bool flag = 0;

void serial_debug()
{
  if (com.available())
  {
    char Received = com.read();
    int motor = 0;
    int Kn;

    switch (Received)
    {

    case 'c':
      com.println("\nPlease Enter Calibration Time.");
      while (!com.available())
        ;
      com.println("\nCalibration on progres .....");
      right(Speed);
      calibrate(com.parseInt(), toggle);
      forword(0);
      break;

    case 'P':
      com.printf("\nRobot Position is  %d ", get_robot_position(toggle));
      break;

    case 'h':
      forword(0);
      break;
    case 'f':
      forword(Speed);
      break;
    case 'b':
      backword(Speed);
      break;
    case 'l':
      left(Speed);
      break;
    case 'r':
      right(Speed);
      break;
    case 'B':
      B = com.parseInt();
      break;
    case 'S':
      Speed = com.parseInt();
      break;

    case 'R':
      get_sensor_raw_value(Sensed, toggle);
      for (int i = 0; i < SAZ; i++)
        com.printf("\nRAW Sensor %d = %d\t", i, Sensed[i]);
      break;

    case 's':
      get_sensor_smoothed_value(Sensed, toggle);
      for (int i = 0; i < SAZ; i++)
        com.printf("\nSmoothed Sensor %d = %d\t", i, Sensed[i]);
      break;

    case 'O':
      motor = com.parseInt();
      com.printf("\nPlease Enter Motor %d Offset.", motor);
      com.read();
      while (!com.available())
        ;
      motor_offset[motor - 1] = com.parseInt();
      com.printf("\nMotor %d Offset = %d", motor, motor_offset[motor - 1]);
      break;

    case 'K':
      Kn = com.parseInt();
      com.printf("\nPlease Enter K %d .", Kn);
      com.read();
      while (!com.available())
        ;
      Line.K[Kn] = com.parseFloat();
      com.printf("\nK %d Offset = %f", Kn, Line.K[Kn]);
      break;

    case 'o':
      flag = !flag;
      break;
    case 'T':
      toggle = !toggle;
      com.printf("\nToggle is %s", toggle ? "true" : "false");
      break;

    default:
      com.printf("\n%c\n", Received);
    }
  }
}
