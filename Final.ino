#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>
#define ledPin1   12 // Điều khiển LED qua chân GPIO 12 trên ESP32
#define ledPin2   14
#define ledPin3   27
#define ledPin4   26
#define analogPin 5
int lightValue;
// WiFi
const char *ssid = "iot"; // Nhập tên WiFi của bạn
const char *password = "123456789";  // Nhập mật khẩu WiFi của bạn

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *ledTopic = "led";
const char *sensorTopic = "sensor";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const char *led1ScheduleStartTopic = "ds1";
const char *led1ScheduleEndTopic = "de1";
const char *led2ScheduleStartTopic = "ds2";
const char *led2ScheduleEndTopic = "de2";
const char *led3ScheduleStartTopic = "ds3";
const char *led3ScheduleEndTopic = "de3";
const char *led4ScheduleStartTopic = "ds4";
const char *led4ScheduleEndTopic = "de4";
const int mqtt_port = 1883;
float hum;
float temp;
int c;
int led1OnHourStart;
int led1OnMinuteStart;
int led1OnHourEnd;
int led1OnMinuteEnd;
int led2OnHourStart;
int led2OnMinuteStart;
int led2OnHourEnd;
int led2OnMinuteEnd;
int led3OnHourStart;
int led3OnMinuteStart;
int led3OnHourEnd;
int led3OnMinuteEnd;
int led4OnHourStart;
int led4OnMinuteStart;
int led4OnHourEnd;
int led4OnMinuteEnd;
// Cảm biến DHT11
#define DHTPIN 4  
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
bool autoControl1 = true; 
bool manualControl1 = false;
bool autoControl2 = true; 
bool manualControl2 = false;
bool autoControl3 = true; 
bool manualControl3 = false;
bool autoControl4 = true; 
bool manualControl4 = false;
bool ledState1 = false;
bool ledState2 = false;
bool ledState3 = false;
bool ledState4 = false;

WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(0x27, 16, 2);
void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    String message;
    for (int i = 0; i < length; i++) {
        message += (char) payload[i];  // Convert *byte to string
    }
     time_t now = time(nullptr);
  int currentHour = hour(now);
  int currentMinute = minute(now);
c=hour(now)+7;
    Serial.print(message);
    if (strcmp(topic, ledTopic) == 0) {
    if (message == "on1" && !ledState1) {
        digitalWrite(ledPin1, HIGH);  // Turn on the LED
        ledState1 = true;
        manualControl1 = true;
        autoControl1 = false;
    } else if (message == "off1" && ledState1) {
        digitalWrite(ledPin1, LOW); // Turn off the LED
        ledState1 = false;
        manualControl1 = true;
        autoControl1 = false;
    } else if (message == "on2") {
        digitalWrite(ledPin2, HIGH);
        ledState2 = true;
        manualControl2 = true;
        autoControl2 = false;
    } else if (message == "off2") {
        digitalWrite(ledPin2, LOW);
        ledState2 = false;
        manualControl2 = true;
        autoControl2 = false;
    }  else if (message == "on3" && !ledState3) {
        digitalWrite(ledPin3, HIGH);  // Turn on the LED
        ledState3 = true;
        manualControl3 = true;
        autoControl3 = false;
    } else if (message == "off3" && ledState3) {
        digitalWrite(ledPin3, LOW); // Turn off the LED
        ledState3 = false;
        manualControl3 = true;
        autoControl3 = false;
    } else if (message == "on4" && !ledState4) {
        digitalWrite(ledPin4, HIGH);  // Turn on the LED
        ledState4 = true;
        manualControl4 = true;
        autoControl4 = false;
    } else if (message == "off4" && ledState4) {
        digitalWrite(ledPin4, LOW); // Turn off the LED
        ledState4 = false;
        manualControl4 = true;
        autoControl4 = false;
    }
    }
     if (strcmp(topic, led1ScheduleStartTopic) == 0) {
    // Định dạng tin nhắn từ MQTT là "hh:mm"
    led1OnHourStart = message.substring(0, 2).toInt();
    led1OnMinuteStart = message.substring(3, 5).toInt();
    Serial.println("Received LED1 start schedule from MQTT: " + message);
  } else if (strcmp(topic, led1ScheduleEndTopic) == 0) {
    // Định dạng tin nhắn từ MQTT là "hh:mm"
    led1OnHourEnd = message.substring(0, 2).toInt();
    led1OnMinuteEnd = message.substring(3, 5).toInt();
    Serial.println("Received LED1 end schedule from MQTT: " + message);
  }
  if (strcmp(topic, led2ScheduleStartTopic) == 0) {
    // Định dạng tin nhắn từ MQTT là "hh:mm"
    led2OnHourStart = message.substring(0, 2).toInt();
    led2OnMinuteStart = message.substring(3, 5).toInt();
    
    Serial.println("Received LED2 start schedule from MQTT: " + message);
  } else if (strcmp(topic, led2ScheduleEndTopic) == 0) {
    // Định dạng tin nhắn từ MQTT là "hh:mm"
    led2OnHourEnd = message.substring(0, 2).toInt();
    led2OnMinuteEnd = message.substring(3, 5).toInt();
    Serial.println("Received LED2 end schedule from MQTT: " + message);
  }
  if (strcmp(topic, led3ScheduleStartTopic) == 0) {
    // Định dạng tin nhắn từ MQTT là "hh:mm"
    led3OnHourStart = message.substring(0, 2).toInt();
    led3OnMinuteStart = message.substring(3, 5).toInt();
    Serial.println("Received LED3 start schedule from MQTT: " + message);
  } else if (strcmp(topic, led3ScheduleEndTopic) == 0) {
    // Định dạng tin nhắn từ MQTT là "hh:mm"
    led3OnHourEnd = message.substring(0, 2).toInt();
    led3OnMinuteEnd = message.substring(3, 5).toInt();
    Serial.println("Received LED3 end schedule from MQTT: " + message);
  }
  if (strcmp(topic, led4ScheduleStartTopic) == 0) {
    // Định dạng tin nhắn từ MQTT là "hh:mm"
    led4OnHourStart = message.substring(0, 2).toInt();
    led4OnMinuteStart = message.substring(3, 5).toInt();
    Serial.println("Received LED4 start schedule from MQTT: " + message);
  } else if (strcmp(topic, led4ScheduleEndTopic) == 0) {
    // Định dạng tin nhắn từ MQTT là "hh:mm"
    led4OnHourEnd = message.substring(0, 2).toInt();
    led4OnMinuteEnd = message.substring(3, 5).toInt();
    Serial.println("Received LED4 end schedule from MQTT: " + message);
  }
   if (c > led1OnHourStart ||
      (c == led1OnHourStart && currentMinute >= led1OnMinuteStart)) {
    if (c < led1OnHourEnd ||
        (c == led1OnHourEnd && currentMinute < led1OnMinuteEnd)){
        if(message == "off1"){
          led1OnHourStart = 0;
        led1OnMinuteStart = 0;
        led1OnHourEnd = 0;
        led1OnMinuteEnd = 0;
        }}}
  
if (c > led1OnHourStart ||
      (c == led1OnHourStart && currentMinute >= led1OnMinuteStart)) {
    if (c < led1OnHourEnd ||
        (c == led1OnHourEnd && currentMinute < led1OnMinuteEnd)){
        manualControl1 = false;
        autoControl1 = true;
        }
    else{ 
          
        manualControl1 = true;
        autoControl1 = false;
        }
}else {
        manualControl1 = true;
        autoControl1 = false;
        }
if(c == led1OnHourEnd && currentMinute == led1OnMinuteEnd){
        digitalWrite(ledPin1, LOW);
        ledState1 = false;
        manualControl1 = true;
        autoControl1 = false;
  
        }
  if (c > led2OnHourStart ||
      (c == led2OnHourStart && currentMinute >= led2OnMinuteStart)) {
    if (c < led2OnHourEnd ||
        (c == led2OnHourEnd && currentMinute < led2OnMinuteEnd)){
        if(message == "off2"){
          led2OnHourStart = 0;
        led2OnMinuteStart = 0;
        led2OnHourEnd = 0;
        led2OnMinuteEnd = 0;
        }}}
  
if (c > led2OnHourStart ||
      (c == led2OnHourStart && currentMinute >= led2OnMinuteStart)) {
    if (c < led2OnHourEnd ||
        (c == led2OnHourEnd && currentMinute < led2OnMinuteEnd)){
        manualControl2 = false;
        autoControl2 = true;
        }
    else{ 
          
        manualControl2 = true;
        autoControl2 = false;
        }
}else {
        manualControl2 = true;
        autoControl2 = false;
        }
if(c == led2OnHourEnd && currentMinute == led2OnMinuteEnd){
        digitalWrite(ledPin2, LOW);
        ledState2 = false;
        manualControl2 = true;
        autoControl2 = false;
  
        }
if (c > led3OnHourStart ||
      (c == led3OnHourStart && currentMinute >= led3OnMinuteStart)) {
    if (c < led3OnHourEnd ||
        (c == led3OnHourEnd && currentMinute < led3OnMinuteEnd)){
        if(message == "off3"){
          led3OnHourStart = 0;
        led3OnMinuteStart = 0;
        led3OnHourEnd = 0;
        led3OnMinuteEnd = 0;
        }}}
  
if (c > led3OnHourStart ||
      (c == led3OnHourStart && currentMinute >= led3OnMinuteStart)) {
    if (c < led3OnHourEnd ||
        (c == led3OnHourEnd && currentMinute < led3OnMinuteEnd)){
        manualControl3 = false;
        autoControl3 = true;
        }
    else{ 
          
        manualControl3 = true;
        autoControl3 = false;
        }
}else {
        manualControl3 = true;
        autoControl3 = false;
        }
if(c == led3OnHourEnd && currentMinute == led3OnMinuteEnd){
        digitalWrite(ledPin3, LOW);
        ledState3 = false;
        manualControl3 = true;
        autoControl3 = false;
  
        }
         if (c > led4OnHourStart ||
      (c == led4OnHourStart && currentMinute >= led4OnMinuteStart)) {
    if (c < led4OnHourEnd ||
        (c == led4OnHourEnd && currentMinute < led4OnMinuteEnd)){
        if(message == "off4"){
          led4OnHourStart = 0;
        led4OnMinuteStart = 0;
        led4OnHourEnd = 0;
        led4OnMinuteEnd = 0;
        }}}
  
if (c > led4OnHourStart ||
      (c == led4OnHourStart && currentMinute >= led4OnMinuteStart)) {
    if (c < led4OnHourEnd ||
        (c == led4OnHourEnd && currentMinute < led4OnMinuteEnd)){
        manualControl4 = false;
        autoControl4 = true;
        }
    else{ 
          
        manualControl4 = true;
        autoControl4 = false;
        }
}else {
        manualControl4 = true;
        autoControl4 = false;
        }
if(c == led4OnHourEnd && currentMinute == led4OnMinuteEnd){
        digitalWrite(ledPin4, LOW);
        ledState4 = false;
        manualControl4 = true;
        autoControl4 = false;
        }
      }
void sendSensorData() {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    if (isnan(hum) || isnan(temp)) {
    Serial.println("Lỗi đọc dữ liệu từ cảm biến DHT11");
    return;
  }
    lightValue = digitalRead(analogPin);
    // Gửi dữ liệu cảm biến lên MQTT
    String payload = "{\"temperature\":" + String(temp) + ",\"humidity\":" + String(hum) + "}";
    client.publish(sensorTopic, payload.c_str());
     time_t now = time(nullptr);
  int currentHour = hour(now);
  int currentMinute = minute(now);
c=hour(now)+7;
 if (!manualControl1 && autoControl1) {
      // Bật đèn LED1
      digitalWrite(ledPin1, HIGH);
      ledState1 = true;
    } 
    if (!manualControl2 && autoControl2) {
      // Bật đèn LED1
      digitalWrite(ledPin2, HIGH);
      ledState2 = true;
    } 
    if (!manualControl3 && autoControl3) {
      // Bật đèn LED1
      digitalWrite(ledPin3, HIGH);
      ledState3 = true;
    } 
    if (!manualControl4 && autoControl4) {
      // Bật đèn LED1
      digitalWrite(ledPin4, HIGH);
      ledState4 = true;
    } 
    // Tắt tất cả đèn nếu nhiệt độ lớn hơn 35 độ C
    if (temp > 35) {
        if (ledState1) {
            digitalWrite(ledPin1, LOW);
            ledState1 = false;
        }
        if (ledState2) {
            digitalWrite(ledPin2, LOW);
            ledState2 = false;
        }
        if (ledState3) {
            digitalWrite(ledPin3, LOW);
            ledState3 = false;
        }
        if (ledState4) {
            digitalWrite(ledPin4, LOW);
            ledState4 = false;
        }
    }

   
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Kết nối đến mạng WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to the WiFi network");

    // Đặt chân LED làm đầu ra
    pinMode(ledPin1, OUTPUT);
    pinMode(ledPin2, OUTPUT);
    pinMode(ledPin3, OUTPUT);
    pinMode(ledPin4, OUTPUT);

    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
    digitalWrite(ledPin4, LOW);

    // Kết nối đến một MQTT broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp8266-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("Failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }

    // Đăng ký các chủ đề MQTT
    client.subscribe(ledTopic);
    client.subscribe(sensorTopic);
    client.subscribe(led1ScheduleStartTopic);
  client.subscribe(led1ScheduleEndTopic);
  client.subscribe(led2ScheduleStartTopic);
  client.subscribe(led2ScheduleEndTopic);
  client.subscribe(led3ScheduleStartTopic);
  client.subscribe(led3ScheduleEndTopic);
  client.subscribe(led4ScheduleStartTopic);
  client.subscribe(led4ScheduleEndTopic);
  // Khởi tạo thời gian từ NTP
  configTime(0, 0, "0.uk.pool.ntp.org"); // UTC+7, múi giờ của Việt Nam
  Serial.println("Waiting for NTP time sync");
  while (time(nullptr) < 1510644967) {
    delay(10);
  }
  Serial.println("Time synced with NTP");
    lcd.init();
  lcd.backlight();
}

void loop() {
    client.loop();
    delay(100);
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    if (isnan(hum) || isnan(temp)) {
    Serial.println("Lỗi đọc dữ liệu từ cảm biến DHT11");
    return;
  }
  // Kiểm tra kết nối MQTT và xử lý callback
  if (!client.connected()) {
    
  }
  client.loop();

  // Lấy thời gian hiện tại từ NTP
  time_t now = time(nullptr);
  int currentHour = hour(now);
  int currentMinute = minute(now);
c=hour(now)+7;
  // In thời gian ra Serial Monitor
  Serial.print("Current time: ");
  Serial.print(c);
  Serial.print(":");
  Serial.print(currentMinute);
  Serial.print(":");
  Serial.println(second(now));

  // Kiểm tra xem có đang trong khoảng thời gian bật LED1 từ MQTT không
  // Kiểm tra xem có đang trong khoảng thời gian bật LED1 từ MQTT không
  // Đặt thời gian chờ giữa các lần kiểm tra
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(temp);
  lcd.print("*C");
  lcd.print("  ");
  lcd.print(hum);
  lcd.print(" %");
  lcd.setCursor(0, 1);
  lcd.print("    ");
  lcd.print(c);
  lcd.print(":");
  lcd.print(currentMinute);
  lcd.print(":");
  lcd.print(second(now));
  sendSensorData();
}
