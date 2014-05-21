CCFLAGS=
OPTFLAGS = -O0 -ffast-math -g -ggdb
SRCDIR = ./
OUTDIR = ./
OBJS = DeviceStateSender.o WiiDeviceServer.o
INCLUDES = -I../../../external/gmtl-0.5.4/ -I../../../include/inVRs -I../../../include/irrXML
LIBS = -lGL -lGLU -lX11 -lXi -lXmu -lm -lSDL -lirrXML
LIBS += -lX11 -lcwiid
LIBDIR = -L/usr/X11R6/lib -L../../../lib
LIBTYPE ?= dbg
OSGCONFIG := /usr/local/bin/osg-config
ADDLIB := X
CCFLAGS += $(shell $(OSGCONFIG) --cflags --$(LIBTYPE) Base System GLUT $(ADDLIB))
LDFLAGS += $(shell $(OSGCONFIG) --libs --$(LIBTYPE) Base System GLUT $(ADDLIB))

.PHONY: default compile link all clean distclean

default: all

compile: $(OBJS)

%.o : $(SRCDIR)%.cpp
	$(CXX) -c $(INCLUDES) $(OPTFLAGS) $(CCFLAGS) $< -o $@

link: $(OBJS)
	$(CXX) $(OBJS) -o $(OUTDIR)wiideviceserver $(LIBDIR) $(LIBS) $(LDFLAGS)

testDeviceStateSender: DeviceStateSender.o

testWiiDevice: DeviceStateSender.o

compile: $(OBJS)

all: compile link

clean:
	-rm -f $(OBJS)

# for distclean, also remove the executable, if it's in this directory:
distclean: clean
	-rm -f *.d
	-rm -f ./wiideviceserver


