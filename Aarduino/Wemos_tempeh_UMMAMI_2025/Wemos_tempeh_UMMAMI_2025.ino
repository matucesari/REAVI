// Librerías necesarias
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHTesp.h"
#include <MQ7.h>

// Configuración de WiFi
const char* ssid = "CESARI_2.4GHz";
const char* password = "m8tu232905";
//const char* ssid = "UMAMI";
//const char* password = "m8tu232905";
WiFiClient espClient;

// Configuración del broker MQTT
const char* mqttServer = "matucesari.servehttp.com";
const int mqttPort = 1883;
const char* mqttUser = "micesari";
const char* mqttPassword = "matu1234";
PubSubClient client(espClient);

// Variable para controlar si se monitorea
bool monitorear = false;

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

// Función para conectar al broker MQTT
void conectarMQTT() {
  while (!client.connected()) {
    Serial.println("Conectando al broker MQTT...");
    if (client.connect("WemosClient", mqttUser, mqttPassword)) {
      Serial.println("Conectado al broker MQTT");
      client.subscribe("tempeh/iniProce");
    } else {
      Serial.print("Falló conexión MQTT, estado: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// Función que se ejecuta cuando llega un mensaje MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("]: ");
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  if (String(topic) == "tempeh/iniProce") {
    if (message == "true") {
      monitorear = true;
      digitalWrite(ledPin, HIGH);  // Encender el LED
      Serial.println("Monitoreo activado");
    } else if (message == "false") {
      monitorear = false;
      digitalWrite(ledPin, LOW);  // Apagar el LED
      Serial.println("Monitoreo desactivado");
    }
  }
}

// Función para publicar datos de los sensores
void publicarDatos() {
  ds18b20.requestTemperatures(); //Se envía el comando para leer la temperatura del tempeh
  float tempDS18B20 = ds18b20.getTempCByIndex(0); //Se obtiene la temperatura en ºC del tempeh
  client.publish("tempeh/tempSonda", String(tempDS18B20,2).c_str());

  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  client.publish("tempeh/tempAmb", String(data.temperature).c_str());
  client.publish("tempeh/humAmb", String(data.humidity).c_str());

  float sensorValueA = analogRead(A0);
  //float sensorValueD = digitalRead(5);
  //float sensorVoltage = sensorValueA/1024*5.0;
  float ppmCO = mq7.getPPM();
  client.publish("tempeh/co2", String(ppmCO).c_str());

  Serial.println("--- Datos publicados ---");
  Serial.println("Temperatura Sonda: " + String(tempDS18B20) + "°C");
  Serial.println("Temperatura Ambiente: " + String(data.temperature, 2) + "°C");
  Serial.println("Humedad Ambiente: " + String(data.humidity, 1) + "%");
  Serial.println("CO2: " + String(ppmCO) + " ppm");
  Serial.println("Mq7: " + String(sensorValueA, 2) + "vol"); 
}

// Configuración inicial
void setup() {
  Serial.begin(115200);
  delay(10);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  conectarMQTT();
  pinMode(ledPin, OUTPUT);  // Configurar el pin del LED como salida
  digitalWrite(ledPin, LOW);  // Asegurarse de que el LED esté apagado inicialmente
  pinMode(DHT_PIN, INPUT);
  pinMode(5, INPUT);
  ds18b20.begin();
  dhtSensor.setup(DHT_PIN, DHTesp::DHT11);
  Serial.println("Sensores inicializados");
}

// Bucle principal
// Declaraciones globales
const unsigned long intervalo = 15 * 60 * 1000; // 15 minutos en milisegundos
unsigned long tiempoAnterior = 0;

void loop() {
unsigned long tiempoActual = millis();
    // Verificamos si han pasado 15 minutos
    if (tiempoActual - tiempoAnterior >= intervalo) {
        tiempoAnterior = tiempoActual;  // Actualizamos el tiempo
        Serial.println("Han pasado 15 minutos");        
        if (monitorear) {
            publicarDatos();
             //delay(900000);
        }
    }
    // Verificamos si la conexión MQTT está activa
        if (!client.connected()) {
            conectarMQTT();
        }
        client.loop();
}
