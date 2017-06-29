APPSOURCES = arraycopy.c
EXECUTABLE = zybo_led_sa.elf

VERBOSE  = #-verbose
PLATFORM = ../../../zybo_hdmi_in

SDS_FLAGS = \
-sds-pf ${PLATFORM} \
-sds-hw arraycopy arraycopy.c -sds-end \
-target-os standalone ${VERBOSE}

CC = sdscc ${SDS_FLAGS}

IDIRS = -I. 
LDIRS = 
LLIBS =  
OLEVEL = 

CFLAGS = -Wall ${OLEVEL} -c ${IDIRS} 
LFLAGS = ${OLEVEL} ${LDIRS} ${LLIBS} 

SOURCES := $(patsubst %,./%,$(APPSOURCES))   
OBJECTS := $(APPSOURCES:.c=.o)

.PHONY: all

all: ${EXECUTABLE}

${EXECUTABLE}: ${OBJECTS}
	${CC} ${OBJECTS} -o $@  ${LFLAGS} 

%.o: %.c
	${CC} ${CFLAGS} $< -o $@

clean:
	${RM} ${EXECUTABLE} ${OBJECTS}

ultraclean: clean
	${RM} -rf _sds sd_card ${EXECUTABLE}.bit

