// 1. We must 'require' the ws library and the built-in 'os' module
const myWebSocket = require('ws');
const myOS = require('os'); // Node's built-in OS module

// 2. We define the port (must match the client code)
const myPortNumber = 8080;
const myServer = new myWebSocket.Server({ port: myPortNumber });

// --- Helper Function to Find Local IP ---
/**
 * Scans the network interfaces to find the machine's local (non-internal) IP address.
 * This is the IP address clients will use to connect.
 */
function myGetLocalIPAddress() {
    const myInterfaces = myOS.networkInterfaces();
    for (const myName in myInterfaces) {
        for (const myInterface of myInterfaces[myName]) {
            // Skip over internal (127.0.0.1) and non-IPv4 addresses
            if (myInterface.family === 'IPv4' && !myInterface.internal) {
                // Return the first valid local IP found
                return myInterface.address;
            }
        }
    }
    // Fallback if no specific IP is found (this is usually the local host)
    return 'localhost';
}

// 3. Wait for a new client connection
myServer.on('connection', function myHandleClientConnect(myClient) {
    console.log('A new client has connected!');

    // 4. Handle incoming messages from that client
    myClient.on('message', async function myHandleIncomingMessage(myMessage) {
        console.log(`Received: ${myMessage}`);
        
        // 5. Broadcast the message to all OTHER connected clients
        myServer.clients.forEach(function myBroadcast(myOtherClient) {
            // Check if the client is open and not the sender
            if (myOtherClient.readyState === myWebSocket.OPEN && myOtherClient !== myClient) {
                myOtherClient.send(myMessage.toString());
            }
        });
    });
    
    // Simple way to handle a client disconnecting
    myClient.on('close', () => console.log('Client disconnected.'));
});

// --- Startup Message ---
const myLocalIP = myGetLocalIPAddress();
console.log(`\n======================================================`);
console.log(`| My chat server is running on port ${myPortNumber}!`);
console.log(`| To connect, use one of these addresses in your client code:`);
console.log(`| 1. Localhost (same computer): ws://localhost:${myPortNumber}`);
console.log(`| 2. Local Network (other devices): ws://${myLocalIP}:${myPortNumber}`);
console.log(`======================================================\n`);

// (You would then run this file using 'node myServer.js' in your terminal)
