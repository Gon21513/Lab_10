import sys
import time
import serial
from Adafruit_IO import Client, Feed, MQTTClient

ADAFRUIT_IO_KEY = "aio_xoCe79L7XBblx8aLFJa9kej8oJgh" # La llave para el servicio Adafruit IO
ADAFRUIT_IO_USERNAME = "Gon21513" # El nombre de usuario para el servicio Adafruit IO
FEED_RECEIVE = "pb-counter" # El nombre del Feed que va a recibir datos
FEED_SEND = "slider-counter" # El nombre del Feed que va a enviar datos

# Funciones de callback para MQTTClient
def connected(client):
    print(f"Subscribiendo al Feed {FEED_RECEIVE}") # Mensaje de subscripción al Feed
    client.subscribe(FEED_RECEIVE) # Subscribirse al Feed especificado
    print("Esperando datos del feed...") # Mensaje de espera de datos

def disconnected(client):
    sys.exit(1) # Salir del programa si se desconecta del servicio

def message(client, feed_id, payload):
    print(f"Feed {feed_id} recibió un nuevo valor: {payload}") # Mensaje de nuevos datos recibidos

# Inicializa MQTTClient y RESTClient
mqtt_client = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY) # Inicializar el cliente MQTT
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY) # Inicializar el cliente REST

# Configura las funciones de callback
mqtt_client.on_connect = connected # Al conectarse, llama a la función connected
mqtt_client.on_disconnect = disconnected # Al desconectarse, llama a la función disconnected
mqtt_client.on_message = message # Al recibir un mensaje, llama a la función message

# Conéctate al servidor Adafruit IO
mqtt_client.connect() # Conectar al servidor

# Configurar la conexión serie
ser = serial.Serial("COM3", 9600, timeout=(1)) # Configurar la comunicación serial

# Intervalo de tiempo en segundos entre envío de datos de 'slider-counter' al PIC
slider_send_interval = 1 
last_slider_send_time = time.time() # Marca de tiempo para el último envío de datos

while True:
    # Ejecuta el bucle del cliente MQTT para procesar las llamadas de retorno
    mqtt_client.loop()

    # Leer un byte del microcontrolador y convertirlo a un entero
    counter = ser.read(1) # Leer un byte de datos del microcontrolador
    int_counter = int.from_bytes(counter, "big") # Convertir el byte a un entero

    # Enviar el valor leído del microcontrolador al feed 'pb-counter'
    if int_counter != 0: # Si el valor leído no es cero
        print(f"Enviando contador al Feed {FEED_RECEIVE}: {int_counter}") # Mensaje de envío de datos
        aio.send_data(FEED_RECEIVE, int_counter) # Enviar los datos al Feed

    # Enviar el valor de 'slider-counter' al PIC periódicamente
    if time.time() - last_slider_send_time > slider_send_interval: # Si ha pasado el intervalo especificado
        slider_data = int(aio.receive(FEED_SEND).value) # Recibir el valor del Feed
        print(f"Enviando datos al Feed {FEED_SEND}: {slider_data}") # Mensaje de envío
        slider_data_byte = bytes([slider_data]) # Convertir el valor a un byte
        ser.write(slider_data_byte) # Enviar el byte a través de la conexión serial
        last_slider_send_time = time.time() # Actualizar la marca de tiempo para el último envío de datos
