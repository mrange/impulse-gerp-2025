#include "precompiled.hpp"

#include "effect.hpp"

#include "shader_sources.h"

namespace {

#ifdef _DEBUG
  void APIENTRY debug__callback(
      GLenum source           // Where the error came from
    , GLenum type             // The type of error
    , GLuint id               // Error ID
    , GLenum severity         // How serious the error is
    , GLsizei length          // Length of the error message
    , GLchar const* message   // The error message itself
    , void const* userParam   // User-provided data (unused)
    ) {
      std::printf(message);
      std::printf("\n");
  }
  char debug__log[0xFFFF];  // 65535 characters
#endif

  const std::size_t no_shaders = 5;
  char const * fragment_shader_sources[no_shaders]={
    fragment_shader__mandelbox
  , fragment_shader__bw
  , fragment_shader__psychedelic
  , fragment_shader__lotus
  , fragment_shader__reflections
  };

  struct t_shader {
    GLuint  program           ;
    GLint   loc__time         ;
    GLint   loc__resolution   ;
    GLint   loc__state        ;
  };

  t_shader shaders[no_shaders];



  void init_effect() {
    auto fp__glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv");
    auto fp__glGetUniformLocation   = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
    assert(fp__glCreateShaderProgramv);
    assert(fp__glGetUniformLocation);

    for (std::size_t i = 0; i < no_shaders; ++i) {
      auto & shader = shaders[i];

      GLchar const* fragment_shaders[] = { fragment_shader_sources[i] };
      shader.program = fp__glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, fragment_shaders);
      assert(shader.program > 0);

      shader.loc__time          = fp__glGetUniformLocation(shader.program, "time");
      shader.loc__resolution    = fp__glGetUniformLocation(shader.program, "resolution");
      shader.loc__state         = fp__glGetUniformLocation(shader.program, "state");
      assert(shader.loc__time > -1);
      assert(shader.loc__resolution > -1);
      assert(shader.loc__state > -1);

#ifdef _DEBUG
      auto glGetProgramInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
      glGetProgramInfoLog(shader.program, sizeof(debug__log), NULL, debug__log);
      printf(debug__log);
#endif
    }

  }

  void deinit_effect() {
    auto fp__glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
    assert(fp__glDeleteProgram);
    for (std::size_t i = 0; i < no_shaders; ++i) {
      auto & shader = shaders[i];
      fp__glDeleteProgram(shader.program);
    }
  }

}


void init__effect8() {
  init_effect();
}

void deinit__effect8() {
  deinit_effect();
}

effect_kind effect8(effect_input const & ei, std::size_t shader_id) {
  assert(shader_id < no_shaders);

  auto & shader = shaders[shader_id];

  auto fp__glUniform1f  = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
  auto fp__glUniform2f  = (PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f");
  auto fp__glUniform4f  = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");
  auto fp__glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
  assert(fp__glUniform1f);
  assert(fp__glUniform1f);
  assert(fp__glUseProgram);

  GLint currentProgram = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

  fp__glUseProgram(shader.program);

  assert(shader.loc__time > -1);
  assert(shader.loc__resolution > -1);

  fp__glUniform1f(shader.loc__time, ei.time);
  fp__glUniform2f(
    shader.loc__resolution
  , static_cast<GLfloat>(ei.viewport__width)
  , static_cast<GLfloat>(ei.viewport__height)
  );
  auto quad = [](float x) { return x*x; };
  switch (shader_id) {
    case 0:
      fp__glUniform4f(
          shader.loc__state
        , music__drum(ei.time)
        ,   smoothstep(music__from_nbeat(ei.beat__start+1), music__from_nbeat(ei.beat__start), ei.time)
          + smoothstep(music__from_nbeat(168+1), music__from_nbeat(168), ei.time)*step(music__from_nbeat(168), ei.time)
        , quad(linstep(music__from_nbeat(ei.beat__end-4), music__from_nbeat(ei.beat__end), ei.time))
        , smoothstep(music__from_nbeat(164), music__from_nbeat(170), ei.time)
        );
      break;
    case 1:
      fp__glUniform4f(
          shader.loc__state
        , music__drum(ei.time)
        , smoothstep(music__from_nbeat(ei.beat__start+1), music__from_nbeat(ei.beat__start), ei.time)
        , quad(linstep(music__from_nbeat(ei.beat__end-4), music__from_nbeat(ei.beat__end), ei.time))
        , smoothstep(music__from_nbeat(164), music__from_nbeat(170), ei.time)
        );
      break;
    case 2:
      fp__glUniform4f(
          shader.loc__state
        , music__beat(ei.time)
        , smoothstep(music__from_nbeat(ei.beat__start+1), music__from_nbeat(ei.beat__start), ei.time)
          + smoothstep(music__from_nbeat(296+1), music__from_nbeat(296), ei.time)*step(music__from_nbeat(296), ei.time)
        , quad(linstep(music__from_nbeat(ei.beat__end-12), music__from_nbeat(ei.beat__end), ei.time))
        , step(music__from_nbeat(296), ei.time)
        );
      break;
    case 3:
      fp__glUniform4f(
          shader.loc__state
        , music__beat(ei.time)
        , smoothstep(music__from_nbeat(ei.beat__start+1), music__from_nbeat(ei.beat__start), ei.time)
        , 0.
        , 0.
        );
      break;
    case 4:
      fp__glUniform4f(
          shader.loc__state
        , music__beat(ei.time)
        , smoothstep(music__from_nbeat(ei.beat__start+1), music__from_nbeat(ei.beat__start), ei.time)
        , 0.
        , 0.
        );
      break;
  }

  glRects(-1, -1, 1, 1);

  fp__glUseProgram(currentProgram);

  return sixel_effect;
}
