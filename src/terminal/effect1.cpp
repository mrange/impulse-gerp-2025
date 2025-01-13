#include "precompiled.hpp"

#include "effect.hpp"

namespace {
  bitmap const meditation = get__meditation().with__foreground(col__black);

  bitmap const spiritualism = get__spiritualism().with__foreground(col__black);
}

effect_kind effect1(effect_input const & ei) {
  auto time = ei.time;

  auto df = [](float x, float y) -> float {
    const float m = 0.5;
    float l = lengthf(x,y);
    l = std::fmodf(l+(0.5F*m),m)-(0.5F*m);
    return std::abs(l)-(m*0.25F);
  };

  for (std::size_t y = 0; y < ei.screen.height; ++y) {
    auto py = (-1.F*ei.screen.height+2.F*(y+0.5F))/ei.screen.height;
    for (std::size_t x = 0; x < ei.screen.width; ++x) {
      auto px = (-1.F*ei.screen.width+2.F*(x+0.5F))/(2*ei.screen.height);

      auto px0 = px;
      auto py0 = py;

      px0 -= std::sinf(0.707f*(time+100));
      py0 -= std::sinf((time+100));

      auto px1 = px;
      auto py1 = py;

      px1 -= std::sinf(0.5F*(time+123));
      py1 -= std::sinf(0.707F*(time+123));
      float sm = 0.125F*lengthf(px, py);

      auto d0 = df(px0, py0);
      auto d1 = df(px1, py1);
      auto d  = d0;
      d = pmax(d, d1, sm);
      float dd = -d0;
      dd = pmax(dd, -d1, sm);
      d =  std::min(d, dd);

      auto col0 = palette(d+time+py);
      auto col1 = palette(d+1.5F+time*0.707F+py);
      auto col = d < 0.0 ? col0 : col1;
      ei.screen.draw__pixel(
          L' '
        , col
        , col
        , x
        , y
        );
    }
  }

  auto meditation__fade = smoothstep(music__from_nbeat(ei.beat__start+4), music__from_nbeat(ei.beat__start+5), ei.time);
  ei.screen.draw__bitmap(meditation, ei.time, 11, 1, meditation__fade);

  auto spiritualism__fade = smoothstep(music__from_nbeat(ei.beat__start+7), music__from_nbeat(ei.beat__start+8), ei.time);
  ei.screen.draw__bitmap(spiritualism, ei.time, 9, 23, spiritualism__fade);

  auto fadein   = smoothstep(music__from_nbeat(ei.beat__start), music__from_nbeat(ei.beat__start+2), ei.time);
  auto fadeout  = linstep(music__from_nbeat(ei.beat__end-4), music__from_nbeat(ei.beat__end), ei.time);

  auto love__fade = linstep(music__from_nbeat(ei.beat__start+11), music__from_nbeat(ei.beat__start+12), ei.time);

  draw__border(time, ei.screen);

  ei.screen.apply_to_all([&ei, fadein, fadeout, love__fade](auto x, auto y, auto p, auto& s, auto& f, auto& b) {
    if (love__fade > 0) {
      auto d = dheart(p);


      if (d < 0.F) {
        f = (vec3 { 1-f.z,1-f.x,1-f.y })+(1-love__fade);
        if (d > -0.066F) {
          s = L'▓';
        } else {
          s = L'█';
        }
      }
    }

    f += 1.0F-fadein;
    b += 1.0F-fadein;

    auto fol = (p - vec2 {0,-2}).length2();

    auto fof = smoothstep(-1, 1, fol-12*fadeout);
    f *= fof;
    b *= fof;
  });

  return ascii_effect;
}
