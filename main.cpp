#include <WiFi.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
 
const int lm35_pin = 34;
 
const int voltageSensor = 14;
float vOUT = 0.0;
float vIN = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
int value = 0;
 
 
float RLDR;
float Vout;
float Lux;
 
String apiKey = "*";
const char* ssid = "";  // Enter your WiFi Network's SSID
const char* pass = "";  // Enter your WiFi Network's Password
const char* server = "api.thingspeak.com";
 
WiFiClient client;
 
void setup() 
{
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Welcome To");
  lcd.setCursor(0, 1);
  lcd.print("Our Projects");
  delay(2000);
  lcd.clear();
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
 
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(100);
    Serial.print("*");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}
 
void loop() 
{
  int temp_adc_val;
  float temp_val;
  float tempF = temp_val * 9 / 5 + 32;
  temp_adc_val = analogRead(lm35_pin); /* Read Temperature */
  temp_val = (temp_adc_val * 4.88);    /* Convert adc value to equivalent voltage */
  temp_val = (temp_val / 10);          /* LM35 gives output of 10mv/°C */
   
  lcd.setCursor(0, 0);
  lcd.print("T:-");
  lcd.print(tempF);
  Serial.print("Temperature = ");
  Serial.print(tempF);
  delay(1000);
 
  value = analogRead(voltageSensor);
  vOUT = (value * 5.0) / 3724.0;
  vIN = vOUT / (R2 / (R1 + R2));
 
  int sensorValue = analogRead(35);
  Vout = (sensorValue * 0.0048828125);
  RLDR = (10000.0 * (3 - Vout)) / Vout;
  Lux = (RLDR / 500);
  lcd.setCursor(10, 0);
  lcd.print("L:");
  lcd.print(Lux);
  delay(1000);
 
  lcd.setCursor(0, 1);
  lcd.print("Solar Volt:");
  lcd.setCursor(12, 1);
  lcd.print(vIN);
 
  if (client.connect(server, 80)) 
  {
 
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(temp_val);
    postStr += "&field2=";
    postStr += String(vIN);
    postStr += "&field3=";
    postStr += String(Lux);
    postStr += "\r\n\r\n\r\n";
 
    client.print("POST /update HTTP/1.1\n");
    delay(100);
    client.print("Host: api.thingspeak.com\n");
    delay(100);
    client.print("Connection: close\n");
    delay(100);
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    delay(100);
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    delay(100);
    client.print("Content-Length: ");
    delay(100);
    client.print(postStr.length());
    delay(100);
    client.print("\n\n");
    delay(100);
    client.print(postStr);
    delay(100);
  }
  client.stop();
  Serial.println("Sending....");
  delay(15000);
}
 
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) 
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}