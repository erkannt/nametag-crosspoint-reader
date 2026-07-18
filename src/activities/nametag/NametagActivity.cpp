#include "NametagActivity.h"

#include <Bitmap.h>
#include <GfxRenderer.h>
#include <HalStorage.h>
#include <Logging.h>
#include <NametagSizeFit.h>
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
// Upper bound the size-fit search asks wrappedText for; the text region can
// physically fit fewer lines than this at every candidate size, so this is
// really just a cap on the vector<string> returned during measurement.
constexpr int MAX_MEASURE_LINES = 32;

int wrapCb(void* ctx, int fontId, std::string_view text, int wrapWidth) {
  const auto* r = static_cast<const GfxRenderer*>(ctx);
  const std::string s(text);
  return static_cast<int>(r->wrappedText(fontId, s.c_str(), wrapWidth, MAX_MEASURE_LINES).size());
}

int lineHeightCb(void* ctx, int fontId) {
  const auto* r = static_cast<const GfxRenderer*>(ctx);
  return r->getLineHeight(fontId);
}

}  // namespace

void NametagActivity::onEnter() {
  Activity::onEnter();

  const auto priorOrientation = renderer.getOrientation();
  renderer.setOrientation(GfxRenderer::LandscapeCounterClockwise);
  renderer.clearScreen();

  bool bmpLoaded = false;
  HalFile bmpFile;
  if (Storage.openFileForRead("NTG", NAMETAG_BMP_PATH, bmpFile)) {
    Bitmap bitmap(bmpFile, /*dithering=*/true);
    if (bitmap.parseHeaders() == BmpReaderError::Ok) {
      renderer.drawBitmap(bitmap, 0, 0, PANEL_W, BMP_H);
      bmpLoaded = true;
    } else {
      LOG_DBG("NTG", "Failed to parse %s headers", NAMETAG_BMP_PATH);
    }
  }

  const int textRegionY = bmpLoaded ? BMP_H : 0;
  const int textRegionH = bmpLoaded ? (PANEL_H - BMP_H) : PANEL_H;

  const auto labels = nametag::parseTexts(std::string_view(SETTINGS.nametagTexts));
  if (!labels.empty()) {
    const std::string_view label = labels[textIndex % labels.size()];

    static constexpr int kCandidates[] = {
#ifndef OMIT_FONTS
        NAMETAG_LARGE_FONT_ID,  //
        NOTOSANS_18_FONT_ID,    //
        NOTOSANS_14_FONT_ID,    //
        NOTOSANS_12_FONT_ID,    //
#endif
        UI_12_FONT_ID,  //
        UI_10_FONT_ID,  //
        SMALL_FONT_ID,
    };
    constexpr size_t kCandidateCount = sizeof(kCandidates) / sizeof(kCandidates[0]);

    const nametag::Measurer m{&wrapCb, &lineHeightCb, const_cast<GfxRenderer*>(&renderer)};
    const auto fit = nametag::chooseFont(kCandidates, kCandidateCount, label, PANEL_W, textRegionH, m);

    const std::string labelStr(label);
    const auto lines = renderer.wrappedText(fit.fontId, labelStr.c_str(), PANEL_W, fit.lineCount);

    // Optical vertical centring: place the midpoint of the baseline stack on the
    // region centre, then shift down by half an ascender so the visible glyph mass
    // (which sits above the baseline) is what feels centred rather than the
    // ascender+descender bounding box.
    const int ascender = renderer.getFontAscenderSize(fit.fontId);
    const int regionCenter = textRegionY + textRegionH / 2;
    const int lineCount = static_cast<int>(lines.size());
    int y = regionCenter + ascender / 2 - (lineCount - 1) * fit.lineHeight / 2;
    for (const auto& line : lines) {
      renderer.drawCenteredText(fit.fontId, y, line.c_str(), true);
      y += fit.lineHeight;
    }
  }

  renderer.displayBuffer(HalDisplay::FULL_REFRESH);

  renderer.setOrientation(priorOrientation);
}
