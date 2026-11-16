# set tools

ifdef GCC
CC = gcc
LD = ld
else
CC = clang-cl
LD = lld-link
endif

ifdef GCC
	LOCAL_LIBDIR = -L"lib"
else
	LOCAL_LIBDIR = /LIBPATH:"lib" 
endif

# reset values
INC_DIRS ?= $(VC_INC_DIRS)
LIB_DIRS ?= $(VC_LIB_DIRS) $(LOCAL_LIBDIR)
LDFLAGS = 
LDLIBS = 
CFLAGS = 

ifdef GCC
	CFLAGS +=  -Wno-unused-value -Wno-ignored-pragma-intrinsic -Wno-missing-prototype-for-cc
else
	CLFLAGS +=
endif


# -v

# seperate link
ifdef GCC
	CFLAGS += -c
else
	CLFLAGS += /c
endif



#
# compiler settings
#

# set 32 bits target
ifdef GCC
	CFLAGS += -m32 -march=i486
else
	CFLAGS += --target=i686-pc-windows-msvc -m32
endif

# no std include
ifdef GCC
	CFLAGS += -nostdinc
else
	CFLAGS += /X 
endif

# no default libraries
ifdef GCC
	CLFAGS += -nostdlib
else
	CFLAGS += /Zl 
endif

# no stack protection
ifdef GCC
	CFLAGS += -fno-stack-protector
else
	CFLAGS += /GS- 
endif


#
# linker settings
#


# entry
ifdef GCC
	LDFLAGS += -e WinMainCRTStartup --subsystem windows
else
	#LDFLAGS += /INCLUDE:_WinMainCRTStartup 
	LDFLAGS += /ENTRY:WinMainCRTStartup
	# image stuff yada yada
	#LDFLAGS +=  /BASE:0x00400000 /DYNAMICBASE:NO /ENTRY:WinMainCRTStartup 
endif

# LDFLAGS += /VERBOSE


ifdef GCC
else
LDFLAGS += /SAFESEH:NO   /OPT:NOREF /INCREMENTAL:NO /NXCOMPAT:NO
endif

# no default libs
ifdef GCC
	#LDFLAGS += -nodefaultlibs
else
	LDFLAGS +=/NODEFAULTLIB 
endif

# executable minimum version
ifdef GCC
	LDFLAGS += --major-subsystem-version=3 --minor-subsystem-version=10
	LDFLAGS += --major-os-version=3 --minor-os-version=10 
else
	# target windows version
	LDFLAGS +=  /SUBSYSTEM:WINDOWS,3.10
	LDFLAGS +=  /VERSION:3.10
endif

#32 bit
ifdef GCC
	LDFLAGS += -m i386pe 
else
	LDFLAGS += /MACHINE:X86
endif

# defines
DEFINES_ += _WIN32_WINNT=0x0310 WINVER=0x0310 _MSC_VER=900 WIN32 _WIN32 _X86_ _M_IX86=300

ifdef GCC
	DEFINES = $(addprefix -D, $(DEFINES_))
else
	DEFINES = /U_MSC_VER /U_M_IX86 
	DEFINES += $(addprefix /D, $(DEFINES_))
endif

#
# libraries
#

WIN32_LIBS = Comdlg32 user32 kernel32 gdi32 crtdll

# libraries
ifdef GCC
	LDLIBS += $(addprefix -l, $(WIN32_LIBS))
else
	LDLIBS += $(addsuffix .lib, $(WIN32_LIBS))
endif



#source files
SRCS    = src/main.c src/ImageRvaToVa.c src/parser.c src/debug.c src/fileio.c src/mainwindow.c src/crtstartup.c

ifdef GCC
	OBJS    = $(SRCS:src/%.c=obj/%.a)
else
	OBJS    = $(SRCS:src/%.c=obj/%.obj)
endif

#build

ifdef GCC
	OUT_FLAG = -o # dont remove, adds trailing space
else
	OUT_FLAG = /OUT:
endif


$(TARGET):  $(OBJS)
	$(LD) $(LDFLAGS) $(LIB_DIRS) $(OBJS) $(LDLIBS)  $(OUT_FLAG)dependsnt.exe
ifdef GCC
obj/%.a: src/%.c
else
obj/%.obj: src/%.c
endif
	$(CC)  $(INC_DIRS) -Iinc $(DEFINES) $(CFLAGS)  $< -o $@ 

cleanexe:
	rm -f $(OBJS) $(TARGET)
