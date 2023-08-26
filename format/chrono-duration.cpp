// Copyright (c) 2023, Paul Dreik
// Licensed under Boost software license 1.0
// SPDX-License-Identifier: BSL-1.0

#include "fuzzcombine.h"
#include "limitediterator.h"
#include <chrono>
#include <cstdint>
#include <format>
#include <ratio>
#include <string_view>
#include <tuple>
#include <vector>

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{

  FuzzCombiner fuzzdata(data, size);
  try {
    // arg1 will be "randomly" instantiated from one of the types in this tuple
    // and correspond to the underlying value of the chrono duration value
    using ArgTypes1 = std::tuple<int, long>;

    // arg2 will be "randomly" selected from one of the types, and set to be
    // the period of the duration
    using ArgTypes2 = std::tuple<std::milli, std::ratio<1>, std::kilo>;

    fuzzdata.combine_args<ArgTypes1, ArgTypes2>(
      []<typename Arg1, typename Arg2>(
        Arg1 arg1, Arg2 arg2, FuzzCombiner* fuzzcombiner) {
        // make a separate allocation to increase likelihood of buffer
        // under/overflow
        const auto raw_fmt = fuzzcombiner->get_remainder();
        std::string_view format_string{ raw_fmt.data(),
                                        raw_fmt.data() + raw_fmt.size() };
        std::vector<char> buf(2000);
        using Duration = std::chrono::duration<Arg1, Arg2>;

        [[maybe_unused]] auto ignored =
          std::vformat_to(LimitedIterator(buf.data(), buf.size() - 2),
                          format_string,
                          std::make_format_args(Duration{ arg1 }));
      });
  } catch (...) {
  }
  return 0;
}
