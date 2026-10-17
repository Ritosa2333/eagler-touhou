#include <eagler/text/RuntimeFontRaster.hpp>
#include <array>
#include <cassert>
#include <cstdio>
#include <vector>
struct Image {unsigned width=256,height=96,pitch=512;std::vector<std::uint8_t> pixels=std::vector<std::uint8_t>(512*96);};
int main(){
    std::array<std::uint8_t,131072> mapping{};
    auto set=[&](unsigned index,unsigned code){mapping[index*2]=code&255;mapping[index*2+1]=code>>8;};
    for(unsigned i=0;i<65536;++i)set(i,0x30fb);
    for(unsigned i=0;i<128;++i)set(i,i);set(0x82a0,0x3042);
    for(unsigned i=0;i<700;++i)set(0x8140+i,0x3000+i);
    eagler::text::RuntimeFontRaster raster{{32,400},{15,700}};
    assert(!raster.initialize(mapping.data(),mapping.size()-1));
    assert(raster.initialize(mapping.data(),mapping.size()));
    Image image;assert(!raster.draw(image,0,0,2,0xffffff,"A"));
    assert(raster.draw(image,0,0,0,0xffffff,"Title"));
    assert(raster.draw(image,0,36,1,0xffffff,std::string("\x82\xa0")));
    bool visible=false;for(auto pixel:image.pixels)visible|=pixel!=0;assert(visible);
    const auto before=image.pixels;const auto glyphs=raster.cachedGlyphs();
    std::fill(image.pixels.begin(),image.pixels.end(),0);
    assert(raster.draw(image,0,0,0,0xffffff,"Title"));assert(raster.draw(image,0,36,1,0xffffff,std::string("\x82\xa0")));
    assert(image.pixels==before&&raster.cachedGlyphs()==glyphs);
    for(unsigned i=0;i<700;++i){const unsigned code=0x8140+i;if(!raster.draw(image,-4,-4,0,0x1234ff,std::string{char(code>>8),char(code)})){std::fprintf(stderr,"Glyph probe failed at U+%04x: %s\n",0x3000+i,SDL_GetError());return 1;}}
    assert(raster.cachedGlyphs()<=512&&raster.cachedBytes()<=2*1024*1024);
    std::printf("Runtime fonts: CP932, Japanese glyphs, multiple sizes, repeatability, clipping and bounded cache PASS (%zu glyphs, %zu bytes)\n",raster.cachedGlyphs(),raster.cachedBytes());
}
