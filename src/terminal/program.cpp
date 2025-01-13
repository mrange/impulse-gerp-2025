#include "precompiled.hpp"

#include "screen.hpp"
#include "effect.hpp"

#include <cstdio>
#include <gl/GL.h>
#include "glext.h"

#include <conio.h>

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "opengl32.lib")

#define USE_BACKGROUND_WRITER_THREAD
#define USE_MMX
#define MUSIC_TIME

//#define SHOW_WINDOW
#define INFO_TEXT

void init__effect8();
void deinit__effect8();

effect_kind effect0(effect_input const & ei);
effect_kind effect1(effect_input const & ei);
effect_kind effect2(effect_input const & ei);
effect_kind effect3(effect_input const & ei);
effect_kind effect4(effect_input const & ei);
effect_kind effect5(effect_input const & ei);
effect_kind effect6(effect_input const & ei);
effect_kind effect7(effect_input const & ei);
effect_kind effect8(effect_input const & ei, std::size_t shader_id);
effect_kind effect9(effect_input const & ei);

namespace {
  std::wstring const info_text = L"\x1B[2J\x1B[H" LR"INFO(╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║        ██▓ ███▄ ▄███▓ ██▓███   █    ██  ██▓      ██████ ▓█████  ▐██▌         ║
║       ▓██▒▓██▒▀█▀ ██▒▓██░  ██▒ ██  ▓██▒▓██▒    ▒██    ▒ ▓█   ▀  ▐██▌         ║
║       ▒██▒▓██    ▓██░▓██░ ██▓▒▓██  ▒██░▒██░    ░ ▓██▄   ▒███    ▐██▌         ║
║       ░██░▒██    ▒██ ▒██▄█▓▒ ▒▓▓█  ░██░▒██░      ▒   ██▒▒▓█  ▄  ▓██▒         ║
║       ░██░▒██▒   ░██▒▒██▒ ░  ░▒▒█████▓ ░██████▒▒██████▒▒░▒████▒ ▒▄▄          ║
║       ░▓  ░ ▒░   ░  ░▒▓▒░ ░  ░░▒▓▒ ▒ ▒ ░ ▒░▓  ░▒ ▒▓▒ ▒ ░░░ ▒░ ░ ░▀▀▒         ║
║        ▒ ░░  ░      ░░▒ ░     ░░▒░ ░ ░ ░ ░ ▒  ░░ ░▒  ░ ░ ░ ░  ░ ░  ░         ║
║        ▒ ░░      ░   ░░        ░░░ ░ ░   ░ ░   ░  ░  ░     ░       ░         ║
║        ░         ░               ░         ░  ░      ░     ░  ░ ░            ║
║                                                                              ║
║                               ♥ G O E S ♥ T O ♥                              ║
║                                                                              ║
║    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░    ║
║    ░░░░░░░░      ░░░░░░░░░        ░░░░░░░░       ░░░░░░░░░       ░░░░░░░░    ║
║    ▒▒▒▒▒▒▒  ▒▒▒▒▒▒▒▒▒▒▒▒▒▒  ▒▒▒▒▒▒▒▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒▒▒    ║
║    ▓▓▓▓▓▓▓  ▓▓▓   ▓▓▓▓▓▓▓▓      ▓▓▓▓▓▓▓▓▓▓       ▓▓▓▓▓▓▓▓▓       ▓▓▓▓▓▓▓▓    ║
║    ███████  ████  ████████  ██████████████  ███  █████████  █████████████    ║
║    ████████      █████████        ████████  ████  ████████  █████████████    ║
║    ██████████████████████████████████████████████████████████████████████    ║
║                                                                              ║
║    Music by amazing Astroboy and licensed under CC BY-NC-ND 3.0              ║
║                         ╰────────→ https://sampleswap.org/artist/astroboy    ║
║                                                                              ║
║    This demo requires a Sixel (tech from 1970s) compatible terminal such as: ║
║     * Windows Terminal Preview version 1.22.3232.0                           ║
║                                                                              ║
║    Make sure the entire border is visible in the terminal.                   ║
║                                                                              ║
╚══════════════════ When ready hit any key to start the demo ══════════════════╝
)INFO";

  std::wstring const end_text = L"\x1B[2J\x1B[H\x1B[?25h";

  std::size_t const desired__width  = 800;
  std::size_t const desired__height = 600;
  wchar_t const windows_class_name[]= L"TerminalEffects";

  std::size_t viewport__width       = desired__width ;
  std::size_t viewport__height      = desired__height;

  std::u8string to_u8string(std::string const & s) {
    return std::u8string(reinterpret_cast<char8_t const *>(s.c_str()), s.size());
  }

  std::u8string const prelude__goto_top    = u8"\x1B[?25l\x1B[H";
  std::u8string const reset__colors        = u8"\x1B[0m";
  std::u8string const prelude__foreground  = u8"\x1B[38;2";
  std::u8string const prelude__background  = u8"\x1B[48;2";

  using f__effect = std::function<effect_kind (
      effect_input const & ei
    )>;

  struct script_part {
    std::size_t   beat__start ;
    f__effect     effect      ;
    std::wstring  name        ;
  };

  struct effective_script_part {
    std::size_t beat__start ;
    std::size_t beat__end   ;
    f__effect   effect      ;
    std::wstring  name        ;
  };

  auto const start_time = 0*music__beat_time;
  std::array<effective_script_part, music__beat_length> effective_script;
  auto script = std::to_array<script_part>({
    {0  , effect7                                                 , L"Running INTRO.COM"                  }
  , {64 , effect2                                                 , L"Gfx by Glimglam, Code by Lance"     }
  , {120, effect9                                                 , L"With Love from Impulse"             }
  , {136, [](effect_input const & ei) { return effect8(ei, 0); }  , L"An approximation of a cube"         }
  , {196, effect1                                                 , L"With Love from Impulse - Encore!"   }
  , {232, [](effect_input const & ei) { return effect8(ei, 1); }  , L"It doesn't matter if you are B or W"}
  , {260, effect5                                                 , L"This star scroller sucks!"          }
  , {264, [](effect_input const & ei) { return effect8(ei, 2); }  , L"Trippy tunneling"                   }
  , {324, effect5                                                 , L"This star scroller still sucks!"    }
  , {328, [](effect_input const & ei) { return effect8(ei, 3); }  , L"The other Lotus challenge"          }
  , {360, [](effect_input const & ei) { return effect8(ei, 4); }  , L"Another approximative cube"         }
  , {392, effect4                                                 , L"Let's greet some people"            }
  , {456, effect3                                                 , L"That's all from Impulse folks!"     }
  , {528, effect0                                                 , L"FITB"                               }
  });

  script_part get__script_part(std::size_t i) {
    if (i < script.size()) {
      return script[i];
    } else {
      return script_part {
        effective_script.size()
      , effect0
      , L"FITB"
      };
    }
  }

  inline void append(
      std::vector<char8_t>          & output
    , std::u8string const           & v
    ) {
    output.insert(output.end(), v.begin(), v.end());
  }

  std::array<std::u8string, 256> generate__color_values() {
    std::array<std::u8string, 256> res;
    for (std::size_t i = 0; i < 256; ++i) {
      res[i] = to_u8string(std::format(";{}", i));
    }
    return res;
  }
  std::array<std::u8string, 256> color_values = generate__color_values();

  void wchar_to_utf8(std::vector<char8_t> & output, wchar_t wc) {
    uint32_t codepoint = wc; // Assume UTF-32
    if (codepoint <= 0x7F) {
      output.push_back(static_cast<char8_t>(codepoint));
    } else if (codepoint <= 0x7FF) {
      output.push_back(static_cast<char8_t>(0xC0 | ((codepoint >> 6) & 0x1F)));
      output.push_back(static_cast<char8_t>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0xFFFF) {
      output.push_back(static_cast<char8_t>(0xE0 | ((codepoint >> 12) & 0x0F)));
      output.push_back(static_cast<char8_t>(0x80 | ((codepoint >> 6) & 0x3F)));
      output.push_back(static_cast<char8_t>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0x10FFFF) {
      output.push_back(static_cast<char8_t>(0xF0 | ((codepoint >> 18) & 0x07)));
      output.push_back(static_cast<char8_t>(0x80 | ((codepoint >> 12) & 0x3F)));
      output.push_back(static_cast<char8_t>(0x80 | ((codepoint >> 6) & 0x3F)));
      output.push_back(static_cast<char8_t>(0x80 | (codepoint & 0x3F)));
    }
  }

  void wchars_to_utf8(std::vector<char8_t> & output, std::wstring const & wcs) {
    for (auto wc : wcs) {
      wchar_to_utf8(output, wc);
    }
  }

  void write__color(
      std::vector<char8_t> &  output
    , std::u8string const  &  prelude
    , vec3 const &            color
    ) {
    append(output, prelude);
    auto to_i = [](float v) -> std::size_t {
      return static_cast<std::size_t>(std::roundf(std::sqrtf(std::clamp<float>(v, 0, 1))*255));
    };
    append(output, color_values[to_i(color.x)]);
    append(output, color_values[to_i(color.y)]);
    append(output, color_values[to_i(color.z)]);
    output.push_back(u8'm');
  }

  void write__reset_color(
      std::vector<char8_t> &  output
    ) {
    append(output, reset__colors);
  }

  void write__screen(
      std::vector<char8_t>  & output
    , screen const &          screen
    ) {
    append(output, prelude__goto_top);

    auto w = screen.width;
    auto h = screen.height;
    assert(w*h == screen.shapes.size());
    assert(w*h == screen.foreground.size());
    assert(w*h == screen.background.size());

    vec3 foreground = vec3 {1,1,1};
    vec3 background = vec3 {0,0,0};

    for (std::size_t y = 0; y < h; ++y) {
      write__color(output, prelude__foreground, foreground);
      write__color(output, prelude__background, background);
      auto y__off = y*w;
      for (std::size_t x = 0; x < w; ++x) {
        auto wc = screen.shapes[y__off+x];
        auto new_foreground = screen.foreground[y__off+x];
        auto new_background = screen.background[y__off+x];

        if (memcmp(&new_foreground, &foreground, sizeof(foreground)) != 0) {
          foreground = new_foreground;
          write__color(output, prelude__foreground, foreground);
        }

        if (memcmp(&new_background, &background, sizeof(background)) != 0) {
          background = new_background;
          write__color(output, prelude__background, background);
        }

        wchar_to_utf8(output, wc);
      }

      write__reset_color(output);
      wchar_to_utf8(output, L'\n');
    }
  }

  void write__footer(
      std::vector<char8_t> &        output
    , float                         time
    , effective_script_part const & part
    ) {
    /*⣀⣤⣶⣿ */
    wchar_t fg__red[]     = L"\x1B[38;2;255;85;85m";
    wchar_t fg__orange[]  = L"\x1B[38;2;255;165;85m";
    wchar_t fg__yellow[]  = L"\x1B[38;2;255;255;85m";
    wchar_t fg__muted[]   = L"\x1B[38;2;80;80;120m";
    wchar_t fg__hilight[] = L"\x1B[38;2;160;160;220m";
    wchar_t fg__cyan[]    = L"\x1B[38;2;85;255;255m";

    wchar_t fg__white[]   = L"\x1B[38;2;255;255;255m";

    wchar_t bg__blue[]    = L"\x1B[48;2;10;10;80m";

    wchar_t* beat__colors[] = {
      fg__red
    , fg__orange
    , fg__yellow
    , fg__white
    };

    wchar_t   info__buffer[2048];
    auto      beat__i         = 3-(music__nsubdivision(time)%4);
    wchar_t   beat__progress  = L"⣀⣤⣶⣿"[beat__i];
    wchar_t*  beat__color     = beat__colors[beat__i];

    auto      run__i          = static_cast<int>(fractf(0.5F*time/music__beat_time)*8);
    wchar_t   run__progress   = L"⣤⣆⡇⠏⠛⠹⢸⣰"[run__i];

    wchar_t const * part__progress_parts[] = {
      L"▱▱▱▱▱"
    , L"▰▱▱▱▱"
    , L"▰▰▱▱▱"
    , L"▰▰▰▱▱"
    , L"▰▰▰▰▱"
    , L"▰▰▰▰▰"
    };
    int part__i;
    {
      auto x = time - music__from_nbeat(part.beat__start);
      auto y = music__from_nbeat(part.beat__end) - music__from_nbeat(part.beat__start);
      part__i = std::clamp<int>(static_cast<int>(6*x/y), 0, 5);
    }
    wchar_t const * part__progress = part__progress_parts[part__i] ;

    auto sub = fractf(time);
    auto sec = static_cast<int>(std::floorf(time));
    auto min = sec/60;
    sec -= min*60;
    auto ms  = static_cast<int>(sub*1000);

    auto info__len = swprintf_s(
        info__buffer
      , L"%s%s%c %s[%s%03d%s/%s%03d%s] %s%02d%s:%s%02d%s.%s%03d %c %s%-38s %s%s GERP 2025 %s%c"
      , bg__blue
      , beat__color
      , beat__progress
      , fg__muted
      , fg__hilight
      , music__nbeat(time)
      , fg__muted
      , fg__hilight
      , music__beat_length
      , fg__muted
      , fg__hilight
      , min
      , fg__muted
      , fg__hilight
      , sec
      , fg__muted
      , fg__hilight
      , ms
      , run__progress
      , fg__cyan
      , part.name.c_str()
      , fg__hilight
      , part__progress
      , beat__color
      , beat__progress
      );
    assert(info__len > -1);
    for (auto i = 0; i < info__len; ++i) {
      wchar_to_utf8(output, info__buffer[i]);
    }

    write__reset_color(output);
  }

  LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_KEYDOWN: {
      if (wParam == VK_ESCAPE) {
        ::PostQuitMessage(0);
        return 0;
      }
      else if ((wParam == 'C') && (GetKeyState(VK_CONTROL) & 0x8000)) {
        ::PostQuitMessage(0);
        return 0;
      }
      break;
    }
    case WM_SIZE: {
      int width = LOWORD(lParam);
      int height = HIWORD(lParam);

      glViewport(0, 0, width, height);

      viewport__width   = width;
      viewport__height  = height;

      return 0;
    }
    case WM_CLOSE:
      ::PostQuitMessage(0);
      return 0;
    }

    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  PIXELFORMATDESCRIPTOR pixel_format_descriptor = {
    sizeof(PIXELFORMATDESCRIPTOR)
  , 1
  , PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER
  , PFD_TYPE_RGBA
  , 32
  , 0
  , 0
  , 0
  , 0
  , 0
  , 0
  , 8
  , 0
  , 0
  , 0
  , 0
  , 0
  , 0
  , 24
  , 8
  , 0
  , PFD_MAIN_PLANE
  , 0
  , 0
  , 0
  , 0
  };

  WNDCLASSEX wnd_class_ex = {
    sizeof(WNDCLASSEX)
  , CS_HREDRAW | CS_VREDRAW | CS_OWNDC
  , WindowProc
  , 0
  , 0
  , 0
  , LoadIcon(nullptr, IDI_APPLICATION)
  , LoadCursor(nullptr, IDC_ARROW)
  , reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1)
  , nullptr
  , windows_class_name
  , nullptr
  };


  using ABGR = std::uint32_t;

  char const        sixel_base      = 63;

  // Hide cursor, clear screen
  //std::u8string const buffer__prelude    = u8"\x1B[2J\x1B[?25l";
  std::u8string const buffer__prelude    = u8"\x1B[?25l";
  // goto top, start sixel image
  std::u8string const sixel__prelude     = u8"\x1B[H\x1BP7;1;q";
  // Sixel image done
  std::u8string const sixel__epilogue    = u8"\x1B\\";

  struct ticks__timer {
    ticks__timer(ticks__timer const &)              = delete;
    ticks__timer(ticks__timer &&)                   = delete;
    ticks__timer& operator=(ticks__timer const &)   = delete;
    ticks__timer& operator=(ticks__timer &&)        = delete;

    inline explicit ticks__timer(DWORD64 * p) noexcept
      : p(p)
      , b(__rdtsc()) {
      assert(p);
    }

    inline ~ticks__timer() noexcept {
      assert(p);
      *p += __rdtsc()-b;
    }
  private:
    DWORD64 * p;
    DWORD64   b;
  };

  struct hires__timer {
    hires__timer(hires__timer const &)              = delete;
    hires__timer(hires__timer &&)                   = delete;
    hires__timer& operator=(hires__timer const &)   = delete;
    hires__timer& operator=(hires__timer &&)        = delete;

    inline explicit hires__timer(LONGLONG * p) noexcept
      : p(p) {
      assert(p);
      QueryPerformanceCounter(&b);
    }

    inline ~hires__timer() noexcept {
      assert(p);
      LARGE_INTEGER e;
      QueryPerformanceCounter(&e);
      *p += (e.QuadPart-b.QuadPart);
    }
  private:
    LONGLONG      * p ;
    LARGE_INTEGER b   ;
  };


  constexpr GLubyte to_rgb(char r, char g, char b) {
    // 3 bits for red
    // 3 bits for green
    // 2 bits for blue
    auto red    = (r & 0x7)<<5;
    auto green  = (g & 0x7)<<2;
    auto blue   = (b & 0x3);
    return static_cast<GLubyte>(blue|green|red);
  }

  std::array<std::u8string, 256> generate_col_selectors() {
    std::array<std::u8string,256> res;

    for (std::size_t i=0; i < res.size(); ++i) {
      res[i] = to_u8string(std::format(
        "#{}"
      , i));
    }

    return res;
  }
  std::array<std::u8string, 256> const sixel__col_selectors = generate_col_selectors();

  std::array<std::u8string, 2048> generate_reps() {
    std::array<std::u8string,2048> res;

    for (std::size_t i=0; i < res.size(); ++i) {
      res[i] = to_u8string(std::format(
        "!{}"
        , i));
    }

    return res;
  }
  std::array<std::u8string, 2048> const sixel__reps = generate_reps();

  std::u8string generate_palette() {
    // 3 bits for red
    // 3 bits for green
    // 2 bits for blue

    std::u8string palette;
    for (auto red = 0; red < 8; ++red) {
      for (auto green = 0; green < 8; ++green) {
        for (auto blue = 0; blue < 4; ++blue) {
          auto idx = (red << 5)|(green<<2)|blue;
          palette.append(to_u8string(std::format(
            "#{};2;{};{};{}"
          , idx
          , static_cast<int>(std::round(red  *100.0/7.0))
          , static_cast<int>(std::round(green*100.0/7.0))
          , static_cast<int>(std::round(blue *100.0/3.0))
          )));
        }
      }
    }
    return palette;
  }

  struct ticks__write_pixel_as_sixels {
    LONGLONG      total__hires    ;
    DWORD64       total           ;
    DWORD64       sixel_pixel     ;
    DWORD64       sixel_buffer    ;
    DWORD64       used_colors     ;
    DWORD64       buffer_append   ;
    DWORD64       buffer_append_n ;
    DWORD64       write_file      ;

    DWORD64       called__buffer_append   ;
    DWORD64       called__buffer_append_n ;
  };

  std::u8string const sixel__palette = generate_palette();

  inline void append(
      std::vector<char8_t>          & buffer
    , std::u8string const           & v
    , ticks__write_pixel_as_sixels  & ticks
    ) {
    ticks__timer time__append(&ticks.buffer_append);
    ++ticks.called__buffer_append;
    buffer.insert(buffer.end(), v.begin(), v.end());
  }

  inline void append_n(
      std::vector<char8_t> &          buffer
    , std::size_t                     n
    , char8_t                         v
    , ticks__write_pixel_as_sixels  & ticks
    ) {
    ticks__timer time__append(&ticks.buffer_append_n);
    ++ticks.called__buffer_append_n;
    buffer.insert(buffer.end(), n ,v);
  }

#ifdef USE_BACKGROUND_WRITER_THREAD
  struct background_writer {
    background_writer()
      : done    (false)
      , pbuffer (nullptr)
      , thread  ([this]{thread_proc();}){
    }

    ~background_writer() {
      done = true;
      cv.notify_one();
      thread.join();
    }

    background_writer(background_writer const &)  = delete;
    background_writer(background_writer &&)       = delete;

    background_writer& operator=(background_writer const &)  = delete;
    background_writer& operator=(background_writer &&)       = delete;

    void enqueue(std::vector<char8_t> & buffer) {
      std::unique_lock<std::mutex> lock(mtx);
      pbuffer = &buffer;
      cv.notify_one();
    }

  private:
    void thread_proc() {
      auto hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
      assert(hstdout != INVALID_HANDLE_VALUE);

      std::unique_lock<std::mutex> lock(mtx);

      while(!done) {
        cv.wait(lock, [this] { return pbuffer != nullptr||done; });

        auto pb = pbuffer;
        pbuffer = nullptr;
        if (pb) {
          auto & buffer = *pb;
          auto writeOk = WriteFile(
            hstdout
          , &buffer.front()
          , static_cast<DWORD>(buffer.size())
          , nullptr
          , nullptr
          );
          assert(writeOk);
//          auto flushOk = FlushFileBuffers(hstdout);
//          assert(flushOk);
        }
      }
    }

    bool                    done    ;
    std::vector<char8_t> *  pbuffer ;
    std::mutex              mtx     ;
    std::condition_variable cv      ;
    std::thread             thread  ;
  };
  background_writer bkg_writer;
  void write__output(
      HANDLE                        hstdout
    , std::vector<char8_t> &        output
    , ticks__write_pixel_as_sixels  & ticks
    ) {
    ticks__timer time__sixel_pixel(&ticks.write_file);
    bkg_writer.enqueue(output);
  }
#else
  void write__output(
      HANDLE                        hstdout
    , std::vector<char8_t> &        output
    , ticks__write_pixel_as_sixels  & ticks
    ) {
    if (output.size() > 0) {
      ticks__timer time__sixel_pixel(&ticks.write_file);
      auto writeOk = WriteFile(
        hstdout
      , &output.front()
      , static_cast<DWORD>(output.size())
      , nullptr
      , nullptr
      );
      assert(writeOk);
//      auto flushOk = FlushFileBuffers(hstdout);
//      assert(flushOk);
    }
  }
#endif

  void write_pixel_as_sixels(
      std::size_t                     width
    , std::size_t                     height
    , std::vector<ABGR> const       & pixels
    , std::vector<GLubyte>          & sixel_pixels
    , std::vector<char8_t>          & buffer
    , ticks__write_pixel_as_sixels  & ticks
    ) {
    hires__timer hires__total(&ticks.total__hires);
    ticks__timer time__total(&ticks.total);

    if (width > sixel__reps.size()) {
      return;
    }

    // We can only handle number of rows divisible by 6
    assert(height%6 == 0);

    auto total_size = width*height;
    assert(pixels.size() > 0);
    assert(pixels.size() == total_size);

    // +8 to handle any potential overruns from using AVX or MMX
    sixel_pixels.resize(total_size+8);

    {
      // Does a few things
      //  Groups 6 pixels under each other sequentially
      //  Converts to from 32 bit ABGR to 8 bit BGR
      auto ptr__output = &sixel_pixels.front();
      for (std::size_t y6 = height; y6 > 0; y6 -= 6) {
        auto y6_off = (y6-1)*width;
        for (std::size_t x = 0; x < width; ++x) {
          auto y_off = y6_off;
          for (std::size_t i = 0; i < 6; ++i) {
            auto from_i = x+y_off;
            auto abgr = pixels[from_i];
            // 3 bits for red
            // 3 bits for green
            // 2 bits for blue
            auto red          = (abgr           )&(0x7<<5);
            auto green        = (abgr >> (8+5-2))&(0x7<<2);
            auto blue         = (abgr >> (8+8+6))&(0x3   );
            auto sixel_pixel  = red|green|blue;
            *ptr__output      = static_cast<GLubyte>(sixel_pixel);

            ++ptr__output;
            y_off -= width;
          }
        }
      }
      assert(ptr__output == &sixel_pixels.front()+total_size);
    }

    buffer.clear();

    append(buffer, buffer__prelude, ticks);

    append(buffer, sixel__prelude, ticks);

    append(buffer, sixel__palette, ticks);

    {
      ticks__timer time__sixel_pixel(&ticks.sixel_buffer);
      bool used_colors[256];
      for (std::size_t y6 = 0; y6 < height; y6 += 6) {
        auto y6_off = y6*width;
        {
          ticks__timer time__used_colors(&ticks.used_colors);
          // Find colors used in this group of 6 lines
          memset(used_colors, 0, sizeof(used_colors));
          auto ptr__input = &sixel_pixels.front() + y6_off;
          for (std::size_t x = 0; x < 6*width; ++x) {
            auto sixel_pixel = *ptr__input;
            used_colors[sixel_pixel] = true;
            ++ptr__input;
          }
        }

#define USE_MMX
        {
          // Convert colors in use to sixels
          for (std::size_t current_col = 0; current_col < 256; ++current_col) {
            if (!used_colors[current_col]) [[likely]] {
              continue;
            }

#ifdef USE_MMX
            auto current_col6 = _mm_set1_pi8(static_cast<char>(current_col));
#endif

            append(buffer, sixel__col_selectors[current_col], ticks);

            auto repeated_sixel     = sixel_base;
            std::size_t sixel_reps  = 0;

            auto ptr__input = &sixel_pixels.front() + y6_off;
            for (std::size_t x = 0; x < width; ++x) {
              GLubyte sixel = 0;
#ifdef USE_MMX
              auto sixel_pixel6 = *reinterpret_cast<__m64 const *>(ptr__input);
              auto cmpeq6 = _mm_cmpeq_pi8(sixel_pixel6, current_col6);
              sixel = (_mm_movemask_pi8(cmpeq6))&0x3F;
              ptr__input += 6;
#else
              for (std::size_t i = 0; i < 6; ++i) {
                auto sixel_pixel = *ptr__input;
                if (current_col == sixel_pixel) {
                  sixel |= 1U << i;
                }
                ++ptr__input;
              }
#endif
              char sixel_char = sixel_base + sixel;

              // Handle run-length encoding
              if (repeated_sixel == sixel_char) [[likely]] {
                ++sixel_reps;
              } else {
                // Output previous run
                if (sixel_reps > 3) [[unlikely]] {
                  // Use RLE for runs longer than 3

                  append(buffer, sixel__reps[sixel_reps], ticks);
                  buffer.push_back(repeated_sixel);
                } else {
                  // Direct output for short runs
                  append_n(buffer, sixel_reps, repeated_sixel, ticks);
                }

                repeated_sixel  = sixel_char;
                sixel_reps      = 1;
              }

            }

            // Output final run if not empty
            if (repeated_sixel != sixel_base) [[unlikely]] {
              if (sixel_reps > 3) [[unlikely]] {
                // Use RLE for runs longer than 3

                append(buffer, sixel__reps[sixel_reps], ticks);
                buffer.push_back(repeated_sixel);
              } else {
                // Direct output for short runs
                append_n(buffer, sixel_reps, repeated_sixel, ticks);
              }
            }

            buffer.push_back('$');  // Return to start of line
          }
          buffer.push_back('-');  // Move to next row
        }

      }
    }

#ifdef USE_MMX
    _mm_empty();
#endif

    append(buffer, sixel__epilogue, ticks);
  }

  void write__sixel_screen(
      std::vector<char8_t>          & output
    , std::vector<ABGR>             & pixels
    , std::vector<GLubyte>          & sixel_pixels
    , ticks__write_pixel_as_sixels  & ticks
    ) {
    glReadBuffer(GL_FRONT);

    if (viewport__width > 0 && viewport__height > 0) {
      // Make sure the number of rows in the buffer is divisible by 6
      std::size_t buffer_height  = ((viewport__height+5)/6)*6;
      assert(buffer_height%6 == 0);
      auto total_size = viewport__width*buffer_height;
      pixels.resize(total_size);

      auto ptr__pixels = &pixels.front();

      glReadPixels(
          0
        , 0
        , static_cast<GLsizei>(viewport__width )
        , static_cast<GLsizei>(viewport__height)
        , GL_RGBA
        , GL_UNSIGNED_BYTE
        , ptr__pixels
        );

      write_pixel_as_sixels(
          viewport__width
        , buffer_height
        , pixels
        , sixel_pixels
        , output
        , ticks
        );
    }
  }

  bool terminal_supports_sixel() {
    // Clear any buffered input
    while (_kbhit()) {
      _getch();
    }
    // Ask the terminal for its capabilities
    std::printf("\x1B[c");

    // Wait for the terminal to respond
    Sleep(100);

    std::string input;
    input.reserve(128);

    while (_kbhit()) {
      auto ch = static_cast<char>(_getch());
      input.push_back(ch);
    }

    // 4 is magic constant for Sixel support
    std::string sixel_flag = "4";
    std::string flag;
    flag.reserve(128);

    auto check_flag = [&flag, &sixel_flag]() {
      return flag == sixel_flag;
    };

    // Read response, it's flags that are semi-colon 
    //  separated so run check_flag after each flag is read
    for (auto ch : input) {
      if (ch == ';') {
        if (check_flag()) {
          return true;
        }
        flag.clear();
      } else {
        flag.push_back(ch);
      }
    }

    if (check_flag()) {
      return true;
    }

    return false;
  }
}

int main() {
  try {
    if (!terminal_supports_sixel()) {
      std::printf("This demo requires a Sixel (tech from 1970s) compatible terminal such as :\n * Windows Terminal Preview version 1.22.3232.0\n");
      return 97;
    }

    {
      // Create effective script
      std::size_t idx = 0;
      auto current    = get__script_part(idx);
      auto next       = get__script_part(idx+1);
      for (std::size_t i = 0; i < effective_script.size(); ++i) {
        if (i >= next.beat__start) {
          ++idx;
          current = get__script_part(idx);
          next    = get__script_part(idx + 1);
        }
        effective_script[i] = effective_script_part {
          current.beat__start
        , next.beat__start
        , current.effect
        , current.name
        };
      }
    }

    CHECK_CONDITION(SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE));

    CHECK_HRESULT(CoInitialize(0));
    auto onexit__counitialize = on_exit([]{ CoUninitialize(); });

    CHECK_HRESULT(MFStartup(MF_VERSION));
    auto onexit__mfshutdown = on_exit([]{ MFShutdown(); });

    auto hinstance = GetModuleHandle(0);
    CHECK_CONDITION(hinstance);

    wnd_class_ex.hInstance = hinstance;

    auto dw_style   = WS_OVERLAPPEDWINDOW | WS_POPUP;
    auto dw_exstyle = WS_EX_NOACTIVATE ;
#ifdef SHOW_WINDOW
    dw_style |= WS_VISIBLE;
#endif

    auto result__register_class = RegisterClassEx(&wnd_class_ex);
    CHECK_CONDITION(result__register_class);

    auto on_exit__unregister_class = on_exit([hinstance]{ UnregisterClass(windows_class_name, hinstance); });

    RECT window_rect = { 0, 0, desired__width, desired__height };
    auto result__rect= AdjustWindowRect(&window_rect, dw_style, 0);
    CHECK_CONDITION(result__rect);

    auto wnd__width   = window_rect.right-window_rect.left;
    auto wnd__height  = window_rect.bottom-window_rect.top;
    auto wnd__x       = GetSystemMetrics(SM_CXSCREEN) - wnd__width;
    auto wnd__y       = 0;

    auto hwnd = CreateWindowEx(
      dw_exstyle          // Extended style
    , windows_class_name  // Window class name
    , windows_class_name  // Window title
    , dw_style            // Window style
    , wnd__x              // StartPosition X
    , wnd__y              // StartPosition Y
    , wnd__width          // Width
    , wnd__height         // Height
    , nullptr             // Parent
    , nullptr             // Menu
    , hinstance           // Instance
    , nullptr             // additional params
    );
    CHECK_CONDITION(hwnd);
    auto on_exit__destroy_window = on_exit([hwnd]{ DestroyWindow(hwnd); });

    // Intentionally ignore return value
#ifdef SHOW_WINDOW
    ShowWindow(hwnd, SW_SHOWNORMAL);
#else
    ShowWindow(hwnd, SW_HIDE);
#endif
    auto result__update_window = UpdateWindow(hwnd);
    assert(result__update_window);

    auto hdc = GetDC(hwnd);
    CHECK_CONDITION(hdc);
    auto on_exit__release_dc = on_exit([hwnd, hdc]{ ReleaseDC(hwnd, hdc); });

    auto pixel_format = ChoosePixelFormat(hdc, &pixel_format_descriptor);
    CHECK_CONDITION(pixel_format);

    CHECK_CONDITION(SetPixelFormat(hdc, pixel_format, &pixel_format_descriptor));

    auto hrc = wglCreateContext(hdc);
    CHECK_CONDITION(hrc);
    auto on_exit__delete_context = on_exit([hrc]{ wglDeleteContext(hrc); });

    auto result__make_current = wglMakeCurrent(hdc, hrc);
    CHECK_CONDITION(result__make_current);
    auto on_exit__make_current = on_exit([]{ wglMakeCurrent(nullptr, nullptr); });

    IMFPMediaPlayer * player = nullptr;
    CHECK_HRESULT(MFPCreateMediaPlayer(
      LR"PATH(D:\assets\astroboy--my-head-is-spiritualism.mp3)PATH"
    , FALSE
    , 0
    , nullptr
    , hwnd
    , &player
    ));
    assert(player);
    auto onexit__release_player = on_exit([player]{ player->Release(); });

    auto hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    CHECK_CONDITION(hstdout != INVALID_HANDLE_VALUE);

    CHECK_CONDITION(SetConsoleOutputCP(CP_UTF8));

    DWORD consoleMode;
    CHECK_CONDITION(GetConsoleMode(hstdout, &consoleMode));

    //consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    // No flickering with just ENABLE_PROCESSED_OUTPUT?
    consoleMode = ENABLE_PROCESSED_OUTPUT;

    CHECK_CONDITION(SetConsoleMode(hstdout, consoleMode));

    init__effect8();
    auto on_exit__deinit__effect8 = on_exit([]{ deinit__effect8(); });


    std::vector<ABGR>     pixels          ;
    std::vector<GLubyte>  sixel_pixels    ;
    std::vector<char8_t>  output0         ;
    std::vector<char8_t>  output1         ;
    auto buffer_selector                  = false;

    output0.reserve(1<<20);
    output1.reserve(1<<20);

#ifdef INFO_TEXT
    {
      ticks__write_pixel_as_sixels ticks = {};
      wchars_to_utf8(output1, info_text);
      write__output(
        hstdout
      , output1
      , ticks
      );
      std::getchar();
    }
#endif

    CHECK_HRESULT(player->Play());
    auto onexit__stop_player = on_exit([player]{ player->Stop(); });

    screen screen = make_screen(screen__width, screen__height);

#ifdef MUSIC_TIME
    {
      PROPVARIANT position_value;
      PropVariantInit(&position_value);
      position_value.vt = VT_I8;
      position_value.hVal.QuadPart = static_cast<LONGLONG>(start_time*1E7F);
      CHECK_HRESULT(player->SetPosition(MFP_POSITIONTYPE_100NS, &position_value));
    }
#endif

    auto done = false;
    auto msg = MSG {};

    while(!done) {
      while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) done = true;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }

      ticks__write_pixel_as_sixels ticks = {};

#ifdef MUSIC_TIME
      float time = 0;
      PROPVARIANT position_value;
      if (SUCCEEDED(player->GetPosition(MFP_POSITIONTYPE_100NS, &position_value))) {
        if (position_value.vt == VT_I8) {
          time = position_value.hVal.QuadPart/1E7F;
        }
      };
#else
      auto now  = GetTickCount64();
      auto time = (now - before) / 1000.0f;
#endif

      auto nbeat = music__nbeat(time);
      done |= nbeat >= static_cast<int>(effective_script.size());
      nbeat = std::clamp<std::size_t>(nbeat, 0, effective_script.size()-1);

      screen.clear();

      auto & part = effective_script[nbeat];

      auto ei = effect_input {
          time
        , part.beat__start
        , part.beat__end
        , screen
        , viewport__width
        , viewport__height
      };

      auto kind = part.effect(ei);

      auto result__swap_buffers = SwapBuffers(hdc);
      assert(result__swap_buffers);

      buffer_selector = !buffer_selector;
      std::vector<char8_t> & output =
        buffer_selector
        ? output0
        : output1
        ;

      output.clear();

      switch(kind) {
      case ascii_effect:
        write__screen(
            output
          , screen
          );
        break;
      case sixel_effect:
        write__sixel_screen(
            output
          , pixels
          , sixel_pixels
          , ticks
          );
        break;
      }

      write__footer(
          output
        , time
        , part
        );

      write__output(
        hstdout
      , output
      , ticks
      );

    }

    {
      buffer_selector = !buffer_selector;
      std::vector<char8_t> & output =
        buffer_selector
        ? output0
        : output1
        ;

      output.clear();

      ticks__write_pixel_as_sixels ticks = {};
      wchars_to_utf8(output, end_text);

      auto sixel__handle = CreateFileA(
        R"PATH(D:\assets\gerp.txt)PATH"
      , GENERIC_READ
      , FILE_SHARE_READ
      , nullptr
      , OPEN_EXISTING
      , FILE_ATTRIBUTE_NORMAL
      , nullptr
      );
      CHECK_CONDITION(sixel__handle != INVALID_HANDLE_VALUE);
      auto onexit__close_sixel = on_exit([sixel__handle]{ CloseHandle(sixel__handle); });

      LARGE_INTEGER sixel__size;
      CHECK_CONDITION(GetFileSizeEx(sixel__handle, &sixel__size));

      if (sixel__size.QuadPart > 0) {
        std::vector<char8_t> buffer;
        auto size = static_cast<std::size_t>(sixel__size.QuadPart);
        buffer.resize(size);
        DWORD sixel__read = 0;
        CHECK_CONDITION(ReadFile(
            sixel__handle
          , &buffer.front()
          , size
          , &sixel__read
          , nullptr
          ));

        output.insert(output.end(), buffer.begin(), buffer.begin() + sixel__read);
      }

      write__output(
        hstdout
      , output
      , ticks
      );
    }

    return 0;
  } catch (std::runtime_error const & e) {
    std::printf("An error occured: %s", e.what());
    return 98;
  } catch (...) {
    std::printf("An error occured: Unknown");
    return 99;
  }
}
