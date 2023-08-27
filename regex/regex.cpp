// Copyright (c) 2021, Paul Dreik
// Licensed under Boost software license 1.0
// SPDX-License-Identifier: BSL-1.0

#include <algorithm>
#include <cstdint>
#include <exception>
#include <regex>
#include <string_view>

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{

  // split the input into two - one regex and a string to match it against

  const size_t option_bytes = 3;
  if (size < option_bytes)
    return 0;

  const auto regex_size = std::min(size - option_bytes, size_t{ data[0] });

  // pick one dialect
  const auto dialects = { std::regex::ECMAScript, std::regex::basic,
                          std::regex::extended,   std::regex::awk,
                          std::regex::grep,       std::regex::egrep };

  const auto Ndialects = std::size(dialects);
  const auto dialect = *(dialects.begin() + (data[1] % Ndialects));

  // pick options
  auto flags = dialect;
  if (data[2] & (1 << 0)) {
    flags |= std::regex::icase;
  }
  if (data[2] & (1 << 1)) {
    flags |= std::regex::nosubs;
  }
  if (data[2] & (1 << 2)) {
    flags |= std::regex::optimize;
  }
  if (data[2] & (1 << 3)) {
    flags |= std::regex::collate;
  }
  if (data[2] & (1 << 4)) {
    // too new, not supported
    flags |= std::regex::multiline;
  }

  data += option_bytes;
  size -= option_bytes;

  const std::string_view regex_pattern{ (const char*)data, regex_size };
  const std::string subject{ (const char*)data + regex_size,
                             size - regex_size };

  try {
    std::regex regex{ regex_pattern.data(), regex_pattern.size(), flags };
    [[maybe_unused]] bool ignored = std::regex_search(subject, regex);
  } catch (...) {
  }
  return 0;
}
