#include <gtest/gtest.h>

#include <string_view>
#include <vector>

#include "NametagText.h"

using nametag::parseTexts;
using namespace std::string_view_literals;

TEST(NametagText, EmptyBufferYieldsEmpty) {
  EXPECT_TRUE(parseTexts("").empty());
}

TEST(NametagText, SingleLine) {
  const auto out = parseTexts("Alice");
  ASSERT_EQ(out.size(), 1u);
  EXPECT_EQ(out[0], "Alice"sv);
}

TEST(NametagText, MultipleLines) {
  const auto out = parseTexts("Alice\nBob\nCarol");
  ASSERT_EQ(out.size(), 3u);
  EXPECT_EQ(out[0], "Alice"sv);
  EXPECT_EQ(out[1], "Bob"sv);
  EXPECT_EQ(out[2], "Carol"sv);
}

TEST(NametagText, TrailingNewlineIsNotAnExtraEntry) {
  const auto out = parseTexts("Alice\nBob\n");
  ASSERT_EQ(out.size(), 2u);
  EXPECT_EQ(out[1], "Bob"sv);
}

TEST(NametagText, EmptyLinesSkipped) {
  const auto out = parseTexts("Alice\n\n\nBob");
  ASSERT_EQ(out.size(), 2u);
  EXPECT_EQ(out[0], "Alice"sv);
  EXPECT_EQ(out[1], "Bob"sv);
}

TEST(NametagText, CrlfLineEndingsStripped) {
  const auto out = parseTexts("Alice\r\nBob\r\n");
  ASSERT_EQ(out.size(), 2u);
  EXPECT_EQ(out[0], "Alice"sv);
  EXPECT_EQ(out[1], "Bob"sv);
}

TEST(NametagText, WhitespaceOnlyLinesSkipped) {
  const auto out = parseTexts("Alice\n   \n\t\nBob");
  ASSERT_EQ(out.size(), 2u);
  EXPECT_EQ(out[0], "Alice"sv);
  EXPECT_EQ(out[1], "Bob"sv);
}

TEST(NametagText, ViewsPointIntoInputBuffer) {
  const std::string buffer = "Alice\nBob";
  const auto out = parseTexts(buffer);
  ASSERT_EQ(out.size(), 2u);
  EXPECT_GE(out[0].data(), buffer.data());
  EXPECT_LT(out[0].data() + out[0].size(), buffer.data() + buffer.size() + 1);
}
