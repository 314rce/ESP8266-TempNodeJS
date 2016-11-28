//This makes use of standard ESP8266 libraries and 
//uses an ESP8266 WebSocket Client: https://github.com/314rce/ESP8266-Websocket

#include "DHT.h" //The temperature sensor device
#include <ESP8266WiFi.h> // The Microcontroller 
#include <WebSocketClient.h> // Arduino library for a web socket client
#include <ESP8266WebServer.h>//Microcontroller's library for Web Server

#define DHTPIN 2 

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//Create an instance of WiFiClient
WiFiClient client;

//create an instance of WebSocketClient
WebSocketClient webSocketClient;

//Config Values
const char* ssid = " "; //Enter your WiFi's SSID (its name)
const char* pass = " "; //Enter Wifi's password
const char* servIP = " "; //Connect to Socket Server IP address
const float socketPort = " "; //Port of the Web Socket
char path[] = "/"; //the root locattion
char host[] = "http://localhost"; //our Client


void setup() {
  
  //Initialize our serial monitor:
  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

  //Attempt to connect to ssid with pass
  WiFi.begin(ssid,pass);
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(" . ");
  }

  //Show our own IP Address
  Serial.println();
  Serial.print("IP Address of this device: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //Attempt to connect to Socket Server
  if (client.connect(servIP,socketPort)){ 
    Serial.println("connected.");
  } else {
    Serial.println("Connection failed.");
  }

  webSocketClient.path = path;
  webSocketClient.host = host;

  //Attempt handshake with Web Socket Server
  if(webSocketClient.handshake(client)){
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");
  }
  
  //Begin taking temperature data.
  dht.begin();
}

void loop() {

  //initializing variables 
  float h=dht.readHumidity(); //reads humidity percentage 
  float t=dht.readTemperature(); // reads temperature in Celsius
  float f=dht.readTemperature(true); // reads temperature in Fahrenheit
  float hif = dht.computeHeatIndex(f, h); // this function will calculate the heat index in F
  float hic = dht.computeHeatIndex(t, h); // this function will calculate the heat index in C


  //if DHT device doesn't have any values for humidity or temperature
  if (isnan(h) || isnan(t) || isnan(f)){
    Serial.println("Failed to read from DHT sensor.");
    return;
  }

  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.print("Temperature: ");
  Serial.println(f);
  Serial.print("Heat Index in Fahrenheit: ");
  Serial.println(hif);
  Serial.println();

  //send Data over the Web Socket
  if(client.connected()){
    //sending temperature in fahrenheit
    webSocketClient.sendData(String(f));
  }
  delay(10000); // takes measurements every 10 seconds.
}
