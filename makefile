ifneq ($(wildcard user.mk),)

include user.mk
else

$(warning User.mk does not exist.)

endif 

ifndef VC_INC_DIRS

ERROR_AND_QUIT_BEFORE_INC=1
$(warning VC_INC_DIRS is not set. set in environment or in user.mk)
$(warning example: [VC_INC_DIRS = -I../vc1/INCLUDE])

endif

ifndef VC_LIB_DIRS

ERROR_AND_QUIT_BEFORE_INC=1
$(warning VC_LIB_DIRS is not set. set in environment or in user.mk)
$(warning example: [/LIBPATH:"C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\lib"  /LIBPATH:"C:\Program Files (x86)\Windows Kits\8.0\Lib\win8\um\x86"])

endif



ifdef ERROR_AND_QUIT_BEFORE_INC
$(error error occurred, exiting...)
endif

#
# okay sanity checks over
#

TARGET  ?= dependsnt.exe




.PHONY: clean libs

all: libs $(TARGET)

clean: cleanlibs cleanexe



include libs.mk
include clang.mk
