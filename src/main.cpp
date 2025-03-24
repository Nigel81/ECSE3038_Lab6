#include <Arduino.h>
#include <WiFi.h>
#include "env.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal.h>
#include <DHTesp.h>

LiquidCrystal lcd(4, 5, 18, 19, 21, 22);

DHTesp dht;

void get_light_status();
void put_temperature_online();
String light = "OFF";

void setup() {
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  Serial.println("Hello client!");

  dht.setup(TEMP_SENSOR,DHTesp::DHT22);

  lcd.begin(16, 2);  // Initialize LCD (16x2)
  lcd.setCursor(0, 0);
  lcd.print("Hello!");
  
  if (IS_WOKWI) 
  WiFi.begin(SSID, PASS, CHANNEL);
  else 
  WiFi.begin(SSID, PASS);

  while(WiFi.status() != WL_CONNECTED){

    delay(500);
    Serial.print(".");
  }
  Serial.print("Wifi connect. IP address is: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);

  if (WiFi.status()==WL_CONNECTED){
    get_light_status();
    put_temperature_online();

  } else {
    Serial.println("Wifi Connection Lost");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Wifi Lost");
  }
  delay(1000);

}

void get_light_status() {
  Serial.println("Entering get_light");
  HTTPClient http;
  http.begin(String(ENDPOINT) + "/api/light");
  http.addHeader("api-key", (API_KEY));
  int responseCode = http.GET();

  Serial.print("HTTP Response: ");
  Serial.println(responseCode);

  if (responseCode == HTTP_CODE_OK) {
    String answer = http.getString();
    JsonDocument object;
    DeserializationError error = deserializeJson(object, answer);

    if (error) {
      Serial.println("Deserialization failed: ");
      Serial.println(error.c_str());
    } else {
      bool light_state = object["light"];

      digitalWrite(LED_PIN, light_state);

      if (light_state) {
        light = "ON";
          } 
      else {
        light = "OFF";
          }

      // Update LCD with light status
      lcd.setCursor(0, 0);
      lcd.print("Light: ");
      lcd.print(light);
      lcd.print("     ");

      Serial.print("Light Status: ");
      Serial.println(light);
    }
  } else {
    // Handle HTTP request failure
    String reponse = http.getString();
    Serial.println("HTTP Fail Response: ");
    Serial.println(reponse);
  }

  http.end();
}

void put_temperature_online(){
  Serial.println("Entering put_temperature");
  TempAndHumidity data = dht.getTempAndHumidity();

  if(dht.getStatus() != DHTesp::ERROR_NONE){
    Serial.println("Unable to read from sensor");
    lcd.setCursor(0,1);
    lcd.println("Invalid");
    return;
  }

  float temperature = data.temperature;
  JsonDocument object;
  object["temp"] = temperature;

  String request_body;
  serializeJson(object,request_body);

  HTTPClient http;
  http.begin(String(ENDPOINT) + "/api/temp");
  http.addHeader("api-key",(API_KEY));
  http.addHeader("Content-Type", "application/json");
  int responseCode = http.PUT(request_body);

  if(responseCode == HTTP_CODE_OK){
    String response = http.getString();
    Serial.println("Response: ");
    Serial.println(response);

    lcd.setCursor(0,1);
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print(" C   ");

    Serial.print("Temperature: ");
    Serial.println(temperature);

      }
      
  else{
    Serial.print("HTTP Request Failed: ");
    String response = http.getString();
    Serial.println(response);
  }
  http.end();
}