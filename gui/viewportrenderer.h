#ifndef HPP_GUI_VIEWPORTRENDERER_H
#define HPP_GUI_VIEWPORTRENDERER_H

#include "renderer.h"

#include "../texture.h"
#include "../font.h"

namespace Hpp
{

class Renderqueue2d;

namespace Gui
{

class ViewportRenderer : public Renderer
{

public:

	enum Alignment { LEFT, RIGHT, TOP, BOTTOM, CENTER };

	ViewportRenderer(Renderqueue2d* rqueue = NULL);
	virtual ~ViewportRenderer(void);

	// Font size setting
	void setFontSize(uint32_t font_size);

	// Texture loading functions
	inline void loadTextureMenubarBg(Path const& path) { loadTexFromFile(tex_menubar_bg, path, false, false); }
	inline void loadTextureMenuseparator(Path const& path) { loadTexFromFile(tex_menuseparator, path, false, false); }
	inline void loadTextureMenuseparatorLeftend(Path const& path) { loadTexFromFile(tex_menuseparator_leftend, path, true, true); }
	inline void loadTextureMenuseparatorRightend(Path const& path) { loadTexFromFile(tex_menuseparator_rightend, path, true, true); }
	inline void loadTextureMenuitemBg(Path const& path) { loadTexFromFile(tex_menuitem_bg, path, false, false); }
	inline void loadTextureWindowBg(Path const& path) { loadTexFromFile(tex_window_bg, path, false, false); }
	inline void loadTextureWindowEdgeTop(Path const& path) { loadTexFromFile(tex_window_edge_top, path, false, true); }
	inline void loadTextureWindowEdgeTopLeft(Path const& path) { loadTexFromFile(tex_window_edge_topleft, path, true, true); }
	inline void loadTextureWindowEdgeTopRight(Path const& path) { loadTexFromFile(tex_window_edge_topright, path, true, true); }
	inline void loadTextureWindowEdgeLeft(Path const& path) { loadTexFromFile(tex_window_edge_left, path, true, false); }
	inline void loadTextureWindowEdgeRight(Path const& path) { loadTexFromFile(tex_window_edge_right, path, true, false); }
	inline void loadTextureWindowEdgeBottom(Path const& path) { loadTexFromFile(tex_window_edge_bottom, path, false, true); }
	inline void loadTextureWindowEdgeBottomLeft(Path const& path) { loadTexFromFile(tex_window_edge_bottomleft, path, true, true); }
	inline void loadTextureWindowEdgeBottomRight(Path const& path) { loadTexFromFile(tex_window_edge_bottomright, path, true, true); }
	inline void loadTextureFieldBg(Path const& path) { loadTexFromFile(tex_field_bg, path, false, false); }
	inline void loadTextureFieldEdgeTop(Path const& path) { loadTexFromFile(tex_field_edge_top, path, false, true); }
	inline void loadTextureFieldEdgeTopLeft(Path const& path) { loadTexFromFile(tex_field_edge_topleft, path, true, true); }
	inline void loadTextureFieldEdgeTopRight(Path const& path) { loadTexFromFile(tex_field_edge_topright, path, true, true); }
	inline void loadTextureFieldEdgeLeft(Path const& path) { loadTexFromFile(tex_field_edge_left, path, true, false); }
	inline void loadTextureFieldEdgeRight(Path const& path) { loadTexFromFile(tex_field_edge_right, path, true, false); }
	inline void loadTextureFieldEdgeBottom(Path const& path) { loadTexFromFile(tex_field_edge_bottom, path, false, true); }
	inline void loadTextureFieldEdgeBottomLeft(Path const& path) { loadTexFromFile(tex_field_edge_bottomleft, path, true, true); }
	inline void loadTextureFieldEdgeBottomRight(Path const& path) { loadTexFromFile(tex_field_edge_bottomright, path, true, true); }
	inline void loadTextureFieldEdgeTopLeftConcave(Path const& path) { loadTexFromFile(tex_field_edge_topleft_concave, path, true, true); }
	inline void loadTextureFieldEdgeTopRightConcave(Path const& path) { loadTexFromFile(tex_field_edge_topright_concave, path, true, true); }
	inline void loadTextureFieldEdgeBottomLeftConcave(Path const& path) { loadTexFromFile(tex_field_edge_bottomleft_concave, path, true, true); }
	inline void loadTextureFieldEdgeBottomRightConcave(Path const& path) { loadTexFromFile(tex_field_edge_bottomright_concave, path, true, true); }
	inline void loadTextureFieldSelectionBg(Path const& path) { loadTexFromFile(tex_field_selection_bg, path, false, false); }
	inline void loadTextureButtonLeft(Path const& path) { loadTexFromFile(tex_button_left, path, true, true); }
	inline void loadTextureButton(Path const& path) { loadTexFromFile(tex_button, path, false, false); }
	inline void loadTextureButtonRight(Path const& path) { loadTexFromFile(tex_button_right, path, true, true); }
	inline void loadTextureButtonPressedLeft(Path const& path) { loadTexFromFile(tex_button_pressed_left, path, true, true); }
	inline void loadTextureButtonPressed(Path const& path) { loadTexFromFile(tex_button_pressed, path, false, false); }
	inline void loadTextureButtonPressedRight(Path const& path) { loadTexFromFile(tex_button_pressed_right, path, true, true); }
	inline void loadTextureFolder(Path const& path) { loadTexFromFile(tex_folder, path, true, true); }
	inline void loadTextureFile(Path const& path) { loadTexFromFile(tex_file, path, true, true); }
	inline void loadTextureScrollbarHoriz(Path const& path) { loadTexFromFile(tex_scrollbar_horiz, path, false, false); }
	inline void loadTextureScrollbarHorizLeft(Path const& path) { loadTexFromFile(tex_scrollbar_horiz_left, path, true, true); }
	inline void loadTextureScrollbarHorizRight(Path const& path) { loadTexFromFile(tex_scrollbar_horiz_right, path, true, true); }
	inline void loadTextureScrollbarVert(Path const& path) { loadTexFromFile(tex_scrollbar_vert, path, false, false); }
	inline void loadTextureScrollbarVertTop(Path const& path) { loadTexFromFile(tex_scrollbar_vert_top, path, true, true); }
	inline void loadTextureScrollbarVertBottom(Path const& path) { loadTexFromFile(tex_scrollbar_vert_bottom, path, true, true); }
	inline void loadTextureScrollbarPressedHoriz(Path const& path) { loadTexFromFile(tex_scrollbar_pressed_horiz, path, false, false); }
	inline void loadTextureScrollbarPressedHorizLeft(Path const& path) { loadTexFromFile(tex_scrollbar_pressed_horiz_left, path, true, true); }
	inline void loadTextureScrollbarPressedHorizRight(Path const& path) { loadTexFromFile(tex_scrollbar_pressed_horiz_right, path, true, true); }
	inline void loadTextureScrollbarPressedVert(Path const& path) { loadTexFromFile(tex_scrollbar_pressed_vert, path, false, false); }
	inline void loadTextureScrollbarPressedVertTop(Path const& path) { loadTexFromFile(tex_scrollbar_pressed_vert_top, path, true, true); }
	inline void loadTextureScrollbarPressedVertBottom(Path const& path) { loadTexFromFile(tex_scrollbar_pressed_vert_bottom, path, true, true); }
	inline void loadTextureScrollbarButtonUp(Path const& path) { loadTexFromFile(tex_scrollbar_button_up, path, true, true); }
	inline void loadTextureScrollbarButtonLeft(Path const& path) { loadTexFromFile(tex_scrollbar_button_left, path, true, true); }
	inline void loadTextureScrollbarButtonRight(Path const& path) { loadTexFromFile(tex_scrollbar_button_right, path, true, true); }
	inline void loadTextureScrollbarButtonDown(Path const& path) { loadTexFromFile(tex_scrollbar_button_down, path, true, true); }
	inline void loadTextureScrollbarButtonPressedUp(Path const& path) { loadTexFromFile(tex_scrollbar_button_pressed_up, path, true, true); }
	inline void loadTextureScrollbarButtonPressedLeft(Path const& path) { loadTexFromFile(tex_scrollbar_button_pressed_left, path, true, true); }
	inline void loadTextureScrollbarButtonPressedRight(Path const& path) { loadTexFromFile(tex_scrollbar_button_pressed_right, path, true, true); }
	inline void loadTextureScrollbarButtonPressedDown(Path const& path) { loadTexFromFile(tex_scrollbar_button_pressed_down, path, true, true); }
	inline void loadTextureScrollbarBgHoriz(Path const& path) { loadTexFromFile(tex_scrollbar_bg_horiz, path, false, false); }
	inline void loadTextureScrollbarBgHorizLeft(Path const& path) { loadTexFromFile(tex_scrollbar_bg_horiz_left, path, true, true); }
	inline void loadTextureScrollbarBgHorizRight(Path const& path) { loadTexFromFile(tex_scrollbar_bg_horiz_right, path, true, true); }
	inline void loadTextureScrollbarBgVert(Path const& path) { loadTexFromFile(tex_scrollbar_bg_vert, path, false, false); }
	inline void loadTextureScrollbarBgVertTop(Path const& path) { loadTexFromFile(tex_scrollbar_bg_vert_top, path, true, true); }
	inline void loadTextureScrollbarBgVertBottom(Path const& path) { loadTexFromFile(tex_scrollbar_bg_vert_bottom, path, true, true); }
	inline void loadTextureSliderHoriz(Path const& path) { loadTexFromFile(tex_slider_horiz, path, false, false); }
	inline void loadTextureSliderVert(Path const& path) { loadTexFromFile(tex_slider_vert, path, false, false); }
	inline void loadTextureSliderPressedHoriz(Path const& path) { loadTexFromFile(tex_slider_pressed_horiz, path, false, false); }
	inline void loadTextureSliderPressedVert(Path const& path) { loadTexFromFile(tex_slider_pressed_vert, path, false, false); }
	inline void loadTextureSliderBgHoriz(Path const& path) { loadTexFromFile(tex_slider_bg_horiz, path, false, false); }
	inline void loadTextureSliderBgHorizLeft(Path const& path) { loadTexFromFile(tex_slider_bg_horiz_left, path, true, true); }
	inline void loadTextureSliderBgHorizRight(Path const& path) { loadTexFromFile(tex_slider_bg_horiz_right, path, true, true); }
	inline void loadTextureSliderBgVert(Path const& path) { loadTexFromFile(tex_slider_bg_vert, path, false, false); }
	inline void loadTextureSliderBgVertTop(Path const& path) { loadTexFromFile(tex_slider_bg_vert_top, path, true, true); }
	inline void loadTextureSliderBgVertBottom(Path const& path) { loadTexFromFile(tex_slider_bg_vert_bottom, path, true, true); }
	inline void loadTextureTab(Path const& path) { loadTexFromFile(tex_tab, path, false, false); }
	inline void loadTextureTabLeft(Path const& path) { loadTexFromFile(tex_tab_left, path, true, true); }
	inline void loadTextureTabRight(Path const& path) { loadTexFromFile(tex_tab_right, path, true, true); }

	// Texture setting functions
	inline void setTextureMenubarBg(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_menubar_bg, tex, false, false, clean_automatically); }
	inline void setTextureMenuseparator(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_menuseparator, tex, false, false, clean_automatically); }
	inline void setTextureMenuseparatorLeftend(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_menuseparator_leftend, tex, true, true, clean_automatically); }
	inline void setTextureMenuseparatorRightend(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_menuseparator_rightend, tex, true, true, clean_automatically); }
	inline void setTextureMenuitemBg(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_menuitem_bg, tex, false, false, clean_automatically); }
	inline void setTextureWindowBg(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_window_bg, tex, false, false, clean_automatically); }
	inline void setTextureWindowEdgeTop(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_window_edge_top, tex, false, true, clean_automatically); }
	inline void setTextureWindowEdgeTopLeft(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_window_edge_topleft, tex, true, true, clean_automatically); }
	inline void setTextureWindowEdgeTopRight(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_window_edge_topright, tex, true, true, clean_automatically); }
	inline void setTextureWindowEdgeLeft(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_window_edge_left, tex, true, false, clean_automatically); }
	inline void setTextureWindowEdgeRight(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_window_edge_right, tex, true, false, clean_automatically); }
	inline void setTextureWindowEdgeBottom(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_window_edge_bottom, tex, false, true, clean_automatically); }
	inline void setTextureWindowEdgeBottomLeft(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_window_edge_bottomleft, tex, true, true, clean_automatically); }
	inline void setTextureWindowEdgeBottomRight(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_window_edge_bottomright, tex, true, true, clean_automatically); }
	inline void setTextureFieldBg(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_field_bg, tex, false, false, clean_automatically); }
	inline void setTextureFieldEdgeTop(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_field_edge_top, tex, false, true, clean_automatically); }
	inline void setTextureFieldEdgeTopLeft(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_field_edge_topleft, tex, true, true, clean_automatically); }
	inline void setTextureFieldEdgeTopRight(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_field_edge_topright, tex, true, true, clean_automatically); }
	inline void setTextureFieldEdgeLeft(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_field_edge_left, tex, true, false, clean_automatically); }
	inline void setTextureFieldEdgeRight(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_field_edge_right, tex, true, false, clean_automatically); }
	inline void setTextureFieldEdgeBottom(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_field_edge_bottom, tex, false, true, clean_automatically); }
	inline void setTextureFieldEdgeBottomLeft(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_field_edge_bottomleft, tex, true, true, clean_automatically); }
	inline void setTextureFieldEdgeBottomRight(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_field_edge_bottomright, tex, true, true, clean_automatically); }
	inline void setTextureFieldEdgeTopLeftConcave(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_field_edge_topleft_concave, tex, true, true, clean_automatically); }
	inline void setTextureFieldEdgeTopRightConcave(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_field_edge_topright_concave, tex, true, true, clean_automatically); }
	inline void setTextureFieldEdgeBottomLeftConcave(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_field_edge_bottomleft_concave, tex, true, true, clean_automatically); }
	inline void setTextureFieldEdgeBottomRightConcave(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_field_edge_bottomright_concave, tex, true, true, clean_automatically); }
	inline void setTextureFieldSelectionBg(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_field_selection_bg, tex, false, false, clean_automatically); }
	inline void setTextureButtonLeft(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_button_left, tex, true, true, clean_automatically); }
	inline void setTextureButton(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_button, tex, false, false, clean_automatically); }
	inline void setTextureButtonRight(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_button_right, tex, true, true, clean_automatically); }
	inline void setTextureButtonPressedLeft(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_button_pressed_left, tex, true, true, clean_automatically); }
	inline void setTextureButtonPressed(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_button_pressed, tex, false, false, clean_automatically); }
	inline void setTextureButtonPressedRight(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_button_pressed_right, tex, true, true, clean_automatically); }
	inline void setTextureFolder(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_folder, tex, true, true, clean_automatically); }
	inline void setTextureFile(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_file, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarHoriz(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_horiz, tex, false, false, clean_automatically); }
	inline void setTextureScrollbarHorizLeft(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_horiz_left, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarHorizRight(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_horiz_right, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarVert(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_vert, tex, false, false, clean_automatically); }
	inline void setTextureScrollbarVertTop(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_vert_top, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarVertBottom(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_vert_bottom, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarPressedHoriz(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_pressed_horiz, tex, false, false, clean_automatically); }
	inline void setTextureScrollbarPressedHorizLeft(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_pressed_horiz_left, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarPressedHorizRight(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_pressed_horiz_right, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarPressedVert(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_pressed_vert, tex, false, false, clean_automatically); }
	inline void setTextureScrollbarPressedVertTop(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_pressed_vert_top, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarPressedVertBottom(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_pressed_vert_bottom, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarButtonUp(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_button_up, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarButtonLeft(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_button_left, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarButtonRight(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_button_right, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarButtonDown(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_button_down, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarButtonPressedUp(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_button_pressed_up, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarButtonPressedLeft(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_button_pressed_left, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarButtonPressedRight(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_button_pressed_right, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarButtonPressedDown(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_button_pressed_down, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarBgHoriz(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_bg_horiz, tex, false, false, clean_automatically); }
	inline void setTextureScrollbarBgHorizLeft(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_bg_horiz_left, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarBgHorizRight(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_bg_horiz_right, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarBgVert(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_bg_vert, tex, false, false, clean_automatically); }
	inline void setTextureScrollbarBgVertTop(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_bg_vert_top, tex, true, true, clean_automatically); }
	inline void setTextureScrollbarBgVertBottom(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_scrollbar_bg_vert_bottom, tex, true, true, clean_automatically); }
	inline void setTextureSliderHoriz(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_slider_horiz, tex, false, false, clean_automatically); }
	inline void setTextureSliderVert(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_slider_vert, tex, false, false, clean_automatically); }
	inline void setTextureSliderPressedHoriz(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_slider_pressed_horiz, tex, false, false, clean_automatically); }
	inline void setTextureSliderPressedVert(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_slider_pressed_vert, tex, false, false, clean_automatically); }
	inline void setTextureSliderBgHoriz(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_slider_bg_horiz, tex, false, false, clean_automatically); }
	inline void setTextureSliderBgHorizLeft(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_slider_bg_horiz_left, tex, true, true, clean_automatically); }
	inline void setTextureSliderBgHorizRight(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_slider_bg_horiz_right, tex, true, true, clean_automatically); }
	inline void setTextureSliderBgVert(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_slider_bg_vert, tex, false, false, clean_automatically); }
	inline void setTextureSliderBgVertTop(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_slider_bg_vert_top, tex, true, true, clean_automatically); }
	inline void setTextureSliderBgVertBottom(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_slider_bg_vert_bottom, tex, true, true, clean_automatically); }
	inline void setTextureTab(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_tab, tex, false, false, clean_automatically); }
	inline void setTextureTabLeft(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_tab_left, tex, true, true, clean_automatically); }
	inline void setTextureTabRight(Texture* tex, bool clean_automatically = false) { setTexFromTex(tex_tab_right, tex, true, true, clean_automatically); }

	// Font loading functions. Multiple fonts can be used. The firstly
	// loaded font will always be preferred, but if some character is not
	// found from there, then it is loaded from second font, and if its
	// missing there too, then third is tried and so on.
	void loadFont(Path const& path);

	inline void setRenderqueue(Renderqueue2d* rqueue) { this->rqueue = rqueue; sizeChanged(); }

	// Fixed sprite rendering functions. These use coordinate
	// system from topleft, rather than from bottomleft.
	void prepareSprites(Real x_origin, Real y_origin);
	inline void textSetColor(Color const& color) { text_color = color; }
	inline void textSetHorizontalAlign(Alignment align) { text_align = align; }
	inline void textSetVerticalAlign(Alignment align) { text_valign = align; }
	void renderSprite(Texture* tex, Vector2 const& pos, Vector2 const& size = Vector2(-1, -1), Color const& color = Color(1, 1, 1));
	void renderString(UnicodeString const& str, Real fontsize, Vector2 const& pos, Vector2 const& size);

	virtual void renderTextCursor(int32_t x_origin, int32_t y_origin);

private:

	struct TexturePtr
	{
		Texture* tex;
		bool clean;
		inline TexturePtr(void) : tex(NULL), clean(true) { }
		inline ~TexturePtr(void) {
			if (clean) {
				delete tex;
				tex = NULL;
			}
		}
	};

	Renderqueue2d* rqueue;

	// Textures
	TexturePtr tex_menubar_bg;
	TexturePtr tex_menuseparator;
	TexturePtr tex_menuseparator_leftend;
	TexturePtr tex_menuseparator_rightend;
	TexturePtr tex_menuitem_bg;
	TexturePtr tex_window_bg;
	TexturePtr tex_window_edge_top;
	TexturePtr tex_window_edge_topleft;
	TexturePtr tex_window_edge_topright;
	TexturePtr tex_window_edge_left;
	TexturePtr tex_window_edge_right;
	TexturePtr tex_window_edge_bottom;
	TexturePtr tex_window_edge_bottomleft;
	TexturePtr tex_window_edge_bottomright;
	TexturePtr tex_field_bg;
	TexturePtr tex_field_edge_top;
	TexturePtr tex_field_edge_topleft;
	TexturePtr tex_field_edge_topright;
	TexturePtr tex_field_edge_left;
	TexturePtr tex_field_edge_right;
	TexturePtr tex_field_edge_bottom;
	TexturePtr tex_field_edge_bottomleft;
	TexturePtr tex_field_edge_bottomright;
	TexturePtr tex_field_edge_topleft_concave;
	TexturePtr tex_field_edge_topright_concave;
	TexturePtr tex_field_edge_bottomleft_concave;
	TexturePtr tex_field_edge_bottomright_concave;
	TexturePtr tex_field_selection_bg;
	TexturePtr tex_button_left;
	TexturePtr tex_button;
	TexturePtr tex_button_right;
	TexturePtr tex_button_pressed_left;
	TexturePtr tex_button_pressed;
	TexturePtr tex_button_pressed_right;
	TexturePtr tex_folder;
	TexturePtr tex_file;
	TexturePtr tex_scrollbar_horiz;
	TexturePtr tex_scrollbar_horiz_left;
	TexturePtr tex_scrollbar_horiz_right;
	TexturePtr tex_scrollbar_vert;
	TexturePtr tex_scrollbar_vert_top;
	TexturePtr tex_scrollbar_vert_bottom;
	TexturePtr tex_scrollbar_pressed_horiz;
	TexturePtr tex_scrollbar_pressed_horiz_left;
	TexturePtr tex_scrollbar_pressed_horiz_right;
	TexturePtr tex_scrollbar_pressed_vert;
	TexturePtr tex_scrollbar_pressed_vert_top;
	TexturePtr tex_scrollbar_pressed_vert_bottom;
	TexturePtr tex_scrollbar_button_up;
	TexturePtr tex_scrollbar_button_left;
	TexturePtr tex_scrollbar_button_right;
	TexturePtr tex_scrollbar_button_down;
	TexturePtr tex_scrollbar_button_pressed_up;
	TexturePtr tex_scrollbar_button_pressed_left;
	TexturePtr tex_scrollbar_button_pressed_right;
	TexturePtr tex_scrollbar_button_pressed_down;
	TexturePtr tex_scrollbar_bg_horiz;
	TexturePtr tex_scrollbar_bg_horiz_left;
	TexturePtr tex_scrollbar_bg_horiz_right;
	TexturePtr tex_scrollbar_bg_vert;
	TexturePtr tex_scrollbar_bg_vert_top;
	TexturePtr tex_scrollbar_bg_vert_bottom;
	TexturePtr tex_slider_horiz;
	TexturePtr tex_slider_vert;
	TexturePtr tex_slider_pressed_horiz;
	TexturePtr tex_slider_pressed_vert;
	TexturePtr tex_slider_bg_horiz;
	TexturePtr tex_slider_bg_horiz_left;
	TexturePtr tex_slider_bg_horiz_right;
	TexturePtr tex_slider_bg_vert;
	TexturePtr tex_slider_bg_vert_top;
	TexturePtr tex_slider_bg_vert_bottom;
	TexturePtr tex_tab;
	TexturePtr tex_tab_left;
	TexturePtr tex_tab_right;

	// Font and its sizes
	Font font;
	uint32_t font_menu_size;
	uint32_t font_menuitem_size;
	uint32_t font_titlebar_size;
	uint32_t font_label_size;
	uint32_t font_input_size;
	uint32_t font_button_size;
	uint32_t font_tablabel_size;

	// Padding and other sizes
	Real padding_menu_h;
	Real padding_menuitem_h, padding_menuitem_v;
	Real folderview_min_width;
	uint32_t folderview_min_rows;
	uint32_t window_dragcorner_size;

	// Sprite rendering functions
	Real spr_x_origin, spr_y_origin;
	Color text_color;
	Alignment text_align, text_valign;

	void loadTexFromFile(TexturePtr& texptr, Hpp::Path const& path, bool clamp_to_edge_horizontally, bool clamp_to_edge_vertically);
	void setTexFromTex(TexturePtr& texptr, Hpp::Texture* tex, bool clamp_to_edge_horizontally, bool clamp_to_edge_vertically, bool clean);

	// Virtual functions required by superclass Renderer
	virtual uint32_t getWidth(void) const;
	virtual uint32_t getHeight(void) const;
	virtual void initRendering(void);
	virtual void deinitRendering(void);
	virtual void renderMenubarBackground(int32_t x_origin, int32_t y_origin, uint32_t width, AreaWithMenubar const* areawithmenubar);
	virtual void renderMenuLabel(int32_t x_origin, int32_t y_origin, Menu const* menu, UnicodeString const& label, bool mouse_over);
	virtual void renderMenuseparator(int32_t x_origin, int32_t y_origin, Menuseparator const* menusep);
	virtual void renderMenuitem(int32_t x_origin, int32_t y_origin, Menuitem const* menuitem, UnicodeString const& label, bool mouse_over);
	virtual void renderWindow(int32_t x_origin, int32_t y_origin, Window const* window, UnicodeString const& title);
	virtual void renderLabel(int32_t x_origin, int32_t y_origin, Label const* label, UnicodeString const& label_str);
	virtual void renderTextinput(int32_t x_origin, int32_t y_origin, Textinput const* textinput);
	virtual void renderTextinputContents(int32_t x_origin, int32_t y_origin, TextinputContents const* textinputcontents, ssize_t cursor);
	virtual void renderButton(int32_t x_origin, int32_t y_origin, Button const* button, UnicodeString const& label, bool pressed);
	virtual void renderFolderview(int32_t x_origin, int32_t y_origin, Folderview const* folderview);
	virtual void renderFolderviewContents(int32_t x_origin, int32_t y_origin, FolderviewContents const* folderviewcontents, Path::DirChildren const& items);
	virtual void renderScrollbar(int32_t x_origin, int32_t y_origin, Scrollbar const* scrollbar, bool horizontal, bool up_or_left_key_pressed, bool down_or_right_key_pressed, bool slider_pressed);
	virtual void renderSlider(int32_t x_origin, int32_t y_origin, Slider const* slider, bool horizontal, bool slider_pressed);
	virtual void renderTabs(int32_t x_origin, int32_t y_origin, Tabs const* tabs);
	virtual uint32_t getMenubarHeight(void) const;
	virtual uint32_t getMenuLabelWidth(UnicodeString const& label) const;
	virtual uint32_t getMenuseparatorMinWidth(void) const;
	virtual uint32_t getMenuseparatorHeight(void) const;
	virtual uint32_t getMenuitemWidth(UnicodeString const& label) const;
	virtual uint32_t getMenuitemHeight(void) const;
	virtual uint32_t getWindowTitlebarHeight(void) const { return tex_window_edge_top.tex->getHeight(); }
	virtual uint32_t getWindowEdgeTopHeight(void) const { return tex_window_edge_bottom.tex->getHeight(); }
	virtual uint32_t getWindowEdgeLeftWidth(void) const { return tex_window_edge_left.tex->getWidth(); }
	virtual uint32_t getWindowEdgeRightWidth(void) const { return tex_window_edge_right.tex->getWidth(); }
	virtual uint32_t getWindowEdgeBottomHeight(void) const { return tex_window_edge_bottom.tex->getHeight(); }
	virtual uint32_t getWindowDragcornerSize(void) const { return window_dragcorner_size; }
	virtual uint32_t getLabelWidth(UnicodeString const& label) const;
	virtual uint32_t getLabelHeight(void) const;
	virtual uint32_t getTextinputWidth(size_t cols) const { return tex_field_edge_left.tex->getWidth() + tex_field_edge_right.tex->getWidth() + font.getStringWidth("_", font_input_size) * cols; }
	virtual uint32_t getTextinputHeight(void) const { return tex_field_edge_top.tex->getHeight() + tex_field_edge_bottom.tex->getHeight() + font_input_size; }
	virtual uint32_t getMinimumTextinputContentsWidth(UnicodeString const& value) const { return font.getStringWidth(value, font_input_size) + font.getStringWidth("|", font_input_size); }
	virtual uint32_t getTextinputContentsHeight(void) const { return font_input_size; }
	virtual void getTextinputContentsCursorProps(uint32_t& cursor_pos_x, uint32_t& cursor_pos_y, uint32_t& cursor_width, uint32_t& cursor_height, UnicodeString const& value, ssize_t cursor) const;
	virtual void getTextinputEdgeSizes(uint32_t& edge_top, uint32_t& edge_left, uint32_t& edge_right, uint32_t& edge_bottom) const;
	virtual uint32_t getButtonWidth(UnicodeString const& label) const;
	virtual uint32_t getButtonHeight(void) const;
	virtual uint32_t getMinimumFolderviewWidth(void) const;
	virtual uint32_t getFolderviewHeight(void) const;
	virtual uint32_t getMinimumFolderviewContentsWidth(UnicodeString const& label) const;
	virtual uint32_t getFolderviewContentsHeight(size_t items) const;
	virtual void getFolderviewEdgeSizes(uint32_t& edge_top, uint32_t& edge_left, uint32_t& edge_right, uint32_t& edge_bottom) const;
	virtual uint32_t getScrollbarWidth(void) const;
	virtual uint32_t getScrollbarHeight(void) const;
	inline virtual uint32_t getScrollbarButtonLeftWidth(void) const { return tex_scrollbar_button_left.tex->getWidth(); };
	inline virtual uint32_t getScrollbarButtonRightWidth(void) const { return tex_scrollbar_button_right.tex->getWidth(); };
	inline virtual uint32_t getScrollbarButtonUpHeight(void) const { return tex_scrollbar_button_up.tex->getHeight(); };
	inline virtual uint32_t getScrollbarButtonDownHeight(void) const { return tex_scrollbar_button_down.tex->getHeight(); };
	inline virtual uint32_t getScrollboxMinWidth(void) const { return tex_scrollbar_bg_vert.tex->getWidth() * 2; }
	inline virtual uint32_t getScrollboxMinHeight(void) const { return tex_scrollbar_bg_horiz.tex->getHeight() * 2; }
	inline virtual uint32_t getHorizSliderWidth(void) const { return tex_slider_horiz.tex->getWidth(); }
	inline virtual uint32_t getVertSliderHeight(void) const { return tex_slider_vert.tex->getHeight(); }
	inline virtual uint32_t getHorizSliderMinWidth(void) const { return tex_slider_bg_horiz_left.tex->getWidth() + tex_slider_bg_horiz_right.tex->getWidth() + tex_slider_bg_horiz.tex->getWidth() + tex_slider_horiz.tex->getWidth(); }
	inline virtual uint32_t getVertSliderMinHeight(void) const { return tex_slider_bg_vert_top.tex->getHeight() + tex_slider_bg_vert_bottom.tex->getHeight() + tex_slider_bg_vert.tex->getHeight() + tex_slider_vert.tex->getHeight(); }
	inline virtual uint32_t getHorizSliderHeight(void) const { return std::max(tex_slider_horiz.tex->getHeight(), tex_slider_bg_horiz.tex->getHeight()); }
	inline virtual uint32_t getVertSliderWidth(void) const { return std::max(tex_slider_vert.tex->getWidth(), tex_slider_bg_vert.tex->getWidth()); }
	inline virtual uint32_t getTabbarHeight(void) const { return tex_tab.tex->getHeight(); }
	inline virtual uint32_t getTabsLeftEdgeWidth(void) const { return tex_field_edge_right.tex->getWidth(); }
	inline virtual uint32_t getTabsRightEdgeWidth(void) const { return tex_field_edge_left.tex->getWidth(); }
	inline virtual uint32_t getTabsBottomEdgeHeight(void) const { return tex_field_edge_top.tex->getHeight(); }
	inline virtual uint32_t getTablabelWidth(UnicodeString const& label) const { return font.getStringWidth(label, font_tablabel_size) + tex_tab_left.tex->getWidth() + tex_tab_right.tex->getWidth(); }
	virtual void setRenderareaLimit(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	virtual void removeRenderareaLimit(void);

};

}

}

#endif


