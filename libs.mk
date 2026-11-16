
# Find all .txt files in src/
SRCS := $(wildcard lib/*.def)


OUTS := $(patsubst lib/%.def,lib/%.lib,$(SRCS))
$(info SRCS=$(SRCS) OUTS=$(OUTS))

libs: $(OUTS)

lib/%.lib: lib/%.def
	llvm-dlltool -m i386  -d $< -l $@ -k

cleanlibs:
	rm -f $(OUTS) 