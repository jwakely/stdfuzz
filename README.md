# stdfuzz
for fuzzing the C++ standard library

## regex
Finds issues in all three mayor implementations.

libstdc++ (gnu): Lots of issues which have already been reported by others, see duplicates of https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61582

libc++ (llvm): https://bugs.llvm.org/show_bug.cgi?id=51659

Microsoft: https://github.com/microsoft/STL/issues/2168

## charconv
