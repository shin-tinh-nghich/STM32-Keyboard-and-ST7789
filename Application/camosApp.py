import time
import requests
import sys, os
import serial
import serial.tools.list_ports

import psutil
import geocoder

from PyQt5 import QtWidgets, QtGui
from PyQt5.QtWidgets import QMessageBox
from PyQt5.QtCore import QThread, QMutex
from PyQt5.QtWidgets import QApplication, QMainWindow, QSystemTrayIcon, QMenu, QAction
from myQtGuiGen import Ui_Form

import keycode

serialMutex = QMutex()

def get_weather(api_key, location):
   base_url = "http://api.openweathermap.org/data/2.5/weather"
   params = {"q": location, "appid": api_key}

   response = requests.get(base_url, params=params)
   weather_data = response.json()

   return weather_data

class print_System_Thread(QThread):
    def __init__(self, _ser):
        super().__init__()
        self.ser = _ser

    def run(self):
        while self.ser.isOpen():
            cpu = psutil.cpu_percent(interval=1)
            _ram = psutil.virtual_memory()
            ram = _ram.percent
            sysCR = "sysI "+ str(cpu) + '%-' + str(ram) + '%-\n'
            print(sysCR)
            serialMutex.lock()
            self.ser.write(bytearray(sysCR.encode())) 
            serialMutex.unlock()
            self.msleep(3000)

class print_Time_Thread(QThread):
    def __init__(self, _ser):
        super().__init__()
        self.ser = _ser

    def run(self):
        while self.ser.isOpen():
            named_tuple = time.localtime() # lấy struct_time
            second_value = named_tuple.tm_sec
            if second_value == 0:
                last_time = time.strftime("time %H:%M\n", named_tuple)
                last_date = time.strftime("date %a %d/%m/%y\n", named_tuple)
                print(last_time)
                print(last_date)
                serialMutex.lock()
                self.ser.write(bytearray(last_time.encode()))
                serialMutex.unlock()
                self.msleep(1000)
                serialMutex.lock()
                self.ser.write(bytearray(last_date.encode()))
                serialMutex.unlock()

class print_Weather_Thread(QThread):
    def __init__(self, _ser):
        super().__init__()
        self.ser = _ser

    def run(self):
        while self.ser.isOpen():
            # Nhập API key của bạn từ OpenWeatherMap
            api_key = "ccc0d8d484692dbb2f36d0d16c8b4619"
            # Lấy vị trí hiện tại của người dùng
            location = geocoder.ip("me").city
            #print(location)
            # Lấy thông tin thời tiết
            weather_data = get_weather(api_key, location)
            _temp = weather_data['main']['temp']
            temp = int(_temp) - 273
            humi = weather_data['main']['humidity']
            # In thông tin thời tiết
            weather_now = "weth "+ location + '-' + str(temp) + '°C-' + str(humi) + '%-\n'
            print (weather_now)
            serialMutex.lock()
            self.ser.write(bytearray(weather_now.encode()))
            serialMutex.unlock()
            self.msleep(1000*60 + 500)

class SystemTrayApp(QApplication):
    def __init__(self, sys_argv):
        super(SystemTrayApp, self).__init__(sys_argv)

        self.main_window = Camos_App()
        self.tray_icon = QSystemTrayIcon(self)
        self.tray_icon.setIcon(QtGui.QIcon(os.path.join(basedir, 'Kirby.ico')))

        menu = QMenu()
        
        show_action = QAction('Show', self)
        show_action.triggered.connect(self.show_window)
        menu.addAction(show_action)

        exit_action = QAction('Exit', self)
        exit_action.triggered.connect(self.exit_app)
        menu.addAction(exit_action)

        self.tray_icon.setContextMenu(menu)
        self.tray_icon.activated.connect(self.tray_icon_activated)
        self.tray_icon.show()

        self.aboutToQuit.connect(self.exit_app)

    def show_window(self):
        self.main_window.showNormal()
        self.main_window.activateWindow()

    def exit_app(self):
        self.tray_icon.hide()
        self.main_window.close()
        self.quit()

    def tray_icon_activated(self, reason):
        if reason == QSystemTrayIcon.DoubleClick:
            self.show_window()

bootkey_code = keycode.bootkey_code
consumer_code = keycode.consumer_code
character_code = keycode.character_code
special_characters_code = keycode.special_characters_code
old_brightness = 0

class Camos_App(QMainWindow, QtWidgets.QWidget, Ui_Form):
    def __init__(self):
        super(Camos_App, self).__init__()
        self.setupUi(self)

        self.init()
        self.setWindowTitle("camos Keyboard Tool")
        self.ser = serial.Serial()
        self.port_check()

    def init(self):
        # Thiết lập chiều rộng tối thiểu cho danh sách
        self.s1__box_6.view().setMinimumWidth(130)
        # Scan button
        self.s1__box_1.clicked.connect(self.port_check)
        # Imformation of serial lable
        self.s1__box_2.currentTextChanged.connect(self.port_imf)
        # Connect button
        self.open_button.clicked.connect(self.port_open)
        # Disconnect button
        self.close_button.clicked.connect(self.port_close)
        # Send data button
        self.backlight_button.clicked.connect(self.change_backlight)
        self.bg_button.clicked.connect(self.change_background)
        self.key_button.clicked.connect(self.change_keyboard)
        # Change brightness
        self.brightnesslSlider.valueChanged.connect(self.change_brightness)
        # Kết nối sự kiện currentIndexChanged với hàm xử lý
        self.s1__box_6.currentIndexChanged.connect(self.key_list)
        # Init thread
        self.system_Thread = None
        self.time_Thread = None
        self.weather_Thread = None

    """ def changeEvent(self, event):
        # Ghi đè phương thức changeEvent để bắt sự kiện thay đổi trạng thái của cửa sổ
        if event.type() == QEvent.WindowStateChange and self.isMinimized():
            self.hide() """

    def closeEvent(self, event):
        event.ignore()
        self.hide()
    
    # Serial check list
    def port_check(self):
        # init serial list
        self.Com_Dict = {}
        port_list = list(serial.tools.list_ports.comports())
        self.s1__box_2.clear()
        for port in port_list:
            self.Com_Dict["%s" % port[0]] = "%s" % port[1]
            self.s1__box_2.addItem(port[0])
        if len(self.Com_Dict) == 0:
            self.state_label.setText(" No serial port")

    # Imformation of serial port
    def port_imf(self):
        # Display information of serial port
        imf_s = self.s1__box_2.currentText()
        if imf_s != "":
            self.state_label.setText(self.Com_Dict[self.s1__box_2.currentText()])

    # Connect serial port
    def port_open(self):
        self.ser.port = self.s1__box_2.currentText()
        self.ser.baudrate = int(self.s1__box_3.currentText())

        try:
            self.ser.open()
        except:
            QMessageBox.critical(self, "Port Error", "Cannot be open port")
            return None
        
        if self.ser.isOpen():
            self.open_button.setEnabled(False)
            self.close_button.setEnabled(True)
            self.formGroupBox.setTitle("Serial port opened")

            self.time_Thread = print_Time_Thread(self.ser)
            self.time_Thread.start()
            time.sleep(2)
            self.system_Thread = print_System_Thread(self.ser)
            self.system_Thread.start()
            time.sleep(1)
            self.weather_Thread = print_Weather_Thread(self.ser)
            self.weather_Thread.start()

    # Disonnect serial port
    def port_close(self):
        try:
            self.ser.close()
            
        except:
            pass
        self.open_button.setEnabled(True)
        self.close_button.setEnabled(False)
        self.formGroupBox.setTitle("Serial port closed")

    # Send data
    def data_send(self):
        if self.ser.isOpen():
            input_s = self.s3__send_text.toPlainText()
            if input_s != "":
                # If empty string
                input_s = (input_s + '\r\n').encode('utf-8')
        else:
            pass

    # change_backlight
    def change_backlight(self):
        if self.ser.isOpen():
            mode = self.s1__box_4.currentText()
            if mode == "Green": i_mode = 1
            elif mode == "Red": i_mode = 2
            elif mode == "Blue": i_mode = 3
            elif mode == "Rainbows 1": i_mode = 4
            elif mode == "Rainbows 2": i_mode = 5
            elif mode == "OFF": i_mode = 0
            modeL = "modL "+ str(i_mode) + '\n'
            serialMutex.lock()
            self.ser.write(bytearray(modeL.encode()))
            serialMutex.unlock()
            print(modeL)
        else:
            pass
    
    # change_background
    def change_background(self):
        if self.ser.isOpen():
            #Developing
            pass
        else:
            pass

    # change_keyboard
    def change_keyboard(self):
        if self.ser.isOpen():
            numberKey = self.s1__box_5.currentText()
            _function = self.s1__box_6.currentText()
            if _function == 'Character Key' or  _function == 'Boot Key':
                function = '0'
                _key1 = self.comboBox.currentText()
                key1 = getattr(keycode, _key1)
                key2 = 0
                key3 = 0
            elif _function == 'Special Key':
                function = '0'
                _key1 = self.comboBox.currentText()
                key1 = ord(_key1)
                key2 = 0
                key3 = 0
            elif _function == 'Consumer Key':
                function = '1'
                _key1 = self.comboBox.currentText()
                key1 = getattr(keycode, _key1)
                key2 = 0
                key3 = 0
            elif _function == 'Keystrokes':
                _key = self.textEdit.toPlainText()
                __key = _key.split("+")
                if len(__key) == 2:
                    function = '2'
                    key1 = getattr(keycode, __key[0])
                    key2 = getattr(keycode, __key[1])
                    key3 = 0
                elif len(__key) == 3:
                    function = '3'
                    key1 = getattr(keycode, __key[0])
                    key2 = getattr(keycode, __key[1])
                    key3 = getattr(keycode, __key[2])
            else:
                pass
                
            editKey = "edit "+ str(numberKey) + '-' + function + '-' + str(key1) + '-' + str(key2) + '-' + str(key3) + '-\n'
            serialMutex.lock()
            self.ser.write(bytearray(editKey.encode()))
            serialMutex.unlock()
            print(editKey)
        else:
            pass
        
    # init key list
    def key_list(self, index):
        if self.ser.isOpen():
            # init keycode list
            function_key = self.s1__box_6.itemText(index)
            if function_key == 'Character Key':
                self.comboBox.clear()
                self.textEdit.setEnabled(False)
                self.textEdit.clear()
                self.textEdit.setFontPointSize(11)
                self.textEdit.insertPlainText('Custom keystrokes')
                for code in character_code:
                    self.comboBox.addItem(code)
                self.comboBox.view().setMinimumWidth(200)
            elif function_key == 'Boot Key':
                self.comboBox.clear()
                self.textEdit.setEnabled(False)
                self.textEdit.clear()
                self.textEdit.setFontPointSize(11)
                self.textEdit.insertPlainText('Custom keystrokes')
                for code in bootkey_code:
                    self.comboBox.addItem(code)
                self.comboBox.view().setMinimumWidth(200)
            elif function_key == 'Special Key':
                self.comboBox.clear()
                self.textEdit.setEnabled(False)
                self.textEdit.clear()
                self.textEdit.setFontPointSize(11)
                self.textEdit.insertPlainText('Custom keystrokes')
                for code in special_characters_code:
                    self.comboBox.addItem(code)
                self.comboBox.view().setMinimumWidth(200)
            elif function_key == 'Consumer Key':
                self.comboBox.clear()
                self.textEdit.setEnabled(False)
                self.textEdit.clear()
                self.textEdit.setFontPointSize(11)
                self.textEdit.insertPlainText('Custom keystrokes')
                for code in consumer_code:
                    self.comboBox.addItem(code)
                self.comboBox.view().setMinimumWidth(200)
            elif function_key == 'Keystrokes':
                self.comboBox.clear()
                self.textEdit.setEnabled(True)
                self.textEdit.clear()
                self.textEdit.setFontPointSize(11)
                self.textEdit.insertPlainText('KEY_A+KEY_B')
        else:
            pass

    def change_brightness(self, val):
        if self.ser.isOpen():
            global old_brightness
            if val > old_brightness:
                old_brightness = val
                i_mode = 7
                modeL = "modL "+ str(i_mode) + '\n'
                serialMutex.lock()
                self.ser.write(bytearray(modeL.encode()))
                serialMutex.unlock()
                print(modeL)
            elif val < old_brightness:
                old_brightness = val
                i_mode = 6
                modeL = "modL "+ str(i_mode) + '\n'
                serialMutex.lock()
                self.ser.write(bytearray(modeL.encode()))
                serialMutex.unlock()
                print(modeL)
            else:
                pass
        else:
            pass    
    
    # Receive data
    def data_receive(self):
        try:
            num = self.ser.inWaiting()
        except:
            self.port_close()
            return None
        if num > 0:
            data = self.ser.read(num)
            num = len(data)

            # Converte string from binary to unicode
            self.s2__receive_text.insertPlainText(data.decode('iso-8859-1'))
            # Get text cursor
            textCursor = self.s2__receive_text.textCursor()
            # Scroll cursor to bottom
            textCursor.movePosition(textCursor.End)
            # Set text cursor
            self.s2__receive_text.setTextCursor(textCursor)
        else:
            pass

basedir = os.path.dirname(__file__)

try:
    from ctypes import windll  # Only exists on Windows.
    myappid = 'mycompany.myproduct.subproduct.version com.camos'
    windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)
except ImportError:
    pass

if __name__ == "__main__":
    app = SystemTrayApp(sys.argv)
    #app = QtWidgets.QApplication(sys.argv)
    app.setWindowIcon(QtGui.QIcon(os.path.join(basedir, 'Kirby.ico')))
    myshow = Camos_App()
    app.main_window = myshow
    myshow.show()
    sys.exit(app.exec_())