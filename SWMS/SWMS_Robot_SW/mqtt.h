#include "Arduino.h"
#include <WiFi.h>
#include <PubSubClient.h>

const char *ssid = "MAK WiFi";
const char *password = "BasYaBaba7a";
const char *mqtt_server = "192.168.1.7";
const char *publish = "Blue";
const char *subscribe = "BlueSlave";
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  com.println();
  com.print("Connecting to ");
  com.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    com.print(".");
  }

  randomSeed(micros());

  com.println("");
  com.println("WiFi connected");
  com.println("IP address: ");
  com.println(WiFi.localIP());
}

int parseInt(byte *c)
{
  boolean isNegative = false;
  int value = 0;
  if (c[0] < 0)
  {
    return 0; // zero returned if timeout
  }

  do
  {
    if (c[0] == '-')
    {
      isNegative = true;
    }
    else if (c[0] >= '0' && c[0] <= '9')
    { // is c a digit?
      value = value * 10 + c[0] - '0';
    }
    *c++;
  } while ((c[0] >= '0' && c[0] <= '9'));

  if (isNegative)
  {
    value = -value;
  }
  return value;
}

float parseFloat(byte *c)
{
  boolean isNegative = false;
  boolean isFraction = false;
  long value = 0;
  float fraction = 1.0;
  // ignore non numeric leading characters
  if (c[0] < 0)
  {
    return 0; // zero returned if timeout
  }

  do
  {
    if (c[0] == '-')
    {
      isNegative = true;
    }
    else if (c[0] == '.')
    {
      isFraction = true;
    }
    else if (c[0] >= '0' && c[0] <= '9')
    { // is c a digit?
      value = value * 10 + c[0] - '0';
      if (isFraction)
      {
        fraction *= 0.1f;
      }
    }
    *c++;
  } while ((c[0] >= '0' && c[0] <= '9') || c[0] == '.');

  if (isNegative)
  {
    value = -value;
  }
  if (isFraction)
  {
    return value * fraction;
  }
  else
  {
    return value;
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  com.print("Message arrived [");
  com.print(topic);
  com.print("] ");
  for (int i = 0; i < length; i++)
  {
    com.print((char)payload[i]);
  }
  com.println();

  int motor = 0;
  int Kn;

  switch (payload[0])
  {
    case 'c':
      snprintf(msg, MSG_BUFFER_SIZE, "\nCalibration on progres .....");
      client.publish(publish, msg);
      right(Speed);
      calibrate(parseInt(payload + 1), toggle);
      forword(0);
      break;

    case 'P':
      snprintf(msg, MSG_BUFFER_SIZE, "\nRobot Position is  %d ", get_robot_position(toggle));
      client.publish(publish, msg);
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
      B = parseInt(payload + 1);
      break;
    case 'S':
      Speed = parseInt(payload + 1);
      break;

    case 'R':
      get_sensor_raw_value(Sensed, toggle);
      for (int i = 0; i < SAZ; i++)
      {
        snprintf(msg, MSG_BUFFER_SIZE, "\nRAW Sensor %d = %d\t", i, Sensed[i]);
        client.publish(publish, msg);
      }
      break;

    case 's':
      get_sensor_smoothed_value(Sensed, toggle);
      for (int i = 0; i < SAZ; i++)
      {
        snprintf(msg, MSG_BUFFER_SIZE, "\nSmoothed Sensor %d = %d\t", i, Sensed[i]);
        client.publish(publish, msg);
      }
      break;

    case 'O':
      motor = parseInt(payload + 1);
      motor_offset[motor - 1] = parseInt(payload + 2);
      snprintf(msg, MSG_BUFFER_SIZE, "\nMotor %d Offset = %d", motor, motor_offset[motor - 1]);
      client.publish(publish, msg);
      break;

    case 'K':
      Kn = parseInt(payload + 1);
      Line.K[Kn] = parseFloat(payload + 2);
      snprintf(msg, MSG_BUFFER_SIZE, "\nK %d Offset = %f", Kn, Line.K[Kn]);
      client.publish(publish, msg);
      break;

    case 'o':
      flag = !flag;
      break;

    case 'T':
      toggle = !toggle;
      snprintf(msg, MSG_BUFFER_SIZE, "\nToggle is %s", toggle? "true" : "false");
      client.publish(publish, msg);
      break;
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    com.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "SWMSClient-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      com.println("connected");
      // Once connected, publish an announcement...
      client.publish(publish, "hello world");
      // ... and resubscribe
      client.subscribe(subscribe);
    }
    else
    {
      com.print("failed, rc=");
      com.print(client.state());
      com.println(" try again in 3 seconds");
      // Wait 5 seconds before retrying
      delay(3000);
    }
  }
}
