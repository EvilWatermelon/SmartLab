const int adc = A0;       

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

//Your Libaries
#include <ESP8266HTTPClient.h>


const int controlAddr = 0;                                  //EEPROM Adresse => 0 = keine Daten im EEPROM || 1 = Daten im EEPROM
const int dataAddr = 1;                                     //EEPROM Adresse => Count der Stellen der SSID

const char *ssidHost = "ESP_Config";                        //SSID falls eigener Hotspot erstellt wird
const char *passwordHost = "password";                      //Password falls eigner Hotspot erstellt wird

String ssid = "";                                           //Falls SSID im EEPROM vorhanden wird diese hier Global gespeichert
String password = "";                                       //Falls Password im EEPROM vorhanden wird dieses hier Global gespeichert

ESP8266WebServer server(80);            

/** WebPages **/
void webConfig() 
{
    server.send(200, "text/html", "<html><head></head><body><form action='http://192.168.4.1/configSubmit' method='POST'>\SSID: <input type='text' name='SSID'><br>\Password: <input type='password' name='password'><br>\<input type='submit' value='Submit'>\</form>\</body></html>");
}
void webConfigSubmit()
{
    server.send(200, "text/html", "<html><head></head><body><h1>The device will be configured and rebooted. Please wait.</h1></body></html>");
}

/** Config Functions **/
void newConfig()
{
    hotspot(ssidHost, passwordHost);                        //Opens a new Hotspot with the declared data for SSID & password
    server.begin();
}
void hotspot(const char *ssid,const char *password)
{
    WiFi.disconnect(true);                                  //Disconnect from WiFi => otherwise there will be errors
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();
    Serial.println(myIP);                                   //Shows the current IP (Hotspot 192.168.4.1)
}
void handleSubmit()
{
  String ssidTmp = "";
  String passwordTmp = "";
  
  if (server.args() > 0 )                                   //in server.args() are the input fields saved
  {
    for ( uint8_t i = 0; i < server.args(); i++ )           //loop all args and saves SSID and Password
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
  if(ssidTmp != "" and passwordTmp != "")                   //If SSID & Password != "" then the EEPROM saves the values
  {
    EEPROM.write(1, ssidTmp.length());                      //Save SSID length and distributes and saves the SSID at all places
    for(int i=0; i<ssidTmp.length(); i++)
    {
      EEPROM.write(i+2, ssidTmp[i]);    
    }
    EEPROM.write(2+ssidTmp.length(), passwordTmp.length()); //Saves after the last SSID length the password length and the password
    for(int y=0; y<passwordTmp.length(); y++)
    {
      EEPROM.write((y+3)+ssidTmp.length(), passwordTmp[y]);
    }
    EEPROM.write(controlAddr, 1);                           //Set first place of the EEPROM on 1 (SSID and password are saved)
    webConfigSubmit();
    EEPROM.commit();                                        //EEPROM Commit => finish write and save
    wifiConnect(ssidTmp, passwordTmp);
  } 
}


/** Functions **/
void wifiConnect(String ssidConnect,String passwordConnect)
{
    WiFi.disconnect(true);                                  //Disconnect from WiFi => otherwise there can be errors
    WiFi.begin(ssidConnect.c_str(), passwordConnect.c_str());
    /*
     * Try to connect to the Wifi
     * If there's after 10 seconds no connection he cancel and do a new configuration
     */
    int doCheck = 0;
    do
    {
        Serial.print(".");
        delay(1000);
        doCheck++;
    }while (WiFi.status() != WL_CONNECTED and doCheck<20);
    Serial.print("test");
    if(doCheck < 20)
    {
        Serial.print("work");
        Serial.println("\n"+WiFi.localIP());
        server.begin();
    }else{
        newConfig();                                        //If there's no Wifi connection he calls the config
    }
}

void httpPost()
{
    HTTPClient http;    //Declare object of class HTTPClient
     
    http.begin("http://192.168.2.81:80/lichtsensor");      //Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
 
    int httpCode = http.POST("brightness="+(String)analogRead(adc)+"");   //Send the request
    String payload = http.getString();                  //Get the response payload
 
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload
 
    http.end();  //Close connection
}
/** Setup & Loop function **/
void setup()
{
    Serial.begin(9600);
    EEPROM.begin(512);
    Serial.println("Start:");
    
    /*
     *DEBUG MODE => setzt die Controladdress auf 0 d.h. keine Wlan Daten gespeichert => Config
     *
     *EEPROM.write(controlAddr, 0);
     *EEPROM.commit();
     */


    int controlVal = EEPROM.read(controlAddr);              //Liest EEPROM(0) aus => 0 = Keine WLan Daten || 1 = Wlan Daten
    if(controlVal == 0)
    {
        newConfig();                                        //Neu konfigurieren
    }else{
        int ssidLength = EEPROM.read(1);                    //Liest SSID & Password aus dem EEPROM aus
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
        wifiConnect(ssid, password);                        //Calls WifiConnect => for the Wifi connection
    }

    /*
     *Serverpath handle  
     */                             
    server.on("/config", webConfig);                        //Config
    server.on("/configSubmit", handleSubmit);               //ConfigSubmit
}

int postTmp = 0;
int wertTmp = 0;

void loop()
{
    server.handleClient();                                  //Wait for website request
    
    if(WiFi.status()== WL_CONNECTED){
      Serial.println(analogRead(adc));
      if(postTmp >= 10){
        Serial.println(">=10"); 
        httpPost();
        wertTmp = analogRead(adc);
        postTmp = 0;
      }else{
        if(wertTmp > analogRead(adc) || wertTmp < analogRead(adc)){
          Serial.println("<10");
          httpPost();
          wertTmp = analogRead(adc);
          postTmp = 0;
        }else{
          postTmp++;
        }
      }
      delay(30000);
      
       
    }else{
      Serial.println("Error in WiFi connection"); 
    }
}




