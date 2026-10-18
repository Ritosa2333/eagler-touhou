#include "../th08_web/cpp/game/AnmLibrary.hpp"
#include <cstdio>
#include <cstdlib>
using namespace th08;
namespace {
u32 checks=0;
bool same(AnmLibrary& original,TextureStore& source,AnmLibrary& cached,TextureStore& target){
    const auto* a=original.get(20);const auto* b=cached.get(20);if(!a||!b)return false;
    if(a->spriteCount!=b->spriteCount||a->scriptCount!=b->scriptCount||a->totalEntries!=b->totalEntries||a->numberEntriesToBeLoaded!=b->numberEntriesToBeLoaded)return false;
    for(u32 i=0;i<a->spriteCount;++i){auto x=a->sprites[i],y=b->sprites[i];x.texture=y.texture=0;if(std::memcmp(&x,&y,sizeof(x)))return false;++checks;}
    for(u32 i=0;i<a->scriptCount;++i){if(reinterpret_cast<const u8*>(a->scripts[i])-static_cast<const u8*>(a->rawData)!=reinterpret_cast<const u8*>(b->scripts[i])-static_cast<const u8*>(b->rawData))return false;++checks;}
    const auto* x=static_cast<const AnmTextureEntry*>(a->textures);const auto* y=static_cast<const AnmTextureEntry*>(b->textures);
    for(i32 i=0;i<a->totalEntries;++i){const auto* p=source.get(x[i].texture);const auto* q=target.get(y[i].texture);if(bool(p)!=bool(q))return false;if(p&&(p->image.format!=q->image.format||p->image.width!=q->image.width||p->image.height!=q->image.height||p->image.pixels!=q->image.pixels||p->priority!=q->priority||p->render_target!=q->render_target))return false;++checks;}
    return true;
}
std::vector<u8> read(const char* name){auto* f=std::fopen(name,"rb");if(!f)std::abort();std::fseek(f,0,SEEK_END);const auto size=std::ftell(f);std::rewind(f);std::vector<u8> b(size);if(std::fread(b.data(),1,size,f)!=size)std::abort();std::fclose(f);return b;}
}
int main(){
    arithmetic_mode(Precision::Single,Rounding::NearestEven);const char* names[]={"ref/title01.anm","ref/result00.anm","ref/resulttext.anm","ref/music00.anm"};
    std::vector<std::vector<u8>> files;for(const auto* name:names)files.push_back(read(name));
    for(bool reduced:{false,true}){
        TextureStore source,target;AnmLibrary original(source),cached(target);original.force_16bit=cached.force_16bit=reduced;
        for(const auto& bytes:files)if(!cached.preload(bytes.data(),bytes.size()))return 1;
        if(cached.preload_count()!=4)return 2;const u32 resident_count=target.live_count();if(!resident_count)return 7;
        for(const auto& bytes:files)for(u32 round=0;round<3;++round){
            const bool deferred=round==1;
            if(!original.load(20,bytes.data(),bytes.size(),deferred)||!cached.load(20,bytes.data(),bytes.size(),deferred)||!same(original,source,cached,target))return 3;
            while(cached.get(20)->numberEntriesToBeLoaded){if(!original.postload(20)||!cached.postload(20)||!same(original,source,cached,target))return 4;}
            auto* changed=cached.get(20);if(changed->spriteCount)changed->sprites[0].widthPx=-123;
            static_cast<u8*>(changed->rawData)[0]^=0xff;
            const auto* textures=static_cast<const AnmTextureEntry*>(changed->textures);for(i32 i=0;i<changed->totalEntries;++i){auto* texture=target.get(textures[i].texture);if(texture&&!texture->image.pixels.empty())texture->image.pixels[0]^=0x80;}
            // A second live instance cannot alias writable texture contents.
            if(!cached.load(21,bytes.data(),bytes.size()))return 8;
            const auto* other=static_cast<const AnmTextureEntry*>(cached.get(21)->textures);
            for(i32 i=0;i<changed->totalEntries;++i)if(textures[i].texture==other[i].texture)return 9;
            cached.release(21);
            cached.release(20);original.release(20);if(source.live_count()||target.live_count()!=resident_count)return 5;
        }
        if(cached.preload_hits()!=24||!cached.resident_hits())return 6;
        cached.clear_preloads();if(target.live_count())return 10;
    }
    std::printf("{\"passed\":true,\"comparisons\":%u,\"formats\":2,\"animations\":4,\"loads\":24}\n",checks);return 0;
}
