#include <WiFi101.h>
#include "secrets.h"
#include "ThingSpeak.h"
#include <Adafruit_BMP085.h>
#include "DHT.h"

#define DHTPIN 7
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

char ssid[] = SECRET_SSID;  
char pass[] = SECRET_PASS;  
WiFiClient client;

unsigned long myChannelNumber = SECRET_CH_ID;        
const char *myWriteAPIKey = SECRET_WRITE_APIKEY;      
Adafruit_BMP085 bmp;

unsigned long lastAverageTime = 0;
float totalTemperature = 0.0;
int readingsCount = 0;
float totalPressure = 0.0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  while (!Serial);
  
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1);
  }
  
  Serial.println(WiFi.status());
  ThingSpeak.begin(client);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    
    while (WiFi.status() != WL_CONNECTED) {
      int status = WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(10000);
    }
    
    Serial.println("\nConnected.");
  }

  float temperatureBMP = bmp.readTemperature();
  float humidity = dht.readHumidity();
  float temperatureDHT = dht.readTemperature();
  float pressure = bmp.readPressure();

  unsigned long currentTime = millis();

  if (currentTime - lastAverageTime >= 20000) {
    // Calcula las medias de temperatura y presión
    float averageTemperature = totalTemperature / readingsCount;
    float averagePressure = totalPressure / readingsCount;

    // Configura los campos para enviar a ThingSpeak
    ThingSpeak.setField(1, averageTemperature);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, temperatureDHT);
    ThingSpeak.setField(4, temperatureBMP);
    ThingSpeak.setField(5, pressure);
    ThingSpeak.setField(6, averagePressure);

    // Envía los datos a ThingSpeak
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }

    // Reinicia variables para el próximo ciclo
    totalPressure = 0.0;
    totalTemperature = 0.0;
    readingsCount = 0;
    lastAverageTime = currentTime;
  }

  // Acumula los valores para calcular medias
  totalPressure += pressure;
  totalTemperature += temperatureBMP;
  readingsCount++;

  // Imprime valores obtenidos cada 2 segundos
  Serial.print("Temperatura BMP180: ");
  Serial.println(temperatureBMP);
  Serial.print("Humedad DHT11: ");
  Serial.println(humidity);
  Serial.print("Temperatura DHT11: ");
  Serial.println(temperatureDHT);
  Serial.print("Presion BMP180: ");
  Serial.println(pressure);

  delay(2000);
}
