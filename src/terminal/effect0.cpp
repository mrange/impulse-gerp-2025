#include "precompiled.hpp"

#include "effect.hpp"

namespace {

  bitmap const fitb = make_bitmap(col__magenta, LR"BITMAP(
FFFFFFFFFFFFFFFFFFFFFFIIIIIIIIIITTTTTTTTTTTTTTTTTTTTTTTBBBBBBBBBBBBBBBBB
F::::::::::::::::::::FI::::::::IT:::::::::::::::::::::TB::::::::::::::::B
F::::::::::::::::::::FI::::::::IT:::::::::::::::::::::TB::::::BBBBBB:::::B
FF::::::FFFFFFFFF::::FII::::::IIT:::::TT:::::::TT:::::TBB:::::B     B:::::B
  F:::::F       FFFFFF  I::::I  TTTTTT  T:::::T  TTTTTT  B::::B     B:::::B
  F:::::F               I::::I          T:::::T          B::::B     B:::::B
  F::::::FFFFFFFFFF     I::::I          T:::::T          B::::BBBBBB:::::B
  F:::::::::::::::F     I::::I          T:::::T          B:::::::::::::BB
  F:::::::::::::::F     I::::I          T:::::T          B::::BBBBBB:::::B
  F::::::FFFFFFFFFF     I::::I          T:::::T          B::::B     B:::::B
  F:::::F               I::::I          T:::::T          B::::B     B:::::B
  F:::::F               I::::I          T:::::T          B::::B     B:::::B
FF:::::::FF           II::::::II      TT:::::::TT      BB:::::BBBBBB::::::B
F::::::::FF           I::::::::I      T:::::::::T      B:::::::::::::::::B
F::::::::FF           I::::::::I      T:::::::::T      B::::::::::::::::B
FFFFFFFFFFF           IIIIIIIIII      TTTTTTTTTTT      BBBBBBBBBBBBBBBBB
)BITMAP");
}

effect_kind effect0(effect_input const & ei) {
  ei.screen.draw__bitmap(fitb, ei.time, 2, 7);
  return ascii_effect;
}
