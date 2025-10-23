// 1. We must 'require' the necessary built-in and external libraries
const myWebSocket = require('ws');
const myOS = require('os');
const myHTTP = require('http'); // Node's built-in HTTP module

// 2. Define the port (used for both HTTP and WebSocket)
const myPortNumber = 8080;

// DEBUG: Print the network interfaces on startup for inspection
console.log("Detected network interfaces:", myOS.networkInterfaces());

// --- Helper Function to Find Local IP ---
function myGetLocalIPAddress() {
    const myInterfaces = myOS.networkInterfaces();
    for (const myName in myInterfaces) {
        for (const myInterface of myInterfaces[myName]) {
            if (myInterface.family === 'IPv4' && !myInterface.internal) {
                return myInterface.address;
            }
        }
    }
    return 'localhost';
}

// Get the local network IP once at startup
const myLocalIP = myGetLocalIPAddress();
const myLocalhostURL = `ws://localhost:${myPortNumber}`;
const myNetworkURL = `ws://${myLocalIP}:${myPortNumber}`;


// 3. Define the HTML content as a JavaScript multiline string (Template Literal)
// NOTE: This string now contains only HTML and minimal inline JS for static attributes.
const myChatClientHTML = `
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 JSON Chat</title>
<style>
body { margin:0; padding:0; background:#f9fafb; font-family:sans-serif; }
#myChatBox { width:100%; height:100vh; display:flex; flex-direction:column; align-items:center; justify-content:flex-start; padding:10px; box-sizing:border-box; }
h2 { color:#2563eb; margin-top: 0; }
#myMessages {width:95%; height:60vh; overflow:auto; border:1px solid #ccc; border-radius:6px; padding:5px; text-align:left; resize:both; }
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
    <input type="button" id="myPromptButton" value="Prompt" onclick="mySendMessage(true)">
</div>

<div id="myMessages"></div>
</div>

<script>
let myWebSocket;
let myName = "";

function myConnect() {
    const myUrl = `ws://${window.location.hostname}:8080`;
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
`;

// -----------------------------------------------------------------
//                         HTTP SERVER SETUP
// -----------------------------------------------------------------

// 5. Create a basic HTTP server
const myHTTPServer = myHTTP.createServer((myRequest, myResponse) => {
    if (myRequest.url === '/') {
        // Embed the network variables into the HTML string
        let myFinalHTML = myChatClientHTML.replace(/@@NETWORK_URL@@/g, myNetworkURL);
        myFinalHTML = myFinalHTML.replace(/@@LOCALHOST_URL@@/g, myLocalhostURL);
        
        // Append the client-side JavaScript wrapped in a <script> tag
        myFinalHTML += `<script>${myClientSideJS}</script></body></html>`;

        myResponse.writeHead(200, {'Content-Type': 'text/html'});
        myResponse.end(myFinalHTML);
        console.log('Served HTML client with embedded network address.');
    } else {
        myResponse.writeHead(404, {'Content-Type': 'text/plain'});
        myResponse.end('Not Found');
    }
});

// 6. Start the HTTP server listening on the port
myHTTPServer.listen(myPortNumber, () => {
    console.log(`\n======================================================`);
    console.log(`| My CHAT & WEB SERVER is running on port ${myPortNumber}!`);
    console.log(`| Open this URL in your browser: http://localhost:${myPortNumber}`);
    console.log(`| Local Network IP: ${myLocalIP}`);
    console.log(`======================================================\n`);
});

// -----------------------------------------------------------------
//                         WEBSOCKET SERVER SETUP
// -----------------------------------------------------------------

// 7. Create the WebSocket server, attaching it to the existing HTTP server
const myServer = new myWebSocket.Server({ server: myHTTPServer });

// 8. WebSocket event handlers 
myServer.on('connection', function myHandleClientConnect(myClient) {
    console.log('A new WebSocket client has connected!');

    myClient.on('message', async function myHandleIncomingMessage(myMessage) {
        const myMessageString = myMessage.toString();
        
        // 1. Extract the username and message content from the incoming string format: "Username: Message"
        const myMatch = myMessageString.match(/^([^:]+):\s*(.*)$/);
        let myUserName = 'System';
        let myMessageContent = myMessageString;

        if (myMatch) {
            myUserName = myMatch[1].trim();
            myMessageContent = myMatch[2].trim();
        }

        // 2. Create the JSON object
        const myDataToSend = {
            username: myUserName,
            message: myMessageContent
        };

        // 3. Convert the object to a JSON string
        const myJSONString = JSON.stringify(myDataToSend);
        
        // LOGIC CHANGE: Check for the [LLM] tag to identify the message type
        const myIsLLMPrompt = myMessageContent.includes('[LLM]');
        
        if (myIsLLMPrompt) {
             console.log(`Received LLM Prompt from ${myUserName} and Broadcasting JSON: ${myJSONString}`);
        } else {
             console.log(`Received Chat Message from ${myUserName} and Broadcasting JSON: ${myJSONString}`);
        }
        
        // 4. Broadcast the JSON string to all *other* clients
        myServer.clients.forEach(function myBroadcast(myOtherClient) {
            if (myOtherClient.readyState === myWebSocket.OPEN && myOtherClient !== myClient) {
                // The only change is sending the myJSONString instead of myMessageString
                myOtherClient.send(myJSONString); 
            }
        });
    });
    
    myClient.on('close', () => console.log('Client disconnected.'));
});
