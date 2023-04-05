# Makefile for building embedded application.
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.
TARGET= water_screamer

SOURCES= audioPlayer.c main.c joystick.c timer.c shutdown.c udp.c distanceSensorLinux.c pump.c
# SOURCES= testwave.c

PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror


# Asound process:
# get alibsound2 lib on target:
# 	# apt-get install libasound2
# Copy target's /usr/lib/arm-linux-gnueabihf/libasound.so.2.0.0 
#      to host  ~/public/asound_lib_BBB/libasound.so
# Copy to just base library:

LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB


# -pg for supporting gprof profiling.
#CFLAGS += -pg

all: wav node
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET)  $(LFLAGS) -lpthread -lasound

# Copy wave files to the shared folder
wav:
	mkdir -p $(PUBDIR)/wave-files/
	cp wave-files/* $(PUBDIR)/wave-files/
node:
	mkdir -p $(PUBDIR)/project-server-copy/ 
	cp -R server/* $(PUBDIR)/project-server-copy/ 
	cd $(PUBDIR)/project-server-copy/ && npm install

clean:
	rm -f $(OUTDIR)/$(TARGET)

# RUN THIS ON THE HOST!
ifneq ("$(wildcard /ID.txt )","")
# Target:
# If you run it on the target, it will print the following error.
all:
	@echo
	@echo "ERROR: Don't run *this* makefile on the BBG"
	@echo "   This makefile copies files to the $(HOME)/cmpt433/public/pru/ folder"
	@echo "   It looks like you are running it on your BBG"
	@echo "   Instead, you should change into the specific sub-directory for your"
	@echo "   current project and run 'make' there."
	@echo

else
# Host:
# Cross compile C/C++ code, and copy PRU code to target
all: nested-cross-compile pru-copy 

# Cross compile your C/C++ programs
# Uncommet this for any folder you want to cross-compile
nested-cross-compile:
	@# make --directory=your-project-subdirectory-name-here
	@# make --directory=logicAnalyzer-Linux

pru-copy:
	mkdir -p $(HOME)/cmpt433/public/pru/project
	cp -r pru/* $(HOME)/cmpt433/public/pru/project
	@echo "COPY ONLY" > $(HOME)/cmpt433/public/pru/project_COPY_ONLY_
	@echo ""
	@echo "You must build the PRU code on the target, then install it:"
	@echo "(bbg)$$ cd /mount/remote/pru/<your-folder>/"
	@echo "(bbg)$$ make"
	@echo "(bbg)$$ sudo make install_PRU0"
endif