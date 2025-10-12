// mySignalingServer.js
const myWebSocketServer = require('ws').Server;
const myHttpServer = require('http').createServer();

// Create a WebSocket server on port 8080
const myWSS = new myWebSocketServer({ server: myHttpServer });

myWSS.on('connection', function myConnection(myWebSocket) {
    console.log('A client connected for signaling.');

    myWebSocket.on('message', function myRelayMessage(myMessage) {
        // Relay the message to all other connected clients
        myWSS.clients.forEach(function myClientSend(myClient) {
            if (myClient !== myWebSocket && myClient.readyState === myWebSocketServer.OPEN) {
                myClient.send(myMessage);
                console.log('Relaying:', myMessage.toString());
            }
        });
    });

    myWebSocket.on('close', function myClose() {
        console.log('A client disconnected from signaling.');
    });
});

// Start the HTTP server to listen for WebSocket connections
const myPort = 8080; // This must match the port in the HTML file's JavaScript
myHttpServer.listen(myPort, function myServerStart() {
    console.log(`Signaling Server listening on port ${myPort}`);
});
