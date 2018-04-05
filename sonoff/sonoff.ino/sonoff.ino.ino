#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// wireless network credentials:
const char* ssid = "moxd-lab";
const char* password = "gf3heTS11c";

//hostname + Port
const char* host = "webcontrol";
ESP8266WebServer server(80);

//PowerLED
const short int BUILTIN_POWERLED = 13; //GPIO13
//BUTTONS
const short int BUILTIN_BUTTON0 = 0; //GPIO0
const short int BUILTIN_BUTTON1 = 9; //GPIO9
const short int BUILTIN_BUTTON2 = 10; //GPIO10
const short int BUILTIN_BUTTON3 = 13; //GPIO14

//LEDS
const short int BUILTIN_LED0 = 12; //GPIO12
const short int BUILTIN_LED1 = 5; //GPIO5
const short int BUILTIN_LED2 = 4; //GPIO4
const short int BUILTIN_LED3 = 15; //GPIO15

//Main WebPage
String getContent()
{
  // create content for the website:
  String content = "<html><head><title>ESP8266 WebControl</title></head><body>";
  content += "This page shows the current state of the ESP8266 outputs and can be used to control these.<br /><br />";

  content += "Output0: <a href=\"powerOne\"><button>1 An</button></a> <a href=\"nopowerOne\"><button>1 Aus</button></a><br />";
  
  content += "Output1: <a href=\"powerTwo\"><button>2 An</button></a> <a href=\"nopowerTwo\"><button>2 Aus</button></a><br />";
 
  content += "Output2: <a href=\"powerThree\"><button>3 An</button></a> <a href=\"nopowerThree\"><button>3 Aus</button></a><br />";
  
  content += "Output3: <a href=\"powerFour\"><button>4 An</button></a> <a href=\"nopowerFour\"><button>4 Aus</button></a><br />";
 

  content += "</body></html>";

  return content;
}

void setup() {
  //Power LED
  pinMode(BUILTIN_POWERLED, OUTPUT);
  //Button Input
  pinMode(BUILTIN_BUTTON0, INPUT);
  pinMode(BUILTIN_BUTTON1, INPUT);
  pinMode(BUILTIN_BUTTON2, INPUT);
  pinMode(BUILTIN_BUTTON3, INPUT);
  //LED Output
  pinMode(BUILTIN_LED0, OUTPUT);
  pinMode(BUILTIN_LED1, OUTPUT);
  pinMode(BUILTIN_LED2, OUTPUT);
  pinMode(BUILTIN_LED3, OUTPUT);

  // initialize serial port for debugging purpose:
  Serial.begin(115200);

  // connect to the wireless network:
  WiFi.begin(ssid, password);
  // wait for wireless network connection and print connection settings:
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("Wireless network connection established.");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // initialize mDNS:
  if (MDNS.begin(host)) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("MDNS responder started");
    Serial.print("You can now connect to http://");
    Serial.print(host);
    Serial.println(".local");
  }
  
  // start HTTP server:
  server.begin();
  Serial.println("HTTP server started");
  
  digitalWrite(BUILTIN_POWERLED, HIGH);
  
  // print start page:
  server.on("/", [](){
    server.send(200, "text/html", getContent());
  });



  // control One:
  server.on("/powerOne", [](){
    digitalWrite(BUILTIN_LED0, HIGH);
  

    server.send(200, "text/html", getContent());
    delay(10);
  });

  server.on("/powerTwo", [](){
    digitalWrite(BUILTIN_LED1, HIGH);
   

    server.send(200, "text/html", getContent());
    delay(10);
  });

  server.on("/powerThree", [](){
    digitalWrite(BUILTIN_LED2, HIGH);
    

    server.send(200, "text/html", getContent());
    delay(10);
  });

  server.on("/powerFour", [](){
    digitalWrite(BUILTIN_LED3, HIGH);
    

    server.send(200, "text/html", getContent());
    delay(10);
  });




  server.on("/nopowerOne", [](){
    digitalWrite(BUILTIN_LED0, LOW);
    
    server.send(200, "text/html", getContent());
    delay(10);
  });

  server.on("/nopowerTwo", [](){
    digitalWrite(BUILTIN_LED1, LOW);
    
    server.send(200, "text/html", getContent());
    delay(10);
  });

  server.on("/nopowerThree", [](){
    digitalWrite(BUILTIN_LED2, LOW);
    
    server.send(200, "text/html", getContent());
    delay(10);
  });

  server.on("/nopowerFour", [](){
    digitalWrite(BUILTIN_LED3, LOW);
    
    server.send(200, "text/html", getContent());
    delay(10);
  });
  
}

void loop() {
  // handle HTTP request:
  server.handleClient();
}



