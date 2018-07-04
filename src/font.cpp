#include "Font.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <fstream>
#include <vector>

Font::Font(float height) : mGlyphs{}, mTex(nullptr), mHeight(height) {}

Font::~Font() {
	ltex_free(mTex);
}

Font* Font::load(const char* filename, float height) {
	// Leemos contenido del archivo
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (!file.is_open()) return nullptr;
	std::vector<uint8_t> fileData(static_cast<uint32_t>(file.tellg()));
	file.seekg(0);
	file.read(reinterpret_cast<char*>(fileData.data()), fileData.size());

	// Creamos objeto Font
	Font* font = new Font(height);

	// Renderizamos alpha map
	int imgWidth = 256, imgHeight = 256;
	std::vector<uint8_t> alphaBuffer(imgWidth * imgHeight);
	while (stbtt_BakeFontBitmap(fileData.data(), 0, height, alphaBuffer.data(), imgWidth, imgHeight, 0, 256, font->mGlyphs.data()) <= 0) {
		if ( imgWidth == imgHeight ) imgWidth *= 2;
		else imgHeight *= 2;
		alphaBuffer.resize(imgWidth * imgHeight);
	}

	// Creamos color buffer
	std::vector<uint8_t> colorBuffer(imgWidth * imgHeight * 4, 255);
	for (size_t i = 0; i < alphaBuffer.size(); ++i) {
		colorBuffer[i*4 + 3] = alphaBuffer[i];
	}

	// Creamos textura
	font->mTex = ltex_alloc(imgWidth, imgHeight, false);
	ltex_setpixels(font->mTex, colorBuffer.data());

	// Devolvemos objetos font
	return font;
}

Vec2 Font::getTextSize(const char* text) const {
	size_t len = strlen(text);
	float x = 0, y = 0;
	stbtt_aligned_quad q;
	float minY = 99999;
	float maxY = -999999;
	for (size_t i = 0; i < len; ++i) {
		stbtt_GetBakedQuad(mGlyphs.data(), mTex->width, mTex->height, text[i], &x, &y, &q, true);
		maxY = std::max(maxY, q.y1);
		minY = std::min(minY, q.y0);
	}
	return Vec2(q.x1, maxY - minY);
}

void Font::draw(const char* text, const Vec2& pos) const {
	size_t len = strlen(text);
	float x = pos.x, y = pos.y;
	for (size_t i = 0; i < len; ++i) {
		stbtt_aligned_quad q;
		stbtt_GetBakedQuad(mGlyphs.data(), mTex->width, mTex->height, text[i], &x, &y, &q, true);
		ltex_drawrotsized(mTex, q.x0, q.y0, 0, 0, 0, q.x1 - q.x0, q.y1 - q.y0, q.s0, q.t0, q.s1, q.t1);
	}
}
