
// ESPduino boiler plate.
// by G0LFP
// Has no actual functionality, but enables a wifi update client.
// Load this into a new ESPDuino so that all you have to do is apply power.
// Connect your phone to select WiFi access point.
// Then you can upload your program 'OTA'

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
#define LEDPIN 16

void setup() 
{
  // Setup code goes here, runs once
  Serial.begin(115200);
  pinMode(LEDPIN,OUTPUT);


   //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing - maybe addd a config pin tto.
  //wifiManager.resetSettings();

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
  Serial.println(ver,HEX);
  Serial.print("Local IP address:");
  Serial.println(WiFi.localIP());
  Serial.println("ESP Boilerplate v1.0");
  

  
  // This chunk enables the update bin server.
    MDNS.begin(host);
    httpUpdater.setup(&httpServer);
    httpServer.begin();
    MDNS.addService("http", "tcp", 80);
    //Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);

    httpServer.on ( "/", handleRoot );
    httpServer.on ( "/pagename1.html", pagename1 );

}

void loop(void)
{
 
  httpServer.handleClient();
    
  HTTPClient http;

  //==============================================================

/*

    // If you really don't need access to the webserver, 
    // you can just put the system to sleep and consume very little power until the next time.

    // This method will need to do its own updating. Use the commented out code below to look for updated versions.
    // You will need to take care of version control by using an appropriate filename.
   
    //======================================================
    
   
        delay(3596000); //wait an hour until next 


  //=-=============================================================
  
  */ 
  
  

  // If this is a remote monitor, it will probably want to call a server page and pass data.
  // 

  int yourData = 1; //Arbitary value.
  
  String pageToLoad = "http://yourDomain/yourpage.php?data=";
    pageToLoad += yourData;
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
    // t_httpUpdate_return ret = ESPhttpUpdate.update("http://www.yourdomain/ESPprogram.bin");

    // Heartbeat flashing LED, this also acts as a yield to the uP such that the update can take place.
    // Initially set to do nothing but listen for 30 seconds.
    
 for (int i = 0; i< 30; i++)
      {
      digitalWrite(LEDPIN, HIGH);   
      delay(500);  
      httpServer.handleClient();
      digitalWrite(LEDPIN, LOW);    
      httpServer.handleClient();
      delay(500);  
      }
    
    http.end();
}

// END of program

// Start of Functions. (and web pages that we might need)


void handleRoot() {
   String message = "<HTML><head><meta http-equiv=\"REFRESH\" content=\"10\"></head><H1>Root web page</H1>  <br><br>";
   message += "</head><body>";
   
   String addt = WiFi.localIP().toString();
   message += addt;
  message += "<br> URI: ";
  message += httpServer.uri();
  message += "<br>Method: ";
  message += ( httpServer.method() == HTTP_GET ) ? "GET" : "POST";
  message += "<br>Arguments: ";
  message += httpServer.args();
  message += "</body></HTML>";

  for ( uint8_t i = 0; i < httpServer.args(); i++ ) {
    message += " " + httpServer.argName ( i ) + ": " + httpServer.arg ( i ) + "\n";
  }

  httpServer.send ( 200, "html", message );
 
}

void pagename1() {
   String message = "<HTML><head><meta http-equiv=\"REFRESH\" content=\"10\"></head><H1>Pagename1</H1>  <br><br>";
   message += "</head><body>";
   
   String addt = WiFi.localIP().toString();
   message += addt;
  message += "<br> URI: ";
  message += httpServer.uri();
  message += "<br>Method: ";
  message += ( httpServer.method() == HTTP_GET ) ? "GET" : "POST";
  message += "<br>Arguments: ";
  message += httpServer.args();
  message += "</body></HTML>";

  for ( uint8_t i = 0; i < httpServer.args(); i++ ) {
    message += " " + httpServer.argName ( i ) + ": " + httpServer.arg ( i ) + "\n";
  }

  httpServer.send ( 200, "html", message );
 
}
