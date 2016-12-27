#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>

Adafruit_SSD1306 display(4,5,2,0,14);

const char* SSID     = "IOTCourse";           // insert your SSID
const char* password = "";   // insert your password
const char* location;
const char* weather;
double humidity;
double temp;

WiFiClient client;

const char* server = "api.openweathermap.org";  // server's address
const char* resource = "/data/2.5/weather?id=1668341&units=metric&APPID=7f9abc6839ab446bf8d5894bdae77685"; // insert your API key

char response[600]; // this fixed sized buffers works well for this project using the NodeMCU.

void setup(){
    Serial.begin(115200);
    // put your setup code here, to run once:
    WiFi.begin(SSID, password); 
    
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
   
   
    Serial.println(WiFi.localIP());  

    display.begin(SSD1306_SWITCHCAPVCC,0x3D);
    
    display.clearDisplay();
    
    display.setTextWrap(false);

}
    

void show(){
   display.clearDisplay();
   display.setTextColor(WHITE);
   display.setTextSize(1);
   display.setCursor(0,0);
   display.print("Location:");
   display.setCursor(64,0);
   display.print(location);
   display.setCursor(0,10);
   display.print("Temp:");
   display.setCursor(64,10);
   display.print(temp);
   display.setCursor(0,20);
   display.print("Humidity:");
   display.setCursor(64,20);
   display.print(humidity);
   display.display();
}

void loop(){
    
        // connect to server  
        bool ok = client.connect(server, 80);
        bool beginFound = false;
        
        while(!ok){
            //Serial.print(".");
            delay(500);        
        }
    
        delay(500);
    
        //Send request to resource
        client.print("GET ");
        client.print(resource);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(server);
        client.println("Connection: close");
        client.println();
    
        delay(100);
    
        //Reading stream and remove headers
        client.setTimeout(10000);
   
        bool ok_header = ok_header = client.find("\r\n\r\n");
    
        while(!ok_header){
        // wait
        }
         
        client.readBytes(response, 800);
        
        // uncomment line below to see data received for debugging purposes or just fun    
        // Serial.println(response); 
        
        // process JSON
        DynamicJsonBuffer jsonBuffer;
        
        // But.....make sure the stream header is valid
        // Sometime OWM includes invalid data after the header
        // Parsing fails if this data is not removed
        
        if(int(response[0]) != 123){
            Serial.println("Wrong start char detected");
        int i = 0;
        while(!beginFound){
            if(int(response[i]) == 123){ // check for the "{" 
            beginFound = true;
            Serial.print("{ found at ");
            Serial.println(i);
            }
            i++;
        }

            int eol = sizeof(response);
            Serial.print("Length = ");
            Serial.println(eol);
  
    
            //restructure by shifting the correct data
            Serial.println("restructure");
            for(int c=0; c<(eol-i); c++){
                response[c] = response[((c+i)-1)];
                Serial.print(response[c]);
            }
     
            Serial.println("Done...");

                
            }
        
        
        JsonObject& root = jsonBuffer.parseObject(response);
        
        if (!root.success()) {
        Serial.println("JSON parsing failed!");
        } 
        else {
        //Serial.println("JSON parsing worked!");
        }
        
        location = root["name"]; 
        weather = root["weather"][0]["description"];
        humidity = root["main"]["humidity"];
        temp = root["main"]["temp"];
        
        // Print data to Serial
        Serial.print("*** ");
        Serial.print(location);
        Serial.println(" ***");
        Serial.print("humidity: ");
        Serial.println(humidity);
        Serial.print("Temp ");
        Serial.print(temp);
        Serial.println("C");
        Serial.println("----------"); 

        show();
        display.clearDisplay();
        
     
        client.stop(); // disconnect from server
    
        for(int x=0; x<10; x++){ // wait for new connection with progress indicator
            Serial.print(".");
            delay(99999); // the OWM free plan API does NOT allow more then 60 calls per minute
        }
        
        Serial.println("");
     
    }
