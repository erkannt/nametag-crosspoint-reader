#pragma once

#include <string_view>
#include <vector>

namespace nametag {

// Split a newline-separated buffer into non-empty label views.
// Views point INTO the input buffer — do not free it while the views are in use.
// Trims trailing '\r' from lines (handles CRLF line endings from the web form)
// and skips lines that are empty or contain only whitespace.
std::vector<std::string_view> parseTexts(std::string_view buffer);

}  // namespace nametag
