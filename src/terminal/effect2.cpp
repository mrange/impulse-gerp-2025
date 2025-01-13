#include "precompiled.hpp"

#include "effect.hpp"

namespace {
  bitmap const code_by = make_bitmap(col__rainbow, LR"BITMAP(
 ██████╗ ██████╗ ██████╗ ███████╗    ██████╗ ██╗   ██╗
██╔════╝██╔═══██╗██╔══██╗██╔════╝    ██╔══██╗╚██╗ ██╔╝
██║     ██║   ██║██║  ██║█████╗      ██████╔╝ ╚████╔╝
██║     ██║   ██║██║  ██║██╔══╝      ██╔══██╗  ╚██╔╝
╚██████╗╚██████╔╝██████╔╝███████╗    ██████╔╝   ██║
 ╚═════╝ ╚═════╝ ╚═════╝ ╚══════╝    ╚═════╝    ╚═╝
 )BITMAP");

  bitmap const gfx_by = make_bitmap(col__rainbow, LR"BITMAP(
 ██████╗ ███████╗██╗  ██╗    ██████╗ ██╗   ██╗
██╔════╝ ██╔════╝╚██╗██╔╝    ██╔══██╗╚██╗ ██╔╝
██║  ███╗█████╗   ╚███╔╝     ██████╔╝ ╚████╔╝
██║   ██║██╔══╝   ██╔██╗     ██╔══██╗  ╚██╔╝
╚██████╔╝██║     ██╔╝ ██╗    ██████╔╝   ██║
 ╚═════╝ ╚═╝     ╚═╝  ╚═╝    ╚═════╝    ╚═╝
 )BITMAP");

  bitmap const lance = make_bitmap(col__rainbow, LR"BITMAP(
                    ___           ___           ___           ___
                   ╱╲  ╲         ╱╲  ╲         ╱╲__╲         ╱╲__╲
                  ╱::╲  ╲        ╲:╲  ╲       ╱:╱  ╱        ╱:╱ _╱_
                 ╱:╱╲:╲  ╲        ╲:╲  ╲     ╱:╱  ╱        ╱:╱ ╱╲__╲
  ___     ___   ╱:╱ ╱::╲  ╲   _____╲:╲  ╲   ╱:╱  ╱  ___   ╱:╱ ╱:╱ _╱_
 ╱╲  ╲   ╱╲__╲ ╱:╱_╱:╱╲:╲__╲ ╱::::::::╲__╲ ╱:╱__╱  ╱╲__╲ ╱:╱_╱:╱ ╱╲__╲
 ╲:╲  ╲ ╱:╱  ╱ ╲:╲╱:╱  ╲╱__╱ ╲::______╱__╱ ╲:╲  ╲ ╱:╱  ╱ ╲:╲╱:╱ ╱:╱  ╱
  ╲:╲  ╱:╱  ╱   ╲::╱__╱       ╲:╲  ╲        ╲:╲  ╱:╱  ╱   ╲::╱_╱:╱  ╱
   ╲:╲╱:╱  ╱     ╲:╲  ╲        ╲:╲  ╲        ╲:╲╱:╱  ╱     ╲:╲╱:╱  ╱
    ╲::╱  ╱       ╲:╲__╲        ╲:╲__╲        ╲::╱  ╱       ╲::╱  ╱
     ╲╱__╱         ╲╱__╱         ╲╱__╱         ╲╱__╱         ╲╱__╱
)BITMAP");

  bitmap const glimglam = make_bitmap(col__rainbow, LR"BITMAP(
┼─────────────────────────────────────────────────────╮
│   ▄▄ • ▄▄▌  ▪  • ▌ ▄ ·.  ▄▄ • ▄▄▌   ▄▄▄· • ▌ ▄ ·.   │
│  ▐█ ▀ ▪██•  ██ ·██ ▐███▪▐█ ▀ ▪██•  ▐█ ▀█ ·██ ▐███▪  │
│  ▄█ ▀█▄██▪  ▐█·▐█ ▌▐▌▐█·▄█ ▀█▄██▪  ▄█▀▀█ ▐█ ▌▐▌▐█·  │
│  ▐█▄▪▐█▐█▌▐▌▐█▌██ ██▌▐█▌▐█▄▪▐█▐█▌▐▌▐█ ▪▐▌██ ██▌▐█▌  │
│  ·▀▀▀▀ .▀▀▀ ▀▀▀▀▀  █▪▀▀▀·▀▀▀▀ .▀▀▀  ▀  ▀ ▀▀  █▪▀▀▀  │
╰─────────────────────────────────────────────────────┼
)BITMAP");

}

effect_kind effect2(effect_input const & ei) {
  auto time = ei.time;

  for (std::size_t y = 0; y < ei.screen.height; ++y) {
    auto py = (-1.F*ei.screen.height+2.F*(y+0.5F))/ei.screen.height;
    for (std::size_t x = 0; x < ei.screen.width; ++x) {
      auto px = (-1.F*ei.screen.width+2.F*(x+0.5F))/(2*ei.screen.height);

      auto p = vec2 {px, py};
      auto h0 = hash(p+std::floorf(-0.125F-0.125F*time/music__beat_time+py*py+0.33F*hash(p)));

      auto shape = L'╳';
      if (h0 > 0.55) {
        shape = L'╱';
      } else if (h0 > 0.1) {
        shape = L'╲';
      } else {
        shape = L'_';
      }
      ei.screen.draw__pixel(
          shape
        , hsv2rgb_approx(std::sinf(time*0.707F)*px*py+0.5F*py*py-0.5F*time, mix(1,0.5F,py*py), 1+py*py)*(smoothstep(0,1,py*py))
        , vec3 {0,0,0}
        , x
        , y
        );
    }
  }

  auto selection= (music__nbeat(time)-ei.beat__start)/16;
  auto nbeat    = selection*16+ei.beat__start;
  auto opacity  = std::expf(-std::max(0.F, time-music__from_nbeat(nbeat+4)));

  switch(selection) {
  case 0:
    ei.screen.draw__bitmap(gfx_by, time, 14, 12, opacity);
    break;
  case 1:
    ei.screen.draw__bitmap(glimglam, time, 12, 12, opacity);
    break;
  case 2:
    ei.screen.draw__bitmap(code_by, time, 13, 12, opacity);
    break;
  case 3:
    ei.screen.draw__bitmap(lance, time, 4, 9, opacity);
    break;
  default:
    break;
  }

  draw__border(time, ei.screen);

  {
    auto gcol = palette(-time*tau/(music__beat_time*16)-2)*0.025F;
    auto const sub = vec3(2,3,1)*0.01;
    float fadein  = smoothstep(
        music__from_nbeat(ei.beat__start+4)
      , music__from_nbeat(ei.beat__start)
      , time
    );
    float fadeout = smoothstep(
        music__from_nbeat(ei.beat__end)
      , music__from_nbeat(ei.beat__end-4)
      , time
    );
    ei.screen.apply_to_all([fadein, fadeout,sub,gcol](auto x, auto y, auto p, auto& s, auto& f, auto& b) {
      auto dot = (p*vec2 {0.707F,1.41F}).length2();
      auto add = (gcol/std::max(dot, 1E-3F)).tanh_approx();
      add -= sub*dot;
      f += add;
      b += add;
      f += fadein;
      b += fadein;
      f *= fadeout;
      b *= fadeout;
    });
  }

  return ascii_effect;
}
