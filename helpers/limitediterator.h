// Copyright (c) 2023, Paul Dreik
// Licensed under Boost software license 1.0
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <exception>
#include <iterator>

struct LimitedIterator
{
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = char;
  using pointer = char*;
  using reference = char&;

  explicit LimitedIterator(char* ptr, std::size_t N)
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
  char& operator*() { return *m_ptr; }
  char* m_ptr{};
  std::size_t m_N{};
};
