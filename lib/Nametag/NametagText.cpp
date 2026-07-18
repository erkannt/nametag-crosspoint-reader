#include "NametagText.h"

#include <cctype>

namespace nametag {

namespace {

bool isBlank(std::string_view s) {
  for (const char c : s) {
    if (!std::isspace(static_cast<unsigned char>(c))) return false;
  }
  return true;
}

}  // namespace

std::vector<std::string_view> parseTexts(std::string_view buffer) {
  std::vector<std::string_view> out;
  size_t start = 0;
  while (start <= buffer.size()) {
    const size_t nl = buffer.find('\n', start);
    const size_t end = (nl == std::string_view::npos) ? buffer.size() : nl;
    std::string_view line = buffer.substr(start, end - start);
    if (!line.empty() && line.back() == '\r') {
      line.remove_suffix(1);
    }
    if (!isBlank(line)) {
      out.push_back(line);
    }
    if (nl == std::string_view::npos) break;
    start = nl + 1;
  }
  return out;
}

}  // namespace nametag
