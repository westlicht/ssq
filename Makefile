# define shell
SHELL = /bin/sh

# define suffixes for implicit rules
# normally only .c .o are necessary
.SUFFIXES:
.SUFFIXES: .c .o

# you probably only want to change the target apps path
TOPDIR        = $(shell /bin/pwd)

# build tools
AS      = as
LD      = ld
CC      = gcc
CPP     = $(CC) -E
AR      = ar
NM      = nm
RANLIB  = ranlib
STRIP   = strip
OBJCOPY = objcopy
OBJDUMP = objdump

# search paths
PATHS = -I/usr/include \
	-I/usr/include/SDL

# common build flags
ARFLAGS = -crus
CFLAGS  = -Wall -g $(PATHS) `pkg-config --cflags gtk+-2.0`
LDFLAGS = -Wl,-warn-common `pkg-config --libs gtk+-2.0`

# export variables
export  ARCH TOPDIR LIBS VPATH AS LD CC CPP AR NM\
        RANLIB STRIP OBJCOPY OBJDUMP ARFLAGS CFLAGS\
        LDFLAGS PATHS



# Application Stuff ###########################################################

# Add your application name here. Leave empty if you have no application
APP1 = ssq

# and also add your object files here...
APP1_OBJS =  ssq.o \
	clock.o \
	core.o \
	config.o \
	line.o \
	list.o \
	mcontrol.o \
	mio.o \
	mmi.o \
	mout.o \
	para.o \
	param.o \
	param_class.o \
	pattern.o \
	screen.o \
	seq.o \
	sequence.o

# add libraries required by your app in ldflags style here (e.g. -lpthread)
APP1_LIBS = -lm -lpthread -lexpat -lportmidi -lporttime -lSDL_gfx


# Add your application name here. Leave empty if you have no application
APP2 = 

# and also add your object files here...
APP2_OBJS = 

# add libraries required by your app in ldflags style here (e.g. -lpthread)
APP2_LIBS =

# Library Stuff ###############################################################

# Add library name here (without extension). Leave empty if you have no library
LIBRARY =

# Add the version information of your library here
LIB_VERSION =
LIB_MINOR =

# Add your library object files here
LIB_OBJS =

# add the header files to install in $(TARGET_PREFIX)/include/$(LIBRARY) here
LIB_HEADERS =

# add libs which are required by your lib in ldflags style here. make sure they
# are position independent (shared, compiled with -fPIC). ld won't warn you. 
LIB_LIBS = 


# Test Program Stuff ##########################################################

# add the name of your test program here. leave empty if you have none
TEST_PROGRAM =

# add the objects file for your test program here.
TEST_OBJS =

# add additional libs required by your testapp in ldflags style (e.g. -lmyapi)
TEST_LIBS =









#    Don't change stuff below this line unless you know what you're doing     #

LINKERNAME = lib$(LIBRARY).so
SONAME     = $(LINKERNAME).$(LIB_VERSION)
REALNAME   = lib$(LIBRARY)-$(LIB_VERSION).$(LIB_MINOR).so
STATNAME   = lib$(LIBRARY).a

###############################################################################
# make all - compile all applications (tests not included)
###############################################################################

ifneq ($(APP1),) 
    ALL_TARGETS = $(APP1) 
endif
ifneq ($(APP2),)
    ALL_TARGETS += $(APP2)
endif
ifneq ($(LIBRARY),)
    ALL_TARGETS += libs
endif
ifneq ($(TEST_PROGRAM),)
    ALL_TARGETS += test
endif

.PHONY: all
all:            $(ALL_TARGETS)

###############################################################################
# make application - compile application
###############################################################################

$(APP1): LDFLAGS := $(LDFLAGS) $(APP1_LIBS)
$(APP1): $(APP1_OBJS)
		$(CC) $(CFLAGS) -o $@ \
		$(APP1_OBJS) $(LDFLAGS)

$(APP2): LDFLAGS := $(LDFLAGS) $(APP2_LIBS)
$(APP2): $(APP2_OBJS)
		$(CC) $(CFLAGS) -o $@ \
		$(APP2_OBJS) $(LDFLAGS)


###############################################################################
# make lib  - compile libraries
###############################################################################
.PHONY: libs
libs:
		$(MAKE) lib-static
		rm $(LIB_OBJS)
		$(MAKE) lib-shared

.PHONY: lib-static
lib-static:	$(LIB_OBJS)	
		$(AR) $(ARFLAGS) $(STATNAME) $(LIB_OBJS)

.PHONY: lib-shared
lib-shared:	CFLAGS := -fPIC $(CFLAGS)
lib-shared:	$(LIB_OBJS)
		$(CC) -shared -Wl,--no-undefined,-soname,$(SONAME)\
                      -o $(REALNAME) $(LIB_OBJS) $(LIB_LIBS)

###############################################################################
# make test - compile test applications
###############################################################################

.PHONY: test
test:           LDFLAGS := $(LDFLAGS) $(TEST_LIBS)
test:   $(TEST_OBJS)
		$(CC) $(CFLAGS) -o $(TEST_PROGRAM) \
		$(TEST_OBJS) $(LDFLAGS)

###############################################################################
# make clean - clean all compiled & generated files
###############################################################################

.PHONY: clean
clean:
		rm -f $(APP1_OBJS) \
		rm -f $(APP2_OBJS) \
		$(LIBRARY) \
		$(APP1) \
		$(APP2) \
		$(TEST_PROGRAM) \
		*.o *.so *.a 
		rm -rf doc

###############################################################################
# make doc - generate doxygen documentation
###############################################################################

.PHONY: doc
doc:
		sed -e "s,\(PROJECT_NAME *= \).*,\1$(APP1)," \
		    -e "s,\(STRIP_FROM_PATH *= \).*,\1$(TOPDIR)," \
		    -e "s,\(INPUT *= \).*,\1$(TOPDIR)," $(DOXYFILE) \
		| doxygen -

