#include "ncios.h"

#include "exception.h"
#include "assert.h"

#include <ncursesw/ncurses.h>

namespace Hpp
{

// Global instances
NCursesOut ncout;

// NCurses stuff
size_t ncurses_init_count = 0;
WINDOW* mainwin;
int mainwin_w, mainwin_h;
bool colors_supported;
int color_in_use = -1;

// Global NCurses initialization and deinitialization functions
void initNCurses(void);
void deinitNCurses(void);
void reinitNCurses(void);
void ensureNCursesInitialized(size_t& automatic_inits);
void initNCursesReal(void);
void deinitNCursesReal(void);
void initNCColors(void);

inline short toColorIndex(NC::Color color);



// ----------------------------------------
// ----------------------------------------
//
// Class NCursesOutBuffer
//
// ----------------------------------------
// ----------------------------------------

NCursesOutBuffer::NCursesOutBuffer(void)
{
	setp(buf, buf + BUF_SIZE);
}

NCursesOutBuffer::~NCursesOutBuffer(void)
{
	HppAssert(pptr() == pbase(), "Buffer is not empty!");
}

int NCursesOutBuffer::overflow(int c)
{
	putChars(pbase(), pptr());
	if (c != Traits::eof()) {
		char c2 = c;
		putChars(&c2, &c2 + 1);
	}
	setp(buf, buf + BUF_SIZE);
	return 0;
}

int NCursesOutBuffer::sync(void)
{
	putChars(pbase(), pptr());
	setp(buf, buf + BUF_SIZE);
	return 0;
}

void NCursesOutBuffer::putChars(char const* begin, char const* end)
{
	addnstr(begin, end - begin);
}



// ----------------------------------------
// ----------------------------------------
//
// Class NCursesOut
//
// ----------------------------------------
// ----------------------------------------

// TODO: In future, code some kind of buffer for the whole screen to prevent problems with unicode!
NCursesOut::NCursesOut(void) :
std::basic_ostream< char, std::char_traits< char > >(&sbuf),
sbuf(),
automatic_inits(0)
{
}

NCursesOut::~NCursesOut(void)
{
	for (size_t i = 0; i < automatic_inits; i ++) {
		deinitNCurses();
	}
}

void NCursesOut::init(void)
{
	initNCurses();
}

void NCursesOut::deinit(void)
{
	deinitNCurses();
}

void NCursesOut::reinit(void)
{
	reinitNCurses();
}

size_t NCursesOut::getScreenWidth(void)
{
	ensureNCursesInitialized(automatic_inits);
	return mainwin_w;
}

size_t NCursesOut::getScreenHeight(void)
{
	ensureNCursesInitialized(automatic_inits);
	return mainwin_h;
}

bool NCursesOut::areColorsSupported(void)
{
	ensureNCursesInitialized(automatic_inits);
	return colors_supported;
}

void NCursesOut::setLoc(size_t col, size_t row)
{
	move(row, col);
}

void NCursesOut::setColors(NC::Color text, NC::Color bg)
{
	ensureNCursesInitialized(automatic_inits);
	HppAssert(colors_supported, "Colors are not supported!");
	int text_i = toColorIndex(text);
	int bg_i = toColorIndex(bg);
	if (color_in_use >= 0) {
		attroff(COLOR_PAIR(color_in_use));
		attroff(A_BOLD);
	}
	if ((int)text >= 8) {
		attron(A_BOLD);
	}
	color_in_use = (COLOR_WHITE - text_i) * 8 + bg_i;
	attron(COLOR_PAIR(color_in_use));
}

void NCursesOut::refresh(void)
{
	::refresh();
}



// ----------------------------------------
// ----------------------------------------
//
// Implementation of global functions
//
// ----------------------------------------
// ----------------------------------------

void initNCurses(void)
{
	if (ncurses_init_count == 0) {
		initNCursesReal();
	}
	ncurses_init_count ++;
}

void deinitNCurses(void)
{
	if (ncurses_init_count == 0) {
		throw Exception("Too many deinits of ncout/ncin!");
	}
	ncurses_init_count --;
	if (ncurses_init_count == 0) {
		deinitNCursesReal();
	}
}

void reinitNCurses(void)
{
	if (ncurses_init_count == 0) {
		return;
	}
	deinitNCursesReal();
	initNCursesReal();
}

void ensureNCursesInitialized(size_t& automatic_inits)
{
	if (ncurses_init_count == 0) {
		initNCurses();
		automatic_inits ++;
	}
}

void initNCursesReal(void)
{
	setlocale(LC_ALL,"");
	if ((mainwin = initscr()) == NULL) {
		throw Exception("Unable to init NCurses!");
	}
	getmaxyx(mainwin, mainwin_h, mainwin_w);
	colors_supported = has_colors();
	if (colors_supported) {
		start_color();
		initNCColors();
	}
}

void deinitNCursesReal(void)
{
	if (endwin() != OK) {
		throw Exception("Unable to deinit NCurses!");
	}
	color_in_use = -1;
}

inline short toColorIndex(NC::Color color)
{
	int color_i = (int)color;
	HppAssert(color_i >= 0 && color_i <= 15, "Invalid color!");
	if (color_i <= 7) {
		return color_i;
	} else {
		return color_i - 8;
	}
}

void initNCColors(void)
{
	int offset = 0;
	for (int foreg = 0; foreg < 8; foreg ++) {
		for (int backg = 0; backg < 8; backg ++) {
			init_pair(offset, COLOR_WHITE - foreg, COLOR_BLACK + backg);
			offset ++;
		}
	}
}

}
