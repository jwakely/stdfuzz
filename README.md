# stdfuzz
for fuzzing the C++ standard library

## regex
Finds issues in all three major implementations.

### libstdc++ (gnu)
* Lots of issues which have already been reported by others, see duplicates of https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61582

### libc++ (llvm)
* https://bugs.llvm.org/show_bug.cgi?id=51659, migrated here: [github 51001](https://github.com/llvm/llvm-project/issues/51001)

### Microsoft
* https://github.com/microsoft/STL/issues/2168

## charconv


## format
### libstdc++ (gnu)
* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=110860 overflow in the following: ```std::format("{:f}",2e304)```
