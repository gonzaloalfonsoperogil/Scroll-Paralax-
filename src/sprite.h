#pragma once

#include "collider.h"
#include "vec2.h"
#include <litegfx.h>
#include <array>
#include <vector>

class Sprite {
public:
	enum CollisionType {
		COLLISION_NONE,
		COLLISION_CIRCLE,
		COLLISION_RECT,
		COLLISION_PIXELS
	};

	typedef void(*CallbackFunc)(Sprite&, float);

	// Indicamos el número de frames en horizontal y vertical
	// que tendrá la imagen del sprite
	Sprite(const ltex_t* tex, int hframes = 1, int vframes = 1) :
		mCallback(nullptr),
		mHframes(hframes),
		mVframes(vframes),
		mTexture(nullptr),
		mBlend(BLEND_ALPHA),
		mColor{1,1,1,1},
		mPosition(0, 0),
		mAngle(0),
		mScale(1, 1),
		mPivot(0.5f, 0.5f),
		mFps(0),
		mCurrentFrame(0),
		mCollisionType(COLLISION_NONE),
		mCollider(nullptr) { setTexture(tex); updateCollisionInfo(); }

	~Sprite() { delete mCollider; }

	const ltex_t* getTexture() const { return mTexture; }
	void setTexture(const ltex_t* tex);

	lblend_t getBlend() const { return mBlend; }
	void setBlend(lblend_t mode) { mBlend = mode; }

	float getRed() const { return mColor[0]; }
	float getGreen() const { return mColor[1]; }
	float getBlue() const { return mColor[2]; }
	float getAlpha() const { return mColor[3]; }
	void setColor(float r, float g, float b, float a) { mColor[0] = r; mColor[1] = g; mColor[2] = b; mColor[3] = a; }

	const Vec2& getPosition() const { return mPosition; }
	void setPosition(const Vec2& pos) { mPosition = pos; updateCollisionInfo(); }

	float getAngle() const { return mAngle; }
	void setAngle(float angle) { mAngle = angle; }

	const Vec2& getScale() const { return mScale; }
	void setScale(const Vec2& scale);

	// Tamaño de un frame multiplicado por la escala
	Vec2 getSize() const { return Vec2(static_cast<float>(mTexture->width / mHframes), static_cast<float>(mTexture->height / mVframes)) * mScale; }

	// Este valor se pasa a ltex_drawrotsized en el pintado
	// para indicar el pivote de rotación
	const Vec2& getPivot() const { return mPivot; }
	void setPivot(const Vec2& pivot) { mPivot = pivot; updateCollisionInfo(); }

	int getHframes() const { return mHframes; }
	int getVframes() const { return mVframes; }
	void setFrames(int hframes, int vframes) { mHframes = hframes; mVframes = vframes; updateCollisionInfo(); }

	// Veces por segundo que se cambia el frame de animación
	int getFps() const { return mFps; }
	void setFps(int fps) { mFps = fps; }

	// Frame actual de animación
	float getCurrentFrame() const { return mCurrentFrame; }
	void setCurrentFrame(int frame) { mCurrentFrame = static_cast<float>(frame); }

	void update(float deltaTime);
	void draw() const;

	CallbackFunc getCallback() { return mCallback; }
	void setCallback(CallbackFunc callback) { mCallback = callback; }

	void* getUserData() { return mUserData; }
	void setUserData(void* data) { mUserData = data; }

	void             setCollisionType(CollisionType type);
	CollisionType    getCollisionType() const { return mCollisionType; }
	const Collider*  getCollider() const { return mCollider; }
	bool             collides(const Sprite& other) const;
protected:
	void			updateCollisionInfo();
	void			updateCollisionPixels();
private:
	const ltex_t*			mTexture;
	int						mHframes;
	int						mVframes;
	lblend_t				mBlend;
	std::array<float, 4>	mColor;
	Vec2					mPosition;
	float					mAngle;
	Vec2					mScale;
	Vec2					mPivot;
	int						mFps;
	float					mCurrentFrame;
	CallbackFunc			mCallback;
	void*					mUserData;

	// Collision
	CollisionType			mCollisionType;
	Collider*				mCollider;
	Vec2					mCircleCenter;
	float					mCircleRadius;
	Vec2					mRectOrigin;
	Vec2					mRectSize;
	std::vector<uint8_t>	mPixels;
};