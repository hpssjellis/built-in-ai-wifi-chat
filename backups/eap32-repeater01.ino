#include <WiFi.h>

// User Preferences: WiFi Credentials for the main network (STA)
// This is the network you want to extend the range of
const char* mySsid = "YOUR_MAIN_WIFI_SSID";
const char* myPassword = "YOUR_MAIN_WIFI_PASSWORD";

// User Preferences: WiFi Credentials for the new hotspot (AP)
// This is the new network that devices will connect to
const char* myRepeaterSsid = "myEsp32Repeater";
const char* myRepeaterPassword = "mysecurepassword"; // Must be at least 8 characters

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("\nStarting ESP32 AP+STA Repeater Setup...");

  // 1. Set the ESP32 to AP_STA dual mode
  // The ESP32 will act as both a client (Station) and a host (Access Point)
  WiFi.mode(WIFI_AP_STA);
  Serial.println("WiFi Mode set to AP_STA.");

  // 2. Configure and start the Access Point (Repeater Hotspot)
  // This creates the new Wi-Fi network
  if (WiFi.softAP(myRepeaterSsid, myRepeaterPassword)) {
    Serial.print("Access Point created with SSID: ");
    Serial.println(myRepeaterSsid);
    Serial.print("Repeater IP Address (for connected devices): ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("ERROR: Failed to start Access Point.");
  }
  
  // 3. Connect to the main Wi-Fi network (Station)
  Serial.print("Connecting to main network: ");
  Serial.println(mySsid);
  WiFi.begin(mySsid, myPassword);
  
  // Wait for connection to the main router
  int myConnectAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && myConnectAttempts < 30) {
    delay(500);
    Serial.print(".");
    myConnectAttempts++;
  }
  
  // 4. Report final connection status
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nSUCCESS: Connected to main Wi-Fi.");
    Serial.print("ESP32 Station IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nERROR: Failed to connect to main Wi-Fi after 30 attempts.");
  }
  
  Serial.println("\nSetup Complete. The ESP32 is now running in AP+STA dual mode.");
  Serial.println("It is now connecting to your main router AND hosting a new network.");
  Serial.println("!!! NOTE: This basic sketch does NOT include the necessary data forwarding/NAT yet. !!!");
  Serial.println("For full repeating functionality, you need to implement a NAT solution (like a dedicated firmware).");
}

void loop() {
  // In a real repeater, the loop would continuously check connection status
  // and handle network data forwarding (NAT).
  // For this basic sketch, we'll just print a status message every 5 seconds.
  
  // Ensure the main network connection is still active
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("!!! WARNING: Main Wi-Fi connection lost. Attempting to reconnect...");
    WiFi.begin(mySsid, myPassword);
  }
  
  delay(5000); // Wait 5 seconds
}
