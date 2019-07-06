# badusb_sd_wifi
BadUSB with WiFi that allows to execute payloads from SD

ESP code is based on the ESPloitV2 code (created by Corey Harding).

www.LegacySecurityGroup.com / www.Exploit.Agency
https://github.com/exploitagency/ESPloitV2

ESPloit is distributed under the MIT License. The license and copyright notice can not be removed and must be distributed alongside all future copies of the software.

**Special thanks to:**
- @Santpapen
- @ernesto_xload
- @LucaBongiorni
- @WHID Injector

BadUSB SD WiFi V1: functional version, but still being tested:

Hardware CJMCU-3212: https://es.aliexpress.com/store/product/Beetle-Virtual-Keyboard-Badusb-Development-Board-ATMEGA32U4-WIFI-ESP8266-ESP-12E-TF-Micro-SD-Card-Storage/2130127_32839570915.html

1.- Functionalities
2.- Payloads syntax
3.- Instalation
4.- Upload HTML by FTP

**1.- WiFi functionalities:**

- **Virtual Keyboard:** Allows to have a virtual keyboard on the screen (it is recommended to use it in mobile phones).

![Alt text](/images/virtualkeyboard.png?raw=true "Virtual Keyboard mode")

- **Physical Keyboard:** This utility has two options: Pc or mobile phone.

Pc: If accessed from a computer, it allows you to use your computer's physical keyboard to send pressed keys to the computer where the badUSB is connected.

![Alt text](/images/physicalpc.png?raw=true "Physical Keyboard PC mode")

Phone: If accessed from your mobile phone, allows you to use your mobile phone keyboard from a textarea.

![Alt text](/images/physicalmobile.jpeg?raw=true "Physical Keyboard mobile mode")

- **Live Payload:** Allows to execute commands and payloads from a textarea.

![Alt text](/images/livepayload.png?raw=true "Live Payload mode")

- **Choose Payload:** Allows you to delete and execute payloads that are stored on the device.

![Alt text](/images/listpayload.png?raw=true "List Payload mode")

- **Upload Payload:** Allows you to upload a payload to the device.

![Alt text](/images/uploadpayload.png?raw=true "Upload Payload mode")

- **FTP Server:** The device has an FTP server enabled. You can use the pftp -i 192.168.1.1 command (Linux) to connect to it.

![Alt text](/images/ftp.png?raw=true "FTP server mode")


**2.- Payloads syntax:**

- print (example: print test)
- println (example: println test)
- press (example: press KEY_RETURN)
- rawpress (example: press KEY_RETURN)
- delay (example: delay 1000)
- release (example: release)
- runwin (example: runwin)
- rungnome (example: rungnome)
- runmac (example: runmac)
- execSD ((example: execSD test.txt, execSD helloworld.txt, execSD remote.txt, etc)

In my github you can find example payloads for this device (coming soon).



**3.- Installation:**

- Download and Install the Arduino IDE: https://www.arduino.cc/en/main/software

- Go to File - Preferences. Locate the field "Additional Board Manager URLs:"
Add "http://arduino.esp8266.com/stable/package_esp8266com_index.json" without quotes.
Click "Ok"

- If Arduino IDE gives you the following error:
"Error downloading http://arduino.esp8266.com/stable/package_esp8266com_index.json"
use "https://github.com/esp8266/Arduino/releases/download/2.3.0/package_esp8266com_index.json" instead.

- Select Tools - Board - Boards Manager. Search for "esp8266".
Install "esp8266 by ESP8266 community version 2.3.0". Click "Close".

- Download simple FTP server and copy into the arduino library directory: https://github.com/nailbuster/esp8266FTPServer

- Download/extract BadUSB WiFi SD repo.


**Upload sketch esp8266Programmer**

![Alt text](/images/normal.png?raw=true "Normal BadUSB")

- Load the esp8266Programmer sketch from the flashing folder.

- Select Tools - Board – "Arduino Leonardo".

- Select the Port your device is connected to under Tools – Port.

- Upload the sketch.


**PROGRAMMING MODE ESP8266**

- Open the ESP_Code sketch from the source folder.

- Select Tools - Board - "Generic ESP8266 Module".

- Select Tools – Upload Speed “115200”

- Select Tools – Flash Size “512K (64K SPIFFS)”

- Select Sketch - "Export Compiled Binary".

- Now start ESP8266 in programming mode.

![Alt text](/images/programmingmode.png?raw=true "Programming mode ESP8266")
  
- Now flash the firmware to the ESP chip using one of the following tools.

Linux: https://github.com/AprilBrother/esptool

sudo python esptool.py --port=/dev/ttyACM0 --baud 115200 write_flash 0x00000 /home/badusb_sd_wifi/esp/esp.ino.generic.bin --flash_size 32m

Windows: https://github.com/nodemcu/nodemcu-flasher

**Upload sketch atmega32u4_code**

- Finally open the Arduino_32u4_code sketch from the source folder.

- Select Tools - Board – "Arduino Leonardo".

- Select the Port your device is connected to under Tools – Port.

- Upload the sketch.


**4.- Upload HTML by FTP**

- Visualize the wifi networks around you and connect to the badusb wifi network

- Enter the password for the wifi network (default password: badUSBWifi)

- Open a terminal in the HTML directory

- Connect to the badusb FTP server with the following command (linux): pftp -i 192.168.1.1

- Enter the FTP server username and password (default username and password: esp8266)

- Upload all HTML from the directory (example command: put virtualkeyboard.html)

- When you have all the HTML files uploaded, disconnect and reconnect the badusb

- Reconnect to the badusb wifi network and enter password (default password: badUSBWifi)

- Open a browser and access the badusb web panel (default IP: 192.168.1.1)


For any questions you can contact me by twitter: **@JoelSernaMoreno**

