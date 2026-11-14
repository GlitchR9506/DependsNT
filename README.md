# DependsNT
Dependency Walker for NT 3.1

Dependency Walker does not run on Windows NT 3.1

This is a (simple) alternative. should run on any Windows NT

I used the https://github.com/NickHackman/C_Vector library in my code, see `inc\vector_c\vector.h` from `src\vector.h`

# Features
Runs on any version of Windows NT
Lists all imports of a PE executable
Can toggle showing only missing exports

# Building
I used Msys2, specifically the `i686-w64-mingw32-gcc` compiler, but normal gcc and clang also work.

I used the INCLUDE and LIB directories from Visual C++ 1.0 to ensure I didn't accidently use anything not supported by NT 3.1
These are not included in the repository.

