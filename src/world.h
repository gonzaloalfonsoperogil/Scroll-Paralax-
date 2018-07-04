#pragma once

#include "sprite.h"
#include <array>
#include <vector>

class World {
public:
  World(
    float clearRed = 0.15f,
    float clearGreen = 0.15f,
    float clearBlue = 0.15f,
    const ltex_t* back0 = nullptr,
    const ltex_t* back1 = nullptr,
    const ltex_t* back2 = nullptr,
    const ltex_t* back3 = nullptr);
  
  float getClearRed() const                                 { return mClearColor[0]; }
  float getClearGreen() const                               { return mClearColor[1]; }
  float getClearBlue() const                                { return mClearColor[2]; }

  const ltex_t* getBackground(size_t index) const           { return mBackgrounds[index]; }

  float getScrollRatio(size_t layer) const                  { return mScrollRatios[layer]; }
  void setScrollRatio(size_t layer, float ratio)            { mScrollRatios[layer] = ratio; }

  const Vec2& getScrollSpeed(size_t layer) const            { return mScrollSpeeds[layer]; }
  void setScrollSpeed(size_t layer, const Vec2& speed)      { mScrollSpeeds[layer] = speed; }

  const Vec2& getCameraPosition() const                     { return mCamPos; }
  void setCameraPosition(const Vec2& pos)                   { mCamPos = pos; }

  void addSprite(Sprite& sprite)                            { mSprites.push_back(&sprite); }
  void removeSprite(Sprite& sprite)                         { mSprites.erase(std::find(mSprites.begin(), mSprites.end(), &sprite)); }

  void update(float deltaTime);
  void draw(const Vec2& screenSize);
private:
  std::array<float, 3>          mClearColor;
  std::array<const ltex_t*, 4>  mBackgrounds;
  std::array<float, 4>          mScrollRatios;
  std::array<Vec2, 4>           mScrollSpeeds;
  std::array<Vec2, 4>           mScrollMovements;
  Vec2                          mCamPos;
  std::vector<Sprite*>          mSprites;
};
