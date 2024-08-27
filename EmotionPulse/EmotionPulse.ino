#include <Wire.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Encoder.h>

#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;

const byte RATE_SIZE = 4;  //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];     //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0;  //Time at which the last beat occurred

HardwareSerial neogps(1);
char datoCmd = 0;

float beatsPerMinute;
int beatAvg;
float globalTemp;

// PulseSensor SDA D21
// PulseSensor SCL D22

#define PIN_RED 18    // D18
#define PIN_GREEN 19  // D19
#define PIN_BLUE 23   // D23

#define RXD2 16
#define TXD2 17
#define NMEA 0

TinyGPSPlus gps;

// Conexión MQTT
#define MQTT_MAX_PACKET_SIZE 256
const char* ssid = "";
const char* password = "";
const char* mqttServer = "broker.hivemq.com";
const char* mqttTopic = "emotionpulse/";
const int mqttPort = 1883;
const char* mqttUser = NULL;
const char* mqttPassword = NULL;

WiFiClient espClient;
PubSubClient client(espClient);

// Acelerometro
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

const float threshold = 1.2;  // Adjust this threshold for step detection sensitivity
const int bufferLength = 15;  // Number of accelerometer readings in the buffer
float buffer[bufferLength];
int bufferIndex = 0;
int stepCount = 0;
bool stepDetected = false;

const unsigned long debounceDelay = 300;  // Debounce delay in milliseconds
unsigned long lastStepTime = 0;

unsigned long lastTime = 0;
const long interval = 10000;

// Modulo grabador de voz
#define PIN_REC 5    // Pin para iniciar la grabación
#define PIN_PLAYE 4  // Pin para reproducir desde el inicio

// Menú Oled
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pines para el encoder
#define ENCODER_PIN1 32        // CLK
#define ENCODER_PIN2 33        // DT
#define ENCODER_BUTTON_PIN 25  // SW

ESP32Encoder encoder;
int lastEncoderPosition = 0;
int menuOption = 0;
bool inSubMenu = false;
int subMenuOption = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))  //Use default I2C port, 400kHz speed
  {
    while (1)
      ;
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);
  particleSensor.enableDIETEMPRDY();

  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);

  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // MQTT
  connectWifi();
  connectMQTT();
  subscribeToTopics();  // Suscribirse a los tópicos

  client.publish("test/topic", "Hello, MQTT");

  // Acelerometro
  Wire.begin();

  if (!accel.begin()) {
    Serial.println("Could not find a valid ADXL345 sensor, check wiring!");
    while (1)
      ;
  }

  accel.setRange(ADXL345_RANGE_16_G);

  // Modulo grabador de voz
  pinMode(PIN_REC, OUTPUT);
  pinMode(PIN_PLAYE, OUTPUT);

  digitalWrite(PIN_REC, LOW);    // Asegura que no esté grabando al iniciar
  digitalWrite(PIN_PLAYE, LOW);  // Asegura que no esté reproduciendo al iniciar

  // Menú Oled
  pinMode(ENCODER_BUTTON_PIN, INPUT_PULLUP);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  // Configurar el encoder
  encoder.attachHalfQuad(ENCODER_PIN1, ENCODER_PIN2);
  encoder.setCount(0);
  displayMenu();
}

void loop() {
  client.loop();

  // Pulso
  int avgBPM = getAvgBPM();

  // Temperatura
  float temp = getTemp();

  // Acelerometro
  int steps = detectStep();

  // int r = 255, g = 255, b = 255;

  // if (avgBPM < 50) {
  //   r = 0, g = 0, b = 255;
  // } else if (avgBPM > 120) {
  //   r = 255, g = 0, b = 0;
  // } else {
  //   r = 0, g = 255, b = 0;
  // }

  // rgbController(r, g, b);

  unsigned long now = millis();

  detectEncoder();

  if (now - lastTime > interval) {
    lastTime = now;

    // Test
    publishMQTT("test", "test");

    // Pulso
    publishMQTT("heartbeat", String(avgBPM));

    // Temperatura
    publishMQTT("temperature", String(temp));

    // Acelerometro
    // publishMQTT("pedometer", String(steps));
  }

  // getGpsInfo();

  // Reproducir grabación
  // playMessage();
}

void publishMQTT(String topic, String str) {
  char charArr[200];  // Buffer para almacenar el string convertido
  char top[200];      // Buffer para almacenar el topic completo

  str.toCharArray(charArr, sizeof(charArr));
  String fullTopic = mqttTopic + topic;
  fullTopic.toCharArray(top, sizeof(top));

  Serial.print("Sending value: ");
  Serial.println(charArr);
  Serial.print("On Topic: ");
  Serial.println(top);

  if (client.connected()) {
    client.publish(top, charArr);
  } else {
    connectMQTT();  // Reintentar la conexión
  }
}

void connectMQTT() {
  //connecting to a mqtt broker
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqttUser, mqttPassword)) {
      Serial.println("Public EMQX MQTT broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void connectWifi() {
  WiFi.begin(ssid, password);
  Serial.println("...................................");

  Serial.print("Connecting to WiFi.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to the WiFi network");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void getGpsInfo() {
  if (NMEA) {
    while (neogps.available()) {
      datoCmd = (char)neogps.read();
      Serial.print(datoCmd);
    }
  }

  else {
    boolean newData = false;
    for (unsigned long start = millis(); millis() - start < 1000;) {
      while (neogps.available()) {
        if (gps.encode(neogps.read())) {
          newData = true;
        }
      }
    }

    if (newData == true) {
      newData = false;
      Serial.println(gps.satellites.value());
      gpsSerialPrint();
    } else {
    }
  }
}

void gpsSerialPrint() {
  if (gps.location.isValid() == 1) {
    Serial.print("Lat: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Lng: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Speed: ");
    Serial.println(gps.speed.kmph());
    Serial.print("SAT:");
    Serial.println(gps.satellites.value());
    Serial.print("ALT:");
    Serial.println(gps.altitude.meters(), 0);

    Serial.print("Date: ");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.println(gps.date.year());

    Serial.print("Hour: ");
    Serial.print(gps.time.hour());
    Serial.print(":");
    Serial.print(gps.time.minute());
    Serial.print(":");
    Serial.println(gps.time.second());
    Serial.println("---------------------------");
  } else {
    Serial.println("Sin señal gps");
  }
}

void rgbController(int r, int g, int b) {
  analogWrite(PIN_RED, r);
  analogWrite(PIN_GREEN, g);
  analogWrite(PIN_BLUE, b);
}

int getAvgBPM() {
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true) {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;  //Store this reading in the array
      rateSpot %= RATE_SIZE;                     //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  // float temperature = particleSensor.readTemperature();

  // Serial.print("IR=");
  // Serial.print(irValue);
  // Serial.print(", BPM=");
  // Serial.print(beatsPerMinute);
  // Serial.print(", Avg BPM=");
  // Serial.print(beatAvg);
  // Serial.print(", TemperatureC=");
  // Serial.print(temperature, 4);

  // if (irValue < 50000)
  //   Serial.print(" No finger?");

  // Serial.println();

  return beatAvg;
}

int detectStep() {
  sensors_event_t event;
  accel.getEvent(&event);

  float accelerationX = event.acceleration.x;
  float accelerationY = event.acceleration.y;
  float accelerationZ = event.acceleration.z;

  float accelerationMagnitude = sqrt(accelerationX * accelerationX + accelerationY * accelerationY + accelerationZ * accelerationZ);

  buffer[bufferIndex] = accelerationMagnitude;
  bufferIndex = (bufferIndex + 1) % bufferLength;

  // Detect a step if the current magnitude is greater than the average of the buffer by the threshold
  float avgMagnitude = 0;
  for (int i = 0; i < bufferLength; i++) {
    avgMagnitude += buffer[i];
  }
  avgMagnitude /= bufferLength;

  unsigned long currentMillis = millis();

  if (accelerationMagnitude > (avgMagnitude + threshold)) {
    if (!stepDetected && (currentMillis - lastStepTime) > debounceDelay) {
      stepCount++;
      stepDetected = true;
      lastStepTime = currentMillis;

      Serial.println("Step detected!");
      Serial.print("Step count: ");
      Serial.println(stepCount);
      publishMQTT("pedometer", String(stepCount));
    }
  } else {
    stepDetected = false;
  }

  return stepCount;
}

void resetStepCount() {
  stepCount = 0;
  publishMQTT("pedometer", String(stepCount));
}

float getTemp() {
  globalTemp = particleSensor.readTemperature();
  return globalTemp;
}

void startRecording() {
  digitalWrite(PIN_REC, HIGH);  // Inicia grabación
  Serial.println("Grabando...");
}

void stopRecording() {
  digitalWrite(PIN_REC, LOW);  // Detiene grabación
  Serial.println("Grabación detenida");
}

void record() {
  startRecording();
  delay(5000);
  stopRecording();
}

void playMessage() {
  digitalWrite(PIN_PLAYE, HIGH);  // Inicia reproducción
  Serial.println("Reproduciendo mensaje...");
  delay(5000);  // Permite que se inicie la reproducción
  digitalWrite(PIN_PLAYE, LOW);
}

void subscribeToTopics() {
  const char* topics[] = {
    "emotionpulse/led",
    "emotionpulse/record",
    "emotionpulse/play",
    "emotionpulse/resetPedometer"
  };

  int numTopics = sizeof(topics) / sizeof(topics[0]);

  for (int i = 0; i < numTopics; i++) {
    if (client.subscribe(topics[i])) {
      Serial.print("Subscribed to: ");
      Serial.println(topics[i]);
    } else {
      Serial.print("Failed to subscribe to: ");
      Serial.println(topics[i]);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message = "";

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println(message);

  if (String(topic) == "emotionpulse/led") {
    // Convert hexadecimal color code to RGB
    long color = strtol(message.c_str(), NULL, 16);
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;

    rgbController(r, g, b);
  } else if (String(topic) == "emotionpulse/record") {
    // Iniciar grabación
    record();
  } else if (String(topic) == "emotionpulse/play") {
    // Reproducir grabación
    playMessage();
  } else if (String(topic) == "emotionpulse/resetPedometer") {
    resetStepCount();
  }
}

int hexToDecimal(const std::string& hex) {
  int decimalValue;
  std::stringstream ss;
  ss << std::hex << hex;
  ss >> decimalValue;
  return decimalValue;
}

void hexToRGB(const std::string& hex, int& r, int& g, int& b) {
  if (hex.length() == 7 && hex[0] == '#') {
    std::string rHex = hex.substr(1, 2);
    std::string gHex = hex.substr(3, 2);
    std::string bHex = hex.substr(5, 2);

    r = hexToDecimal(rHex);
    g = hexToDecimal(gHex);
    b = hexToDecimal(bHex);
  } else {
    std::cerr << "Formato hexadecimal inválido" << std::endl;
  }
}

// -----------------------------------------------------------------------------------------------------------
// Menú Oled
void detectEncoder() {
  // Leer el valor actual del encoder
  long currentEncoderPosition = encoder.getCount();

  // Detectar cambios en el valor del encoder
  if (currentEncoderPosition != lastEncoderPosition) {
    if (!inSubMenu) {
      menuOption = (abs(currentEncoderPosition % 2));  // Ajuste de sensibilidad
      Serial.print("Menu");
      Serial.println(menuOption);
    } else {
      if (currentEncoderPosition < lastEncoderPosition) {
        if (subMenuOption == 4) {
          subMenuOption = 0;
        } else {
          subMenuOption += 1;
        }
      } else {
        if (subMenuOption == 0) {
          subMenuOption = 4;
        } else {
          subMenuOption -= 1;
        }
      }
      Serial.print("SubMenuOption");
      Serial.println(subMenuOption);
    }
    lastEncoderPosition = currentEncoderPosition;
    displayMenu();
  }

  // Detectar pulsación del botón del encoder
  if (digitalRead(ENCODER_BUTTON_PIN) == LOW) {
    delay(200);  // Debounce
    if (!inSubMenu && menuOption == 1) {
      inSubMenu = true;
      subMenuOption = 0;
      displayMenu();
    } else if (inSubMenu) {
      executeSubMenuAction();
    } else {
      displayData();
    }
  }
}

void displayMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  if (!inSubMenu) {
    display.println(menuOption == 0 ? "> Datos" : "  Datos");
    display.println(menuOption == 1 ? "> Operaciones" : "  Operaciones");
  } else {
    display.println(subMenuOption == 0 ? "> Detectar Emocion" : "  Detectar Emocion");
    display.println(subMenuOption == 1 ? "> Grabar" : "  Grabar");
    display.println(subMenuOption == 2 ? "> Reproducir" : "  Reproducir");
    display.println(subMenuOption == 3 ? "> Reiniciar Pasos" : "  Reiniciar Pasos");
    display.println(subMenuOption == 4 ? "> Volver" : "  Volver");
  }

  display.display();
}

void displayData() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("BPM: " + String(beatAvg));
  display.println("Temp: " + String(globalTemp));
  display.println("Ubicacion: 21°10'03.3'N 100°55'45.9'W");
  display.println("Pasos: " + String(stepCount));
  display.display();
  delay(2000);  // Mostrar los datos por 2 segundos
  displayMenu();
}

void executeSubMenuAction() {
  display.clearDisplay();
  display.setCursor(0, 0);
  switch (subMenuOption) {
    case 0:
      display.println("Detectando Emocion...");
      display.display();
      clasificarEmocion();
      break;
    case 1:
      display.println("Grabando...");
      display.display();
      record();
      break;
    case 2:
      display.println("Reproduciendo...");
      display.display();
      playMessage();
      break;
    case 3:
      display.println("Reiniciando Pasos...");
      display.display();
      resetStepCount();
      delay(1000);
      break;
    case 4:
      inSubMenu = false;
      display.println("Volviendo...");
      display.display();
      delay(1000);
      break;
  }
  displayMenu();
}

void clasificarEmocion() {
  int bpm = beatAvg;
  int temperatura = globalTemp;

  // Umbrales para la BPM y temperatura
  const int bpmFelizMax = 100;
  const int bpmEstresadoMin = 101;
  const int bpmEstresadoMax = 120;
  const int bpmAnsiosoMin = 121;
  const float temperaturaFelizMax = 37.5;
  const float temperaturaEstresadoMin = 37.6;
  const float temperaturaAnsiosoMin = 38.0;

  delay(2000);

  // Clasificación de emociones
  if (bpm <= bpmFelizMax && temperatura <= temperaturaFelizMax) {
    rgbController(235, 229, 52);
  } else if (bpm >= bpmEstresadoMin && bpm <= bpmEstresadoMax && temperatura >= temperaturaEstresadoMin && temperatura < temperaturaAnsiosoMin) {
    rgbController(235, 52, 52);
  } else if (bpm >= bpmAnsiosoMin || temperatura >= temperaturaAnsiosoMin) {
    rgbController(52, 82, 235);
  } else {
    rgbController(52, 235, 64);
  }
}