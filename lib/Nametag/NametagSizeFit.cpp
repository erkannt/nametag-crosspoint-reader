#include "NametagSizeFit.h"

namespace nametag {

FontFit chooseFont(const int* candidates, size_t candidateCount, std::string_view text, int wrapWidth, int regionHeight,
                   const Measurer& m) {
  FontFit last{};
  for (size_t i = 0; i < candidateCount; ++i) {
    const int fontId = candidates[i];
    const int lines = m.wrap(m.ctx, fontId, text, wrapWidth);
    const int lineH = m.lineHeight(m.ctx, fontId);
    last = FontFit{fontId, lineH, lines};
    if (lines * lineH <= regionHeight) {
      return last;
    }
  }
  return last;
}

}  // namespace nametag
