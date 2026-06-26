#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <math.h>

//para temperatura
#include <OneWire.h>                
#include <DallasTemperature.h>
OneWire ourWire1(7);                //Se establece el pin 7  como bus OneWire
DallasTemperature sensorsCORTO(&ourWire1); //Se declara una variable u objeto para nuestro sensor CORTO

//para huemdad
const int Valor_Sensor_Aire = 591; // Valor calculado con el programa de calibración con el sensor al aire
const int Valor_Sensor_Agua = 250; // Valor calculado con el programa de calibración con el sensor sumergido en agua
int valor_sensor = 0; // Variable que almacena el valor de salida del sensor de humedad capacitivo
int porcentaje = 0; // Variable que almacena el porcentaje de humedad relativa del terreno
 
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Inicia el LCD en la dirección 0x27, con 16 caracteres y 2 líneas
 
void setup()
{
   Serial.begin(9600); // Comienzo de la comunicación con el monitor serie del IDE de Arduino
   lcd.begin();     // Inicializar el LCD       
   lcd.backlight();
   lcd.setCursor(0, 0);  // Ubicamos el cursor en la primera posición(columna:0) de la PRIMER línea(fila:0)
   lcd.print("Linea 1");
   lcd.setCursor(0, 1); // Ubicamos el cursor en la primera posición(columna:0) de la SEGUNDA línea(fila:1)
   lcd.print("Linea 2");
   delay(2500);
   lcd.clear();
   //Se inicia los sensor
   sensorsCORTO.begin(); 
}
 
void loop()
{
  //MEDIR HUMEDAD
    valor_sensor = analogRead(0);   // Leemos el valor de la salida del sensor de humedad capacitivo, conectada al pin analógico "A0"
    porcentaje = map(valor_sensor, Valor_Sensor_Agua, Valor_Sensor_Aire, 100, 0);   // Se calcula el porcentaje de humedad relativa teniendo en cuenta los dos límites
    if(porcentaje < 0) porcentaje = 0; // Evita porcentajes negativos en la medida del sensor
    if(porcentaje > 100) porcentaje = 100; // Evita porcentajes negativos en la medida del sensor
   // Se presenta el porcentaje a través del monitor serie
    Serial.print("HUMEDAD: ");
    Serial.print(valor_sensor);
    Serial.print(" , ");
    Serial.print(porcentaje);
    Serial.println("% HR");

  //MEDIR TEMPERATURA Sensor de Temperatura contra agua NTC 10K
   sensorsCORTO.requestTemperatures();  //Se envía el comando para leer la temperatura
   float Temperatura= sensorsCORTO.getTempCByIndex(0); //Se obtiene la temperatura en ºC 
   Serial.print("TEMPERATURA: ");
   Serial.print(Temperatura); //temperatura 
   Serial.println(" C");
 
  //MOSTRAR EN DISPLAY LCD
   //temperatura
   lcd.setCursor(0, 0);  // Ubicamos el cursor en la primera posición(columna:0) de la PRIMER línea(fila:0)   
   lcd.print("Temp: ");
   lcd.print(Temperatura,2); //1 decimal
   lcd.print(" C"); 
   //humedad
   lcd.setCursor(0, 1); // Ubicamos el cursor en la primera posición(columna:0) de la SEGUNDA línea(fila:1)
   lcd.print("Hume: ");
   lcd.print(porcentaje,1); //1 decimal
   lcd.print(" %");
   delay(1000);
}
