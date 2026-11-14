CC ?= gcc
INC_DIRS  ?= ../vc1/INCLUDE
LIB_DIRS  ?= ../vc1/LIB


# compiler settings
CFLAGS = -m32 -march=i486 -mwindows 

ifndef NDEBUG
	CLFAGS += -g -DDEBUG -D_DEBUG
endif

# dont use any mingw stuff
CFLAGS += -nostdinc -nostdlib -nodefaultlibs 

# set minimum version to NT 3.1
CFLAGS += -Wl,--major-subsystem-version,3 -Wl,--minor-subsystem-version,10
CFLAGS += -Wl,--major-os-version,3 -Wl,--minor-os-version,10 

# defines
CFLAGS += -D_WIN32_WINNT=0x0310 -DWINVER=0x0310 -D_MSC_VER=900

# linker libs
LDLIBS = 

# C runtime
ifndef CRT
	CRT=MSVCRT
endif

ifeq ($(CRT),MSVCRT)
LDLIBS += -lmsvcrt 
else ifeq ($(CRT),LIBC)
LDLIBS += -static -llibcmt 
endif

# libraries
LDLIBS += -lComdlg32 -luser32 -lkernel32 -lgdi32



#source files
SOURCE_FILES = src/main.c src/ImageRvaToVa.c src/parser.c src/debug.c src/fileio.c src/mainwindow.c

#build

all: dependsnt.exe

dependsnt.exe:  $(SOURCE_FILES)
	$(CC) $(SOURCE_FILES) $(CFLAGS) -I$(INC_DIRS) -Iinc -L$(LIB_DIRS) $(LDLIBS) -o dependsnt.exe

clean:
	rm -f dependsnt.exe
