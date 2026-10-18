// Hash-verified original resource-to-Direct3D routine versus reconstructed C++.
// The hidden test window, device and textures are isolated; WinMain never runs.
#define wmain unused_native_cpu_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "texture_load.hpp"
#include "anm_vm.hpp"
#include "../archive/archive.hpp"
#include <memory>
namespace s=th20::source::sprite;
namespace {
struct Device {
    HWND window=nullptr;IDirect3D9* d3d=nullptr;IDirect3DDevice9* device=nullptr;
    Device(){
        window=CreateWindowExW(0,L"STATIC",L"TH20 texture verification",WS_POPUP,0,0,64,64,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
        if(!window)throw std::runtime_error("Cannot create hidden texture-test window");
        d3d=Direct3DCreate9(D3D_SDK_VERSION);if(!d3d)throw std::runtime_error("Direct3D9 unavailable");
        D3DPRESENT_PARAMETERS parameters{};parameters.Windowed=TRUE;parameters.SwapEffect=D3DSWAPEFFECT_DISCARD;
        parameters.hDeviceWindow=window;parameters.BackBufferWidth=64;parameters.BackBufferHeight=64;parameters.BackBufferFormat=D3DFMT_UNKNOWN;
        const auto status=d3d->CreateDevice(0,D3DDEVTYPE_HAL,window,D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED|D3DCREATE_FPU_PRESERVE,&parameters,&device);
        if(FAILED(status))throw std::runtime_error("Cannot create real Direct3D9 HAL test device");
    }
    ~Device(){if(device)device->Release();if(d3d)d3d->Release();if(window)DestroyWindow(window);}
};
template<class R,class... A> R original(std::uint32_t va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
template<class T>T& global(std::uint32_t va){return *reinterpret_cast<T*>(mapped_image_base+va-0x400000);}
struct FileFixture {
    th20::source::Bytes bytes;
    s::AnimationFile file{};
    s::TextureRecord texture{};
    std::vector<s::SpriteData> sprites;
    std::vector<s::AnmInstruction*> scripts;
    FileFixture(const th20::source::Bytes& source,std::size_t offset,std::uint32_t id):bytes(source){
        auto& header=*reinterpret_cast<s::AnmHeader*>(bytes.data()+offset);
        sprites.resize(header.sprite_count);scripts.resize(header.script_count);file.id=id;file.bytes=bytes.data();file.filename="oracle.anm";
        file.textures=&texture;file.texture_count=1;file.sprites=sprites.data();file.scripts=scripts.data();
    }
    ~FileFixture(){if(texture.texture)texture.texture->Release();}
};
std::vector<std::uint8_t> pixels(IDirect3DTexture9& texture){
    D3DSURFACE_DESC description{};if(FAILED(texture.GetLevelDesc(0,&description)))throw std::runtime_error("Texture descriptor failed");
    if(description.Format!=D3DFMT_A8R8G8B8)throw std::runtime_error("Unexpected real texture format");
    D3DLOCKED_RECT locked{};if(FAILED(texture.LockRect(0,&locked,nullptr,D3DLOCK_READONLY)))throw std::runtime_error("Texture readback failed");
    std::vector<std::uint8_t> output(std::size_t(description.Width)*description.Height*4);
    for(UINT row=0;row<description.Height;++row)std::memcpy(output.data()+std::size_t(row)*description.Width*4,static_cast<const std::uint8_t*>(locked.pBits)+std::ptrdiff_t(row)*locked.Pitch,description.Width*4);
    texture.UnlockRect(0);return output;
}
}
int wmain(int argc,wchar_t** argv){
    try{
        if(argc!=4)throw std::runtime_error("Usage: texture_cpu_compare TH20.exe RAW_DIR REPORT.json");
        const auto exe=th20::read_file(argv[1]);if(sha256(exe)!=expected_sha)throw std::runtime_error("Original SHA256 mismatch");
        const auto pe=th20::parse_pe(exe);Mapping image(exe,pe);mapped_image_base=image.address();
        HMODULE d3dx=LoadLibraryW(L"d3dx9_43.dll");if(!d3dx)throw std::runtime_error("D3DX9 texture API unavailable");
        unsigned imports=0;for(const auto& entry:pe.imports)if(entry.name=="D3DXCreateTexture"||entry.name=="D3DXLoadSurfaceFromFileInMemory"){
            const auto function=GetProcAddress(d3dx,entry.name.c_str());if(!function)throw std::runtime_error("Required D3DX export missing");
            *reinterpret_cast<std::uintptr_t*>(mapped_image_base+entry.iat_rva)=reinterpret_cast<std::uintptr_t>(function);++imports;
        }
        if(imports!=2)throw std::runtime_error("Original D3DX imports not uniquely resolved");
        Device graphics;global<IDirect3DDevice9*>(0x5c4d48)=graphics.device;
        global<D3DFORMAT>(0x5c4e2c)=D3DFMT_A8R8G8B8;
        global<int>(0x5b87f8)=1280;global<int>(0x5b87fc)=960;
        FloatingEnvironment restore;th20::source::runtime::Log log;unsigned cases=0,failed=0,embedded=0,dynamic=0,render_targets=0,external=0;
        std::uint64_t compared_pixels=0;std::vector<std::string> failures;
        for(const auto& entry:std::filesystem::directory_iterator(argv[2]))if(entry.path().extension()==".anm"){
            const auto data=th20::read_file(entry.path());std::size_t offset=0;
            for(;;){
                const auto& header=*reinterpret_cast<const s::AnmHeader*>(data.data()+offset);
                const auto* name=reinterpret_cast<const char*>(data.data()+offset+header.name_offset);
                // External-image load gets a separate test with explicit image ownership.
                if(header.has_data||name[0]=='@')for(float scale:{1.f,1.5f,2.f}){
                    FileFixture a(data,offset,17),b(data,offset,17);
                    global<float>(0x5b8818)=scale;s::TextureContext context{*graphics.device,1280,960,D3DFMT_A8R8G8B8,scale};
                    auto* ha=reinterpret_cast<s::AnmHeader*>(a.bytes.data()+offset);auto* hb=reinterpret_cast<s::AnmHeader*>(b.bytes.data()+offset);
                    FloatingEnvironment::prepare();const auto ra=original<int>(0x44e350,nullptr,&a.file,0u,0,0,ha);
                    FloatingEnvironment::prepare();const auto rb=s::postload_animation_entry(b.file,0,0,0,hb,context,log);
                    bool ok=ra==rb&&a.file.fields_5c[2]==b.file.fields_5c[2]&&a.texture.flags==b.texture.flags&&a.texture.bytes_per_pixel==b.texture.bytes_per_pixel&&a.bytes==b.bytes;
                    ok=ok&&a.sprites.size()==b.sprites.size()&&std::memcmp(a.sprites.data(),b.sprites.data(),a.sprites.size()*sizeof(s::SpriteData))==0;
                    for(std::size_t i=0;i<a.scripts.size();++i)ok=ok&&(reinterpret_cast<const std::uint8_t*>(a.scripts[i])-a.bytes.data())==(reinterpret_cast<const std::uint8_t*>(b.scripts[i])-b.bytes.data());
                    D3DSURFACE_DESC da{},db{};a.texture.texture->GetLevelDesc(0,&da);b.texture.texture->GetLevelDesc(0,&db);ok=ok&&std::memcmp(&da,&db,sizeof(da))==0;
                    if(header.has_data){auto pa=pixels(*a.texture.texture),pb=pixels(*b.texture.texture);ok=ok&&pa==pb;compared_pixels+=pa.size()/4;++embedded;}
                    else if(name[1]=='R')++render_targets;else ++dynamic;
                    ++cases;if(!ok){++failed;if(failures.size()<16)failures.push_back(entry.path().filename().string()+"@"+std::to_string(offset)+" scale "+std::to_string(scale));}
                }
                if(!header.next_offset)break;offset+=header.next_offset;
            }
        }
        for(const auto& entry:std::filesystem::directory_iterator(argv[2]))if(entry.path().extension()==".png"){
            const auto data=th20::read_file(entry.path());
            for(float scale:{1.f,1.5f,2.f})for(bool low_resolution:{false,true})for(int inset:{0,1,8}){
                s::AnmHeader header{};header.padding_21[1]=low_resolution?1:0;
                struct OwnedTexture{s::TextureRecord value{};~OwnedTexture(){if(value.texture)value.texture->Release();}} a,b;
                a.value.unknown_04=b.value.unknown_04=reinterpret_cast<std::uint32_t>(data.data());a.value.unknown_08=b.value.unknown_08=static_cast<std::uint32_t>(data.size());
                a.value.header=b.value.header=reinterpret_cast<s::TextureHeader*>(&header);a.value.flags=b.value.flags=0x812346a7u;
                global<float>(0x5b8818)=scale;s::TextureContext context{*graphics.device,1280,960,D3DFMT_A8R8G8B8,scale};
                FloatingEnvironment::prepare();const auto ra=original<int>(0x44e8f0,nullptr,&a.value,5,0,32,32,inset,inset,32,32);
                FloatingEnvironment::prepare();const auto rb=s::create_external_texture(b.value,5,32,32,inset,inset,32,32,context);
                bool ok=ra==rb&&a.value.flags==b.value.flags&&a.value.bytes_per_pixel==b.value.bytes_per_pixel;
                if(ra>=0&&rb>=0){auto pa=pixels(*a.value.texture),pb=pixels(*b.value.texture);ok=ok&&pa==pb;compared_pixels+=pa.size()/4;}
                ++cases;++external;if(!ok){++failed;if(failures.size()<16)failures.push_back("External "+entry.path().filename().string());}
            }
        }
        FreeLibrary(d3dx);
        std::ofstream out(argv[3]);out<<"{\n  \"status\":\""<<(failed?"failed":"passed")<<"\",\n  \"cases\":"<<cases<<",\n  \"failed\":"<<failed<<",\n  \"embedded\":"<<embedded<<",\n  \"dynamic\":"<<dynamic<<",\n  \"render_targets\":"<<render_targets<<",\n  \"external_png_cases\":"<<external<<",\n  \"compared_pixels\":"<<compared_pixels<<",\n  \"postload_entry_cpp_sha256\":\""<<TH20_POSTLOAD_ENTRY_SHA<<"\",\n  \"texture_load_cpp_sha256\":\""<<TH20_TEXTURE_LOAD_SHA<<"\",\n  \"scope\":\"Original 44e350 and reconstructed C++ on real Direct3D9 HAL textures: all asset entries at 1/1.5/2 screen scales; all sprite bytes, script offsets, mutable file bytes, descriptors and embedded texture RGBA pixels; original44e8f0 external PNG crops, low-resolution scaling and transparent-edge pixels\",\n  \"limitations\":[\"No frame rendering or device-loss comparison\",\"Dynamic and render-target uninitialized pixel contents not compared\"],\n  \"failure_examples\":[";
        for(std::size_t i=0;i<failures.size();++i){if(i)out<<',';out<<'"'<<failures[i]<<'"';}out<<"]\n}\n";
        if(!out)throw std::runtime_error("Cannot write texture report");std::cout<<"Texture CPU/GPU: "<<cases<<" entries, "<<compared_pixels<<" pixels, "<<failed<<" failures\n";return failed?1:0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 2;}
}

