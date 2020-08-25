#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//Server-Pre-Setup
ESP8266WebServer server(80);
void handlePulse();
void handleColour();
void handle404();

void setup() {
  //WiFi-Setup
  Serial.begin(38400);
  WiFi.begin("SSID", "PASSWORD");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  //Server-Setup
  server.on("/pulse", HTTP_POST, handlePulse);
  server.on("/colour", HTTP_POST, handleColour);
  server.on("/effect", HTTP_POST, handleEffect);
  server.onNotFound(handle404);
  
  server.begin();

  //LED-Setup
  #define REDPIN 4
  #define GREENPIN 5
  #define BLUEPIN 0

  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

  //Turn off internal LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void pulse(int cycle){
  analogWrite(REDPIN, cycle);
  analogWrite(GREENPIN, cycle);
  analogWrite(BLUEPIN, cycle);
}

void handlePulse(){
  if (!server.hasArg("pulse") || server.arg("pulse") == NULL){
    server.send(400, "text/plain", "400: Invalid Request");
    return;
  }
  pulse(server.arg("pulse").toInt());
  server.send(200);
}

void setColour(int r, int g, int b){
  analogWrite(REDPIN, r);
  analogWrite(GREENPIN, g);
  analogWrite(BLUEPIN, b);
}

void handleColour(){
  if (!server.hasArg("r") || !server.hasArg("g") || !server.hasArg("b") ||
      server.arg("r") == NULL || server.arg("g") == NULL || server.arg("b") == NULL){
        server.send(400, "text/plain", "400: Invalid Request");
        return;
      }
  setColour(
    server.arg("r").toInt(),
    server.arg("g").toInt(),
    server.arg("b").toInt()
  );
  server.send(200);
}

void handle404(){
  server.send(404, "text/plain", "404: Not found");
}

//effect-controll vars
//TODO: clean up whole effects section
bool blueFade=false;
bool colourFade=false;

void handleEffect(){
  if (!server.hasArg("effect") || server.arg("effect") == NULL){
    server.send(400, "text/plain", "400: Invalid Request");
    return;
  }
  int selEffect=(server.arg("effect")).toInt();
  switch(selEffect){
    case 0:
      blueFade=false;
      colourFade=false;
      delay(100);
      setColour(0,0,0);
      server.send(200);
      Serial.println("eff off");
      break;
    case 1:
      colourFade=false;
      blueFade=true;
      server.send(200);
      Serial.println("eff blue fade");
      effBlueFade();
      break;
    case 2:
      blueFade=false;
      colourFade=true;
      server.send(200);
      Serial.println("eff colour fade");
      effColourSwirl();
      break;
  }
}

void effBlueFade(){
  //could use blueFade instead of true
  while (blueFade){
    for (int i=0; i<257; i++){
      if(!blueFade){
        setColour(0,0,0);
        break;
      }
      setColour(0,0,i);
      server.handleClient();
      delay(10);
    }
    for (int i=255; i>(-1); i--){
      if(!blueFade){
        setColour(0,0,0);
        break;
      }
      setColour(0,0,i);
      server.handleClient();
      delay(10);
    }
  }
}

void effColourSwirl(){
  int r,g,b;
  int fs=10;
  while (colourFade){
      for (r = 0; r < 256; r++) { 
        analogWrite(REDPIN, r);
        server.handleClient();
        if(!colourFade){
          setColour(0,0,0);
          break;
        }
        delay(fs);
      } 
      for (b = 255; b > 0; b--) { 
        analogWrite(BLUEPIN, b);
        server.handleClient();
        if(!colourFade){
          setColour(0,0,0);
          break;
        }
        delay(fs);
      } 
      for (g = 0; g < 256; g++) { 
        analogWrite(GREENPIN, g);
        server.handleClient();
        if(!colourFade){
          setColour(0,0,0);
          break;
        }
        delay(fs);
      } 
      for (r = 255; r > 0; r--) { 
        analogWrite(REDPIN, r);
        server.handleClient();
        if(!colourFade){
          setColour(0,0,0);
          break;
        }
        delay(fs);
      } 
      for (b = 0; b < 256; b++) { 
        analogWrite(BLUEPIN, b);
        server.handleClient();
        if(!colourFade){
          setColour(0,0,0);
          break;
        }
        delay(fs);
      } 
      for (g = 255; g > 0; g--) { 
        analogWrite(GREENPIN, g);
        server.handleClient();
        if(!colourFade){
          setColour(0,0,0);
          break;
        }
        delay(fs);
      }
  }
}

void loop(){
  server.handleClient();
}
