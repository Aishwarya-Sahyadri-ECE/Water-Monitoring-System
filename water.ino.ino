#define BLYNK_TEMPLATE_ID "TMPL3H62QRVKV"
#define BLYNK_TEMPLATE_NAME "IOT WATER MONITORING"
#define BLYNK_AUTH_TOKEN "NCrNwIR5pHmtgQDvrErTS1pA78I_COxT"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <HTTPClient.h>

// WiFi credentials
char ssid[] = "realme";
char pass[] = "@123456789";

// OLED display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Sensor & pin definitions
#define TRIG_PIN 5
#define ECHO_PIN 18
#define DHT_PIN 4
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// LEDs and buzzer
#define LED_RED 27
#define LED_YELLOW 26
#define LED_BLUE 25
#define BUZZER 2

const float tankHeight = 20.0;   // cm
const float mlPerCm = 40.0;

// Pipedream
const char* pipedreamUrl = "https://eor59yu94k6km64.m.pipedream.net";  // Replace with your real trigger URL
unsigned long lastEmailTime = 0;
const unsigned long emailInterval = 5 * 60 * 1000; // 10 minutes

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Water Monitor Init...");
  display.display();
  delay(2000);
}

void loop() {
  Blynk.run();

  // Trigger ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;
  float waterHeight = tankHeight - distance;
  if (waterHeight < 0) waterHeight = 0;
  float waterML = waterHeight * mlPerCm;
  if (waterML > 800) waterML = 800;

  float temperature = dht.readTemperature();
  String pumpStatus = "OFF";

  // LED and buzzer logic
  bool redOn = false, yellowOn = false, blueOn = false;

  if (waterML < 200) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(BUZZER, HIGH);
    pumpStatus = "ON";
    redOn = true;

    Blynk.logEvent("low_water", "Warning: Water level is low!");

    // Email trigger with rate limiting
    unsigned long currentTime = millis();
    if (currentTime - lastEmailTime > emailInterval) {
      sendPipedreamEmail(waterML, temperature);
      lastEmailTime = currentTime;
    }

  } else if (waterML < 500) {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(BUZZER, HIGH);
    yellowOn = true;
  } else {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(BUZZER, LOW);
    blueOn = true;
  }

  // OLED display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Water: ");
  display.print(waterML);
  display.println(" ml");

  display.print("Temp : ");
  display.print(temperature);
  display.println(" C");

  display.print("Pump : ");
  display.println(pumpStatus);
  display.display();

  // Serial monitor
  Serial.print("Water: ");
  Serial.print(waterML);
  Serial.println(" ml");

  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Pump: ");
  Serial.println(pumpStatus);
  Serial.println("----------------");

  // Blynk virtual pins
  Blynk.virtualWrite(V0, waterML);
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, pumpStatus);
  Blynk.virtualWrite(V3, redOn ? 255 : 0);
  Blynk.virtualWrite(V4, yellowOn ? 255 : 0);
  Blynk.virtualWrite(V5, blueOn ? 255 : 0);

  delay(2000);
}

void sendPipedreamEmail(float waterLevel, float temperature) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(pipedreamUrl);
    http.addHeader("Content-Type", "application/json");

    String payload = "{";
    payload += "\"Subject\": \"Critical Water Level Alert\",";
    payload += "\"Text\": \"Water level critically low: " + String(waterLevel) + " ml\\nTemperature: " + String(temperature) + " °C\"";
    payload += "}";

    int httpResponseCode = http.POST(payload);
    Serial.print("Pipedream email response: ");
    Serial.println(httpResponseCode);
    http.end();
  } else {
    Serial.println("WiFi not connected. Cannot send email.");
  }
}