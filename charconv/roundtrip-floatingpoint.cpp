// Copyright (c) 2023, Paul Dreik
// Licensed under Boost software license 1.0
// SPDX-License-Identifier: BSL-1.0

#include "fuzzcombine.h"
#include "limitediterator.h"
#include <charconv>
#include <cmath>
#include <cstdint>
#include <tuple>

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{

  FuzzCombiner fuzzdata(data, size);
  try {
    // arg1 will be "randomly" instantiated from one of the types in this tuple
    using ArgTypes1 = std::tuple<float, double, long double>;

    fuzzdata.combine_args<ArgTypes1>(
      []<typename Arg1>(const Arg1 arg1, FuzzCombiner* fuzzcombiner) {
        if (fuzzcombiner->is_empty())
          return;
        using enum std::chars_format;
        constexpr std::chars_format formats[] = {
          scientific,
          fixed,
          hex,
          general,
          // scientific | fixed, // already given by general
          // scientific | hex,  //<--- does not roundtrip, the hex is ignored at
          // least on libstdc++!
          fixed | hex,
          scientific | fixed | hex
        };
        const int formatselector =
          fuzzcombiner->consume_byte_in_range<0, std::size(formats) - 1>();

        char buf[8192];
        const auto ec1 = std::to_chars(
          std::begin(buf), std::end(buf), arg1, formats[formatselector]);
        assert(ec1.ec == std::errc());

        // convert back - no failure is expected
        Arg1 recovered;
        const auto ec2 = std::from_chars(
          std::cbegin(buf), ec1.ptr, recovered, formats[formatselector]);
        assert(ec2.ec == std::errc());

        // the recovered value should match the original
        if (std::isnan(arg1)) {
          assert(std::isnan(recovered));
        } else {
          assert(arg1 == recovered);
        }
      });
  } catch (...) {
  }
  return 0;
}
