
ifdef GCC
	WINDRES = windres -o
	RESOURCEFILE = obj/resource.o
else
	WINDRES = llvm-rc /fo
	RESOURCEFILE = obj/resource.res
endif



res: $(RESOURCEFILE)

$(RESOURCEFILE) : src/resource.rc  src/appicon.ico
	$(WINDRES) $(RESOURCEFILE) src/resource.rc

cleanres:
	rm -f obj/resource.* 