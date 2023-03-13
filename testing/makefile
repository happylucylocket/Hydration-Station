OUTFILE = display_test
OUTDIR = $(HOME)/cmpt433/public/myApps/

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -lpthread

all: app

app:
	$(CC_C) $(CFLAGS) timer.c -o $(OUTDIR)/$(OUTFILE) 
	#//display_test

clean:
	rm $(OUTDIR)/$(OUTFILE)