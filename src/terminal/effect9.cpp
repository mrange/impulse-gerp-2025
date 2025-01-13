#include "precompiled.hpp"

#include "effect.hpp"

namespace {
  auto gen__col = [](float & fade) {
    return [&fade](float time, std::size_t x, std::size_t y) -> vec3 {
      if (fade <= 0.F) {
        return vec3();
      }
      auto col = palette(time-(x+2.0F*y)/20.F);
      col += std::expf(-2.F*std::max(fade-0.125F,0.F));
      return mix(col, vec3(0.025, 0.075, 0.125), smoothstep(0.5F, 1.0F, fade));
    };
  };

  float meditation__fade;
  f__generate_color const meditation__col  = gen__col(meditation__fade);
  bitmap const meditation = get__meditation().with__foreground(meditation__col);

  float spiritualism__fade;
  f__generate_color const spiritualism__col  = gen__col(spiritualism__fade);
  bitmap const spiritualism = get__spiritualism().with__foreground(spiritualism__col);

}

effect_kind effect9(effect_input const & ei) {
  meditation__fade = linstep(music__from_nbeat(ei.beat__start), music__from_nbeat(ei.beat__start+5), ei.time);
  ei.screen.draw__bitmap(meditation, ei.time, 11, 4);
  spiritualism__fade = linstep(music__from_nbeat(ei.beat__start+3), music__from_nbeat(ei.beat__start+8), ei.time);
  ei.screen.draw__bitmap(spiritualism, ei.time, 9, 19);

  auto fadeout = smoothstep(music__from_nbeat(ei.beat__end-4), music__from_nbeat(ei.beat__end), ei.time);
  auto heart__fade = linstep(music__from_nbeat(ei.beat__start+7), music__from_nbeat(ei.beat__start+12), ei.time);
  if (music__nbeat(ei.time) > static_cast<int>(ei.beat__start) + 7) {
    ei.screen.apply_to_all([&ei, fadeout, heart__fade](auto x, auto y, auto p, auto& s, auto& f, auto& b) {
      //auto h = hash(p);

      auto d = dheart(p) - fadeout*screen__width/screen__height;

      if (d < 0.F) {
        auto col = hsv2rgb_approx(
            0.95F+(p.y)/8.0F
          , std::clamp(0.75F + (0.125F*p.x+p.y)/2.F, 0.F, 1.F)
          , std::clamp(1.0F-p.y, 0.F, 1.F)
          );
        col += std::expf(-4.F*std::max(heart__fade-0.125F,0.F));
        f = col;
        if (d > -0.066F) {
          s = L'▓';
          f *= f;
        } else {
          s = L'█';
        }
        f *= 1-fadeout;
      }
    });
  }


  return ascii_effect;
}
