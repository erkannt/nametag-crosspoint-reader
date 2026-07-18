#include <NametagLayout.h>
#include <gtest/gtest.h>

using nametag::headerStripH;
using nametag::labelRegionH;
using nametag::maxLabelLines;

TEST(NametagLayout, HeaderStripHeightIsPadPlusFontHeight) {
  // 8 pad-top + font + 8 pad-bottom
  EXPECT_EQ(headerStripH(24), 40);
  EXPECT_EQ(headerStripH(0), 16);
}

TEST(NametagLayout, LabelRegionShrinksByHeaderStrip) {
  // Panel below the BMP is 320px tall (480 - 160). Header strip subtracts.
  EXPECT_EQ(labelRegionH(24), 320 - 40);
  EXPECT_EQ(labelRegionH(0), 320 - 16);
}

TEST(NametagLayout, MaxLinesFloorsDivision) {
  // labelRegion = 280, lineHeight = 64 → 4 lines (4*64=256 fits, 5*64=320 overflows)
  EXPECT_EQ(maxLabelLines(/*headerLH=*/24, /*labelLH=*/64), 4);
  // labelRegion = 280, lineHeight = 70 → 4 lines exactly (4*70=280)
  EXPECT_EQ(maxLabelLines(24, 70), 4);
  // labelRegion = 280, lineHeight = 71 → 3 lines (3*71=213, 4*71=284)
  EXPECT_EQ(maxLabelLines(24, 71), 3);
}

TEST(NametagLayout, MaxLinesGuardsAgainstZeroLineHeight) {
  EXPECT_EQ(maxLabelLines(24, 0), 1);
  EXPECT_EQ(maxLabelLines(24, -1), 1);
}

TEST(NametagLayout, LabelFitsWhenLineCountWithinMax) {
  EXPECT_TRUE(nametag::labelFits(/*lineCount=*/1, /*maxLines=*/4));
  EXPECT_TRUE(nametag::labelFits(4, 4));
  EXPECT_FALSE(nametag::labelFits(5, 4));
  EXPECT_FALSE(nametag::labelFits(1, 0));
}
