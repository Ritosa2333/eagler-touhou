#include "EnemyNameAtlas.hpp"
#include "TextureResample.hpp"
namespace th08 {
i32 EnemyNameAtlas::select(i32 stage,bool practice,i32 spell){
    switch(stage){
    case 1:return 17;case 2:return 18;
    case 3:if(!practice||spell==214)return 19;return spell>=216&&spell<=221?spell-190:-1;
    case 4:return 20;
    case 5:return !practice||spell==212?21:22;
    case 6:return 23;
    case 7:return !practice||(spell>=147&&spell<=150)?23:24;
    case 8:return !practice||(spell>=191&&spell<=193)||spell==213?32:25;
    default:return 16;
    }
}
bool EnemyNameAtlas::copy(AnmLoaded& front,i32 index){
    if(front.totalEntries<2||!front.textures||index<0||u32(index)>=front.spriteCount||front.spriteCount<=10)return false;
    auto* entries=static_cast<AnmTextureEntry*>(front.textures);auto* dest=textures.get(entries[0].texture);const auto* source=textures.get(entries[1].texture);if(!dest||!source)return false;
    const auto rect=[](const AnmLoadedSprite& s){return TextureRect{Scalar::truncate(s.startPixelInclusive.x),Scalar::truncate(s.startPixelInclusive.y),Scalar::truncate(s.endPixelInclusive.x),Scalar::truncate(s.endPixelInclusive.y)};};
    const auto to=rect(front.sprites[10]),from=rect(front.sprites[index]);
    auto surface=[](TexturePixels& image){return PixelSurface{image.format,image.width,image.height,image.width*TexturePixels::describe(image.format).bytes,image.pixels.data()};};
    auto output=surface(dest->image);auto input=surface(const_cast<TexturePixels&>(source->image));renderer.flush();
    // D3DX_DEFAULT is triangle filtering; equal-sized name rectangles take its
    // exact copy path. No alpha blending is performed on the name atlas.
    if(TextureResample::triangle(output,to,input,from,true,true,false))return false;textures.changed(entries[0].texture);return true;
}
}
