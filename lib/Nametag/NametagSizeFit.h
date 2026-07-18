#pragma once

#include <cstddef>
#include <string_view>

namespace nametag {

struct FontFit {
  int fontId;
  int lineHeight;
  int lineCount;
};

// Callbacks the size-fit algorithm needs from the renderer. Passed by value;
// `ctx` is opaque to the algorithm and forwarded to each callback so tests can
// use a fake and the real code can capture the GfxRenderer pointer.
struct Measurer {
  int (*wrap)(void* ctx, int fontId, std::string_view text, int wrapWidth);
  int (*lineHeight)(void* ctx, int fontId);
  void* ctx;
};

// Picks the largest candidate font whose wrapped text fits `regionHeight`.
// `candidates` is expected largest-to-smallest; when nothing fits the smallest
// is returned with its (over-budget) line count so the caller can still render
// something rather than a blank region. `candidateCount` must be >= 1.
FontFit chooseFont(const int* candidates, size_t candidateCount, std::string_view text, int wrapWidth, int regionHeight,
                   const Measurer& m);

}  // namespace nametag
