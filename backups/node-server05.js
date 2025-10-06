// ... existing client-side script
        async function mySendMessage() {
            const myMessage = myMessageInput.value.trim();
            if (myMessage === "") { return; }
            
            // MODIFIED LINE: Get the current username directly from the input field
            const myCurrentUserName = myUserNameInput.value.trim(); 
            
            // PREPEND THE USER NAME
            const myFullMessage = `${myCurrentUserName}: ${myMessage}`; 
            
            if (myWebSocket && myWebSocket.readyState === WebSocket.OPEN) {
                myWebSocket.send(myFullMessage);
                // Display un-prefixed message locally
                myDisplayMessage(myMessage, 'user'); 
                myMessageInput.value = '';
            } else {
                myDisplaySystemMessage("Cannot send. Not connected to server.", 'error');
                console.warn("WebSocket is not open. ReadyState:", myWebSocket ? myWebSocket.readyState : 'Undefined');
            }
        }
// ... existing client-side script
