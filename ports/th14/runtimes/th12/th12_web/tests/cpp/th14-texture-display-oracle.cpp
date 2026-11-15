#include "TextureImage.hpp"
#include <algorithm>
#include <cstdio>
using namespace th11;

int main() {
    for (const auto display : {640u, 960u, 1280u}) {
        for (const bool flagged : {false, true}) {
            for (const bool odd : {false, true}) {
                AnmTexture source;
                source.width=16+odd; source.height=20+odd;
                source.pixel_width=12+odd; source.pixel_height=16+odd;
                source.format=source.pixel_format=1;
                source.half_resolution=flagged;
                const u32 row_bytes=source.pixel_width*4;
                source.pixels.resize(row_bytes*source.pixel_height);
                for(u32 i=0;i<source.pixels.size();++i)
                    source.pixels[i]=u8(i*73+29);

                TextureImage image;
                if (!image.load(source,false,false,display)) {
                    std::fprintf(stderr,"%s\n",image.failure.c_str()); return 1;
                }
                bool identical=image.width==source.width&&image.height==source.height;
                if(identical)for(u32 y=0;y<source.pixel_height;++y){
                    const auto start=source.pixels.begin()+y*row_bytes;
                    identical&=std::equal(start,start+row_bytes,image.pixels.begin()+y*image.pitch);
                }
                std::printf("%u %u %u %u %u %u\n",display,unsigned(flagged),unsigned(odd),
                    image.width,image.height,unsigned(identical));
            }
        }
    }
}
