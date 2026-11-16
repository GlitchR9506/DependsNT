
# set tools
CC = clang-cl
LD = lld-link



# reset values
INC_DIRS ?= $(VC_INC_DIRS)
LIB_DIRS ?= $(VC_LIB_DIRS) /LIBPATH:"lib" 
LDFLAGS = 
LDLIBS = 
CFLAGS = -v -Wno-unused-value -Wno-ignored-pragma-intrinsic -Wno-missing-prototype-for-cc




#
# compiler settings
#
CFLAGS += --target=i686-pc-windows-msvc -m32 # 32 bit
CFLAGS += /X # no std include
CFLAGS += /Zl # no default linker
CFLAGS += /GS- # no stack protection


#ifndef NDEBUG
#	CFLAGS += -g -DDEBUG -D_DEBUG
#endif

#
# linker settings
#

#LDFLAGS += /INCLUDE:_WinMainCRTStartup 
LDFLAGS += /ENTRY:WinMainCRTStartup
# image stuff yada yada
#LDFLAGS +=  /BASE:0x00400000 /DYNAMICBASE:NO /ENTRY:WinMainCRTStartup 

# LDFLAGS += /VERBOSE
LDFLAGS += /SAFESEH:NO   /OPT:NOREF /INCREMENTAL:NO /NXCOMPAT:NO



LDFLAGS +=/NODEFAULTLIB 

# target windows version
LDFLAGS +=  /SUBSYSTEM:WINDOWS,3.10
LDFLAGS +=  /VERSION:3.10

#32 bit
LDFLAGS += /MACHINE:X86


# defines
DEFINES += /D_WIN32_WINNT=0x0310 /DWINVER=0x0310 /U_MSC_VER /D_MSC_VER=900 /DWIN32 /D_WIN32 /D_X86_ -U_M_IX86 /D_M_IX86=300

# libraries
LDLIBS += Comdlg32.lib user32.lib kernel32.lib gdi32.lib crtdll.lib



#source files
SRCS    = src/main.c src/ImageRvaToVa.c src/parser.c src/debug.c src/fileio.c src/mainwindow.c src/crtstartup.c
OBJS    = $(SRCS:src/%.c=obj/%.obj)


#build


$(TARGET):  $(OBJS)
	$(LD) $(LDFLAGS) $(LIB_DIRS) $(OBJS) $(LDLIBS)  /OUT:dependsnt.exe
obj/%.obj: src/%.c
	$(CC)  $(INC_DIRS) -Iinc $(DEFINES) $(CFLAGS)  /c $< -o $@ 

cleanexe:
	rm -f $(OBJS) $(TARGET)
