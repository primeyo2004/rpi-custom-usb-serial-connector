# rpi-custom-usb-serial-connector
A custom-made usb-serial connector (made from 2 Arduino Nano) for Rasberry PI UART0

## Background

Ever heard about "Serial Console" in Linux ?

Well, it's essentially an "old school" way of gaining access and controlling your Unix/Linux system through a physical serial cable connection.

But now a days, it's basically nothing compared to using a directly-attached keyboard and monitor to your Unix/Linux system, and that even the famous SSH for remote access is far more superior than this method.

However, when it comes to Embedded Linux (e.g. your headless Raspbian Linux on Raspberry PI), you may probably want learn how to use the serial console as you'll never know when your SSH will start to unfriend you (the moment you accidentally screw up your Linux configuration :) )

The good news is that your Raspbian (or other Linux distros) may already have been configured to accept terminal commands from UART0 (Header Pin 8 and Pin 10). But the not-so-good news is, the UART voltages is not compatible with your PC/Laptop's RS232 connectors (if you're even lucky to have one).

So if you're still with me, then your options are:
(1) to buy a UART to RS232 converter for your Raspberry PI, (the easy way) Or...
(2) you can try to follow the details below (the fun way).

Enjoy!

## Instructions
1. Compile and upload the **arduino_uart2usbserial** project using Arduino Sketch to the 2 Arduino Nanos
2. The follow the Connection Diagram and/or Actual Setup below

## Connection Diagram
![alt tag](https://github.com/primeyo2004/rpi-custom-usb-serial-connector/blob/master/rpi-custom-usb-serial-connector-diagram.jpg)

## Actual Setup
![alt tag](https://github.com/primeyo2004/rpi-custom-usb-serial-connector/blob/master/rpi-custom-usb-serial-connector-setup.jpg)

## Sample Video
[![CUSTOM USB-SERIAL](https://img.youtube.com/vi/I6nUVXbevhg/0.jpg)](https://www.youtube.com/watch?v=I6nUVXbevhg)
