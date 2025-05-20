#include <WiFi.h>
#include <Wire.h>
#include <MPU9250_asukiaaa.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <ESPmDNS.h>

// Endereço padrão do MPU9250
#define MPU9250_ADDRESS 0x68
MPU9250_asukiaaa mySensor(MPU9250_ADDRESS);

// Servidor Web e DNS
AsyncWebServer server(80);
DNSServer dnsServer;
Preferences preferences;

// Configurações do Access Point
const char* apSSID = "SafeCare-AP";
const char* apPassword = "safecare123";

// Variáveis globais para detecção de queda e tremor
float previousAccelZ = 0;
bool fallDetected = false;
unsigned long fallAlertStartTime = 0;
bool tremorDetected = false;
bool prolongedTremorDetected = false;
unsigned long tremorStartTime = 0;
const float tremorThreshold = 0.4; // Ajuste conforme necessário
const unsigned long tremorDuration = 3000; // Tempo mínimo de tremor para ativar alerta (ms)
const unsigned long fallAlertDuration = 8000; // Duração do alerta de queda (ms)
const unsigned long tremorAlertDuration = 6000; // Duração do alerta de tremor (ms)
const float fallThresholdZ = 1.3; // Novo limite para queda

void setup() {
  Serial.begin(115200);
  preferences.begin("wifi-config", false);

  // Tenta conectar ao Wi-Fi com credenciais salvas
  String savedSSID = preferences.getString("ssid", "");
  String savedPassword = preferences.getString("password", "");

  if (savedSSID != "" && savedPassword != "") {
    Serial.println("Tentando conectar ao Wi-Fi salvo...");
    WiFi.begin(savedSSID.c_str(), savedPassword.c_str());
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(1000);
      Serial.print(".");
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    // Inicia o Access Point
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSSID, apPassword);
    Serial.println("\nAccess Point iniciado");
    Serial.print("IP do AP: ");
    Serial.println(WiFi.softAPIP());

    // Configura o DNS para redirecionar todas as requisições para o IP do AP
    dnsServer.start(53, "*", WiFi.softAPIP());
  } else {
    Serial.println("\nWi-Fi conectado!");
    Serial.println(WiFi.localIP());
    // Configura mDNS
    if (MDNS.begin("safecare")) {
      Serial.println("mDNS iniciado: safecare.local");
    }
  }

  // Inicializa o MPU9250
  Wire.begin();
  mySensor.setWire(&Wire);
  mySensor.beginAccel();
  Serial.println("MPU9250 inicializado com sucesso!");

  // Configura rotas do servidor web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/home");
  });

  server.on("/home", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html><html><head><title>SafeCare</title>";
    html += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
    html += "<style>body { font-family: Arial, sans-serif; background-color: #b3d9ff; color: #333; text-align: center; margin: 0; padding: 0; }";
    html += "h1 { color: #00509e; } .alert { color: #d9534f; font-weight: bold; font-size: 1.5em; } .data { margin: 20px auto; padding: 10px; width: 50%; background: #ffffff; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }";
    html += "a { color: #00509e; text-decoration: none; margin: 10px; display: inline-block; }";
    html += "</style></head><body>";
    html += "<h1>SafeCare</h1>";
    html += "<a href='/wifi'>Configurar Wi-Fi</a>";
    html += "<div id='data' class='data'></div>";
    html += "<canvas id='chart' width='200' height='100'></canvas>";
    html += "<script>";
    html += "const ctx = document.getElementById('chart').getContext('2d');";
    html += "const chart = new Chart(ctx, { type: 'line', data: { labels: [], datasets: [{ label: 'Acelerometro Z', data: [], borderColor: 'rgba(0, 0, 0)', borderWidth: 2, fill: false }] }, options: { scales: { x: { title: { display: true, text: 'Tempo (s)' } }, y: { title: { display: true, text: 'Aceleracao (g)' } } } } });";
    html += "setInterval(() => {";
    html += "  fetch('/data').then(r => r.json()).then(d => {";
    html += "    document.getElementById('data').innerHTML = d.html;";
    html += "    const time = new Date().toLocaleTimeString();";
    html += "    if (chart.data.labels.length > 20) { chart.data.labels.shift(); chart.data.datasets[0].data.shift(); }";
    html += "    chart.data.labels.push(time);";
    html += "    chart.data.datasets[0].data.push(d.accelZ);";
    html += "    chart.update();";
    html += "  });";
    html += "}, 500);";
    html += "</script>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html><html><head><title>SafeCare Config</title>";
    html += "<style>body { font-family: Arial, sans-serif; background-color: #b3d9ff; color: #333; text-align: center; }";
    html += "h1 { color: #00509e; } form { margin: 20px auto; padding: 20px; width: 50%; background: #ffffff; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }";
    html += "input, button { padding: 10px; margin: 5px; width: 80%; font-size: 1em; } button { background-color: #00509e; color: white; border: none; cursor: pointer; }";
    html += "a { color: #00509e; text-decoration: none; margin: 10px; display: inline-block; }";
    html += "</style></head><body>";
    html += "<h1>Configurar Wi-Fi SafeCare</h1>";
    if (WiFi.status() == WL_CONNECTED) {
      html += "<a href='/home'>Voltar para Home</a>";
    }
    html += "<form action='/save' method='POST'>";
    html += "<input type='text' name='ssid' placeholder='Nome do Wi-Fi' required><br>";
    html += "<input type='password' name='password' placeholder='Senha do Wi-Fi' required><br>";
    html += "<button type='submit'>Salvar</button>";
    html += "</form></body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
      String ssid = request->getParam("ssid", true)->value();
      String password = request->getParam("password", true)->value();
      preferences.putString("ssid", ssid);
      preferences.putString("password", password);
      request->send(200, "text/plain", "Credenciais salvas! Reiniciando...");
      delay(1000);
      ESP.restart();
    } else {
      request->send(400, "text/plain", "Erro: SSID ou senha não fornecidos");
    }
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<p>Acelerômetro - X: " + String(mySensor.accelX()) + " Y: " + String(mySensor.accelY()) + " Z: " + String(mySensor.accelZ()) + "</p>";
    if (fallDetected && millis() - fallAlertStartTime < fallAlertDuration) {
      html += "<p class='alert'>ALERTA: Queda detectada!</p>";
    } else if (fallDetected) {
      fallDetected = false;
    }
    if (prolongedTremorDetected && millis() - tremorStartTime < tremorAlertDuration) {
      html += "<p class='alert'>ALERTA: Tremor detectado por tempo prolongado!</p>";
    } else if (prolongedTremorDetected) {
      prolongedTremorDetected = false;
    }
    String response = "{\"html\":\"" + html + "\",\"accelZ\":" + String(mySensor.accelZ()) + "}";
    request->send(200, "application/json", response);
  });

  server.begin();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    dnsServer.processNextRequest();
  }

  mySensor.accelUpdate();
  float accelZ = mySensor.accelZ();

  if (abs(accelZ - previousAccelZ) > fallThresholdZ) {
    fallDetected = true;
    fallAlertStartTime = millis();
    Serial.println("ALERTA: Queda detectada!");
  }

  if (abs(mySensor.accelX()) > tremorThreshold || abs(mySensor.accelY()) > tremorThreshold || abs(mySensor.accelZ() - previousAccelZ) > tremorThreshold) {
    if (!tremorDetected) {
      tremorDetected = true;
      tremorStartTime = millis();
      Serial.println("Tremor detectado!");
    }
    if (millis() - tremorStartTime >= tremorDuration) {
      prolongedTremorDetected = true;
      Serial.println("ALERTA: Tremor detectado por tempo prolongado!");
    }
  } else {
    tremorDetected = false;
  }

  previousAccelZ = accelZ;
  delay(500);
}