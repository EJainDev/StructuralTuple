export module structural_tuple;

import std;

namespace structural_tuple {

constexpr std::size_t count_digits(std::size_t val) {
  if (val == 0) {
    return 1;
  }
  std::size_t count = 0;
  while (val != 0) {
    val /= 10;
    ++count;
  }
  return count;
}

template <std::size_t I>
consteval auto to_string() {
  std::string s = "m";
  s.reserve(count_digits(I) + 1);

  std::size_t temp = I;
  for (std::size_t i = s.capacity() - 1; i > 0; --i) {
    s += static_cast<char>('0' + (temp % 10));
    temp /= 10;
  }
  return std::define_static_string(s);
}

template <typename... Ts, std::size_t... Is>
consteval auto make_specs(std::index_sequence<Is...>) {
  // Explicitly typing the array prevents CTAD failure on empty packs
  return std::array<std::meta::info, sizeof...(Ts)>{
      std::meta::data_member_spec(^^Ts, {
                                            .name = to_string<Is>()})...};
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

  constexpr tuple(Ts... ts)
    requires(sizeof...(Ts) > 0)
      : s{std::forward<Ts>(ts)...} {}

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

template <typename T>
struct is_tuple : std::false_type {};

template <typename... Ts>
struct is_tuple<tuple<Ts...>> : std::true_type {};

template <typename T>
concept tuple_t = is_tuple<std::remove_cvref_t<T>>::value;

export template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(tuple<Ts...>& t) noexcept {
  return t.template get<I>();
}

export template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(const tuple<Ts...>& t) noexcept {
  return t.template get<I>();
}

export template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(tuple<Ts...>&& t) noexcept {
  return std::move(t).template get<I>();
}

export template <std::size_t I, typename... Ts>
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

export template <tuple_t... Tuples>
constexpr auto tuple_cat(Tuples&&... tuples) {
  using Result =
      typename[:std::meta::substitute(^^tuple, get_all_types<std::remove_cvref_t<Tuples>...>()):];
  Result result;

  constexpr auto pre_sizes =
      std::array<std::size_t, sizeof...(Tuples)>{std::remove_cvref_t<Tuples>::size()...};
  constexpr auto sizes = compute_prefix_sums(pre_sizes);

  static constexpr auto idx_seq = std::make_index_sequence<sizeof...(Tuples)>{};

  template for (constexpr auto i : idx_seq) {
    tuple_insert<std::remove_cvref_t<Result>, std::remove_cvref_t<decltype(tuples...[i])>,
                 sizes[i]>(result, tuples...[i]);
  }

  return result;
}
}  // namespace structural_tuple

namespace std {
template <typename... Ts>
struct tuple_size<::structural_tuple::tuple<Ts...>>
    : integral_constant<std::size_t, ::structural_tuple::tuple<Ts...>::size()> {};

template <std::size_t I, typename... Ts>
struct tuple_element<I, ::structural_tuple::tuple<Ts...>> {
  using type = Ts...[I];
};
}  // namespace std