#include "precompiled.hpp"

#include "effect.hpp"

namespace {
  float const time__step            = 0.175F;
  float       const fade__out       = std::logf(20);

  bitmap const qc__impulse_0 = make_bitmap(col__white, LR"BITMAP(
  ╔╗╔╧╗╔╩╗╔═╩╗╔╗╔╗╔╗ ╔╧═╗╔═╩╗╔╗
  ╠╣║:╚╝:║║╔╗║║║║║║║ ║╔═╝║╔═╝║║
  ╢║║:**:║║╚╝║║║║║║║ ║╚═╗║╚═╗║║
  ║║║╔╗╔╗║║╔╤╝║║║║║║ ╚═╗║║╔═╝║╠
  ╢║║║╚╝║║║║  ║╚╝║║╚╗╔═╝║║╚═╗╠╣
  ╚╝╚╝  ╚╝╚╝  ╚╤═╝╚╦╝╚═╦╝╚╤═╝╚╝
)BITMAP");
  bitmap const qc__impulse_1 = make_bitmap(col__white, LR"BITMAP(
  ┌╮┌┴╮╭╨┐┌─╨╮┌╮╭┐┌╮ ╭──┐╭─╨┐╭┐
  ├┤│:╰╯:││╭╮│││││││ │╭─╯│╭─╯││
  ┤││:**:││└╯│││││││ │└─╮│└─╮││
  │││╭╮╭╮││╭┬╯│││││├╯╰─╮││╭─╯│╞
  ┤│││└╯││││  │└╯││└╮╭─┘││└─╮├┤
  └╯└╯  ╰┘└╯  └┬─╯└╥╯└─╥╯╰┬─┘╰┘
)BITMAP");


  struct cell {
    wchar_t shape               ;
    char    connection__single  ;
    char    connection__double  ;
    int     priority            ;
  };

  cell cells[] {
    { L' ', 0b0000, 0b0000, 10 }
  , { L'─', 0b1010, 0b0000, 10 }
  , { L'│', 0b0101, 0b0000, 10 }
  , { L'┌', 0b0011, 0b0000, 10 }
  , { L'┐', 0b1001, 0b0000, 10 }
  , { L'└', 0b0110, 0b0000, 10 }
  , { L'┘', 0b1100, 0b0000, 10 }
  , { L'├', 0b0111, 0b0000, 10 }
  , { L'┤', 0b1101, 0b0000, 10 }
  , { L'┬', 0b1011, 0b0000, 10 }
  , { L'┴', 0b1110, 0b0000, 10 }
  , { L'┼', 0b1111, 0b0000, 10 }

  , { L'╭', 0b0011, 0b0000, 8  }
  , { L'╯', 0b1100, 0b0000, 8  }
  , { L'╮', 0b1001, 0b0000, 8  }
  , { L'╰', 0b0110, 0b0000, 8  }


  , { L'═', 0b0000, 0b1010, 10 }
  , { L'║', 0b0000, 0b0101, 10 }
  , { L'╔', 0b0000, 0b0011, 10 }
  , { L'╗', 0b0000, 0b1001, 10 }
  , { L'╚', 0b0000, 0b0110, 10 }
  , { L'╝', 0b0000, 0b1100, 10 }
  , { L'╠', 0b0000, 0b0111, 10 }
  , { L'╣', 0b0000, 0b1101, 10 }
  , { L'╦', 0b0000, 0b1011, 10 }
  , { L'╩', 0b0000, 0b1110, 10 }
  , { L'╬', 0b0000, 0b1111, 10 }

  , { L'╒', 0b0001, 0b0010, 5  }
  , { L'╓', 0b0010, 0b0001, 5  }
  , { L'╕', 0b0001, 0b1000, 5  }
  , { L'╖', 0b1000, 0b0001, 5  }
  , { L'╘', 0b0100, 0b0010, 5  }
  , { L'╙', 0b0010, 0b0100, 5  }
  , { L'╛', 0b0100, 0b1000, 5  }
  , { L'╜', 0b1000, 0b0100, 5  }
  , { L'╞', 0b0101, 0b0010, 5  }
  , { L'╟', 0b0010, 0b0101, 5  }
  , { L'╡', 0b0101, 0b1000, 5  }
  , { L'╢', 0b1000, 0b0101, 5  }
  , { L'╤', 0b0001, 0b1010, 5  }
  , { L'╥', 0b1010, 0b0001, 5  }
  , { L'╧', 0b0100, 0b1010, 5  }
  , { L'╨', 0b1010, 0b0100, 5  }
  , { L'╪', 0b0101, 0b1010, 5  }
  , { L'╫', 0b1010, 0b0101, 5  }

  , { L'╴', 0b1000, 0b0000, 1  }
  , { L'╵', 0b0100, 0b0000, 1  }
  , { L'╶', 0b0010, 0b0000, 1  }
  , { L'╷', 0b0001, 0b0000, 1  }

  };

  std::map<wchar_t, cell> create__lookup__cell() {
    std::map<wchar_t, cell> res;

    for (auto & c : cells) {
      auto [_, success] = res.insert(std::make_pair(c.shape, c));
      assert(success);
    }

    return res;
  }
  std::map<wchar_t, cell> lookup__cell = create__lookup__cell();

  enum connection {
    undecided
  , free
  , no_connection
  , connected_to_single
  , connected_to_double
  };

  enum qc__type {
    qc__null  = 0
  , qc__grid  = 1
  , qc__logo  = 2
  , qc__flash = 3
  , qc__fade  = 4
  };

  struct qc {
    std::size_t freedom     ;
    wchar_t     shape       ;
    std::size_t x           ;
    std::size_t y           ;
    qc__type    type        ;

    float       type__begin ;
    float       type__end   ;

    qc()
      : freedom     (0)
      , shape       (0)
      , x           (0)
      , y           (0)
      , type        (qc__null)
      , type__begin (end__time)
      , type__end   (end__time)
    {
    }
  };

  using qcs = std::array<qc, screen__width*screen__height>;

  void reduce__freedom(
      qcs &       res
    , qc const &  sel
    , int         delta__x
    , int         delta__y
    ) {
    int x = sel.x + delta__x;
    int y = sel.y + delta__y;

    if (x < 0 || x >= screen__width) {
      return;
    }

    if (y < 0 || y >= screen__height) {
      return;
    }

    auto & update = res[x+y*screen__width];

    if (update.freedom > 0) {
      --update.freedom;
    }
  }

  connection determine__connection(
      qcs const & res
    , qc const &  sel
    , char        test
    , int         delta__x
    , int         delta__y
    ) {
    int x = sel.x + delta__x;
    int y = sel.y + delta__y;

    if (x < 0 || x >= screen__width) {
      return free;
    }

    if (y < 0 || y >= screen__height) {
      return free;
    }

    auto neighbour  = res[x+y*screen__width];
    auto shape      = neighbour.shape;
    if (shape == 0) {
      return free;
    } else {
      auto f = lookup__cell.find(shape);
      if (f == lookup__cell.end()) {
        return free;
      }
      auto connection__single = f->second.connection__single;
      auto connection__double = f->second.connection__double;
      assert((connection__single&connection__double) == 0);

      auto has__single = (test & connection__single) != 0;
      auto has__double = (test & connection__double) != 0;
      assert(!(has__single&&has__double));

      if (has__single) {
        return connected_to_single;
      } else if (has__double) {
        return connected_to_double;
      } else {
        return no_connection;
      }
    }
  }

  std::size_t get__freedom(
      qcs const & res
    , qc const &  sel
    , int         delta__x
    , int         delta__y
    ) {
    int x = sel.x + delta__x;
    int y = sel.y + delta__y;

    if (x < 0 || x >= screen__width) {
      return 1;
    }

    if (y < 0 || y >= screen__height) {
      return 1;
    }

    auto neighbour  = res[x+y*screen__width];

    return neighbour.shape == 0 ? 1 : 0;

  }


  bool check__candidate(
    cell const &  c
  , connection    conn
  , char          test
  )
  {
    assert((c.connection__single&c.connection__double) == 0);
    switch(conn) {
    case free               :
      return true;
    case no_connection      :
      return (test & c.connection__single) == 0 && (test & c.connection__double) == 0;
    case connected_to_single:
      return (test & c.connection__single) != 0;
    case connected_to_double:
      return (test & c.connection__double) != 0;
    default:
      assert(false);
      return false;
    }
  }

  qcs create__board() {
    qcs res;

    for (std::size_t y = 0; y < screen__height; ++y) {
      auto y__off = y*screen__width;
      for (std::size_t x = 0; x < screen__width; ++x) {
        auto & qc = res[y__off+x];
        qc.type = qc__grid;
        qc.x    = x;
        qc.y    = y;
      }
    }

    auto & logo = qc__impulse_1;

    for (std::size_t from__y = 0; from__y < logo.height; ++from__y) {
      auto to__y        = from__y + 12;
      auto from__y__off = from__y*logo.width;
      auto to__y__off   = to__y*screen__width;
      for (std::size_t from__x = 0; from__x < logo.width; ++from__x) {
        auto to__x  = from__x + 24;
        auto shape  = logo.shapes[from__y__off+from__x];
        auto & qc   = res[to__y__off+to__x];
        if (shape > 32) {
          qc.type     = qc__logo;
          qc.shape    = shape;
          qc.freedom  = 0;
        }
      }
    }

    for (std::size_t y = 0; y < screen__height; ++y) {
      auto y__off = y*screen__width;
      for (std::size_t x = 0; x < screen__width; ++x) {
        auto & qc = res[y__off+x];
        if (qc.shape == 0) {
          std::size_t freedom = 0;
          freedom += get__freedom(res, qc, -1, 0);
          freedom += get__freedom(res, qc,  1, 0);
          freedom += get__freedom(res, qc,  0,-1);
          freedom += get__freedom(res, qc,  0, 1);
          qc.freedom = freedom;
        } else {
          qc.freedom = 0;
        }
      }
    }

    std::vector<qc>   candidates__qc;
    std::vector<cell> candidates__cell;
    candidates__qc.reserve(res.size());
    candidates__cell.reserve(128);

    while(true) {
      std::size_t min   = 1000;
      std::size_t open  = 0   ;
      for (std::size_t i = 0; i < res.size(); ++i) {
        auto & qc = res[i];
        if (qc.shape == 0) {
          min = std::min(min, qc.freedom);
          ++open;
        }
      }

      if (open == 0) {
        break;
      }

      candidates__qc.clear();
      for (std::size_t i = 0; i < res.size(); ++i) {
        auto & qc = res[i];
        if (qc.freedom == min && qc.shape == 0) {
          candidates__qc.push_back(qc);
        }
      }

      assert(candidates__qc.size() > 0);
      auto sel = candidates__qc[pick_a_number(0, candidates__qc.size()-1)];
      assert(sel.shape == 0);

      auto  left    = undecided;
      auto  top     = undecided;
      auto  right   = undecided;
      auto  bottom  = undecided;

      left    = determine__connection(res, sel, 0b0010, -1, 0);
      right   = determine__connection(res, sel, 0b1000, 1, 0);
      top     = determine__connection(res, sel, 0b0001, 0, -1);
      bottom  = determine__connection(res, sel, 0b0100, 0, 1);

      assert(left   != undecided);
      assert(top    != undecided);
      assert(right  != undecided);
      assert(bottom != undecided);

      candidates__cell.clear();
      for (auto & c : cells) {
        auto candidate = true;

        candidate &= check__candidate(c, left   , 0b1000);
        candidate &= check__candidate(c, top    , 0b0100);
        candidate &= check__candidate(c, right  , 0b0010);
        candidate &= check__candidate(c, bottom , 0b0001);

        if (candidate) {
          auto copy = c;
          copy.priority += pick_a_number(0, 10);
          candidates__cell.push_back(copy);
        }
      }

      auto & update = res[sel.x+sel.y*screen__width];

      // assert(candidates__cell.size() > 0);
      if (candidates__cell.size() == 0) {
        update.shape   = L'*';
        update.freedom = 0;
      } else {
        std::sort(
            candidates__cell.begin()
          , candidates__cell.end()
          , [](auto & l, auto & r) { return l.priority > r.priority; }
          );
        auto c = candidates__cell.front();
        update.shape = c.shape;
        update.freedom = 0;
      }

      reduce__freedom(res, sel,-1, 0);
      reduce__freedom(res, sel, 1, 0);
      reduce__freedom(res, sel, 0,-1);
      reduce__freedom(res, sel, 0, 1);
    }

    return res;
  }

  qcs board = create__board();

  void flash(
      float time
    , int   x
    , int   y
    , bool  allow__fade
    , float off
    )
    {
    if (x < 0 || x > screen__width - 1) {
      return;
    }

    if (y < 0 || y > screen__height - 1) {
      return;
    }

    auto & sel = board[x+y*screen__width];

    switch (sel.type) {
    case qc__fade:
      if (!allow__fade) {
        break;
      }
      // Fall through intended
    case qc__grid:
      sel.type        = qc__flash;
      sel.type__begin = time;
      sel.type__end   = time+off+time__step;
      break;
    default:
      break;
    }
  }
}


effect_kind effect3(effect_input const & ei) {

/*
┌─┬─┐
│ │ │
├─┼─┤
│ │ │
└─┴─┘

╔╗╔═╗╔═╗╔══╗╔╗╔╗╔╗ ╔══╗╔══╗╔╗
╠╣║ ╚╝ ║║╔╗║║║║║║║ ║╔═╝║╔═╝║║
║║║ ** ║║╚╝║║║║║║║ ║╚═╗║╚═╗║║
║║║╔╗╔╗║║╔═╝║║║║║║ ╚═╗║║╔═╝║║
║║║║╚╝║║║║  ║╚╝║║╚╗╔═╝║║╚═╗╠╣
╚╝╚╝  ╚╝╚╝  ╚══╝╚═╝╚══╝╚══╝╚╝
╭╮╭─╮╭─╮╭──╮╭╮╭╮╭╮ ╭──╮╭──╮╭╮
├┤│ ╰╯ ││╭╮│││││││ │╭─╯│╭─╯││
│││ ** ││╰╯│││││││ │╰─╮│╰─╮││
│││╭╮╭╮││╭─╯││││││ ╰─╮││╭─╯││
││││╰╯││││  │╰╯││╰╮╭─╯││╰─╮├┤
╰╯╰╯  ╰╯╰╯  ╰──╯╰─╯╰──╯╰──╯╰╯

╔╗╔╧╗╔╩╗╔═╩╗╔╗╔╗╔╗ ╔╧═╗╔═╩╗╔╗
╠╣║ ╚╝ ║║╔╗║║║║║║║ ║╔═╝║╔═╝║║
╢║║ ** ║║╚╝║║║║║║║ ║╚═╗║╚═╗║║
║║║╔╗╔╗║║╔╤╝║║║║║║ ╚═╗║║╔═╝║╠
╢║║║╚╝║║║║  ║╚╝║║╚╗╔═╝║║╚═╗╠╣
╚╝╚╝  ╚╝╚╝  ╚╤═╝╚╦╝╚═╦╝╚╤═╝╚╝
┌╮┌┴╮╭╨┐┌─╨╮┌╮╭┐┌╮ ╭──┐╭─╨┐╭┐
├┤│ ╰╯ ││╭╮│││││││ │╭─╯│╭─╯││
┤││ ** ││└╯│││││││ │└─╮│└─╮││
│││╭╮╭╮││╭┬╯││││││ ╰─╮││╭─╯│╞
┤│││└╯││││  │└╯││└╮╭─┘││└─╮├┤
└╯└╯  ╰┘└╯  └┬─╯└╥╯└─╥╯╰┬─┘╰┘

Single lines:     ─ │ ┌ ┐ └ ┘ ├ ┤ ┬ ┴ ┼
Double lines:     ═ ║ ╔ ╗ ╚ ╝ ╠ ╣ ╦ ╩ ╬
Mixed doubles:    ╒ ╓ ╕ ╖ ╘ ╙ ╛ ╜ ╞ ╟ ╡ ╢ ╤ ╥ ╧ ╨ ╪ ╫
Half elements:    ╴ ╵ ╶ ╷
Bend elements:    ╭ ╯ ╮ ╰
*/
  auto time = ei.time;

  std::array<qc, 4> candidates__cell  ;

  std::size_t flashes = 0;
  for (std::size_t i = 0; i < board.size(); ++i) {
    auto & sel = board[i];
    switch(sel.type) {
    case qc__flash:
      if (sel.type__end < time) {
        std::size_t candidates__no  = 0;
        auto        has__logo       = false;
        auto collect = [time, &has__logo, &sel, &candidates__cell, &candidates__no](int delta__x, int delta__y) {
          int x = sel.x + delta__x;
          int y = sel.y + delta__y;

          if (x < 0 || x >= screen__width) {
            return;
          }

          if (y < 0 || y >= screen__height) {
            return;
          }

          auto & c = board[x + y*screen__width];
          switch(c.type) {
          case qc__grid:
            break;
          case qc__fade:
            if (c.type__begin + time__step*2 < time) {
              break;
            } else {
              return;
            }
          case qc__logo:
            has__logo = true;
            return;
          case qc__flash:
          case qc__null:
          default:
            return;
          }

          candidates__cell[candidates__no] = c;
          ++candidates__no;
        };

        auto f = lookup__cell.find(sel.shape);
        if (f != lookup__cell.end()) {
          auto connections = f->second.connection__single|f->second.connection__double;

          if (0b1000 & connections) collect(-1, 0);
          if (0b0010 & connections) collect( 1, 0);
          if (0b0100 & connections) collect( 0, -1);
          if (0b0001 & connections) collect( 0,  1);

          assert(candidates__no <= 4);
          if (!has__logo && candidates__no > 0) {
            auto & c = candidates__cell[pick_a_number(0, candidates__no-1)];
            flash(
                time
              , c.x
              , c.y
              , true
              , 0
              );
          }

        }

        sel.type          = qc__fade;
        sel.type__begin   = time;
        sel.type__end     = time+time__step*16;
      }

      ++flashes;
      break;
    case qc__fade:
      if (sel.type__end < time) {
        sel.type          = qc__grid;
        sel.type__begin   = time;
        sel.type__end     = end__time;
      }
      break;
    default:
      break;
    }
  }

  for (std::size_t i = flashes; i < 20; ++i) {
    auto off = time__step*hash(i+time+123.4F);
    flash(
        time
      , pick_a_number(0, screen__width-1)
      , pick_a_number(0, screen__height-1)
      , false
      , off
      );
  }


  for (std::size_t y = 0; y < ei.screen.height; ++y) {
    auto py = (-1.F*ei.screen.height+2.F*(y+0.5F))/ei.screen.height;
    auto y__off = y*screen__width;
    for (std::size_t x = 0; x < ei.screen.width; ++x) {
      auto px = (-1.F*ei.screen.width+2.F*(x+0.5F))/(2*ei.screen.height);
      auto & qc = board[x+y__off];
      auto col = col__rainbow(time, x, y);
      float grid__fade = mix(0.25F, 0.00, tanh_approxf(length2f(px,py)));
      switch (qc.type) {
      case qc__grid:
        col *= grid__fade;
        break;
      case qc__logo:
        col = col.sqrt();
        break;
      case qc__flash:
        col = vec3 {1,1,1};
        break;
      case qc__fade: {
        auto fade = std::expf(-fade__out*(time-qc.type__begin)/(qc.type__end-qc.type__begin));
        col += 0.5F*fade*fade;
        col *= mix(grid__fade, 1, fade);
        }
        break;
      default:
        break;
      }
      ei.screen.draw__pixel(
          qc.shape
        , col
        , vec3 {0,0,0}
        , x
        , y
        );
    }
  }

  {
    auto luma = vec3 { 0.299F, 0.587F, 0.114F };
    float fadein   = smoothstep(music__from_nbeat(ei.beat__start), music__from_nbeat(ei.beat__start+8), time);
    float fadeout  = smoothstep(music__from_nbeat(ei.beat__end-32), music__from_nbeat(ei.beat__end), time);
    ei.screen.apply_to_all([luma, fadein ,fadeout](auto x, auto y, auto p, auto& s, auto& f, auto& b) {
      float flum = luma.dot(f);
      float blum = luma.dot(b);
      f *= fadein;
      b *= fadein;
      f *= smoothstep(flum, flum-0.25F, fadeout);
      b *= smoothstep(blum, blum-0.25F, fadeout);
    });
  }

  return ascii_effect;
}
