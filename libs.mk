
ifdef GNU_DLLTOOL
	DLLTOOL = dlltool -m i386
else
	DLLTOOL = llvm-dlltool  -m i386
endif

# Find all .txt files in src/
SRCS := $(wildcard lib/*.def)


OUTS := $(patsubst lib/%.def,lib/%.lib,$(SRCS))
$(info SRCS=$(SRCS) OUTS=$(OUTS))

libs: $(OUTS)

lib/%.lib: lib/%.def
	$(DLLTOOL) -d $< -l $@ -k

cleanlibs:
	rm -f $(OUTS) 