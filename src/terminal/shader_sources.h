#pragma once

//  col = floor(vec3(8,8,4)*col)/vec3(8,8,4);

namespace {
  char const vertex_shader__basic[] = R"SHADER(
#version 300 es
layout (location = 0) in vec3 aPos;
void main() {
  gl_Position = vec4(aPos.x, -aPos.y, aPos.z, 1.0);
}
)SHADER";

  char const fragment_shader__mandelbox[] = R"SHADER(
#version 300 es
#define USE_UNIFORMS

precision highp float;

uniform float time;
uniform vec2 resolution;

#ifdef USE_UNIFORMS
uniform vec4 state;

float beat() {
  return state.x;
}

float fade_in() {
  return state.y;
}

float fade_out() {
  return state.z;
}

float fade() {
  return state.w;
}


#else
const float BPM = 145.0/60.;

float fade_in() {
  return 0.0;
}

float fade_out() {
  return 0.;
}

float fade() {
  return 1.;
//  return smoothstep(-0.707, 0.707, sin(time));
}

float beat() {
  return exp(-1.*fract(0.25+0.5*time*BPM));
}
#endif

const float fixed_radius2 = 1.9;
const float min_radius2 = 0.1;
const float folding_limit = 1.0;
const float scale = -2.2;

out vec4 fragColor;

#define TIME        time
#define RESOLUTION  resolution
#define PI          3.141592654
#define TAU         (2.0*PI)


//#define THEME0
#define THEME1

#define TOLERANCE       0.001
#define NORMTOL         0.00125
#define MAX_RAY_LENGTH  15.0
#define MAX_RAY_MARCHES 90
#define ROT(a)          mat2(cos(a), sin(a), -sin(a), cos(a))
#define PI              3.141592654
#define TAU             (2.0*PI)

// https://stackoverflow.com/a/17897228/418488
const vec4 hsv2rgb_K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
vec3 hsv2rgb(vec3 c) {
  vec3 p = abs(fract(c.xxx + hsv2rgb_K.xyz) * 6.0 - hsv2rgb_K.www);
  return c.z * mix(hsv2rgb_K.xxx, clamp(p - hsv2rgb_K.xxx, 0.0, 1.0), c.y);
}
#define HSV2RGB(c)  (c.z * mix(hsv2rgb_K.xxx, clamp(abs(fract(c.xxx + hsv2rgb_K.xyz) * 6.0 - hsv2rgb_K.www) - hsv2rgb_K.xxx, 0.0, 1.0), c.y))

mat3 g_rot;

float g_quad = 0.0;
float g_beat = 0.0;

// License: Unknown, author: Unknown, found: don't remember
float tanh_approx(float x) {
//  return tanh(x);
  float x2 = x*x;
  return clamp(x*(27.0 + x2)/(27.0+9.0*x2), -1.0, 1.0);
}

// License: MIT, author: Inigo Quilez, found: https://www.iquilezles.org/www/articles/spherefunctions/spherefunctions.htm
float sphered(vec3 ro, vec3 rd, vec4 sph, float dbuffer) {
  float ndbuffer = dbuffer/sph.w;
  vec3  rc = (ro - sph.xyz)/sph.w;

  float b = dot(rd,rc);
  float c = dot(rc,rc) - 1.0;
  float h = b*b - c;
  if( h<0.0 ) return 0.0;
  h = sqrt( h );
  float t1 = -b - h;
  float t2 = -b + h;

  if( t2<0.0 || t1>ndbuffer ) return 0.0;
  t1 = max( t1, 0.0 );
  t2 = min( t2, ndbuffer );

  float i1 = -(c*t1 + b*t1*t1 + t1*t1*t1/3.0);
  float i2 = -(c*t2 + b*t2*t2 + t2*t2*t2/3.0);
  return (i2-i1)*(3.0/4.0);
}

// License: MIT, author: Inigo Quilez, found: https://www.iquilezles.org/www/articles/smin/smin.htm
float pmin(float a, float b, float k) {
  float h = clamp(0.5+0.5*(b-a)/k, 0.0, 1.0);
  return mix(b, a, h) - k*h*(1.0-h);
}

// License: CC0, author: Mårten Rånge, found: https://github.com/mrange/glsl-snippets
float pmax(float a, float b, float k) {
  return -pmin(-a, -b, k);
}

// -------------------------------------------------
// Mandelbox - https://www.shadertoy.com/view/XdlSD4


void sphere_fold(inout vec3 z, inout float dz) {
  float r2 = dot(z, z);
  if(r2 < min_radius2) {
    float temp = (fixed_radius2 / min_radius2);
    z *= temp;
    dz *= temp;
  } else if(r2 < fixed_radius2) {
    float temp = (fixed_radius2 / r2);
    z *= temp;
    dz *= temp;
  }
}

void box_fold(inout vec3 z, inout float dz) {
  z = clamp(z, -folding_limit, folding_limit) * 2.0 - z;
}

float mb(vec3 z) {
  float dd = length(z)-mix(2., 2.3, g_beat);
  vec3 offset = z;
  float dr = 1.0;

  for(int n = 0; n < 5; ++n) {
    box_fold(z, dr);
    sphere_fold(z, dr);

    z = scale * z + offset;
    dr = dr * abs(scale) + 1.5;
  }

  float d = (length(z))/abs(dr)-0.04;
  dd = pmax(dd, -d, 0.5);
  if (dd < d) {
    g_quad = -1.0;
  } else {
    g_quad = 1.0;
  }
  g_quad = -g_quad;
  d = min(d, dd);

  return d;
}
// -------------------------------------------------


float df(vec3 p) {
  p *= g_rot;
  float d = mb(p);
  return d;
}

float rayMarch(in vec3 ro, in vec3 rd, out int iter) {
  float t = 2.0;
  int i = 0;
  for (i = 0; i < MAX_RAY_MARCHES; i++) {
    float d = df(ro + rd*t);
    if (d < TOLERANCE || t > MAX_RAY_LENGTH) break;
    t += d;
  }
  iter = i;
  return t;
}

vec3 normal(vec3 pos) {
  vec2  eps = vec2(NORMTOL, 0.0);
  vec3 nor;
  nor.x = df(pos+eps.xyy) - df(pos-eps.xyy);
  nor.y = df(pos+eps.yxy) - df(pos-eps.yxy);
  nor.z = df(pos+eps.yyx) - df(pos-eps.yyx);
  return normalize(nor);
}

float softShadow(in vec3 pos, in vec3 ld, in float ll, float mint, float k) {
  const float minShadow = 0.25;
  float res = 1.0;
  float t = mint;
  for (int i=0; i<25; ++i) {
    float distance = df(pos + ld*t);
    res = min(res, k*distance/t);
    if (ll <= t) break;
    if(res <= minShadow) break;
    t += max(mint*0.2, distance);
  }
  return clamp(res,minShadow,1.0);
}

vec3 render(vec3 ro, vec3 rd) {
  const vec3 lightPos0  = 2.5*vec3(1.0, 1.0, 1.0);
  const vec3 lightPos1  = vec3(0.0);

  const vec3 scol = HSV2RGB(vec3(0.0, 0.95, 0.005));
  vec3 skyCol = vec3(0.0);
  float a = atan(rd.x, rd.z);

  int iter = 0;
  float t = rayMarch(ro, rd, iter);
  float quad = g_quad;

  float tt = float(iter)/float(MAX_RAY_MARCHES);
  float bs = 1.0-tt*tt*tt*tt;

  vec3 pos = ro + t*rd;
  float beat = g_beat;
  float lsd1  = sphered(ro, rd, vec4(lightPos1, mix(2.0, 2.8, beat*beat*beat)), t);

  const vec3 bcol0 = HSV2RGB(vec3(0.6, 0.6, 3.0));
  const vec3 bcol1 = HSV2RGB(vec3(0.55, 0.8, 7.0));
  vec3 bcol   = mix(bcol0, bcol1, beat);
  vec3 gcol   = lsd1*bcol;

  if (t >= MAX_RAY_LENGTH) {
    return skyCol+gcol;
  }

  float d     = df(pos);
  vec3 nor    = normal(pos);
  float fre   = 1.0+dot(nor, rd);
  fre *= fre;
  fre *= fre;

  vec3 lv0    = lightPos0 - pos;
  float ll20  = dot(lv0, lv0);
  float ll0   = sqrt(ll20);
  vec3 ld0    = lv0 / ll0;
  float dm0   = 8.0/ll20;
  float sha0  = softShadow(pos, ld0, ll0, 0.125, 32.0);
  float dif0  = max(dot(nor,ld0),0.0)*dm0;
  float spe0  = pow(max(dot(reflect(rd, nor), ld0), 0.), 10.0);


  vec3 col = vec3(0.0);
  quad = -quad;
  const vec3 black = vec3(0.0);
#if defined(THEME0)
  const vec3 dcol0 = HSV2RGB(vec3(0.6, 0.5, 1.0));
  const vec3 dcol1 = black;
#elif defined(THEME1)
  const vec3 dcol0 = HSV2RGB(vec3(0.55 , 0.5, -0.025));
  const vec3 dcol1 = HSV2RGB(vec3(0.08, 1.0, 1.0));
#else
  const vec3 dcol0 = black;
  const vec3 dcol1 = dcol0;
#endif
  col += dif0*dif0*sha0*mix(dcol0, dcol1, 0.5+0.5*quad);
  col += spe0*bcol*bs*sha0;
  col += gcol;

  return col;
}

// License: Unknown, author: Unknown, found: don't remember
float hash(vec2 co) {
  return fract(sin(dot(co.xy ,vec2(12.9898,58.233))) * 13758.5453);
}

vec3 palette(float a) {
  return 0.5*(1.+sin(vec3(0,1,2)+a));
}

mat3 rot(float time) {
  float
    angle1 = time * 0.5
  , angle2 = time * 0.707
  , angle3 = time * 0.33
  , c1 = cos(angle1); float s1 = sin(angle1)
  , c2 = cos(angle2); float s2 = sin(angle2)
  , c3 = cos(angle3); float s3 = sin(angle3)
  ;

  return mat3(
      c1 * c2,
      c1 * s2 * s3 - c3 * s1,
      s1 * s3 + c1 * c3 * s2,

      c2 * s1,
      c1 * c3 + s1 * s2 * s3,
      c3 * s1 * s2 - c1 * s3,

      -s2,
      c2 * s3,
      c2 * c3
  );
}
// License: Unknown, author: Matt Taylor (https://github.com/64), found: https://64.github.io/tonemapping/
vec3 aces_approx(vec3 v) {
  v = max(v, 0.0);
  v *= 0.6;
  float a = 2.51;
  float b = 0.03;
  float c = 2.43;
  float d = 0.59;
  float e = 0.14;
  return clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0, 1.0);
}


vec3 effect(vec2 q) {
  g_beat = beat();
  const vec2 csz = 1.0/vec2(80.,30.);

  vec2 nq = floor(q/csz)*csz;
  vec2 np = -1.+2.*nq;
  float fade = fade();
  float h0 = hash(nq);
  float f0 = -0.1*h0+0.5*(np.y*np.y+np.x*np.x)+mix(-1.4, 0.6, fade);
  if (f0 < 0.) {
    q = nq;
  }
  vec2 p = -1.0 + 2.0*q;
  p.x *= RESOLUTION.x/RESOLUTION.y;

  const vec3 cam  = 5.0*vec3(1.0, 0.5, 1.0);
  const vec3 dcam = normalize(vec3(0.0) - cam);
  const vec3 ro = cam;
  const vec3 ww = dcam;
  const vec3 uu = normalize(cross(vec3(0.0,1.0,0.0), ww));
  const vec3 vv = cross(ww,uu);
  const float rdd = 2.0;
  vec3 rd = normalize(-p.x*uu + p.y*vv + rdd*ww);

  g_rot = rot(-5.+(time+3.*length(p)*fade_out()));
  vec3 col = render(ro, rd);
  col += 3.*(palette(time+(np.x+np.y)*PI/2.+PI)+f0)*exp(-10.*f0)*step(0.,f0);

  col = aces_approx(col);
  col +=3.*fade_in();
  col = sqrt(col);

  return col;
}

void main() {
  vec2 q = gl_FragCoord.xy/RESOLUTION.xy;
  vec3 col = effect(q);
  fragColor = vec4(col, 1.0);
}
)SHADER";

  char const fragment_shader__bw[] = R"SHADER(
#version 300 es
#define USE_UNIFORMS

precision highp float;

uniform float time;
uniform vec2 resolution;

out vec4 fragColor;

#ifdef USE_UNIFORMS
uniform vec4 state;

float beat() {
  return state.x;
}

float fade_in() {
  return state.y;
}

float fade_out() {
  return state.z;
}

float fade() {
  return state.w;
}

#else
const float BPM = 145.0/60.;

float fade_in() {
  return 0.;
}

float fade_out() {
  return 0.;
}

float fade() {
  return smoothstep(-0.707, 0.707, sin(time));
}

float beat() {
  return exp(-1.*fract(0.25+0.5*time*BPM));
}
#endif


#define TIME        time
#define RESOLUTION  resolution

// License CC0: Flying through kaleidoscoped truchet patterns
// Experimenting with simple truchet patterns + kaleidoscope turned out rather nice
//  so I wanted to share.

// SABS by ollj

#define PI              3.141592654
#define TAU             (2.0*PI)
#define TIME            time
#define RESOLUTION      resolution
#define LESS(a,b,c)     mix(a,b,step(0.,c))
#define SABS(x,k)       LESS((.5/(k))*(x)*(x)+(k)*.5,abs(x),abs(x)-(k))
#define ROT(a)          mat2(cos(a), sin(a), -sin(a), cos(a))
#define BPM             145.0

const vec3 std_gamma        = vec3(2.2, 2.2, 2.2);

float hash(float co) {
  return fract(sin(co*12.9898) * 13758.5453);
}

float hash(vec3 co) {
  return fract(sin(dot(co, vec3(12.9898,58.233, 12.9898+58.233))) * 13758.5453);
}

vec2 toPolar(vec2 p) {
  return vec2(length(p), atan(p.y, p.x));
}

vec2 toRect(vec2 p) {
  return vec2(p.x*cos(p.y), p.x*sin(p.y));
}

vec2 mod2_1(inout vec2 p) {
  vec2 c = floor(p + 0.5);
  p = fract(p + 0.5) - 0.5;
  return c;
}

float modMirror1(inout float p, float size) {
  float halfsize = size*0.5;
  float c = floor((p + halfsize)/size);
  p = mod(p + halfsize,size) - halfsize;
  p *= mod(c, 2.0)*2.0 - 1.0;
  return c;
}

float smoothKaleidoscope(inout vec2 p, float sm, float rep) {
  vec2 hp = p;

  vec2 hpp = toPolar(hp);
  float rn = modMirror1(hpp.y, TAU/rep);

  float sa = PI/rep - SABS(PI/rep - abs(hpp.y), sm);
  hpp.y = sign(hpp.y)*(sa);

  hp = toRect(hpp);

  p = hp;

  return rn;
}

vec3 toScreenSpace(vec3 col) {
  return pow(col, 1.0/std_gamma);
}

vec3 alphaBlend(vec3 back, vec4 front) {
  vec3 colb = back.xyz;
  vec3 colf = front.xyz;
  vec3 xyz = mix(colb, colf.xyz, front.w);
  return xyz;
}

float circle(vec2 p, float r) {
  return length(p) - r;
}

vec3 offset(float z) {
  float a = z;
  vec2 p = -0.1*(vec2(cos(a), sin(a*sqrt(2.0))) + vec2(cos(a*sqrt(0.75)), sin(a*sqrt(0.5))));
  return vec3(p, z);
}

vec3 doffset(float z) {
  float eps = 0.1;
  return 0.5*(offset(z + eps) - offset(z - eps))/eps;
}

vec3 ddoffset(float z) {
  float eps = 0.1;
  return 0.125*(doffset(z + eps) - doffset(z - eps))/eps;
}

// -----------------------------------------------------------------------------
// PLANE0__BEGIN
// -----------------------------------------------------------------------------

const float plane0_lw = 0.05;

const mat2[] plane0_rots = mat2[](ROT(0.0*PI/2.0), ROT(1.00*PI/2.0), ROT(2.0*PI/2.0), ROT(3.0*PI/2.0));

vec2 plane0_cell0(vec2 p, float h) {
  float d0  = circle(p-vec2(0.5), 0.5);
  float d1  = circle(p+vec2(0.5), 0.5);

  float d = 1E6;
  d = min(d, d0);
  d = min(d, d1);
  return vec2(d, 1E6); // 1E6 gives a nice looking bug, 1E4 produces a more "correct" result
}

vec2 plane0_cell1(vec2 p, float h) {
  float d0  = abs(p.x);
  float d1  = abs(p.y);
  float d2  = circle(p, mix(0.2, 0.4, h));

  float d = 1E6;
  d = min(d, d0);
  d = min(d, d1);
  d = min(d, d2);
  return vec2(d, d2+plane0_lw);
}

vec2 plane0_df(vec3 pp, float h, out vec3 n) {
  vec2 p = pp.xy*ROT(TAU*h+TIME*fract(23.0*h)*0.5);
  float hd = circle(p, 0.4);

  vec2 hp = p;
  float rep = 2.0*floor(mix(5.0, 25.0, fract(h*13.0)));
  float sm = mix(0.05, 0.125, fract(h*17.0))*24.0/rep;
  float kn = 0.0;
  kn = smoothKaleidoscope(hp, sm, rep);
  vec2 hn = mod2_1(hp);
  float r = hash(vec3(hn, h));

  hp *= plane0_rots[int(r*4.0)];
  float rr = fract(r*31.0);
  vec2 cd0 = plane0_cell0(hp, rr);
  vec2 cd1 = plane0_cell1(hp, rr);
  vec2 d0 = mix(cd0, cd1, vec2(fract(r*13.0) > 0.5));

  hd = min(hd, d0.y);

  float d = 1E6;
  d = min(d, d0.x);
  d = abs(d) - plane0_lw;
  d = min(d, hd - plane0_lw*2.0);

  n = vec3(hn, kn);

  return vec2(hd, d);
}

vec4 plane0(vec3 ro, vec3 rd, vec3 pp, vec3 off, float aa, float n) {

  float h = hash(n);
  float s = mix(0.05, 0.25, h);

  vec3 hn;
  vec3 p = pp-off*vec3(1.0, 1.0, 0.0);

  vec2 dd = plane0_df(p/s, h, hn)*s;
  float d = dd.y;

  float a  = smoothstep(-aa, aa, -d);
  float ha = smoothstep(-aa, aa, dd.x);

  vec4 col = vec4(mix(vec3(1.0), vec3(0.0), a), ha);

  return col;
}

// -----------------------------------------------------------------------------
// PLANE0__END
// -----------------------------------------------------------------------------

vec4 plane(vec3 ro, vec3 rd, vec3 pp, vec3 off, float aa, float n) {
  return plane0(ro, rd, pp, off, aa, n);
}

vec3 skyColor(vec3 ro, vec3 rd) {
  float ld = max(dot(rd, vec3(0.0, 0.0, 1.0)), 0.0);
  return vec3(tanh(3.0*pow(ld, 100.0)));
}

float psin(float x) {
  return 0.5+0.5*sin(x);
}

const float planeDist = 0.707;

vec3 color(vec3 ww, vec3 uu, vec3 vv, vec3 ro, vec2 p) {
  const float per = 40.;
  float lp = length(p);
  vec2 np = p + 1.0/RESOLUTION.xy;
  float rdd = (mix(2.,4., fade_out())+0.5*lp*tanh(lp+0.9*psin(per*p.x)*psin(per*p.y)));
  vec3 rd = normalize(p.x*uu + p.y*vv + rdd*ww);
  vec3 nrd = normalize(np.x*uu + np.y*vv + rdd*ww);

  const vec3 errorCol = vec3(1.0, 0.0, 0.0);

  const int furthest = 6;
  const int fadeFrom = max(furthest-4, 0);

  float nz = floor(ro.z / planeDist);

  vec3 skyCol = skyColor(ro, rd);

  vec3 col = skyCol;

  for (int i = furthest; i >= 1 ; --i) {
    float pz = planeDist*nz + planeDist*float(i);

    float pd = (pz - ro.z)/rd.z;

    if (pd > 0.0) {
      vec3 pp = ro + rd*pd;
      vec3 npp = ro + nrd*pd;

      float aa = 3.0*length(pp - npp);

      vec3 off = offset(pp.z);

      vec4 pcol = plane(ro, rd, pp, off, aa, nz+float(i));

      float nz = pp.z-ro.z;
      float fadeIn = (1.0-smoothstep(planeDist*float(fadeFrom), planeDist*float(furthest), nz));
      float fadeOut = smoothstep(0.0, planeDist*0.1, nz);
      pcol.xyz = mix(skyCol, pcol.xyz, (fadeIn));
      pcol.w *= fadeOut;

      col = alphaBlend(col, pcol);
    } else {
      break;
    }

  }

  return col;
}

vec3 effect(vec2 p, vec2 q) {
  float tm  = planeDist*TIME*0.5*BPM/60.+0.25;
  vec3 ro   = offset(tm);
  vec3 dro  = doffset(tm);
  vec3 ddro = ddoffset(tm);

  vec3 ww = normalize(dro);
  vec3 uu = normalize(cross(normalize(vec3(0.0,1.0,0.0)+ddro), ww));
  vec3 vv = normalize(cross(ww, uu));

  vec3 col = color(ww, uu, vv, ro, p);
  col += fade_in();
  col = sqrt(col);
  return col;
}

void main() {
  vec2 q = gl_FragCoord.xy/RESOLUTION.xy;
  vec2 p = -1. + 2. * q;
  p.x *= RESOLUTION.x/RESOLUTION.y;

  vec3 col = effect(p, q);

  fragColor = vec4(col, 1.0);
}
)SHADER";

  char const fragment_shader__psychedelic[] = R"SHADER(
#version 300 es
#define USE_UNIFORMS

precision highp float;

uniform float time;
uniform vec2 resolution;

out vec4 fragColor;

const float bpm = 145.0;

#ifdef USE_UNIFORMS
uniform vec4 state;

float beat() {
  return state.x;
}

float fade_in() {
  return state.y;
}

float fade_out() {
  return state.z;
}

float fade() {
  return state.w;
}

#else

float fade_in() {
  return 0.;
}

float fade_out() {
  return smoothstep(-0.707, 0.707, sin(time));
}

float fade() {
  return 1.0;
}

float beat() {
  return exp(-1.*fract(time*bpm/60.));
}
#endif


#define TIME        time
#define RESOLUTION  resolution

#define PI              3.141592654
#define TAU             (2.0*PI)
#define TTIME           (TAU*TIME)

#define ROT(a)          mat2(cos(a), sin(a), -sin(a), cos(a))

const vec2 hexcell_sz       = vec2(1.0, sqrt(3.0));
const vec2 hexcell_hsz      = 0.5*hexcell_sz;

vec3 hsv2rgb(vec3 hsv) {
  return (cos(hsv.x*2.*acos(-1.)+vec3(0,4,2))*hsv.y+2.-hsv.y)*hsv.z/2.;
}

vec3 palette(float a) {
  return 0.5*(1.+sin(vec3(0,1,2)+a));
}

float hash(float co) {
  return fract(sin(co*12.9898) * 13758.5453);
}

float pmin(float a, float b, float k) {
  float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
  return mix( b, a, h ) - k*h*(1.0-h);
}

float pabs(float a, float k) {
  return -pmin(a,-a, k);
}

vec2 toPolar(vec2 p) {
  return vec2(length(p), atan(p.y, p.x));
}

vec2 toRect(vec2 p) {
  return vec2(p.x*cos(p.y), p.x*sin(p.y));
}

float tanh_approx(float x) {
  float x2 = x*x;
  return clamp(x*(27.0 + x2)/(27.0+9.0*x2), -1.0, 1.0);
}

vec2 mod2_1(inout vec2 p) {
  vec2 c = floor(p + 0.5);
  p = fract(p + 0.5) - 0.5;
  return c;
}

float modMirror1(inout float p, float size) {
  float halfsize = size*0.5;
  float c = floor((p + halfsize)/size);
  p = mod(p + halfsize,size) - halfsize;
  p *= mod(c, 2.0)*2.0 - 1.0;
  return c;
}

float modPolar(inout vec2 p, float repetitions) {
  float angle = 2.0*PI/repetitions;
  float a = atan(p.y, p.x) + angle/2.;
  float r = length(p);
  float c = floor(a/angle);
  a = mod(a,angle) - angle/2.;
  p = vec2(cos(a), sin(a))*r;
  if (abs(c) >= (repetitions/2.0)) c = abs(c);
  return c;
}

float hex(vec2 p, float r) {
  const vec3 k = vec3(-sqrt(3.0)/2.0,1.0/2.0,sqrt(3.0)/3.0);
  p = p.yx;
  p = abs(p);
  p -= 2.0*min(dot(k.xy,p),0.0)*k.xy;
  p -= vec2(clamp(p.x, -k.z*r, k.z*r), r);
  return length(p)*sign(p.y);
}

vec2 hextile(inout vec2 p) {
  vec2 p1 = mod(p, hexcell_sz)-hexcell_hsz;
  vec2 p2 = mod(p - hexcell_hsz, hexcell_sz)-hexcell_hsz;
  vec2 p3 = mix(p2, p1, vec2(dot(p1, p1) < dot(p2, p2)));
  vec2 n = round((p3 - p + hexcell_hsz)/hexcell_hsz);

  p = p3;

  return round(n*2.0)/2.0;
}

float circle(vec2 p, float r) {
  return length(p) - r;
}

float box(vec2 p, vec2 b) {
  vec2 d = abs(p)-b;
  return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}

float unevenCapsule(vec2 p, float r1, float r2, float h) {
  p.x = abs(p.x);
  float b = (r1-r2)/h;
  float a = sqrt(1.0-b*b);
  float k = dot(p,vec2(-b,a));
  if( k < 0.0 ) return length(p) - r1;
  if( k > a*h ) return length(p-vec2(0.0,h)) - r2;
  return dot(p, vec2(a,b) ) - r1;
}

vec2 cogwheel(vec2 p, float innerRadius, float outerRadius, float cogs, float holes) {
  float cogWidth  = 0.25*innerRadius*TAU/cogs;

  float d0 = circle(p, innerRadius);

  vec2 icp = p;
  modPolar(icp, holes);
  icp -= vec2(innerRadius*0.55, 0.0);
  float d1 = circle(icp, innerRadius*0.25);

  vec2 cp = p;
  modPolar(cp, cogs);
  cp -= vec2(innerRadius, 0.0);
  float d2 = unevenCapsule(cp.yx, cogWidth, cogWidth*0.75, (outerRadius-innerRadius));

  float d3 = circle(p, innerRadius*0.20);

  float hd = 1E6;

  hd = min(hd, d1);
  hd = min(hd, d3);

  float d = 1E6;
  d = min(d, d0);
  d = pmin(d, d2, 0.5*cogWidth);
  d = min(d, d2);
  d = max(d, -d1);
  d = max(d, -d3);

  return vec2(hd, d);
}

float smoothKaleidoscope(inout vec2 p, float sm, float rep) {
  vec2 hp = p;

  vec2 hpp = toPolar(hp);
  float rn = modMirror1(hpp.y, TAU/rep);

  float sa = PI/rep - pabs(PI/rep - abs(hpp.y), sm);
  hpp.y = sign(hpp.y)*(sa);

  hp = toRect(hpp);

  p = hp;

  return rn;
}

vec4 alphaBlend(vec4 back, vec4 front) {
  float w = front.w + back.w*(1.0-front.w);
  vec3 xyz = (front.xyz*front.w + back.xyz*back.w*(1.0-front.w))/w;
  return w > 0.0 ? vec4(xyz, w) : vec4(0.0);
}

vec3 alphaBlend(vec3 back, vec4 front) {
  vec3 colb = back.xyz;
  vec3 colf = front.xyz;
  vec3 xyz = mix(colb, colf.xyz, front.w);
  return xyz;
}

vec3 offset(float z) {
  float a = z;
  vec2 p = -0.075*(vec2(cos(a), sin(a*sqrt(2.0))) + vec2(cos(a*sqrt(0.75)), sin(a*sqrt(0.5))));
  return vec3(1.5*p, z);
}

vec3 doffset(float z) {
  float eps = 0.1;
  return 0.5*(offset(z + eps) - offset(z - eps))/eps;
}

vec3 ddoffset(float z) {
  float eps = 0.1;
  return 0.5*(doffset(z + eps) - doffset(z - eps))/eps;
}

const float plane0_lw = 0.05;

const mat2[] plane0_rots = mat2[](ROT(0.0*PI/2.0), ROT(1.00*PI/2.0), ROT(2.0*PI/2.0), ROT(3.0*PI/2.0));

const float plane0_smallCount = 16.0;

vec2 plane0_df0(vec2 p2, float n, float nr, out vec2 np2) {
  vec2 p = p2;
  vec2 hp = p;

  float hd = hex(hp, 0.5-plane0_lw)-plane0_lw;
  vec2 hn = hextile(hp);

  float d  = 1E6;
  d = min(d, circle(hp, 0.44));
  d = min(d, hd- plane0_lw);
  d = abs(d) - plane0_lw;

  np2 = hn;

  return vec2(hd, d);
}

vec2 plane0_cell0(vec2 p) {
  float d0  = circle(p-vec2(0.5), 0.5);
  float d1  = circle(p+vec2(0.5), 0.5);

  float d = 1E6;
  d = min(d, d0);
  d = min(d, d1);
  return vec2(d, 1E6);
}

vec2 plane0_cell1(vec2 p) {
  float d0  = abs(p.x);
  float d1  = abs(p.y);
  float d2 = circle(p, 0.25);

  float d = 1E6;
  d = min(d, d0);
  d = min(d, d1);
  return vec2(d, d2);
}

vec2 plane0_df1(vec2 p2, float n, float nr, out vec2 np2) {
  vec2 p = p2;
  float hd = hex(p, 0.4);

  vec2 hp = p;
  vec2 hn = mod2_1(hp);
  float r = hash(vec3(hn, n)+100.);

  hp *= plane0_rots[int(r*4.0)];

  vec2 cd0 = plane0_cell0(hp);
  vec2 cd1 = plane0_cell1(hp);
  vec2 d0 = mix(cd0, cd1, fract(r*13.) > 0.5);

  hd = min(hd, d0.y);

  float d = 1E6;
  d = min(d, d0.x);
  d = abs(d) - plane0_lw;
  d = min(d, hd - plane0_lw*2.);

  np2 = hn;

  return vec2(hd, d);
}

float psin(float a) {
  return 0.5 + 0.5*sin(a);
}


vec2 plane0_cell2(vec2 p, float r) {
  vec2 d = vec2(1E6);
  const float bigCount = 60.0;

  vec2 cp0 = p;
  cp0 *= ROT(-TTIME/bigCount);
  vec2 d0 = cogwheel(cp0, 0.36, 0.38, bigCount, 5.0);

  vec2 cp1 = p;
  float nm = modPolar(cp1, 6.0);

  cp1 -= vec2(0.5, 0.0);
  cp1 *= ROT(0.2+TAU*nm/2.0 + TTIME/plane0_smallCount);
  vec2 d1 = cogwheel(cp1, 0.11, 0.125, plane0_smallCount, 5.0);

  d = min(d, d0);
  d = min(d, d1);
  return d;
}

vec2 plane0_cell3(vec2 p, float r) {
  vec2 d = vec2(1E6);
  vec2 cp0 = p;
  float nm = modPolar(cp0, 6.0);
  vec2 cp1 = cp0;
  const float off = 0.275;
  const float count = plane0_smallCount + 2.0;
  cp0 -= vec2(off, 0.0);
  cp0 *= ROT(TAU*nm/2.0 - TTIME/count);
  vec2 d0 = cogwheel(cp0, 0.09, 0.105, count, 5.0);


  cp1 -= vec2(0.5, 0.0);
  cp1 *= ROT(0.2+TAU*nm/2.0 + TTIME/plane0_smallCount);
  vec2 d1 = cogwheel(cp1, 0.11, 0.125, plane0_smallCount, 5.0);

  float l = length(p);
  float d2 = l - (off+0.055);
  float d3 = d2 + 0.020;;

  vec2 tp0 = p;
  modPolar(tp0, 60.0);
  tp0.x -= off;
  float d4 = box(tp0, vec2(0.0125, 0.005));

  float ctime = -(TIME*0.05 + r)*TAU;

  vec2 tp1 = p;
  tp1 *= ROT(ctime*12.0);
  tp1.x -= 0.13;
  float d5 = box(tp1, vec2(0.125, 0.005));

  vec2 tp2 = p;
  tp2 *= ROT(ctime);
  tp2.x -= 0.13*0.5;
  float d6 = box(tp2, vec2(0.125*0.5, 0.0075));

  float d7 = l - 0.025;
  float d8 = l - 0.0125;

  d = min(d, d0);
  d = min(d, d1);

  float hd = d.x;
  hd = max(hd, -d2);
  hd = min(hd, d8);

  d = min(d, d2);
  d = max(d, -d3);
  d = min(d, d4);
  d = min(d, d5);
  d = min(d, d6);
  d = min(d, d7);
  d = max(d, -d8);

  return vec2(hd, d.y);
}

vec2 plane0_df4(vec2 p2, float n, float nr, out vec2 np2) {
  float hd = hex(p2, 0.45);

  vec2 hp = p2;

  vec2 hn = hextile(hp);
  float r = hash(vec3(hn, n));

  vec2 d;

  if (hd <0.05) {
    d = vec2(hd, hd-0.05);
  } else if (r < 0.5) {
    d = plane0_cell2(hp, r);
  } else {
    d = plane0_cell3(hp, r);
  }

  np2 = hn;
  return d;
}

vec2 plane0_df(vec3 pp, float n, out vec3 np) {
  float nr = hash(n);
  vec2 p2 = pp.xy;

  p2 *= ROT(TAU*nr+0.5*TIME*fract(3.0*nr));

  float rep = 2.0*floor(mix(3.0, 20.0, fract(7.0*nr)));
  float sm = mix(0.1, 0.025, fract(13.*nr))*24.0/rep;
  float skn = 0.;

  skn = smoothKaleidoscope(p2, sm, rep);

  float s = mix(0.05, 0.125, fract(nr*17.));

  vec2 np2;
  vec2 d = plane0_df1(p2/s, n, nr, np2)*s;

  np = vec3(np2, skn);

  return d;
}

vec4 plane(vec3 ro, vec3 rd, vec3 pp, vec3 off, float aa, float n) {
  vec3 hn;
  vec3 p = pp-off*vec3(1.0, 1.0, 0.0);
  float dc = length(p.xy);

  vec2 dd = plane0_df(p, n, hn);
  float d = dd.y;
  float a  = smoothstep(-aa, aa, -d);
  float ha = smoothstep(-aa, aa, dd.x);

  vec3 hsv = vec3(1.0, 0.0, 1.0);

  hsv = (vec3(sin(n*0.35)+sin(5.5*d+1.5*dc+length(p.xy-vec2(0.0, -0.25))-TTIME/13.0), 1.0, clamp(1.0-tanh_approx(200.*d), 0.0, 1.0)));
  vec3 baseCol = hsv2rgb(hsv)*fade();

  float m = mix(-1.0, 1.0, mod(n, 2.0));

  vec4 acol = vec4(baseCol, clamp(ha*pow(hsv.z, 0.05), 0.0, 1.0));
  vec4 bcol = vec4(0.0);
  vec4 ccol = vec4(mix(baseCol, vec3(0.0), a), ha*mix(0.66, 1.0, hsv.z));
  vec4 dcol = m*mix(bcol, acol, (1.0-tanh_approx(3.*length(p.xy))));
  vec4 col = ccol;
  return col;
}

vec3 skyColor(vec3 ro, vec3 rd) {
  float b = beat();
  float ld = max(dot(rd, vec3(0.0, 0.0, 1.0)), 0.0);
  vec3 baseCol = 3.*(0.25+palette(time))*(b*b*pow(ld, mix(40.0, 80.0, fade())));
  return baseCol;

}

const float planeDist = 1.0-0.88;
vec3 color(vec3 ww, vec3 uu, vec3 vv, vec3 ro, vec2 p) {

  vec2 aa = 1.0/RESOLUTION.xy;

  float fo = fade_out();
  float lp = length(p);
  p *= ROT(PI*lp*fo);
  vec2 np = p + aa;
  const float per = TAU*10.0;
  float rdd = (2.0+0.5*lp*tanh_approx(lp+0.9*psin(per*p.x)*psin(per*p.y)));
  rdd += -1.66*fo;
  vec3 rd = normalize(p.x*uu + p.y*vv + rdd*ww);
  vec3 nrd = normalize(np.x*uu + np.y*vv + rdd*ww);

  const int furthest = 11;
  const int fadeFrom = max(furthest-8, 0);
  const float fadeDist = planeDist*float(furthest - fadeFrom);
  float nz = floor(ro.z / planeDist);

  vec3 skyCol = skyColor(ro, rd);

  vec4 acol = vec4(0.0);
  const float cutOff = 0.95;
  bool cutOut = false;

  for (int i = 1; i <= furthest; ++i) {
    float pz = planeDist*nz + planeDist*float(i);

    float pd = (pz - ro.z)/rd.z;

    if (pd > 0.0 && acol.w < cutOff) {
      vec3 pp = ro + rd*pd;
      vec3 npp = ro + nrd*pd;

      float aa = 3.0*length(pp - npp);

      vec3 off = offset(pp.z);

      vec4 pcol = plane(ro, rd, pp, off, aa, nz+float(i));

      float nz = pp.z-ro.z;
      float fadeIn = exp(-2.5*max((nz - planeDist*float(fadeFrom))/fadeDist, 0.0));
      float fadeOut = smoothstep(0.0, planeDist*0.1, nz);
      pcol.xyz = mix(skyCol, pcol.xyz, (fadeIn));
      pcol.w *= fadeOut;

      pcol = clamp(pcol, 0.0, 1.0);

      acol = alphaBlend(pcol, acol);
    } else {
      cutOut = true;
      break;
    }

  }

  vec3 col = alphaBlend(skyCol, acol);
  col = pow(col+0.25*fo*fo*fo*fo, vec3(mix(1.0, 0.125, fo*fo)));
  col *= smoothstep(mix(4.0, 0.0, fo*fo), 0.0, lp);
  return col;
}

vec3 postProcess(vec3 col, vec2 q) {
  col = clamp(col, 0.0, 1.0);
  col = sqrt(col);
  col = col*0.6+0.4*col*col*(3.0-2.0*col);
  col = mix(col, vec3(dot(col, vec3(0.33))), -0.5);
  return col;
}

vec3 effect(vec2 p, vec2 q) {
  float tm = TIME*planeDist*bpm/60.;
  vec3 ro   = offset(tm);
  vec3 dro  = doffset(tm);
  vec3 ddro = ddoffset(tm);

  vec3 ww = normalize(dro);
  vec3 uu = normalize(cross(normalize(vec3(0.0,1.0,0.0)+ddro), ww));
  vec3 vv = normalize(cross(ww, uu));

  vec3 col = color(ww, uu, vv, ro, p);
  col = postProcess(col, q);
  col += fade_in();
  return col;
}

void main(void) {
  vec2 q = gl_FragCoord.xy/RESOLUTION.xy;
  vec2 p = -1. + 2. * q;
  p.x *= RESOLUTION.x/RESOLUTION.y;

  vec3 col = effect(p, q);
  fragColor = vec4(col, 1.0);
}
)SHADER";

  char const fragment_shader__lotus[] = R"SHADER(
#version 300 es
#define USE_UNIFORMS

precision highp float;

uniform float time;
uniform vec2 resolution;

out vec4 fragColor;

const float bpm = 145.0;

#ifdef USE_UNIFORMS
uniform vec4 state;

float beat() {
  return state.x;
}

float fade_in() {
  return state.y;
}

float fade_out() {
  return state.z;
}

float fade() {
  return state.w;
}

#else

float fade_in() {
  return 0.;
}

float fade_out() {
  return smoothstep(-0.707, 0.707, sin(time));
}

float fade() {
  return 1.0;
}

float beat() {
  return exp(-1.*fract(time*bpm/60.));
}
#endif


#define TIME        time
#define RESOLUTION  resolution
#define ROT(a)      mat2(cos(a), sin(a), -sin(a), cos(a))

const float
  pi    = acos(-1.)
, tau   = pi*2.
, phi   = (1.+sqrt(5.))*0.5
, phi2  = phi*phi
, phi4  = phi2*phi2
, iphi4 = 1./phi4
, br = 0.99
, con = 0.5
, conf = 1.3825
;

const vec2
  A = vec2(con,1.)
, B = vec2(con,-con*conf)
, C = vec2(-1.,-con*conf)
;

const mat2
  rot180 = ROT(2.*pi*0.5)
;


float circle(vec2 p, float r) {
  return length(p)-r;
}

float superCircle8(vec2 p, float r) {
  p *= p;
  p *= p;
  return pow(dot(p,p), 1./8.)-r;
}

vec3 palette(float a) {
  return 0.5*(1.+sin(vec3(0.,1.,2.)+a));
}

float geometric(float a, float r) {
  return a/(1.-r);
}

float igeometric(float a, float r, float x) {
  return log2(1.-x*(1.-r)/a)/log2(r);
}

vec2 geometric2(float a, float r, float n) {
  float rn = pow(r, n);
  float rn1 = rn*r;
  return (a/(1.-r))*(1.-vec2(rn, rn1));
}

vec4 boxySpiralCoord(vec2 p, float z, out float side) {
  float px = p.x;
  float ax = abs(px);
  float sx = sign(px);
  float a = px > 0. ? phi2 : 1.;
  a *= z;
  float gdx     = geometric(a, iphi4);
  ax            -= gdx;
  float x       = igeometric(a, iphi4, -ax);
  float nx      = floor(x);
  vec2  lx      = geometric2(a, iphi4, nx)-gdx;
  float minx    = lx.x;
  float maxx    = lx.y;
  float radiusx = (maxx-minx)*0.5;
  float meanx   = minx+radiusx;

  p -= vec2(-1.,1./3.)*meanx*sx;
  side = sx;
  return vec4(p, radiusx, 2.*nx+(sx>0.?0.:1.));
}

float dot2(vec2 p) {
  return dot(p,p);
}

// License: MIT, author: Inigo Quilez, found: https://www.iquilezles.org/www/articles/smin/smin.htm
float pmin(float a, float b, float k) {
  float h = clamp(0.5+0.5*(b-a)/k, 0.0, 1.0);
  return mix(b, a, h) - k*h*(1.0-h);
}
// License: CC0, author: Mårten Rånge, found: https://github.com/mrange/glsl-snippets
float pmax(float a, float b, float k) {
  return -pmin(-a, -b, k);
}

float pabs(float a, float k) {
  return -pmin(a,-a,k);
}


float bezier(vec2 pos, vec2 A, vec2 B, vec2 C ) {
    vec2 a = B - A;
    vec2 b = A - 2.0*B + C;
    vec2 c = a * 2.0;
    vec2 d = A - pos;
    float kk = 1.0/dot(b,b);
    float kx = kk * dot(a,b);
    float ky = kk * (2.0*dot(a,a)+dot(d,b)) / 3.0;
    float kz = kk * dot(d,a);
    float res = 0.0;
    float p = ky - kx*kx;
    float p3 = p*p*p;
    float q = kx*(2.0*kx*kx-3.0*ky) + kz;
    float h = q*q + 4.0*p3;
    if( h >= 0.0)
    {
        h = sqrt(h);
        vec2 x = (vec2(h,-h)-q)/2.0;
        vec2 uv = sign(x)*pow(abs(x), vec2(1.0/3.0));
        float t = clamp( uv.x+uv.y-kx, 0.0, 1.0 );
        res = dot2(d + (c + b*t)*t);
    }
    else
    {
        float z = sqrt(-p);
        float v = acos( q/(p*z*2.0) ) / 3.0;
        float m = cos(v);
        float n = sin(v)*1.732050808;
        vec3  t = clamp(vec3(m+m,-n-m,n-m)*z-kx,0.0,1.0);
        res = min( dot2(d+(c+b*t.x)*t.x),
                   dot2(d+(c+b*t.y)*t.y) );
        // the third root cannot be the closest
        // res = min(res,dot2(d+(c+b*t.z)*t.z));
    }
    return sqrt( res );
}

vec2 toSmith(vec2 p)  {
  // z = (p + 1)/(-p + 1)
  // (x,y) = ((1+x)*(1-x)-y*y,2y)/((1-x)*(1-x) + y*y)
  float d = (1.0 - p.x)*(1.0 - p.x) + p.y*p.y;
  float x = (1.0 + p.x)*(1.0 - p.x) - p.y*p.y;
  float y = 2.0*p.y;
  return vec2(x,y)/d;
}

vec2 fromSmith(vec2 p)  {
  // z = (p - 1)/(p + 1)
  // (x,y) = ((x+1)*(x-1)+y*y,2y)/((x+1)*(x+1) + y*y)
  float d = (p.x + 1.0)*(p.x + 1.0) + p.y*p.y;
  float x = (p.x + 1.0)*(p.x - 1.0) + p.y*p.y;
  float y = 2.0*p.y;
  return vec2(x,y)/d;
}


vec2 transform(vec2 p) {
  p.x = pabs(p.x,0.1)+0.3;
  p *= 0.6;

  float anim = TIME+20.0;
  vec2 sp0 = toSmith(p-0.);
  vec2 sp1 = toSmith(p+vec2(1.0)*ROT(0.12*anim));
  vec2 sp2 = toSmith(p-vec2(1.0)*ROT(0.23*anim));
  p = fromSmith(sp0+sp1-sp2);
  return p;
}

// http://mercury.sexy/hg_sdf/
float mod1(inout float p, float size) {
  float halfsize = size*0.5;
  float c = floor((p + halfsize)/size);
  p = mod(p + halfsize, size) - halfsize;
  return c;
}

// License: MIT OR CC-BY-NC-4.0, author: mercury, found: https://mercury.sexy/hg_sdf/
float modMirror1(inout float p, float size) {
  float halfsize = size*0.5;
  float c = floor((p + halfsize)/size);
  p = mod(p + halfsize,size) - halfsize;
  p *= mod(c, 2.0)*2.0 - 1.0;
  return c;
}

// https://iquilezles.org/articles/distfunctions2d
float vesica(vec2 p, float r, float d) {
  p = abs(p);
  float b = sqrt(r*r-d*d);
  return ((p.y-b)*d>p.x*b) ? length(p-vec2(0.0,b))
                           : length(p-vec2(-d,0.0))-r;
}
vec2 toPolar(vec2 p) {
  return vec2(length(p), atan(p.y, p.x));
}


vec2 toRect(vec2 p) {
  return vec2(p.x*cos(p.y), p.x*sin(p.y));
}

// https://iquilezles.org/articles/distfunctions2d
float unevenCapsule(vec2 p, float r1, float r2, float h) {
  p.x = abs(p.x);
  float b = (r1-r2)/h;
  float a = sqrt(1.0-b*b);
  float k = dot(p,vec2(-b,a));
  if( k < 0.0 ) return length(p) - r1;
  if( k > a*h ) return length(p-vec2(0.0,h)) - r2;
  return dot(p, vec2(a,b) ) - r1;
}

// https://iquilezles.org/articles/distfunctions2d
float parabola(vec2 pos, float wi, float he) {
  pos.x = abs(pos.x);
  float ik = wi*wi/he;
  float p = ik*(he-pos.y-0.5*ik)/3.0;
  float q = pos.x*ik*ik*0.25;
  float h = q*q - p*p*p;
  float r = sqrt(abs(h));
  float x = (h>0.0) ?
      pow(q+r,1.0/3.0) - pow(abs(q-r),1.0/3.0)*sign(r-q) :
      2.0*cos(atan(r/q)/3.0)*sqrt(p);
  x = min(x,wi);
  return length(pos-vec2(x,he-x*x/ik)) *
         sign(ik*(pos.y-he)+pos.x*pos.x);
}


float segmenty(vec2 p, float off) {
  p.y = abs(p.y);
  p.y -= off;
  float d0 = abs(p.x);
  float d1 = length(p);
  return p.y > 0.0 ? d1 : d0;
}

vec2 eye(vec2 p) {
  float a  = mix(0.0, 0.85, smoothstep(0.995, 1.0, cos(tau*TIME/5.0)));
  const float b = 4.0;
  float rr = mix(1.6, b, a);
  float dd = mix(1.12, b, a);

  vec2 p0 = p;
  p0 = p0.yx;
  float d0 =  vesica(p0, rr, dd);
  float d5 = d0;

  vec2 p1 = p;
  p1.y -= 0.28;
  float d1 = circle(p1, 0.622);
  d1 = max(d1,d0);

  vec2 p2 = p;
  p2 -= vec2(-0.155, 0.35);
  float d2 = circle(p2, 0.065);

  vec2 p3 = p;
  p3.y -= 0.28;
  p3 = toPolar(p3);
  float n3 = mod1(p3.x, 0.05);
  float d3 = abs(p3.x)-0.0125*(1.0-length(p1));

  vec2 p4 = p;
  p4.y -= 0.28;
  float d4 = circle(p4, 0.285);

  d3 = max(d3,-d4);

  d1 = pmax(d1,-d2, 0.0125);
  d1 = max(d1,-d3);

  float t0 = abs(0.9*p.x);
  t0 *= t0;
  t0 *= t0;
  t0 *= t0;
  t0 = clamp(t0, 0.0, 1.0);
  d0 = abs(d0)-mix(0.0125, -0.0025, t0);


  float d = d0;
  d = pmin(d, d1, 0.0125);
  return vec2(d, d5);
}


float starn(vec2 p, float r, float n, float m) {
  // next 4 lines can be precomputed for a given shape
  float an = 3.141593/float(n);
  float en = 3.141593/m;  // m is between 2 and n
  vec2  acs = vec2(cos(an),sin(an));
  vec2  ecs = vec2(cos(en),sin(en)); // ecs=vec2(0,1) for regular polygon

  float bn = mod(atan(p.x,p.y),2.0*an) - an;
  p = length(p)*vec2(cos(bn),abs(sin(bn)));
  p -= r*acs;
  p += ecs*clamp( -dot(p,ecs), 0.0, r*acs.y/ecs.y);
  return length(p)*sign(p.x);
}


vec2 lotus(vec2 p) {
  p.x = abs(p.x);
  vec2 p0 = p;
  p0 -= vec2(0.0, 0.180+0.00);
  float d0 = segmenty(p0, 0.61)-0.1;
  vec2 p1 = p;
  p1 -= vec2(0.2, 0.125);
  float d1 = segmenty(p1,0.55)-0.09;
  vec2 p2 = p;
  p2 -= vec2(0.0, -0.38+0.3);
  p2.y = -p2.y;
  float d2 = unevenCapsule(p2, 0.3, 0.38, 0.3);
  vec2 p3 = p;
  p3 -= vec2(0.47, -0.31);
  float d3 = parabola(p3, 0.37, 0.5);

  vec2 p4 = p;
  p4 -= vec2(0.99, -0.4);
  float d4 = circle(p4, 0.61);
  d3 = max(d3, -d4);

  vec2 p5 = p;
  p5 -= vec2(0.0, -0.45);
  float d5 = starn(p5.yx, 0.33, 10.0, 3.5);
  float d6 = abs(d5-0.005)-0.005;


  d0 = min(d0, d1);
  d3 = p.y > -0.40 ? d3 : d2;

  float dd = d3;
  dd = min(dd, d0);
  dd -= 0.02;

  float d = d3;
  d = min(d, d2);
  d = pmax(d, -(d0-0.01), 0.025);
  d = min(d, d0);
  float ds = max(min(d0, d3), -d5);
  d = max(d, -d6);

  float od = d;
  od = abs(od-0.02)-0.0075;

  d = min(d, od);
  d = pmin(d, d5, 0.01);
  return vec2(d, dd);
}

vec3 df(vec2 p) {
  const float zz = 1.0;
  p/=zz;
  p.y -= -0.05;
  vec2 dh = lotus(p);
  const float ze = 0.28;
  vec2 pe = p;
  pe -= vec2(0.0, -0.45);
  pe /= ze;
  vec2 de = eye(pe);
  de *= ze;

  float d = dh.x;
  d = max(d, -de.x);
  return vec3(d, dh.y, de.y)*zz;
}


vec3 effect(vec3 col, vec2 p) {
  vec2 dp = p;
  vec2 np = p + 1.0/RESOLUTION.y;
  vec2 tp = transform(p);
  vec2 ntp = transform(np);
  float aa = sqrt(2.0)*distance(tp, ntp);
  p = tp;

  float anim = TIME*0.5;

  float pft = fract(anim*0.25);
  float nft = floor(anim*0.25);
  float piz = exp2(log2(phi4)*pft);
  float sx;
  float sy;
  vec4 gcx = boxySpiralCoord(p, piz,sx);
  vec4 gcy = boxySpiralCoord(vec2(p.y,-p.x), phi*piz,sy);

  float dbx = superCircle8(gcx.xy, gcx.z*br);
  float dby = superCircle8(gcy.xy, gcy.z*br);

  float nx = 1.+2.*gcx.w+4.*nft;
  float ny = 2.*gcy.w+4.*nft;

  float db;
  vec4 gc;
  float n;
  float s;
  if (dbx < dby) {
    db = dbx;
    gc = gcx;
    n = nx;
    s = sx;
  } else {
    db = dby;
    gc = gcy;
    n = ny;
    s = sy;
  }

  if (s == -1.) {
    gc.xy*= rot180;
  }

  float sr = length(p)*0.025;
  vec3 bcol = palette(n*tau/16.);
  float dc = circle(gc.xy, gc.z*br)/gc.z;
  float ds = bezier(gc.xy, gc.z*A, gc.z*B, gc.z*C);

  db = pmax(db, -(ds-sr), 2.*sr);
  col = mix(col, bcol-0.5*dc, smoothstep(aa, -aa, db));
  col = clamp(col, 0.0, 1.0);

  vec3 d = df(dp);
  float daa = sqrt(2.)/RESOLUTION.y;

  vec3 lotusCol = (palette(dp.y+log(1.+p.y*p.y)/log(4.)+0.5*time+pi).zyx+aa*aa*mix(0.1,2.,beat()));
  lotusCol = mix(lotusCol, vec3(1.), smoothstep(daa, -daa, d.z));
  col = mix(col, vec3(0.0), smoothstep(daa, -daa, d.y));
  col = mix(col, lotusCol, smoothstep(daa, -daa, d.x));

  return col;
}

void main() {
  vec2 q = gl_FragCoord.xy/RESOLUTION.xy;
  vec2 p = -1. + 2. * q;
  p.x *= RESOLUTION.x/RESOLUTION.y;

  vec3 col = vec3(0.0);
  col = effect(col, p);
  col += fade_in();
  col = sqrt(col);

  fragColor = vec4(col, 1.0);
}
)SHADER";

  char const fragment_shader__reflections[] = R"SHADER(
#version 300 es
#define USE_UNIFORMS

precision highp float;

uniform float time;
uniform vec2 resolution;

out vec4 fragColor;

const float bpm = 145.0;

#ifdef USE_UNIFORMS
uniform vec4 state;

float beat() {
  return state.x;
}

float fade_in() {
  return state.y;
}

float fade_out() {
  return state.z;
}

float fade() {
  return state.w;
}

#else

float fade_in() {
  return 0.;
}

float fade_out() {
  return smoothstep(-0.707, 0.707, sin(time));
}

float fade() {
  return 1.0;
}

float beat() {
  return exp(-2.*fract(0.5*time*bpm/60.));
}
#endif


#define TIME        time
#define RESOLUTION  resolution

float g_beat;
mat3 g_rot;

const vec4 hsv2rgb_K = vec4(1., 2. / 3., 1. / 3., 3.);
#define HSV2RGB(c)  (c.z * mix(hsv2rgb_K.xxx, clamp(abs(fract(c.xxx + hsv2rgb_K.xyz) * 6.0 - hsv2rgb_K.www) - hsv2rgb_K.xxx, 0.0, 1.0), c.y))

#define INNER_REFLECTION

//#define INNER_SPHERE
//#define INNER_CAPSULE

#define TIME        time
#define RESOLUTION  resolution

#define TOLERANCE       1E-4
#define MAX_RAY_LENGTH  10.0
#define MAX_RAY_MARCHES 60
#define NORM_OFF        1E-3
#define ROT(a)          mat2(cos(a), sin(a), -sin(a), cos(a))
#define MAX_BOUNCES     6

const float
  pi            = acos(-1.)
, tau           = 2.*pi
, poly_U        = 1.
, poly_V        = 1.
, poly_W        = 0.
, poly_type     = 5.
, zoom          = 3.
, refraction    = .8
, inner_factor  = 1.
, soft_factor   = .25
, poly_cospin   = cos(pi/float(poly_type))
, poly_scospin  = sqrt(.75-poly_cospin*poly_cospin)
, initt         = .125
;

const vec2 impulse_sca0 = vec2(0.,1.);

const vec3
  skyCol     = HSV2RGB(vec3(.6, .86, 1.))
, lightCol   = HSV2RGB(vec3(0.6, 0.5, 2.0))
, bounceCol0 = HSV2RGB(vec3(0.0, 0.8, 0.8))
, bounceCol1 = HSV2RGB(vec3(0.6, 0.5, 0.8))
, poly_nc    = vec3(-0.5, -poly_cospin, poly_scospin)
, poly_pab   = vec3(0., 0., 1.)
, poly_pbc_  = vec3(poly_scospin, 0., 0.5)
, poly_pca_  = vec3(0., poly_scospin, poly_cospin)
, poly_p     = normalize((poly_U*poly_pab+poly_V*poly_pbc_+poly_W*poly_pca_))
, poly_pbc   = normalize(poly_pbc_)
, poly_pca   = normalize(poly_pca_)
, ro         = vec3(0., 0., 6.)
, la         = vec3(0., 0., 0.)
, up         = vec3(0., 1., 0.)
, ww         = normalize(la - ro)
, uu         = normalize(cross(up, ww ))
, vv         = normalize(cross(ww,uu))
;

vec3 aces_approx(vec3 v) {
  v = max(v, 0.0);
  v *= 0.6;
  const float
    a = 2.51
  , b = 0.03
  , c = 2.43
  , d = 0.59
  , e = 0.14
  ;
  return clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0f, 1.0f);
}

float dot2(vec3 p) {
  return dot(p, p);
}

float pmin(float a, float b, float k) {
  float h = clamp(0.5+0.5*(b-a)/k, 0.0, 1.0);
  return mix(b, a, h) - k*h*(1.0-h);
}

float pmax(float a, float b, float k) {
  return -pmin(-a, -b, k);
}

float capsule(vec3 p, float h, float r ) {
  p.y = abs(p.y);
  p.y -= clamp(p.y, 0.0, h);
  return length(p) - r;
}

float box(vec2 p, vec2 b) {
  vec2 d = abs(p)-b;
  return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}

float rayPlane(vec3 ro, vec3 rd, vec4 p) {
  return -(dot(ro,p.xyz)+p.w)/dot(rd,p.xyz);
}

void poly_fold(inout vec3 pos) {
  vec3 p = pos;

  for(float i = 0.; i < poly_type; ++i){
    p.xy  = abs(p.xy);
    p    -= 2.*min(0., dot(p,poly_nc)) * poly_nc;
  }

  pos = p;
}

float poly_plane(vec3 pos) {
  float d0 = dot(pos, poly_pab);
  float d1 = dot(pos, poly_pbc);
  float d2 = dot(pos, poly_pca);
  float d = d0;
  d = max(d, d1);
  d = max(d, d2);
  return d;

}

float poly_corner(vec3 pos) {
  float d = length(pos) - .05;
  return d;
}

float poly_edge(vec3 pos) {
  float dla = dot2(pos-min(0., pos.x)*vec3(1., 0., 0.));
  float dlb = dot2(pos-min(0., pos.y)*vec3(0., 1., 0.));
  float dlc = dot2(pos-min(0., dot(pos, poly_nc))*poly_nc);
  return sqrt(min(min(dla, dlb), dlc))-0.025;
}

float poly_planes(vec3 pos, out vec3 pp) {
  poly_fold(pos);
  pos -= poly_p;

  pp = pos;
  return poly_plane(pos);
}

float poly_edges(vec3 pos, out vec3 pp) {
  poly_fold(pos);
  pos -= poly_p;

  pp = pos;
  return poly_edge(pos);
}


vec3 skyColor(vec3 ro, vec3 rd) {
  vec3 col = vec3(0.);

  float tp0  = rayPlane(ro, rd, vec4(vec3(0.0, 1.0, 0.0), 4.0));
  float tp1  = rayPlane(ro, rd, vec4(vec3(0.0, -1.0, 0.0), 6.0));
  float tp = tp1;
  tp = max(tp0,tp1);


  if (tp1 > 0.0) {
    vec3 pos  = ro + tp1*rd;
    vec2 pp = pos.xz;
    float db = box(pp, vec2(6.0, 9.0))-1.0;

    col += vec3(4.0)*skyCol*rd.y*rd.y*smoothstep(0.25, 0.0, db);
    col += vec3(0.8)*skyCol*exp(-0.5*max(db, 0.0));
  }

  if (tp0 > 0.0) {
    vec3 pos  = ro + tp0*rd;
    vec2 pp = pos.xz;
    float ds = length(pp) - 0.5;

    col += vec3(0.25)*skyCol*exp(-.5*max(ds, 0.0));
  }


  return col;
}

float dfExclusion(vec3 p, out vec3 pp) {
  p*= g_rot;
  p /= zoom;
  float d0 = -poly_edges(p, pp);
  float d = d0;
  return d*zoom;
}

float shape(vec3 p) {
  vec3 pp;
  p /= zoom;

  float d0 = poly_planes(p, pp);
  float d = d0;

  return d*zoom;
}

float df0(vec3 p) {
  p*= g_rot;
  float d0 = shape(p);
  float d = d0;
  return d;
}

float df1(vec3 p) {
  p*= g_rot;
  float d0 = -shape(p);
  float d = d0;
#if defined(INNER_SPHERE)
  float d1 = length(p) - (2.0*inner_factor);
  d = min(d, d1);
#elif defined(INNER_CAPSULE)
  float d1 = capsule(p, .95*inner_factor, 1.25*inner_factor);
  d = min(d, d1);
#endif
  return d;
}

vec3 normal1(vec3 pos) {
  vec2  eps = vec2(NORM_OFF,0.0);
  vec3 nor;
  nor.x = df1(pos+eps.xyy) - df1(pos-eps.xyy);
  nor.y = df1(pos+eps.yxy) - df1(pos-eps.yxy);
  nor.z = df1(pos+eps.yyx) - df1(pos-eps.yyx);
  return normalize(nor);
}

float rayMarch1(vec3 ro, vec3 rd) {
  float t = 0.0;
  for (int i = 0; i < MAX_RAY_MARCHES; i++) {
    if (t > MAX_RAY_LENGTH) {
      t = MAX_RAY_LENGTH;
      break;
    }
    float d = df1(ro + rd*t);
    if (d < TOLERANCE) {
      break;
    }
    t  += d;
  }
  return t;
}

vec3 normal0(vec3 pos) {
  vec2  eps = vec2(NORM_OFF,0.0);
  vec3 nor;
  nor.x = df0(pos+eps.xyy) - df0(pos-eps.xyy);
  nor.y = df0(pos+eps.yxy) - df0(pos-eps.yxy);
  nor.z = df0(pos+eps.yyx) - df0(pos-eps.yyx);
  return normalize(nor);
}

float rayMarch0(vec3 ro, vec3 rd) {
  float t = 0.0;
  for (int i = 0; i < MAX_RAY_MARCHES; i++) {
    if (t > MAX_RAY_LENGTH) {
      t = MAX_RAY_LENGTH;
      break;
    }
    float d = df0(ro + rd*t);
    if (d < TOLERANCE) {
      break;
    }
    t  += d;
  }
  return t;
}

vec3 render1(vec3 ro, vec3 rd) {
  vec3 agg = vec3(0.0, 0.0, 0.0);
  float tagg = initt;
  vec3 ragg = vec3(1.0);

  for (int bounce = 0; bounce < MAX_BOUNCES; ++bounce) {
    float mragg = max(max(ragg.x, ragg.y), ragg.z);
    if (mragg < 0.1) break;
    float st = rayMarch1(ro, rd);
    tagg += st;
    vec3 sp = ro+rd*st;
    vec3 spp;
    float de = dfExclusion(sp, spp);
    vec3 sn = normal1(sp);
    float fre = 1.0+dot(rd, sn);


    float si = cos(2.0*tau*zoom*spp.z-0.5*sp.y);
    float lf = mix(0.0, 1.0, smoothstep(0., 0.9, si));

    vec3 gcol = ragg*lightCol*exp(8.0*(min(de-0.2, 0.0)));
    if (de < 0.0) {
      agg += gcol;
      ragg *= mix(bounceCol1, bounceCol0, fre);
    } else {
      agg += gcol*lf;
      agg += ragg*lightCol*1.5*lf;
      ragg = vec3(0.0);
    }

    rd = reflect(rd, sn);
    ro = sp+initt*rd;
    tagg += initt;
  }
  return agg*exp(-mix(1.,-1., g_beat)*vec3(0.1, 0.2, 0.15)*tagg);
}

vec3 render0(vec3 ro, vec3 rd) {
  vec3 skyCol = skyColor(ro, rd);

  vec3 col = vec3(0.0);

  float st = rayMarch0(ro, rd);
  vec3 sp = ro+rd*st;
  vec3 sn = normal0(sp);
  vec3 spp;
  float de = dfExclusion(sp, spp);
  float ptime = mod(TIME, 30.0);
  if (st < MAX_RAY_LENGTH) {
    float sfre = 1.0+dot(rd, sn);
    sfre *= sfre;
    sfre = mix(0.1, 1.0, sfre);
    vec3 sref   = reflect(rd, sn);
    vec3 srefr  = refract(rd, sn, refraction);
    vec3 ssky = sfre*skyColor(sp, sref);

    if (de > 0.0) {
      col = ssky;
    } else {
      col = 0.5*sfre*ssky;
      vec3 col1 = (1.0-sfre)*render1(sp+srefr*initt, srefr);
      col += col1;
    }
  }


  return col;
}


float circle(vec2 p, float r) {
  return length(p) - r;
}

float horseshoe(vec2 p, vec2 c, float r, vec2 w) {
  p.x = abs(p.x);
  float l = length(p);
  p = mat2(-c.x, c.y,
            c.y, c.x)*p;
  p = vec2((p.y>0.0)?p.x:l*sign(-c.x),
            (p.x>0.0)?p.y:l );
  p = vec2(p.x,abs(p.y-r))-w;
  return length(max(p,0.0)) + min(0.0,max(p.x,p.y));
}

float impulse_e(inout vec2 pp, float off) {
  vec2 p = pp;
  pp.x -= 1.05+off;
  p -= vec2(0.5, 0.5);
  return min(box(p, vec2(0.4, 0.1)), max(circle(p, 0.5), -circle(p, 0.3)));
}

float impulse_I(inout vec2 pp, float off) {
  vec2 p = pp;
  pp.x -= 0.25+off;
  p -= vec2(0.125, 0.75);
  return box(p, vec2(0.125, 0.75));
}

float impulse_l(inout vec2 pp, float off) {
  vec2 p = pp;
  pp.x -= 0.2+off;
  p -= vec2(0.10, 0.5);
  return box(p, vec2(0.1, 0.666));
}

float impulse_m(inout vec2 pp, float off) {
  vec2 p = pp;
  pp.x -= 2.2+off;
  p -= vec2(1.1, 0.5);
  p.y = -p.y;
  p.x = abs(p.x);
  p -= vec2(0.5, 0.0);
  float d = horseshoe(p, impulse_sca0, 0.5, vec2(0.5, 0.1));
  return d;
}

float impulse_p(inout vec2 pp, float off) {
  vec2 p = pp;
  pp.x -= 1.05+off;
  p -= vec2(0.55, 0.5);
  float b = box(p - vec2(-0.45, -0.25), vec2(0.1, 0.75));
  float c = abs(circle(p, 0.4)) - 0.1;
  return min(b, c);
}

float impulse_s(inout vec2 pp, float off) {
  const mat2
    rots1 = ROT(-2.*pi/3.0)
  , rots2 = ROT(pi)
  ;
  vec2 p = pp;
  pp.x -= 0.875+off;
  p -= vec2(0.435, 0.5);
  p *= rots1;
  float u = horseshoe(p - vec2(-0.25*3.0/4.0, -0.125/2.0), impulse_sca0, 0.375, vec2(0.2, 0.1));
  p *= rots2;
  float l = horseshoe(p - vec2(-0.25*3.0/4.0, -0.125/2.0), impulse_sca0, 0.375, vec2(0.2, 0.1));
  return min(u,l);
}

float impulse_u(inout vec2 pp, float off) {
  vec2 p = pp;
  pp.x -= 1.2+off;
  p -= vec2(0.6, 0.475);
  return horseshoe(p - vec2(0.0, 0.125), impulse_sca0, 0.5, vec2(0.4, 0.1));
}

float impulse(vec2 p, float off) {
  p += vec2(3.385+3.0*off, 0.5);

  float d = 1E6;
  d = min(d, impulse_I(p, off));
  d = min(d, impulse_m(p, off));
  d = min(d, impulse_p(p, off));
  d = min(d, impulse_u(p, off));
  d = min(d, impulse_l(p, off));
  d = min(d, impulse_s(p, off));
  d = min(d, impulse_e(p, off));

  return d;
}

float dfcos(float x) {
  return sqrt(x*x+1.0)*0.8-1.8;
}

float dfcos(vec2 p, float freq) {
  float x = p.x;
  float y = p.y;
  x *= freq;

  float x1 = abs(mod(x+pi,tau)-pi);
  float x2 = abs(mod(x   ,tau)-pi);

  float a = 0.18*freq;

  x1 /= max( y*a+1.0-a,1.0);
  x2 /= max(-y*a+1.0-a,1.0);
  return (mix(-dfcos(x2)-1.0,dfcos(x1)+1.0,clamp(y*0.5+0.5,0.0,1.0)))/max(freq*0.8,1.0)+max(abs(y)-1.0,0.0)*sign(y);
}


float df(vec2 p) {
  const float
    z0 = 0.25
  , z1 = 0.05
  ;
  float b = fract(0.5*bpm*time/60.+0.125*p.x)-0.5;
  b *= b;
  vec2 p0 = p;
  p0 /= z0;
  vec2 p1 = p;
  p1 /= z1;
  p1.x += TIME*2.;

  float d0 = impulse(p0,0.25)*z0;
  float d1 = abs(dfcos(p1, z1*tau))*z1-mix(0.01, 0.002, p.x*p.x);

  float d = d0;
  d = pmax(d,-(d1-0.01), 0.02);
  d = min(d,d1);
  return d;
}

mat3 rot(float time) {
  float
    angle1 = time * 0.5
  , angle2 = time * 0.707
  , angle3 = time * 0.33
  , c1 = cos(angle1); float s1 = sin(angle1)
  , c2 = cos(angle2); float s2 = sin(angle2)
  , c3 = cos(angle3); float s3 = sin(angle3)
  ;

  return mat3(
      c1 * c2,
      c1 * s2 * s3 - c3 * s1,
      s1 * s3 + c1 * c3 * s2,

      c2 * s1,
      c1 * c3 + s1 * s2 * s3,
      c3 * s1 * s2 - c1 * s3,

      -s2,
      c2 * s3,
      c2 * c3
  );
}

vec3 effect(vec2 p) {
  g_rot = rot(TIME);
  g_beat = beat();
  vec3 rd = normalize(-p.x*uu + p.y*vv + 1.41*ww);

  vec3 col = render0(ro, rd);

  return col;
}

void main() {
  vec2 q = gl_FragCoord.xy/RESOLUTION.xy;
  vec2 p = -1. + 2. * q;
  p.x *= RESOLUTION.x/RESOLUTION.y;
  vec3 col = vec3(0.0);
  col = effect(p);
  col = aces_approx(col);
  float d = df(p);
  float aa = sqrt(2.)/RESOLUTION.y;

  col = mix(col, vec3(1.0), smoothstep(aa, -aa, d));
  col += fade_in();
  col = sqrt(col);
  fragColor = vec4(col, 1.0);
}
)SHADER";
}
