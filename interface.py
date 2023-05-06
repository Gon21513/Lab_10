import sys  # Importar la biblioteca sys para manejar la salida del programa
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QLabel, QLineEdit, QPushButton  # Importar elementos de la GUI
from PyQt5.QtCore import QTimer  # Importar QTimer para la actualización periódica del contador
import serial  # Importar la biblioteca serial para comunicarse con el microcontrolador

class AppWindow(QWidget):  # Definir la clase AppWindow que hereda de QWidget
    def __init__(self):
        super().__init__()  # Llamar al constructor de la clase base QWidget

        self.ser = serial.Serial('COM3', 9600, timeout=1)  # Configurar la comunicación serial
        self.initUI()  # Llamar al método para inicializar la interfaz de usuario

    def initUI(self):
        self.setWindowTitle('PIC16F887 Interface')  # Establecer el título de la ventana

        layout = QVBoxLayout()  # Crear un layout de tipo vertical

        self.counter_label = QLabel('Counter: 0')  # Crear un QLabel para mostrar el contador
        layout.addWidget(self.counter_label)  # Agregar el QLabel al layout

        self.value_input = QLineEdit()  # Crear un QLineEdit para ingresar valores
        layout.addWidget(self.value_input)  # Agregar el QLineEdit al layout

        self.send_button = QPushButton('Send value')  # Crear un QPushButton para enviar valores
        self.send_button.clicked.connect(self.send_value)  # Conectar la señal del botón a la función send_value
        layout.addWidget(self.send_button)  # Agregar el QPushButton al layout

        self.setLayout(layout)  # Establecer el layout de la ventana

        self.timer = QTimer()  # Crear un QTimer para la actualización del contador
        self.timer.timeout.connect(self.update_counter)  # Conectar la señal del timer a la función update_counter
        self.timer.start(30)  # Actualizar cada 100 ms

    def send_value(self):  # Método para enviar el valor ingresado al microcontrolador
        value = self.value_input.text()  # Obtener el texto del QLineEdit
        if value.isdigit() and 0 <= int(value) <= 255:  # Verificar si es un número válido
            value_as_byte = int(value).to_bytes(1, 'little')  # Convertir el valor a un byte
            self.ser.write(value_as_byte)  # Escribir el valor en la comunicación serial

    def update_counter(self):  # Método para actualizar el contador
        if self.ser.in_waiting:  # Verificar si hay datos en el buffer de entrada
            data = self.ser.readline().decode().strip()  # Leer y decodificar la línea de datos
            if data.isdigit():  # Verificar si el dato es un número
                self.counter_label.setText(f'Counter: {data}')  # Actualizar el texto del QLabel con el nuevo contador

if __name__ == '__main__':
    app = QApplication(sys.argv)  # Crear una instancia de QApplication
    window = AppWindow()  # Crear una instancia de AppWindow
    window.show()  # Mostrar la ventana
    sys.exit(app.exec_())  # Ejecutar el loop de eventos y cerrar el programa al finalizar

