#include "precompiled.hpp"

#include "effect.hpp"

namespace {
  bitmap const gerp = make_bitmap(col__white, LR"BITMAP(
                    I M P U L S E ♥ G O E S ♥ T O
                   ────────────○○○○○○○────────────

░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
░░░░░░░░      ░░░░░░░░░        ░░░░░░░░       ░░░░░░░░░       ░░░░░░░░
▒▒▒▒▒▒▒  ▒▒▒▒▒▒▒▒▒▒▒▒▒▒  ▒▒▒▒▒▒▒▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒▒▒
▓▓▓▓▓▓▓  ▓▓▓   ▓▓▓▓▓▓▓▓      ▓▓▓▓▓▓▓▓▓▓       ▓▓▓▓▓▓▓▓▓       ▓▓▓▓▓▓▓▓
███████  ████  ████████  ██████████████  ███  █████████  █████████████
████████      █████████        ████████  ████  ████████  █████████████
██████████████████████████████████████████████████████████████████████
)BITMAP");

  vec2 mandelmap(vec2 p, vec2 c) {
    auto z = p;
    for (std::size_t i = 0; i < 8; ++i) {
      auto zx = z.x*z.x-z.y*z.y+c.x;
      auto zy = 2*z.x*z.y+c.y;
      z.x = zx;
      z.y = zy;
    }
    return z;
  }
}

effect_kind effect7(effect_input const & ei) {
  auto time = ei.time;

  auto rot = rotator {time};
  {
    float start = music__from_nbeat(ei.beat__start+24);
    float end   = music__from_nbeat(ei.beat__start+48);
    float fade  = smoothstep(start, end, time);

    for (std::size_t y = 0; y < ei.screen.height; ++y) {
      auto py = (-1.F*ei.screen.height+2.F*(y+0.5F))/ei.screen.height;
      for (std::size_t x = 0; x < ei.screen.width; ++x) {
        auto px = (-1.F*ei.screen.width+2.F*(x+0.5F) )/(2*ei.screen.height);
        auto p = vec2 {px, py};

        auto vp= p;
        auto constexpr per = tau/6;
        auto constexpr amp = 8.0F;
        vp.x += amp*std::sinf(time*(per/amp));
        vp.y += amp*std::sinf(time*(per*0.707F/amp));
        auto n = vnoise(vp);

        auto mp = vec2 { p.y, p.x };
        mp.x -= 0.25;
        mp *= 0.4;
        auto mmp = mandelmap(mp, mp);

        auto pp = mmp;

  //      rot(pp.x, pp.y);
        pp *= (0.5F+n);
        pp.x += 0.13F*time;
        auto np = pp.round();
        auto cp = pp-np;

        auto ml = mmp.length();

        auto col = vec3 {0,0,0};

        if (ml < 2.) {
          auto d = cp.length()-0.5F*n;
          col = palette(d+time)*0.001/std::max(d*d, 0.0001F);
        }

//        col.saturate__inplace();
        col *= fade;

        ei.screen.draw__pixel(
            L' '
          , vec3 {0,0,0}
          , col
          , x
          , y
          );
      }
    }
  }

  ei.screen.draw__bitmap(gerp, time, 5, 9);

  {
    wchar_t s;
    vec3    f;
    vec3    b;
    wchar_t const t2025[] = L"2025";
    for (std::size_t i = 0; i < 4; ++i) {
      auto c = t2025[i];
      if (ei.screen.get__pixel(
          s
        , f
        , b
        , 67+i*2
        , 18
        )) {
        ei.screen.draw__pixel(
            c
          , b
          , vec3 {1,1,1}
          , 67+i*2
          , 18
          );
      }
    }
  }

  draw__border(time, ei.screen);

  {
    float start = music__from_nbeat(ei.beat__start+2);
    float end   = music__from_nbeat(ei.beat__start+32);
    float fade  = smoothstep(start, end, time);
    ei.screen.apply_to_all([fade](auto x, auto y, auto p, auto& s, auto& f, auto& b) {
      f *= fade;
      b *= fade;
    });
  }

  return ascii_effect;
}
