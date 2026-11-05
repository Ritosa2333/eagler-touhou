#pragma once
#include "AnmLibrary.hpp"
#include "AnmRenderer.hpp"
namespace th08 {
class EnemyNameAtlas {
public:
    EnemyNameAtlas(TextureStore& textures,AnmRenderer& renderer):textures(textures),renderer(renderer){}
    bool copy(AnmLoaded& front,i32 sprite);
    // A negative result preserves the current name (unused Last Word slots).
    static i32 select(i32 stage,bool spell_practice,i32 spell);
private:
    TextureStore& textures;AnmRenderer& renderer;
};
}
