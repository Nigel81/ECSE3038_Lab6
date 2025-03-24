**Purpose of the Lab**
<TO use an ESP32 to measure temperature and use a put request to upload temperature data to the internet. Also, to get the light status form an api using a get request.> 

**Summary**
<Get request:
When the ESP32 is connected to WiFi, a get request is used to get the status of the light switch using the function get light_status();
This is use to turn off or on the LED connected to the ESP32. Addtionally, the status of the LED is displayed on the screen.>

<Put request:
When the ESP32 is connected to WiFi, temperature is recorded from the sensor and upload to the api using the function put_temperature_online()>

<If there is no WiFi connection, none of the above functions will execute and the device will continue to wait for WiFi.>