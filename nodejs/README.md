This fully works, but getting the hotspot to work when there is no internet is a challenge. 
I purchased a usb-WiFi adaptor which did help but it still was a pain [$20 USD  Amazon.com Archer T3U](https://www.amazon.ca/TP-Link-Archer-T3U-Adapter-Wireless/dp/B07P6N2TZH)

The best part of the nodeJS server is that someone without an esp32 can get a sense of how the system works without purchasing an esp32.

## Instructions







Download and unzip this repository on your local computer and navigate to the nodejs folder

in the nodejs folder right click and open a new terminal, NOTE: open CMD not powershell since for me powershell did not like my installing ```ws```
You might have to open CMD and navigate to the correct folder.

Test that both NodeJS and npm are installed

```
node -v
npm -v
```

Install ws

```
npm install ws
```

Run the node server, and record the IP it generates

```
node node-server.js
```

Using a browser http://localhost:8080 will generate a student-chat

```
localhost:8080
```

Open from your local download the teacher-derver.html file and use ws://localhost:8080
Note: You could load the student-socket.html file the same way but having the server generate it is much easier.


Now you can see how the system works. Load multiple students and one teacher, the teacher using the Chrome Built in AI ability.

For others to use the system you need to activate your computers hotspot and give others access to it the three values 1. SSID and 2. password Node Server IP

Students access the generated student-socket using the IP
```
http://#.#.#.#:8080
```

The teacher-socket can be loaded from a different Chrome Built in AI working computer but the page needs to be loaded locally and the login is slightly
different than the esp32

```



