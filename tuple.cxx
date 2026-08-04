export module annotest:tuple;

import std;

import :utils;

using std::size_t;

namespace annotest {
template <size_t I>
struct member_name {
  // NOTE: This template only handles indices up to 9999. For I >= 10000 the
  // generated names will overflow the character array and produce undefined
  // behavior. This should never be the case. Might want to consider why you're
  // creating a test with 9999+ parameters...
  static constexpr auto internal_value() {
    if constexpr (I < 10) {
      return std::array<char, 3>{'m', static_cast<char>('0' + I), '\0'};
    } else if constexpr (I < 100) {
      return std::array<char, 4>{'m', static_cast<char>('0' + I / 10),
                                 static_cast<char>('0' + I % 10), '\0'};
    } else if constexpr (I < 1000) {
      return std::array<char, 5>{'m', static_cast<char>('0' + I / 100),
                                 static_cast<char>('0' + (I / 10) % 10),
                                 static_cast<char>('0' + I % 10), '\0'};
    } else {
      return std::array<char, 7>{'m',
                                 static_cast<char>('0' + I / 1000),
                                 static_cast<char>('0' + (I / 100) % 10),
                                 static_cast<char>('0' + (I / 10) % 10),
                                 static_cast<char>('0' + I % 10),
                                 '\0'};
    }
  }

  static constexpr auto value = member_name<I>::internal_value();
};

template <typename... Ts, std::size_t... Is>
consteval auto make_specs(std::index_sequence<Is...>) {
  // Explicitly typing the array prevents CTAD failure on empty packs
  return std::array<std::meta::info, sizeof...(Ts)>{
      std::meta::data_member_spec(^^Ts, {
                                            .name = member_name<Is>::value.data()})...};
}

export template <typename... Ts>
struct tuple {
  struct storage;

  consteval {
    auto storage_meta = ^^storage;

    // Generate the specs using an index sequence to keep 'I' constant
    auto specs = make_specs<Ts...>(std::make_index_sequence<sizeof...(Ts)>{});

    std::meta::define_aggregate(storage_meta, specs);
  }

  static constexpr auto nsdms = std::define_static_array(
      std::meta::nonstatic_data_members_of(^^storage, std::meta::access_context::current()));
  static constexpr auto types_refl =
      std::define_static_array(std::array<std::meta::info, sizeof...(Ts)>{(^^Ts)...});

  storage s;

  constexpr tuple() = default;

  constexpr tuple(const tuple&) = default;
  constexpr tuple& operator=(const tuple&) = default;

  constexpr tuple(tuple&&) = default;
  constexpr tuple& operator=(tuple&&) = default;

  constexpr ~tuple() = default;

  constexpr tuple(Ts... ts) : s{std::forward<Ts>(ts)...} {}

  static consteval auto size() -> std::size_t { return sizeof...(Ts); }

  template <std::size_t index>
  constexpr const auto& get() const {
    return s.[:nsdms[index]:];
  }
  template <std::size_t index>
  constexpr auto& get() {
    return s.[:nsdms[index]:];
  }

  template <std::meta::info member>
  constexpr const auto& get() const {
    return s.[:member:];
  }
  template <std::meta::info member>
  constexpr auto& get() {
    return s.[:member:];
  }

  consteval auto begin() { return nsdms.begin(); }
  consteval auto end() { return nsdms.end(); }
};

export template <typename T>
concept tuple_t = is_instantiation_of<tuple, T>::value;

export template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(tuple<Ts...>& t) noexcept {
  return t.template get<I>();
}

export template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(const tuple<Ts...>& t) noexcept {
  return t.template get<I>();
}

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(tuple<Ts...>&& t) noexcept {
  return std::move(t).template get<I>();
}

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(const tuple<Ts...>&& t) noexcept {
  return std::move(t).template get<I>();
}

template <tuple_t... Tuples>
consteval auto get_all_types() {
  std::vector<std::meta::info> all_types;
  all_types.reserve((Tuples::types_refl.size() + ...));

  (all_types.insert(all_types.end(), Tuples::types_refl.begin(), Tuples::types_refl.end()), ...);
  return std::define_static_array(all_types);
}

template <tuple_t Result, tuple_t Current, std::size_t Offset>
constexpr void tuple_insert(Result& result, const Current& current) {
  static constexpr auto idx_seq = std::make_index_sequence<Current::size()>{};

  template for (constexpr auto i : idx_seq) {
    result.template get<Offset + i>() = current.template get<i>();
  }
}

template <std::size_t N>
consteval auto compute_prefix_sums(const std::array<std::size_t, N>& sizes) {
  std::array<std::size_t, N> prefix_sums{};
  prefix_sums[0] = 0;
  for (std::size_t i = 1; i < N; ++i) {
    prefix_sums[i] = sizes[i] + prefix_sums[i - 1];
  }
  return prefix_sums;
}

template <tuple_t... Tuples>
constexpr auto tuple_cat(Tuples... tuples) {
  using Result = typename[:std::meta::substitute(^^tuple, get_all_types<Tuples...>()):];
  Result result;

  constexpr auto pre_sizes = std::array<std::size_t, sizeof...(Tuples)>{Tuples::size()...};
  constexpr auto sizes = compute_prefix_sums(pre_sizes);

  static constexpr auto idx_seq = std::make_index_sequence<sizeof...(Tuples)>{};

  template for (constexpr auto i : idx_seq) {
    tuple_insert<Result, decltype(tuples...[i]), sizes[i]>(result, tuples...[i]);
  }

  return result;
}
}  // namespace annotest

namespace std {
template <typename... Ts>
struct tuple_size<::annotest::tuple<Ts...>>
    : integral_constant<std::size_t, ::annotest::tuple<Ts...>::size()> {};

template <std::size_t I, typename... Ts>
struct tuple_element<I, ::annotest::tuple<Ts...>> {
  using type = Ts...[I];
};
}  // namespace std
