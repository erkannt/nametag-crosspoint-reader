#pragma once
#include "activities/Activity.h"

// Kiosk-style sleep screen: fixed BMP on the top third + a rotating text
// label filling the bottom two thirds. Renders once in onEnter(); the caller
// (enterDeepSleep) is expected to return the device to deep sleep afterwards.
//
// isCycleContinuation must be true only on the TimerWakeup path where the
// panel already shows the previous nametag frame — that lets us use
// FAST_REFRESH for a flash-free differential paint. Every FULL_REFRESH_EVERY
// timer wakes we still do a FULL to purge accumulated ghost.
class NametagActivity final : public Activity {
 public:
  static constexpr uint32_t FULL_REFRESH_EVERY = 12;

  explicit NametagActivity(GfxRenderer& renderer, MappedInputManager& mappedInput, uint32_t textIndex = 0,
                           bool isCycleContinuation = false)
      : Activity("Nametag", renderer, mappedInput),
        textIndex(textIndex),
        isCycleContinuation(isCycleContinuation) {}
  void onEnter() override;

 private:
  uint32_t textIndex;
  bool isCycleContinuation;
};
