#include <WiFi.h>
#include <WebServer.h>
// 
// ================== НАЛАШТУВАННЯ ==================
const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Оптимальний діапазон: 3000–5000 мс
// >8000 мс — не рекомендовано
const uint32_t SCAN_TIMEOUT_MS = 5000;

// Таймаут підключення до Wi-Fi
const uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000;
// ==================================================

WebServer server(80);
uint32_t requestCount = 0;

// ----- Encryption → текст -----
const char* encryptionToStr(wifi_auth_mode_t type) {
  switch (type) {
    case WIFI_AUTH_OPEN:            return "OPEN";
    case WIFI_AUTH_WEP:             return "WEP";
    case WIFI_AUTH_WPA_PSK:         return "WPA";
    case WIFI_AUTH_WPA2_PSK:        return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:    return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-ENT";
    case WIFI_AUTH_WPA3_PSK:        return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK:   return "WPA2/WPA3";
    default:                        return "UNKNOWN";
  }
}

// ----- Wi-Fi статус → текст (для помилок) -----
const char* wifiStatusToStr(wl_status_t s) {
  switch (s) {
    case WL_NO_SSID_AVAIL: return "SSID not available";
    case WL_CONNECT_FAILED: return "Connection failed";
    case WL_CONNECTION_LOST: return "Connection lost";
    case WL_DISCONNECTED: return "Disconnected";
    default: return "Unknown status";
  }
}

// ----- HTTP handler -----
void handleRoot() {
  requestCount++;
  uint32_t reqStart = millis();

  Serial.println("\n--- HTTP REQUEST ---");
  Serial.print("[HTTP] #"); Serial.println(requestCount);
  Serial.print("[HTTP] Client IP: ");
  Serial.println(server.client().remoteIP());
  Serial.print("[HTTP] URI: ");
  Serial.println(server.uri());
  Serial.print("[HTTP] Free heap (before scan): ");
  Serial.println(ESP.getFreeHeap());

  // ----- Wi-Fi scan -----
  Serial.println("[SCAN] Starting Wi-Fi scan...");
  uint32_t scanStart = millis();

  int networks = WiFi.scanNetworks(false, true);
  uint32_t scanTime = millis() - scanStart;

  Serial.print("[SCAN] Done. Networks: ");
  Serial.print(networks);
  Serial.print(" | Time: ");
  Serial.print(scanTime);
  Serial.println(" ms");

  Serial.print("[SCAN] Free heap (after scan): ");
  Serial.println(ESP.getFreeHeap());

  // ----- HTML -----
  String html;
  html.reserve(4096);

  html += "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  html += "<title>ESP32 Wi-Fi Scan</title>";
  html += "<style>";
  html += "body{font-family:Arial;background:#111;color:#eee;}";
  html += ".row{padding:6px;}";
  html += ".odd{background:#1e1e1e;}";
  html += ".even{background:#2a2a2a;}";
  html += ".hdr{font-weight:bold;background:#333;}";
  html += "</style></head><body>";

  html += "<h2>ESP32 Wi-Fi Scan</h2>";
  html += "<p><b>Networks:</b> " + String(networks) + "<br>";
  html += "<b>Scan time:</b> " + String(scanTime) + " ms</p>";

  html += "<div class='row hdr'>";
  html += "# | SSID | BSSID | CH | RSSI | ENC";
  html += "</div>";

  for (int i = 0; i < networks; i++) {
    String rowClass = (i % 2 == 0) ? "even" : "odd";
    String ssid = WiFi.SSID(i);
    if (ssid.length() == 0) ssid = "[hidden]";

    html += "<div class='row " + rowClass + "'>";
    html += String(i + 1) + " | ";
    html += ssid + " | ";
    html += WiFi.BSSIDstr(i) + " | ";
    html += String(WiFi.channel(i)) + " | ";
    html += String(WiFi.RSSI(i)) + " | ";
    html += encryptionToStr(WiFi.encryptionType(i));
    html += "</div>";
  }

  html += "</body></html>";

  server.send(200, "text/html", html);

  Serial.print("[HTTP] Total request time: ");
  Serial.print(millis() - reqStart);
  Serial.println(" ms");
  Serial.println("--------------------");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== ESP32 Wi-Fi Scan (educational) ===");
  Serial.print("[WiFi] Connecting to: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint32_t startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startAttempt > WIFI_CONNECT_TIMEOUT_MS) {
      Serial.println("\n[WiFi][ERROR] Connection timeout");
      Serial.print("[WiFi][ERROR] Status: ");
      Serial.println(wifiStatusToStr(WiFi.status()));
      break;
    }
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] Connected successfully");
    Serial.print("[WiFi] IP: "); Serial.println(WiFi.localIP());
    Serial.print("[WiFi] Gateway: "); Serial.println(WiFi.gatewayIP());
    Serial.print("[WiFi] Mask: "); Serial.println(WiFi.subnetMask());
    Serial.print("[WiFi] DNS: "); Serial.println(WiFi.dnsIP());
    Serial.print("[WiFi] RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  }

  server.on("/", handleRoot);
  server.begin();
  Serial.println("[HTTP] Server started");
}

void loop() {
  server.handleClient();
}
