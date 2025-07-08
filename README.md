# Tiny85FanControl

A project to control a fan based on the temperature of an attached sensor.

This project is **WORK IN PROGRESS** and may not work as expected.

## Hardware

The following hardware components are required:
- [ATTiny85](https://www.microchip.com/en-us/product/attiny85)
- A MOSFET of some sort. I haven't gotten to this part, yet.
- [USBtinyISP](http://www.fischl.de/usbtinyisp/) programmer (or similar)


## Tools needed

If you're on Ubuntu 24.04, the dependencies are:

```bash
>> sudo apt-get install build-essential avrdude gcc-avr binutils-avr avr-libc git
```

## Building and uploading

To build this project, run `make` in the root directory of the project.

```bash
cd /path/to/project/
make all # Builds the binary
make flash # Uploads the binary to the ATTiny85
```

## TODO

Many things!
