# Microcontroller Usage - Notes

This repository serves to demonstrate more hard to find/grasp/lesser talked about concepts in microcontroller development using Arduino. Concepts here work for Pi Pico, Esp32-SX and Arduino Uno unless specified otherwise.

Due to the closed source nature of my latest project with the Växjö Linnaeus Science Park, any descriptions used from it will only be posted here after being sufficiently decontextualized.

## Contents
    1.0.................. Arduino OTA Updates

    2.0.................. ESP32 Image Capture
        2.1.............. Camera Initialization
        2.2.............. Camera Buffer
        2.3............... Camera de-init

    3.0.................. String Precautions

    4.0.................. Networking
        4.1.............. Wi-Fi Connections
        4.2.............. Getting Correct Time
        4.3.............. Keeping Connections

    5.0.................. Sensor Reading


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


## 2.0. ESP32-SX Image Capture

The ESP32 consistently seems to be the best microcontroller for image capture/manipulation because of a number of factors:
- Larger (PS)RAM, starting at 8MB.
- Better vector instructions upwards from first gen boards.
- Compatibility & relatively good support for most camera sensors via DVP.
<br>

Though the ESP32-S3 and similar boards have USB-OTG, the easiest way to use camera modules is via DVP on the board. This will necessarily leave certain pins unusable. DOcumentation and exmaples for this are well made, especially by FreeNove, who I use regularly. There are three (3) steps in camera usage:
1. Initialization
2. Capturing to / Freeing image buffer
3. De-initialization

### 2.1. Camera Initialization

The exact steps for initialization will depend on the exact chip and manufacturer, but it seems that the FreeNove version works for most "CAM-enabled" ESP32 boards. Here, after importing "esp32_camera.h" in the Arduino IDE, or downlaoding the source from FreeNove's Github, we must change a few things.

1. Firstly, we need to define the pins for the Camera usage. These will be indicated on the diagram of the board you've got.

    ```cpp
    #if defined(CAMERA_MODEL_WROVER_KIT)
    #define PWDN_GPIO_NUM    -1
    #define RESET_GPIO_NUM   -1
    #define XCLK_GPIO_NUM    21
    #define SIOD_GPIO_NUM    26
    #define SIOC_GPIO_NUM    27

              ...

    #elif defined(CAMERA_MODEL_ESP_EYE)
    #define PWDN_GPIO_NUM    -1
    #define RESET_GPIO_NUM   -1
    #define XCLK_GPIO_NUM    4
    #define SIOD_GPIO_NUM    18
    #define SIOC_GPIO_NUM    23

              ...

    #elif defined(CAMERA_MODEL_M5STACK_PSRAM)
    #define PWDN_GPIO_NUM     -1
    #define RESET_GPIO_NUM    15
    #define XCLK_GPIO_NUM     27
    #define SIOD_GPIO_NUM     25
    #define SIOC_GPIO_NUM     23

              ...

    #elif defined(CAMERA_MODEL_M5STACK_V2_PSRAM)

              ...
    ```
  This would simply be setting the correct constant in our relative cpp/ino file.  

2. Next is setting the frequency, pins, and other settings. 
NOTE: You must set the correct frequency for the camera you've selected. Camera models will not function correctly otherwise.

    ```cpp

    debugln("Setting up camera...");

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
              ...
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = CAMERA_CLK;
    config.frame_size = FRAMESIZE_FHD;
    config.pixel_format = PIXFORMAT_JPEG;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 15;
    config.fb_count = 1;

    ```
    
    Next is checking for PSRAM. This is redundant if you already know the specs of your board and don't intend of using this code elsewhere. This check does make your code more portable however, as older boards may not have PSRAM.
    
    ```cpp

    /**
     * if PSRAM IC present, init with higher resolution * 
     * and higher JPEG quality for larger pre-allocated frame buffer. 
     */ 
    if(psramFound()){
      config.jpeg_quality = 5;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      /** 
       *Limit the frame size when PSRAM is not available
       */
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
      debugf("Camera init failed with error 0x%x", err);
    }

    sensor_t * s = esp_camera_sensor_get();
    /** 
     * initial sensors are flipped vertically 
     */
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -1); // lower the saturation
    
    debugln("Camera configuration complete!");
    ```


### 2.2. Camera buffer

### 2.3. Camera de-init


## 3.0. String Precautions

## 4.0. Networking
### 4.1. Wi-Fi Connection
### 4.2. Getting Correct Time
### 4.3. Keeping Connections

## 5.0. Sensor Reading
