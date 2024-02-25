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
int keyIndex = 0;
WiFiClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char *myWriteAPIKey = SECRET_WRITE_APIKEY;
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

  float temperatureBMP = bmp.readTemperature();
  float humidity = dht.readHumidity();
  float temperatureDHT = dht.readTemperature();
  float altitude = bmp.readAltitude();

  unsigned long currentTime = millis();

  if (currentTime - lastAverageTime >= 20000) {
    float averageTemperature = totalTemperature / readingsCount;

    ThingSpeak.setField(1, averageTemperature);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, temperatureDHT);
    ThingSpeak.setField(4, altitude);

    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    
    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }

    totalTemperature = 0.0;
    readingsCount = 0;
    lastAverageTime = currentTime;
  }

  totalTemperature += temperatureBMP;
  readingsCount++;

  Serial.print("Temperatura BMP180: ");
  Serial.println(temperatureBMP);
  Serial.print("Humedad DHT11: ");
  Serial.println(humidity);
  Serial.print("Temperatura DHT11: ");
  Serial.println(temperatureDHT);
  Serial.print("Altura BMP180: ");
  Serial.println(altitude);

  delay(2000);
}
