// Copyright (c) 2023, Paul Dreik

#include "fuzzcombine.h"
#include "limitediterator.h"
#include <chrono>
#include <sstream>
#include <string>
#include <tuple>

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  FuzzCombiner fuzzdata(data, size);
  try {
    // arg1 will be "randomly" instantiated from one of the types in this tuple
    // and correspond to the underlying value of the chrono duration value
    using ArgTypes1 = std::tuple<std::chrono::seconds,
                                 std::chrono::milliseconds,
                                 std::chrono::microseconds>;

    fuzzdata.combine_args<ArgTypes1>(
      []<typename Arg1>(Arg1 arg1, FuzzCombiner* fuzzcombiner) {
        std::string parse_buffer = fuzzcombiner->get_half_remainder_as_string();
        // make a separate allocation to increase likelihood of buffer
        // under/overflow

        const auto raw_fmt = fuzzcombiner->get_remainder();
        std::string format_string{ raw_fmt.data(),
                                   raw_fmt.data() + raw_fmt.size() };
        std::istringstream is(parse_buffer);
        is >> std::chrono::parse(format_string, arg1);
      });
  } catch (...) {
  }
  return 0;
}
