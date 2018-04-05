#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <RFID.h>

const char* ssid = "moxd-lab";
const char* password = "gf3heTS11c";
const char* serverIP = "192.168.2.166";
const char* stuhlID="1";
// constants won't change. They're used here to set pin numbers:
const int weigthPin = 2;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
#define SS_PIN 4  //D2
#define RST_PIN 5 //D1

RFID rfid(SS_PIN, RST_PIN); 

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
bool sittingState = false;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Serial Open");
  Serial.println("Try to Connect to:");
  Serial.println(ssid);
  Serial.println(password);
  WiFi.begin(ssid, password);
  Serial.print("Connecting.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(weigthPin, INPUT);
  
  SPI.begin();  // Init SPI bus
  rfid.init();
}



 
void sendSittingState() {
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
     Serial.println("tryToSend: SittingState"); 
    HTTPClient http;  //Declare an object of class HTTPClient
    //http.begin("http://jsonplaceholder.typicode.com/users/1");
    String sState;
    if(sittingState) {
      sState = "0";
    }else{
      sState = "1";
    }
    http.begin("http://"+(String)serverIP+"/sitState/"+(String)stuhlID+"/"+sState); 
    int httpCode = http.GET();//Send the request
 
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload
    }
    http.end();   //Close connection
  }else{
    Serial.println("noConnection");
  }
}

void sendRFIDState(String tagID) {
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    Serial.println("tryToSend: tagID"); 
    HTTPClient http;  //Declare an object of class HTTPClient
    String sState;
    if(sittingState) {
      sState = "0";
    }else{
      sState = "1";
    }
    String sendingString = "http://"+(String)serverIP+"/rfidState/"+(String)stuhlID+"/"+(String)tagID;
    Serial.println(sendingString);   
    http.begin(sendingString); 
    int httpCode = http.GET();//Send the request
 
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload
    }
    http.end();   //Close connection
  }else{
    Serial.println("noConnection");
  }
}

String getRFID(){
   String serial = "";
   int serNum0;
  int serNum1;
  int serNum2;
  int serNum3;
  int serNum4;
  //Serial.print("try To read rfid");
  if (rfid.isCard()) 
    {
        if (rfid.readCardSerial()) 
        {
                // einlesen der serial-frames
                serNum0 = rfid.serNum[0]; 
                serNum1 = rfid.serNum[1];
                serNum2 = rfid.serNum[2];
                serNum3 = rfid.serNum[3];
                serNum4 = rfid.serNum[4];
                 
                serial = String(serNum0, HEX) + String(serNum1, HEX) + String(serNum2, HEX) + String(serNum3, HEX) + String(serNum4, HEX);
                serial.toUpperCase();
                
                Serial.print("NFC-Serial: ");
                Serial.print(serial);
                Serial.println();
                
                Serial.println();
                delay(2000);
          }
    }
    
    rfid.halt();
    return serial;
}


void loop() {
  
  // read the state of the pushbutton value:
  bool stoolState = digitalRead(weigthPin);
  
  String tagID = getRFID();
  if(tagID !=""){
    sendRFIDState(tagID);
  }

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (sittingState != stoolState) {
    sittingState = stoolState;
    digitalWrite(ledPin, sittingState);
    if(sittingState){
      Serial.println("SittingStae: 1");
    }else{
      Serial.println("SittingStae: 0");
    }
    sendSittingState();
  }
}



