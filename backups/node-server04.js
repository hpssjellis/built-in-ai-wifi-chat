// 1. We must 'require' the necessary built-in and external libraries
const myWebSocket = require('ws');
const myOS = require('os');
const myHTTP = require('http'); // Node's built-in HTTP module

// 2. Define the port (used for both HTTP and WebSocket)
const myPortNumber = 8080;

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
// NOTE: We now use unique placeholders (@@...@@) for all dynamic values.
const myChatClientHTML = `<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>My Flexible Offline Chat Client</title>
    
    <style>
        /* CSS is unchanged */
        body { font-family: Arial, sans-serif; margin: 0; background-color: #f3f4f6; }
        #myContainer { width: 95%; max-width: 500px; margin: 0 auto; padding-top: 10px; padding-bottom: 90px; }
        h1 { font-size: 24px; font-weight: bold; margin-bottom: 8px; text-align: center; color: #4f46e5; }
        .status-text { text-align: center; margin-bottom: 15px; font-size: 14px; font-weight: 500; }
        .my-server-select-area { display: flex; flex-direction: column; gap: 10px; padding: 10px; margin-bottom: 15px; background-color: #fff; border-radius: 8px; box-shadow: 0 1px 3px rgba(0,0,0,0.1); }
        .my-server-select-area select, .my-server-select-area input, .my-server-select-area button { padding: 10px; border: 1px solid #ccc; border-radius: 6px; }
        #myConnectButton { background-color: #10b981; color: white; font-weight: 600; border: none; cursor: pointer; transition: background-color 0.2s; }
        #myConnectButton:hover { background-color: #059669; }
        #myConnectButton:disabled { opacity: 0.5; cursor: not-allowed; }
        #myChatLog { background-color: white; padding: 15px; border-radius: 8px; border: 1px solid #e5e7eb; box-shadow: 0 1px 3px rgba(0,0,0,0.1); height: calc(100vh - 250px); overflow-y: auto; scroll-behavior: smooth; }
        .my-input-area { position: fixed; bottom: 0; left: 0; right: 0; background-color: white; padding: 10px; box-shadow: 0 -2px 5px rgba(0,0,0,0.1); display: flex; gap: 8px; width: 100%; }
        .my-input-area > div { max-width: 500px; margin: 0 auto; }
        #myMessageInput { flex-grow: 1; padding: 10px; border: 1px solid #ccc; border-radius: 8px; font-size: 16px; }
        #mySendButton { background-color: #4f46e5; color: white; font-weight: 600; padding: 10px 20px; border: none; border-radius: 8px; cursor: pointer; transition: background-color 0.2s; }
        #mySendButton:hover:not(:disabled) { background-color: #4338ca; }
        #mySendButton:disabled { opacity: 0.5; cursor: not-allowed; }
        .my-message-bubble { margin-bottom: 12px; display: flex; max-width: 80%; }
        .message-content { padding: 10px; border-radius: 12px; box-shadow: 0 1px 2px rgba(0,0,0,0.1); }
        .message-time { font-size: 10px; opacity: 0.7; margin-top: 4px; display: block; }
        .my-message-user { justify-content: flex-end; margin-left: auto; }
        .my-message-user .message-content { background-color: #6366f1; color: white; border-bottom-right-radius: 0; }
        .my-message-user .message-time { text-align: right; }
        .my-message-other { justify-content: flex-start; }
        .my-message-other .message-content { background-color: #e5e7eb; color: #1f2937; border-bottom-left-radius: 0; }
        .my-message-other .message-time { text-align: left; }
        .my-message-system { text-align: center; font-size: 12px; margin-bottom: 12px; font-weight: 500; }
        .my-message-system.error { color: #ef4444; }
        .my-message-system.system { color: #6b7280; }
        .status-green { color: #10b981; }
        .status-red { color: #ef4444; }
        .status-yellow { color: #f59e0b; }
    </style>
</head>
<body>

    <div id="myContainer">
        <h1>Local Chat</h1>
        <p id="myConnectionStatus" class="status-text status-red">
            Status: Disconnected. Select an address.
        </p>

        <div class="my-server-select-area">
            <label for="myServerSelect" style="font-weight: 600; font-size: 14px;">Server Address:</label>
            <select id="myServerSelect" onchange="myHandleSelectChange()">
                <option value="" disabled selected>--- Select Server Option ---</option>
                <option value="@@LOCALHOST_URL@@">1. Localhost: @@LOCALHOST_URL@@</option>
                <option value="@@NETWORK_URL@@">2. Local Network: @@NETWORK_URL@@</option>
                <option value="custom">3. Enter Custom Address Below</option>
            </select>

            <input 
                type="text" 
                id="myCustomInput" 
                placeholder="Enter custom ws://address:port" 
                style="display: none;"
                onchange="myHandleCustomInputChange()"
            />
            
            <button 
                id="myConnectButton" 
                onclick="myInitWebSocket()"
                disabled
            >
                Connect
            </button>
        </div>

        <div id="myChatLog">
            </div>
    </div>

    <div class="my-input-area">
        <div style="display:flex; width: 100%; max-width: 500px; margin: 0 auto; gap: 8px;">
            <input 
                type="text" 
                id="myMessageInput" 
                placeholder="Type your message..."
                onkeypress="if(event.key === 'Enter') mySendMessage();"
                disabled
            />
            <button 
                id="mySendButton"
                onclick="mySendMessage()"
                disabled
            >
                Send
            </button>
        </div>
    </div>

    <script>
        // --- My Core Variables (Unchanged) ---
        let myWebSocket;
        let myServerUrl = ''; 
        
        const myChatLog = document.getElementById('myChatLog');
        const myMessageInput = document.getElementById('myMessageInput');
        const mySendButton = document.getElementById('mySendButton');
        const myConnectionStatus = document.getElementById('myConnectionStatus');
        const myServerSelect = document.getElementById('myServerSelect');
        const myCustomInput = document.getElementById('myCustomInput');
        const myConnectButton = document.getElementById('myConnectButton');

        const myMaxRetries = 5;
        let myRetryCount = 0;

        // --- UI Logic Functions (Unchanged) ---

        function myHandleSelectChange() {
            const mySelectedValue = myServerSelect.value;
            
            if (mySelectedValue === 'custom') {
                myCustomInput.style.display = 'block';
                myServerUrl = myCustomInput.value.trim();
                myConnectButton.disabled = !myServerUrl;
            } else if (mySelectedValue) {
                myCustomInput.style.display = 'none';
                myServerUrl = mySelectedValue;
                myConnectButton.disabled = false;
            } else {
                myCustomInput.style.display = 'none';
                myServerUrl = '';
                myConnectButton.disabled = true;
            }
        }

        function myHandleCustomInputChange() {
            myServerUrl = myCustomInput.value.trim();
            myConnectButton.disabled = !myServerUrl;
        }

        // --- Core WebSocket Functions (Unchanged) ---

        async function myInitWebSocket() {
            if (!myServerUrl) {
                myDisplaySystemMessage("Please select or enter a server address.", 'error');
                return;
            }
            
            if (myWebSocket && myWebSocket.readyState !== WebSocket.CLOSED) {
                myWebSocket.close(1000, 'Reconnecting');
            }

            myConnectionStatus.textContent = "Status: Connecting to " + myServerUrl + "...";
            myConnectionStatus.classList.remove('status-green', 'status-red', 'status-yellow');
            myConnectionStatus.classList.add('status-yellow');
            myConnectButton.disabled = true; 

            try {
                myWebSocket = new WebSocket(myServerUrl);

                myWebSocket.onopen = async () => {
                    console.log("WebSocket connected successfully.");
                    myConnectionStatus.textContent = "Status: Connected and ready to chat!";
                    myConnectionStatus.classList.remove('status-red', 'status-yellow');
                    myConnectionStatus.classList.add('status-green');
                    mySendButton.disabled = false;
                    myMessageInput.disabled = false;
                    myConnectButton.disabled = false; 
                    myRetryCount = 0; 
                    myDisplaySystemMessage("Welcome to the local chat!", 'system');
                };

                myWebSocket.onmessage = async (myEvent) => {
                    myDisplayMessage(myEvent.data, 'other');
                };

                myWebSocket.onerror = async (myError) => {
                    console.error("WebSocket Error:", myError);
                    myConnectionStatus.textContent = "Status: Connection error.";
                    myConnectionStatus.classList.remove('status-green', 'status-yellow');
                    myConnectionStatus.classList.add('status-red');
                    mySendButton.disabled = true;
                    myMessageInput.disabled = true;
                };

                myWebSocket.onclose = async (myEvent) => {
                    mySendButton.disabled = true;
                    myMessageInput.disabled = true;
                    if (myEvent.wasClean) {
                        myConnectionStatus.textContent = \`Status: Disconnected (Code: \${myEvent.code}). Click Connect to restart.\`;
                        myConnectButton.disabled = false;
                    } else {
                        myConnectionStatus.textContent = "Status: Connection lost! Trying to reconnect...";
                        myConnectionStatus.classList.add('status-yellow');
                        await myHandleReconnect();
                    }
                };

            } catch (myError) {
                console.error("Failed to create WebSocket instance:", myError);
                myConnectionStatus.textContent = "Status: Failed to initialize. Check console.";
                myConnectButton.disabled = false;
                mySendButton.disabled = true;
                myMessageInput.disabled = true;
            }
        }

        async function myHandleReconnect() {
            if (myRetryCount < myMaxRetries) {
                myRetryCount++;
                const myDelay = Math.pow(2, myRetryCount) * 1000;
                console.log(\`Attempting reconnection in \${myDelay / 1000} seconds. (Attempt \${myRetryCount}/\${myMaxRetries})\`);
                myConnectionStatus.textContent = \`Status: Reconnecting in \${myDelay / 1000}s...\`;
                
                await new Promise(myResolve => setTimeout(myResolve, myDelay));
                myInitWebSocket();
            } else {
                myConnectionStatus.textContent = "Status: Reconnection failed after multiple attempts. Click Connect.";
                myConnectionStatus.classList.remove('status-yellow');
                myConnectionStatus.classList.add('status-red');
                myConnectButton.disabled = false;
            }
        }

        async function mySendMessage() {
            const myMessage = myMessageInput.value.trim();
            if (myMessage === "") { return; }
            if (myWebSocket && myWebSocket.readyState === WebSocket.OPEN) {
                myWebSocket.send(myMessage);
                myDisplayMessage(myMessage, 'user');
                myMessageInput.value = '';
            } else {
                myDisplaySystemMessage("Cannot send. Not connected to server.", 'error');
                console.warn("WebSocket is not open. ReadyState:", myWebSocket ? myWebSocket.readyState : 'Undefined');
            }
        }

        function myDisplayMessage(myText, myType) {
            const myNewMessageDiv = document.createElement('div');
            myNewMessageDiv.classList.add('my-message-bubble', \`my-message-\${myType}\`);
            const myTime = new Date().toLocaleTimeString('en-US', { hour: '2-digit', minute: '2-digit' });
            myNewMessageDiv.innerHTML = \`
                <div class="message-content">
                    <span class="block">\${myText}</span>
                    <span class="message-time">\${myTime}</span>
                </div>
            \`;
            myChatLog.appendChild(myNewMessageDiv);
            myChatLog.scrollTop = myChatLog.scrollHeight;
        }

        function myDisplaySystemMessage(myText, myType) {
            const mySystemDiv = document.createElement('div');
            mySystemDiv.classList.add('my-message-system', myType);
            mySystemDiv.textContent = \`--- \${myText} ---\`;
            myChatLog.appendChild(mySystemDiv);
            myChatLog.scrollTop = myChatLog.scrollHeight;
        }

        // --- Initialization (Unchanged) ---
        document.addEventListener('DOMContentLoaded', () => {
             myHandleSelectChange();
        });

    </script>
</body>
</html>`;

// -----------------------------------------------------------------
//                               HTTP SERVER SETUP
// -----------------------------------------------------------------

// 4. Create a basic HTTP server
const myHTTPServer = myHTTP.createServer((myRequest, myResponse) => {
    // Only handle requests for the root path
    if (myRequest.url === '/') {
        // 🚨 CORRECT DYNAMIC INSERTION: 
        // 1. Replace the network IP placeholder
        let myFinalHTML = myChatClientHTML.replace(/@@NETWORK_URL@@/g, myNetworkURL);
        
        // 2. Replace the localhost placeholder
        myFinalHTML = myFinalHTML.replace(/@@LOCALHOST_URL@@/g, myLocalhostURL);
        
        // Send the generated HTML content
        myResponse.writeHead(200, {'Content-Type': 'text/html'});
        myResponse.end(myFinalHTML);
        console.log('Served HTML client with CORRECT embedded network address.');
    } else {
        // For any other URL, send a 404
        myResponse.writeHead(404, {'Content-Type': 'text/plain'});
        myResponse.end('Not Found');
    }
});

// 5. Start the HTTP server listening on the port
myHTTPServer.listen(myPortNumber, () => {
    // This is where the startup message is printed
    console.log(`\n======================================================`);
    console.log(`| My CHAT & WEB SERVER is running on port ${myPortNumber}!`);
    console.log(`| Open this URL in your browser: http://localhost:${myPortNumber}`);
    console.log(`| Local Network IP: ${myLocalIP}`);
    console.log(`======================================================\n`);
});

// -----------------------------------------------------------------
//                             WEBSOCKET SERVER SETUP (Unchanged)
// -----------------------------------------------------------------

// 6. Create the WebSocket server, attaching it to the existing HTTP server
const myServer = new myWebSocket.Server({ server: myHTTPServer });

// 7. WebSocket event handlers 
myServer.on('connection', function myHandleClientConnect(myClient) {
    console.log('A new WebSocket client has connected!');

    myClient.on('message', async function myHandleIncomingMessage(myMessage) {
        console.log(`Received: ${myMessage}`);
        
        myServer.clients.forEach(function myBroadcast(myOtherClient) {
            if (myOtherClient.readyState === myWebSocket.OPEN && myOtherClient !== myClient) {
                myOtherClient.send(myMessage.toString());
            }
        });
    });
    
    myClient.on('close', () => console.log('Client disconnected.'));
});
