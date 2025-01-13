#include "precompiled.hpp"

#include "effect.hpp"

namespace {

  float r(float th, vec3 n, vec3 q) {
    return std::powf(powf(std::fabsf(1.0F/q.y*std::cosf(q.x*th*0.25F)), n.y) + std::powf(std::fabsf(1.0F/q.z*std::sinf(q.x*th*0.25F)), n.z), -1.0F/n.x);
  }

  struct shape {
    float r0  ;
    vec3  r1_0;
    vec3  r1_1;
    vec3  r2_0;
    vec3  r2_1;
  };

  struct vertex {
    vec3 pos;
    vec3 col;
  };


  auto const scale = vec2 {tau, pi};

  vec3 supershape(
      shape const & shape__0
    , shape const & shape__1
    , vec2 p
    , float m1
    ) {
    p -= 0.5;
    p *= scale;

    float r0    = mix(shape__0.r0   , shape__1.r0  , m1);
    vec3  r1_0  = mix(shape__0.r1_0 , shape__1.r1_0, m1);
    vec3  r1_1  = mix(shape__0.r1_1 , shape__1.r1_1, m1);
    vec3  r2_0  = mix(shape__0.r2_0 , shape__1.r2_0, m1);
    vec3  r2_1  = mix(shape__0.r2_1 , shape__1.r2_1, m1);

    float r1    = r(p.x, r1_0, r1_1);
    float r2    = r(p.y, r2_0, r2_1);


    return vec3(
        r1*std::cosf(p.x)*r2*std::cosf(p.y)
      , r1*std::sinf(p.x)*r2*std::cosf(p.y)
      , r2*std::sinf(p.y)
      )*(0.25F*r0);
  }

  shape const Rose = shape {
      4.0
    , vec3(0.2, 1.7, 1.7)
    , vec3(7.0, 1.0, 1.0)
    , vec3(0.2, 1.7, 1.7)
    , vec3(7.0, 1.0, 1.0)
  };

  shape const Pollen = shape {
      3.0
    , vec3(60.0 , 30.0, 30.0)
    , vec3(6.0  , 1.0 , 1.0 )
    , vec3(10.0 , 10.0, 10.0)
    , vec3(2.0  , 1.0 , 1.0 )
  };

  shape const HexBox = shape {
      3.0
    , vec3(60.0 , 25.0  , 25.0  )
    , vec3(6.0  , 1.0   , 1.0   )
    , vec3(250.0, 100.0 , 100.0 )
    , vec3(6.0  , 1.0   , 1.0   )
  };

  shape const Saucer = shape {
      6.0
    , vec3(0.2, 1.7, 1.7)
    , vec3(0.2, 1.0, 1.0)
    , vec3(0.5, 0.2, 0.2)
    , vec3(1.0, 1.0, 1.0)
  };

  shape const Plankton = shape {
      4.0
    , vec3(0.1, 1.0, 2.5)
    , vec3(2.6, 1.0, 1.0)
    , vec3(3.0, 0.2, 1.0)
    , vec3(3.0, 1.0, 1.0)
  };

  shape const FlatFish = shape {
      4.0
    , vec3(0.7  , 0.3   , 0.2 )
    , vec3(2.0  , 1.0   , 1.0 )
    , vec3(100.0, 100.0 , 20.0)
    , vec3(2.0  , 1.0   , 1.0 )
  };

  shape const Lantern = shape {
      8.0
    , vec3(0.2  , 1.7, 1.7)
    , vec3(20.0 , 1.0, 1.0)
    , vec3(0.5  , 0.2, 0.2)
    , vec3(6.0  , 1.0, 1.0)
  };

  shape const Hedgehog = shape {
      3.0
    , vec3(60.0, 25.0, 25.0)
    , vec3(100.0, 1.0, 1.0)
    , vec3(250.0, 100.0, 100.0)
    , vec3(40.0, 1.0, 1.0)
  };

  shape const Starflake = shape {
      5.0
    , vec3(0.5, 1.0, 1.0)
    , vec3(20.0, 1.0, 1.0)
    , vec3(0.2, 1.0, 1.0)
    , vec3(5.0, 1.0, 1.0)
  };

  shape const Crystal = shape {
      5.0
    , vec3(0.8, 0.8, 0.8)
    , vec3(3.0, 1.0, 1.0)
    , vec3(0.8, 1.5, 1.5)
    , vec3(10.0, 1.0, 1.0)
  };


}

effect_kind effect6(effect_input const & ei) {
  auto time = ei.time;

  {
    auto m = smoothstep(-0.5F, 0.5F, std::cosf(time*tau/10));
    float const zf  = 2.0;
    auto hwidth     = 0.5F*screen__width;
    auto hheight    = 0.5F*screen__height;

    auto rot0 = rotator {0.707F*time};
    auto rot1 = rotator {0.5F*time  };
    auto rot2 = rotator {0.34F*time };

    auto xf = 2.F*screen__height/screen__width;

    std::size_t constexpr dim__x = 17;
    std::size_t constexpr dim__y = 43;

    std::array<vertex, dim__x*dim__y> verticies;

    for (std::size_t y = 0; y < dim__y; ++y) {
      auto py = y/(dim__y-1.F);
      auto y__off = y*dim__x;
      for (std::size_t x = 0; x < dim__x; ++x) {
        auto px = x/(dim__x-1.F);
        auto p = vec2 {px, py};
        auto pos = supershape(HexBox, Plankton, p, m);
        rot0(pos.x,pos.y);
        rot1(pos.y,pos.z);
        rot2(pos.y,pos.x);
        verticies[y__off+x] = vertex {
          pos
        , palette((py)*tau+time)
        };
      }
    }

    std::sort(
        verticies.begin()
      , verticies.end()
      , [](auto & l, auto & r) { return l.pos.z > r.pos.z; }
      );

    for (auto & v : verticies) {
      auto zz = zf/(zf+v.pos.z);
      int xx = static_cast<int>(std::roundf(zz*hwidth*xf*v.pos.x)+hwidth);
      int yy = static_cast<int>(std::roundf(zz*hheight*v.pos.y)+hheight);
      auto col = v.col;
      auto cf = smoothstep(1, -1, v.pos.z);
      col *= cf*cf;
      /* █■▪ */
      auto shape = L'▪';
      if (v.pos.z < -0.5F*1.F/3.F) {
        shape = L'█';
      } else if (v.pos.z < 0.5F*1.F/3.F) {
        shape = L'■';
      }
      /* ·∘○◎ */
      /* ·∘●◎ */
      /*
      auto shape = L'·';
      if (v.pos.z < -0.25F) {
        shape = L'◎';
      } else if (v.pos.z < 0.F) {
        shape = L'○';
      } else if (v.pos.z < 0.25F) {
        shape = L'∘';
      }
      */
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
  }

  {
    auto gcol = palette(-time)*0.025F;
    auto sub = vec3(2,3,1)*0.0033;
    for (std::size_t y = 0; y < ei.screen.height; ++y) {
      auto py = (-1.F*ei.screen.height+2.F*(y+0.5F))/ei.screen.height;
      for (std::size_t x = 0; x < ei.screen.width; ++x) {
        auto px = (-1.F*ei.screen.width+2.F*(x+0.5F) )/(2*ei.screen.height);
        px += 1E-3;
        py += 1E-3;
        wchar_t s;
        vec3    f;
        vec3    b;
        if (ei.screen.get__pixel(s,f,b,x,y)) {
          auto dot = length2f(px,py);
          auto add = gcol/dot;
          add -= sub*dot;
          f += add;
          b += add;
          ei.screen.draw__pixel(s,f,b,x,y);
        }
      }
    }
  }

  return ascii_effect;
}
