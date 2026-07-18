#include <gtest/gtest.h>

#include <string_view>
#include <unordered_map>

#include "NametagSizeFit.h"

using nametag::chooseFont;
using nametag::FontFit;
using nametag::Measurer;

namespace {

// Fake measurer backed by two tables keyed by fontId.
struct Fake {
  // fontId -> line count returned regardless of text (test decides per-scenario)
  std::unordered_map<int, int> wrapLines;
  // fontId -> line height in pixels
  std::unordered_map<int, int> lineHeights;
};

int fakeWrap(void* ctx, int fontId, std::string_view /*text*/, int /*wrapWidth*/) {
  return static_cast<Fake*>(ctx)->wrapLines.at(fontId);
}

int fakeLineHeight(void* ctx, int fontId) { return static_cast<Fake*>(ctx)->lineHeights.at(fontId); }

Measurer makeMeasurer(Fake& fake) {
  return Measurer{fakeWrap, fakeLineHeight, &fake};
}

}  // namespace

TEST(NametagSizeFit, PicksLargestWhenItFits) {
  Fake fake{{{48, 1}, {24, 2}, {12, 3}}, {{48, 60}, {24, 30}, {12, 15}}};
  const int candidates[] = {48, 24, 12};
  const FontFit fit = chooseFont(candidates, 3, "Hello", 800, 320, makeMeasurer(fake));
  EXPECT_EQ(fit.fontId, 48);
  EXPECT_EQ(fit.lineHeight, 60);
  EXPECT_EQ(fit.lineCount, 1);
}

TEST(NametagSizeFit, StepsDownWhenLargestOverflows) {
  // 48pt renders 3 lines at 60px = 180px, fits 320. But nothing overflows -- craft a case that does.
  // 48pt wraps into 8 lines * 60px = 480 > 320. 24pt wraps into 8 lines * 30px = 240 <= 320.
  Fake fake{{{48, 8}, {24, 8}, {12, 8}}, {{48, 60}, {24, 30}, {12, 15}}};
  const int candidates[] = {48, 24, 12};
  const FontFit fit = chooseFont(candidates, 3, "long text", 800, 320, makeMeasurer(fake));
  EXPECT_EQ(fit.fontId, 24);
  EXPECT_EQ(fit.lineCount, 8);
  EXPECT_EQ(fit.lineHeight, 30);
}

TEST(NametagSizeFit, FallsBackToSmallestWhenNothingFits) {
  // Every candidate overflows; expect the smallest to be returned as fallback.
  Fake fake{{{48, 20}, {24, 20}, {12, 20}}, {{48, 60}, {24, 30}, {12, 15}}};
  const int candidates[] = {48, 24, 12};
  const FontFit fit = chooseFont(candidates, 3, "very very long", 800, 100, makeMeasurer(fake));
  EXPECT_EQ(fit.fontId, 12);
  EXPECT_EQ(fit.lineCount, 20);
  EXPECT_EQ(fit.lineHeight, 15);
}

TEST(NametagSizeFit, SingleCandidateAlwaysReturned) {
  Fake fake{{{18, 5}}, {{18, 20}}};
  const int candidates[] = {18};
  const FontFit fit = chooseFont(candidates, 1, "x", 800, 480, makeMeasurer(fake));
  EXPECT_EQ(fit.fontId, 18);
  EXPECT_EQ(fit.lineCount, 5);
}

TEST(NametagSizeFit, ExactFitAccepted) {
  // 4 lines * 80px = 320 == regionHeight -- must be treated as "fits".
  Fake fake{{{48, 4}}, {{48, 80}}};
  const int candidates[] = {48};
  const FontFit fit = chooseFont(candidates, 1, "x", 800, 320, makeMeasurer(fake));
  EXPECT_EQ(fit.fontId, 48);
}
