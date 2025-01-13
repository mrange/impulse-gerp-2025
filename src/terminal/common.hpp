#pragma once

#include "precompiled.hpp"

#define CHECK_CONDITION(cond) check_condition(cond, #cond, __FILE__, __LINE__)
#define CHECK_HRESULT(cond)   check_hresult(cond, #cond, __FILE__, __LINE__)

static_assert(sizeof(char) == sizeof(char8_t), "Must be same size");
float       constexpr pi              = 3.141592654F;
float       constexpr tau             = 2*pi;
std::size_t constexpr screen__width   = 80;
std::size_t constexpr screen__height  = 30;
float       constexpr end__time       = 1E6;

float       constexpr music__bpm              = 145;
std::size_t constexpr music__beat_length      = 528;
float       constexpr music__beat_time        = 60/music__bpm;
float       constexpr music__subdivision_time = music__beat_time/4;
float       constexpr music__bar_time         = music__beat_time*4;

float       music__beat         (float time);
float       music__drum         (float time);
int         music__nbeat        (float time);
int         music__nsubdivision (float time);
int         music__nbar         (float time);
float       music__fbeat        (float time);
float       music__fsubdivision (float time);
float       music__fbar         (float time);

float       music__from_nbeat   (int beat);

std::size_t pick_a_number(std::size_t min, std::size_t max);

float pick_a_float(float min, float max);

inline float length2f(float x, float y) {
  return x*x+y*y;
}

inline float lengthf(float x, float y) {
  return std::sqrtf(length2f(x,y));
}

inline float fractf(float x) {
  return x-std::floorf(x);
}

inline float roundf(float x) {
  return std::floorf(x+0.5F);
}

inline float mix(float b, float e, float x) {
  return b+(e-b)*x;
}

inline float signf(float x) {
  return x >= 0 ? 1.F : -1.F;
}

// License: Unknown, author: Unknown, found: don't remember
inline float hash(float co) {
  return fractf(sinf(co*12.9898F) * 13758.5453F);
}

inline float saturate(float x) {
  return std::clamp<float>(x,0,1);
}

inline float step(float edge, float x) {
  return x < edge ? 0.F : 1.F;
}

float linstep(float edge0, float edge1, float x);

float smoothstep(float edge0, float edge1, float x);

// License: Unknown, author: Claude Brezinski, found: https://mathr.co.uk/blog/2017-09-06_approximating_hyperbolic_tangent.html
float tanh_approxf(float x);

// License: MIT, author: Inigo Quilez, found: https://www.iquilezles.org/www/articles/smin/smin.htm
float pmin(float a, float b, float k);

// License: CC0, author: Mårten Rånge, found: https://github.com/mrange/glsl-snippets
float pmax(float a, float b, float k);

struct rotator {
  float const c;
  float const s;
  explicit rotator(float a)
  : c(std::cosf(a))
  , s(std::sinf(a)) {
  }

  inline void operator()(float & x, float & y) const noexcept {
    auto xx = c*x+s*y;
    auto yy = -s*x+c*y;
    x = xx;
    y = yy;
  }

};

template<typename TOnExit>
struct on_exit__impl {
  on_exit__impl ()                                = delete;
  on_exit__impl (on_exit__impl const &)           = delete;
  on_exit__impl& operator=(on_exit__impl const &) = delete;

  explicit on_exit__impl (TOnExit && on_exit)
    : suppress (false)
    , on_exit  (std::move(on_exit)) {
  }

  explicit on_exit__impl (on_exit__impl && impl)
    : suppress (impl.suppress)
    , on_exit  (std::move(impl.on_exit)) {
    impl.suppress = true;
  }

  ~on_exit__impl() noexcept {
    if(!suppress) {
      suppress = true;
      on_exit();
    }
  }

  bool suppress;
private:
  TOnExit on_exit;
};

template<typename TOnExit>
auto on_exit(TOnExit && on_exit) {
  return on_exit__impl<std::decay_t<TOnExit>>(std::move(on_exit));
}

void check_condition(
    bool          condition
  , char const *  condition_
  , char const *  file_name
  , int           line_no
  );

void check_hresult(
    HRESULT       condition
  , char const *  condition_
  , char const *  file_name
  , int           line_no
  );