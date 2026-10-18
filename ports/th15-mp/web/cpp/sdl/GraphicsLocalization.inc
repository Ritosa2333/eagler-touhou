#if defined(TH_ENABLE_THCRAP)
namespace {
// TH10 AnimationResources::override_embedded uses thcrap's per-sprite "auto"
// replacement rule: opaque originals are blended with the PNG, while sprites
// with transparency are replaced (including transparent pixels). Keep all
// unmentioned atlas regions from the retail THTX bitmap.
bool patched_texture(const AnmResource& file,const AnmTexture& source,u32 index,AnmTexture& result){
    if(source.kind!=AnmTexture::Kind::Embedded||source.name.empty()||
       source.name.front()=='/'||source.name.find("..")!=std::string::npos||
       source.name.find(':')!=std::string::npos)return false;
    std::string name=source.name;
    for(char& c:name)if(c=='\\')c='/';
    // The original ASCII glyph atlas has fixed codepoint slots; translating
    // its PNG directly would invalidate those indices.
    if(name=="ascii/ascii.png"||name=="ascii/ascii_960.png"||name=="ascii/ascii_1280.png")return false;
    const std::string path="/thcrap/th15/"+name;
    size_t bytes=0;auto* input=static_cast<u8*>(SDL_LoadFile(path.c_str(),&bytes));
    if(!input)return false;
    int width=0,height=0,channels=0;
    auto* image=bytes<=32u*1024u*1024u?stbi_load_from_memory(input,int(bytes),&width,&height,&channels,4):nullptr;
    SDL_free(input);
    if(!image||width<=0||height<=0||width>16384||height>16384){stbi_image_free(image);return false;}
    std::vector<u8> original;
    if(!source.rgba(original)){stbi_image_free(image);return false;}
    const u32 atlas_width=source.pixel_width,atlas_height=source.pixel_height;
    // thcrap anm.cpp patches the complete entry for zero/one sprite. PNGs can
    // cover more than its lone displayed sprite (e.g. signature and logos).
    std::vector<AnmSprite> regions;
    for(const auto& sprite:file.sprites)if(sprite.texture==index)regions.push_back(sprite);
    if(regions.size()<=1){AnmSprite whole;whole.texture=index;whole.width=float(atlas_width);whole.height=float(atlas_height);regions={whole};}
    const auto authored=original;
    bool changed=false;
    for(const auto& sprite:regions){
        if(sprite.texture!=index)continue;
        const i32 left=i32(std::lround(sprite.x))%atlas_width,top=i32(std::lround(sprite.y))%atlas_height;
        const i32 patch_left=source.source_x+left,patch_top=source.source_y+top;
        const i32 sw=i32(std::lround(sprite.width)),sh=i32(std::lround(sprite.height));
        if(left<0||top<0||patch_left<0||patch_top<0||sw<=0||sh<=0||patch_left>=width||patch_top>=height||
           left>=i32(atlas_width)||top>=i32(atlas_height))continue;
        const i32 w=std::min({sw,width-patch_left,i32(atlas_width)-left});
        const i32 h=std::min({sh,height-patch_top,i32(atlas_height)-top});
        bool empty=true,opaque=true;
        for(i32 y=0;y<h;++y)for(i32 x=0;x<w;++x){
            const auto* patch=image+(size_t(patch_top+y)*width+patch_left+x)*4;
            const auto* base=authored.data()+(size_t(top+y)*atlas_width+left+x)*4;
            if(patch[3])empty=false;
            if(base[3]!=255)opaque=false;
        }
        if(empty)continue;
        for(i32 y=0;y<h;++y)for(i32 x=0;x<w;++x){
            const auto* patch=image+(size_t(patch_top+y)*width+patch_left+x)*4;
            auto* base=original.data()+(size_t(top+y)*atlas_width+left+x)*4;
            if(!opaque){std::copy(patch,patch+4,base);continue;}
            const i32 alpha=patch[3];if(!alpha)continue;
            for(int c=0;c<3;++c)base[c]=u8((base[c]*(255-alpha)+patch[c]*alpha)>>8);
            base[3]=alpha==255?u8(255):u8(std::min<i32>(base[3]+alpha,255));
        }
        changed=true;
    }
    stbi_image_free(image);
    if(!changed)return false;
    result=source;result.pixel_format=1;result.pixels.resize(original.size());
    for(size_t p=0;p<original.size();p+=4){
        result.pixels[p]=original[p+2];result.pixels[p+1]=original[p+1];
        result.pixels[p+2]=original[p];result.pixels[p+3]=original[p+3];
    }
    return true;
}
}
#endif
