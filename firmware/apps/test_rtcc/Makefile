# ***************************************************************************************
#
# Makefile
#
# ***************************************************************************************
#
#        Copyright (c) 2022, Martin Garon, All rights reserved.
#
# This source code is licensed under the GPL-3.0-style license found in the
# LICENSE file in the root directory of this source tree.
#
# ***************************************************************************************
#
# File Name....: Makefile
#
# Description..: Makefile for generation of project's executable.
#
# ***************************************************************************************


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Makefile variables
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.PHONY: help
help:
	@echo Building configurations: Debug
	@echo 
	@echo Targets:
	@echo 'make          - Debug (default) configuration.'
	@echo
	@echo Cleaning configurations: Debug
	@echo 
	@echo Targets:
	@echo 'make clean         - Debug (default) configuration.'
	@echo 'make *_clean - Cleans target and intermediate files.'

# Project name
PROJECT := test_rtcc

# Directories/paths.
REPO_ROOT_DIR := ../..
FIRMWARE_PATH := $(REPO_ROOT_DIR)/firmware
PROJECT_ROOT_DIR := $(REPO_ROOT_DIR)/firmware/apps/test_rtcc
PROJECT_FW_DIR := $(PROJECT_ROOT_DIR)

# TivaWare/TI driver library.
TIVAWARE_LIB_PATH := $(FIRMWARE_PATH)/3rdparty/TivaWare_C_Series-2.2.0.295

# SEGGER RTT library.
SEGGER_RTT_PATH := $(FIRMWARE_PATH)/3rdparty/segger_rtt_v758

# CMSIS library.
CMSIS_LIB_PATH := $(FIRMWARE_PATH)/3rdparty/CMSIS_5-5.1.1

# List of all source directories used by this project.
VPATH = \
    ../../ \
    $(PROJECT_FW_DIR) \
    $(FIRMWARE_PATH)/corelink/src \
    $(FIRMWARE_PATH)/drivers/src \
    $(SEGGER_RTT_PATH)/RTT \
    $(SEGGER_RTT_PATH)/SysCalls


# List of all include directories needed by this project.
INCLUDES = \
    -I. \
    -I$(FIRMWARE_PATH)/corelink \
    -I$(FIRMWARE_PATH)/drivers \
    -I$(TIVAWARE_LIB_PATH) \
    -I$(CMSIS_LIB_PATH)/CMSIS/Core/Include \
    -I$(SEGGER_RTT_PATH)/RTT \
    -I$(SEGGER_RTT_PATH)/Config

# Files
# Assembler source files.
ASM_SRCS :=

SEGGER_RTT_SRCS := \
    SEGGER_RTT.c \
    SEGGER_RTT_printf.c

# C source files.
C_SRCS := \
    startup_TM4C129ENCPDT.c \
    system_TM4C129.c
	
	# \
    #uartstdio.c


# C++ source files.
CPP_SRCS := \
    DS3234.cpp \
    GPIO.cpp \
    main.cpp \
    SPIMasterDev.cpp \
    SPISlaveCfg.cpp


OUTPUT    := $(PROJECT)
LD_SCRIPT := $(PROJECT).ld

DRIVER_LIB_DIR := -L$(TIVAWARE_LIB_PATH)/driverlib/gcc
DRIVER_LIB     := -ldriver

LIB_DIRS := $(DRIVER_LIB_DIR)
LIBS     := $(DRIVER_LIB)

# Defines.
DEFINES   += -D__STARTUP_CLEAR_BSS
DEFINES   += -DARMCM4_FP
DEFINES   += -DTARGET_IS_SNOWFLAKE_RA0
DEFINES   += -DPART_TM4C129ENCPDT
DEFINES   += -DUSE_TIVAWARE
DEFINES   += -DTM4C129ENCPDT
DEFINES   += -D__ARM_ARCH_7EM__

# ARM CPU, ARCH, FPU, and Float-ABI types...
# ARM_CPU:   [cortex-m0 | cortex-m0plus | cortex-m1 | cortex-m3 | cortex-m4]
# ARM_ARCH:  [6 | 7] (NOTE: must match ARM_CPU!)
# ARM_FPU:   [ | vfp]
# FLOAT_ABI: [ | soft | softfp | hard]
ARM_CPU   := -mcpu=cortex-m4
#ARM_ARCH  := -march=armv7-m   # NOTE: must match the ARM_CPU!
ARM_ARCH :=
ARM_FPU   := -mfpu=auto
FLOAT_ABI := -mfloat-abi=hard

# GNU-ARM toolset (NOTE: You need to adjust to your machine)
# see http://gnutoolchains.com/arm-eabi/
ifeq ($(GNU_ARM),)
#GNU_ARM = C:/tools/gnu_arm-eabi
#GNU_ARM = C:/apps/GNU\ Tools\ ARM\ Embedded/8\ 2019-q3-update
#GNU_ARM = C:/apps/GNU\ Tools\ ARM\ Embedded/9\ 2019-q4-major
#GNU_ARM = C:/apps/GNU\ Arm\ Embedded\ Toolchain/10\ 2021.10
GNU_ARM = C:/apps/Arm\ GNU\ Toolchain\ arm-none-eabi/11.2\ 2022.02
endif

# make sure that the GNU-ARM toolset exists...
ifeq ("$(wildcard $(GNU_ARM))","")
$(error GNU_ARM toolset not found. Please adjust the Makefile)
endif

CC    := $(GNU_ARM)/bin/arm-none-eabi-gcc
CPP   := $(GNU_ARM)/bin/arm-none-eabi-g++
AS    := $(GNU_ARM)/bin/arm-none-eabi-as
LINK  := $(GNU_ARM)/bin/arm-none-eabi-g++
BIN   := $(GNU_ARM)/bin/arm-none-eabi-objcopy
SIZE  := $(GNU_ARM)/bin/arm-none-eabi-size

# Basic utilities (included in Windows bin tools), see:
MKDIR := mkdir
RM    := rm


# Build options for various configurations for ARM Cortex-M4F
# Add the pre-defined symbol __FPU_PRESENT if needed...
ifneq (,$(ARM_FPU))
DEFINES += -D__FPU_PRESENT=1
ASM_FPU += -defsym=__FPU_PRESENT=1
endif

# Default Debug configuration.
BIN_DIR := dbg

ASFLAGS = -g $(ARM_CPU) $(ARM_ARCH) $(ARM_FPU) $(ASM_CPU) $(ASM_FPU)

CFLAGS = -g $(ARM_CPU) $(ARM_ARCH) $(ARM_FPU) $(FLOAT_ABI) \
    -mthumb -Wall -Wextra -Wpedantic -Wconversion \
    -ffunction-sections -fdata-sections \
    -O $(INCLUDES) $(DEFINES) -DUSE_RTT

CPPFLAGS = -g $(ARM_CPU) $(ARM_ARCH) $(ARM_FPU) $(FLOAT_ABI) \
    -mthumb -Wall -Wextra -Wpedantic -Wconversion \
	-Wno-psabi -Wno-volatile \
    -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions \
    -std=c++20 -Wno-register \
    -O $(INCLUDES) $(DEFINES) -DUSE_RTT


LIB_DIRS += -L$(BIN_DIR)

LINKFLAGS = -T$(LD_SCRIPT) $(ARM_CPU) $(ARM_FPU) $(FLOAT_ABI) \
	-mthumb \
	-specs=nosys.specs -specs=nano.specs \
	-u _printf_float \
	-Wl,-Map,$(BIN_DIR)/$(OUTPUT).map,--cref,--gc-sections $(LIB_DIRS)


ASM_OBJS     := $(patsubst %.s,%.o,  $(notdir $(ASM_SRCS)))
C_OBJS       := $(patsubst %.c,%.o,  $(notdir $(C_SRCS)))
CPP_OBJS     := $(patsubst %.cpp,%.o,$(notdir $(CPP_SRCS)))

TARGET_BIN   := $(BIN_DIR)/$(OUTPUT).bin
TARGET_ELF   := $(BIN_DIR)/$(OUTPUT).elf
ASM_OBJS_EXT := $(addprefix $(BIN_DIR)/, $(ASM_OBJS))
C_OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT   := $(patsubst %.o, %.d, $(C_OBJS_EXT))
CPP_OBJS_EXT := $(addprefix $(BIN_DIR)/, $(CPP_OBJS))
CPP_DEPS_EXT := $(patsubst %.o, %.d, $(CPP_OBJS_EXT))

# create $(BIN_DIR) if it does not exist
ifeq ("$(wildcard $(BIN_DIR))","")
$(shell $(MKDIR) $(BIN_DIR))
endif

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Targets.
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

all: show $(TARGET_BIN)
#all: $(TARGET_ELF)

.SILENT:

$(TARGET_BIN): $(TARGET_ELF)
	$(BIN) -O binary $< $@
	$(SIZE) $<

$(TARGET_ELF) : $(ASM_OBJS_EXT) $(C_OBJS_EXT) $(CPP_OBJS_EXT)
	echo "  LD    $@"
	$(LINK) $(LINKFLAGS) -o $@ $^ $(LIBS)

$(BIN_DIR)/%.d : %.c
	echo "  DEP   $<"
	$(CC) -MM -MT $(@:.d=.o) $(CFLAGS) $< > $@

$(BIN_DIR)/%.d : %.cpp
	echo "  DEP   $<"
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.o : %.s
	echo "  AS    $<"
	$(AS) $(ASFLAGS) $< -o $@

$(BIN_DIR)/%.o : %.c
	echo "  CC    $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%.o : %.cpp
	echo "  CCP   $<"
	$(CPP) $(CPPFLAGS) -c $< -o $@

# Include dependency files only if our goal depends on their existence
ifneq ($(MAKECMDGOALS),clean)
  ifneq ($(MAKECMDGOALS),show)
-include $(C_DEPS_EXT) $(CPP_DEPS_EXT)
  endif
endif


.PHONY: clean
show:
	@echo PROJECT = $(PROJECT)

#	@echo CONF = $(CONF)
#	@echo DEFINES = $(DEFINES)
#	@echo ASM_FPU = $(ASM_FPU)
#	@echo ASM_SRCS = $(ASM_SRCS)
#	@echo C_SRCS = $(C_SRCS)
#	@echo CPP_SRCS = $(CPP_SRCS)
#	@echo ASM_OBJS_EXT = $(ASM_OBJS_EXT)
#	@echo C_OBJS_EXT = $(C_OBJS_EXT)
#	@echo C_DEPS_EXT = $(C_DEPS_EXT)
#	@echo CPP_DEPS_EXT = $(CPP_DEPS_EXT)
#	@echo TARGET_ELF = $(TARGET_ELF)
#	@echo TARGET_BIN = $(TARGET_BIN)
