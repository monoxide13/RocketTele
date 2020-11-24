# This makefile is a slightly modified version of github user shanet.
# Find the original here https://github.com/shanet/Osprey/blob/master/arm/Makefile
# A HUGE thanks to him because I struggled a long time trying to make my own version work.

# The Basics
# -------------------------------------------------------------------------------------------------------------
BOARD := adafruit_feather_m0
DEVICE := ttyACM0
TARGET := BaseStation
BAUD := 230400
RADIO := ttyUSB0

ROOT_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
IDIR := $(ROOT_DIR)/inc
SDIR := $(ROOT_DIR)/src
ODIR := $(ROOT_DIR)/obj
LDIR := $(ROOT_DIR)/lib
GDIR := $(ROOT_DIR)/../libraries
BDIR := $(ROOT_DIR)/bin
SAMD_DIR := $(GDIR)/ArduinoCore-samd
CMSIS_DIR := $(GDIR)/ArduinoModule-CMSIS-Atmel/CMSIS-Atmel/CMSIS/Device/ATMEL

SOURCES := $(wildcard $(addprefix $(SDIR)/, *.c *.cpp))
HEADERS :=  $(wildcard $(addprefix $(SDIR)/, *.h *.hpp))
OUTPUT := $(BDIR)

# Tools
# -------------------------------------------------------------------------------------------------------------
TOOLS_DIR := /usr/bin
CC := $(TOOLS_DIR)/arm-none-eabi-gcc
CPPC := $(TOOLS_DIR)/arm-none-eabi-g++
AR := $(TOOLS_DIR)/arm-none-eabi-ar
OBJCOPY := $(TOOLS_DIR)/arm-none-eabi-objcopy
#UPLOADER := $(TOOLS_DIR)/bossac
UPLOADER := $(ROOT_DIR)/../tools/bossac

# Board info
# -------------------------------------------------------------------------------------------------------------
GET_BOARDS_PARAM = $(shell sed -ne "s/$(BOARD).$(1)=\(.*\)/\1/p" $(SAMD_DIR)/boards.txt)

BOARD_BOARD := $(call GET_BOARDS_PARAM,build.board)
BOARD_BOOTLOADER_FILE := $(call GET_BOARDS_PARAM,bootloader.file)
BOARD_BUILD_FCPU := $(call GET_BOARDS_PARAM,build.f_cpu)
BOARD_BUILD_MCU := $(call GET_BOARDS_PARAM,build.mcu)
BOARD_BUILD_VARIANT := $(call GET_BOARDS_PARAM,build.variant)
BOARD_LD_SCRIPT := $(call GET_BOARDS_PARAM,build.ldscript)
BOARD_USB_MANUFACTURER := $(call GET_BOARDS_PARAM,build.usb_manufacturer)
BOARD_USB_PID := $(call GET_BOARDS_PARAM,build.pid)
BOARD_USB_PRODUCT := $(call GET_BOARDS_PARAM,build.usb_product)
BOARD_USB_VID := $(call GET_BOARDS_PARAM,build.vid)

# The variant file for the board contains essential information specific to the device we're compiling for
SOURCES += $(SAMD_DIR)/variants/$(BOARD_BUILD_VARIANT)/variant.cpp

OBJECTS := $(addsuffix .o, $(basename $(SOURCES)))
COMPILED_OBJECTS := $(addprefix $(ODIR)/, $(addsuffix .o, $(basename $(SOURCES))))

# Flags
# -------------------------------------------------------------------------------------------------------------
CFLAGS := -mcpu=$(BOARD_BUILD_MCU) -mthumb -c -g -Os -w -std=gnu11 -ffunction-sections -fdata-sections \
	-nostdlib --param max-inline-insns-single=500 -MMD
CPPFLAGS := -mcpu=$(BOARD_BUILD_MCU) -mthumb -c -g -Os -w -std=gnu++11 -ffunction-sections -fdata-sections \
	-fno-threadsafe-statics -fno-rtti -fno-exceptions -nostdlib --param max-inline-insns-single=500 -MMD
ASMFLAGS := -c -g -x assembler-with-cpp
ELFFLAGS := -Os -Wl,--gc-sections -save-temps --specs=nano.specs --specs=nosys.specs -mcpu=$(BOARD_BUILD_MCU) \
	-mthumb -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--unresolved-symbols=report-all \
	-Wl,--warn-common -Wl,--warn-section-align

# Defines
# -------------------------------------------------------------------------------------------------------------
DEFINES := -DF_CPU=$(BOARD_BUILD_FCPU) -DARDUINO=10605 -DARDUINO_$(BOARD_BOARD) -DARDUINO_ARCH_SAMD \
	-D__SAMD21G18A__ -DUSB_VID=$(BOARD_USB_VID) -DUSB_PID=$(BOARD_USB_PID) -DUSBCON \
	-DUSB_MANUFACTURER=\"$(BOARD_USB_MANUFACTURER)\" -DUSB_PRODUCT=\"$(BOARD_USB_PRODUCT)\"

# Includes
# -------------------------------------------------------------------------------------------------------------
# Add something that searches for all folders inside LDIR folder.
INCLUDE_DIRS := $(LDIR) \
    $(LDIR)/UBLOX/src \
    $(LDIR)/SD/src \
	$(wildcard $(LDIR)/*) \
	$(GDIR)/CMSIS_5/CMSIS/Core/Include \
	$(GDIR)/CMSIS_5/CMSIS/DSP/Include \
	$(CMSIS_DIR) \
	$(SAMD_DIR)/cores/arduino \
	$(SAMD_DIR)/variants/$(BOARD_BUILD_VARIANT) \
	$(wildcard $(SAMD_DIR)/libraries/*)

# Add the include flag before each include
INCLUDES := $(foreach dir, $(INCLUDE_DIRS), \
	$(addprefix -I, $(dir)))

# Project Libraries
# -------------------------------------------------------------------------------------------------------------
LIBRARY_SEARCH_PATHS ?= $(LDIR) $(SAMD_DIR)/libraries

# Search the headers for what libraries are included
#FOUND_LIBRARIES := $(filter $(notdir $(wildcard $(addsuffix $(ROOT_DIR)/*, $(LIBRARY_SEARCH_PATHS)))), \
	$(shell sed -ne "s/^ *\# *include *[<\"]\(.*\)\.h[>\"]/\1/p" $(HEADERS)))
# Compile all libraries regardless of whether included in header files
FOUND_LIBRARIES := $(notdir $(wildcard $(addsuffix /*, $(LIBRARY_SEARCH_PATHS))))

# Add the path to the found libraries
LIBRARY_DIRS := $(foreach lib, $(FOUND_LIBRARIES), \
	$(firstword $(wildcard $(addsuffix /$(lib), $(LIBRARY_SEARCH_PATHS)))))

# Explicitly add all files in lib/
EXPLICIT_LIBRARIES = $(notdir $(wildcard $(LDIR)/*))

LIBRARY_DIRS += $(foreach lib, $(EXPLICIT_LIBRARIES), \
	$(shell find -H $(LDIR)/$(lib) -type d))

# Platform Libraries
# -------------------------------------------------------------------------------------------------------------
ARDUINOCOREDIR := $(SAMD_DIR)/cores/arduino \
	$(SAMD_DIR)/cores/arduino/USB \
	$(SAMD_DIR)/cores/arduino/avr

# Link all the platform libraries into a single archive
CORE_LIB := $(ODIR)/core.a
CORE_LIB_OBJECTS := $(foreach dir, $(ARDUINOCOREDIR) $(LIBRARY_DIRS), \
	$(patsubst %, $(ODIR)/%.o, $(wildcard $(addprefix $(dir)/, *.c *.cpp *.S))))

# Recipes
# -------------------------------------------------------------------------------------------------------------

.PHONY:	all target upload size console clean reset

all: target

target: $(TARGET).bin

upload: target
	stty -F /dev/$(DEVICE) 1200
	sleep 4
	stty -F /dev/$(DEVICE) hupcl # I don't need this line, but have read some do.
	sleep 1
	$(UPLOADER) -i -d --port=$(DEVICE) -U true -i -e -w -v $(OUTPUT)/$(TARGET).bin -R

size: $(TARGET).elf
	echo && avr-size $(OUTPUT)/$(TARGET).elf

debug:
	@echo HEADERS: $(HEADERS)
	@echo LIBRARY_SEARCH_PATHS: $(LIBRARY_SEARCH_PATHS)
	@echo FOUND_LIBRARIES: $(FOUND_LIBRARIES)
	@echo EXPLICIT_LIBRARIES: $(EXPLICIT_LIBRARIES)
	@echo LIBRARY_DIRS: $(LIBRARY_DIRS)
	@echo $(CORE_LIB_OBJECTS)


console:
	screen /dev/$(RADIO) $(BAUD)

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $(OUTPUT)/$< $(OUTPUT)/$@

$(TARGET).elf: $(CORE_LIB) $(OBJECTS)
	$(CPPC) -L$(ODIR) -T$(SAMD_DIR)/variants/$(BOARD_BUILD_VARIANT)/$(BOARD_LD_SCRIPT) \
		-Wl,-Map,$(OUTPUT)/$(TARGET).map $(ELFFLAGS) -o $(OUTPUT)/$@ $(COMPILED_OBJECTS) \
		-Wl,--start-group -lm $(CORE_LIB) -Wl,--end-group

# Build the source files
# -------------------------------------------------------------------------------------------------------------

%.o: %.c
	mkdir -p $(ODIR)$(dir $<)
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o $(ODIR)/$@ $<

%.o: %.cpp
	mkdir -p $(ODIR)/$(dir $<)
	$(CPPC) $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $(ODIR)/$@ $<

# Build the core library files
# -------------------------------------------------------------------------------------------------------------

$(CORE_LIB): $(CORE_LIB_OBJECTS)
	$(AR) rcs $@ $?

$(ODIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(ODIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CPPC) $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(ODIR)/%.S.o: %.S
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

clean:
	rm -f $(COMPILED_OBJECTS)
	rm -f $(OUTPUT)/$(TARGET).elf $(OUTPUT)/$(TARGET).bin $(CORE_LIB) $(OUTPUT)/$(TARGET).map
	rm -rf $(ODIR)/src $(ODIR)/lib

reset:
	rm -rf $(ODIR)/*
	@touch $(ODIR)/empty
	rm -rf $(BDIR)/*
	@touch $(BDIR)/empty

# End
