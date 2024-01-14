// Copyright (c) 2023, Paul Dreik
// Licensed under Boost software license 1.0
// SPDX-License-Identifier: BSL-1.0

#include "fuzzcombine.h"
#include "limitediterator.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <exception>
#include <format>
#include <iostream>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace {
template<typename T>
constexpr std::string_view
name_of_type()
{
  std::string_view name{ __PRETTY_FUNCTION__ };
  auto start = name.find("= ");
  auto stop = name.find("]", start);

  return name.substr(start + 2, stop - (start + 2));
}
}

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  //  std::cout << "invoked LLVMFuzzerTestOneInput\n";
  if (size <= 3)
    return 0;

  FuzzCombiner fuzzdata(data, size);
  try {
    using ArgTypes1 = std::tuple<bool,
                                 char,
                                 unsigned char,
                                 signed char,
                                 short,
                                 unsigned short,
                                 int,
                                 unsigned int,
                                 long,
                                 unsigned long,
                                 __int128_t,
                                 __uint128_t,
                                 float,
                                 double,
                                 long double>;
    using ArgTypes2 = ArgTypes1;
    fuzzdata.combine_args<ArgTypes1, ArgTypes2>(
      []<typename Arg1, typename Arg2>(
        Arg1 arg1, Arg2 arg2, FuzzCombiner* fuzzcombiner) {
        //        std::cout << "invoked with " << name_of_type<Arg1>() << "=" <<
        //        +arg1
        //                  << ",  " << name_of_type<Arg2>() << "=" << +arg2 <<
        //                  '\n';
        // make a separate allocation to increase likelihood of buffer
        // under/overflow
        const auto raw_fmt = fuzzcombiner->get_remainder();
        std::string_view format_string{ raw_fmt.data(),
                                        raw_fmt.data() + raw_fmt.size() };
        std::vector<char> buf(2000);
        [[maybe_unused]] auto ignored =
          std::vformat_to(LimitedIterator(buf.data(), buf.size() - 2),
                          format_string,
                          std::make_format_args(arg1, arg2));
      });
  } catch (...) {
  }
  return 0;
}
