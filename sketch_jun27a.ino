// библиотека для работы с GPRS устройством
#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>  // подключение библиотеки Wire

SoftwareSerial gprsSerial(10, 11);
// создаём объект класса GPRS и передаём в него объект Serial1
GPRS gprs(gprsSerial);
// можно указать дополнительные параметры — пины PK и ST
// по умолчанию: PK = 2, ST = 3
// GPRS gprs(Serial1, 2, 3);

int ADXL345 = 0x53; // Адрес I2C датчика ADXL345
int X_out, Y_out, Z_out;  // Выходы
char* data;
const int _size = 5;
char x[_size];
char y[_size];
char z[_size];
char result[13];
//char charBuf[_size];

void setup()
{
  // открываем последовательный порт для мониторинга действий в программе
  Serial.begin(9600);
  Wire.begin(); // Инициализация библиотеки Wire
  // Установите ADXL345 в режим измерения
  Wire.beginTransmission(ADXL345); // Начать общение с устройством
  Wire.write(0x2D); // работа с регистром  POWER_CTL  - 0x2D
  // Включить измерение
  Wire.write(8); // (8dec -> 0000 1000 двоичный) Бит D3 High для разрешения измерения
  Wire.endTransmission();
  delay(10);
  // ждём, пока не откроется монитор последовательного порта
  // для того, чтобы отследить все события в программе
  while (!Serial) {
  }
  Serial.print("Serial init OK\r\n");
  // открываем Serial-соединение с GPRS Shield
  gprsSerial.begin(9600);
    // включаем GPRS шилд
    gprs.powerOn();
    // проверяем есть ли связь с GPRS устройством
    while (!gprs.init()) {
    // если связи нет, ждём 1 секунду
    // и выводим сообщение об ошибке
    // процесс повторяется в цикле
    // пока не появится ответ от GPRS устройства
    Serial.print("GPRS Init error\r\n");
    }
    // выводим сообщение об удачной инициализации GPRS Shield
    Serial.println("GPRS init success");
    // подключение к серверу
}

void loop()
{
  // соединение с интернетом и с сервером
  ConnectToServer("internet.yota", "0.tcp.eu.ngrok.io",11483);
  for (int n = 0; n < 19; n++) {
    // считывание данных с акселерометра
    Wire.beginTransmission(ADXL345);
    Wire.write(0x32); // Начать с регистра 0x32 (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(ADXL345, 6, true); // Чтение всех 6 регистров, значение каждой оси сохраняется в 2 регистрах
    X_out = ( Wire.read() | Wire.read() << 8); // Значение по оси X
    Y_out = ( Wire.read() | Wire.read() << 8); // Значение по оси Y
    Z_out = ( Wire.read() | Wire.read() << 8); // Значение по оси Z
    // шифрование данных
    encode(X_out, x);
    encode(Y_out, y);
    encode(Z_out, z);
    // объединение в единую строку 
    int j = 0;
    for (int i = 0; i < 4; i++, j++) {
      result[j] = x[i];
    }
    for (int i = 0; i < 4; i++, j++) {
      result[j] = y[i];
    }
    for (int i = 0; i < 4; i++, j++) {
      result[j] = z[i];
    }
    result[12]='\0';
    // отправка
    gprs.send(result);
  }
}

void ConnectToServer(char* apn, char* ip, int port) {
  //соединение с apn
  while (!gprs.join(apn, "", "")) {
    Serial.print("joining error\r\n");
  }
  Serial.println("joining success");
  //подключение к tcp сокету
  while (!gprs.connect(TCP, ip, port)) {
    Serial.println("TCP connection error");
  }
  Serial.println("TCP connection success");
}

void encode(int a, char* code) {
  if (a > 0)
  {
    code[0] = 'p';
  }
  else if (a < 0)
  {
    code[0] = 'm';
    a = -a;
  }
  else
  {
    code[0] = 'v';
    code[1] = 'v';
    code[2] = 'v';
    code[3] = 'v';
    code[4] = '\0';
  }
  switch (a / 100) {
    case 1:
      code[1] = 'a';
      break;
    case 2:
      code[1] = 'b';
      break;
    case 3:
      code[1] = 'c';
      break;
    case 4:
      code[1] = 'd';
      break;
    case 5:
      code[1] = 'e';
      break;
    case 6:
      code[1] = 'f';
      break;
    case 7:
      code[1] = 'g';
      break;
    case 8:
      code[1] = 'h';
      break;
    case 9:
      code[1] = 'i';
      break;
    case 0:
      code[1] = 'v';
      break;
  }
  a = a % 100;
  switch (a / 10) {
    case 1:
      code[2] = 'a';
      break;
    case 2:
      code[2] = 'b';
      break;
    case 3:
      code[2] = 'c';
      break;
    case 4:
      code[2] = 'd';
      break;
    case 5:
      code[2] = 'e';
      break;
    case 6:
      code[2] = 'f';
      break;
    case 7:
      code[2] = 'g';
      break;
    case 8:
      code[2] = 'h';
      break;
    case 9:
      code[2] = 'i';
      break;
    case 0:
      code[2] = 'v';
      break;
  }
  a = a % 10;
  switch (a) {
    case 1:
      code[3] = 'a';
      break;
    case 2:
      code[3] = 'b';
      break;
    case 3:
      code[3] = 'c';
      break;
    case 4:
      code[3] = 'd';
      break;
    case 5:
      code[3] = 'e';
      break;
    case 6:
      code[3] = 'f';
      break;
    case 7:
      code[3] = 'g';
      break;
    case 8:
      code[3] = 'h';
      break;
    case 9:
      code[3] = 'i';
      break;
    case 0:
      code[3] = 'v';
      break;
  }
  code[4] = '\0';
}
