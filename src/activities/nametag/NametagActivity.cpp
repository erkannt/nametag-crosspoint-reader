#include "NametagActivity.h"

#include <Bitmap.h>
#include <GfxRenderer.h>
#include <HalStorage.h>
#include <I18n.h>
#include <Logging.h>
#include <NametagText.h>

#include <string>
#include <string_view>
#include <vector>

#include "CrossPointSettings.h"
#include "fontIds.h"

namespace {

constexpr char NAMETAG_BMP_PATH[] = "/nametag.bmp";
constexpr int PANEL_W = 800;
constexpr int PANEL_H = 480;
constexpr int BMP_H = 160;

}  // namespace

void NametagActivity::onEnter() {
  Activity::onEnter();

  const auto priorOrientation = renderer.getOrientation();
  renderer.setOrientation(GfxRenderer::LandscapeCounterClockwise);
  renderer.clearScreen();

  HalFile bmpFile;
  if (Storage.openFileForRead("NTG", NAMETAG_BMP_PATH, bmpFile)) {
    Bitmap bitmap(bmpFile, /*dithering=*/true);
    if (bitmap.parseHeaders() == BmpReaderError::Ok) {
      renderer.drawBitmap(bitmap, 0, 0, PANEL_W, BMP_H);
    } else {
      LOG_DBG("NTG", "Failed to parse %s headers", NAMETAG_BMP_PATH);
    }
  }

  // The text always occupies the bottom two-thirds of the panel regardless of
  // whether the BMP loaded. Keeps the layout predictable and stops the size-fit
  // algorithm from opportunistically choosing a font that only fits when the
  // whole panel is available.
  constexpr int textRegionY = BMP_H;
  constexpr int textRegionH = PANEL_H - BMP_H;

  // GfxRenderer::drawText treats `y` as the top of the line (it adds the font's
  // ascender internally to get the baseline). All positions below are top-of-line.

  // Header near the top-left of the text region — bold 18pt, roughly triple the
  // previous 10pt UI size. Reserves a strip at the top so the size-fit label
  // below doesn't collide with it.
  constexpr int HEADER_PAD_TOP = 8;
  constexpr int HEADER_PAD_BOTTOM = 8;
  constexpr int HEADER_LEFT = 12;
  renderer.drawText(NOTOSANS_18_FONT_ID, HEADER_LEFT, textRegionY + HEADER_PAD_TOP, tr(STR_NAMETAG_HEADER), true,
                    EpdFontFamily::BOLD);
  const int headerStripH = HEADER_PAD_TOP + renderer.getLineHeight(NOTOSANS_18_FONT_ID) + HEADER_PAD_BOTTOM;
  const int labelRegionY = textRegionY + headerStripH;
  const int labelRegionH = textRegionH - headerStripH;

  const auto labels = nametag::parseTexts(std::string_view(SETTINGS.nametagTexts));
  if (!labels.empty()) {
    // Always render at 48pt bold with wrapping. Text that doesn't fit in
    // maxLines lines will be truncated — the web UI's live preview warns the
    // user before they save so this is a rare fallback.
    constexpr int fontId = NAMETAG_LARGE_FONT_ID;
    const int lineHeight = renderer.getLineHeight(fontId);
    const int maxLines = lineHeight > 0 ? labelRegionH / lineHeight : 1;
    const std::string labelStr(labels[textIndex % labels.size()]);
    const auto lines = renderer.wrappedText(fontId, labelStr.c_str(), PANEL_W, maxLines);

    // Centre the top-of-line block in the label sub-region.
    const int lineCount = static_cast<int>(lines.size());
    const int blockH = lineCount * lineHeight;
    int y = labelRegionY + (labelRegionH - blockH) / 2;
    for (const auto& line : lines) {
      renderer.drawCenteredText(fontId, y, line.c_str(), true);
      y += lineHeight;
    }
  }

  // FAST_REFRESH is a differential paint against whatever is currently on the
  // panel — safe only when isCycleContinuation, i.e. the panel already shows
  // the previous nametag frame. On any fresh entry (user going to sleep, cold
  // boot) the panel holds arbitrary content, so we FULL. We also FULL every
  // FULL_REFRESH_EVERY cycles to purge FAST-refresh ghost accumulation.
  const bool fullRefresh = !isCycleContinuation || (textIndex % NametagActivity::FULL_REFRESH_EVERY == 0);
  renderer.displayBuffer(fullRefresh ? HalDisplay::FULL_REFRESH : HalDisplay::FAST_REFRESH);

  renderer.setOrientation(priorOrientation);
}
