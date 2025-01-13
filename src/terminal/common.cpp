#include "precompiled.hpp"

#include "common.hpp"

namespace {
  std::mt19937      random__generator { 19740531 };
}


float music__beat(float time) {
  return std::expf(-1.F*std::max(fractf((time+0.043F)/music__beat_time), 0.F));
}

float music__drum(float time) {
  return std::expf(-1.F*std::max(fractf((0.25F+0.5F*time)/music__beat_time), 0.F));
}

int music__nbeat(float time) {
  return static_cast<int>(std::floorf(time/music__beat_time));
}

int music__nsubdivision(float time) {
  return static_cast<int>(std::floorf(time/music__subdivision_time));
}

int music__nbar(float time) {
  return static_cast<int>(std::floorf(time/music__bar_time));
}

float music__fbeat(float time) {
  return fractf(time/music__beat_time);
}

float music__fsubdivision(float time) {
  return fractf(time/music__subdivision_time);
}

float music__fbar(float time) {
  return fractf(time/music__bar_time);
}

float music__from_nbeat (int nbeat) {
  return nbeat*music__beat_time;
}


std::size_t pick_a_number(std::size_t min, std::size_t max) {
  assert(max >= min);
  std::uniform_int_distribution<std::size_t> dist(min, max);
  return dist(random__generator);
}

float pick_a_float(float min, float max) {
  assert(max >= min);
  std::uniform_real_distribution<float> dist(min, max);
  return dist(random__generator);
}

float linstep(float edge0, float edge1, float x) {
  float t = std::clamp<float>((x - edge0) / (edge1 - edge0), 0.0F, 1.0F);
  return t;
}

float smoothstep(float edge0, float edge1, float x) {
  float t = std::clamp<float>((x - edge0) / (edge1 - edge0), 0.0F, 1.0F);
  return t * t * (3.0F - 2.0F * t);
}

// License: Unknown, author: Claude Brezinski, found: https://mathr.co.uk/blog/2017-09-06_approximating_hyperbolic_tangent.html
float tanh_approxf(float x) {
  //  Found this somewhere on the interwebs
  //  return tanh(x);
  float x2 = x*x;
  return std::clamp<float>(x*(27 + x2)/(27+9*x2), -1, 1);
}

// License: MIT, author: Inigo Quilez, found: https://www.iquilezles.org/www/articles/smin/smin.htm
float pmin(float a, float b, float k) {
  float h = std::clamp<float>(0.5F+0.5F*(b-a)/k, 0.0F, 1.0F);
  return mix(b, a, h) - k*h*(1.0F-h);
}

// License: CC0, author: Mårten Rånge, found: https://github.com/mrange/glsl-snippets
float pmax(float a, float b, float k) {
  return -pmin(-a, -b, k);
}

void check_condition(
    bool          condition
  , char const *  condition_
  , char const *  file_name
  , int           line_no
  ) {
  if (!condition) {
    char buffer[1024];
    auto r = sprintf_s(buffer, "%s(%d): Check condition failed: %s", file_name, line_no, condition_);
    if (r > -1) {
      throw std::runtime_error(buffer);
    } else {
      throw std::runtime_error("Check condition failed: Unknown reason");
    }
  }
}


void check_hresult(
    HRESULT       condition
  , char const *  condition_
  , char const *  file_name
  , int           line_no
  ) {
  if (FAILED(condition)) {
    char buffer[1024];
    auto r = sprintf_s(buffer, "%s(%d): Check hresult failed (0x%x): %s", file_name, line_no, condition, condition_);
    if (r > -1) {
      throw std::runtime_error(buffer);
    } else {
      throw std::runtime_error("Check hresult failed: Unknown reason");
    }
  }
}
