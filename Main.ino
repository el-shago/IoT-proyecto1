// #include <WiFi101.h>
// #include "secrets.h"
// #include "ThingSpeak.h"
// #include <Adafruit_BMP085.h>
// #include "DHT.h"

// #define DHTPIN 2
// #define DHTTYPE DHT22
// DHT dht(DHTPIN, DHTTYPE);

// char ssid[] = SECRET_SSID;   // your network SSID (name) 
// char pass[] = SECRET_PASS;   // your network password
// int keyIndex = 0;            // your network key Index number (needed only for WEP)
// WiFiClient  client;

// unsigned long myChannelNumber = SECRET_CH_ID;
// const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// // Initialize our values
// int number1 = 0;
// int number2 = random(0,100);
// int number3 = random(0,100);
// int number4 = random(0,100);
// String myStatus = "";

// Adafruit_BMP085 bmp;

// void setup() {
//   Serial.begin(115200);
//   dht.begin();

//   while (!Serial) {
//     ;
//   }

//   if (!bmp.begin()) {
//     Serial.println("Could not find a valid BMP085 sensor, check wiring!");
//     while (1) {}
//   }

//   ThingSpeak.begin(client);
// }


// void loop(){
// // Connect or reconnect to WiFi
//   if(WiFi.status() != WL_CONNECTED){
//     Serial.print("Attempting to connect to SSID: ");
//     Serial.println(SECRET_SSID);
//     while(WiFi.status() != WL_CONNECTED){
//       WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
//       Serial.print(".");    
//     } 
//     Serial.println("\nConnected.");
//   }

//   // set the fields with the values
//   ThingSpeak.setField(1, number1);
//   ThingSpeak.setField(2, number2);
//   ThingSpeak.setField(3, number3);
//   ThingSpeak.setField(4, number4);

//   // figure out the status message
//   if(number1 > number2){
//     myStatus = String("field1 is greater than field2"); 
//   }
//   else if(number1 < number2){
//     myStatus = String("field1 is less than field2");
//   }
//   else{
//     myStatus = String("field1 equals field2");
//   }
  
//   // set the status
//   ThingSpeak.setStatus(myStatus);
  
//   // write to the ThingSpeak channel 
//   int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
//   if(x == 200){
//     Serial.println("Channel update successful.");
//   }
//   else{
//     Serial.println("Problem updating channel. HTTP error code " + String(x));
//   }
  
//   // change the values
//   number1++;
//   if(number1 > 99){
//     number1 = 0;
//   }
//   number2 = random(0,100);
//   number3 = random(0,100);
//   number4 = random(0,100);
  
 

//   Serial.print("Temperature = ");
//     Serial.print(bmp.readTemperature());
//     Serial.println(" *C");
    
//     Serial.print("Pressure = ");
//     Serial.print(bmp.readPressure());
//     Serial.println(" Pa");
    
//     // Calculate altitude assuming 'standard' barometric
//     // pressure of 1013.25 millibar = 101325 Pascal
//     Serial.print("Altitude = ");
//     Serial.print(bmp.readAltitude());
//     Serial.println(" meters");

//     Serial.print("Pressure at sealevel (calculated) = ");
//     Serial.print(bmp.readSealevelPressure());
//     Serial.println(" Pa");

//   // you can get a more precise measurement of altitude
//   // if you know the current sea level pressure which will
//   // vary with weather and such. If it is 1015 millibars
//   // that is equal to 101500 Pascals.
//     Serial.print("Real altitude = ");
//     Serial.print(bmp.readAltitude(101500));
//     Serial.println(" meters");
    
//     Serial.println();
    

//   // Reading temperature or humidity takes about 250 milliseconds!
//   // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
//   float h = dht.readHumidity();
//   // Read temperature as Celsius (the default)
//   float t = dht.readTemperature();
//   // Read temperature as Fahrenheit (isFahrenheit = true)
//   float f = dht.readTemperature(true);

//   // Check if any reads failed and exit early (to try again).
//   if (isnan(h) || isnan(t) || isnan(f)) {
//     Serial.println(F("Failed to read from DHT sensor!"));
//     return;
//   }

//   // Compute heat index in Fahrenheit (the default)
//   float hif = dht.computeHeatIndex(f, h);
//   // Compute heat index in Celsius (isFahreheit = false)
//   float hic = dht.computeHeatIndex(t, h, false);

//   Serial.print(F("Humidity: "));
//   Serial.print(h);
//   Serial.print(F("%  Temperature: "));
//   Serial.print(t);
//   Serial.print(F("°C "));
//   Serial.print(f);
//   Serial.print(F("°F  Heat index: "));
//   Serial.print(hic);
//   Serial.print(F("°C "));
//   Serial.print(hif);
//   Serial.println(F("°F"));


//    delay(20000);
// }

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
      WiFi.begin(ssid, pass);
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

