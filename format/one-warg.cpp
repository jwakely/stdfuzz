// Copyright (c) 2023, Paul Dreik
// Licensed under Boost software license 1.0
// SPDX-License-Identifier: BSL-1.0

#include "format"
#include <cstdint>
#include <cstring>
#include <exception>
#include <string_view>
#include <vector>

constexpr std::size_t fixed_size = 16;

template<typename Char>
struct LimitedIterator
{
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = Char;
  using pointer = Char*;
  using reference = Char&;

  explicit LimitedIterator(Char* ptr, std::size_t N)
    : m_ptr(ptr)
    , m_N(N)
  {
  }
  // Prefix increment
  LimitedIterator& operator++()
  {
    if (m_N == 0)
      throw std::runtime_error("out of data");
    ++m_ptr;
    --m_N;
    return *this;
  }
  // Postfix increment
  LimitedIterator operator++(int)
  {
    if (m_N == 0)
      throw std::runtime_error("out of data");
    LimitedIterator tmp = *this;
    ++m_ptr;
    --m_N;
    return tmp;
  }
  Char& operator*() { return *m_ptr; }
  Char* m_ptr{};
  std::size_t m_N;
};

template<typename T>
void
invoke_fmt(const uint8_t* data, size_t size)
{
  static_assert(sizeof(T) <= fixed_size, "fixed_size is too small");
  if (size <= fixed_size)
    return;
  T value{};
  if constexpr (std::is_same_v<bool, T>) {
    value = !!data[0];
  } else {
    std::memcpy(&value, data, sizeof(T));
  }

  data += fixed_size;
  size -= fixed_size;

  //  const auto* chardata = reinterpret_cast<const char*>(data);
  //  std::string format_string{ chardata, chardata + size };
  std::vector<wchar_t> blah(size / sizeof(wchar_t));
  std::memcpy(blah.data(), data, blah.size());

  //  format_string.append(10, '}');
  const std::wstring_view format_string{ blah.data(), blah.size() };

  try {
    std::vector<wchar_t> buf(2000);
    [[maybe_unused]] auto ignored =
      std::vformat_to(LimitedIterator<wchar_t>(buf.data(), buf.size() - 2),
                      format_string,
                      std::make_wformat_args(value));
  } catch (std::exception&) {
  }
}

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  //  static_assert(sizeof(wchar_t) == 4);
  if (size <= 1 + fixed_size + sizeof(wchar_t))
    return 0;

  try {

    const auto first = data[0];
    data++;
    size--;

    switch (first & 0xF) {
      case 0:
        invoke_fmt<bool>(data, size);
        break;
      case 1:
        invoke_fmt<char>(data, size);
        break;
      case 2:
        invoke_fmt<unsigned char>(data, size);
        break;
      case 3:
        invoke_fmt<signed char>(data, size);
        break;
      case 4:
        invoke_fmt<short>(data, size);
        break;
      case 5:
        invoke_fmt<unsigned short>(data, size);
        break;
      case 6:
        invoke_fmt<int>(data, size);
        break;
      case 7:
        invoke_fmt<unsigned int>(data, size);
        break;
      case 8:
        invoke_fmt<long>(data, size);
        break;
      case 9:
        invoke_fmt<unsigned long>(data, size);
        break;

      case 10:
        invoke_fmt<float>(data, size);
        break;
      case 11:
        invoke_fmt<double>(data, size);
        break;
      case 12:
        invoke_fmt<long double>(data, size);
        break;
    }
  } catch (...) {
  }
  return 0;
}
