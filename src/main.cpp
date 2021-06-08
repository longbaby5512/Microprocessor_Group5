#include "DHT22.h"
//#include "HTML.h"
#include "MQ7.h"
#include "PM25.h"
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <String>
#include <WebServer.h>
#include <WiFi.h>

#define NOP() asm volatile("nop")

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "duclong1209"
#define AIO_KEY "4127c18370b6431cb863c82b0c075c3d"

const char INDEX_HTML[] =
    "<!DOCTYPE HTML>"
    "<html>"
    "<head>"
    "<meta content=\"text/html; charset=ISO-8859-1\""
    " http-equiv=\"content-type\">"
    "<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
    "<title>ESP8266 Web Form Demo</title>"
    "<style>"
    "\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; text-align:center;}\""
    "</style>"
    "</head>"
    "<body>"
    "<h3>Enter your WiFi credentials</h3>"
    "<form action=\"/\" method=\"post\">"
    "<p>"
    "<label>SSID:&nbsp;</label>"
    "<input maxlength=\"30\" name=\"ssid\"><br>"
    "<label>Key:&nbsp;&nbsp;&nbsp;&nbsp;</label><input maxlength=\"30\" name=\"password\"><br>"
    "<input type=\"submit\" value=\"Save\">"
    "</p>"
    "</form>"
    "</body>"
    "</html>";

/********* Variables *********/
DHT22 dht(18);
PM25 pm25(34, 19);
// MQ7 mq7(35, 17);

WebServer server(80);
WiFiServer wifi(80);
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

float temperature = 0;
float humidity = 0;
float dustDensity = 0;

/********* Function *********/

void IRAM_ATTR delayMilliseconds(uint32_t ms)
{
  delayMicroseconds(1000 * ms);
}

void readSensor()
{

  temperature = dht.getTemperature();  // Gets the values of the temperature
  humidity = dht.getHumidity();        // Gets the values of the humidity
  dustDensity = pm25.getDustDensity(); // Gets the values of the dust density
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Dust Density: ");
  Serial.println(dustDensity);
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

/**
 * @brief Function for writing WiFi creds to EEPROM
 * @param ssid ssid of WiFi to write to EEPROM
 * @param pass password of WiFi to write to EEPROM
 * @return rue if save successful, else false 
 */
bool writeToMemory(String ssid, String pass)
{
  char buff1[30];
  char buff2[30];
  ssid.toCharArray(buff1, 30);
  pass.toCharArray(buff2, 30);
  EEPROM.writeString(100, buff1);
  EEPROM.writeString(200, buff2);
  delayMicroseconds((int)100e3);
  String s = EEPROM.readString(100);
  String p = EEPROM.readString(200);

  if (ssid == s && pass == p)
    return true;
  return false;
}

/**
 * @brief Function for handling form
 */
void handleSubmit()
{
  String response_success = "<h1>Success</h1>";
  response_success += "<h2>Device will restart in 3 seconds</h2>";

  String response_error = "<h1>Error</h1>";
  response_error += "<h2><a href='/'>Go back</a>to try again";

  if (writeToMemory(String(server.arg("ssid")), String(server.arg("password"))))
  {
    server.send(200, "text/html", response_success);
    EEPROM.commit();
    delayMicroseconds((uint32_t)3000e3);
    ESP.restart();
  }
  else
    server.send(200, "text/html", response_error);
}

/**
 * @brief Function for home page
 * 
 */
void handleRoot()
{
  if (server.hasArg("ssid") && server.hasArg("password"))
    handleSubmit();
  else
    server.send(200, "text/html", INDEX_HTML);
}

/**
 * @brief Function for loading form
 * 
 * @return true if have WiFi creds in EEPROM, else false 
 */
bool loadWiFiCredsForm()
{
  String s = EEPROM.readString(100);
  String p = EEPROM.readString(200);

  const char *ssid = "ESP32 WiFi Manager";
  const char *password = "admin";
  // const char *password = "12345678";

  Serial.println("Setting Access Point...");

  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();

  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.println("HTTP server started");

  while (s.length() <= 0 && p.length() <= 0)
  {
    server.handleClient();
    delayMilliseconds(100);
  }

  return false;
}

/**
 * @brief Function checking WiFi creds in memory
 * 
 * @return true if not empty, else if empty 
 */
bool checkWiFiCreds()
{
  Serial.println("Checking WiFi credentials");
  String s = EEPROM.readString(100);
  String p = EEPROM.readString(200);
#if DEBUG
  Serial.print("Found credentials: ");
  Serial.print(s);
  Serial.print("/");
  Serial.print(p);
  delayMilliseconds(5000);
#endif
  if (s.length() > 0 && p.length() > 0)
    return true;
  return false;
}

/**
 * @brief Wipe EEPROM
 * 
 */
void wipeEEPROM()
{
  for (int i = 0; i < 400; i++)
  {
    EEPROM.writeByte(i, 0);
  }
  EEPROM.commit();
}

String sendHTML(float temperature, float humidity, float dustDensity)
{
  String str = "<!DOCTYPE html><html>\n";
  str += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  str += "<link rel=\"icon\" href=\"data:,\">\n";
  str += "<style>body { text-align: center; font-family: \"Arial\", Arial;}\n";
  str += "table { border-collapse: collapse; width:40%; margin-left:auto; margin-right:auto;border-spacing: 2px;background-color: white;border: 4px solid green; }\n";
  str += "th { padding: 20px; background-color: #008000; color: white; }\n";
  str += "tr { border: 5px solid green; padding: 2px; }\n";
  str += "tr:hover { background-color:yellow; }\n";
  str += "td { border:4px; padding: 12px; }\n";
  str += ".sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }\n";
  str += "</style></head><body><h1>ESP32 Web Server Reading sensor values</h1>\n";
  str += "<table><tr><th>MEASUREMENT</th><th>VALUE</th></tr>\n";
  str += "<tr><td>Temperature</td><td><span class=\"sensor\">\n";
  str += temperature;
  str += " &deg;C</span></td></tr>\n";
  str += "<tr><td>Humidity</td><td><span class=\"sensor\">\n";
  str += humidity;
  str += " %</span></td></tr>\n";
  str += "<tr><td>Dust Density</td><td><span class=\"sensor\">\n";
  str += dustDensity;
  str += " &mu;g/m³</span></td></tr>\n";
  // str += "<tr><td>CO</td><td><span class=\"sensor\">\n";
  // str += co;
  // str += " ppm</span></td></tr>\n";
  str += "</body>\n";
  str += "<script>\n";
  str += "setInterval(loadDoc,200);\n";
  str += "function loadDoc() {\n";
  str += "var xhttp = new XMLHttpRequest();\n";
  str += "xhttp.onreadystatechange = function() {\n";
  str += "if (this.readyState == 4 && this.status == 200) {\n";
  str += "document.getElementById(\"webpage\").innerHTML =this.responseText}\n";
  str += "};\n";
  str += "xhttp.open(\"GET\", \"/\", true);\n";
  str += "xhttp.send();\n";
  str += "}\n";
  str += "</script></html>";
  return str;
}

void printClient(WiFiServer &sever, float temperature, float humidity, float dustDensity)
{
  WiFiClient client = sever.available();
  if (client)
  {
    Serial.println("Web Client connected ");
    String request = client.readStringUntil('\r');
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
    client.print(sendHTML(temperature, humidity, dustDensity));
    client.println();
    client.stop();
    Serial.println("");
  }
}

void MQTT_connect()
{
  int8_t ret;

  //Stop if already connected
  if (mqtt.connected())
    return;

  Serial.println("Connecting to MQTT...");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();

    delayMilliseconds(5000);

    retries--;
    if (retries == 0)
    {
      while (1)
        ;
    }
  }

  Serial.println("MQTT connected");
}

void mqttTask(void *pvParameters)
{
  Adafruit_MQTT_Publish temperatureAdafruit = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
  Adafruit_MQTT_Publish humidityAdafruit = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
  Adafruit_MQTT_Publish dustDensityAdafruit = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/dustdensity");
  for (;;)
  {

    MQTT_connect();
    if (!temperatureAdafruit.publish(temperature))
      Serial.println(F("Temperature: Failed"));
    else
      Serial.println(F("Temperature: OK!"));
    if (!humidityAdafruit.publish(humidity))
      Serial.println(F("Humidity: Failed"));
    else
      Serial.println(F("Humidity: OK!"));
    if (!dustDensityAdafruit.publish(dustDensity))
      Serial.println(F("Dust Density: Failed"));
    else
      Serial.println(F("Dust Density: OK!"));
    delayMilliseconds(5 * 60 * 1000);
  }
}

void webServerTask(void *pvParameters)
{
  for (;;)
  {
    readSensor();
    printClient(wifi, temperature, humidity, dustDensity);
    delayMilliseconds(1 * 60 * 1000);
  }
}

void setup()
{
  Serial.begin(9600);
  pm25.begin();
  // mq7.begin();
  pinMode(2, OUTPUT);
  pinMode(15, INPUT); //for resetting WiFi creds
  EEPROM.begin(400);
  if (!checkWiFiCreds())
  {
    Serial.println("No WiFi credentials stored in memory. Loading form...");
    digitalWrite(2, HIGH);
    while (loadWiFiCredsForm())
      ;
  }

  String ssid = EEPROM.readString(100);
  String password = EEPROM.readString(200);
  Serial.print("Connecting to WiFi Network ");
  Serial.print(ssid);
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED)
  {
    delayMicroseconds((uint32_t)500e3);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Successfully connected to WiFi.");
  Serial.println("IP address of ESP32 is : ");
  Serial.println(WiFi.localIP());
  Serial.println("Server started");
  wifi.begin();
  xTaskCreate(&mqttTask, "Adafruit MQTT", 2000, NULL, 1, NULL);
  xTaskCreate(&webServerTask, "Web Server", 2000, NULL, 1, NULL);
}

void loop()
{
  if (digitalRead(15) == HIGH)
  {
    Serial.println("Wiping WiFi credentials from memory...");
    wipeEEPROM();
    while (loadWiFiCredsForm())
      ;
  }

  digitalWrite(2, HIGH);
  delayMilliseconds(1000);
  digitalWrite(2, LOW);
  delayMilliseconds(1000);
}
