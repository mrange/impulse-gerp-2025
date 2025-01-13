#include "precompiled.hpp"

#include "effect.hpp"

namespace {
  bitmap const impulse = make_bitmap(col__graybar, LR"BITMAP(
 ██▓ ███▄ ▄███▓ ██▓███   █    ██  ██▓      ██████ ▓█████  ▐██▌
▓██▒▓██▒▀█▀ ██▒▓██░  ██▒ ██  ▓██▒▓██▒    ▒██    ▒ ▓█   ▀  ▐██▌
▒██▒▓██    ▓██░▓██░ ██▓▒▓██  ▒██░▒██░    ░ ▓██▄   ▒███    ▐██▌
░██░▒██    ▒██ ▒██▄█▓▒ ▒▓▓█  ░██░▒██░      ▒   ██▒▒▓█  ▄  ▓██▒
░██░▒██▒   ░██▒▒██▒ ░  ░▒▒█████▓ ░██████▒▒██████▒▒░▒████▒ ▒▄▄
░▓  ░ ▒░   ░  ░▒▓▒░ ░  ░░▒▓▒ ▒ ▒ ░ ▒░▓  ░▒ ▒▓▒ ▒ ░░░ ▒░ ░ ░▀▀▒
 ▒ ░░  ░      ░░▒ ░     ░░▒░ ░ ░ ░ ░ ▒  ░░ ░▒  ░ ░ ░ ░  ░ ░  ░
 ▒ ░░      ░   ░░        ░░░ ░ ░   ░ ░   ░  ░  ░     ░       ░
 ░         ░               ░         ░  ░      ░     ░  ░ ░
)BITMAP");

  bitmap const sixel_pixel = make_bitmap(col__rainbow, LR"BITMAP(
  ████████ ██                  ██   ███████  ██                  ██
 ██░░░░░░ ░░                  ░██  ░██░░░░██░░                  ░██
░██        ██ ██   ██  █████  ░██  ░██   ░██ ██ ██   ██  █████  ░██
░█████████░██░░██ ██  ██░░░██ ░██  ░███████ ░██░░██ ██  ██░░░██ ░██
░░░░░░░░██░██ ░░███  ░███████ ░██  ░██░░░░  ░██ ░░███  ░███████ ░██
       ░██░██  ██░██ ░██░░░░  ░██  ░██      ░██  ██░██ ░██░░░░  ░██
 ████████ ░██ ██ ░░██░░██████ ███  ░██      ░██ ██ ░░██░░██████ ███
░░░░░░░░  ░░ ░░   ░░  ░░░░░░ ░░░   ░░       ░░ ░░   ░░  ░░░░░░ ░░░
)BITMAP");

  bitmap const border = make_bitmap(col__rainbow, LR"BITMAP(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)BITMAP");

}

// License: Unknown, author: Matt Taylor (https://github.com/64), found: https://64.github.io/tonemapping/
vec3 aces_approx(vec3 v) {
  auto f = [](float v) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;

    v = std::max(v, 0.0F);
    v *= 0.6F;
    return std::clamp<float>((v*(a*v+b))/(v*(c*v+d)+e), 0, 1);
  };

  return vec3 {
    f(v.x)
  , f(v.y)
  , f(v.z)
  };
}

// License: Unknown, author: XorDev, found: https://x.com/XorDev/status/1808902860677001297
vec3 hsv2rgb_approx(float h, float s, float v) {
  float r  = (std::cosf(h*tau+0)*s+2-s)*v*0.5F;
  float g  = (std::cosf(h*tau+4)*s+2-s)*v*0.5F;
  float b  = (std::cosf(h*tau+2)*s+2-s)*v*0.5F;

  return vec3 {
    r
  , g
  , b
  };
}

// License: Unknown, author: XorDev, found: https://x.com/XorDev/status/1808902860677001297
vec3 palette(float a) {
  float r  = (1+std::sinf(a+0))*0.5F;
  float g  = (1+std::sinf(a+1))*0.5F;
  float b  = (1+std::sinf(a+2))*0.5F;

  return vec3 {
    r
  , g
  , b
  };
}

float vnoise(vec2 p) {
  auto i = p.floor();
  auto f = p.fract();

  auto u = f*f*(f*-2+3);

  auto a = hash(i + vec2(0,0));
  auto b = hash(i + vec2(1,0));
  auto c = hash(i + vec2(0,1));
  auto d = hash(i + vec2(1,1));

  auto m0 = mix(a, b, u.x);
  auto m1 = mix(c, d, u.x);
  auto m2 = mix(m0, m1, u.y);

  return m2;
}

float dheart(vec2 p) {
  p.y += -0.5F;
  p.y = -p.y;
  p.x = abs(p.x);

  if( p.y+p.x>1.0 )
      return (p-vec2(0.25F,0.75F)).length() - std::sqrtf(2)/4;
  return std::sqrtf(std::min((p-vec2(0,1)).length2(),
                  (p-0.5F*std::max(p.x+p.y,0.F)).length2())) * signf(p.x-p.y);
}

void draw__border(float time, screen & screen) {
  screen.draw__bitmap(border, time, 0, 0);
}

bitmap const & get__impulse_logo() {
  static bitmap const bmp = make_bitmap(col__black, LR"BITMAP(
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
  )BITMAP");

  return bmp;
}

bitmap const & get__meditation() {
  static bitmap const bmp = make_bitmap(col__white, LR"BITMAP(
• ▌ ▄ ·. ▄▄▄ .·▄▄▄▄  ▪  ▄▄▄▄▄ ▄▄▄· ▄▄▄▄▄▪         ▐ ▄
·██ ▐███▪▀▄.▀·██▪ ██ ██ •██  ▐█ ▀█ •██  ██ ▪     •█▌▐█
▐█ ▌▐▌▐█·▐▀▀▪▄▐█· ▐█▌▐█· ▐█.▪▄█▀▀█  ▐█.▪▐█· ▄█▀▄ ▐█▐▐▌
██ ██▌▐█▌▐█▄▄▌██. ██ ▐█▌ ▐█▌·▐█ ▪▐▌ ▐█▌·▐█▌▐█▌.▐▌██▐█▌
▀▀  █▪▀▀▀ ▀▀▀ ▀▀▀▀▀• ▀▀▀ ▀▀▀  ▀  ▀  ▀▀▀ ▀▀▀ ▀█▄▀▪▀▀ █▪
)BITMAP");
  return bmp;
}

bitmap const & get__spiritualism() {
  static bitmap const bmp = make_bitmap(col__white, LR"BITMAP(
.▄▄ ·  ▄▄▄·▪  ▄▄▄  ▪  ▄▄▄▄▄▄• ▄▌ ▄▄▄· ▄▄▌  ▪  .▄▄ · • ▌ ▄ ·.
▐█ ▀. ▐█ ▄███ ▀▄ █·██ •██  █▪██▌▐█ ▀█ ██•  ██ ▐█ ▀. ·██ ▐███▪
▄▀▀▀█▄ ██▀·▐█·▐▀▀▄ ▐█· ▐█.▪█▌▐█▌▄█▀▀█ ██▪  ▐█·▄▀▀▀█▄▐█ ▌▐▌▐█·
▐█▄▪▐█▐█▪·•▐█▌▐█•█▌▐█▌ ▐█▌·▐█▄█▌▐█ ▪▐▌▐█▌▐▌▐█▌▐█▄▪▐███ ██▌▐█▌
 ▀▀▀▀ .▀   ▀▀▀.▀  ▀▀▀▀ ▀▀▀  ▀▀▀  ▀  ▀ .▀▀▀ ▀▀▀ ▀▀▀▀ ▀▀  █▪▀▀▀
)BITMAP");
  return bmp;
}

