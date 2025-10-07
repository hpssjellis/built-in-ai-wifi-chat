#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// --- Configuration ---
// Set the credentials for the Hotspot (Access Point)
const char* myHotspotSSID = "YOURSSID";
const char* myHotspotPassword = "YOURPASS";

// Create AsyncWebServer object on port 80
AsyncWebServer myServer(80);
// Create a WebSocket object on a specific URL
AsyncWebSocket myWebSocket("/ws"); 

// --- HTML Content (Minimal Inline CSS) ---
const char myIndexHtml[] = R"raw(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Chat Hotspot</title>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <style>
    /* Minimal custom CSS for readability */
    body { font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f4f4f4; }
    #myHeader { background-color: #333; color: white; padding: 10px; text-align: center; }
    #myChatBox { 
      height: 300px; 
      overflow-y: scroll; 
      padding: 10px; 
      margin: 10px; 
      border: 1px solid #ccc; 
      background-color: white; 
      border-radius: 8px; /* Added rounded corners */
    }
    #myInputContainer { 
      display: flex; 
      padding: 10px; 
      box-shadow: 0 -2px 5px rgba(0, 0, 0, 0.1); 
      background-color: #fff;
    }
    #myMessageInput { 
      flex-grow: 1; 
      padding: 10px; 
      border: 1px solid #ccc; 
      border-radius: 4px;
    }
    #mySendButton { 
      padding: 10px 20px; 
      margin-left: 10px; 
      cursor: pointer; 
      background-color: #4CAF50; /* A friendly green */
      color: white; 
      border: none; 
      border-radius: 4px;
    }
  </style>
</head>
<body>
  <div id="myHeader">
    <h2>XIAO ESP32S3 JSON Chat</h2>
    <p>Hotspot IP: 192.168.4.1</p>
  </div>
  <div id="myChatBox"></div>
  <div id="myInputContainer">
    <input type="text" id="myMessageInput" placeholder="Type a message...">
    <button id="mySendButton" onclick="mySendMessage()">Send</button>
  </div>

  <script>
    const myChatBox = document.getElementById('myChatBox');
    const myMessageInput = document.getElementById('myMessageInput');
    
    // The XIAO's AP IP is always 192.168.4.1
    // The WebSocket attempts to connect to the ESP32's fixed AP IP
    let myWebSocket = new WebSocket('ws://192.168.4.1/ws');

    // --- Core Functions ---

    // Function to handle opening the connection
    myWebSocket.onopen = function (myEvent) {
      myAppendMessage('System', 'Connected to the ESP32.', 'blue');
    };

    // Function to handle messages received from the ESP32
    myWebSocket.onmessage = async function (myEvent) {
      const myRawData = myEvent.data;
      
      try {
        // Use await to simulate waiting for JSON parsing (good practice for teaching async)
        const myData = await new Promise(resolve => resolve(JSON.parse(myRawData)));
        
        // Use the data to display the message
        myAppendMessage(myData.mySender, myData.myMessage, 'black');
        
      } catch (myError) {
        // Handle non-JSON data or parsing errors
        myAppendMessage('System Error', 'Received non-JSON data.', 'red');
      }
    };
    
    // Function to handle connection closure
    myWebSocket.onclose = function (myEvent) {
        myAppendMessage('System', 'Connection closed.', 'red');
    };

    // Function to send a JSON message to the ESP32
    async function mySendMessage() {
      const myMessage = myMessageInput.value.trim();
      if (myMessage === "") return;

      if (myWebSocket.readyState === WebSocket.OPEN) {
        // 1. Create a JavaScript object with my-prefixed keys
        const myDataObject = {
          myMessage: myMessage,
          mySender: "Client" 
        };
        
        // 2. Convert the object to a JSON string
        const myJsonString = JSON.stringify(myDataObject);

        // 3. Send the JSON string
        myWebSocket.send(myJsonString);
        
        // Display the sent message immediately
        myAppendMessage('You', myMessage, 'green'); 
        myMessageInput.value = ''; // Clear input
      } else {
        myAppendMessage('System', 'WebSocket is not open. Refresh the page.', 'red');
      }
    }

    // Function to dynamically add messages to the chat box
    function myAppendMessage(mySender, myText, myColor) {
      const myElement = document.createElement('p');
      myElement.style.cssText = 'margin: 5px 0; line-height: 1.4;'; // Inline CSS for spacing
      myElement.innerHTML = `<b style="color: ${myColor};">${mySender}:</b> ${myText}`;
      myChatBox.appendChild(myElement);
      // Auto-scroll to the bottom
      myChatBox.scrollTop = myChatBox.scrollHeight;
    }
    
    // Static link equivalent using an event listener for "Enter" key press
    myMessageInput.addEventListener('keypress', function(e) {
      if (e.key === 'Enter') {
        mySendMessage();
      }
    });

  </script>
</body>
</html>
)raw";

// --- ESP32 Logic ---

// Function to handle incoming WebSocket events
void myOnWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    // Send JSON welcome message
    client->text("{\"mySender\":\"System\", \"myMessage\":\"Welcome to the chat!\"}"); 
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    
    // *** FIX APPLIED HERE: Changed info->opCode to info->opcode ***
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      // Data is a text message, expected to be JSON
      StaticJsonDocument<200> myJsonDoc;
      
      // Deserialize the JSON string
      DeserializationError myError = deserializeJson(myJsonDoc, data);

      if (myError) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(myError.f_str());
        // Notify client of JSON error
        client->text("{\"mySender\":\"System\", \"myMessage\":\"Invalid JSON structure received.\"}")
;        return;
      }
      
      // Successfully parsed JSON (using keys matching the JS structure)
      const char* myMessage = myJsonDoc["myMessage"];
      const char* mySender = myJsonDoc["mySender"];
      
      Serial.printf("Client #%u received: Sender: %s, Message: %s\n", client->id(), mySender, myMessage);

      // --- Broadcasting the received JSON message back to ALL connected clients ---
      // This is the core chat server function.
      String myResponse;
      serializeJson(myJsonDoc, myResponse);
      myWebSocket.textAll(myResponse);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // 1. Setup Access Point (Hotspot) Mode
  WiFi.softAP(myHotspotSSID, myHotspotPassword);
  Serial.print("Hotspot created! Connect to: ");
  Serial.println(myHotspotSSID);
  Serial.print("Web Server IP (use this URL): ");
  Serial.println(WiFi.softAPIP()); // This will be 192.168.4.1 by default

  // 2. Configure WebSocket and server handlers
  myWebSocket.onEvent(myOnWsEvent);
  myServer.addHandler(&myWebSocket);

  // Route to serve the main chat page (index.html)
  myServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", myIndexHtml);
  });

  // Handle all other requests
  myServer.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  });

  // 3. Start the Server
  myServer.begin();
}

void loop() {
  // WebSocket maintenance - essential for cleaning up closed connections
  myWebSocket.cleanupClients(); 
}
