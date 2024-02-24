#include <WiFi101.h>
#include "secrets.h"
#include "ThingSpeak.h"
#include <Adafruit_BMP085.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;
WiFiClient  client;


unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

Adafruit_BMP085 bmp;

unsigned long lastAverageTime = 0;
float totalTemperature = 0.0;
int readingsCount = 0;

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

  // Lecturas de sensores
  float temperatureBMP = bmp.readTemperature();
  float humidity = dht.readHumidity();
  float temperatureDHT = dht.readTemperature();
  float altitude = bmp.readAltitude();


  // Cálculo de temperatura promedio cada 20 segundos
  unsigned long currentTime = millis();
  if (currentTime - lastAverageTime >= 20000) {
    float averageTemperature = totalTemperature / readingsCount;

    // Envío de datos a ThingSpeak
    ThingSpeak.setField(1, averageTemperature);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, temperatureDHT);
    ThingSpeak.setField(4, altitude);

    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }

    // Reiniciar variables para el siguiente ciclo de 20 segundos
    totalTemperature = 0.0;
    readingsCount = 0;
    lastAverageTime = currentTime;
  }

  // Acumular lecturas para el promedio
  totalTemperature += temperatureBMP;
  readingsCount++;

  // Imprimir valores obtenidos cada 2 segundos
  Serial.print("Temperature BMP180: ");
  Serial.println(temperatureBMP);
  Serial.print("Humidity DHT22: ");
  Serial.println(humidity);
  Serial.print("Temperature DHT22: ");
  Serial.println(temperatureDHT);
  Serial.print("Altitude BMP180: ");
  Serial.println(altitude);

  delay(2000);  // Espera 2 segundos antes de realizar la siguiente lectura y envío
}

