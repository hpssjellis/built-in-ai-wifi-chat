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

// --- HTML Page ---
const char *myHtmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 JSON Chat</title>
<style>
body { font-family: sans-serif; background: #f9fafb; text-align:center; padding: 20px; }
#myChatBox { width:95%; max-width:400px; margin:auto; background:white; border-radius:12px; box-shadow:0 4px 6px rgba(0,0,0,0.1); padding:10px; }
#myMessages { height:300px; overflow-y:auto; border:1px solid #ccc; padding:5px; border-radius:6px; text-align:left; }
.myName { font-weight:bold; color:#2563eb; }
.mySystem { color:#6b7280; font-size:12px; text-align:center; margin:4px 0; }
</style>
</head>
<body>
<div id="myChatBox">
<h2>ESP32 JSON Chat</h2>
<p>Status: <span id="myStatus" style="color:red;">Disconnected</span></p>
<input id="myNameInput" placeholder="Your name" style="width:95%; padding:6px; margin:4px;"><br>
<input id="myMessageInput" placeholder="Type a message..." style="width:75%; padding:6px;">
<input type="button" value="Send" onclick="mySendMessage()" style="padding:6px;">

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
                myDisplayMessage(myData.username, myData.message);
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

function mySendMessage() {
    if (!myWebSocket || myWebSocket.readyState !== WebSocket.OPEN) return;
    if (!myName) myName = document.getElementById('myNameInput').value.trim();
    const myMessage = document.getElementById('myMessageInput').value.trim();
    if (myName && myMessage) {
        const myJson = JSON.stringify({ username: myName, message: myMessage });
        myWebSocket.send(myJson);
        document.getElementById('myMessageInput').value = "";
    }
}

function myDisplayMessage(name, msg) {
    const myMessages = document.getElementById('myMessages');
    const myDiv = document.createElement('div');
    myDiv.innerHTML = `<span class="myName">${name}:</span> ${msg}`;
    myMessages.appendChild(myDiv);
    myMessages.scrollTop = myMessages.scrollHeight;
}

function myDisplaySystemMessage(msg) {
    const myMessages = document.getElementById('myMessages');
    const myDiv = document.createElement('div');
    myDiv.className = "mySystem";
    myDiv.textContent = "--- " + msg + " ---";
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
        StaticJsonDocument<100> myDoc;
        myDoc["system"] = "A new client has joined.";
        String myJson;
        serializeJson(myDoc, myJson);
        myWebSocket.textAll(myJson);
    } 
    else if (myType == WS_EVT_DISCONNECT) {
        Serial.printf("Client #%u disconnected\n", myClient->id());
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
        DeserializationError myError = deserializeJson(myDoc, myIncoming);
        if (myError) {
            Serial.println("Invalid JSON");
            return;
        }

        const char *myUser = myDoc["username"];
        const char *myMsg = myDoc["message"];
        if (myUser && myMsg) {
            StaticJsonDocument<200> myOutDoc;
            myOutDoc["username"] = myUser;
            myOutDoc["message"] = myMsg;
            String myOut;
            serializeJson(myOutDoc, myOut);
            myWebSocket.textAll(myOut);
        }
    }
}

// --- Setup ---
void setup() {
    Serial.begin(115200);
    delay(3000);
    WiFi.softAP(mySsid, myPassword);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP: "); Serial.println(myIP);

    myWebSocket.onEvent(myHandleWsEvent);
    myWebServer.addHandler(&myWebSocket);
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
