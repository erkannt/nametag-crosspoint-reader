#pragma once
#include "activities/Activity.h"

// Kiosk-style sleep screen: fixed BMP on the top third + a rotating text
// label filling the bottom two thirds. Renders once in onEnter(); the caller
// (enterDeepSleep) is expected to return the device to deep sleep afterwards.
class NametagActivity final : public Activity {
 public:
  explicit NametagActivity(GfxRenderer& renderer, MappedInputManager& mappedInput, uint32_t textIndex = 0)
      : Activity("Nametag", renderer, mappedInput), textIndex(textIndex) {}
  void onEnter() override;

 private:
  uint32_t textIndex;
};
