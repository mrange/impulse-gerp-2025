#include "precompiled.hpp"

#include "effect.hpp"

namespace {

  auto const impulse_logo = get__impulse_logo().with__foreground(col__rainbow);

  struct star {
    vec3 pos;
    vec3 col;
  };

  using stars = std::array<star, 100>;

  stars create__stars_1() {
    stars res;
    for (std::size_t i = 0; i < res.size(); ++i) {
      res[i] = star {
          vec3 {
          pick_a_float(-0.5F, 0.5)
        , pick_a_float(-0.5F, 0.5)
        , pick_a_float(-0.5F, 0.5)
        }
      , palette(tau*hash(1.F*i)).sqrt()
      };
    }
    return res;
  }

  stars create__stars_2() {
    stars res;
    for (std::size_t i = 0; i < res.size(); ++i) {
      auto f = (1.F*i)/res.size();
      res[i] = star {
          vec3 {
          0.25F*std::cosf(tau*f*4)
        , 0.25F*std::sinf(tau*f*4)
        , mix(-0.5F, 0.5F, f)
        }
      , palette(tau*f).sqrt()
      };
    }
    return res;
  }

  stars universe = create__stars_1();

}

effect_kind effect5(effect_input const & ei) {
  auto time       = ei.time;

  float const zf  = 2.0;
  auto hwidth     = 0.5F*screen__width;
  auto hheight    = 0.5F*screen__height;

  auto rot0 = rotator {0.05F*std::cosf(0.5F*time)};
  auto rot1 = rotator {0.05F*std::sinf(0.23F*time)};

  for (std::size_t i = 0; i < universe.size(); ++i) {
    auto & star = universe[i];
    star.pos.z -= 0.011;
//    rot0(star.pos.x, star.pos.z);
//    rot1(star.pos.y, star.pos.z);
  }

  for (std::size_t i = 0; i < universe.size(); ++i) {
    auto & star = universe[i];
    star.pos.x = fractf(star.pos.x+0.5F)-0.5F;
    star.pos.y = fractf(star.pos.y+0.5F)-0.5F;
    star.pos.z = fractf(star.pos.z+0.5F)-0.5F;
  }

  std::sort(universe.begin(), universe.end(), [](auto & l, auto & r) { return l.pos.z > r.pos.z; });

  auto xf = 4.F*screen__height/screen__width;

  for (std::size_t i = 0; i < universe.size(); ++i) {
    auto star = universe[i];
    auto zz = zf/(zf+star.pos.z);
    int xx = static_cast<int>(std::roundf(zz*hwidth*xf*star.pos.x)+hwidth);
    int yy = static_cast<int>(std::roundf(zz*hheight*2*star.pos.y)+hheight);
    auto col = star.col;
    auto cf = smoothstep(0.5F, -0.5F, star.pos.z);
    col *= cf*cf;
    /* █■▪ */
    /*
    auto shape = L'▪';
    if (star.pos.z < -0.5F*1.F/3.F) {
      shape = L'█';
    } else if (star.pos.z < 0.5F*1.F/3.F) {
      shape = L'■';
    }
    */
    /* ·∘○◎ */
    /* ·∘●◎ */
    auto shape = L'·';
    if (star.pos.z < -0.25F) {
      shape = L'◎';
    } else if (star.pos.z < 0.F) {
      shape = L'○';
    } else if (star.pos.z < 0.25F) {
      shape = L'∘';
    }
    ei.screen.draw__pixel(
        shape
      , col
      , vec3 {0,0,0}
      , xx
      , yy
      );
    if (shape == L'█') {
      ei.screen.draw__pixel(
          shape
        , col
        , vec3 {0,0,0}
        , xx+1
        , yy
        );
    }
  }

  auto gcol = palette(-time)*0.05F;

  for (std::size_t y = 0; y < ei.screen.height; ++y) {
    auto py = (-1.F*ei.screen.height+2.F*(y+0.5F))/ei.screen.height;
    for (std::size_t x = 0; x < ei.screen.width; ++x) {
      auto px = (-1.F*ei.screen.width+2.F*(x+0.5F))/(2*ei.screen.height);
      px += 1E-3;
      py += 1E-3;
      wchar_t s;
      vec3    f;
      vec3    b;
      if (ei.screen.get__pixel(s,f,b,x,y)) {
        f += gcol/length2f(px,py);
        b += gcol/length2f(px,py);
        ei.screen.draw__pixel(s,f,b,x,y);
      }
    }
  }

  ei.screen.draw__bitmap(impulse_logo, time, 6, 7);

  draw__border(time, ei.screen);

  auto fadein = smoothstep(music__from_nbeat(ei.beat__start), music__from_nbeat(ei.beat__start+1), ei.time);
  ei.screen.apply_to_all([&ei, fadein](auto x, auto y, auto p, auto& s, auto& f, auto& b) {
    f += 1-fadein;
    b += 1-fadein;
  });

  return ascii_effect;
}
