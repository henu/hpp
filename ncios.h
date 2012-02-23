#ifndef HPP_NCIOS_H
#define HPP_NCIOS_H

#include <ostream>
#include <glob.h>

namespace Hpp
{

namespace NC
{

	enum Color { BLACK = 0,
	             RED = 1,
	             GREEN = 2,
	             BROWN = 3,
	             BLUE = 4,
	             MAGENTA = 5,
	             CYAN = 6,
	             GRAY = 7,
	             DARK_GRAY = 8,
	             LIGHT_RED = 9,
	             LIGHT_GREEN = 10,
	             YELLOW = 11,
	             LIGHT_BLUE = 12,
	             LIGHT_MAGENTA = 13,
	             LIGHT_CYAN = 14,
	             WHITE = 15 };

}

class NCursesOutBuffer : public std::basic_streambuf< char, std::char_traits< char > >
{

public:

	NCursesOutBuffer(void);
	~NCursesOutBuffer(void);

protected:

	virtual int overflow(int c = Traits::eof());
	virtual int sync(void);

private:

	static size_t const BUF_SIZE = 64;

	typedef std::char_traits< char > Traits;

	char buf[BUF_SIZE];

	std::string line_buf;

	void putChars(char const* begin, char const* end);

};

class NCursesOut : public std::basic_ostream< char, std::char_traits< char > >
{

public:

	NCursesOut(void);
	virtual ~NCursesOut(void);

	// Initializes/deinitializes NCurses. These are called automatically,
	// but they can be called from here also. These can be called multiple
	// times, but there must be same amount of deinits as inits.
	void init(void);
	void deinit(void);

	// Reinitializes NCurses. This should be called
	// for example when terminal is resized.
	void reinit(void);

	// Some getters
	size_t getScreenWidth(void);
	size_t getScreenHeight(void);
	bool areColorsSupported(void);

	// Sets location
	void setLoc(size_t col, size_t row);

	// Sets colors. You need to ensure that colors are supported
	void setColors(NC::Color text, NC::Color bg);

	// Sets cursor visibility
	void setCursorVisible(bool visible);

	// Refreshes screen
	void refresh(void);

private:

	NCursesOutBuffer sbuf;

	size_t automatic_inits;

};

extern NCursesOut ncout;

}

#endif
