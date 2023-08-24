# assume using clang with a local gcc trunk (currently gcc 14)  installation
set(GCCDIR $ENV{HOME}/local/)
foreach(fuzzer one-arg one-warg)
    add_executable(${fuzzer} ${fuzzer}.cpp)
    target_compile_options(${fuzzer} PUBLIC -nostdinc++ -nostdinc)
    # these can be obtained by executing  ~/local/bin/g++ -E -Wp,-v - </dev/null
    target_compile_options(${fuzzer} PUBLIC "SHELL:-cxx-isystem ${GCCDIR}/include/c++/14.0.0/" )
    target_compile_options(${fuzzer} PUBLIC "SHELL:-cxx-isystem ${GCCDIR}/include/c++/14.0.0/x86_64-pc-linux-gnu/" )
    target_compile_options(${fuzzer} PUBLIC "SHELL:-cxx-isystem ${GCCDIR}/lib/gcc/x86_64-pc-linux-gnu/14.0.0/include")
    target_compile_options(${fuzzer} PUBLIC "SHELL:-cxx-isystem /usr/local/include")
    target_compile_options(${fuzzer} PUBLIC "SHELL:-cxx-isystem ${GCCDIR}/include")
    target_compile_options(${fuzzer} PUBLIC "SHELL:-cxx-isystem ${GCCDIR}/lib/gcc/x86_64-pc-linux-gnu/14.0.0/include-fixed")
    target_compile_options(${fuzzer} PUBLIC "SHELL:-cxx-isystem /usr/include/x86_64-linux-gnu")
    target_compile_options(${fuzzer} PUBLIC "SHELL:-cxx-isystem /usr/include/")
    # this is needed for clang<=16
    target_compile_definitions(${fuzzer} PUBLIC __cplusplus=202302L)
    target_link_libraries(${fuzzer} PUBLIC ${GCCDIR}/lib64/libstdc++.a)
endforeach()

