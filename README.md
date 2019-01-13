# Kayaband
A SMART dog collar with differen sensors. With a webapp you can send your instructions to via a remote.

Sensors and components:
- 2 on/off led strips.
- Buzzer
- 2 TIP120 transistor
- 2 ESP32 microcontrollers
- 2 LoRa modules
- NEO-6M GPS Module
- An OLED display for the ESP32
- LDR sensor

First I connected the ledstrips wit the transistors, buzzer, gps, ldr sensor and 1 LoRa to one of the ESP32 microcontrollers and used the
KayaBan_esp32_code.ino
Then you connect the other LoRa Module and the OLED display to the second ESP32 en you upload to the controller the Remote_esp32_code_WebServer_v2.ino
It works as follows:
- You turn everything on.
- With the information on the display you connect your device to the WiFi network made by the second ESP32 controller.
- You go to the given IP-adress on your browser
- You choose what you want to do, for example turn on the buzzer.
- Your devices communicates with the second ESP32 via Wifi
- The LoRa module on the second ESP32 sends a signal to the LoRa module on the first ESP32 that turns on the buzzer on the collar.
- You can do this with all of the options.
- To click on the coordinates on the app and open it on Google Maps you have to disconnect your Wifi.

Tutorials I followed to connect the sensors and components:
1. Arduino Tutorial - https://www.arduino.cc/en/Tutorial/HomePage
2. Arduino LEDStrip Tutorial 1 - https://www.instructables.com/id/PWM-an-RGB-LED-
Strip-with-Arduino/
3. Arduino LEDStrip Tutorial 2 - https://learn.adafruit.com/rgb-led-strips/usage
4. Arduino LEDStrip code - https://learn.adafruit.com/rgb-led-strips/arduino-code
5. Arduino LDR - https://www.instructables.com/id/Using-an-LDR-Sensor-With-
Arduino/
6. Arduino LDR - https://www.instructables.com/id/How-to-Use-a-Light-Dependent-
Resistor-LDR/
7. Arduino Buzzer - https://www.instructables.com/id/How-to-use-a-Buzzer-Arduino-
Tutorial/
8. Arduino Speaker - http://www.utopiamechanicus.com/article/safe-arduino-audio/
9. GPS Tutorial - https://www.instructables.com/id/How-to-Communicate-Neo-6M-
GPS-to-Arduino/
10. GPS met ESP32 hulpbron - https://forum.arduino.cc/index.php?topic=298198.0
11. TinyGPS++ library - http://arduiniana.org/libraries/tinygpsplus/
12. GPS hulpbron - https://forum.arduino.cc/index.php?topic=530310.0
13. GPS hulpbron - https://www.pjrc.com/teensy/td_uart.html
14. Inspiratie Webapp - https://www.mickmake.com/post/sms-lora-long-distance-sms-
without-4g-project
15. Autodesk Fusion tutorial - https://www.youtube.com/watch?v=A5bc9c3S12g
16. ESP32 als Access Point tutorial - https://randomnerdtutorials.com/esp32-access-
point-ap-web-server/
17. WebServer tutorial - https://randomnerdtutorials.com/esp32-web-server-arduino-
ide/
18. Code reference - https://www.arduino.cc/reference/en/language/variables/data-
types/string/
19. Code reference - https://www.arduino.cc/en/Reference/FunctionDeclaration
20. Code reference -
https://github.com/olikraus/u8g2/wiki/u8x8reference#refreshdisplay
21. Code reference - https://forum.arduino.cc/index.php?topic=42934.0
22. Code reference - https://arduino.stackexchange.com/questions/1013/how-do-i-split-
an-incoming-string
23. OLED display library - https://github.com/olikraus/u8g2/wiki
