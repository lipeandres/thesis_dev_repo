BASE = $(HOME)/thesis_dev_repo/buildroot-2011.08/output/host/usr/arm-unknown-linux-gnueabi/sysroot

CROSS = arm-none-linux-gnueabi-


##BASE = /
##CXX = g++
##CC  = gcc
CXX = $(CROSS)g++ #sysroot=${BASE}
CC  = $(CROSS)gcc #sysroot=${BASE}
STRIP = $(CROSS)strip

EXEC    = test_serial

DEBUG = -O3 -g0

CC_SOURCES  =  serial_helper.c  

CXX_SOURCES = test_serial.cpp

HXX_SOURCES = timestamp.h hart.h vr.h \
              dad_measures.h incom.h

WARNINGS= -Wcast-align -Wpacked -Wpadded -Wall

INCLUDE = -I. -I$(BASE)/usr/include -I$(BASE)/usr/include/libxml2/

CCFLAGS = ${INCLUDE} ${DEBUG} ${WARNINGS} 

LDFLAGS = -L$(BASE)/lib 

CXX_OBJECTS = $(CXX_SOURCES:.cpp=.o)

CC_OBJECTS = $(CC_SOURCES:.c=.o)

$(EXEC): $(CC_OBJECTS) $(CXX_OBJECTS) 
	$(CXX)  $(CCFLAGS) $(CC_OBJECTS) $(CXX_OBJECTS) $(LDFLAGS) -o $@
	$(STRIP) $@

.cpp.o:
	$(CXX) -c $(CCFLAGS) $< -o $@

.c.o:
	$(CC) -c $(CCFLAGS) $< -o $@

clean: 
	rm -f *.o ${EXEC} *~

indent:
	astyle $(CXX_SOURCES) $(HXX_SOURCES) $(CC_SOURCES)
