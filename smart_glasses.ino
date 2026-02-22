// Blynk and project setup
#define BLYNK_TEMPLATE_ID "YOUR_BLYNK_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_BLYNK_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#include <Wire.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <WidgetLCD.h>
#include <math.h>
#include <FirebaseESP32.h>

// Wi‑Fi credentials and GPS location
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
const char* LOCATION = "51.5074,-0.1278";

// Firebase credentials (host should be without "https://")
#define FIREBASE_HOST "smart-glasses-defc9-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "BIzJlffLEoR5BiqLCS4ZCyxiMOc88gDuUBRSzE73"

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// Pin definitions
const int trigL = 12, echoL = 13;
const int trigC = 5,  echoC = 18;
const int trigR = 14, echoR = 27;
const int buzzerPin = 26;
const int buttonPin = 32; // wired to GND, using internal pull‑up

MPU6050 mpu;
WidgetLCD lcdV1(V1); // For fall alerts
WidgetLCD lcdV2(V2); // For manual emergency alerts

// Object detection settings
const long OBJ_THRESH = 100;       // in cm
const unsigned long OBJ_BEEP = 2000; // duration in ms
bool objBeeping = false;

// Manual emergency settings
bool btnWasPressed = false;
bool btnAlertActive = false;
unsigned long btnAlertTime = 0;

// Fall detection settings
bool alertActive = false;
const float GYRO_THRESHOLD = 200.0; // threshold in dps

// Reads the distance (cm) from an ultrasonic sensor
long readDist(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 30000);
  return duration ? (duration * 0.0343 / 2) : -1;
}

// Checks for an object nearby; sounds buzzer if within range and writes event to Firebase.
void checkObject() {
  long dL = readDist(trigL, echoL);
  long dC = readDist(trigC, echoC);
  long dR = readDist(trigR, echoR);
  bool detected = false;
  String side = "";
  
  if (dL > 0 && dL < OBJ_THRESH) { detected = true; side = "LEFT"; }
  if (dC > 0 && dC < OBJ_THRESH) { detected = true; side = "CENTRE"; }
  if (dR > 0 && dR < OBJ_THRESH) { detected = true; side = "RIGHT"; }
  
  if (detected && !objBeeping) {
    Serial.println("Object detected on " + side);
    Firebase.setString(firebaseData, "/object_detection", "Object detected on " + side);
    tone(buzzerPin, 1000);
    delay(OBJ_BEEP);
    noTone(buzzerPin);
    objBeeping = true;
  } 
  else if (!detected) {
    objBeeping = false;
    Firebase.setString(firebaseData, "/object_detection", "Cleared");
  }
}

// Checks the manual emergency button and sends an alert with location to Blynk and Firebase for 10 seconds.
void checkButton() {
  bool pressed = (digitalRead(buttonPin) == LOW);
  if (pressed && !btnWasPressed && !btnAlertActive) {
    String msg = "Emergency Alert! Help needed. | Location: " + String(LOCATION);
    Serial.println(msg);
    lcdV2.clear();
    lcdV2.print(0, 0, "Emergency Alert!");
    lcdV2.print(0, 1, LOCATION);
    Blynk.virtualWrite(V2, msg);
    Firebase.setString(firebaseData, "/manual_alert", msg);
    btnAlertActive = true;
    btnAlertTime = millis();
  }
  btnWasPressed = pressed;
  
  if (btnAlertActive && (millis() - btnAlertTime >= 10000)) {
    lcdV2.clear();
    Blynk.virtualWrite(V2, "");
    Firebase.setString(firebaseData, "/manual_alert", "Cleared");
    btnAlertActive = false;
    Serial.println("Emergency alert cleared.");
  }
}

// Checks the MPU6050 gyro values. If the overall dps is above the threshold, a fall alert is active.
// When the gyro value drops below the threshold, it clears the alert and writes the status to Firebase.
void checkFall() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  
  float gxds = gx / 131.0;
  float gyds = gy / 131.0;
  float gzds = gz / 131.0;
  float gyro_mag = sqrt(gxds * gxds + gyds * gyds + gzds * gzds);
  
  Serial.print("Gyro dps: ");
  Serial.println(gyro_mag, 2);
  
  if (gyro_mag > GYRO_THRESHOLD) {
    if (!alertActive) {
      alertActive = true;
      sendFallAlert();
      Firebase.setString(firebaseData, "/fall_alert", "Fall detected! | Loc: " + String(LOCATION));
      Serial.println("Fall alert sent.");
    }
  }
  else {
    if (alertActive) {
      clearFallAlert();
      Firebase.setString(firebaseData, "/fall_alert", "Cleared");
      alertActive = false;
      Serial.println("Fall alert cleared.");
    }
  }
}

void sendFallAlert() {
  lcdV1.clear();
  lcdV1.print(0, 0, "Fall detected!");
  lcdV1.print(0, 1, String("Loc: ") + LOCATION);
  Blynk.virtualWrite(V1, "Fall detected! | Loc: " + String(LOCATION));
}

void clearFallAlert() {
  lcdV1.clear();
  Blynk.virtualWrite(V1, "");
}

void setup() {
  Serial.begin(115200);
  
  pinMode(trigL, OUTPUT); pinMode(echoL, INPUT);
  pinMode(trigC, OUTPUT); pinMode(echoC, INPUT);
  pinMode(trigR, OUTPUT); pinMode(echoR, INPUT);
  
  pinMode(buzzerPin, OUTPUT);
  noTone(buzzerPin);
  
  pinMode(buttonPin, INPUT_PULLUP);
  
  Wire.begin(21, 22);
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }
  Serial.println("MPU6050 connected.");
  
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);
  
  // Setup Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
  Serial.println("Connected to Firebase.");
  
  Serial.println("System ready.");
}

void loop() {
  Blynk.run();
  checkObject();
  checkButton();
  checkFall();
  delay(200);
}
//Added main Arduino code
