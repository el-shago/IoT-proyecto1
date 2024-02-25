import requests
import os
from dotenv import load_dotenv
import matplotlib.pyplot as plt
from datetime import datetime

load_dotenv()

channel_id = os.getenv('CHID','')
read_api_key = os.getenv('READKEY','')

url = f"https://api.thingspeak.com/channels/{channel_id}/feeds.json"
params = {"api_key": read_api_key}

response = requests.get(url, params=params)

data = response.json()["feeds"]

temperature_promedio_dht11 = [float(entry['field1']) for entry in data]  # Temperatura promedio DHT11
temperature_puntual_dht11 = [float(entry['field2']) for entry in data]  # Temperatura puntual DHT11
humedad_promedio = [float(entry['field3']) for entry in data]             # Humedad promedio
humedad_puntual = [float(entry['field4']) for entry in data]               # Humedad puntual

# Gráficas
timestamps = [datetime.strptime(entry['created_at'], "%Y-%m-%dT%H:%M:%SZ") for entry in data]

plt.figure(figsize=(10, 6))

plt.subplot(2, 1, 1)
plt.plot(timestamps, temperature_promedio_dht11, label="Temperatura Promedio DHT11")
plt.plot(timestamps, temperature_puntual_dht11, label="Temperatura Puntual DHT11")
plt.title('Comparativa de Temperatura DHT11')
plt.xlabel('Tiempo')
plt.ylabel('Temperatura (°C)')
plt.legend()

plt.subplot(2, 1, 2)
plt.plot(timestamps, humedad_promedio, label="Humedad Promedio")
plt.plot(timestamps, humedad_puntual, label="Humedad Puntual")
plt.title('Comparativa de Humedad')
plt.xlabel('Tiempo')
plt.ylabel('Humedad (%)')
plt.legend()

plt.tight_layout()
plt.show()
