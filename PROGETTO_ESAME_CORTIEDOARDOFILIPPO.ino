/*DIGITAL LIGHT SENSOR

THIS CODE MAKES AN ARDUINO MKR WIFI 1010 BOARD READ A Digital_Light_TSL2561 sensor
TEMPERATURE AND HUMIDITY SENSOR AND PRINT THE DATA THANKS TO
THE WIFI CONNECTION ON A WEB PAGE WHICH STRUCTURE IS BASED ON THE
HTML CODE. THE WEB PAGE, HAS TO ALLOW THE USER TO SET THE 
THRESHOLD OF THE SENSOR OUTSIDE ONLY READING THE DATA.
THE ADD ON OF THE PROJECT CONSISTS IN WORKING PEER TO PEER WITH 
A TWIN MKR WIFI 1010 BOARD EQUIPPED WITH DHT11 HUMIDITY AND TEMPERATURE SENSOR THAT HAS TO MAKE THE SAME JOB OF THIS BOARD

///Politecnico di Milano - Bachelor Degree course - Informatics course
///Relators: Prof Marco Domenico Santambrogio - Ing Andrea Damiani

////Code by Edoardo Filippo Corti

/////Resources:

-https://www.arduino.cc/
-https://github.com/
-https://www.seeedstudio.com/
-https://www.youtube.com/?hl=it&gl=IT
-phone contacts

NEEDED HARDWARE (for the whole project): 
-2 Arduino MKR 1010 WiFi
-1 Digital_Light_TSL2561 sensor
-1 DHT11 sentor
-2 breadboard
-9 LED
-wires

NEEDED LIBRARIES:
-Digital_Light_TSL2561 own library https://wiki.seeedstudio.com/Grove-Digital_Light_Sensor/
-WiFININA https://www.arduino.cc/en/Reference/WiFiNINA
-SPI https://www.arduino.cc/en/reference/SPI
-Wire https://www.arduino.cc/en/reference/wire


*/
#include <Wire.h>                   //this library allows you to communicate with I2C/TWI devices
#include <Digital_Light_TSL2561.h>  //this is the library of the sensor
#include <SPI.h>                    //this libray allows you to communicate with SPI devices, with the arduino as the master device
#include <WiFiNINA.h>               //this library allows you to use the Arduino MKR 1010 WiFI capabilities. it can serve either as a server accepting incoming connections or a client making outgoing ones
#include "arduino_secrets.h"        //this library is create to protect your  WiFi sensible data, like passwords

//enter your sensitive data in the Secret tab/arduino_secrets.h (side tab)
char ssid[] = SECRET_SSID;        
char pass[] = SECRET_PASS;

WiFiServer server(80);  //set the server port

int status = WL_IDLE_STATUS;

WiFiClient Actor;
char OtherSensorIP[]="192.168.1.101";  //the other board ip is printed on the Serial Monitor of the other program 
int Temperature=0;
int Humidity=0;  
int Light=0;
int TempThr=0;              //set the native threshold for the light sensor
int HumThr=0;               //set the native threshold for the light sensor
int LightThr=100;           //set the native threshold for the light sensor

void setup() {

  checkLed();
  
  server.begin();
  
  Wire.begin();        //Initialize Wire library
  TSL2561.init();      //Initialize sensor library
    
  Serial.begin(9600);  //Initialize serial and wait for port to open
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 
  Serial.println("DIGITAL LIGHT SENSOR");  //title printed on the Serial Monitor

  inizializeWiFi();  //the board try to connect with the WiFi
}



void loop() {

  recurrentWiFiCheck();    //call the function recurrentWiFi check

  lightSensor();           //the sensor do it's work

  webServerDataPrint();    //manage the web server/pages

}



void checkLed(){  //check if all the led are working

  pinMode(6, OUTPUT);   //Set analog pin number 6 as an output to control the light indicator led on the breadboard (Red)
  pinMode(7, OUTPUT);   //Set analog pin number 7 as an output to control the allarm led on the breadboard (Blue)
  pinMode(8, OUTPUT);   //Set analog pin number 8 as an output to control the WiFi led on the breadboard (Green)
  pinMode(9, OUTPUT);   //Set analog pin number 9 as an output to control the Server led on the breadboard (Yellow)

  digitalWrite(6, HIGH);
  delay(500);
  digitalWrite(6, LOW);
  delay(500);
  
  digitalWrite(7, HIGH);
  delay(500);
  digitalWrite(7, LOW);
  delay(500);
  
  digitalWrite(8, HIGH);
  delay(500);
  digitalWrite(8, LOW);
  delay(500);

  digitalWrite(9, HIGH);
  delay(500);
  digitalWrite(9, LOW);
  delay(500);
  
  }

  
void inizializeWiFi(){  //the board try to connect to your WIFI
  
// check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("\nCommunication with WiFi module failed!");
    // don't continue
    while (true);
  }

  for (int i=0; (status != WL_CONNECTED) && i < 2; i++){
    Serial.print("\nAttempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

   if (status != WL_CONNECTED){
     Serial.println("\nWiFI failed to connect!\n");
   } else{
      Serial.println("Connected to wifi");
      digitalWrite(8, HIGH);
      printWifiStatus(); //display the network basic information via the printWIFiStatus funcion
      } 
}


void printWifiStatus() {  //this function print on the Serial Monitor the WiFi data
  
  Serial.print("SSID: ");          //print the SSID of the network you're attached to
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();   //print your board IP address
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();         //print the received signal strength
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void recurrentWiFiCheck(){  //check if the board is on line, if not turn off the green led
  
   if (status != WL_CONNECTED){
     Serial.println("\nBoard Offline\n");
     digitalWrite( 8, LOW); //turn off the green led
   }
}


void lightSensor(){                  //the sensor start to collect data, print them on the serial monitor and communicate with the LED buil-it and on the breadbord
  
  //the sensor start to collect data after the board has check the connection
  Light = TSL2561.readVisibleLux();
  
  Serial.print("\nVisibleLight: ");
  Serial.print(Light);              //print on the Serial Monitor the measurements relevated by the sensor
  Serial.print(" Lux");
  Serial.print("\nThreshold: ");
  Serial.print(LightThr);           //print on the Serial Monitorr the threshold set by the user (also on the web page)
  Serial.print(" Lux");
  if(Light > LightThr){             //if the light measured by the sensor is higher than the threshold print a notification message on the Serial Monitor and turn on the red and the builtin LED
    Serial.print("\nNOTE: threshold exceeded\n");
    digitalWrite(7, LOW);           //turn off LIGHT (blue)
    digitalWrite(6, HIGH);          //turn on notification LED (red) and builtin LED
    delay(5000);
    }else{                          //else if the light measured by the sensor is minor than the threshold turn on the blue LED
        digitalWrite(7, HIGH);      //turn on LIGHT (blue)
        digitalWrite(6, LOW);       //turn off notification LED (red) and the builtin LED
        Serial.print("\n\n");
        delay(5000);                //set a delay between each relevation from the sensor
    }

    if (Actor.connect(OtherSensorIP, 80)) {  //prepare the relevation of the sensor to be sent to the other arduino
      // Make a HTTP request:
      Actor.println("LIGHT=" + String(Light));  //send the measurements to the other arduino
      Actor.println("LIGHTTHR=" + String(LightThr));  //send the threshold to the other arduino
      Actor.stop();
    }
}


void webServerDataPrint(){ //this function manage the web server/pages
   
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             
    Serial.println("new client");           // Print a message out the serial port
    digitalWrite(9, HIGH);                  // Turn on the yellow led
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            server.println("HTTP/1.1 200 OK");
            server.println("Content-type:text/html");
            server.println("Refresh: 5");    
            server.println();
            server.println("<!DOCTYPE HTML>");
            server.println("<html>");
            server.println("<h1>Visible Light Sensor</h1>");  //primary web page title
            server.println("<h2>Real-time surveys</h2>");  //primary web page subtitle

            //show the data on the web page
            server.print("Visible light is: ");  //print on the web page the light measured by the sensor
            server.print(TSL2561.readVisibleLux());
            server.print(" Lux");
            server.print("<br  />");
            server.print("Threshold is: ");  //print on the web page the threshold set by the user (also via the web server)
            server.print(LightThr);
            server.print(" Lux");
            server.print("<br />");
            if (TSL2561.readVisibleLux() > LightThr){  //if the light measured by the sensor is highet than the threshold print a notification message on the Web page
                server.print("NOTE: Threshold exceeded");
                server.print("<br />");
              }
               server.println("received Temperature=" + String(Temperature));  //print on the webpage the temperature value received from the other arduino
               server.println("received TemperatureThreshold=" + String(TempThr));        //print on the webpage the temperature threshold recrived from the other arduino
               server.println("received Humidity=" + String(Humidity));        //print on the webpage the humidity value recrived from the other arduino
               server.println("received HumidityThreshold=" + String(HumThr));        //print on the webpage the humidity threshold recrived from the other arduino     
               server.println();

            //button to redirect the user to the new page where he can set the thresholds
            server.println("<br />");
            server.println("Click <a href=\"/NEWPAGE\">here</a> to change the sensor threshold<br>");  //a button that redirect the user to the secondary page created for set the new threshold
            server.println("<br />");
            server.println("Click <a href=\"/STOP\">here</a> to confirm the new threshold");  //a button tha stop the incremntatio/decrementation of the threshold
            server.println("<br />");
            
            // The HTTP response ends with another blank line:
            client.println();
            break;
          } else {
              int tmp = currentLine.indexOf("HUM="); //tmp= variabile temporanea
              if (tmp>-1){
                Humidity=currentLine.substring(tmp+4).toInt();
                Serial.println("received Humidity=" + String(Humidity));  //print on the Serial Monitor the humidity value recrived from the other arduino
              }
              tmp = currentLine.indexOf("TEMP=");  
              if (tmp>-1){
                Temperature=currentLine.substring(tmp+5).toInt();
                Serial.println("received Temperature=" + String(Temperature));  //print on the Serial Monitor the humidity value recrived from the other arduino
              }
              tmp = currentLine.indexOf("LIGHT=");
              if (tmp>-1){
                Light=currentLine.substring(tmp+4).toInt();
                Serial.println("received Light=" + String(Light));
              }
              tmp = currentLine.indexOf("HUMTHR=");
              if (tmp>-1){
                HumThr=currentLine.substring(tmp+7).toInt();
                Serial.println("received HumidityThreshold=" + String(HumThr));  //print on the Serial Monitor the humidity Threshold recrived from the other arduino
              }
              tmp = currentLine.indexOf("TEMPTHR=");
              if (tmp>-1){
                TempThr=currentLine.substring(tmp+8).toInt();
                Serial.println("received TemperatureThreshold=" + String(TempThr));  //print on the Serial Monitor the temperature Threshold recrived from the other arduino
              }
              tmp = currentLine.indexOf("LIGHTTHR=");
              if (tmp>-1){
                LightThr=currentLine.substring(tmp+7).toInt();
                Serial.println("received LightThreshold=" + String(LightThr));
              }
    
            currentLine = "";    // if you got a newline, then clear currentLine:
          }
          } else if (c != '\r') {  //if you got anything else but a carriage return character,
          currentLine += c;        //add it to the end of the currentLine
         }

          // Check to see if the client request was "GET /H" or "GET /L":
          if (currentLine.endsWith("GET /+50Lux")) {
            LightThr = LightThr + 50;
          }
        
          if (currentLine.endsWith("GET /-50Lux")) {
              LightThr = LightThr - 50;
          }

          if (currentLine.endsWith("GET /STOP")) {  //if the line ends with /STOP the incremntatio/decrementation of the threshold stop
              //LightThr = LightThr;
          }

          if (currentLine.endsWith("GET /RESET")) {  //if the line ends with /RESET the threshold will be reset to 100 Lux
             LightThr = 100;
          }
        
          if (currentLine.endsWith("GET /NEWPAGE")){
            server.println();
            server.println("<!DOCTYPE HTML>");
            server.println("<html>");
            server.println("<h1>Visible Light Sensor</h1>");       //secondary page title
            server.println("<h2>Sensor threshold settings</h2>");  //secondary page subtitle
            server.println("<body>");
            server.println("Click <a href=\"/+50Lux\">here</a> to increase the Visibile Light threshold of 50 Lux every 5 sec");  //a button that make the threshold increment by 50 lux every 5 second
            server.println("<br />");
            server.println();
            server.println("Click <a href=\"/-50Lux\">here</a> to decrease the Visible Light threshold of 50 Lux every 5 sec");  //a button that make the threshold decrement by 50 lux every 5 second
            server.println("<br />");
            server.println("<br />");
            server.println("Click <a href=\"/RESET\">here</a> to reset the native threshold (100Lux)");  //a button that reset the native threshold
            server.println("<br />");
            server.println("<br />");
            server.println("Threshold is: ");  //print on the secondary web page the new threshold
            server.println(LightThr);
            server.println("<br />");
            server.println("<br />");
            server.println("Click <a href=\"/STOP\">here</a> to confirm the new threshold");  //a button that stop the incremntatio/decrementation of the threshold
            server.println("<br />");
            server.println("Click <a href=\"/ip\">here</a> to rerturn to homepage<br>");  //a button that redirect you to the homepage
            server.println("<br />");
            server.println();           
            server.println("</body>");
            server.println("</html>");
            server.println();
            break;
          
          }
        }
      }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}
