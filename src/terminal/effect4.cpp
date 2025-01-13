#include "precompiled.hpp"

#include "effect.hpp"

namespace {

  bitmap const impulse2 = make_bitmap(col__black, LR"BITMAP(
 ██▓ ███▄ ▄███▓ ██▓███   █    ██  ██▓      ██████ ▓█████  ▐██▌
▓██▒▓██▒▀█▀ ██▒▓██░  ██▒ ██  ▓██▒▓██▒    ▒██    ▒ ▓█   ▀  ▐██▌
▒██▒▓██    ▓██░▓██░ ██▓▒▓██  ▒██░▒██░    ░ ▓██▄   ▒███    ▐██▌
░██░▒██    ▒██ ▒██▄█▓▒ ▒▓▓█  ░██░▒██░      ▒   ██▒▒▓█  ▄  ▓██▒
░██░▒██▒   ░██▒▒██▒ ░  ░▒▒█████▓ ░██████▒▒██████▒▒░▒████▒ ▒▄▄
░▓  ░ ▒░   ░  ░▒▓▒░ ░  ░░▒▓▒ ▒ ▒ ░ ▒░▓  ░▒ ▒▓▒ ▒ ░░░ ▒░ ░ ░▀▀▒
 ▒ ░░  ░      ░░▒ ░     ░░▒░ ░ ░ ░ ░ ▒  ░░ ░▒  ░ ░ ░ ░  ░ ░  ░
 ▒ ░░      ░   ░░        ░░░ ░ ░   ░ ░   ░  ░  ░     ░       ░
 ░         ░               ░         ░  ░      ░     ░  ░ ░
                                     ╭─────╮
               ╭─────────────────────┤▄▀▄▀▄├───────────────╮
   ┼───────────┼ ▀ G L I M G L A M ▄ │▄▀▄▀▄│ ▄ L A N C E ▀ │
   │ ▄ J E Z ▀ ┼──────────┼──────────┼─────┼────┼──────────┼
   ╰───────────┼          │ ▀ L O N G S H O T ▄ │
                          ╘═════════════════════╛



               ▄▀▄▀  SEND OUR GREETINGS TO ▄▀▄▀
)BITMAP");

  bitmap const greetings = make_bitmap(col__black, LR"BITMAP(
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
░░░░░      ░░░       ░░░        ░░        ░░        ░░░      ░░░░░
▒▒▒▒  ▒▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒  ▒▒▒▒▒▒▒▒  ▒▒▒▒▒▒▒▒▒▒▒  ▒▒▒▒▒  ▒▒▒▒▒▒▒▒▒▒
▓▓▓▓  ▓▓▓   ▓▓       ▓▓▓      ▓▓▓▓      ▓▓▓▓▓▓▓  ▓▓▓▓▓▓      ▓▓▓▓▓
████  ████  ██  ███  ███  ████████  ███████████  ███████████  ████
█████      ███  ████  ██        ██        █████  ██████      █████
██████████████████████████████████████████████████████████████████

                              ♥ ♥ ♥

        ▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄
        ▀▄                                              ▀▄
        ▀▄                                              ▀▄
        ▀▄                                              ▀▄
        ▀▄                                              ▀▄
        ▀▄                                              ▀▄
        ▀▄                                              ▀▄
        ▀▄                                              ▀▄
        ▀▄                                              ▀▄
        ▀▄                                              ▀▄
        ▀▄                                              ▀▄
        ▀▄                                              ▀▄
        ▀▄                                              ▀▄
        ▀▄                                              ▀▄
        ▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄
)BITMAP");
  bitmap const greetings_0 = make_bitmap(col__black, LR"BITMAP(



            Brainless Institute
               Fisk Kompaniet
                  NewCore
                   NoCrew
                    ICE

                  and XiA
                      ───

)BITMAP");

  bitmap const greetings_1 = make_bitmap(col__black, LR"BITMAP(

              The Twitch Tribe
           ────────○○○○○○────────
  h0ffman, essenbee, fieldfxdemo, lug00ber,
  ggn2, proton, styxColor, TheCaptainCoder,
  TheGrumpyGameDev and Thindal

             Shader Superheroes
          ─────────○○○○○○─────────
  Big Wings, Byt3 M3chanic, Dave Hoskins,
  EvilRyu, Evvvil, IQ, Kali, Kishimisu,
  msm01, Nimitz, Shane, Totetmatt and XorDev

)BITMAP");

  bitmap const greetings_2 = make_bitmap(col__black, LR"BITMAP(

      Finally, greetings to everyone at:

░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
░░      ░░░░░        ░░░░       ░░░░░       ░░
▒  ▒▒▒▒▒▒▒▒▒▒  ▒▒▒▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒▒▒  ▒▒▒▒  ▒
▓  ▓▓▓   ▓▓▓▓      ▓▓▓▓▓▓       ▓▓▓▓▓       ▓▓
█  ████  ████  ██████████  ███  █████  ███████
██      █████        ████  ████  ████  ███████
██████████████████████████████████████████████
)BITMAP");

}

effect_kind effect4(effect_input const & ei) {
  auto time = ei.time;

  auto const rot1 = rotator {-1};

  for (std::size_t y = 0; y < ei.screen.height; ++y) {
    auto py = (-1.F*ei.screen.height+2.F*(y+0.5F))/ei.screen.height;
    for (std::size_t x = 0; x < ei.screen.width; ++x) {
      auto px = (-1.F*ei.screen.width+2.F*(x+0.5F) )/(2*ei.screen.height);
      auto p = vec2 {px, py};
      // To avoid division by 0
      p += 1E-3;

      auto l = p.length();
      auto r = rotator {time-l};
      r(p.x, p.y);

      auto const pcol = palette(0.707F*time+l+music__beat(time)).sqrt()*mix(0.0, 1.0, music__beat(time));
      auto ang= std::atan2(p.y, p.x);

      auto col = vec3 {0,0,0};
      if (ang < pi-2E-1) {
        p *= std::expf(-time);
        auto vp = vec2 { std::logf(l), ang*6.F/tau };
  //      auto vp = p;
        auto h = 0.F;
        auto a = 1.0F;
        for (std::size_t i = 0; i < 3; ++i) {
          h   += a*vnoise(vp);
          a   *= 0.5F;
          vp  *= 2.03F;
          rot1(vp.x, vp.y);
          vp  += 1.234;
        }
        col = palette(h-time+lengthf(px, py));
      } else {
        col = pcol;
      }

      col += 0.05;
      col *= 1/std::max(l,0.01F);
      col -= 0.25F*l;
      //col = aces_approx(col);
      ei.screen.draw__pixel(
          L' '
        , vec3 {0,0,0}
        , col
        , x
        , y
        );
    }
  }

  {
    auto bmp_sel = ((music__nbeat(time)-ei.beat__start)/16)%4;
    switch(bmp_sel) {
    case 0:
      ei.screen.draw__bitmap(impulse2   , time, 8, 6);
      break;
    default:
      ei.screen.draw__bitmap(greetings, time, 8, 2);
      break;
    }

    switch(bmp_sel) {
    case 0:
      break;
    case 1:
      ei.screen.draw__bitmap(greetings_0, time, 18, 13);
      break;
    case 2:
      ei.screen.draw__bitmap(greetings_1, time, 18, 13);
      break;
    case 3:
      ei.screen.draw__bitmap(greetings_2, time, 18, 13);
      break;
    }
  }

  {
    float fadein  = smoothstep(music__from_nbeat(ei.beat__start+1), music__from_nbeat(ei.beat__start), time);
    ei.screen.apply_to_all([&ei, time, fadein](auto x, auto y, auto p, auto& s, auto& f, auto& b) {
      f += fadein;
      b += fadein;
      float fadeout = smoothstep(music__from_nbeat(ei.beat__end-16), music__from_nbeat(ei.beat__end), time+p.length2());
      f *= 1.F-fadeout;
      b *= 1.F-fadeout;
    });
  }

  return ascii_effect;
}
