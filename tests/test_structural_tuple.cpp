#include <gtest/gtest.h>

import structural_tuple;
import std;

using namespace structural_tuple;

TEST(StructuralTupleTest, ConstructorInitializationAndTupleTraits) {
  tuple<int, double, std::string> t{42, 3.14, "Hello"};

  EXPECT_EQ(std::tuple_size<decltype(t)>::value, 3u);
  static_assert(std::is_same_v<std::tuple_element_t<0, decltype(t)>, int>);
  static_assert(std::is_same_v<std::tuple_element_t<1, decltype(t)>, double>);
  static_assert(std::is_same_v<std::tuple_element_t<2, decltype(t)>, std::string>);

  EXPECT_EQ(get<0>(t), 42);
  EXPECT_DOUBLE_EQ(get<1>(t), 3.14);
  EXPECT_EQ(get<2>(t), "Hello");

  auto& first = get<0>(t);
  first = 7;
  EXPECT_EQ(get<0>(t), 7);

  const auto& second = get<1>(t);
  EXPECT_DOUBLE_EQ(second, 3.14);
}

TEST(StructuralTupleTest, CopyMoveAndAssignment) {
  tuple<int, std::string> original{7, "alpha"};

  tuple<int, std::string> copy = original;
  EXPECT_EQ(get<0>(copy), 7);
  EXPECT_EQ(get<1>(copy), "alpha");

  tuple<int, std::string> moved{std::move(original)};
  EXPECT_EQ(get<0>(moved), 7);
  EXPECT_EQ(get<1>(moved), "alpha");

  moved = tuple<int, std::string>{11, "beta"};
  EXPECT_EQ(get<0>(moved), 11);
  EXPECT_EQ(get<1>(moved), "beta");
}

TEST(StructuralTupleTest, StructuredBindingsAndReferenceSemantics) {
  tuple<int, double, std::string> value_tuple{42, 3.14, "Hello"};
  auto [first, second, third] = value_tuple;

  EXPECT_EQ(first, 42);
  EXPECT_DOUBLE_EQ(second, 3.14);
  EXPECT_EQ(third, "Hello");

  int payload = 8;
  tuple<int&, std::string> ref_tuple{payload, "ref"};
  auto [ref_value, text] = ref_tuple;

  ref_value = 21;
  EXPECT_EQ(payload, 21);
  EXPECT_EQ(text, "ref");
}

TEST(StructuralTupleTest, EmptyTuple) {
  tuple<> empty{};
  EXPECT_EQ(std::tuple_size<decltype(empty)>::value, 0u);
}

TEST(StructuralTupleTest, DefaultConstructionAndTraitAliases) {
  tuple<int, double> defaulted{};
  EXPECT_EQ(std::tuple_size_v<decltype(defaulted)>, 2u);
  static_assert(std::tuple_size_v<tuple<int, double>> == 2);
  static_assert(std::is_same_v<std::tuple_element_t<1, tuple<int, double>>, double>);

  tuple<char, std::string, bool> values{'x', "hello", true};
  EXPECT_EQ(get<0>(values), 'x');
  EXPECT_EQ(get<1>(values), "hello");
  EXPECT_TRUE(get<2>(values));
}

TEST(StructuralTupleTest, NestedTupleAccessAndReferenceCategory) {
  tuple<int, tuple<double, std::string>> nested{1, tuple<double, std::string>{2.5, "inner"}};

  static_assert(std::is_same_v<decltype(get<0>(nested)), int&>);
  static_assert(std::is_same_v<decltype(get<0>(std::move(nested))), int&&> ||
                std::is_same_v<decltype(get<0>(std::move(nested))), int&>);

  EXPECT_EQ(get<0>(nested), 1);
  EXPECT_DOUBLE_EQ(get<0>(get<1>(nested)), 2.5);
  EXPECT_EQ(get<1>(get<1>(nested)), "inner");
}

TEST(StructuralTupleTest, ConstCorrectnessAndValueSemantics) {
  const tuple<int, std::string, double> constant{7, "stable", 1.25};

  static_assert(std::is_same_v<decltype(get<0>(constant)), const int&>);
  static_assert(std::is_same_v<decltype(get<1>(constant)), const std::string&>);

  EXPECT_EQ(get<0>(constant), 7);
  EXPECT_EQ(get<1>(constant), "stable");
  EXPECT_DOUBLE_EQ(get<2>(constant), 1.25);
}

TEST(StructuralTupleTest, StructuredBindingsAndStdTupleLikeTraits) {
  tuple<int, double, std::string> value_tuple{42, 3.14, "Hello"};
  auto [first, second, third] = value_tuple;

  static_assert(std::tuple_size_v<decltype(value_tuple)> == 3);
  static_assert(std::tuple_size_v<tuple<int, double, std::string>> == 3);
  static_assert(std::is_same_v<std::tuple_element_t<0, decltype(value_tuple)>, int>);
  static_assert(std::is_same_v<std::tuple_element_t<1, decltype(value_tuple)>, double>);
  static_assert(std::is_same_v<std::tuple_element_t<2, decltype(value_tuple)>, std::string>);

  EXPECT_EQ(first, 42);
  EXPECT_DOUBLE_EQ(second, 3.14);
  EXPECT_EQ(third, "Hello");

  const tuple<char, bool, int> const_tuple{'a', true, 9};
  static_assert(std::tuple_size_v<decltype(const_tuple)> == 3);
  static_assert(std::is_same_v<std::tuple_element_t<1, const decltype(const_tuple)>, const bool>);
  static_assert(std::is_same_v<std::tuple_element_t<2, const decltype(const_tuple)>, const int>);

  EXPECT_EQ(get<0>(const_tuple), 'a');
  EXPECT_TRUE(get<1>(const_tuple));
}

TEST(StructuralTupleTest, TupleCatCombinesMultipleTuples) {
  tuple<int, double> left{1, 2.5};
  tuple<std::string, bool> right{"hello", true};

  auto combined = structural_tuple::tuple_cat(left, right);

  static_assert(std::tuple_size_v<decltype(combined)> == 4);
  static_assert(std::is_same_v<std::tuple_element_t<0, decltype(combined)>, int>);
  static_assert(std::is_same_v<std::tuple_element_t<1, decltype(combined)>, double>);
  static_assert(std::is_same_v<std::tuple_element_t<2, decltype(combined)>, std::string>);
  static_assert(std::is_same_v<std::tuple_element_t<3, decltype(combined)>, bool>);

  EXPECT_EQ(get<0>(combined), 1);
  EXPECT_DOUBLE_EQ(get<1>(combined), 2.5);
  EXPECT_EQ(get<2>(combined), "hello");
  EXPECT_TRUE(get<3>(combined));
}