# BlackmagicAtemToM5Atom
Code for sending signals from an Blackmagic Atem video mixer to an M5 atom board (Arduino) via a server (C#)

Instructions for the server:
- Download and install ATEM Switcher software (https://www.blackmagicdesign.com/nl/developer/product/atem)
- Open ..\Server\Atem.Switcher.sln
- Change the ip-address in program.cs to the ip-address of your Blackmagic Atem Switch
- (optional) You can change the udp ip/port here, but you should also change that in de M5 Atom
- Run the program

Instruction for M5 Atom
- Download and install Arduino (https://www.arduino.cc)
- Download and install the M5 stack usb drivers (CP210x_VCP) (https://m5stack.com/pages/download)
- Open the Arduino IDE to install some required dependencies
- Go to File -> Preferences
- Add the following url to 'Additional Boards Manager URLs:
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  (source: https://github.com/espressif/arduino-esp32)
- Go to Tools -> Board -> Boards manager
- Search for esp32 and install esp32 by Espressif Systems
- Close the Boards manager
- Go to Tools -> Boards and select 'ESP32 Pico Kit'
- Go to Tools -> Upload speed and select '115200'
- Go to Tools -> Manage libraries
- Search for and install the following libraries
    - M5Atom by M5Stack
    - FastLED by Daniel Garcia
- Open tally.ino
- Change the following variables
    - networkName
    - networkPswd
    - (optional) If you changed the udp ip/port in the server, change the variables 'IPAddress' and 'udpPort' to match the server
- Now you can verify and upload the file to you M5Atom
