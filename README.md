# WifiParMarker

When powered ON, the TTGO T-Display board automatically connects to the **iotroam** network and displays its assigned IP address on the LCD screen.

The IP address must then be entered in the desktop or VR headset acting as the TCP client. 
Once the TCP client establishes a connection, the board can send/receive data over TCP protocol, enabling communication through a WebSocket connection.

![WifiParMarker](media/board.JPG "WifiParMarker")

> [!IMPORTANT]
> Make sure to enter the correct password for iotroam network in the Main Sketch before uploading it to the board.

> For more information about using GitHub, see the [SOLO GitHub User Guide](https://solo-fsw-projects.github.io/).
