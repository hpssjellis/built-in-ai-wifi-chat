
For this project to work you need an ESP32 with the older board version V2X  I use V2.0.15 you need to downgrade V3 useing the Arduino IDE Board Manager.

The best version in my Opinion is the XIAOML kit by seeedstudion for $38 USD at https://www.seeedstudio.com/The-XIAOML-Kit.html but the barebones that are needed is the XIAO esp32S3 for &7.49 USD https://www.seeedstudio.com/XIAO-ESP32S3-p-5627.html but if you can get the full set.


Include files will also have to be loaded using the library manager. 

```
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
```
I may have downloaded the ESPAsyncWebServer from it's github but that might not be relevant.

On compile (hold boot button when plugging in the esp32). You will need to do a full restart (pull out and in the USB-C cable) and open the Serial Monitor to get a printout of the IP of the Assess point you have created. 


The first lines of code set the SSID and your password for the ESP23

```
// --- Soft AP Credentials ---
const char *mySsid = "myChatAP88";
const char *myPassword = "myPassword";
```
Students will have to connect their cell phones or laptops to that hot spot you created (It might give warnings of no internet). then students type into a web browser the IP given by the esp32 and it should auto load the webSocket webpage. Students write a first name and connect (which should be very fast to connect if everything is correct)

Students send chats or prompts and everyone on the private local WiFi network can see all the chats and prompt replies.


Teacher from a laptop loads the teacher socket and write the ESP32 IP in the form ```ws://###.###.###.###/ws``` not localhost since the esp32 is not part of the laptop. When the teacher connects and has Chrome Built-in-AI working then the chat reponsds to LLM prompts.






