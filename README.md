# Microcontroller Usage

This repository serves to demonstrate more hard to find/grasp/lesser talked about concepts in microcontroller development using Arduino. Concepts here work for Pi Pico, Esp32-SX and Arduino Uno unless specified otherwise.

Due to the closed source nature of my latest project with the Växjö Linnaeus Science Park, any descriptions used from it will only be posted here after being sufficiently decontextualized.

## Topics Covered
    1.0.................. Arduino OTA Updates
    2.0.................. ESP32 Image Capture
    3.0.................. String Precautions
    4.0.................. Networking
        4.1.............. Wi-Fi Connections
        4.2.............. Getting Correct Time
        4.3.............. Keeping COnnections


## 1.0. Arduino OTA Updates

The ability to update networked projects can take a normal IoT project to the next level by removing the need for disassembly and possible remounting during updates. 

This can be done a number of ways, though the general-purpose solution is using an HTTP server to retrieve the updated firmware.

There are a number of ways to specify this control flow, though the easiest is this:

    -> Board sends current firmware version number (as String) to server.
    -> Server checks against most updated version number.
        -> If update is needed, server sends HTTP_UPDATE_OK (200 OK) then firmware file.
        -> If no update is needed, server sends HTTP_UPDATE_NO_UPDATES (304 Not Modified).
    
    -> Update happens automatically and board restarts.
    -> If server error of any kind, the return value we see later is HTTP_UPDATE_FAILED.

One of the simplest ways to accomplish this is in [ota.cpp](ota.cpp):

```cpp
t_httpUpdate_return ret = httpUpdate.update(*CLIENT, HOST.toString(), PORT, "/", firmware_version); 
```

- CLIENT:-> Reference to WifiClient.
- HOST:-> Address of server.
- "/":-> Route on server to access. In the example here it's the root directory.

Here. "ret" will return (1) of the (3) values we specified earlier. We can handle this easily with a switch statement:

```cpp
switch (ret) {
    case HTTP_UPDATE_FAILED:
      debugf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      debugln("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      debugln("HTTP_UPDATE_OK");
      break;
  }
```
To reliably check for updates now, we first call this function in our setup before any other functionality whihc may break our ability to connect to our server, such as sensor initialization. Then, we call this at a given point in our main program loop. 


## 2.0. ESP32 Image Capture

The ESP32 consistently seems to be the best microcontroller for image capture/manipulation because of a number of factors:
- Larger (PS)RAM, starting at 8MB.
- Better vector instructions upwards from first gen boards.
- Compatibility & relatively good support for most camera sensors via DVP.



## 3.0. String Precautions
## 4.0. Networking
### 4.1. Wi-Fi Connection
### 4.2. Getting Correct Time
### 4.3. Keeping Connections

