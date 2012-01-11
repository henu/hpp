#ifndef HPP_KEY_H
#define HPP_KEY_H

#ifndef HPP_NO_SDL
#include <SDL/SDL.h>
#endif

namespace Hpp
{

namespace Key
{

#ifndef HPP_NO_SDL
enum Keycode { BACKSPACE = SDLK_BACKSPACE,
               TAB = SDLK_TAB,
               RETURN = SDLK_RETURN,
               ESCAPE = SDLK_ESCAPE,
               SPACE = SDLK_SPACE,
               A = SDLK_a,
               B = SDLK_b,
               C = SDLK_c,
               D = SDLK_d,
               E = SDLK_e,
               F = SDLK_f,
               G = SDLK_g,
               H = SDLK_h,
               I = SDLK_i,
               J = SDLK_j,
               K = SDLK_k,
               L = SDLK_l,
               M = SDLK_m,
               N = SDLK_n,
               O = SDLK_o,
               P = SDLK_p,
               Q = SDLK_q,
               R = SDLK_r,
               S = SDLK_s,
               T = SDLK_t,
               U = SDLK_u,
               V = SDLK_v,
               W = SDLK_w,
               X = SDLK_x,
               Y = SDLK_y,
               Z = SDLK_z,
               DEL = SDLK_DELETE,
               UP = SDLK_UP,
               DOWN = SDLK_DOWN,
               RIGHT = SDLK_RIGHT,
               LEFT = SDLK_LEFT,
               INSERT = SDLK_INSERT,
               HOME = SDLK_HOME,
               END = SDLK_END,
               PAGE_UP = SDLK_PAGEUP,
               PAGE_DOWN = SDLK_PAGEDOWN,
               LEFT_CONTROL = SDLK_LCTRL,
               LEFT_ALT = SDLK_LALT,
               LEFT_SHIFT = SDLK_LSHIFT,
               RIGHT_CONTROL = SDLK_RCTRL,
               RIGHT_SHIFT = SDLK_RSHIFT,
               PRINTSCREEN = SDLK_PRINT };
#else
enum Keycode { BACKSPACE = 8,
               TAB = 9,
               RETURN = 13,
               ESCAPE = 27,
               SPACE = 32,
               A = 97,
               B = 98,
               C = 99,
               D = 100,
               E = 101,
               F = 102,
               G = 103,
               H = 104,
               I = 105,
               J = 106,
               K = 107,
               L = 108,
               M = 109,
               N = 110,
               O = 111,
               P = 112,
               Q = 113,
               R = 114,
               S = 115,
               T = 116,
               U = 117,
               V = 118,
               W = 119,
               X = 120,
               Y = 121,
               Z = 122,
               DEL = 127,
               UP,
               DOWN,
               RIGHT,
               LEFT,
               INSERT,
               HOME,
               END,
               PAGE_UP,
               PAGE_DOWN,
               LEFT_CONTROL,
               LEFT_ALT,
               LEFT_SHIFT,
               RIGHT_CONTROL,
               RIGHT_SHIFT,
               PRINTSCREEN };
#endif

}

}

#endif
