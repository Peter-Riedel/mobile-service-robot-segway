# Miniprojekt: Mobile Service-Roboter - Segway
Hier findet ihr eine Zusammenfassung wichtiger Links und Quellen für das Projekt.

## ev3dev: Setup & Konfiguration
- [Step 4: Boot ev3dev](https://www.ev3dev.org/docs/getting-started/#step-4-boot-ev3dev)
- [Step 5: Set up a network connection](https://www.ev3dev.org/docs/networking/)
  - [Connecting to the Internet via USB](https://www.ev3dev.org/docs/tutorials/connecting-to-the-internet-via-usb/)
- [Step 6: Connect to the EV3 via SSH](https://www.ev3dev.org/docs/tutorials/connecting-to-ev3dev-with-ssh/)
- [Tutorials](https://www.ev3dev.org/docs/tutorials/)
  - [Setting Up an NFS File Share](https://www.ev3dev.org/docs/tutorials/setting-up-an-nfs-file-share/)
  - [Using Docker to Cross-Compile](https://www.ev3dev.org/docs/tutorials/using-docker-to-cross-compile/)
  - [Using the EV3 Speaker](https://www.ev3dev.org/docs/tutorials/using-ev3-speaker/)

## Library: ev3dev-c
- [Hauptseite](https://github.com/in4lio/ev3dev-c)
  - [Beispiele](https://github.com/in4lio/ev3dev-c/tree/master/eg)
  - [Schnittstellen / Header-Dateien](https://github.com/in4lio/ev3dev-c/tree/master/source/ev3)

## Programmieren in C
- [Programmieren in C](http://www.netzmafia.de/skripten/programmieren/index.html)
- [C Standard Library Reference](https://www.tutorialspoint.com/c_standard_library/index.htm)

## Konstruktion & Bau
- [Details zu Sensoren & Motoren](https://www.lego.com/de-de/themes/mindstorms)
  - [Gyro Sensor](https://www.youtube.com/watch?v=rZxuwxOpLYU)
  - [US Sensor](https://www.youtube.com/watch?v=yU7db5kFdXA)
  - [Color Sensor (Part 1)](https://www.youtube.com/watch?v=if1yk4WiaiQ)
  - [Color Sensor (Part 2)](https://www.youtube.com/watch?v=IplkMchOabc)
  - [Touch Sensor](https://www.youtube.com/watch?v=FN4VSRGu9_E)
  - [Motors/Output Devices](http://docs.ev3dev.org/projects/lego-linux-drivers/en/ev3dev-jessie/motors.html)
- Ideen:
  - [Gyro Boy - Bauanleitung](https://www.youtube.com/watch?v=zDjWz4jC59s)
  - [Gyro Boy - Demonstration](https://www.youtube.com/watch?v=wejXE9jlszo)

## TODOs
1. **Planung einreichen (bis 04.11.2019) per Email an Björn Zeise und Jan Michel Rotter**
2. Programmieraufgaben:
   1. motor debug
   2. implement coroutine for standing / driving (gyro-sensor + tacho motors)
   3. implement detection of barrels by ultrasonic sensor
3. Konstruktion / Bau ...
