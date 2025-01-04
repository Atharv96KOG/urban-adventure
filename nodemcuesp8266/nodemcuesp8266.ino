#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>

// Define pins for RFID
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN); // RFID instance

// Wi-Fi credentials
const char* ssid = "Atharv";         // Replace with your Wi-Fi name
const char* password = "9697981972"; // Replace with your Wi-Fi password

// Static IP configuration
IPAddress local_IP(192, 168, 73, 30);   // Updated local IP
IPAddress gateway(192, 168, 73, 1);     // Updated gateway
IPAddress subnet(255, 255, 255, 0);     // Subnet mask

WiFiServer server(8080); // Use port 8080

// Timer variables
unsigned long startTime = 0;
unsigned long elapsedTime = 0;
bool timerRunning = false;

// Last scanned UID storage
byte lastUID[10] = {0}; // Support UIDs up to 10 bytes
byte lastUIDLength = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  // Initialize Wi-Fi with static IP
  WiFi.begin(ssid, password);
  WiFi.config(local_IP, gateway, subnet);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
  Serial.print("Static IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin(); // Start the web server
}

void loop() {
  // RFID card detection
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    if (!compareUID(mfrc522.uid.uidByte, mfrc522.uid.size)) {
      storeUID(mfrc522.uid.uidByte, mfrc522.uid.size);

      if (!timerRunning) {
        startTime = millis();
        timerRunning = true;
        Serial.println("Timer Started");
      } else {
        elapsedTime = millis() - startTime;
        timerRunning = false;
        Serial.print("Timer Stopped. Elapsed Time: ");
        Serial.println(elapsedTime / 1000.0, 2);
      }
    }

    delay(1000); // Avoid duplicate scans
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }

  // Handle HTTP requests
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("GET /status") >= 0) {
      handleStatusRequest(client); // Respond to the status request
    } else {
      handleClient(client); // Serve the main HTML page
    }
  }
}

// Compare current UID with the last scanned UID
bool compareUID(byte* uid, byte uidLength) {
  if (uidLength != lastUIDLength) return false;

  for (byte i = 0; i < uidLength; i++) {
    if (uid[i] != lastUID[i]) return false;
  }
  return true;
}

// Store the current UID as the last scanned UID
void storeUID(byte* uid, byte uidLength) {
  lastUIDLength = uidLength;
  for (byte i = 0; i < uidLength; i++) {
    lastUID[i] = uid[i];
  }
}

// Handle incoming HTTP requests
void handleClient(WiFiClient client) {
  String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

  // Dynamic HTML page with timer status
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>RFID Timer</title>
  <script>
    function fetchStatus() {
      fetch('/status').then(response => response.json()).then(data => {
        document.getElementById('status').textContent = data.status;
        document.getElementById('time').textContent = data.time + " seconds";
      });
    }
    setInterval(fetchStatus, 1000); // Update every second
  </script>
</head>
<body onload="fetchStatus()">
  <h1>RFID Timer</h1>
  <p>Status: <span id="status"></span></p>
  <p>Elapsed Time: <span id="time"></span></p>
</body>
</html>
)rawliteral";

  // Send HTML page to the client
  client.print(header + html);
  client.stop();
}

// Handle AJAX requests for status updates
void handleStatusRequest(WiFiClient client) {
  String response = "{";
  response += "\"status\": \"" + String(timerRunning ? "Running" : "Stopped") + "\",";
  response += "\"time\": " + String(elapsedTime / 1000.0, 2);
  response += "}";

  client.print("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
  client.print(response);
  client.stop();
}