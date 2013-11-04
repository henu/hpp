#ifndef HPP_GUI_LABEL_H
#define HPP_GUI_LABEL_H

#include "widget.h"
#include "renderer.h"
#include "../types.h"

#include "../unicodestring.h"

namespace Hpp
{

namespace Gui
{

class Label : public Widget
{

public:

	inline Label(void);
	inline virtual ~Label(void);

	inline Color getColor(void) const { return color; }

	inline void setText(UnicodeString const& text) { this->text = text; text_total_width = 0; markToNeedReposition(); }
	inline void setColor(Color const& color) { this->color = color; }
	inline void setTextAlignment(Alignment align) { text_align = align; }
	inline void setMultiline(bool multiline) { this->multiline = multiline; markToNeedReposition(); }
// TODO: Support shadow!

	// Virtual functions for Widget
	inline virtual uint32_t doGetMinWidth(void) const;
	inline virtual uint32_t doGetMinHeight(uint32_t width) const;

private:

	typedef std::vector< uint32_t > Nums;

	UnicodeString text;
	Color color;
	bool multiline;
	Alignment text_align;

	// Cached stuff
	mutable uint32_t text_total_width;
	Strings multilines;
	mutable Nums multiline_widths;

	inline void formMultilines(Strings& lines, uint32_t width, bool form_multiline_widths) const;

	inline void renderLineWithAlignment(Hpp::UnicodeString const& text, int32_t text_width, int32_t pos_x, int32_t pos_y, int32_t width);

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual void onEnvironmentUpdated(void);
	inline virtual void doRepositioning(void);

};

inline Label::Label(void) :
color(0, 0, 0),
multiline(false),
text_align(CENTER),
text_total_width(0)
{
}

inline Label::~Label(void)
{
}

inline uint32_t Label::doGetMinWidth(void) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return 0;

	if (!multiline) {
		if (!text_total_width) {
			text_total_width = rend->getLabelWidth(text);
		}
		return text_total_width;
	} else {
		uint32_t result = 0;
		UnicodeString word = "";
		for (UnicodeString::const_iterator text_it = text.begin();
		     text_it != text.end();
		     ++ text_it) {
			Hpp::UChr c = *text_it;
			if (c == ' ' || c == '\n' || c == '\t') {
				result = std::max(result, rend->getLabelWidth(word));
				word = "";
			} else {
				word += c;
			}
		}
		result = std::max(result, rend->getLabelWidth(word));
		return result;
	}
}

inline uint32_t Label::doGetMinHeight(uint32_t width) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return 0;

	if (!multiline) {
		return rend->getLabelHeight(1);
	} else {
		Strings temp_lines;
		formMultilines(temp_lines, width, false);
		return rend->getLabelHeight(temp_lines.size());
	}
}

inline void Label::doRendering(int32_t x_origin, int32_t y_origin)
{
	Renderer* rend = getRenderer();
	if (!rend) return;

	if (!multiline) {
		renderLineWithAlignment(text, text_total_width, x_origin, y_origin, getWidth());
	} else {
		uint32_t render_pos_y = y_origin;
		for (ssize_t line_id = multilines.size() - 1; line_id >= 0 ; -- line_id) {
			Hpp::UnicodeString const& line = multilines[line_id];
			uint32_t line_width = multiline_widths[line_id];
			renderLineWithAlignment(line, line_width, x_origin, render_pos_y, getWidth());
			render_pos_y -= rend->getLabelHeight(1);
		}
	}
}

inline void Label::onEnvironmentUpdated(void)
{
	markToNeedReposition();
}

void Label::doRepositioning(void)
{
	// Only multiline support needs something to be done
	if (multiline) {
		formMultilines(multilines, getWidth(), true);
	}
}

inline void Label::formMultilines(Strings& lines, uint32_t width, bool form_multiline_widths) const
{
	lines.clear();

	Renderer const* rend = getRenderer();
	if (!rend) return;

	std::string line = "";
	std::string word = "";
	for (UnicodeString::const_iterator text_it = text.begin();
	     text_it != text.end();
	     ++ text_it) {
		Hpp::UChr c = *text_it;
		if (c == ' ' || c == '\n' || c == '\t') {
			// If this is first word, then add it to line, no matter what
			if (line.size() == 0) {
				line = word + " ";
			}
			// If there is already stuff at current line, then
			// we have to check if this word fits there.
			else {
				uint32_t line_width = rend->getLabelWidth(line + word);
				if (line_width > width && word.size() > 0) {
					lines.push_back(line);
					line = word + " ";
				} else {
					line += word + " ";
				}
			}
			word = "";
			// If this was endline, then add new line
			if (c == '\n') {
				lines.push_back(line);
				line = "";
			}
		} else {
			word += c;
		}
	}
	if (word.size() > 0) {
		// If this is first word, then add it to line, no matter what
		if (line.size() == 0) {
			lines.push_back(word);
		}
		// If there is already stuff at current line, then
		// we have to check if this word fits there.
		else {
			float line_width = rend->getLabelWidth(line + word);
			if (line_width > width) {
				lines.push_back(line);
				lines.push_back(word);
			} else {
				lines.push_back(line + word);
			}
		}
	}

	// Go lines through and remove whitespace from their tails.
	for (size_t line_id = 0; line_id < lines.size(); line_id ++) {
		lines[line_id] = trim(lines[line_id]);
	}

	// Form widths, if requested
	if (form_multiline_widths) {
		multiline_widths.clear();
		multiline_widths.reserve(lines.size());
		for (size_t line_id = 0; line_id < lines.size(); line_id ++) {
			multiline_widths.push_back(rend->getLabelWidth(lines[line_id]));
		}
	}
}

void Label::renderLineWithAlignment(Hpp::UnicodeString const& text, int32_t text_width, int32_t pos_x, int32_t pos_y, int32_t width)
{
	Renderer* rend = getRenderer();
	HppAssert(rend, "No renderer!");

	if (text_align == LEFT) {
		rend->renderLabel(pos_x, pos_y, this, text);
	} else {
		int32_t extra_space = width - text_width;
		if (text_align == RIGHT) {
			rend->renderLabel(pos_x + extra_space, pos_y, this, text);
		} else {
			rend->renderLabel(pos_x + extra_space / 2, pos_y, this, text);
		}
	}
}

}

}

#endif
