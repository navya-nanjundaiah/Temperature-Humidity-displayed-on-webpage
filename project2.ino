#include <DHT.h>
#include <DHT_U.h>

// Project 2
#include <SoftwareSerial.h> // This library allows for seial communication on other digital
                            // pins of the Arduino (rather than just using pins 0 and 1)
                            // note: Arduino UART has a 64 byte serial buffer!

#include<Adafruit_Sensor.h>
#include<DHT.h>          // This library is used for the DHT-11 sensor. It allows for keywords
                            // such as readHumidity(), readTemperature(), etc. See gitHub for more
                            // info.

#include<LiquidCrystal.h>   // This library allows for use of LCD.                             
                            
#define DEBUG true          
#define DHTTYPE DHT11       // Create instance for sensor
#define DHTPIN A0       // Connect sensor to Arduino pin 

// variables for dht-11 data
float humidity;
float tempCelcius;
float tempFahrenheit;
float heatIndexF;

// Initialize pins of LCD and create instance for module
const int reset = 12, enable = 11, lcdD4 = 5, lcdD5 = 4, lcdD6 = 3, lcdD7 = 2;

LiquidCrystal LCDboard(reset, enable, lcdD4, lcdD5, lcdD6, lcdD7); // Initialize LCD

SoftwareSerial esp(8,9);    // Arduino Rx is now pin #, Tx, is now pin # 
DHT dht(DHTPIN, DHTTYPE);   // Initialize the DHT sensor
                            
void setup(){
  Serial.begin(9600);      // Set baud rate of serial data transmission
  esp.begin(115200);         // This baud rate should match the ESP8266 module (may differ)
  dht.begin();
  LCDboard.begin(16,2);     // (Number of columns,rows of LCD module)

  transmit("AT+RST\r\n",2000,DEBUG); // Reset module
  Serial.print("Hello");
  Serial.print(humidity);
  transmit("AT+CWMODE=2\r\n",1000,DEBUG); // Configure ESP8266 as an AP (access point)
  transmit("AT+CIFSR\r\n",1000,DEBUG); // Get IP address
  transmit("AT+CIPMUX=1\r\n",1000,DEBUG); // Configure ESP8266 for multiple connections
  transmit("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // Turn on server on port 80
}

void loop(){
  delay(2000);
  LCDboard.clear();  
  humidity = dht.readHumidity(); // Get humidity percentage
  Serial.print(humidity);
  tempCelcius = dht.readTemperature(); // Get temperature in Celcius 
  tempFahrenheit = dht.readTemperature(true); // Convert from Celcius to Fahrenheit
  heatIndexF = dht.computeHeatIndex(tempFahrenheit, humidity); // Calculate the heat index
  
  if(esp.available()){ // Ensure data transmission
    if(esp.find("+IPD,")){
      delay(1000);
      int id = esp.read()-48; // Read() returns ASCII decimal value, 0 starts at 48
      
      // This is what will display on the created webpage (written in HTML)
      // Note: your webpage format should look exactly like the class example
      String webpage = "<!DOCTYPE html><html><head><style>table, th, td { border: 1px solid black; padding: 5px;}table {border-spacing: 0px;}</style>";

      // This creates the command that communicates how many bytes are going to be sent over
      String cipsend = "AT+CIPSEND=";
      cipsend += id;
      cipsend += ",";
      cipsend += webpage.length();
      cipsend += "\r\n";

      // Send the command then send webpage display
      transmit(cipsend,1000,DEBUG);
      transmit(webpage,1000,DEBUG);

      // Repeat to display the rest of the webpage output
      String webpage1 = "<title>EGCP 565</title></head><body><h1 style=text-align:center;><mark>EGCP-565 Project-3</mark></h1>";
      String webpage2="<table style=width:100%><tr><th>Humidity</th><th>Tem: Celcius</th><th>Tem: Fahrenheit</th><th>HeatIndex</th></tr>";
      webpage=webpage1+webpage2;
      cipsend = "AT+CIPSEND=";
      cipsend += id;
      cipsend += ",";
      cipsend += webpage.length();
      cipsend += "\r\n";
      
      transmit(cipsend,1000,DEBUG);
      transmit(webpage,1000,DEBUG);

      // Display the humidity
      webpage = "<tr><td>";  
      webpage += humidity;
      webpage += "</td>";
     cipsend = "AT+CIPSEND=";
      cipsend += id;
      cipsend += ",";
      cipsend += webpage.length();
      cipsend += "\r\n";
      
      transmit(cipsend,1000,DEBUG);
      transmit(webpage,1000,DEBUG);

      // Display the temperature in Celcius 
      webpage = "<td>"; 
      webpage += tempCelcius;
      webpage += "</td>";     // Use hex html value for degree Celcius symbol
      cipsend = "AT+CIPSEND=";
      cipsend += id;
      cipsend += ",";
      cipsend += webpage.length();
      cipsend += "\r\n";
      
      transmit(cipsend,1000,DEBUG);
      transmit(webpage,1000,DEBUG);

      // Display the temperature in Fahrenheit
      webpage = "<td>";  
      webpage += tempFahrenheit;
      webpage += "</td>";     // Use hex html value for degree Fahrenheit sybmol
      cipsend = "AT+CIPSEND=";
      cipsend += id;
      cipsend += ",";
      cipsend += webpage.length();
      cipsend += "\r\n";
      
      transmit(cipsend,1000,DEBUG);
      transmit(webpage,1000,DEBUG);

      // Display the heat index (what the temperature feels like, in Fahrenheit)
      webpage = "<td>>"; 
      webpage += heatIndexF;
      webpage += "</td></table>"; // Use hex html value for degree Fahrenheit sybmol
      cipsend = "AT+CIPSEND=";
      cipsend += id;
      cipsend += ",";
      cipsend += webpage.length();
      cipsend += "\r\n";
      
      transmit(cipsend,1000,DEBUG);
      transmit(webpage,1000,DEBUG);

     /* // This code allows for the webpage to refresh every 10 seconds. Allows
      // for fresh data to be displayed. 
      webpage = ###;
      cipsend = "AT+CIPSEND=";
      cipsend += id;
      cipsend += ",";
      cipsend += webpage.length();
      cipsend += "\r\n";
      
      transmit(cipsend,1000,DEBUG);
      transmit(webpage,1000,DEBUG);*/

      // Append the connection (you did it right if the browser finishes loading the page)
      String cipclose = "AT+CIPCLOSE=";
      cipclose += id;
      cipclose += "\r\n";

      transmit(cipclose,3000,DEBUG);
    }
  }
  
  // Display humidity onto LCD board
  LCDboard.blink(); 
  delay(1000);
  LCDboard.noBlink();  
  LCDboard.print("Humidity: "); // display "Humidity: " onto lcd screen
  delay(1000);
  LCDboard.setCursor(0,0); // move cursor to the next line 
  LCDboard.print("="); // display humidity value (ex: "= 53 %")
  LCDboard.print(humidity);
  LCDboard.print(" ");
  delay(1000);
  LCDboard.clear(); // clear the lcd screen

  // Display temperature in Celcius onto LCD board
  LCDboard.blink();
  delay(1000);
  LCDboard.noBlink();
  LCDboard.print("Temperature: "); // display "Temperature: " onto lcd screen
  delay(1000);
  LCDboard.setCursor(0,0); // move cursor to the next line 
  LCDboard.print("="); // display temperature in Celcius (ex: "= 17 °C") 
  LCDboard.print(tempCelcius);
  LCDboard.print(" ");
  LCDboard.print((char)223); // ASCII value for degree symbol
  LCDboard.print("C");
  delay(1000);
  LCDboard.clear(); // clear the lcd screen

  // Display temperature in Fahrenheit onto LCD board
  LCDboard.blink();
  delay(1000);
  LCDboard.noBlink();
  LCDboard.print("Temperature: "); // display "Temperature: " onto lcd screen
  delay(1000);
  LCDboard.setCursor(0,0); // move cursor to the next line
  LCDboard.print("="); // display temperature in Fahrenheit (ex: "= 62 °F")
  LCDboard.print(tempFahrenheit);
  LCDboard.print(" ");
  LCDboard.print((char)223); // ASCII value for degree symbol
  LCDboard.print("F");
  delay(1000);
  LCDboard.clear(); // clear the lcd screen

  // Display heat index in Fahrenheit
  LCDboard.blink();
  delay(1000);
  LCDboard.noBlink();
  LCDboard.print("Feels like: "); // display "Feels like: " onto lcd screen
  delay(1000);
  LCDboard.setCursor(0,0); // move cursor to the next line 
  LCDboard.print("="); // display temperature in Fahrenheit (ex: "= 62 °F")
  LCDboard.print(heatIndexF);
  LCDboard.print(" ");
  LCDboard.print((char)223); // ASCII value for degree symbol
  LCDboard.print("%");
  delay(1000);
  LCDboard.clear(); // clear the lcd screen
}

// This function will be used to send the appropriate commands to the ESP module
// automatically, rather than manulally inputing them in the serial monitor
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

  // Error check
  if(debug){
    Serial.print(response);
  }
  return response; 
}