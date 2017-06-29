APPSOURCES = arraycopy.cpp
EXECUTABLE = zybo_led.elf

VERBOSE  = #-verbose
PLATFORM = ../../../zybo_hdmi_in

SDS_FLAGS = \
-sds-pf ${PLATFORM} \
-sds-hw arraycopy arraycopy.cpp -sds-end \
${VERBOSE}

CC = sds++ ${SDS_FLAGS}

IDIRS = -I. 
LDIRS = 
LLIBS =  -lzybo_led
OLEVEL = -g

CFLAGS = -Wall ${OLEVEL} -c ${IDIRS} 
LFLAGS = ${OLEVEL} ${LDIRS} ${LLIBS} 

SOURCES := $(patsubst %,./%,$(APPSOURCES))   
OBJECTS := $(APPSOURCES:.cpp=.o)
DEPS := $(OBJECTS:.o=.d)

.PHONY: all

all: ${EXECUTABLE}

${EXECUTABLE}: ${OBJECTS}
	${CC} ${OBJECTS} -o $@  ${LFLAGS} 

-include ${DEPS}

%.o: %.cpp
	${CC} ${CFLAGS} $< -o $@

clean:
	${RM} ${EXECUTABLE} ${OBJECTS}

ultraclean: clean
	${RM} -rf _sds sd_card ${EXECUTABLE}.bit

