# The dongle is intended to be based upon the nano, but I'm using my Mega-2560 based
# K3NG hardware for development, which explains the board_tag
#BOARD_TAG    = nano
#BOARD_TAG    = mega2560
BOARD_TAG    = nano328
ARDUINO_LIBS = 
USER_LIB_PATH = ${PWD}/libraries
#MONITOR_PORT = /dev/ttyACM0
MONITOR_PORT = /dev/ttyUSB0

include /usr/share/arduino/Arduino.mk

