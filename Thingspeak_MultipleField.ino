
#include "ThingSpeak.h"
#include "secrets.h"
#include <DHT.h>
#include <ESP8266WiFi.h>

#define DHTPIN 2          //DHT11 is connected to GPIO Pin 2

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)

//float humi;
//float temp;

WiFiClient  client;
DHT dht(DHTPIN, DHT11);

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
unsigned int LedFieldNumber = 3; 
void setup() {
  pinMode(0, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(0, LOW);   // Turn the LED on (Note that LOW is the voltage level
  Serial.begin(115200);  // Initialize serial
  dht.begin();
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  int statusCode = 0;
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("\r\n\r\nAttempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\r\nConnected.");
  }

  // set the fields with the values
  ThingSpeak.setField(1, dht.readTemperature());
  ThingSpeak.setField(2, dht.readHumidity());

  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
 // Read from the Thingspeak channel for LED status
 int LED_STATUS = ThingSpeak.readLongField(SECRET_CH_ID, LedFieldNumber, SECRET_READ_APIKEY);  

   // Check the status of the read operation to see if it was successful
  statusCode = ThingSpeak.getLastReadStatus();
  if(statusCode == 200){
    Serial.println("LED: " + String(LED_STATUS));
    if(LED_STATUS)
    {
       digitalWrite(0, HIGH);   // Turn the LED ON
    }else  digitalWrite(0, LOW);   // Turn the LED OFF
  }
  else{
    Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
  }
  delay(15000); // Wait 20 seconds to update the channel again
}
