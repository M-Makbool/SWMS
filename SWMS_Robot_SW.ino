#define BT

#include "PID.h"
#include "Sensors.h"
#include "Motors.h"
#include "Debug.h"
#include "mqtt.h"
#include "Cell.h"

Cells white, blue, green;

void setup()
{
#ifdef BT
  SerialBT.begin("White");
#else
  Serial.begin(115200);
#endif
  attachSensors();
  attachMotor();
  while (!com.available())
  {
  }
  com.println("استعنا عالشقا بالله");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  if (!client.connected())
    reconnect();

  green.mouseColumn = 0;
  green.mouseRow = 0;
  green.mouseHeading = 0;
}

void loop()
{
  if (!client.connected())
    reconnect();
  client.loop();
  serial_debug();
  Position = get_robot_position(toggle);
  Move_Cell(3);
}

void Move_Cell(byte N)
{
  if (flag)
  {
    for (int8_t i = 0; i < N; i++)
    {
      forword(Speed);
      delay(250);
      do
      {
        Position = get_robot_position(toggle);
        switch (Position)
        {
        case 6:
          break;
        case 5:
          if (get_robot_position(toggle) == 5)
          {
            follow_line(0);
            break;
          }
        default:
          follow_line(Speed);
        }
      } while (get_robot_position(toggle) != 5);
      green.mouseRow++;
    }
    forword(0);
    flag = false;
    snprintf(msg, MSG_BUFFER_SIZE, "\nRow is %d", green.mouseRow);
    client.publish(publish, msg);
  }
}
