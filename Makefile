# proxy makefile to build everything

all: 
	$(MAKE) -C skyline

clean:
	$(MAKE) -C skyline clean
