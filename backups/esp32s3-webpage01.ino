/*
  WiFiAccessPoint.ino - Complete Chat Code for ESP32S3
  Architecture: ESP32 relays ONLY the latest message; JS maintains history in Local Storage.
*/

#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiAP.h>
#include <ArduinoJson.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2 
#endif

// Set these to your desired credentials.
const char *mySsid = "myChatAP88";
const char *myPassword = "myPassword";

// *** Globals: ESP32 only stores the LATEST message data ***
String myCurrentJson = "{\"user\":\"System\",\"message\":\"Welcome! The server will relay the newest message.\" }";
String myCurrentUser = "System"; 
String myCurrentMessage = "Welcome! The server will relay the newest message.";

// Number of seconds before the page auto-refreshes.
const int myRefreshTimeSeconds = 10; 

NetworkServer myServer(80);

// Function to safely extract a URL parameter value (remains the same)
String myGetUrlParameterValue(String myRequest, String myParamName) {
    String mySearchString = myParamName + "=";
    int myStartIndex = myRequest.indexOf(mySearchString);

    if (myStartIndex == -1) {
        return ""; 
    }

    myStartIndex += mySearchString.length();
    
    int myEndIndex = myRequest.indexOf('&', myStartIndex);
    if (myEndIndex == -1) {
        myEndIndex = myRequest.indexOf(' ', myStartIndex);
    }

    if (myEndIndex == -1) {
        return "";
    }

    String myValue = myRequest.substring(myStartIndex, myEndIndex);
    
    // Replace URL-encoded characters
    myValue.replace('+', ' '); 
    myValue.replace("%20", " ");
    myValue.replace("%21", "!");
    myValue.replace("%2C", ","); 
    myValue.replace("%3F", "?");
    
    return myValue;
}

// Function to handle the incoming HTTP request and send the HTML page
void mySendWebPage(NetworkClient myClient) {
    // --- START of HTTP Response Headers ---
    myClient.println("HTTP/1.1 200 OK");
    myClient.println("Content-type:text/html");
    myClient.print("Refresh: ");
    myClient.println(myRefreshTimeSeconds);
    myClient.println(); 

    // --- START of HTML Content ---
    myClient.println("<!DOCTYPE html><html><head>");
    myClient.print("<meta http-equiv=\"refresh\" content=\"");
    myClient.print(myRefreshTimeSeconds);
    myClient.println("\">");
    myClient.println("<title>ESP32 Shared History Chat</title>");
    
    // Minimal inline CSS
    myClient.println("<style>body{font-family: Arial;} .chatbox{border: 2px solid #333; padding: 10px; min-height: 50px; background-color: #f0f0f0;} input[type=text]{width: 35%; padding: 10px; margin: 8px 0; box-sizing: border-box;} label{font-weight: bold;} #myMessageLog{width: 90%; height: 200px; padding: 10px; background-color: #eee;} </style>");
    myClient.println("</head><body>");

    myClient.println("<h2>ESP32 Simple Chat (Client History)</h2>");
    myClient.print("<p>Page refreshes every ");
    myClient.print(myRefreshTimeSeconds);
    myClient.println("s.</p>");
    
    // *** NEW: Hidden field to pass the LATEST JSON from the server to JavaScript ***
    myClient.print("<input type='hidden' id='myLatestJson' value='");
    myClient.print(myCurrentJson); 
    myClient.println("'>");
    
    // All Messages Log Text Area 
    myClient.println("<h3>Message Log (Shared History)</h3>");
    myClient.println("<textarea id='myMessageLog' readonly></textarea>");
    myClient.println("<hr>");

    // *** THE MESSAGE SENDING FORM ***
    myClient.println("<form action='/' method='get' id='myForm' onsubmit='return myProcessMessage()'>");
    
    // Input for Username
    myClient.println("  <label for='myUser'>User:</label>");
    myClient.print("  <input type='text' id='myUser' name='myUser' value='");
    myClient.print(myCurrentUser); 
    myClient.println("' required>");
    myClient.println("  <br>");
    
    // Input for Message (Draft)
    myClient.println("  <label for='myMessage'>Message:</label>");
    myClient.println("  <input type='text' id='myMessage' name='myMessage' placeholder='Type your message here...' required>");
    myClient.println("  <input type='submit' value='Send Message' style='padding: 10px;'>");
    myClient.println("</form>");

    // *** JAVASCRIPT FOR LOCAL STORAGE & HISTORY SYNC ***
    myClient.println("<script>");
    
    // Local Storage Keys
    myClient.println("const myUserKey = 'myChatUserName';");
    myClient.println("const myDraftKey = 'myChatDraftMessage';");
    myClient.println("const myLogKey = 'myChatMessageLog';");
    myClient.println("const myLastMessageKey = 'myLastReceivedMessage';"); // Tracks the last received message

    // Function to process user input before sending to server
    myClient.println("function myProcessMessage() {");
    myClient.println("  const myUserNameInput = document.getElementById('myUser');");
    myClient.println("  const myMessageInput = document.getElementById('myMessage');");
    
    // Save current user and clear draft message on successful send
    myClient.println("  localStorage.setItem(myUserKey, myUserNameInput.value);");
    myClient.println("  localStorage.removeItem(myDraftKey);"); 
    
    myClient.println("  return true; // Allow the form submission (redirect) to proceed");
    myClient.println("}");

    // Function to add a new message string to the log and save to Local Storage
    myClient.println("function myAddMessageToLog(myEntry) {");
    myClient.println("  const myLogArea = document.getElementById('myMessageLog');");
    myClient.println("  let myLog = localStorage.getItem(myLogKey) || '';");
    myClient.println("  if(myLog.length > 0) { myLog += '\\n'; }"); 
    myClient.println("  myLog += myEntry;");
    myClient.println("  localStorage.setItem(myLogKey, myLog);");
    myClient.println("  myLogArea.value = myLog;");
    myClient.println("  myLogArea.scrollTop = myLogArea.scrollHeight; // Scroll to bottom");
    myClient.println("}");
    
    // Logic to sync history on page load
    myClient.println("const mySyncHistory = () => {");
    myClient.println("  const myLatestJsonElement = document.getElementById('myLatestJson');");
    myClient.println("  const myNewMessageJson = myLatestJsonElement.value;");
    
    // 1. Check if this message is new compared to the last one we added to the log
    myClient.println("  const myLastMessageAdded = localStorage.getItem(myLastMessageKey);");
    
    myClient.println("  if (myNewMessageJson !== myLastMessageAdded) {");
    myClient.println("    try {");
    // Parse the JSON string from the server
    myClient.println("      const myMessageData = JSON.parse(myNewMessageJson);");
    myClient.println("      if (myMessageData.user && myMessageData.message) {");
    // Format the new entry
    myClient.println("        const myTime = new Date().toLocaleTimeString();");
    myClient.println("        const myNewEntry = myTime + ' - ' + myMessageData.user + ': ' + myMessageData.message;");
    
    // 2. Add the message to the log
    myClient.println("        myAddMessageToLog(myNewEntry);");
    
    // 3. Update the tracking key to prevent duplicates on the next refresh
    myClient.println("        localStorage.setItem(myLastMessageKey, myNewMessageJson);");
    myClient.println("      }");
    myClient.println("    } catch (e) {");
    myClient.println("      console.error('Failed to parse JSON:', e);");
    myClient.println("    }");
    myClient.println("  }");

    // Load User (persistence)
    myClient.println("  const myUserNameInput = document.getElementById('myUser');");
    myClient.println("  const mySavedUser = localStorage.getItem(myUserKey);");
    myClient.println("  if (mySavedUser) { myUserNameInput.value = mySavedUser; }");
    
    // Load Draft Message (persistence)
    myClient.println("  const myMessageInput = document.getElementById('myMessage');");
    myClient.println("  const mySavedDraft = localStorage.getItem(myDraftKey);");
    myClient.println("  if (mySavedDraft) { myMessageInput.value = mySavedDraft; }");
    
    // Add event listener to save draft message every time the user types
    myClient.println("  myMessageInput.addEventListener('input', () => {");
    myClient.println("    localStorage.setItem(myDraftKey, myMessageInput.value);");
    myClient.println("  });");

    // Initialize the log area with existing data
    myClient.println("  const myLogArea = document.getElementById('myMessageLog');");
    myClient.println("  const mySavedLog = localStorage.getItem(myLogKey);");
    myClient.println("  if (mySavedLog) {");
    myClient.println("    myLogArea.value = mySavedLog;");
    myClient.println("    myLogArea.scrollTop = myLogArea.scrollHeight;");
    myClient.println("  }");
    
    myClient.println("};");
    
    // Run the sync function immediately
    myClient.println("mySyncHistory();");
    
    myClient.println("</script>");
    
    myClient.println("</body></html>");
    myClient.println(); 
}

// Function to handle the HTTP 302 Redirect (remains the same)
void mySendRedirect(NetworkClient myClient, String myLocation) {
    myClient.println("HTTP/1.1 302 Found"); 
    myClient.print("Location: ");
    myClient.println(myLocation); 
    myClient.println("Connection: close");
    myClient.println();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); 

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  if (!WiFi.softAP(mySsid, myPassword)) {
    log_e("Soft AP creation failed.");
    while (true);
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  myServer.begin();

  Serial.println("Server started");
}

void loop() {
  NetworkClient myClient = myServer.accept(); 

  if (myClient) {
    Serial.println("New Client.");
    String myCurrentLine = "";
    String myRequestLine = "";
    bool myMessageReceived = false; 
    
    while (myClient.connected()) {
      if (myClient.available()) {
        char myChar = myClient.read();
        Serial.write(myChar);
        if (myChar == '\n') {

          if (myCurrentLine.length() == 0) {
            
            // 1. EXTRACT USER AND MESSAGE FROM URL
            String myIncomingUser = myGetUrlParameterValue(myRequestLine, "myUser");
            String myIncomingMessage = myGetUrlParameterValue(myRequestLine, "myMessage");
            
            if (myIncomingMessage.length() > 0 && myIncomingUser.length() > 0) {
              myMessageReceived = true; 
              
              // 2. SERVER UPDATES ITS LATEST MESSAGE (to be relayed to ALL clients)
              StaticJsonDocument<200> myJsonDoc; 
              myJsonDoc["user"] = myIncomingUser;
              myJsonDoc["message"] = myIncomingMessage;
              serializeJson(myJsonDoc, myCurrentJson);

              // 3. Update the global display variables (for the sender's retained user name)
              myCurrentUser = myIncomingUser;
              myCurrentMessage = myIncomingMessage;
              
              Serial.print("--- NEW JSON RECEIVED: ");
              Serial.println(myCurrentJson);
            }
            
            // 4. DECIDE ACTION: REDIRECT or SEND PAGE
            if (myMessageReceived) {
                mySendRedirect(myClient, "/");
            } else {
                mySendWebPage(myClient);
            }

            break; 

          } else { 
            if (myRequestLine == "") {
                myRequestLine = myCurrentLine;
            }
            myCurrentLine = "";
          }
        } else if (myChar != '\r') { 
          myCurrentLine += myChar;
        }
      }
    }
    // close the connection:
    myClient.stop(); 
    Serial.println("Client Disconnected.");
  }
}
