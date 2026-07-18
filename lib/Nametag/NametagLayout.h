#pragma once

// Nametag panel geometry, header-only so the on-device renderer and the
// web-UI preview endpoint share the same numbers. Font metrics (line heights)
// come in as parameters — they're only known at runtime.

namespace nametag {

constexpr int PANEL_W = 800;
constexpr int PANEL_H = 480;
constexpr int BMP_H = 160;

constexpr int HEADER_PAD_TOP = 8;
constexpr int HEADER_PAD_BOTTOM = 8;
constexpr int HEADER_LEFT = 12;

// Top-of-text and total height of the region below the BMP.
constexpr int TEXT_REGION_Y = BMP_H;
constexpr int TEXT_REGION_H = PANEL_H - BMP_H;

// Vertical strip reserved at the top of the text region for the "Nametag" header.
constexpr int headerStripH(int headerLineHeight) {
  return HEADER_PAD_TOP + headerLineHeight + HEADER_PAD_BOTTOM;
}

// Remaining height available for the wrapped label text.
constexpr int labelRegionH(int headerLineHeight) {
  return TEXT_REGION_H - headerStripH(headerLineHeight);
}

// Largest wrapped-line count that fits inside labelRegionH given the label's
// line height. Guards against a zero or negative divisor: no font info yet →
// pretend one line is possible so we still render *something*.
constexpr int maxLabelLines(int headerLineHeight, int labelLineHeight) {
  if (labelLineHeight <= 0) return 1;
  return labelRegionH(headerLineHeight) / labelLineHeight;
}

// Whether a wrapped label with `lineCount` lines fits inside `maxLines`.
// Zero maxLines means nothing fits.
constexpr bool labelFits(int lineCount, int maxLines) {
  return maxLines > 0 && lineCount <= maxLines;
}

}  // namespace nametag
