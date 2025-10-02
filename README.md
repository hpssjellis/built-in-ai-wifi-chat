# built-in-ai-wifi-chat
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



Here is the link to the web-socket page. bst to download it when you have internet.

https://hpssjellis.github.io/built-in-ai-wifi-chat/web-socket.html




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

