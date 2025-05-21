#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Пины для Slave Select (SS) и Reset (RST)
#define SS_1_PIN   5   // RC522 №1 (SS)
#define RST_1_PIN  22  // RC522 №1 (RST)
#define SS_2_PIN   16  // RC522 №2 (SS)
#define RST_2_PIN  17  // RC522 №2 (RST)

#define GREEN_LED_PIN 2   // GPIO2 для зеленого светодиода
#define RED_LED_PIN 4     // GPIO4 для красного светодиода
#define BUZZER_PIN 15     // GPIO15 для зуммера
#define DOOR_SENSOR_PIN 13  

const char* ssid = "RT-GPON-9FB0";
const char* password = "piNSj7pbYs";
const char* serverUrl = "http://192.168.0.4:8000/api/access";
const char* doorUrl = "http://192.168.0.4:8000/api/door";

// Создаем два экземпляра RC522 с Reset
MFRC522 mfrc522_1(SS_1_PIN, RST_1_PIN);
MFRC522 mfrc522_2(SS_2_PIN, RST_2_PIN);

// Задаем UID ключей для каждого датчика
byte key1[] = {0xD1, 0xA6, 0xBB, 0x89}; // UID для первого датчика
byte key2[] = {0x6F, 0x79, 0x63, 0xDE}; // UID для второго датчика

// Переменные для работы с дверью
bool lastDoorState = false;
unsigned long doorAccessTime = 0;
const unsigned long DOOR_OPEN_TIME = 5000; // 5 секунд открытого состояния
int lastAccessedReader = 0; // 0 - нет доступа, 1 - дверь 1, 2 - дверь 2

void testServerConnection() {
  HTTPClient http;
  http.begin("http://192.168.0.4:8000");
  int httpCode = http.GET();
  Serial.printf("Тест подключения: %d\n", httpCode);
  http.end();
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  
  mfrc522_1.PCD_Init();
  mfrc522_2.PCD_Init();

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP);
  
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nПодключено к WiFi");
  testServerConnection();

  Serial.println("Два RFID RC522 с Reset готовы к работе!");
  Serial.println("MC-38 датчик инициализирован");
}

String getUIDString(byte *uid, byte size) {
  String uidStr = "";
  for (byte i = 0; i < size; i++) {
    if (uid[i] < 0x10) uidStr += "0";
    uidStr += String(uid[i], HEX);
  }
  return uidStr;
}

bool compareUID(byte *uid1, byte *uid2, byte size) {
  for (byte i = 0; i < size; i++) {
    if (uid1[i] != uid2[i]) {
      return false;
    }
  }
  return true;
}

void sendAccessData(int readerNum, bool accessGranted, String uid) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Нет подключения к WiFi");
    return;
  }

  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc(256);
  doc["reader"] = readerNum;
  doc["status"] = accessGranted ? "Access Complete" : "Access Denied";
  doc["uid"] = uid;

  String requestBody;
  serializeJson(doc, requestBody);

  int httpCode = http.POST(requestBody);
  
  if (httpCode > 0) {
    Serial.printf("Ответ сервера: %d\n", httpCode);
  } else {
    Serial.printf("Ошибка: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();
}

void sendDoorStatus(bool isOpen, int readerNum = 0) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Ошибка: нет подключения к WiFi");
    return;
  }

  HTTPClient http;
  http.begin(doorUrl);
  http.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc(128);
  doc["status"] = isOpen ? "Open" : "Closed";
  doc["reader"] = readerNum;

  String requestBody;
  serializeJson(doc, requestBody);

  // Отправляем PUT запрос вместо POST
  int httpCode = http.PUT(requestBody);
  
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_NO_CONTENT) {
      Serial.printf("PUT: Статус двери успешно обновлен. Код: %d, Считыватель: %d\n", httpCode, readerNum);
    } else {
      Serial.printf("PUT: Сервер вернул неожиданный код: %d\n", httpCode);
      String payload = http.getString();
      Serial.println("Ответ сервера: " + payload);
    }
  } else {
    Serial.printf("PUT: Ошибка отправки статуса двери: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();
}

void playSuccessTone() {
  tone(BUZZER_PIN, 1000, 200);
  delay(200);
  tone(BUZZER_PIN, 1500, 200);
}

void playErrorTone() {
  tone(BUZZER_PIN, 300, 500);
  delay(600);
  tone(BUZZER_PIN, 200, 300);
}

void controlAccess(bool accessGranted, int readerNum) {
  if (accessGranted) {
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
    playSuccessTone();
    delay(2600);
    digitalWrite(GREEN_LED_PIN, LOW);
    
    // Автоматически "открываем" дверь при успешном доступе
    doorAccessTime = millis();
    lastAccessedReader = readerNum; // Запоминаем, к какой двери был доступ
    Serial.print("Дверь открыта по доступу к считывателю ");
    Serial.println(readerNum);
  } else {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    playErrorTone();
    delay(2100);
    digitalWrite(RED_LED_PIN, LOW);
  }
}

bool checkDoorState() {
  // Если дверь была открыта по доступу, эмулируем открытое состояние
  if (millis() - doorAccessTime < DOOR_OPEN_TIME) {
    return true;
  }
  
  // Иначе читаем реальный датчик
  return digitalRead(DOOR_SENSOR_PIN) == HIGH;
}

void processReader(MFRC522 &reader, byte *key, int readerNum) {
  if (!reader.PICC_IsNewCardPresent() || !reader.PICC_ReadCardSerial()) {
    return;
  }

  String uid = getUIDString(reader.uid.uidByte, reader.uid.size);
  Serial.print("RC522 №");
  Serial.print(readerNum);
  Serial.print(": UID=");
  Serial.println(uid);

  bool accessGranted = compareUID(reader.uid.uidByte, key, reader.uid.size);
  
  if (accessGranted) {
    Serial.println("Access Complete");
  } else {
    Serial.println("Access Denied");
  }

  controlAccess(accessGranted, readerNum);
  sendAccessData(readerNum, accessGranted, uid);
  reader.PICC_HaltA();
}

void monitorDoor() {
  bool currentDoorState = checkDoorState();
  
  if (currentDoorState != lastDoorState) {
    lastDoorState = currentDoorState;
    Serial.print("Состояние двери: ");
    Serial.println(currentDoorState ? "ОТКРЫТА" : "ЗАКРЫТА");
    
    // Всегда отправляем последний активный считыватель, даже при закрытии
    sendDoorStatus(currentDoorState, lastAccessedReader);
    
    // Визуальная индикация
    if (currentDoorState) {
      digitalWrite(RED_LED_PIN, HIGH);
    } else {
      digitalWrite(RED_LED_PIN, LOW);
      // Не сбрасываем lastAccessedReader здесь, чтобы знать, кто последний открывал
    }
  }
}

void loop() {
  processReader(mfrc522_1, key1, 1);
  processReader(mfrc522_2, key2, 2);
  monitorDoor();
  delay(200);
}