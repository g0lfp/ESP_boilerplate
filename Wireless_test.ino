// ESP boiler plate.
// by G0LFP
// Has no actual functionality, but enables a wifi update client.
// Except on a relay board it enables the relay. 
// http://192.168.x.y/?relay=1 to turn the relay on
// http://192.168.x.y/?relay=0 to turn the relay off again

// The opto isolated input part isn't coded into this version.

// Load this into a new ESPDuino so that all you have to do is apply power.
// Connect your phone to select WiFi access point.
// Then you can upload your program 'OTA'

// http://192.168.x.y/update to send a .bin of your new program to the ESP OTA.

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>

#include "WiFiManager.h"          //https://github.com/tzapu/WiFiManager

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

const char* host = "esp8266-http-Update";
#define LEDPIN 2
#define RELAYPIN 4

void setup() 
{
  // Setup code goes here, runs once
  Serial.begin(115200);
  pinMode(LEDPIN,OUTPUT);
  pinMode (RELAYPIN,OUTPUT);

   //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing - maybe addd a config pin tto.
  //wifiManager.resetSettings();    Uncommenting this line forces a reset to wifi settings.
                                    // This means you will have to connect and config wifi every time you boot the device.

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect()) 
  {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    //delay(1000);
  } 

  // Once you get to this point, you have connected successfully to your wifi
  Serial.println("Connected!");
  long ver = ESP.getChipId();
  Serial.print("Conn:ESP");
  Serial.println(ver,DEC);
  Serial.print("Local IP address:");
  Serial.println(WiFi.localIP());
  

  
  // This chunk enables the update bin server.
    MDNS.begin(host);
    httpUpdater.setup(&httpServer);
    httpServer.begin();
    MDNS.addService("http", "tcp", 80);
    //Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);

    httpServer.on ( "/", handleRoot );  //see the function at the end of the code. 
}

void loop(void)
{

  httpServer.handleClient();
    
  HTTPClient http;
  //==============================================================

        int temper = analogRead(0);
        Serial.println(temper,2);  // doesn't actually do anything but is left here for reference.

  //=-=============================================================
    // If this is a remote monitor, it will probbly want to call a server page and pass data.
  // 
  
  String pageToLoad = "http://www.5and9.co.uk/test.php?ip="; // This chunk of code enables me to see it call my server
    pageToLoad += WiFi.localIP().toString();                 //  and post its ID and IP address.
    pageToLoad += "&id=";
    pageToLoad += ESP.getChipId();
    Serial.println(pageToLoad);
  http.begin(pageToLoad); 
  int httpCode = http.GET();
  delay(500);
  if(httpCode > 0) 
    {    
    // Page load success
      String payload = http.getString();
    }
    else
    {
    // handle page load failure      
    }

    // The line below tells the ESPduino where to go looking for an update. 
    // it's probably not a good idea to have it constantly hitting a server.
    // The line is left here as an example for you to edit and use at your discretion.
    //t_httpUpdate_return ret = ESPhttpUpdate.update("http://www.yourdomain/ESPprogram.bin");

    // Heartbeat flashing LED, this also acts as a yield to the uP such that the update can take place.
 for (int i = 0; i< 600; i++)
      {
      digitalWrite(LEDPIN, HIGH);   
      delay(500);  
      httpServer.handleClient();
      digitalWrite(LEDPIN, LOW);    
      httpServer.handleClient();
      delay(500);  
      }
    
    http.end();
   
   //delay(36000); // Don't be tempted to do this..  you cannot talk to the ESP in this state.
  
  
}

// The websserver code actually lives here, this reads the args and acts upon them. The relay is already here.
void handleRoot() {
   String message = "<HTML><H1>Relay Board </H1>  <br><br>";
   message += WiFi.localIP().toString();
   message += "<br>ESP";
   message += ESP.getChipId();
  message += "<br> URI: ";
  message += httpServer.uri();
  message += "<br>Method: ";
  message += ( httpServer.method() == HTTP_GET ) ? "GET" : "POST";
  message += "<br>Arguments: ";
  message += httpServer.args();
  message += "</HTML>";

  for ( uint8_t i = 0; i < httpServer.args(); i++ ) {
    message += " " + httpServer.argName ( i ) + ": " + httpServer.arg ( i ) + "\n";
  }
  if (httpServer.arg("relay") == "1")
    {
      digitalWrite(4,HIGH);
      Serial.println("Relay = on");
    }
  if (httpServer.arg("relay") == "0")
    {
      digitalWrite(4,LOW);
      Serial.println("Relay = off");
    }
  httpServer.send ( 200, "html", message );
 
}

