#pragma once

#include "common.hpp"

#include "vectors.hpp"

using f__generate_color = std::function<vec3 (float time, std::size_t x, std::size_t y)>;

struct bitmap {
  std::wstring          shapes;
  std::size_t           width ;
  std::size_t           height;
  f__generate_color     foreground;

  bitmap with__foreground(
      f__generate_color f
    ) const {
    auto c = *this;
    c.foreground = std::move(f);
    return c;
  }

};

struct screen {
  std::vector<wchar_t>  shapes      ;
  std::vector<vec3>     foreground  ;
  std::vector<vec3>     background  ;
  std::size_t           width       ;
  std::size_t           height      ;

  void clear() {
    shapes.clear();
    foreground.clear();
    background.clear();
    shapes.resize(width*height, L' ');
    foreground.resize(width*height  , vec3 {1,1,1});
    background.resize(width*height  , vec3 {0,0,0});
  }

  template<typename TFunc>
  void apply_to_all(TFunc f) {
    assert(width*height == shapes.size());
    assert(width*height == foreground.size());
    assert(width*height == background.size());
    for (std::size_t y = 0; y < height; ++y) {
      auto y__off = y * width;
      auto py = (-1.F*height+2.F*(y+0.5F))/height;
      for (std::size_t x = 0; x < width; ++x) {
        auto px = (-1.F*width+2.F*(x+0.5F) )/(2*height);
        auto off = x + y__off;
        f(
          x
        , y
        , vec2{ px, py }
        , shapes[off]
        , foreground[off]
        , background[off]
        );
      }
    }
  }

  void draw__pixel(
    wchar_t s
  , vec3    f
  , vec3    b
  , int     x
  , int     y
  ) {
    assert(width*height == shapes.size());
    assert(width*height == foreground.size());
    assert(width*height == background.size());
    if (y >= 0 && y < static_cast<int>(height)) {
      auto y__off = y * width;
      if (x >= 0 && x < static_cast<int>(width)) {
        auto off = x + y__off;
        shapes[off]     = s;
        foreground[off] = f;
        background[off] = b;
      }
    }
  }

  bool get__pixel(
    wchar_t & s
  , vec3    & f
  , vec3    & b
  , int     x
  , int     y
  ) {
    assert(width*height == shapes.size());
    assert(width*height == foreground.size());
    assert(width*height == background.size());
    if (y >= 0 && y < static_cast<int>(height)) {
      auto y__off = y * width;
      if (x >= 0 && x < static_cast<int>(width)) {
        auto off = x + y__off;
        s = shapes[off]     ;
        f = foreground[off] ;
        b = background[off] ;
        return true;
      }
    }

    return false;
  }

  void draw__bitmap(
    bitmap const &  bmp
  , float           time
  , int             x
  , int             y
  , float           opacity = 1
  ) {
    assert(bmp.foreground);
    std::size_t from__x = std::clamp<int>(-x, 0, bmp.width - 1);
    std::size_t from__y = std::clamp<int>(-y, 0, bmp.height- 1);

    std::size_t to__x = std::clamp<int>(x, 0, width);
    std::size_t to__y = std::clamp<int>(y, 0, height);

    std::size_t effective__width  = std::min(bmp.width  - from__x, width - to__x);
    std::size_t effective__height = std::min(bmp.height - from__y, height - to__y);

    for (std::size_t yy = 0; yy < effective__height; ++yy) {
      assert(yy + from__y < bmp.height);
      assert(yy + to__y   < height);
      auto from__off= (yy+from__y)*bmp.width;
      auto to__off  = (yy+to__y)*width;
      for (std::size_t xx = 0; xx < effective__width; ++xx) {
        assert(xx + from__x < bmp.width);
        assert(xx + to__x   < width);
        auto s = bmp.shapes[from__off+xx+from__x];
        if (s > 32) {
          auto f = bmp.foreground(time, xx+from__x, yy+from__y);
          shapes[to__off+xx+to__x]      = s;
          foreground[to__off+xx+to__x]  = mix(foreground[to__off+xx+to__x], f, opacity);
        }
      }
    }
  }
};



screen make_screen(std::size_t w, std::size_t h);
bitmap make_bitmap(
    f__generate_color foreground
  , std::wstring      pixels
  );

using ABGR = std::uint32_t;

