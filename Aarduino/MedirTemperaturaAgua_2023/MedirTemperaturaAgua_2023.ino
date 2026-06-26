#include <ArduinoWiFiServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ThingSpeak.h>
#include "ThingSpeak.h" 
#include <WiFiManager.h>

#include <OneWire.h>                
#include <DallasTemperature.h>
OneWire ourWire1(3);                //Se establece el pin D0 RX  como bus OneWire
DallasTemperature sensorsCORTO(&ourWire1); //Se declara una variable u objeto para nuestro sensor CORTO

// replace with your channel’s thingspeak API key and your SSID and password
const char * myWriteAPIKey = "A4WERPKG32G186VZ"; //Write API Key
unsigned long myChannelNumber = 2028405;  //Channel ID
const char* server = "api.thingspeak.com";

WiFiClient cliente;

void setup()
{
    Serial.begin(115200);
    delay(10);    
    //para envio de datos a Excel
    Serial.println("CLEARDATA"); // Limpia los datos en Excell
    // Envia ETIQUETAS columnas de datos a Excell
    Serial.println("LABEL,HORA,TEMPERATURA");

    pinMode (3, INPUT);
    sensorsCORTO.begin(); 
  
	 // Creamos una instancia de la clase WiFiManager
       WiFiManager wifiManager;
   // Descomentar para resetear configuración
	 // wifiManager.resetSettings();
 
	// Cremos AP y portal cautivo
   wifiManager.autoConnect();
   Serial.println("Ya estás conectado");
    
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
     
 //sensores de temperatura digital 
    sensorsCORTO.requestTemperatures();  //Se envía el comando para leer la temperatura TEMPEH
    float t_agua= sensorsCORTO.getTempCByIndex(0); //Se obtiene la temperatura en ºC de tempeh 
                
    // set the fields with the values
    ThingSpeak.setField(1, t_agua);
        
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
      Serial.print(t_agua); //temperatura agua
      Serial.println();
    
    Serial.println("espere 1 minuto");
    // thingspeak needs at least a 15 sec delay between updates
    //   20 seconds to be safe
  delay(20000);
 //delay(900000);   //son 15 minutos, 900 segundos
}
