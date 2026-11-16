# DependsNT
Dependency Walker for NT 3.1

Dependency Walker does not run on Windows NT 3.1

This is a (simple) alternative. should run on any Windows NT

I used the https://github.com/NickHackman/C_Vector library in my code, see `inc\vector_c\vector.h` from `src\vector.h`

# Features
Runs on any version of Windows NT, NT 3.10 thru Windows 11

Lists all imports of a PE executable

Can toggle showing only missing exports

<img width="1188" height="833" alt="image" src="https://github.com/user-attachments/assets/9a144170-c298-4d4b-9339-c37b51f50faf" />

thats it, no dynamic stuff or anything.




# Building-Prerequisites
have `make`, `clang-cl`, `lld-link`, and `llvm-dlltool`, and windows headers (`VC_INC_DIRS`)

set `VC_INC_DIRS` in your environment, when calling `make` or in user.mk

 example: `make VC_INC_DIRS=-I../vc1/INCLUDE`
 
# Actually building..

run `make` and hopefully it works!

## More info.
valid targets are `all, clean, cleanlibs, cleanexe, libs, and $(TARGET)`, `$(TARGET)` is defined as `dependsnt.exe` if `TARGET` is not defined or set in `user.mk`

libs are built first.

`.def` files exist in `lib/` which are used by `llvm-dlltool` to generate `.lib` files where DependsNT can use link to DLLs

the definitions are not complete in the `.def` files so if you try to use any functions not defined properly already, you will have to addthem to the `.def`, but this is easy.

the main program is then built

`.out` files generated from the c code is thrown into `obj/`, the generated `.exe` is thrown into the base directory.


# Structure
```
./inc/
     /c_vector/      - see @ https://github.com/NickHackman/C_Vector/tree/master/src [MIT license]
./lib/               - see ## More info
./obj/               - .obj files are deposited here
     /*.*            - .gitignore'd
./src/               - source code for the program
     /crtstartup.c   - 
     /debug.c        - functions for displaying debug messages and exiting the program on fatal error (todo: update these errors, since the release version is GUI program and not CMD-line tool, so we don't want to full exit the program most of the time.)
     /fileio.c       - code that interacts directly with the file API
     /ImageRvaToVa.c - ImageRvaToVa does not exist on NT3.1 so this functions for it
     /main.c         - Just WinMain and the window title define
     /mainwindow.c   - main window procedure and functions that interact directly with the window and sub-windows
     /parser.c       - code that extracts the import from the file loaded into memory
     /importlst.h    - global defines, (I started with a cmd line program that just printed exports of a PE file)
./clang.mk           - makefile for the program executable
./libs.mk            - makefile for the libs, lib/ directory
./makefile           - makefile that invokes the other makefiles
./*user.mk           - for user defines, .gitignore'd
./*dependsnt.exe     - generated on make, .gitignore'd
```




~~I used Msys2, specifically the `i686-w64-mingw32-gcc` compiler, but normal gcc and clang also work.~~


~~I used the INCLUDE and LIB directories from Visual C++ 1.0 to ensure I didn't accidently use anything not supported by NT 3.1
These are not included in the repository.~~

