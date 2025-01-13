#pragma once

#include "common.hpp"

#include "screen.hpp"


// License: Unknown, author: Unknown, found: don't remember
inline float hash(vec2 const & co) {
  return fractf(sin(co.dot(vec2(12.9898,58.233F))) * 13758.5453F);
}

enum effect_kind {
  ascii_effect
, sixel_effect
};

struct effect_input {
  float       time        ;
  std::size_t beat__start ;
  std::size_t beat__end   ;
  screen &    screen      ;

  std::size_t viewport__width;
  std::size_t viewport__height;
};

vec3 aces_approx(vec3 v);

// License: Unknown, author: XorDev, found: https://x.com/XorDev/status/1808902860677001297
vec3 hsv2rgb_approx(float h, float s, float v);

// License: Unknown, author: XorDev, found: https://x.com/XorDev/status/1808902860677001297
vec3 palette(float a);

f__generate_color const col__white    = [](float time, std::size_t x, std::size_t y) -> vec3 { return vec3 {1,1,1}; };
f__generate_color const col__black    = [](float time, std::size_t x, std::size_t y) -> vec3 { return vec3 {0,0,0}; };
f__generate_color const col__gray     = [](float time, std::size_t x, std::size_t y) -> vec3 { return vec3 {0.5,0.5,0.5}; };
f__generate_color const col__graybar  = [](float time, std::size_t x, std::size_t y) -> vec3 {
  auto c = std::sqrtf(std::clamp<float>(y/10.0F, 0, 1));
  c = 1-c;
  return vec3 {c,c,c};
};
f__generate_color const col__magenta  = [](float time, std::size_t x, std::size_t y) -> vec3 { return vec3 {1,0,1}; };

f__generate_color const col__rainbow  = [](float time, std::size_t x, std::size_t y) -> vec3 {
  return palette(time-(x+2.0F*y)/20.F);
};

f__generate_color const col__flame  = [](float time, std::size_t x, std::size_t y) -> vec3 {
  auto c = std::clamp<float>(1-y/10.0F, 0, 1);
  return hsv2rgb_approx(0.05,0.5F,c);
};

float vnoise(vec2 p);

float dheart(vec2 p);

void draw__border(float time, screen & screen);

bitmap const & get__impulse_logo();

bitmap const & get__meditation();

bitmap const & get__spiritualism();