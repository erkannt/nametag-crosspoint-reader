#include "NametagActivity.h"

#include <Bitmap.h>
#include <GfxRenderer.h>
#include <HalStorage.h>
#include <I18n.h>
#include <Logging.h>
#include <NametagLayout.h>
#include <NametagText.h>

#include <string>
#include <string_view>
#include <vector>

#include "CrossPointSettings.h"
#include "fontIds.h"

namespace {

constexpr char NAMETAG_BMP_PATH[] = "/nametag.bmp";

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
      renderer.drawBitmap(bitmap, 0, 0, nametag::PANEL_W, nametag::BMP_H);
    } else {
      LOG_DBG("NTG", "Failed to parse %s headers", NAMETAG_BMP_PATH);
    }
  }

  // GfxRenderer::drawText treats `y` as the top of the line (it adds the font's
  // ascender internally to get the baseline). All positions below are top-of-line.
  // The label region always occupies the bottom two-thirds of the panel regardless
  // of whether the BMP loaded — predictable geometry, and shared with the web-UI
  // preview via NametagLayout.
  const int headerLineH = renderer.getLineHeight(UI_12_FONT_ID);
  renderer.drawText(UI_12_FONT_ID, nametag::HEADER_LEFT,
                    nametag::TEXT_REGION_Y + nametag::HEADER_PAD_TOP, tr(STR_NAMETAG_HEADER), true,
                    EpdFontFamily::BOLD);
  const int labelRegionY = nametag::TEXT_REGION_Y + nametag::headerStripH(headerLineH);
  const int labelRegionH = nametag::labelRegionH(headerLineH);

  const auto labels = nametag::parseTexts(std::string_view(SETTINGS.nametagTexts));
  if (!labels.empty()) {
    // Always render at 48pt bold with wrapping. Text that doesn't fit in
    // maxLines lines will be truncated — the web UI's live preview warns the
    // user before they save so this is a rare fallback.
    constexpr int fontId = NAMETAG_LARGE_FONT_ID;
    const int lineHeight = renderer.getLineHeight(fontId);
    const int maxLines = nametag::maxLabelLines(headerLineH, lineHeight);
    const std::string labelStr(labels[textIndex % labels.size()]);
    const auto lines = renderer.wrappedText(fontId, labelStr.c_str(), nametag::PANEL_W, maxLines);

    // Centre the top-of-line block in the label sub-region.
    const int lineCount = static_cast<int>(lines.size());
    const int blockH = lineCount * lineHeight;
    int y = labelRegionY + (labelRegionH - blockH) / 2;
    for (const auto& line : lines) {
      renderer.drawCenteredText(fontId, y, line.c_str(), true);
      y += lineHeight;
    }
  }

  renderer.displayBuffer(HalDisplay::FULL_REFRESH);

  renderer.setOrientation(priorOrientation);
}
