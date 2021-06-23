#include "DHT22.h"
#include "MQ7.h"
#include "PM25.h"
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <String>
#include <WebServer.h>
#include <WiFi.h>

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
MQ7 mq7(35, 21);

WiFiServer wifi(80);
WiFiClient client;
WebServer server(80);
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

float temperature = 0;
float humidity = 0;
float dustDensity = 0;
float co = 0;

/********* Function *********/
void readSensor();

void printClient(WiFiServer, float, float, float, float, int);
String sendHTML(float, float, float, int);

void MQTT_connect();
void mqttTask(void *);
void webServerTask(void *);

void loadWiFi();

void setup()
{
  Serial.begin(9600);
  pm25.begin();
  mq7.begin();
  EEPROM.begin(400);

  loadWiFi();

  xTaskCreate(&mqttTask, "Adafruit MQTT", 2000, NULL, 2, NULL);
  xTaskCreate(&webServerTask, "Web Server", 2000, NULL, 2, NULL);
}

void loop() {}

void mqttTask(void *pvParameters)
{
  Adafruit_MQTT_Publish temperatureAdafruit = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/air-info.temperature");
  Adafruit_MQTT_Publish humidityAdafruit = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/air-info.humidity");
  Adafruit_MQTT_Publish dustDensityAdafruit = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/air-info.dustdensity");
  Adafruit_MQTT_Publish coAdafruit = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/air-info.co");
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

    if (!coAdafruit.publish(dustDensity))
      Serial.println(F("CO: Failed"));
    else
      Serial.println(F("CO: OK!"));
    delay(5 * 60 * 1000);
  }
}

void webServerTask(void *pvParameters)
{
  for (;;)
  {
    readSensor();
    printClient(wifi, temperature, humidity, dustDensity, co);
    delay(5000);
  }
}

void readSensor()
{
  temperature = dht.getTemperature();  // Gets the values of the temperature
  humidity = dht.getHumidity();        // Gets the values of the humidity
  dustDensity = pm25.getDustDensity(); // Gets the values of the dust density
  co = mq7.getPPM();                   // Gets the values of the CO

  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Dust Density: ");
  Serial.println(dustDensity);
  Serial.print("CO: ");
  Serial.println(co);
}

String sendHTML(float temperature, float humidity, float dustDensity, float co)
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
  str += "<tr><td>CO</td><td><span class=\"sensor\">\n";
  str += co;
  str += " ppm</span></td></tr>\n";
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

void printClient(WiFiServer &sever, float temperature, float humidity, float dustDensity, float co)
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
    client.print(sendHTML(temperature, humidity, dustDensity, co));
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

    delay(5000);

    retries--;
    if (retries == 0)
    {
      while (1)
        ;
    }
  }

  Serial.println("MQTT connected");
}

void loadWiFi()
{

  if (!checkWiFiCreds())
  {
    Serial.println("No WiFi credentials stored in memory. Loading form...");
    while (loadWiFiCredsForm())
      ;
  }
}

/**
 * @brief Function if don't generate web_server
 */
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
 * @return true if save successful, else false 
 */
bool writeToMemory(String ssid, String pass)
{
  char buff1[30];
  char buff2[30];
  ssid.toCharArray(buff1, 30);
  pass.toCharArray(buff2, 30);
  EEPROM.writeString(100, buff1);
  EEPROM.writeString(200, buff2);
  delay(100);
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
  response_error += "<h2><a href='/'>Go back</a> to try again";

  if (writeToMemory(String(server.arg("ssid")), String(server.arg("password"))))
  {
    server.send(200, "text/html", response_success);
    EEPROM.commit();
    delay(3000);
    ESP.restart();
  }
  else
    server.send(200, "text/html", response_error);
}

/**
 * @brief Function for home page
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
 * @return true if hagit ve WiFi creds in EEPROM, else false 
 */
bool loadWiFiCredsForm()
{
  String s = EEPROM.readString(100);
  String p = EEPROM.readString(200);

  const char *ssid = "ESP32 WiFi Manager";

  Serial.println("Setting Access Point...");

  WiFi.softAP(ssid);

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
    delay(100);
  }
  return false;
}

/**
 * @brief Function checking WiFi creds in memory
 * @return true if not empty, else if empty 
 */
bool checkWiFiCreds()
{
  Serial.println("Checking WiFi credentials");
  String s = EEPROM.readString(100);
  String p = EEPROM.readString(200);

  delay(5000);
  if (s.length() <= 0 || p.length() <= 0)
    return false;

  Serial.print("Found credentials: ");
  Serial.print(s);
  Serial.print("/");
  Serial.println(p);

  delay(2000);
  Serial.print("Connecting to WiFi Network ");
  Serial.print(s);
  WiFi.begin(s.c_str(), p.c_str());
  int startTimeConnect = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (millis() - startTimeConnect > 60 * 1000)
    {
      Serial.println("False");
      return false;
    }
  }
  Serial.println("");
  Serial.println("Successfully connected to WiFi.");
  Serial.println("IP address of ESP32 is ");
  Serial.println(WiFi.localIP());
  Serial.println("Server started");
  wifi.begin();
  return true;
}

/**
 * @brief Wipe EEPROM
 */
void wipeEEPROM()
{
  for (int i = 0; i < 400; i++)
  {
    EEPROM.writeByte(i, 0);
  }
  EEPROM.commit();
}
