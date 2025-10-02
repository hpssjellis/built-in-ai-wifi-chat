// 1. We must 'require' the necessary built-in and external libraries
const myWebSocket = require('ws');
const myOS = require('os');
const myHTTP = require('http'); // Node's built-in HTTP module
const myFS = require('fs');     // Node's built-in File System module
const myPath = require('path'); // Node's built-in Path module

// 2. Define the port (used for both HTTP and WebSocket)
const myPortNumber = 8080;

// --- Helper Function to Find Local IP (from original server) ---
/**
 * Scans the network interfaces to find the machine's local (non-internal) IP address.
 */
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

// -----------------------------------------------------------------
//                               HTTP SERVER SETUP
// -----------------------------------------------------------------

// 3. Create a basic HTTP server
const myHTTPServer = myHTTP.createServer((myRequest, myResponse) => {
    // We only care about the root path to serve the chat page
    if (myRequest.url === '/') {
        // Resolve the path to the HTML file (assuming the HTML is saved locally)
        const myFilePath = myPath.join(__dirname, 'chatClient.html');

        // Read the HTML file content asynchronously
        myFS.readFile(myFilePath, (myError, myData) => {
            if (myError) {
                // If the file isn't found, send a 404
                myResponse.writeHead(404, {'Content-Type': 'text/plain'});
                myResponse.end('Error: chatClient.html file not found.');
                console.error(`Error: Could not find HTML file at ${myFilePath}`);
                return;
            }

            // Success! Send the HTML content
            myResponse.writeHead(200, {'Content-Type': 'text/html'});
            myResponse.end(myData);
            console.log('Served chatClient.html to a client.');
        });
    } else {
        // For any other URL, send a 404
        myResponse.writeHead(404, {'Content-Type': 'text/plain'});
        myResponse.end('Not Found');
    }
});

// 4. Start the HTTP server listening on the port
myHTTPServer.listen(myPortNumber, () => {
    // This is where the startup message is printed
    const myLocalIP = myGetLocalIPAddress();
    console.log(`\n======================================================`);
    console.log(`| My chat server is running on port ${myPortNumber}!`);
    console.log(`| Open this URL in your browser: http://localhost:${myPortNumber}`);
    console.log(`| Or use this for other devices: http://${myLocalIP}:${myPortNumber}`);
    console.log(`======================================================\n`);
});

// -----------------------------------------------------------------
//                             WEBSOCKET SERVER SETUP
// -----------------------------------------------------------------

// 5. Create the WebSocket server, attaching it to the existing HTTP server
const myServer = new myWebSocket.Server({ server: myHTTPServer });

// 6. WebSocket event handlers (same logic as your original server)
myServer.on('connection', function myHandleClientConnect(myClient) {
    console.log('A new WebSocket client has connected!');

    // Handle incoming messages from that client
    myClient.on('message', async function myHandleIncomingMessage(myMessage) {
        console.log(`Received: ${myMessage}`);
        
        // Broadcast the message to all OTHER connected clients
        myServer.clients.forEach(function myBroadcast(myOtherClient) {
            if (myOtherClient.readyState === myWebSocket.OPEN && myOtherClient !== myClient) {
                myOtherClient.send(myMessage.toString());
            }
        });
    });
    
    // Simple way to handle a client disconnecting
    myClient.on('close', () => console.log('Client disconnected.'));
});

// -----------------------------------------------------------------
//                               REQUIRED SETUP
// -----------------------------------------------------------------

// **IMPORTANT**: You must save the second file you provided as 'chatClient.html'
// in the same directory as this Node.js file.
