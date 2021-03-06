#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const int controlAddr = 0;                                  //EEPROM Adress => 0 = no data in the EEPROM || 1 = Data in the EEPROM
const int dataAddr = 1;                                     //EEPROM Adress => Count of the places at the SSID

const char *ssidHost = "ESP_Config";                        //SSID if there will be an own hotspot
const char *passwordHost = "password";                      //Password if there will be an own hotspot

String ssid = "";                                           //If SSID saved in the EEPROM it will be saved global
String password = "";                                       //If Password saved in the EEPROM it will be saved global

ESP8266WebServer server(80);  

//Sonoff
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
          

/** WebPages **/
void webConfig() 
{
    server.send(200, "text/html", "<html><head><style>*{box-sizing: border-box;}form{width: 150px;margin-left: auto;margin-right: auto;margin-bottom: 1em;margin-top: 0em;display: block;font-family: -apple-system,BlinkMacSystemFont,\"Segoe UI\",Roboto,\"Helvetica Neue\",Arial,sans-serif,\"Apple Color Emoji\",\"Segoe UI Emoji\",\"Segoe UI Symbol\";font-size: 1rem;font-weight: 400;line-height: 1.5;color: #212529;text-align: left;}.form-group{margin-bottom: 1rem;display: block;font-family: -apple-system,BlinkMacSystemFont,\"Segoe UI\",Roboto,\"Helvetica Neue\",Arial,sans-serif,\"Apple Color Emoji\",\"Segoe UI Emoji\",\"Segoe UI Symbol\";font-size: 1rem;font-weight: 400;line-height: 1.5;color: #212529;text-align: left;}.form-control{display: block;width: 100%;padding: .375rem .75rem;font-size: 1rem;line-height: 1.5;color: #495057;background-color: #fff;background-clip: padding-box;border: 1px solid #ced4da;border-radius: .25rem;}input{overflow: visible;margin: 0;font-family: inherit;}.btn-primary{margin-left: auto;margin-right: auto;display: block;cursor: pointer;color: #fff;background-color: #007bff;border-color: #007bff;}.btn{font-weight: 400;text-align: center;white-space: nowrap;vertical-align: middle;user-select: none;border: 1px solid transparent;padding: .375rem .75rem;font-size: 1rem;line-height: 1.5;border-radius: .25rem;-webkit-appearance: button;text-transform: none;overflow: visible;margin: 0;font-family: inherit;}</style></head><body><form action='http://192.168.4.1/configSubmit' method='POST'><div class=\"form-group\"><input type=\"text\" name=\"SSID\" class=\"form-control\" id=\"inputSSID\"  placeholder=\"SSID\"></div><div class=\"form-group\"><input type=\"password\" name=\"password\" class=\"form-control\" id=\"inputPassword\"  placeholder=\"Password\"></div><button type=\"submit\" class=\"btn btn-primary\">Submit</button></form></body></html>");
}
void webConfigSubmit()
{
    server.send(200, "text/html", "<html><head></head><body><h1>Das Gerät wird Configuriert und anschließend neu gestartet. Bitte warten Sie.</h1></body></html>");
}

/** Config Functions **/
void newConfig()
{
    hotspot(ssidHost, passwordHost);                        //Creates a hotspot with the declared SSID & Password
    server.begin();
}
void hotspot(const char *ssid,const char *password)
{
    WiFi.disconnect(true);                                  //Disconnect from WiFi => otherwise there will be errors
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();
    Serial.println(myIP);                                   //Shows the current IP (hotspot 192.168.4.1)
}
void handleSubmit()
{
  String ssidTmp = "";
  String passwordTmp = "";
  
  if (server.args() > 0 )                                   //in server.args() there are saved Input fields
  {
    for ( uint8_t i = 0; i < server.args(); i++ )           //loops all args and saves SSID and Password
    {
      if (server.argName(i) == "SSID")              
      {
        ssidTmp = server.arg(i);
      }
      if(server.argName(i) == "password")
      {
        passwordTmp = server.arg(i);
      }
    }
  }
  if(ssidTmp != "" and passwordTmp != "")                   //If SSID & Password != "" all values will be written into the EEPROM 
  {
    EEPROM.write(1, ssidTmp.length());                      //Save SSID length and then distribute the SSID on all places and save
    for(int i=0; i<ssidTmp.length(); i++)
    {
      EEPROM.write(i+2, ssidTmp[i]);    
    }
    EEPROM.write(2+ssidTmp.length(), passwordTmp.length()); //After the last point of the SSID length save the password length + Password     
    for(int y=0; y<passwordTmp.length(); y++)
    {
      EEPROM.write((y+3)+ssidTmp.length(), passwordTmp[y]);
    }
    EEPROM.write(controlAddr, 1);                           //Create a place of the EEPROMS with value 1 (SSID & password are saved)
    webConfigSubmit();
    EEPROM.commit();                                        //EEPROM Commit => finish write and save
    wifiConnect(ssidTmp, passwordTmp);
  } 
}


/** Functions **/
void wifiConnect(String ssidConnect,String passwordConnect)
{
    WiFi.disconnect(true);                                  //Disconnect from WiFi => otherwise there will be errors
    WiFi.begin(ssidConnect.c_str(), passwordConnect.c_str());
    /*
     * Try to connect with the Wifi
     * If there is no connection after 10 seconds configure a new one
     */
    int doCheck = 0;
    do
    {
        Serial.print(".");
        delay(1000);
        doCheck++;
    }while (WiFi.status() != WL_CONNECTED and doCheck<20);
    
    if(doCheck < 20)
    {
        Serial.println("\n"+WiFi.localIP());
        server.begin();
    }else{
        newConfig();                                        //Calls Config if there is no Wifi connection
    }
}



//Sonoff Code
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


/** Setup & Loop function **/
void setup()
{
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
    Serial.begin(9600);
    EEPROM.begin(512);
    Serial.println("Start: ");
    

    /*
     *DEBUG MODE => set controladdress on 0 it means no Wifi connections will be saved => Config
     */
     //EEPROM.write(controlAddr, 0);
     //EEPROM.commit();
     


    int controlVal = EEPROM.read(controlAddr);              //Reads EEPROM(0) => 0 = no Wifi data || 1 = Wifi data
    if(controlVal == 0)
    {
        newConfig();                                        //New coniguration
    }else{
        int ssidLength = EEPROM.read(1);                    //Reads SSID & Password from the EEPROM 
        int passwordLength = EEPROM.read(2+ssidLength);
        for(int x=0; x<ssidLength; x++)
        {
            char ssidOutTmp = char(EEPROM.read(x+2));
            ssid = ssid + String(ssidOutTmp);
        }
        for(int z=0; z<passwordLength; z++)
        {
            char passwordOutTmp = char(EEPROM.read(z+3+ssidLength));
            password = password + String(passwordOutTmp);
        }
        wifiConnect(ssid, password);                        //Calls wifiConnect => for Wifi connect
    }

    /*
     *Serverpath handle  
     */                             
    server.on("/config", webConfig);                        //Config
    server.on("/configSubmit", handleSubmit);               //ConfigSubmit

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

void loop()
{
    server.handleClient();                                  //Wait for website request

}




