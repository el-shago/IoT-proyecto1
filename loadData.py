import os
from dotenv import load_dotenv
import requests
import matplotlib.pyplot as plt
from datetime import datetime

load_dotenv()

channel_id = os.getenv('CHID','')  
read_api_key = os.getenv('READKEY','')

url = f"https://api.thingspeak.com/channels/{channel_id}/feeds.json"
params = {"api_key": read_api_key}

response = requests.get(url, params=params)

data = response.json()["feeds"]
timestamps = [datetime.strptime(entry['created_at'], "%Y-%m-%dT%H:%M:%SZ") for entry in data]
average_temperature_bmp = [float(entry['field1']) for entry in data]  # Campo 1: Temperatura promedio BMP180 en Arduino
humidity = [float(entry['field2']) for entry in data]      # Campo 2: Humedad puntual
temperature_dht11 = [float(entry['field3']) for entry in data]      # Campo 3: Temperatura DHT11
temperature_bmp180 = [float(entry['field4']) for entry in data]     # Campo 4: Temperatura BMP180
pressure = [float(entry['field5']) if entry['field5'] is not None else 0.0 for entry in data]  # Campo 5: Presión puntual BMP180
average_pressure = [float(entry['field6']) if entry['field6'] is not None else 0.0 for entry in data]  # Campo 6: Presión promedio BMP180 en Arduino
average_humidity = [float(entry['field7']) if entry['field7'] is not None else 0.0 for entry in data]  # Campo 7: Humedad promedio en Arduino
average_temperature_dht = [float(entry['field8']) if entry['field8'] is not None else 0.0 for entry in data]  # Campo 8: Temperatura promedio DHT11 en Arduino

# Crear gráficas
plt.figure(figsize=(10, 8))

plt.subplot(3, 1, 1)
plt.plot(timestamps, average_temperature_bmp, label="Temp. Promedio BMP180")
plt.plot(timestamps, average_temperature_dht, label="Temp. Promedio DHT11")
plt.plot(timestamps, temperature_dht11, label="Temperatura DHT11")
plt.plot(timestamps, temperature_bmp180, label="Temperatura BMP180")
plt.title('Comparativa de Temperatura')
plt.xlabel('Tiempo')
plt.ylabel('Temperatura (°C)')
plt.legend()

plt.subplot(3, 1, 2)
plt.plot(timestamps, humidity, label="Humedad Puntual")
plt.plot(timestamps, average_humidity, label="Humedad Promedio")
plt.title('Humedad')
plt.xlabel('Tiempo')
plt.ylabel('Humedad (%)')
plt.legend()

plt.subplot(3, 1, 3)
plt.plot(timestamps, average_pressure, label="Presión Promedio BMP180")
plt.plot(timestamps, pressure, label="Presión Puntual")
plt.title('Comparativa de Presión')
plt.xlabel('Tiempo')
plt.ylabel('Presión (Pa)')
plt.legend()

plt.tight_layout()
plt.show()
