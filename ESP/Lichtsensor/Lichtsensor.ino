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
    server.send(200, "text/html", "<html><head></head><body><h1>Das Gerät wird Configuriert und anschließend neu gestartet. Bitte warten Sie.</h1></body></html>");
}

/** Config Functions **/
void newConfig()
{
    hotspot(ssidHost, passwordHost);                        //Erstellt einen Hotspot mit den oben deklarierten Daten für SSID & Password
    server.begin();
}
void hotspot(const char *ssid,const char *password)
{
    WiFi.disconnect(true);                                  //Disconnect vom WiFi => andernfalls kann es zu Fehlern kommen
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();
    Serial.println(myIP);                                   //Gib die aktuelle IP Adresse aus (normalfall bei Hotspot 192.168.4.1)
}
void handleSubmit()
{
  String ssidTmp = "";
  String passwordTmp = "";
  
  if (server.args() > 0 )                                   //in server.args() sind die Input Felder gespeichert
  {
    for ( uint8_t i = 0; i < server.args(); i++ )           //loopt alle args durch und speichert SSID und Password ab (sofern eingegeben)
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
  if(ssidTmp != "" and passwordTmp != "")                   //Sofern SSID & Password != "" sind werden die Werte in den EEPROM geschrieben
  {
    EEPROM.write(1, ssidTmp.length());                      //SSID Länge einspeichern danach die SSID auf die einzelnen stellen verteilen und speichern
    for(int i=0; i<ssidTmp.length(); i++)
    {
      EEPROM.write(i+2, ssidTmp[i]);    
    }
    EEPROM.write(2+ssidTmp.length(), passwordTmp.length()); //Hinter die letzte Stelle der SSID Länge die Password Länge + Password speichern
    for(int y=0; y<passwordTmp.length(); y++)
    {
      EEPROM.write((y+3)+ssidTmp.length(), passwordTmp[y]);
    }
    EEPROM.write(controlAddr, 1);                           //Erste Stelle des EEPROMS auf 1 setzen (SSID & Password sind gespeichert)
    webConfigSubmit();
    EEPROM.commit();                                        //EEPROM Commit => Write abschließen und speichern
    wifiConnect(ssidTmp, passwordTmp);
  } 
}


/** Functions **/
void wifiConnect(String ssidConnect,String passwordConnect)
{
    WiFi.disconnect(true);                                  //Disconnect vom WiFi => andernfalls kann es zu Fehlern kommen
    WiFi.begin(ssidConnect.c_str(), passwordConnect.c_str());
    /*
     * Versuchen mit Wlan zuverbinden
     * Sollte nach 10 Sekunden keine Verbindung möglich sein springt er raus und ruft zum neu konfigurieren auf
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
        newConfig();                                        //Ruft Config auf sofern er keine WiFi Verbindung herstellen kann
    }
}

/** Deine Funktionen **/
/** Viel Spaß **/
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
        wifiConnect(ssid, password);                        //Ruft WifiConnect auf => um sich mit Wlan zuverbinden
    }

    /*
     *Serverpfad händeln  
     */                             
    server.on("/config", webConfig);                        //Config
    server.on("/configSubmit", handleSubmit);               //ConfigSubmit

    /** Dein Setup Code **/
    /** Viel Spaß **/
}

int postTmp = 0;
int wertTmp = 0;

void loop()
{
    server.handleClient();                                  //Auf Anfrage der Website warten

    /** Dein Loop Code **/
    /** Viel Spaß **/
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




