# BlackmagicAtemToM5Atom
Code for sending signals from an Blackmagic Atem video mixer to an M5 atom board (Arduino) via a server (C#)

Instructions for the server:
- Download and install ATEM Switcher software (https://www.blackmagicdesign.com/nl/developer/product/atem)
- Open ..\Server\Atem.Switcher.sln
- Change the ip-address in program.cs to the ip-address of your Blackmagic Atem Switch
- Run the program

Instruction for M5 Atom
- Download and install Arduino (https://www.arduino.cc)
- Download and install the M5 stack usb drivers (CP210x_VCP) (https://m5stack.com/pages/download)
- 