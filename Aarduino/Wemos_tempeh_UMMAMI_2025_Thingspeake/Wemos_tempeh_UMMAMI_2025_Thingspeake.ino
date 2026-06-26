// Librerías necesarias
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHTesp.h"
#include <MQ7.h>

// Configuración de WiFi
const char* ssid = "Umami";
//const char* ssid = "CESARI_2.4GHz";
const char* password = "m8tu232905";

WiFiClient espClient;

//Servidor de Matlab
#include <ThingSpeak.h>
unsigned long myChannelNumber = 1326670;  // Canal Medir Tempeh
const char* myWriteAPIKey = "NYHB5ZQQDCX8GE0E"; // reemplace con API KEY del canal thingspeak 
const char* server = "api.thingspeak.com";

// Definir el pin del LED
const int ledPin = 13;  // Usaremos el pin D7	GPIO13

// Pines y configuración de sensores
OneWire ourWire(4);                // DS18B20 D2	GPIO4
DallasTemperature ds18b20(&ourWire);

const int DHT_PIN = 2; // DHT11   D4	GPIO2
DHTesp dhtSensor;

MQ7 mq7(A0, 5.0); // MQ7 A0

// Función para conectar al WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

// Configuración inicial
void setup() {
  Serial.begin(115200);
  delay(10);
  
  pinMode(ledPin, OUTPUT);  // Configurar el pin del LED como salida
  digitalWrite(ledPin, LOW);  // Asegurarse de que el LED esté apagado inicialmente
  pinMode(DHT_PIN, INPUT);
  pinMode(5, INPUT);
  ds18b20.begin();
  dhtSensor.setup(DHT_PIN, DHTesp::DHT11);
  Serial.println("Sensores inicializados");

  setup_wifi();
  ThingSpeak.begin(espClient);  // Initialize ThingSpeak
}

// Bucle principal
void loop() {
  // temperatura y humedad ambiente
    TempAndHumidity  data = dhtSensor.getTempAndHumidity();    
    Serial.println("---");
    Serial.println("Temperatura Ambiente: " + String(data.temperature, 1) + "°C");
    Serial.println("Humedad Ambiente: " + String(data.humidity, 1) + "%");         
    Serial.println("---");
    
    // Medir la sonda temperatura bolsa de hongo
    ds18b20.requestTemperatures();  //Se envía el comando para leer la temperatura TEMPEH
    float t_tempeh= ds18b20.getTempCByIndex(0); //Se obtiene la temperatura en ºC de tempeh 
    Serial.println("Temperatura Sonda: " + String(t_tempeh, 2) + "°C");
    Serial.println("---");
    
    //mq-7
    float sensorValueA = analogRead(A0);
    float sensorValueD = digitalRead(5);
    float ppm = mq7.getPPM();
    float vol = sensorValueA;
    Serial.println("Mq7: " + String(ppm, 2) + "ppm");
    Serial.println("Mq7: " + String(sensorValueA, 2) + "vol");   
    Serial.println("---");
      
    // set the fields with the values
    ThingSpeak.setField(1, t_tempeh);
    ThingSpeak.setField(2, data.temperature);
    ThingSpeak.setField(3, data.humidity); 
    ThingSpeak.setField(4, ppm);   
    ThingSpeak.setField(5, vol);
        
    if (espClient.connect(server,80)) {
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
    espClient.stop(); 
       
    Serial.println("espere unos minutos");
    // thingspeak needs at least a 15 sec delay between updates    
   //delay(20000);  //   20 seconds to be safe
   delay(900000);   //son 15 minutos, 900 segundos
}
