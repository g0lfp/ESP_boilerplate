
// ESPduino boiler plate.
// by G0LFP
// Has no actual functionality, but enables a wifi update client.
// Load this into a new ESPDuino so that all you have to do is apply power.
// Connect your phone to select WiFi access point.
// Then you can upload your program 'OTA'


#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino


//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
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

void setup() 
{
  // Setup code goes here, runs once
  Serial.begin(115200);

   //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
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
  Serial.println(ver,DEC);
  Serial.print("Local IP address:");
  Serial.println(WiFi.localIP());
  

  
  // This chunk enables the update bin server.
    MDNS.begin(host);
    httpUpdater.setup(&httpServer);
    httpServer.begin();
    MDNS.addService("http", "tcp", 80);
    //Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
}

void loop(void)
{

  
 
     httpServer.handleClient();
    
  HTTPClient http;

  String pageToLoad = "http://serverName/pageToLoad.php";
    
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
    delay(5000);  // this also acts as a yield to the uP such that the update can take place.
    }
    
    http.end();
   
  
  
  
}