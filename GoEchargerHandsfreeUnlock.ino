#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>


const int plugInWaitTime= 300; // time in seconds the charger stays unlocked waiting to be plugged into the car

// Set these to your desired credentials for the unlock hotspot.
const char *assid = "Go-E-Charger-Access";
const char *apassword = "01234567";

// add your go e charger credentials from the card here
const char *goessid = "go-e-012345";
const char *goepassword = "1a2s3d4f56";

ESP8266WiFiMulti WiFiMulti;
StaticJsonDocument<10000> doc;
int state = 5;
int timeremaining = 0;

void setup() {
delay(3000);
  Serial.begin(115200);
  Serial.println();
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFiMulti.cleanAPlist();
  delay(5000);
  WiFiMulti.addAP(goessid, goepassword);
  state=5; // start by disabling the charger, as we set it to enabled by default for portability
  delay(5000);
  Serial.print("d1 is up.");
}

void loop() {
    if(state==0)
    {
        Serial.println("Configuring access point...");
        /* You can remove the password parameter if you want the AP to be open. */
        WiFi.softAP(assid, apassword);
        delay(1000);
        IPAddress myIP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(myIP);
        state++;
    }
    else if(state== 1)
    {
      if(WiFi.softAPgetStationNum()>0)
      {
        Serial.print("connected client");
        delay(2000);
        WiFi.softAPdisconnect(true);
        state++;
      }
      delay(100);
    }
    else if(state == 2)
    {
      delay(1000);
      WiFiMulti.addAP(goessid, goepassword);
      state++;
    }
    else if(state == 3)
    {
      // wait for WiFi connection

      if((WiFiMulti.run() == WL_CONNECTED)) 
      {
          HTTPClient http;
          IPAddress ip = WiFi.gatewayIP();
          char ipstr[50];
          sprintf(ipstr, "%d.%d.%d.%d",ip[0], ip[1], ip[2], ip[3]);


          char url[100];

          sprintf(url, "http://%s/mqtt?payload=alw=1", ipstr);
          getHttp(url);




          timeremaining = plugInWaitTime;
          state++;
      }
      delay(1000);
    }
    else if(state==4)
    {
      int result = getCarState();
      if(result!= -1)
      {
        if(result!=1) // if the car is not plugged into the charger
        {
          if(timeremaining>0) // wait for a total of 
          {
            timeremaining=timeremaining-10;
          }
          else
          {
            state++;
          }
        }
      }
      delay(10000);
    }
    else if(state==5) //disable charging, disconnect charger wifi and restart ap.
    {
      if((WiFiMulti.run() == WL_CONNECTED)) {
        HTTPClient http;
        IPAddress ip = WiFi.gatewayIP();
        char ipstr[50];
        sprintf(ipstr, "%d.%d.%d.%d",ip[0], ip[1], ip[2], ip[3]);
        char url[100];
        sprintf(url, "http://%s/mqtt?payload=alw=0", ipstr); //disable charger
        getHttp(url);
        delay(1000);
        WiFiMulti.cleanAPlist();
        WiFi.disconnect();
        state = 0;
      }
      delay(1000);
    }
}
//returns 1 if the car is connected to the charger (whether charging or not does not matter)
//returns 0 if the car isn't connected to the charger
//returns -1 if there was an error getting the info from the charger
int getCarState()
{
  HTTPClient http;
  IPAddress ip = WiFi.gatewayIP();
  char ipstr[50];
  sprintf(ipstr, "%d.%d.%d.%d",ip[0], ip[1], ip[2], ip[3]);
  char url[100];
  sprintf(url, "http://%s/status", ipstr);
  doc.clear();
  Serial.print("[HTTP] begin...\n");
  Serial.print(url);
  http.begin(url); //HTTP

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          // Deserialize the JSON document
          DeserializationError error = deserializeJson(doc, payload);
        
          // Test if parsing succeeds.
          if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            return -1;
          }
          Serial.println(payload);
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      return -1;
  }

  http.end();
  if(doc["car"]=="4" || doc["car"]=="2")
  {
    doc.clear();
    Serial.println("car connected");
    return 1;
  }
  doc.clear();
  Serial.println("car not connected");
  return 0;
}

//used to send a get request without processing the return value -> trigger rest events
void getHttp(char* url)
{
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  Serial.print(url);
  http.begin(url); //HTTP

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}
