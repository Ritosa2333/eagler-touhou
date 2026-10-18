// Only this oracle maps verified original instructions; production consists of
// texture_edges.cpp and uses ordinary Direct3D COM calls.
#define wmain unused_native_oracle_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../../sprite_renderer/texture_edges.hpp"
#include <algorithm>
namespace s=th20::source::sprite;
namespace d=s::texture_edge_detail;
namespace {
void* surface_vtable[17]{},*texture_vtable[22]{};
struct Surface {void** vtable=surface_vtable;D3DSURFACE_DESC description{};D3DLOCKED_RECT locked{};};
struct Texture {void** vtable=texture_vtable;Surface* surface;};
std::vector<std::uint32_t> calls;
HRESULT result=S_OK;
HRESULT WINAPI get_surface(Texture* texture,UINT level,IDirect3DSurface9** output) {
    calls.push_back(18);calls.push_back(level);*output=reinterpret_cast<IDirect3DSurface9*>(texture->surface);return result;
}
HRESULT WINAPI get_description(Surface* surface,D3DSURFACE_DESC* output) {calls.push_back(12);*output=surface->description;return result;}
HRESULT WINAPI lock_surface(Surface* surface,D3DLOCKED_RECT* output,const RECT* rectangle,DWORD flags) {
    calls.push_back(13);calls.push_back(rectangle?1:0);calls.push_back(flags);*output=surface->locked;return result;
}
HRESULT WINAPI unlock_surface(Surface*) {calls.push_back(14);return result;}
ULONG WINAPI release_surface(Surface*) {calls.push_back(2);return 1;}
}
int wmain(int argc,wchar_t** argv) {
    try {
        if(argc!=3)throw std::runtime_error("Usage: th20_texture_edges_cpu_compare VERIFIED_TH20.exe OUTPUT.json");
        const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Specimen hash mismatch");
        Mapping mapping(bytes,th20::parse_pe(bytes));mapped_image_base=mapping.address();
        surface_vtable[12]=reinterpret_cast<void*>(get_description);surface_vtable[13]=reinterpret_cast<void*>(lock_surface);
        surface_vtable[14]=reinterpret_cast<void*>(unlock_surface);surface_vtable[2]=reinterpret_cast<void*>(release_surface);
        texture_vtable[18]=reinterpret_cast<void*>(get_surface);
        std::mt19937 random(0x451910);std::uint32_t passed=0,failed=0;
        std::map<std::string,unsigned> counts;
        auto check=[&](const std::string& label,bool success) {
            ++counts[label];if(success)++passed;else {if(failed<12)std::cerr<<label<<" mismatch "<<counts[label]<<'\n';++failed;}
        };
        using Original16=void(__cdecl*)(std::uint32_t*,const std::uint16_t*,std::uint32_t*);
        using Source16=void(*)(std::uint32_t*,const std::uint16_t*,std::uint32_t&);
        const struct {std::uint32_t va;Source16 source;const char* name;} helpers[]={
            {0x451640,d::accumulate_argb1555,"A1R5G5B5_helper"},{0x451700,d::accumulate_argb4444,"A4R4G4B4_helper"},
            {0x4517c0,d::accumulate_argb8332,"A8R3G3B2_helper"}};
        for(auto helper:helpers) for(std::uint32_t value=0;value<65536;++value) {
            const auto pixel=static_cast<std::uint16_t>(value);std::uint32_t original[3]={random(),random(),random()},native[3];
            std::memcpy(native,original,sizeof(native));std::uint32_t a=random(),b=a;
            auto original_call=reinterpret_cast<Original16>(mapped_image_base+helper.va-0x400000);
            original_call(original,&pixel,&a);helper.source(native,&pixel,b);
            check(helper.name,a==b&&std::memcmp(original,native,sizeof(native))==0);
        }
        using Original32=void(__cdecl*)(std::uint32_t*,const std::uint8_t*,std::uint32_t*);
        const auto original32=reinterpret_cast<Original32>(mapped_image_base+0x51880);
        for(unsigned i=0;i<20000;++i) {
            std::uint32_t pixel=random();if(i%4==0)pixel&=0xffffff;
            std::uint32_t original[3]={random(),random(),random()},native[3];std::memcpy(native,original,sizeof(native));
            std::uint32_t a=random(),b=a;
            original32(original,reinterpret_cast<std::uint8_t*>(&pixel),&a);d::accumulate_argb8888(native,reinterpret_cast<std::uint8_t*>(&pixel),b);
            check("A8R8G8B8_helper",a==b&&std::memcmp(original,native,sizeof(native))==0);
        }
        for(auto helper:helpers) {std::uint32_t a=random(),b=a;reinterpret_cast<Original16>(mapped_image_base+helper.va-0x400000)(nullptr,nullptr,&a);helper.source(nullptr,nullptr,b);check("null_accumulator",a==b);}
        {std::uint32_t a=random(),b=a;original32(nullptr,nullptr,&a);d::accumulate_argb8888(nullptr,nullptr,b);check("null_accumulator",a==b);}
        using OriginalRepair=void(__thiscall*)(void*,IDirect3DTexture9*);
        const auto original_repair=reinterpret_cast<OriginalRepair>(mapped_image_base+0x51910);
        const D3DFORMAT formats[]={D3DFMT_UNKNOWN,D3DFMT_A8R8G8B8,D3DFMT_A1R5G5B5,D3DFMT_A4R4G4B4,D3DFMT_A8R3G3B2,
            D3DFMT_X8R8G8B8,D3DFMT_R5G6B5,D3DFMT_A8,D3DFMT_DXT5};
        for(auto format:formats) for(unsigned test=0;test<600;++test) {
            const bool full32=format==D3DFMT_UNKNOWN || format==D3DFMT_A8R8G8B8;
            const int pixel_bytes=full32?4:2;
            const UINT width=test%31==0?0:1+random()%31,height=test%29==0?0:1+random()%23;
            const int magnitude=static_cast<int>(width)*pixel_bytes+4+static_cast<int>(test%4);
            const int pitch=test%2?-magnitude:magnitude;
            const unsigned body=magnitude*std::max(height,1u);
            std::vector<std::uint8_t> original(body+128),native;
            for(auto& byte:original)byte=static_cast<std::uint8_t>(random());
            auto* base=original.data()+64+(pitch<0?magnitude*(std::max(height,1u)-1):0);
            const auto mask=full32?0xff000000u:format==D3DFMT_A1R5G5B5?0x8000u:format==D3DFMT_A4R4G4B4?0xf000u:0xff00u;
            for(UINT y=0;y<height;++y) for(UINT x=0;x<width;++x) {
                auto* pixel=base+pitch*static_cast<int>(y)+x*pixel_bytes;std::uint32_t value=0;std::memcpy(&value,pixel,pixel_bytes);
                const bool transparent=test%4==0 || (test%4==2&&((x+y)&1)) || (test%4==3&&random()%2);
                if(transparent)value&=~mask;else value|=mask;
                std::memcpy(pixel,&value,pixel_bytes);
            }
            native=original;
            Surface surface;surface.description.Format=format;surface.description.Width=width;surface.description.Height=height;
            surface.locked.Pitch=pitch;surface.locked.pBits=base;Texture texture{texture_vtable,&surface};
            auto& com_texture=*reinterpret_cast<IDirect3DTexture9*>(&texture);
            result=test%5==0?E_FAIL:S_OK; // specimen ignores HRESULT if pointers/fields are valid
            calls.clear();original_repair(nullptr,&com_texture);const auto expected_calls=calls;
            surface.locked.pBits=native.data()+(base-original.data());calls.clear();s::repair_transparent_texels(com_texture);
            check("surface_format_"+std::to_string(format),original==native&&expected_calls==calls);
        }
        for(HRESULT status:{S_OK,E_FAIL}) {
            Texture texture{texture_vtable,nullptr};auto& com_texture=*reinterpret_cast<IDirect3DTexture9*>(&texture);
            result=status;calls.clear();original_repair(nullptr,&com_texture);const auto expected_calls=calls;
            calls.clear();s::repair_transparent_texels(com_texture);check("missing_surface",expected_calls==calls);
        }
        std::ofstream out{std::filesystem::path(argv[2])};
        out<<"{\n  \"status\": \""<<(failed?"failed":"passed")<<"\",\n  \"specimen_sha256\": \""<<expected_sha<<"\",\n"
           <<"  \"passed\": "<<passed<<",\n  \"failed\": "<<failed<<",\n  \"counts\": {";
        bool first=true;for(auto& [name,count]:counts) {if(!first)out<<',';out<<"\n    \""<<name<<"\": "<<count;first=false;}
        out<<"\n  },\n  \"scope\": \"Three complete 16-bit helper domains, 20000 32-bit helper samples, null accumulators; all output bytes including guards/padding and COM method trace for four supported formats, UNKNOWN alias, four unsupported formats, positive/negative and odd pitches, zero/one dimensions, full/zero/mixed alpha, ignored HRESULT failures, null surfaces.\"\n}\n";
        std::cout<<passed<<" passed; "<<failed<<" failed\n";return failed?1:0;
    } catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 2;}
}
