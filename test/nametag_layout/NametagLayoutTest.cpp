#include <NametagLayout.h>
#include <gtest/gtest.h>

using nametag::headerStripH;
using nametag::labelRegionH;
using nametag::maxLabelLines;

TEST(NametagLayout, HeaderStripHeightIsPadPlusFontHeight) {
  // 4 pad-top + font + 4 pad-bottom
  EXPECT_EQ(headerStripH(24), 32);
  EXPECT_EQ(headerStripH(0), 8);
}

TEST(NametagLayout, LabelRegionShrinksByHeaderStrip) {
  // Panel below the BMP is 320px tall (480 - 160). Header strip subtracts.
  EXPECT_EQ(labelRegionH(24), 320 - 32);
  EXPECT_EQ(labelRegionH(0), 320 - 8);
}

TEST(NametagLayout, MaxLinesFloorsDivision) {
  // labelRegion(24) = 288. lineHeight 72 → 4 lines (4*72=288 exactly).
  EXPECT_EQ(maxLabelLines(/*headerLH=*/24, /*labelLH=*/72), 4);
  // lineHeight 73 → 3 lines (3*73=219, 4*73=292 overflows).
  EXPECT_EQ(maxLabelLines(24, 73), 3);
  // Real-world: 48pt bold has advanceY=136. Header UI_12 bold has advanceY=29.
  // labelRegion(29) = 279 → 2 lines (2*136=272 fits, 3*136=408 overflows).
  EXPECT_EQ(maxLabelLines(29, 136), 2);
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
