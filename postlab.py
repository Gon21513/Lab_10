import sys
import time
import serial
from Adafruit_IO import Client, Feed, MQTTClient

ADAFRUIT_IO_KEY = "aio_xoCe79L7XBblx8aLFJa9kej8oJgh"
ADAFRUIT_IO_USERNAME = "Gon21513"
FEED_RECEIVE = "pb-counter"
FEED_SEND = "slider-counter"

# Funciones de callback para MQTTClient
def connected(client):
    print(f"Subscribiendo al Feed {FEED_RECEIVE}")
    client.subscribe(FEED_RECEIVE)
    print("Esperando datos del feed...")

def disconnected(client):
    sys.exit(1)

def message(client, feed_id, payload):
    print(f"Feed {feed_id} recibió un nuevo valor: {payload}")

# Inicializa MQTTClient y RESTClient
mqtt_client = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

# Configura las funciones de callback
mqtt_client.on_connect = connected
mqtt_client.on_disconnect = disconnected
mqtt_client.on_message = message

# Conéctate al servidor Adafruit IO
mqtt_client.connect()

# Configurar la conexión serie
ser = serial.Serial("COM3", 9600, timeout=(1))

# Intervalo de tiempo en segundos entre envío de datos de 'slider-counter' al PIC
slider_send_interval = 1
last_slider_send_time = time.time()

while True:
    # Ejecuta el bucle del cliente MQTT para procesar las llamadas de retorno
    mqtt_client.loop()

    # Leer un byte del microcontrolador y convertirlo a un entero
    counter = ser.read(1)
    int_counter = int.from_bytes(counter, "big")

    # Enviar el valor leído del microcontrolador al feed 'pb-counter'
    if int_counter != 0:
        print(f"Enviando contador al Feed {FEED_RECEIVE}: {int_counter}")
        aio.send_data(FEED_RECEIVE, int_counter)

    # Enviar el valor de 'slider-counter' al PIC periódicamente
    if time.time() - last_slider_send_time > slider_send_interval:
        slider_data = int(aio.receive(FEED_SEND).value) 
        print(f"Enviando datos al Feed {FEED_SEND}: {slider_data}")
        slider_data_byte = bytes([slider_data])
        ser.write(slider_data_byte)
        last_slider_send_time = time.time()
