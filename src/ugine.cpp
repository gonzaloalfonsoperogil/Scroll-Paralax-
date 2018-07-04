#include "ugine.h"


#define LITE_GFX_IMPLEMENTATION
#include <litegfx.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

ltex_t* loadTexture(const char* filename) {
	// Cargamos pixeles con STB Image
	int w, h;
	uint8_t* buffer = stbi_load(filename, &w, &h, nullptr, 4);
	if (!buffer) return nullptr;

	// Creamos la textura con LiteGFX
	ltex_t* tex = ltex_alloc(w, h, false);

	// Copiamos pixeles
	ltex_setpixels(tex, buffer);

	// Borramos buffer de STB Image
	stbi_image_free(buffer);

	return tex;
}