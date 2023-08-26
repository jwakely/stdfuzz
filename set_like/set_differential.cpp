#include <bit>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  std::bitset<128> reference;
  std::set<unsigned char> s1;
  std::map<unsigned char, unsigned char> s2;
  std::unordered_set<unsigned char> s3;
  std::unordered_map<unsigned char, unsigned char> s4;
  std::multimap<unsigned char, unsigned char> s5;
  std::unordered_multimap<unsigned char, unsigned char> s6;
  std::multiset<unsigned char> s7;
  std::unordered_multiset<unsigned char> s8;
  // room for flat_set and flat_map

  for (size_t i = 0; i < size; ++i) {
    const unsigned char ux = data[i];
    const signed char x = ux;
    if (x == -128)
      continue;
    if (x > 0) {
      reference.set(ux, true);
      s1.insert(ux);
      s2[ux] = 0;
      s3.insert(ux);
      s4[ux] = 0;
      s5.emplace(ux, 0);
      s6.emplace(ux, 0);
      s7.insert(ux);
      s8.insert(ux);
    } else {
      reference.set(-x, false);
      if (auto it = s1.find(-x); it != s1.end()) {
        s1.erase(it);
      }
      if (auto it = s2.find(-x); it != s2.end()) {
        s2.erase(it);
      }
      if (auto it = s3.find(-x); it != s3.end()) {
        s3.erase(it);
      }
      if (auto it = s4.find(-x); it != s4.end()) {
        s4.erase(it);
      }
      s5.erase(-x);
      s6.erase(-x);
      s7.erase(-x);
      s8.erase(-x);
    }
  }

  std::uint16_t bitmap[128] = {};
  // bit 0 - reference
  for (size_t i = 0; i < reference.size(); ++i) {
    bitmap[i] |= (reference.test(i) ? 1 : 0);
  }
  // bit 1 - std::set
  for (const auto e : s1) {
    bitmap[e] |= 0x2;
  }
  // bit 2 - std::map
  for (const auto [e, _] : s2) {
    bitmap[e] |= 0x4;
  }
  // bit 3 - std::unordered_set
  for (const auto e : s3) {
    bitmap[e] |= 0x8;
  }
  // bit 4 - std::unordered_map
  for (const auto [e, _] : s4) {
    bitmap[e] |= 0x10;
  }
  // bit 5 - std::multimap
  for (const auto [e, _] : s5) {
    bitmap[e] |= 0x20;
  }
  // bit 6 - std::unordered_multimap
  for (const auto [e, _] : s6) {
    bitmap[e] |= 0x40;
  }
  // bit 6 - std::multiset
  for (const auto [e, _] : s6) {
    bitmap[e] |= 0x40;
  }
  // bit 7 - std::multiset
  for (const auto e : s7) {
    bitmap[e] |= 0x80;
  }
  // bit 8 - std::unordered_multiset
  for (const auto e : s8) {
    bitmap[e] |= 0x100;
  }
  // each element of the bitmap should have either two bits set, or none
  for (size_t i = 0; i < 128; ++i) {
    const auto bits_set = std::popcount(bitmap[i]);
    assert(bits_set == 0 || bits_set == 9);
  }
  return 0;
}
