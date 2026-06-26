#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include "ThingSpeak.h" 

#include <OneWire.h>                
#include <DallasTemperature.h>
OneWire ourWire1(5);                //Se establece el pin D1 como bus OneWire
DallasTemperature sensorsCORTO(&ourWire1); //Se declara una variable u objeto para nuestro sensor CORTO

//senspr humedad tempeh
const int Valor_Sensor_Aire = 776; // Valor calculado con el programa de calibración con el sensor al aire
const int Valor_Sensor_Agua = 312; // Valor calculado con el programa de calibración con el sensor sumergido en agua
int humedad = 0; // Variable que almacena el valor de salida del sensor de humedad capacitivo
int h_solido = 0; // Variable que almacena el porcentaje de humedad relativa


// replace with your channel’s thingspeak API key and your SSID and password
const char * myWriteAPIKey = "YJBYF0TXWROQPUU3"; 
unsigned long myChannelNumber = 1341723;  
//const char* ssid = "CESARI_2.4GHz";
const char* ssid = "Umami";
const char* password = "m8tu232905";
const char* server = "api.thingspeak.com";

DHT dht(2, DHT21);

WiFiClient cliente;

void setup()
{
    Serial.begin(115200);
    delay(10);    
    //para envio de datos a Excel
    Serial.println("CLEARDATA"); // Limpia los datos en Excell
    // Envia ETIQUETAS columnas de datos a Excell
    Serial.println("LABEL,HORA,TEMPERATURA producto,HUMEDAD producto,TEMPERATURA AMBIENTE,HUMEDAD AMBIENTE");

    pinMode (2, INPUT);
    pinMode (3, INPUT);
    pinMode (5, INPUT);
    dht.begin();
    sensorsCORTO.begin(); 
   
    WiFi.begin(ssid, password);
    
    Serial.println();
    Serial.println();
    Serial.print("Conectando con  ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
  ThingSpeak.begin(cliente);  // Initialize ThingSpeak 
}

void loop()
{    
    // Leemos el valor de la salida analógica del sensor capacitivo, conectada al pin analógico "A0"
    humedad = analogRead(A0);  
   // Se calcula el porcentaje de humedad relativa teniendo en cuenta los dos límites
    h_solido = map(humedad, Valor_Sensor_Agua, Valor_Sensor_Aire, 100, 0);
    if(h_solido < 0) h_solido = 0; // Evita porcentajes negativos en la medida del sensor
    if(h_solido > 100) h_solido = 100; // Evita porcentajes negativos en la medida del sensor

    //temperatura y humedad ambiente
    float h_ambiente = dht.readHumidity();
    float t_ambiente = dht.readTemperature();
    
 //sensores de temperatura digital 
    sensorsCORTO.requestTemperatures();  //Se envía el comando para leer la temperatura TEMPEH
    float t_solido= sensorsCORTO.getTempCByIndex(0); //Se obtiene la temperatura en ºC de tempeh 

             
    // set the fields with the values
    ThingSpeak.setField(1, t_solido);
    ThingSpeak.setField(2, h_solido);
    ThingSpeak.setField(3, t_ambiente);
    ThingSpeak.setField(4, h_ambiente);    
      
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
    
  //envío de datos
      Serial.print("DATA,TIME,"); //comando en excel
      Serial.print(t_solido); //temperatura producto
      Serial.print(", ");
      Serial.print(h_solido); //humedad producto en porcentaje
      Serial.print(", ");
      Serial.print(t_ambiente); //temperatura ambiente
      Serial.print(", ");
      Serial.print(h_ambiente); //humedad ambiente
      Serial.println();
    
    Serial.println("espere 15 minutos");
    //delay(20000);  //   20 seconds to be safe
   delay(900000);   //son 15 minutos, 900 segundos
}
