// Copyright (c) 2023, Paul Dreik
// Licensed under Boost software license 1.0
// SPDX-License-Identifier: BSL-1.0

#include "fuzzcombine.h"
#include "limitediterator.h"
#include <charconv>
#include <cstdint>
#include <tuple>

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{

  FuzzCombiner fuzzdata(data, size);
  try {
    // arg1 will be "randomly" instantiated from one of the types in this tuple
    using ArgTypes1 = std::tuple<char,
                                 unsigned char,
                                 signed char,
                                 short,
                                 unsigned short,
                                 int,
                                 unsigned int,
                                 long,
                                 unsigned long,
                                 long long,
                                 unsigned long long>;

    fuzzdata.combine_args<ArgTypes1>(
      []<typename Arg1>(const Arg1 arg1, FuzzCombiner* fuzzcombiner) {
        if (fuzzcombiner->is_empty())
          return;
        const int base = fuzzcombiner->consume_byte_in_range<2, 36>();

        // no integers should fail to convert
        char buf[128];
        const auto ec1 =
          std::to_chars(std::begin(buf), std::end(buf), arg1, base);
        assert(ec1.ec == std::errc());

        // convert back - no failure is expected
        Arg1 recovered;
        const auto ec2 =
          std::from_chars(std::cbegin(buf), ec1.ptr, recovered, base);
        assert(ec2.ec == std::errc());

        // the recovered value should match the original
        assert(arg1 == recovered);
      });
  } catch (...) {
  }
  return 0;
}
