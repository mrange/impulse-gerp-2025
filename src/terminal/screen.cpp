#include "precompiled.hpp"

#include "screen.hpp"

namespace {
  void ltrim__inplace(std::wstring & s) {
    auto b = s.begin();
    auto e = s.end();

    for (auto i = b; i != e; ++i) {
      auto c = *i;
      if (c == L'\n') {
        // Found a new line, drop all leading white space chars including the new line
        ++i;
        s.erase(b, i);
        return;
      } else if (c <= 32) {
        // Found leading whitespace, continue
      } else {
        // Found non whitespace char, assuming entire string should be preserved
        return;
      }
    }
  }

  void rtrim__inplace(std::wstring & s) {
    auto b = s.rbegin();
    auto e = s.rend();

    for (auto i = b; i != e; ++i) {
      auto c = *i;
      if (c == L'\n') {
        // Found a new line, drop all leading white space chars including the new line
        ++i;
        s.erase(i.base(), b.base());
        return;
      } else if (c <= 32) {
        // Found leading whitespace, continue
      } else {
        // Found non whitespace char, assuming entire string should be preserved
        return;
      }
    }
  }

  void trim__inplace(std::wstring & s) {
    rtrim__inplace(s);
    ltrim__inplace(s);
  }

}

bitmap make_bitmap(
    f__generate_color foreground
  , std::wstring      pixels
  ) {
  std::size_t max__width      = 0;
  std::size_t max__height     = 0;

  std::size_t current__width  = 0;

  trim__inplace(pixels);

  for (std::size_t i = 0; i < pixels.size(); ++i) {
    auto c = pixels[i];

    if (c == '\n') {
      ++max__height;
      max__width = std::max(current__width, max__width);
      current__width = 0;
    } else if (c < 32) {
      // Skip non-printable chars
    } else {
      ++current__width;
    }
  }

  if (pixels.size() > 0) {
    if (pixels.back() > 31) {
      ++max__height;
    }
  }

  assert(max__width > 0);
  assert(max__height > 0);

  std::wstring result;
  result.reserve(max__width*max__height);

  current__width = 0;
  for (std::size_t i = 0; i < pixels.size(); ++i) {
    auto c = pixels[i];

    if (c == '\n') {
      for (std::size_t j = current__width; j < max__width; ++j) {
        result.push_back(L' ');
      }
      current__width = 0;
    } else if (c < 32) {
      // Skip non-printable chars
    } else {
      result.push_back(c);
      ++current__width;
    }
  }

  if (current__width > 0) {
    for (std::size_t j = current__width; j < max__width; ++j) {
      result.push_back(L' ');
    }
  }

  assert(result.size() == max__width*max__height);

  return bitmap {
    std::move(result)
  , max__width
  , max__height
  , std::move(foreground)
  };
}


screen make_screen(std::size_t w, std::size_t h) {
  assert(w > 0);
  assert(h > 0);
  return {
    {}
  , {}
  , {}
  , w
  , h
  };
}


