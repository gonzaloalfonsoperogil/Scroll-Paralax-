#include "stringutils.h"
#include "ugine.h"
#include "world.h"

World::World(
    float clearRed, float clearGreen, float clearBlue,
    const ltex_t* back0, const ltex_t* back1, const ltex_t* back2, const ltex_t* back3)
	: mScrollRatios{1, 1, 1, 1} {
    mClearColor[0] = clearRed;
    mClearColor[1] = clearGreen;
    mClearColor[2] = clearBlue;
    mBackgrounds[0] = back0;
    mBackgrounds[1] = back1;
    mBackgrounds[2] = back2;
    mBackgrounds[3] = back3;
}

void World::update(float deltaTime) {
  // update scroll
  for ( int i = mBackgrounds.size() - 1; i >= 0; --i ) {
    if ( mBackgrounds[i] ) mScrollMovements[i] = mScrollMovements[i] + mScrollSpeeds[i] * deltaTime;
  }

  // update sprites
  for ( Sprite* sprite : mSprites ) sprite->update(deltaTime);
}

void World::draw(const Vec2& screenSize) {
  // clear background
  lgfx_clearcolorbuffer(mClearColor[0], mClearColor[1], mClearColor[2]);

  // draw scrolling backgrounds
  lgfx_setblend(BLEND_ALPHA);
  lgfx_setcolor(1, 1, 1, 1);
  for ( int i = mBackgrounds.size() - 1; i >= 0; --i ) {
    if ( mBackgrounds[i] ) {
      Vec2 scrollPos = getCameraPosition() * mScrollRatios[i] - mScrollMovements[i];
      float u0 = scrollPos.x / mBackgrounds[i]->width;
      float v0 = scrollPos.y / mBackgrounds[i]->height;
      float u1 = (scrollPos.x + screenSize.x) / mBackgrounds[i]->width;
      float v1 = (scrollPos.y + screenSize.y) / mBackgrounds[i]->height;
      ltex_drawrotsized(mBackgrounds[i], 0, 0, 0, 0, 0, screenSize.x, screenSize.y, u0, v0, u1, v1);
    }
  }

  // set scroll for map and sprites
  lgfx_setorigin(mCamPos.x, mCamPos.y);

  // draw sprites
  for ( Sprite* sprite : mSprites ) sprite->draw();

  // reset origin
  lgfx_setorigin(0, 0);
}
