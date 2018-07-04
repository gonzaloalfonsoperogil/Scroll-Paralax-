#include "sprite.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

void Sprite::setTexture(const ltex_t* tex) {
	if (tex == nullptr) return;
	mTexture = tex;
	updateCollisionInfo();
	if ( mCollisionType == COLLISION_PIXELS ) updateCollisionPixels();
}

void Sprite::setScale(const Vec2& scale) {
	mScale = scale;
	updateCollisionInfo();
	if (mCollisionType == COLLISION_PIXELS) updateCollisionPixels();
}

void Sprite::update(float deltaTime) {
	// Llamamos callback
	if (mCallback) mCallback(*this, deltaTime);

	// Actualizamos animacion
	int totalFrames = mHframes * mVframes;
	mCurrentFrame += mFps * deltaTime;
	if (mCurrentFrame >= totalFrames) mCurrentFrame -= totalFrames;
	if (mCurrentFrame < 0) mCurrentFrame += totalFrames;
}

void Sprite::draw() const {
	// Obtenemos coordenadas UV
	int row = static_cast<int>(mCurrentFrame) / mHframes;
	int col = static_cast<int>(mCurrentFrame) % mHframes;
	float stepU = 1.0f / mHframes;
	float stepV = 1.0f / mVframes;
	float u0 = col * stepU;
	float v0 = row * stepV;
	float u1 = u0 + stepU;
	float v1 = v0 + stepV;

	// Establecemos propiedades de pintado
	lgfx_setblend(mBlend);
	lgfx_setcolor(mColor[0], mColor[1], mColor[2], mColor[3]);

	// Pintamos
	Vec2 size = getSize();
	ltex_drawrotsized(mTexture,
		mPosition.x, mPosition.y,
		mAngle, mPivot.x, mPivot.y,
		size.x, size.y,
		u0, v0, u1, v1);
}

void Sprite::setCollisionType(CollisionType type) {
	mCollisionType = type;
	delete mCollider;
	mCollider = nullptr;

	switch ( type ) {
	case COLLISION_CIRCLE:
		mCollider = new CircleCollider(mCircleCenter, mCircleRadius);
		break;
	case COLLISION_RECT:
		mCollider = new RectCollider(mRectOrigin, mRectSize);
		break;
	case COLLISION_PIXELS:
		mCollider = new PixelsCollider(mRectOrigin, mRectSize, mPixels);
		if (mPixels.size() == 0) updateCollisionPixels();
		break;
	}

	if (type != COLLISION_PIXELS) mPixels.clear();
}

bool Sprite::collides(const Sprite& other) const {
	if (!mCollider || !other.mCollider) return false;
	return mCollider->collides(*other.mCollider);
}

void Sprite::updateCollisionInfo() {
	Vec2 size = getSize();
	mRectOrigin = mPosition - mPivot * size;
	mRectSize = size;
	mCircleCenter = mRectOrigin + mRectSize / 2;
	mCircleRadius = std::min(size.x, size.y) / 2;
}

void Sprite::updateCollisionPixels() {
	if (mTexture == nullptr) return;

	// Get original unscaled pixels
	std::vector<uint8_t> pixels(mTexture->width * mTexture->height * 4);
	ltex_getpixels(mTexture, pixels.data());

	// Scale pixels buffer
	Vec2 size = getSize();
	mPixels.resize(static_cast<size_t>(size.x) * static_cast<size_t>(size.y) * 4);

	// Copy scaled pixels
	stbir_resize_uint8(
		pixels.data(), mTexture->width, mTexture->height, 0,
		mPixels.data(), static_cast<int>(size.x), static_cast<int>(size.y), 0, 4
	);
}