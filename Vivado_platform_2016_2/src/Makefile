ifeq ($(shell uname), Linux)
	S =;
	MKDIR = mkdir -p
	fixpath = $1
else
	S =&
	MKDIR = mkdir
	fixpath = $(subst /,\,$1)
endif

.PHONY: all

all: libzybo_led.a

libzybo_led.a: uio_axi_gpio.c
	arm-xilinx-linux-gnueabi-gcc -c -g uio_axi_gpio.c
	arm-xilinx-linux-gnueabi-ar crvs libzybo_led.a uio_axi_gpio.o

clean:
	rm -rf libzybo_led.a uio_axi_gpio.o


