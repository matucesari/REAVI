//Red Wifi
#include <ESP8266WiFi.h>
WiFiClient cliente; //Cliente web  
const char* ssid = "CESARI_2.4GHz";
const char* password = "m8tu232905";
//const char* ssid = "UMAMI";
//const char* password = "m8tu232905";
//const char* ssid = "CGA2121_CESARI";
//const char* password = "m8tu232905";

//Servidor de Matlab
#include <ThingSpeak.h>
unsigned long myChannelNumber = 1326670;  // Canal Medir Tempeh
const char* myWriteAPIKey = "NYHB5ZQQDCX8GE0E"; // reemplace con API KEY del canal thingspeak 
const char* server = "api.thingspeak.com";


//Sensor temperatura sonda DS18B20
#include <OneWire.h>                
#include <DallasTemperature.h>
OneWire ourWire1(3);    //Se establece el pin D4 como bus OneWire
DallasTemperature sensorsCORTO(&ourWire1); //Se declara una variable u objeto para nuestro sensor CORTO

//Sensor de humedad y temperatura ambiente DTH11
#include "DHTesp.h"
const int DHT_PIN = 2; // Puerto Digital del Arduino 2 corresponde D4
DHTesp dhtSensor;

//sensor dioxido carbono mq7
//#include <MQ7.h>
//MQ7 mq7(A0, 5.0);  //variable inicial del sensor mq7


void setup()
{
    Serial.begin(115200);
    delay(10); 
    pinMode (3, INPUT);
    pinMode(2, INPUT);
    //pinMode(5, INPUT);  //digital mq7

    //Se inicia los sensores
    dhtSensor.setup(DHT_PIN, DHTesp::DHT22);    
    sensorsCORTO.begin(); 
    
    setup_wifi(); //Conectar al Wifi  
    
    ThingSpeak.begin(cliente);  // Initialize ThingSpeak
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{         
    // temperatura y humedad ambiente
    TempAndHumidity  data = dhtSensor.getTempAndHumidity();    
    Serial.println("---");
    Serial.println("Temperatura Ambiente: " + String(data.temperature, 1) + "°C");
    Serial.println("Humedad Ambiente: " + String(data.humidity, 1) + "%");         
    Serial.println("---");
    
    // Medir la sonda temperatura bolsa de hongo
    sensorsCORTO.requestTemperatures();  //Se envía el comando para leer la temperatura TEMPEH
    float t_tempeh= sensorsCORTO.getTempCByIndex(0); //Se obtiene la temperatura en ºC de tempeh 
    Serial.println("Temperatura Sonda: " + String(t_tempeh, 2) + "°C");
    Serial.println("---");
    
    //mq-7
    //float sensorValueA = analogRead(A0);
    //float sensorValueD = digitalRead(5);
    //float ppm = mq7.getPPM();
    //Serial.println("Mq7: " + String(ppm, 2) + "ppm");
   // Serial.println("Mq7: " + String(sensorValueA, 2) + "vol");   
   // Serial.println("---");
      
    // set the fields with the values
    ThingSpeak.setField(1, t_tempeh);
    ThingSpeak.setField(2, data.temperature);
    ThingSpeak.setField(3, data.humidity); 
    //ThingSpeak.setField(4, ppm);   
        
    if (cliente.connect(server,80)) {
        // write to the ThingSpeak channel
        int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
        if(x == 200)
        {
           Serial.println("Channel update successful.");
        }
        else
        {
          Serial.println("Problem updating channel. HTTP error code " + String(x));
        }                
    }
    cliente.stop(); 
       
    Serial.println("espere unos minutos");
    // thingspeak needs at least a 15 sec delay between updates    
   // delay(20000);  //   20 seconds to be safe
   delay(900000);   //son 15 minutos, 900 segundos
}
