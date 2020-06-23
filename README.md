# Temperature-Humidity-displayed-on-webpage
In this project, web page will display the sensor's data(DHT-11) and will refresh to ensure updated information is presented. Alongside this, an LCD screen will also be interfaced to display the sensor data.
## Components Required
- Microcontroller
- Type-B USB cable
- ESP8266 Wi-Fi Module
- Breadboard
- Breadboard Power Supply Module
- Jumper Wires
- Wall Wart Power Supply (9v, 1A)
- DHT-11 Temperature/Humidity Sensor
- LCD Screen
- 1x 220 Ω Resistor
- 10 K Ω Potentiometer
- LCD
## Steps Invovled 
- Include all the necessary libraries
```
#include <DHT.h>
#include <DHT_U.h>
#include <SoftwareSerial.h> 
#include<Adafruit_Sensor.h>
#include<DHT.h>         
#include<LiquidCrystal.h>  
```
- Set the corresponding baud rates 
```
 esp.begin(115200);
```
- Intial LCD pins
```
const int reset = 12, enable = 11, lcdD4 = 5, lcdD5 = 4, lcdD6 = 3, lcdD7 = 2;

LiquidCrystal LCDboard(reset, enable, lcdD4, lcdD5, lcdD6, lcdD7); 
```
- Set rows and columns for the LCD screen
```
LCDboard.begin(16,2); 
```

- Send the commands for module to configured 
```
 transmit("AT+RST\r\n",2000,DEBUG); // Reset module
  Serial.print("Hello");
  Serial.print(humidity);
  transmit("AT+CWMODE=2\r\n",1000,DEBUG); // Configure ESP8266 as an AP (access point)
  transmit("AT+CIFSR\r\n",1000,DEBUG); // Get IP address
  transmit("AT+CIPMUX=1\r\n",1000,DEBUG); // Configure ESP8266 for multiple connections
  transmit("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // Turn on server on port 80
 
```
- Data transmission enabled then send command and HTML code to genrate the webpage

- Created the function that will send command to ESP module
```
String transmit(String esp_command, const int timeout, bool debug){
  String response;
  char c;
  esp.print(esp_command); 
  long int time = millis();
  while((time+timeout) > millis()){
    while(esp.available()){
      c = esp.read();
      response += c;
    }
  }
```
- error check 
```
  if(debug){
    Serial.print(response);
  }
  return response; 
}
```
