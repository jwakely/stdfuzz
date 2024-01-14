// Copyright (c) 2021, Paul Dreik
// Licensed under Boost software license 1.0
// SPDX-License-Identifier: BSL-1.0

#include <charconv>
#include <cstdint>
#include <exception>

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  try {
    // skip forward a fixed size so the different cases pollinate each other
    // well
    constexpr size_t N = 8;
    if (size <= N)
      return 0;

    // pick a random type
    const unsigned action = data[0];

    // pick a random base
    const unsigned base = 2 + (data[1] & 0x3F);
    if (base > 36) {
      return 0;
    }

    // pick a random option
    constexpr std::chars_format options[] = {
      std::chars_format::scientific,
      std::chars_format::fixed,
      std::chars_format::hex,
      std::chars_format::general,
    };
    [[maybe_unused]] const auto option = options[data[2] & 0b11];

    data += N;
    size -= N;

    auto invoke_integral = [&](auto dummy) {
      auto whatever = std::from_chars(
        (const char*)data, (const char*)data + size, dummy, base);
      return whatever;
    };

    auto invoke_float = [&](auto /*dummy*/) {
#if 0
      auto whatever = std::from_chars(
        (const char*)data, (const char*)data + size, dummy, option);
      return whatever;
#endif
    };

    switch (action) {
      case 0:
        // invoke_integral(bool{});
        break;
      case 1:
        invoke_integral(char{});
        break;
      case 2:
        invoke_integral((unsigned char)0);
        break;
      case 3:
        invoke_integral((signed char)0);
        break;
      case 4:
        invoke_integral((short)0);
        break;
      case 5:
        invoke_integral((unsigned short)0);
        break;
      case 6:
        invoke_integral((int)0);
        break;
      case 7:
        invoke_integral((unsigned int)0);
        break;
      case 8:
        invoke_integral((long)0);
        break;
      case 9:
        invoke_integral((unsigned long)0);
        break;
      case 10:
        invoke_integral((long long)0);
        break;
      case 11:
        invoke_integral((unsigned long long)0);
        break;
      case 12:
        invoke_float((float)0);
      case 13:
        invoke_float((double)0);
      case 14:
        invoke_float((long double)0);
        break;
    }
  } catch (std::exception&) {
  }
  return 0;
}
