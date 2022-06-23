#define BT

#include "PID.h"
#include "Sensors.h"
#include "Motors.h"
#include "Debug.h"
#include "mqtt.h"
#include "Cell.h"

Cells white, blue;

void setup()
{
#ifdef BT
  SerialBT.begin("ESP32_Sensor");
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
}

void loop()
{
  if (!client.connected())
    reconnect();
  client.loop();
  serial_debug();
  if (flag)
    follow_line(Speed);
  else
    follow_line(0);
  snprintf(msg, MSG_BUFFER_SIZE, "\nposition is %d", Position);
  client.publish(publish, msg);
}
