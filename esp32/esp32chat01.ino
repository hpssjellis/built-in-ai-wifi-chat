#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Wi-Fi credentials
const char * mySsid = "YourNetworkName";
const char * myPassword = "YourNetworkPassword";

// Server and WebSocket setup
AsyncWebServer myServer(80);
AsyncWebSocket myWebSocket("/ws");

void myNotifyClients(const String& myMessage) {
  // Send the message to all connected clients
  myWebSocket.textAll(myMessage);
}

void onMyWebSocketEvent(
  AsyncWebSocket * myServer, 
  AsyncWebSocketClient * myClient, 
  AwsEventType myType, 
  void * myArg, 
  uint8_t * myData, 
  size_t myLen) {

  // Handle various WebSocket events
  if(myType == WS_EVT_CONNECT){
    Serial.printf("Client #%u connected from %s\n", myClient->id(), myClient->remoteIP().toString().c_str());
  } else if(myType == WS_EVT_DISCONNECT){
    Serial.printf("Client #%u disconnected\n", myClient->id());
  } else if(myType == WS_EVT_DATA){
    // Data received from client
    AwsFrameInfo * myInfo = (AwsFrameInfo*)myArg;
    if(myInfo->final && myInfo->index == 0 && myInfo->len == myLen){
      if(myInfo->opcode == WS_TEXT){
        // Convert the received data to a String and broadcast it
        myData[myLen] = 0; // Null-terminate the string
        String myClientMessage = (char*)myData;
        Serial.printf("Received message: %s\n", myClientMessage.c_str());
        
        // Broadcast the message to all connected clients
        myNotifyClients(myClientMessage);
      }
    }
  }
}

const char myIndexHtml[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32-S3 Chat Server</title>
  <style>
    /* Minimal inline CSS as requested */
    body { font-family: Arial, sans-serif; margin: 20px; }
    #myChatBox { height: 300px; border: 1px solid #ccc; padding: 10px; overflow-y: scroll; margin-bottom: 10px; }
    #myMessageBox { width: 80%; }
    #mySendButton { width: 18%; }
  </style>
</head>
<body>
  <h1>ESP32 Chat Server</h1>
  <div id="myChatBox"></div>
  <input type="text" id="myMessageBox" placeholder="Type a message...">
  <button id="mySendButton" onclick="mySendMessage()">Send</button>

  <script>
    var myWebSocket;
    
    // Use async/await for an asynchronous flow
    async function myInitWebSocket() {
      console.log('Trying to open a WebSocket connection...');
      
      // The gateway is the WebSocket URL: ws://[ESP32_IP_Address]/ws
      var myGateway = `ws://${window.location.hostname}/ws`;
      myWebSocket = new WebSocket(myGateway);
      
      // Static links to function names
      myWebSocket.onopen = myOnOpen;
      myWebSocket.onclose = myOnClose;
      myWebSocket.onmessage = myOnMessage;
      myWebSocket.onerror = myOnError;
    }

    function myOnOpen(event) {
      console.log('WebSocket connection opened');
      myUpdateChat('System: Connected to chat server.');
    }

    function myOnClose(event) {
      console.log('WebSocket connection closed');
      myUpdateChat('System: Disconnected. Trying to reconnect in 2s...');
      // Reconnect logic
      setTimeout(myInitWebSocket, 2000); 
    }

    function myOnError(event) {
      console.error('WebSocket Error:', event);
    }

    function myOnMessage(event) {
      // Message received from the server (another client's message)
      myUpdateChat(event.data);
    }

    function myUpdateChat(message) {
      var myChatBox = document.getElementById('myChatBox');
      var myNewMessage = document.createElement('p');
      myNewMessage.innerHTML = message;
      myChatBox.appendChild(myNewMessage);
      // Auto-scroll to the latest message
      myChatBox.scrollTop = myChatBox.scrollHeight;
    }

    function mySendMessage() {
      var myMessageBox = document.getElementById('myMessageBox');
      var myMessage = myMessageBox.value.trim();
      
      if (myMessage !== '') {
        // You can add a username or ID here before sending: `Client-A: ${myMessage}`
        myWebSocket.send(myMessage); 
        myMessageBox.value = ''; // Clear the input field
      }
    }
    
    // Start WebSocket when the page loads
    window.onload = myInitWebSocket;
  </script>
</body>
</html>)rawliteral";

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(mySsid, myPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());

  // Setup WebSocket handler
  myWebSocket.onEvent(onMyWebSocketEvent);
  myServer.addHandler(&myWebSocket);

  // Serve the HTML page on the root ("/")
  myServer.on("/", HTTP_GET, [](AsyncWebServerRequest * myRequest){
    myRequest->send_P(200, "text/html", myIndexHtml);
  });

  // Start the server
  myServer.begin();
  Serial.println("HTTP and WebSocket server started.");
}

void loop() {
  // Use cleanupClients in the loop to manage connections
  myWebSocket.cleanupClients();
  // Since we use the Asynchronous server, the loop is kept clean.
}
