#pragma once

#include "vec2.h"
#include <litegfx.h>
#include <stb_truetype.h>
#include <array>

class Font {
public:
	~Font();

	static Font*  load(const char* filename, float height);
	float         getHeight()								const	{ return mHeight; }
	Vec2          getTextSize(const char* text)				const;
	void          draw(const char* text, const Vec2& pos)	const;
protected:
	Font(float height);
private:
	std::array<stbtt_bakedchar, 256>	mGlyphs;
	ltex_t*								mTex;
	float								mHeight;
};

