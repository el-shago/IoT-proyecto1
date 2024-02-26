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

temperatura_puntual = [float(entry['field1']) for entry in data]  # Temperatura puntual
temperatura_promedio = [float(entry['field2']) for entry in data]  # Temperatura promedio
humedad_puntual = [float(entry['field3']) for entry in data]             # Humedad puntual
humedad_promedio = [float(entry['field4']) for entry in data]               # Humedad promedio

# Gráficas
timestamps = [datetime.strptime(entry['created_at'], "%Y-%m-%dT%H:%M:%SZ") for entry in data]

plt.figure(figsize=(10, 6))

plt.subplot(2, 1, 1)
plt.plot(timestamps, temperatura_promedio, label="Temperatura Puntual")
plt.plot(timestamps, temperatura_puntual, label="Temperatura Promedio")
plt.title('Comparativa de Temperatura')
plt.xlabel('Tiempo')
plt.ylabel('Temperatura (°C)')
plt.legend()

plt.subplot(2, 1, 2)
plt.plot(timestamps, humedad_promedio, label="Humedad Puntual")
plt.plot(timestamps, humedad_puntual, label="Humedad Promedio")
plt.title('Comparativa de Humedad')
plt.xlabel('Tiempo')
plt.ylabel('Humedad (%)')
plt.legend()

plt.tight_layout()
plt.show()
