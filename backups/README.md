
The main demo page is at 


https://hpssjellis.github.io/built-in-ai-wifi-chat/student-socket.html



## Node server

https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/node-server00.js   always the most stable node server typical te same as the main page  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/node-server01.js  Still kind of the best  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/node-server02.js  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/node-server03.js  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/node-server04.js  works  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/node-server05.js  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/node-server06.js   trying to get chat and LLM   
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/node-server07.js  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/node-server08.js Good new stable Oct 6th, 2025    
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/node-server09.js  Trying to open  ability for other computers to access 
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/node-server10.js     
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/node-server11.js     
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/node-server12.js       


## But these backups are also at for the student

https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/student-socket00.html    Typically the same as the main one and the most stable  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/student-socket01.html   
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/student-socket02.html    New name pre-entry  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/student-socket03.html    
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/student-socket04.html    
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/student-socket05.html    Good new stable OCt 6th, 2025
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/student-socket06.html    
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/student-socket07.html    
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/student-socket08.html    



## These backups are for the teacher with a laptop with Chrome Built in AI and serving the node server.

https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket00.html    Typically the same as the main one and the most stable  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket01.html    
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket02.html   wrong API  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket03.html   
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket04.html  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket05.html  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket06.html kind of OK  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket07.html  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket08.html     expecting JSON  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket09.html     Good new stable Oct 6, 2025  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket10.html     expecting JSON, not really what I thought it would do  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket11.html  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket12.html    trying to connect to the esp32  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket13.html   
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket14.html  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket15.html  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket16.html  works just cant send the LLM data back.  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket17.html  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket18.html  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/teacher-socket19.html    



## ESP32 wifi access and websocket server and student HTML generator with send and prompt

https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/esp32s3-webpage01.ino   proof of concept counter  

https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/esp32-access-web-socket02.ino  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/esp32-access-web-socket03.ino   Kind of fully works  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/esp32-access-web-socket04.ino  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/esp32-access-web-socket05.ino  
https://hpssjellis.github.io/built-in-ai-wifi-chat/backups/esp32-access-web-socket06.ino  


## Notes


Open Command Prompt as an Administrator (Search for cmd, right-click, and select "Run as administrator").

Type the following command and press Enter:
```
netsh wlan show drivers
```
Look for the line that says "Hosted network supported". If it says "Yes", you are good to go.

Set Up the Hosted Network:

In the same Command Prompt window, type the following command to set up your network name and password. Replace myHotspotName and myHotspotPassword with your desired name and a password (minimum 8 characters).
```
netsh wlan set hostednetwork mode=allow ssid=myHotspotName key=myHotspotPassword
```
Press Enter. You should see confirmation messages.

Start the Hosted Network:

Type the following command and press Enter:
```
netsh wlan start hostednetwork
```
This should create a local Wi-Fi network that devices can connect to for a LAN (local network) connection, even without internet access.

```



if installing ws has issues
```
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
npm install -g ws
```

then close the command or power shell window





## try this

Steps to Create and Use a Loopback Adapter:
Open Device Manager:

Search for "Device Manager" in the Start Menu and open it.

Add Legacy Hardware:

Click on your computer's name at the very top of the list.

Go to the Action menu and select "Add legacy hardware."

Manual Installation:

Click "Next" on the first screen.

Select "Install the hardware that I manually select from a list (Advanced)" and click "Next."

Select Network Adapter:

Choose "Network adapters" from the list and click "Next."

Install Loopback:

On the left side, select "Microsoft."

On the right side, select "Microsoft KM-TEST Loopback Adapter" (or just "Loopback Adapter").

Click "Next" twice and then "Finish."

Start Mobile Hotspot:

Go to Settings → Network & internet → Mobile hotspot.

In the "Share my internet connection from" dropdown, select the new adapter you just created (it might be listed as Ethernet 2 or a similar local connection).

Turn on the Mobile Hotspot.

Your Archer T3U should now broadcast a local-only Wi-Fi network that your students' devices can connect to for a local area network (LAN) connection.
