# built-in-ai-wifi-chat




[https://www.youtube.com/watch?v=NTYGkRdZ5Yw&list=PL57Dnr1H_egsvfrvJdJdeVPaHVmy87S0i&index=3&pp=iAQB
![built-in-ai-wifi-chat](https://img.youtube.com/vi/NTYGkRdZ5Yw/0.jpg)](https://www.youtube.com/watch?v=NTYGkRdZ5Yw&list=PL57Dnr1H_egsvfrvJdJdeVPaHVmy87S0i&index=3&pp=iAQB)


Update: Oct 12, 2025

1. We have moved the ESP32 code into it's own folder. That is the main code area that needs the ESP32 microcontroller

2. The nodeJS code has been moved to it's own folder and we will continue to develop it, however we had problems making a standalone hotspot on windows, while the esp32 solution worked immediately. The node code is now working but has several steps that need to be completed explained in the node folder



Note: For all methods to reduce CORS complaints from your browser any webpages should be loaded locally with then allows ```http``` connections. IF the page was loaded from the internet your browser would inforce ```https``` connections and the websocket would fail. To create a secure websocet ```wss``` that is more complex than we wanted to work with in this project.






.







.

<br><br>Github and WiFi Login<br>
<img width="1891" height="961" alt="wifi-SSID" src="https://github.com/user-attachments/assets/47b6633b-a4b5-4304-a566-8d73c079561b" />




<br><br>Arduino IDE passwords and Serial Monitor for the IP<br>
<img width="632" height="585" alt="arduino-serial-monitor" src="https://github.com/user-attachments/assets/8e230ed2-b353-4b7a-9816-97659ff8b1e4" />



<br><br>Student access<br>
<img width="968" height="704" alt="student-hello" src="https://github.com/user-attachments/assets/d44e9f3a-973e-4b0b-9382-7af693cebd7c" />


<br><br>HTTP connection (insecure)<br>
<img width="812" height="451" alt="not-secure" src="https://github.com/user-attachments/assets/5a22c258-b477-45e8-92db-a19306d05a06" />


<br><br>ws://#.#.#.#/ws<br>
<img width="323" height="365" alt="teacher-login" src="https://github.com/user-attachments/assets/cf5df16e-99e2-4e61-9dbb-f0beb0693fd0" />


<br><br>Teacher Connect<br>
<img width="1273" height="779" alt="teacher-start" src="https://github.com/user-attachments/assets/cf085d5e-22aa-43e5-9ca2-06dfb07e7441" />

<br><br>Pre-Prompt<br>
<img width="1376" height="720" alt="teacher-monitor" src="https://github.com/user-attachments/assets/53a53eac-f271-4043-b476-f5f198f4cb19" />

<br><br>Student Prompt<br>
<img width="1361" height="525" alt="student-prompt" src="https://github.com/user-attachments/assets/ef0d2f34-d292-4cc0-83ba-585dfc9478d3" />






.


.



.



.



.



.



.



.





## Old info

```
Download this repository
extract it
open the main folder that has the node-server.js file
right click Open windows terminal
node -v
npm -v
npm install -g ws
node node-server.js
load teacher-socket.html file

have students enter the url given in the node-server output, or give them the link to the student-socket.html file
students send prompts to the teacher-socket.html which sends back the LLM reply.

```









All AI chat using chrome built in AI and a mobile hotspot (hopefully without internet)

```
With internet working
Start the mobile hotspot
disconect your internet.
Set up the node server
access your client to the hotspot url
load the url or run websocket.html and enter the url ws://#.#.#.#:8080

Chat

```


Probably a good idea to have a [usb wifi adaptor](https://www.amazon.com/Adapter-Plug-n-Play-Antenna-Suitable-Windows11/dp/B0D1BJTGFR) so that the hotspot setup allows wifi without a working internet site.


Here is the link to the web-socket page. Best to download it when you have internet. The web server also generates a version of this also.

https://hpssjellis.github.io/built-in-ai-wifi-chat/student-socket.html




## I have not tested this yet.


Windows Mobile Hotspot is designed primarily to share an internet connection, which is why it often prevents you from starting it when no internet is available. Since you only want a local network, there are a few workarounds:

1. Install a Virtual Network Adapter
A common workaround is to create a virtual, non-physical network adapter that Windows can "share" as its internet source, thus tricking the Mobile Hotspot feature into activating.

Open Device Manager (right-click the Start button and select it).

Click on your computer name at the top of the list.

Go to the Action menu and select "Add legacy hardware."

In the wizard, choose "Install the hardware that I manually select from a list (Advanced)" and click Next.

Select "Network Adapters" and click Next.

On the left, select "Microsoft" and on the right, select "Microsoft KM-TEST Loopback Adapter" (you may have to wait for the list to populate).

Click Next and then Finish.

Open Settings and navigate to Network & Internet > Mobile hotspot.

Under "Share my internet connection from," select the new Loopback adapter you just created.

Turn on the Mobile hotspot toggle.

Since the Loopback adapter provides a valid "connection" to share, the hotspot should start, creating your local Wi-Fi network even without an external internet source.

2. Use Third-Party Hotspot Software
Alternatively, you can use third-party software designed to create Wi-Fi hotspots, as many of these applications don't have the same internet connection requirement as the built-in Windows feature.

A quick search may reveal options such as MyPublicWiFi or others available in the Microsoft Store that specifically allow you to create a hotspot for a local network only.

