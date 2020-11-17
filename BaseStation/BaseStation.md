Prerequisites:
- [Arm Compiler (gcc-arm-none-eabi)](https://packages.debian.org/buster/gcc-arm-none-eabi)
- [bossa-cli: command line programmer](https://downloads.arduino.cc/bossac-1.6.1-arduino-x86_86-linux-gnu.tar.gz) - I'm using version 1.7 that came with the latest arduino release. Version 1.8 and up I couldn't get to upload. Also can be downloaded from [github](https://github.com/shumateck/BOSSA).
- [Arduino SAMD Library](https://github.com/adafruit/arduino-board-index/blob/gh-pages/boards/adafruit-samd-1.6.1.tar.bz2)
- [CMSIS: Arm Cortex librarys](https://github.com/ARM-software/CMSIS_5)
- [CMSIS: Arduino Arm Cortex Librarys](https://github.com/arduino/ArduinoModule-CMSIS-Atmel)

I want to use the Library from Atmel, but I believe the Arduino Library protects the bootloader. Not sure on this, but I want to keep the bootloader until I know more.
