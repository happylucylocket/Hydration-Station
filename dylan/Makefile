OUTFILE = projectTest
OUTDIR = $(HOME)/cmpt433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread

all: project 

project:
	$(CC_C) $(CFLAGS) main.c pump.c timer.c distanceSensorLinux.c -o $(OUTDIR)/$(OUTFILE)

clean: 
	rm $(OUTDIR)/$(OUTFILE)

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
