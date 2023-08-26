#pragma once

#include <algorithm> // std::max
#include <bit>       // bit_ceil
#include <cassert>
#include <cstddef> //size_t
#include <cstdint> //uint8_t
#include <cstring> // memcpy
#include <string>
#include <tuple>
#include <vector>

namespace detail {
template<typename>
struct is_tuple : std::false_type
{
};
template<typename... T>
struct is_tuple<std::tuple<T...>> : std::true_type
{
};
}

template<typename T>
concept Tuple = detail::is_tuple<T>::value;

struct FuzzCombiner
{
  static constexpr std::size_t fixed_size = 16;
  FuzzCombiner(const uint8_t* data, size_t size)
    : m_data(data)
    , m_size(size)
  {
  }

  /**
   * Based on the contents of fuzz data:
   * - selects one of the types from Argset1 and instantiates it
   *   with fuzz data by bit blasting into it.
   * - selects one of the types from Argset2 and instantiates it
   *   with fuzz data by bit blasting into it.
   * invokes the given callback as callback(arg1,arg2,this);
   *
   * This is useful to test functions for templated types where
   * it takes a lot of effort to write all combinations of types.
   */
  template<Tuple Argset1, Tuple Argset2>
  void combine_args(auto callback)
  {
    combine_tuple(Argset1{}, [&](auto arg1, FuzzCombiner* fuzzcombiner) {
      fuzzcombiner->combine_tuple(Argset2{},
                                  [&](auto arg2, FuzzCombiner* fuzzcombiner) {
                                    callback(arg1, arg2, fuzzcombiner);
                                  });
    });
  }
  template<Tuple Argset1>
  void combine_args(auto callback)
  {
    combine_tuple(Argset1{}, [&](auto arg1, FuzzCombiner* fuzzcombiner) {
      callback(arg1, fuzzcombiner);
    });
  }

  /// consumes the remainder of the fuzz data
  std::vector<char> get_remainder()
  {
    std::vector<char> ret;
    auto beg = m_data;
    auto end = beg + m_size;
    m_data = end;
    m_size = 0;
    return { beg, end };
  }

  std::string get_half_remainder_as_string()
  {
    std::string ret;
    auto beg = m_data;
    auto end = beg + m_size / 2;
    m_data = end;
    m_size -= m_size / 2;
    return { beg, end };
  }

private:
  /**
   * @brief combine invokes callback with a dynamically selected
   * value of type ArgTypes as the first argument
   * @param callback will be invoked as callback(arg,this)
   */
  template<typename... ArgTypes>
  void combine(auto callback)
  {
    //    std::cout << "combine!\n";
    if (m_size < 1 + fixed_size) {
      return;
    }

    FuzzCombiner* fuzzcombiner = this;
    constexpr auto Ntypes = sizeof...(ArgTypes);
    static_assert(Ntypes > 0);
    static_assert(std::max({ sizeof(ArgTypes)... }) <= fixed_size,
                  "all args must not exceed fixed_size");
    const std::size_t runtime_selected_index = select_type<Ntypes>();
    auto instantiate_all = [&]<std::size_t... I>(std::index_sequence<I...>)
    {
      auto maybe_act =
        [&]<std::size_t Index>(
          std::integral_constant<std::size_t, Index> compile_time_index)
        -> bool {
        if (Index == runtime_selected_index) {
          using Value =
            std::tuple_element<Index, std::tuple<ArgTypes...>>::type;
          callback(consume_type<Value>(), fuzzcombiner);
          return true;
        } else {
          return false;
        }
      };
      (maybe_act(std::integral_constant<std::size_t, I>{}) || ...);
    };
    instantiate_all(std::make_index_sequence<Ntypes>{});
  }

  template<typename... ArgTypes>
  void combine_tuple(std::tuple<ArgTypes...>, auto&& callback)
  {
    combine<ArgTypes...>(std::forward<decltype(callback)>(callback));
  }

  template<std::size_t Ntypes>
  std::size_t select_type()
  {
    static_assert(Ntypes < 256, "must use at most one byte for selection");
    constexpr std::size_t mask = std::bit_ceil(1 + Ntypes) - 1;
    const std::size_t runtime_selected_index = (consume_byte() & mask);
    return runtime_selected_index;
  }

  char consume_byte()
  {
    assert(!is_empty());
    const char ret = m_data[0];
    m_data += 1;
    m_size -= 1;
    return ret;
  }

  template<typename T>
  T consume_type()
  {
    static_assert(
      std::is_trivially_copyable_v<T>,
      "T must be possible to instantiate by bit blasting using memcpy");
    assert(m_size >= sizeof(T) && sizeof(T) <= fixed_size);
    T value;
    if constexpr (std::is_same_v<T, bool>) {
      value = !!m_data[0];
    } else {
      std::memcpy(&value, m_data, sizeof(T));
    }
    m_data += fixed_size;
    m_size -= fixed_size;
    return value;
  }

  [[nodiscard]] bool is_empty() const { return m_size == 0; }
  const uint8_t* m_data;
  size_t m_size;
};
