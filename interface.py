import sys
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QLabel, QLineEdit, QPushButton
from PyQt5.QtCore import QTimer
import serial


class AppWindow(QWidget):
    def __init__(self):
        super().__init__()

        self.ser = serial.Serial('COM3', 9600, timeout=1)
        self.initUI()

    def initUI(self):
        self.setWindowTitle('PIC16F887 Interface')

        layout = QVBoxLayout()

        self.counter_label = QLabel('Counter: 0')
        layout.addWidget(self.counter_label)

        self.value_input = QLineEdit()
        layout.addWidget(self.value_input)

        self.send_button = QPushButton('Send value')
        self.send_button.clicked.connect(self.send_value)
        layout.addWidget(self.send_button)

        self.setLayout(layout)

        self.timer = QTimer()
        self.timer.timeout.connect(self.update_counter)
        self.timer.start(30)  # Actualizar cada 100 ms

    def send_value(self):
        value = int(self.value_input.text())
        if 0 <= value <= 255:
            self.ser.write(chr(value).encode())
        


    def update_counter(self):
        if self.ser.in_waiting:
            data = self.ser.readline().decode().strip()
            if data.isdigit():
                self.counter_label.setText(f'Counter: {data}')
                
if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = AppWindow()
    window.show()
    sys.exit(app.exec_())


