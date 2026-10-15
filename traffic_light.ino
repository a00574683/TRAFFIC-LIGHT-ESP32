// traffic_light.ino — Semáforo ESP32
// Rojo: GPIO2 (20s) | Verde: GPIO5 (15s) | Amarillo: GPIO4 (5s)
// Envía estado por WiFi HTTP POST + Serial USB (115200) cada segundo y en cada cambio.

#include <WiFi.h>
#include <HTTPClient.h>

// --- Configuración WiFi y servidor ---
const char* WIFI_SSID = "TU_WIFI";
const char* WIFI_PASS = "TU_PASSWORD";
// IP de este equipo (donde corre server.py). Ej: "http://192.168.1.50:8000/api/status"
const char* SERVER_URL = "http://192.168.1.50:8000/api/status";

// --- Pines y duraciones (segundos) ---
const int RED_PIN = 2;
const int YELLOW_PIN = 4;
const int GREEN_PIN = 5;

const int RED_SECS = 20;
const int GREEN_SECS = 15;
const int YELLOW_SECS = 5;

// Secuencia: ROJO -> VERDE -> AMARILLO -> ROJO...
enum Light { L_RED, L_GREEN, L_YELLOW };
Light current = L_RED;
unsigned long stateStartMs = 0;
int lastReportedRemaining = -1;

int totalOf(Light l) {
  if (l == L_RED) return RED_SECS;
  if (l == L_GREEN) return GREEN_SECS;
  return YELLOW_SECS;
}

const char* nameOf(Light l) {
  if (l == L_RED) return "red";
  if (l == L_GREEN) return "green";
  return "yellow";
}

void applyPins() {
  digitalWrite(RED_PIN, current == L_RED ? HIGH : LOW);
  digitalWrite(GREEN_PIN, current == L_GREEN ? HIGH : LOW);
  digitalWrite(YELLOW_PIN, current == L_YELLOW ? HIGH : LOW);
}

void sendStatus(int remaining, bool force) {
  if (!force && remaining == lastReportedRemaining) return;
  lastReportedRemaining = remaining;

  // JSON: {"light":"red","remaining":12,"total":20}
  String payload = String("{\"light\":\"") + nameOf(current) +
                   String("\",\"remaining\":") + remaining +
                   String(",\"total\":") + totalOf(current) + String("}");

  // 1) Siempre por Serial (para debug / modo USB)
  Serial.println(payload);

  // 2) Por WiFi si hay conexión
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");
    int code = http.POST(payload);
    if (code < 200 || code >= 300) {
      Serial.printf("POST fail: %d\n", code);
    }
    http.end();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Conectando WiFi");
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi OK, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Sin WiFi: solo Serial. El semaforo sigue corriendo.");
  }

  current = L_RED;
  stateStartMs = millis();
  applyPins();
  sendStatus(totalOf(current), true);
}

void loop() {
  unsigned long elapsed = (millis() - stateStartMs) / 1000;
  int total = totalOf(current);
  int remaining = total - (int)elapsed;

  if (remaining <= 0) {
    // Avanzar secuencia
    if (current == L_RED) current = L_GREEN;
    else if (current == L_GREEN) current = L_YELLOW;
    else current = L_RED;
    stateStartMs = millis();
    applyPins();
    sendStatus(totalOf(current), true);
  } else {
    sendStatus(remaining, false);
  }
  delay(100);
}
