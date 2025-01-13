#pragma once

#include "common.hpp"

struct vec1 {
  float x;

  explicit vec1()
    : x(0)
  {
  }


  explicit vec1(
      float x_
    )
    : x(x_)
  {
  }

  vec1& operator+=(float other) {
    x += other;

    return *this;
  }

  vec1 operator+(float other) const {
    vec1 c = *this;
    c += other;
    return c;
  }

  vec1& operator+=(vec1 const & other) {
    x += other.x;

    return *this;
  }

  vec1 operator+(vec1 const & other) const {
    vec1 c = *this;
    c += other;
    return c;
  }

  vec1& operator-=(float other) {
    x -= other;

    return *this;
  }

  vec1 operator-(float other) const {
    vec1 c = *this;
    c -= other;
    return c;
  }

  vec1& operator-=(vec1 const & other) {
    x -= other.x;

    return *this;
  }

  vec1 operator-(vec1 const & other) const {
    vec1 c = *this;
    c -= other;
    return c;
  }

  vec1& operator*=(float other) {
    x *= other;

    return *this;
  }

  vec1 operator*(float other) const {
    vec1 c = *this;
    c *= other;
    return c;
  }

  vec1& operator*=(vec1 const & other) {
    x *= other.x;

    return *this;
  }

  vec1 operator*(vec1 const & other) const {
    vec1 c = *this;
    c *= other;
    return c;
  }

  vec1& operator/=(float other) {
    x /= other;

    return *this;
  }

  vec1 operator/(float other) const {
    vec1 c = *this;
    c /= other;
    return c;
  }

  vec1& operator/=(vec1 const & other) {
    x /= other.x;

    return *this;
  }

  vec1 operator/(vec1 const & other) const {
    vec1 c = *this;
    c /= other;
    return c;
  }


  void cos__inplace() {
    x = std::cosf(x);
  }

  vec1 cos() const {
    vec1 c = *this;
    c.cos__inplace();
    return c;
  }

  void sin__inplace() {
    x = std::sinf(x);
  }

  vec1 sin() const {
    vec1 c = *this;
    c.sin__inplace();
    return c;
  }

  void floor__inplace() {
    x = std::floorf(x);
  }

  vec1 floor() const {
    vec1 c = *this;
    c.floor__inplace();
    return c;
  }

  void log__inplace() {
    x = std::logf(x);
  }

  vec1 log() const {
    vec1 c = *this;
    c.log__inplace();
    return c;
  }

  void sqrt__inplace() {
    x = std::sqrtf(x);
  }

  vec1 sqrt() const {
    vec1 c = *this;
    c.sqrt__inplace();
    return c;
  }

  void fract__inplace() {
    x = ::fractf(x);
  }

  vec1 fract() const {
    vec1 c = *this;
    c.fract__inplace();
    return c;
  }

  void round__inplace() {
    x = ::roundf(x);
  }

  vec1 round() const {
    vec1 c = *this;
    c.round__inplace();
    return c;
  }

  void tanh_approx__inplace() {
    x = ::tanh_approxf(x);
  }

  vec1 tanh_approx() const {
    vec1 c = *this;
    c.tanh_approx__inplace();
    return c;
  }

  void saturate__inplace() {
    x = ::saturate(x);
  }

  vec1 saturate() const {
    vec1 c = *this;
    c.saturate__inplace();
    return c;
  }


  float dot(vec1 const & other) const {
    float sum = 0.F;

    sum += x*other.x;

    return sum;
  }


  float length2() const {
    return dot(*this);
  }

  float length() const {
    return std::sqrt(dot(*this));
  }

  void normalize__inplace() {
    *this *= 1.F/length();
  }

  vec1 normalized() const {
    vec1 c = *this;
    c.normalize__inplace();
    return c;
  }

  float distance(vec1 const & other) const {
    return (*this - other).length();
  }
};

inline vec1 mix(vec1 const & b, vec1 const & e, float x) {
  vec1 c;

  c.x = ::mix(b.x, e.x, x);

  return c;
}

inline vec1 smoothstep(vec1 const & edge0, vec1 const & edge1, vec1 const & x) {
  vec1 c;

  c.x = ::smoothstep(edge0.x, edge1.x, x.x);

  return c;
}

struct vec2 {
  float x;
  float y;

  explicit vec2()
    : x(0)
    , y(0)
  {
  }

  explicit vec2(float c)
    : x(c)
    , y(c)
  {
  }

  explicit vec2(
      float x_
    , float y_
    )
    : x(x_)
    , y(y_)
  {
  }

  vec2& operator+=(float other) {
    x += other;
    y += other;

    return *this;
  }

  vec2 operator+(float other) const {
    vec2 c = *this;
    c += other;
    return c;
  }

  vec2& operator+=(vec2 const & other) {
    x += other.x;
    y += other.y;

    return *this;
  }

  vec2 operator+(vec2 const & other) const {
    vec2 c = *this;
    c += other;
    return c;
  }

  vec2& operator-=(float other) {
    x -= other;
    y -= other;

    return *this;
  }

  vec2 operator-(float other) const {
    vec2 c = *this;
    c -= other;
    return c;
  }

  vec2& operator-=(vec2 const & other) {
    x -= other.x;
    y -= other.y;

    return *this;
  }

  vec2 operator-(vec2 const & other) const {
    vec2 c = *this;
    c -= other;
    return c;
  }

  vec2& operator*=(float other) {
    x *= other;
    y *= other;

    return *this;
  }

  vec2 operator*(float other) const {
    vec2 c = *this;
    c *= other;
    return c;
  }

  vec2& operator*=(vec2 const & other) {
    x *= other.x;
    y *= other.y;

    return *this;
  }

  vec2 operator*(vec2 const & other) const {
    vec2 c = *this;
    c *= other;
    return c;
  }

  vec2& operator/=(float other) {
    x /= other;
    y /= other;

    return *this;
  }

  vec2 operator/(float other) const {
    vec2 c = *this;
    c /= other;
    return c;
  }

  vec2& operator/=(vec2 const & other) {
    x /= other.x;
    y /= other.y;

    return *this;
  }

  vec2 operator/(vec2 const & other) const {
    vec2 c = *this;
    c /= other;
    return c;
  }


  void cos__inplace() {
    x = std::cosf(x);
    y = std::cosf(y);
  }

  vec2 cos() const {
    vec2 c = *this;
    c.cos__inplace();
    return c;
  }

  void sin__inplace() {
    x = std::sinf(x);
    y = std::sinf(y);
  }

  vec2 sin() const {
    vec2 c = *this;
    c.sin__inplace();
    return c;
  }

  void floor__inplace() {
    x = std::floorf(x);
    y = std::floorf(y);
  }

  vec2 floor() const {
    vec2 c = *this;
    c.floor__inplace();
    return c;
  }

  void log__inplace() {
    x = std::logf(x);
    y = std::logf(y);
  }

  vec2 log() const {
    vec2 c = *this;
    c.log__inplace();
    return c;
  }

  void sqrt__inplace() {
    x = std::sqrtf(x);
    y = std::sqrtf(y);
  }

  vec2 sqrt() const {
    vec2 c = *this;
    c.sqrt__inplace();
    return c;
  }

  void fract__inplace() {
    x = ::fractf(x);
    y = ::fractf(y);
  }

  vec2 fract() const {
    vec2 c = *this;
    c.fract__inplace();
    return c;
  }

  void round__inplace() {
    x = ::roundf(x);
    y = ::roundf(y);
  }

  vec2 round() const {
    vec2 c = *this;
    c.round__inplace();
    return c;
  }

  void tanh_approx__inplace() {
    x = ::tanh_approxf(x);
    y = ::tanh_approxf(y);
  }

  vec2 tanh_approx() const {
    vec2 c = *this;
    c.tanh_approx__inplace();
    return c;
  }

  void saturate__inplace() {
    x = ::saturate(x);
    y = ::saturate(y);
  }

  vec2 saturate() const {
    vec2 c = *this;
    c.saturate__inplace();
    return c;
  }


  float dot(vec2 const & other) const {
    float sum = 0.F;

    sum += x*other.x;
    sum += y*other.y;

    return sum;
  }


  float length2() const {
    return dot(*this);
  }

  float length() const {
    return std::sqrt(dot(*this));
  }

  void normalize__inplace() {
    *this *= 1.F/length();
  }

  vec2 normalized() const {
    vec2 c = *this;
    c.normalize__inplace();
    return c;
  }

  float distance(vec2 const & other) const {
    return (*this - other).length();
  }
};

inline vec2 mix(vec2 const & b, vec2 const & e, float x) {
  vec2 c;

  c.x = ::mix(b.x, e.x, x);
  c.y = ::mix(b.y, e.y, x);

  return c;
}

inline vec2 smoothstep(vec2 const & edge0, vec2 const & edge1, vec2 const & x) {
  vec2 c;

  c.x = ::smoothstep(edge0.x, edge1.x, x.x);
  c.y = ::smoothstep(edge0.y, edge1.y, x.y);

  return c;
}

struct vec3 {
  float x;
  float y;
  float z;

  explicit vec3()
    : x(0)
    , y(0)
    , z(0)
  {
  }

  explicit vec3(float c)
    : x(c)
    , y(c)
    , z(c)
  {
  }

  explicit vec3(
      float x_
    , float y_
    , float z_
    )
    : x(x_)
    , y(y_)
    , z(z_)
  {
  }

  vec3& operator+=(float other) {
    x += other;
    y += other;
    z += other;

    return *this;
  }

  vec3 operator+(float other) const {
    vec3 c = *this;
    c += other;
    return c;
  }

  vec3& operator+=(vec3 const & other) {
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
  }

  vec3 operator+(vec3 const & other) const {
    vec3 c = *this;
    c += other;
    return c;
  }

  vec3& operator-=(float other) {
    x -= other;
    y -= other;
    z -= other;

    return *this;
  }

  vec3 operator-(float other) const {
    vec3 c = *this;
    c -= other;
    return c;
  }

  vec3& operator-=(vec3 const & other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
  }

  vec3 operator-(vec3 const & other) const {
    vec3 c = *this;
    c -= other;
    return c;
  }

  vec3& operator*=(float other) {
    x *= other;
    y *= other;
    z *= other;

    return *this;
  }

  vec3 operator*(float other) const {
    vec3 c = *this;
    c *= other;
    return c;
  }

  vec3& operator*=(vec3 const & other) {
    x *= other.x;
    y *= other.y;
    z *= other.z;

    return *this;
  }

  vec3 operator*(vec3 const & other) const {
    vec3 c = *this;
    c *= other;
    return c;
  }

  vec3& operator/=(float other) {
    x /= other;
    y /= other;
    z /= other;

    return *this;
  }

  vec3 operator/(float other) const {
    vec3 c = *this;
    c /= other;
    return c;
  }

  vec3& operator/=(vec3 const & other) {
    x /= other.x;
    y /= other.y;
    z /= other.z;

    return *this;
  }

  vec3 operator/(vec3 const & other) const {
    vec3 c = *this;
    c /= other;
    return c;
  }


  void cos__inplace() {
    x = std::cosf(x);
    y = std::cosf(y);
    z = std::cosf(z);
  }

  vec3 cos() const {
    vec3 c = *this;
    c.cos__inplace();
    return c;
  }

  void sin__inplace() {
    x = std::sinf(x);
    y = std::sinf(y);
    z = std::sinf(z);
  }

  vec3 sin() const {
    vec3 c = *this;
    c.sin__inplace();
    return c;
  }

  void floor__inplace() {
    x = std::floorf(x);
    y = std::floorf(y);
    z = std::floorf(z);
  }

  vec3 floor() const {
    vec3 c = *this;
    c.floor__inplace();
    return c;
  }

  void log__inplace() {
    x = std::logf(x);
    y = std::logf(y);
    z = std::logf(z);
  }

  vec3 log() const {
    vec3 c = *this;
    c.log__inplace();
    return c;
  }

  void sqrt__inplace() {
    x = std::sqrtf(x);
    y = std::sqrtf(y);
    z = std::sqrtf(z);
  }

  vec3 sqrt() const {
    vec3 c = *this;
    c.sqrt__inplace();
    return c;
  }

  void fract__inplace() {
    x = ::fractf(x);
    y = ::fractf(y);
    z = ::fractf(z);
  }

  vec3 fract() const {
    vec3 c = *this;
    c.fract__inplace();
    return c;
  }

  void round__inplace() {
    x = ::roundf(x);
    y = ::roundf(y);
    z = ::roundf(z);
  }

  vec3 round() const {
    vec3 c = *this;
    c.round__inplace();
    return c;
  }

  void tanh_approx__inplace() {
    x = ::tanh_approxf(x);
    y = ::tanh_approxf(y);
    z = ::tanh_approxf(z);
  }

  vec3 tanh_approx() const {
    vec3 c = *this;
    c.tanh_approx__inplace();
    return c;
  }

  void saturate__inplace() {
    x = ::saturate(x);
    y = ::saturate(y);
    z = ::saturate(z);
  }

  vec3 saturate() const {
    vec3 c = *this;
    c.saturate__inplace();
    return c;
  }


  float dot(vec3 const & other) const {
    float sum = 0.F;

    sum += x*other.x;
    sum += y*other.y;
    sum += z*other.z;

    return sum;
  }

  vec3 cross(vec3 const & other) const {
    return vec3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
  }

  float length2() const {
    return dot(*this);
  }

  float length() const {
    return std::sqrt(dot(*this));
  }

  void normalize__inplace() {
    *this *= 1.F/length();
  }

  vec3 normalized() const {
    vec3 c = *this;
    c.normalize__inplace();
    return c;
  }

  float distance(vec3 const & other) const {
    return (*this - other).length();
  }
};

inline vec3 mix(vec3 const & b, vec3 const & e, float x) {
  vec3 c;

  c.x = ::mix(b.x, e.x, x);
  c.y = ::mix(b.y, e.y, x);
  c.z = ::mix(b.z, e.z, x);

  return c;
}

inline vec3 smoothstep(vec3 const & edge0, vec3 const & edge1, vec3 const & x) {
  vec3 c;

  c.x = ::smoothstep(edge0.x, edge1.x, x.x);
  c.y = ::smoothstep(edge0.y, edge1.y, x.y);
  c.z = ::smoothstep(edge0.z, edge1.z, x.z);

  return c;
}

struct vec4 {
  float x;
  float y;
  float z;
  float w;

  explicit vec4()
    : x(0)
    , y(0)
    , z(0)
    , w(0)
  {
  }

  explicit vec4(float c)
    : x(c)
    , y(c)
    , z(c)
    , w(c)
  {
  }

  explicit vec4(
      float x_
    , float y_
    , float z_
    , float w_
    )
    : x(x_)
    , y(y_)
    , z(z_)
    , w(w_)
  {
  }

  vec4& operator+=(float other) {
    x += other;
    y += other;
    z += other;
    w += other;

    return *this;
  }

  vec4 operator+(float other) const {
    vec4 c = *this;
    c += other;
    return c;
  }

  vec4& operator+=(vec4 const & other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;

    return *this;
  }

  vec4 operator+(vec4 const & other) const {
    vec4 c = *this;
    c += other;
    return c;
  }

  vec4& operator-=(float other) {
    x -= other;
    y -= other;
    z -= other;
    w -= other;

    return *this;
  }

  vec4 operator-(float other) const {
    vec4 c = *this;
    c -= other;
    return c;
  }

  vec4& operator-=(vec4 const & other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;

    return *this;
  }

  vec4 operator-(vec4 const & other) const {
    vec4 c = *this;
    c -= other;
    return c;
  }

  vec4& operator*=(float other) {
    x *= other;
    y *= other;
    z *= other;
    w *= other;

    return *this;
  }

  vec4 operator*(float other) const {
    vec4 c = *this;
    c *= other;
    return c;
  }

  vec4& operator*=(vec4 const & other) {
    x *= other.x;
    y *= other.y;
    z *= other.z;
    w *= other.w;

    return *this;
  }

  vec4 operator*(vec4 const & other) const {
    vec4 c = *this;
    c *= other;
    return c;
  }

  vec4& operator/=(float other) {
    x /= other;
    y /= other;
    z /= other;
    w /= other;

    return *this;
  }

  vec4 operator/(float other) const {
    vec4 c = *this;
    c /= other;
    return c;
  }

  vec4& operator/=(vec4 const & other) {
    x /= other.x;
    y /= other.y;
    z /= other.z;
    w /= other.w;

    return *this;
  }

  vec4 operator/(vec4 const & other) const {
    vec4 c = *this;
    c /= other;
    return c;
  }


  void cos__inplace() {
    x = std::cosf(x);
    y = std::cosf(y);
    z = std::cosf(z);
    w = std::cosf(w);
  }

  vec4 cos() const {
    vec4 c = *this;
    c.cos__inplace();
    return c;
  }

  void sin__inplace() {
    x = std::sinf(x);
    y = std::sinf(y);
    z = std::sinf(z);
    w = std::sinf(w);
  }

  vec4 sin() const {
    vec4 c = *this;
    c.sin__inplace();
    return c;
  }

  void floor__inplace() {
    x = std::floorf(x);
    y = std::floorf(y);
    z = std::floorf(z);
    w = std::floorf(w);
  }

  vec4 floor() const {
    vec4 c = *this;
    c.floor__inplace();
    return c;
  }

  void log__inplace() {
    x = std::logf(x);
    y = std::logf(y);
    z = std::logf(z);
    w = std::logf(w);
  }

  vec4 log() const {
    vec4 c = *this;
    c.log__inplace();
    return c;
  }

  void sqrt__inplace() {
    x = std::sqrtf(x);
    y = std::sqrtf(y);
    z = std::sqrtf(z);
    w = std::sqrtf(w);
  }

  vec4 sqrt() const {
    vec4 c = *this;
    c.sqrt__inplace();
    return c;
  }

  void fract__inplace() {
    x = ::fractf(x);
    y = ::fractf(y);
    z = ::fractf(z);
    w = ::fractf(w);
  }

  vec4 fract() const {
    vec4 c = *this;
    c.fract__inplace();
    return c;
  }

  void round__inplace() {
    x = ::roundf(x);
    y = ::roundf(y);
    z = ::roundf(z);
    w = ::roundf(w);
  }

  vec4 round() const {
    vec4 c = *this;
    c.round__inplace();
    return c;
  }

  void tanh_approx__inplace() {
    x = ::tanh_approxf(x);
    y = ::tanh_approxf(y);
    z = ::tanh_approxf(z);
    w = ::tanh_approxf(w);
  }

  vec4 tanh_approx() const {
    vec4 c = *this;
    c.tanh_approx__inplace();
    return c;
  }

  void saturate__inplace() {
    x = ::saturate(x);
    y = ::saturate(y);
    z = ::saturate(z);
    w = ::saturate(w);
  }

  vec4 saturate() const {
    vec4 c = *this;
    c.saturate__inplace();
    return c;
  }


  float dot(vec4 const & other) const {
    float sum = 0.F;

    sum += x*other.x;
    sum += y*other.y;
    sum += z*other.z;
    sum += w*other.w;

    return sum;
  }


  float length2() const {
    return dot(*this);
  }

  float length() const {
    return std::sqrt(dot(*this));
  }

  void normalize__inplace() {
    *this *= 1.F/length();
  }

  vec4 normalized() const {
    vec4 c = *this;
    c.normalize__inplace();
    return c;
  }

  float distance(vec4 const & other) const {
    return (*this - other).length();
  }
};

inline vec4 mix(vec4 const & b, vec4 const & e, float x) {
  vec4 c;

  c.x = ::mix(b.x, e.x, x);
  c.y = ::mix(b.y, e.y, x);
  c.z = ::mix(b.z, e.z, x);
  c.w = ::mix(b.w, e.w, x);

  return c;
}

inline vec4 smoothstep(vec4 const & edge0, vec4 const & edge1, vec4 const & x) {
  vec4 c;

  c.x = ::smoothstep(edge0.x, edge1.x, x.x);
  c.y = ::smoothstep(edge0.y, edge1.y, x.y);
  c.z = ::smoothstep(edge0.z, edge1.z, x.z);
  c.w = ::smoothstep(edge0.w, edge1.w, x.w);

  return c;
}


