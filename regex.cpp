// Copyright (c) 2021, Paul Dreik

#include <algorithm>
#include <cstdint>
#include <exception>
#include <regex>
#include <string_view>

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{

  // split the input into two - one regex and a string to match it against

  if (size < 1)
    return 0;

  const auto regex_size = std::min(size - 1, size_t{ data[0] });
  data++;
  size--;

  const std::string_view regex_pattern{ (const char*)data, regex_size };
  const std::string_view subject{ (const char*)data + regex_size,
                                  size - regex_size };

  try {
    std::regex regex{ regex_pattern.data(), regex_pattern.size() };
  } catch (...) {
  }
  return 0;
}
