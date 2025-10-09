// ===========================================================
// ESP32 Async WebSocket JSON Chat Server
// Compatible with ESP32 Arduino Core v2.0.x
// ===========================================================

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// --- Soft AP Credentials ---
const char *mySsid = "myChatAP88";
const char *myPassword = "myPassword";

// --- Server Setup ---
AsyncWebServer myWebServer(80);
AsyncWebSocket myWebSocket("/ws");

// --- HTML Page (UPDATED to include LLM Prompt button and function) ---
const char *myHtmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 JSON Chat</title>
<style>
body { font-family: sans-serif; background: #f9fafb; text-align:center; padding: 20px; }
#myChatBox { width:95%; max-width:400px; resize: both; 
overflow: auto; margin:auto; background:white; border-radius:12px; box-shadow:0 4px 6px rgba(0,0,0,0.1); padding:10px; }
h2 { color:#2563eb; margin-top: 0; }
#myMessages { height:300px; overflow-y:auto; border:1px solid #ccc; padding:5px; border-radius:6px; text-align:left; margin-bottom: 10px; }
.myName { font-weight:bold; color:#2563eb; }
.mySystem { color:#6b7280; font-size:12px; text-align:center; margin:4px 0; }
.my-input-group { display: flex; gap: 4px; }
.my-input-group > input:first-child { flex-grow: 1; }
/* Style buttons */
.my-input-group input[type="button"] {
    padding: 6px 10px;
    border: none;
    border-radius: 6px;
    cursor: pointer;
    font-weight: bold;
    transition: background-color 0.2s;
}
#myChatButton { background-color: #2563eb; color: white; }
#myChatButton:hover { background-color: #1d4ed8; }
#myPromptButton { background-color: #f59e0b; color: white; }
#myPromptButton:hover { background-color: #d97706; }
</style>
</head>
<body>
<div id="myChatBox">
<h2>ESP32 JSON Chat</h2>
<p>Status: <span id="myStatus" style="color:red;">Disconnected</span></p>
<input id="myNameInput" placeholder="Your name" style="width:95%; padding:6px; margin:4px; border-radius: 6px; border: 1px solid #ccc;"><br>

<div class="my-input-group">
    <input id="myMessageInput" placeholder="Type a message..." style="padding:6px; border-radius: 6px; border: 1px solid #ccc;">
    <input type="button" id="myChatButton" value="Send" onclick="mySendMessage(false)">
    <input type="button" id="myPromptButton" value="Prompt 🤖" onclick="mySendMessage(true)">
</div>

<div id="myMessages"></div>
</div>

<script>
let myWebSocket;
let myName = "";

function myConnect() {
    const myUrl = `ws://${window.location.hostname}/ws`;
    myWebSocket = new WebSocket(myUrl);

    myWebSocket.onopen = () => {
        document.getElementById('myStatus').textContent = "Connected";
        document.getElementById('myStatus').style.color = "green";
        myDisplaySystemMessage("Connected to server");
    };

    myWebSocket.onmessage = (event) => {
        try {
            const myData = JSON.parse(event.data);
            if (myData.system) {
                myDisplaySystemMessage(myData.system);
            } else if (myData.username && myData.message) {
                // If this is a self-message, don't display it twice.
                if (myData.username !== myName) {
                    myDisplayMessage(myData.username, myData.message);
                }
            }
        } catch {
            console.log("Non-JSON:", event.data);
        }
    };

    myWebSocket.onclose = () => {
        document.getElementById('myStatus').textContent = "Disconnected";
        document.getElementById('myStatus').style.color = "red";
        myDisplaySystemMessage("Connection closed. Retrying...");
        setTimeout(myConnect, 3000);
    };
}

// UPDATED function to include optional LLM prefix
function mySendMessage(myIsPrompt) {
    if (!myWebSocket || myWebSocket.readyState !== WebSocket.OPEN) return;
    
    // Check and set the name if needed
    if (!myName) myName = document.getElementById('myNameInput').value.trim();
    
    let myMessage = document.getElementById('myMessageInput').value.trim();
    
    if (myName && myMessage) {
        let myPrefix = '';
        if (myIsPrompt) {
            myPrefix = '[LLM] ';
        }
        
        myMessage = myPrefix + myMessage; // Prepend prefix to the message content
        
        const myJson = JSON.stringify({ myUserName: myName, myMessage: myMessage });
        myWebSocket.send(myJson);
        
        // Display the message locally for the sender (simulates 'user' type on Node client)
        myDisplayMessage(myName, myMessage);
        
        document.getElementById('myMessageInput').value = "";
    }
}

function myDisplayMessage(myName, myMsg) {
    const myMessages = document.getElementById('myMessages');
    const myDiv = document.createElement('div');
    myDiv.innerHTML = `<span class="myName">${myName}:</span> ${myMsg}`;
    myMessages.appendChild(myDiv);
    myMessages.scrollTop = myMessages.scrollHeight;
}

function myDisplaySystemMessage(myMsg) {
    const myMessages = document.getElementById('myMessages');
    const myDiv = document.createElement('div');
    myDiv.className = "mySystem";
    myDiv.textContent = "--- " + myMsg + " ---";
    myMessages.appendChild(myDiv);
    myMessages.scrollTop = myMessages.scrollHeight;
}

window.onload = myConnect;
</script>
</body>
</html>
)rawliteral";

// --- WebSocket Handler ---
void myHandleWsEvent(AsyncWebSocket *myServer, AsyncWebSocketClient *myClient, AwsEventType myType, void *myArg, uint8_t *myPayload, size_t myLength) {
    if (myType == WS_EVT_CONNECT) {
        Serial.printf("Client #%u connected\n", myClient->id());
        // Broadcast system message to all clients including the new one
        StaticJsonDocument<100> myDoc;
        myDoc["system"] = "A new client has joined.";
        String myJson;
        serializeJson(myDoc, myJson);
        myWebSocket.textAll(myJson);
    } 
    else if (myType == WS_EVT_DISCONNECT) {
        Serial.printf("Client #%u disconnected\n", myClient->id());
        // Broadcast system message to remaining clients
        StaticJsonDocument<100> myDoc;
        myDoc["system"] = "A client has left.";
        String myJson;
        serializeJson(myDoc, myJson);
        myWebSocket.textAll(myJson);
    } 
    else if (myType == WS_EVT_DATA) {
        // Parse incoming JSON message from a client
        String myIncoming = "";
        for (size_t i = 0; i < myLength; i++) myIncoming += (char)myPayload[i];
        Serial.printf("Received: %s\n", myIncoming.c_str());

        StaticJsonDocument<200> myDoc;
        // The ESP32 client is now sending JSON with keys: myUserName, myMessage
        // We use the same keys here to be robust.
        DeserializationError myError = deserializeJson(myDoc, myIncoming);
        if (myError) {
            Serial.println("Invalid JSON");
            return;
        }

        // Use the custom keys from the client JS: myUserName and myMessage
        const char *myUser = myDoc["myUserName"];
        const char *myMsg = myDoc["myMessage"];
        
        if (myUser && myMsg) {
            // Re-package the message into the standard format (username, message)
            // that the Node.js client expects to receive on myWebSocket.onmessage
            StaticJsonDocument<200> myOutDoc;
            myOutDoc["username"] = myUser;
            myOutDoc["message"] = myMsg;
            String myOut;
            serializeJson(myOutDoc, myOut);
            
            // Broadcast the re-formatted JSON to all connected clients
            myWebSocket.textAll(myOut);
        }
    }
}

// --- Setup ---
void setup() {
    Serial.begin(115200);
    delay(3000);
    
    // Set up Soft Access Point
    WiFi.softAP(mySsid, myPassword);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP: "); Serial.println(myIP);

    // Initialize WebSocket handler
    myWebSocket.onEvent(myHandleWsEvent);
    myWebServer.addHandler(&myWebSocket);
    
    // Handle root path HTTP requests
    myWebServer.on("/", HTTP_GET, [](AsyncWebServerRequest *myRequest){
        myRequest->send_P(200, "text/html", myHtmlPage);
    });
    
    myWebServer.begin();
    Serial.println("WebSocket Chat Server started!");
}

// --- Loop ---
void loop() {
    // Nothing needed for Async server
}
