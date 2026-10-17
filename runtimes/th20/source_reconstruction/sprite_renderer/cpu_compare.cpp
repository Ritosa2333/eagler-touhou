// Test-only hash-gated original code and COM ABI spies, never linked into game.
#define wmain unused_native_oracle_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "sprite.hpp"
#include "animation.hpp"
#include "binding.hpp"
#include "vertex_buffer.hpp"
#include <algorithm>
#include <memory>
namespace s=th20::source::sprite;
namespace {
struct FakeCom { void** vtable; std::uint32_t id; };
void* device_vtable[119]{};
void* texture_vtable[22]{};
void* vertex_vtable[14]{};
FakeCom fake_device{device_vtable,1},fake_texture{texture_vtable,2};
FakeCom fake_vertex_buffer{vertex_vtable,3};
std::array<std::uint8_t,720> locked_vertices;
bool lock_has_storage=true;
HRESULT lock_status=S_OK;
s::Controller* current_controller=nullptr;
std::vector<std::vector<std::uint32_t>> calls;
HRESULT create_result=S_OK;
std::uint32_t normalized(const void* pointer) {
    const auto p=reinterpret_cast<std::uintptr_t>(pointer);
    const auto c=reinterpret_cast<std::uintptr_t>(current_controller);
    if(c&&p>=c&&p<c+sizeof(s::Controller)) return std::uint32_t(p-c);
    return std::uint32_t(p);
}
HRESULT WINAPI set_fvf(FakeCom*,DWORD fvf) { calls.push_back({89,fvf});return S_OK; }
HRESULT WINAPI draw(FakeCom*,D3DPRIMITIVETYPE type,UINT count,const void* vertices,UINT stride) {
    calls.push_back({83,std::uint32_t(type),count,normalized(vertices),stride});return S_OK;
}
HRESULT WINAPI create(FakeCom*,UINT width,UINT height,UINT levels,DWORD usage,D3DFORMAT format,D3DPOOL pool,IDirect3DTexture9** output,HANDLE* shared) {
    calls.push_back({23,width,height,levels,usage,std::uint32_t(format),std::uint32_t(pool),shared?1u:0u});
    if(create_result==S_OK)*output=reinterpret_cast<IDirect3DTexture9*>(&fake_texture);
    return create_result;
}
ULONG WINAPI release(FakeCom* texture) { calls.push_back({2,texture->id});return 1; }
HRESULT WINAPI create_vertices(FakeCom*,UINT length,DWORD usage,DWORD fvf,D3DPOOL pool,IDirect3DVertexBuffer9** output,HANDLE* shared){
    calls.push_back({26,length,usage,fvf,std::uint32_t(pool),normalized(output),shared?1u:0u});
    *output=reinterpret_cast<IDirect3DVertexBuffer9*>(&fake_vertex_buffer);return create_result;
}
HRESULT WINAPI lock_vertices(FakeCom*,UINT offset,UINT size,void** output,DWORD flags){
    calls.push_back({11,offset,size,flags});*output=lock_has_storage?locked_vertices.data():nullptr;return lock_status;
}
HRESULT WINAPI unlock_vertices(FakeCom*){calls.push_back({12});return lock_status;}
HRESULT WINAPI set_stream(FakeCom*,UINT stream,IDirect3DVertexBuffer9* buffer,UINT offset,UINT stride){
    calls.push_back({100,stream,reinterpret_cast<FakeCom*>(buffer)->id,offset,stride});return create_result;
}
template<class Return,class... Args> Return original(std::uint32_t va,void* self,Args... args) {
    using Function=Return(__thiscall*)(void*,Args...);
    return reinterpret_cast<Function>(mapped_image_base+va-0x400000)(self,args...);
}
std::vector<std::uint32_t> buffer_state(const s::Controller& c) {
    return {c.quad_count,c.colored_primitive_count,c.draw_calls,normalized(c.textured_write),
        normalized(c.textured_batch_start),normalized(c.colored_write),normalized(c.colored_batch_start)};
}
}
int wmain(int argc,wchar_t** argv) {
    try {
        if(argc!=3)throw std::runtime_error("Usage: th20_sprite_cpu_compare VERIFIED_TH20.exe OUTPUT.json");
        const std::filesystem::path source(argv[1]),report(argv[2]);
        if(std::filesystem::weakly_canonical(source)==std::filesystem::weakly_canonical(report))throw std::runtime_error("Report cannot replace original EXE");
        auto bytes=th20::read_file(source);const auto digest=sha256(bytes);
        if(digest!=expected_sha)throw std::runtime_error("Original SHA-256 mismatch");
        auto pe=th20::parse_pe(bytes);Mapping image(bytes,pe);mapped_image_base=image.address();
        device_vtable[89]=reinterpret_cast<void*>(&set_fvf);device_vtable[83]=reinterpret_cast<void*>(&draw);device_vtable[23]=reinterpret_cast<void*>(&create);
        texture_vtable[2]=reinterpret_cast<void*>(&release);
        device_vtable[26]=reinterpret_cast<void*>(&create_vertices);device_vtable[100]=reinterpret_cast<void*>(&set_stream);
        vertex_vtable[11]=reinterpret_cast<void*>(&lock_vertices);vertex_vtable[12]=reinterpret_cast<void*>(&unlock_vertices);
        *reinterpret_cast<void**>(mapped_image_base+0x1c4d48)=&fake_device;
        auto& device=*reinterpret_cast<IDirect3DDevice9*>(&fake_device);
        std::map<std::string,std::size_t> counts;std::vector<std::string> failures;std::size_t failed=0;
        auto check=[&](const char* label,bool equal){++counts[label];if(!equal){++failed;if(failures.size()<20)failures.push_back(std::string(label)+" case "+std::to_string(counts[label]));}};
        std::mt19937 random(0x4455c0);
        for(unsigned i=0;i<1000;++i) {
            s::PooledAnimation x,y;
            for(auto* p=reinterpret_cast<unsigned char*>(&x);p!=reinterpret_cast<unsigned char*>(&x)+sizeof(x);++p)*p=static_cast<unsigned char>(random());
            y=x;
            auto* ret=original<void*>(0x448d70,&x.animation.base);s::construct_animation_base(y.animation.base);
            check("animation_base_all_bytes",ret==&x.animation.base&&std::memcmp(&x,&y,sizeof(x))==0);
            ret=original<void*>(0x448b40,&x.animation);s::construct_animation(y.animation);
            auto equivalent=[&]{auto normalized_y=y;for(auto& link:normalized_y.animation.links)if(link.value==&y.animation)link.value=&x.animation;return std::memcmp(&x,&normalized_y,sizeof(x))==0;};
            check("animation_all_bytes_and_self_links",ret==&x.animation&&equivalent());
            ret=original<void*>(0x4489f0,&x);s::construct_pooled_animation(y);
            check("pooled_animation_all_bytes_and_padding",ret==&x&&equivalent());
            // Reset arbitrary bytes too, not merely constructor-produced zeros.
            for(auto* p=reinterpret_cast<unsigned char*>(&x);p!=reinterpret_cast<unsigned char*>(&x)+sizeof(x);++p)*p=static_cast<unsigned char>(random());
            y=x;
            original<void>(0x4299d0,&x.animation);s::reset_animation_state(y.animation);
            check("animation_reset_all_1536_bytes",std::memcmp(&x,&y,sizeof(x))==0);
            original<void>(0x429e30,&x.animation);s::clear_animation_suffix(y.animation);
            check("animation_clear_suffix",std::memcmp(&x,&y,sizeof(x))==0);
        }
        for(unsigned i=0;i<300;++i) {
            s::Vertex28 a,b;for(auto* p=reinterpret_cast<unsigned char*>(&a);p!=reinterpret_cast<unsigned char*>(&a)+sizeof(a);++p)*p=static_cast<unsigned char>(random());b=a;
            auto* ret=original<void*>(0x449170,&a);s::initialize_textured_vertex(b);check("textured_vertex_constructor",ret==&a&&std::memcmp(&a,&b,sizeof(a))==0);
            s::Vertex20 x,y;std::memcpy(&x,&a,sizeof(x));y=x;
            ret=original<void*>(0x449140,&x);s::initialize_colored_vertex(y);check("colored_vertex_constructor",ret==&x&&std::memcmp(&x,&y,sizeof(x))==0);
            s::TexturedCorner20 cx,cy;std::memcpy(&cx,&a,sizeof(cx));cy=cx;
            ret=original<void*>(0x449110,&cx);s::initialize_corner(cy);check("corner_constructor",ret==&cx&&std::memcmp(&cx,&cy,sizeof(cx))==0);
        }
        for(unsigned i=0;i<3000;++i) {
            s::Animation x,y,source_animation;
            for(auto* p=reinterpret_cast<unsigned char*>(&x);p!=reinterpret_cast<unsigned char*>(&x)+sizeof(x);++p)*p=static_cast<unsigned char>(random());y=x;
            for(auto* p=reinterpret_cast<unsigned char*>(&source_animation);p!=reinterpret_cast<unsigned char*>(&source_animation)+sizeof(source_animation);++p)*p=static_cast<unsigned char>(random());
            original<void>(0x4371f0,&x,&source_animation);s::copy_animation_base(y,source_animation);
            check("copy_only_animation_base",std::memcmp(&x,&y,sizeof(x))==0);
            s::AnimationFile file{};file.templates=&source_animation;
            original<void>(0x438b70,&file,&x,0);s::select_animation_template(file,y,0);
            check("select_template_and_timers_all_bytes",std::memcmp(&x,&y,sizeof(x))==0);
            const auto layer=i%2?th20::recovered::signed_bits(random()):std::int32_t(i%70)-5;
            original<void>(0x450350,&x,layer);s::set_animation_layer(y,layer);
            check("layer_and_flags_signed_ranges",std::memcmp(&x,&y,sizeof(x))==0);
            s::SpriteData sprite;
            for(auto* p=reinterpret_cast<unsigned char*>(&sprite);p!=reinterpret_cast<unsigned char*>(&sprite)+sizeof(sprite);++p)*p=static_cast<unsigned char>(random());
            file.bytes=i%5?reinterpret_cast<std::uint8_t*>(0x1000):nullptr;file.sprites=&sprite;
            FloatingEnvironment::prepare();const auto ra=original<std::int32_t>(0x438620,&file,&x,0);
            FloatingEnvironment::prepare();const auto rb=s::assign_animation_sprite(file,y,0);
            check("assign_sprite_all_bytes_and_return",ra==rb&&std::memcmp(&x,&y,sizeof(x))==0);
            s::AnmInstruction* script_pointer=i%3?reinterpret_cast<s::AnmInstruction*>(0x1000):nullptr;
            file.scripts=&script_pointer;file.id=random();
            const auto ia=original<int>(0x438940,&file,&x,0),ib=s::initialize_animation_script(file,y,0);
            check("initialize_script_or_null_all_bytes",ia==ib&&std::memcmp(&x,&y,sizeof(x))==0);
            s::SpriteData result_a{},result_b{};
            file.sprites=&result_a;FloatingEnvironment::prepare();original<void>(0x4506f0,&file,0,&sprite);
            file.sprites=&result_b;FloatingEnvironment::prepare();s::store_sprite_descriptor(file,0,sprite);
            check("sprite_descriptor_uv_extent_all_bytes",std::memcmp(&result_a,&result_b,sizeof(result_a))==0);
        }
        // Full original-size storage belongs only to the test fixtures. This is
        // not a substitute for the still-recovering real Controller constructor.
        auto a=std::make_unique<s::Controller>(),b=std::make_unique<s::Controller>();
        for(unsigned test=0;test<1024;++test){
            for(auto& byte:locked_vertices)byte=static_cast<std::uint8_t>(random());const auto before=locked_vertices;
            for(auto* p=reinterpret_cast<std::uint8_t*>(s::world_quad);p!=reinterpret_cast<std::uint8_t*>(s::world_quad)+sizeof(s::world_quad);++p)*p=static_cast<std::uint8_t>(random());
            std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1aef80),s::world_quad,sizeof(s::world_quad));
            lock_has_storage=(test%3)!=0;lock_status=(test%5)?S_OK:E_FAIL;create_result=(test%7)?S_OK:S_FALSE;
            current_controller=a.get();calls.clear();FloatingEnvironment::prepare();original<void>(0x44d020,a.get());const auto trace=calls;const auto after_vertices=locked_vertices;
            locked_vertices=before;current_controller=b.get();calls.clear();FloatingEnvironment::prepare();s::initialize_corner_buffer(*b,device);
            check("corner_buffer_9_quads_calls_bytes_and_global_colors",trace==calls&&after_vertices==locked_vertices&&a->corner_buffer==b->corner_buffer&&std::memcmp(a->corners,b->corners,sizeof(a->corners))==0&&std::memcmp(reinterpret_cast<void*>(mapped_image_base+0x1aef80),s::world_quad,sizeof(s::world_quad))==0);
        }
        for(unsigned i=0;i<1000;++i) {
            a->quad_count=b->quad_count=random();a->draw_calls=b->draw_calls=random();
            a->colored_primitive_count=b->colored_primitive_count=random();
            current_controller=a.get();original<void>(0x445a40,a.get());const auto state_a=buffer_state(*a);
            current_controller=b.get();s::prepare_buffers(*b);check("prepare_buffers",state_a==buffer_state(*b));
            const auto start=random()%1000u,end=start+random()%1000u;
            a->textured_batch_start=a->textured_vertices+start;b->textured_batch_start=b->textured_vertices+start;
            a->textured_write=a->textured_vertices+end;b->textured_write=b->textured_vertices+end;
            a->quad_count=b->quad_count=i%3?random():0;
            current_controller=a.get();calls.clear();original<void>(0x4455c0,a.get());const auto trace=calls;const auto after=buffer_state(*a);
            current_controller=b.get();calls.clear();s::flush_textured_quads(*b,device);check("flush_quads_com_trace_and_state",trace==calls&&after==buffer_state(*b));
        }
        for(UINT width:{0u,1u,1024u,0xffffffffu})for(UINT height:{0u,1u,4096u,0xfffffffeu})for(UINT format=0;format<9;++format)for(HRESULT status:{S_OK,S_FALSE,E_FAIL}) {
            create_result=status;s::TextureRecord x{},y{};x.flags=y.flags=random();
            calls.clear();const auto ra=original<std::uint32_t>(0x44c0b0,nullptr,&x,width,height,format);const auto trace=calls;
            calls.clear();const auto rb=s::create_dynamic_texture(y,device,width,height,format);
            check("dynamic_texture_result_state_com_arguments",ra==rb&&trace==calls&&std::memcmp(&x,&y,sizeof(x))==0);
            for(auto backbuffer:{D3DFMT_A8R8G8B8,D3DFMT_X8R8G8B8,D3DFMT_R5G6B5}) {
                *reinterpret_cast<D3DFORMAT*>(mapped_image_base+0x1c4e2c)=backbuffer;
                x.flags=y.flags=random();calls.clear();const auto rta=original<std::int32_t>(0x44c150,nullptr,&x,width,height);const auto rttrace=calls;
                calls.clear();const auto rtb=s::create_render_target(y,device,width,height,backbuffer);
                check("render_target_result_state_com_arguments",rta==rtb&&rttrace==calls&&std::memcmp(&x,&y,sizeof(x))==0);
            }
        }
        for(unsigned test=0;test<300;++test) {
            s::TextureHeader headers[8]{};s::TextureRecord x[8]{},y[8]{};s::AnimationFile fa{},fb{};
            fa.texture_count=fb.texture_count=test%9;fa.textures=x;fb.textures=y;
            for(unsigned i=0;i<8;++i){headers[i].width=std::uint16_t(random());headers[i].height=std::uint16_t(random());headers[i].format=std::uint16_t(random()%9);
                x[i].header=y[i].header=&headers[i];x[i].flags=y[i].flags=random();
                x[i].texture=y[i].texture=(random()&1)?reinterpret_cast<IDirect3DTexture9*>(&fake_texture):nullptr;}
            const auto file_index=test%42;a->files[file_index]=&fa;b->files[file_index]=&fb;
            current_controller=a.get();calls.clear();original<void>(0x41dbe0,a.get());const auto trace=calls;
            current_controller=b.get();calls.clear();s::release_device_textures(*b);check("release_texture_table_state_com_trace",trace==calls&&std::memcmp(x,y,sizeof(x))==0);
            create_result=test%2?S_OK:E_FAIL;const auto backbuffer=test%2?D3DFMT_A8R8G8B8:D3DFMT_R5G6B5;
            *reinterpret_cast<D3DFORMAT*>(mapped_image_base+0x1c4e2c)=backbuffer;
            calls.clear();original<void>(0x41d9f0,a.get());const auto recreate_trace=calls;
            calls.clear();s::recreate_device_textures(*b,device,backbuffer);check("recreate_texture_table_state_com_trace",recreate_trace==calls&&std::memcmp(x,y,sizeof(x))==0);
            a->files[file_index]=b->files[file_index]=nullptr;
        }
        std::size_t total=0;for(auto& row:counts)total+=row.second;
        std::ofstream out(report,std::ios::binary|std::ios::trunc);if(!out)throw std::runtime_error("Cannot create report");
        out<<"{\n  \"status\":\""<<(failed?"failed":"passed")<<"\",\n  \"source_sha256\":\""<<digest
           <<"\",\n  \"buffers_cpp_sha256\":\""<<TH20_BUFFERS_SHA<<"\",\n  \"sprite_hpp_sha256\":\""<<TH20_SPRITE_SHA
           <<"\",\n  \"animation_cpp_sha256\":\""<<TH20_ANIMATION_CPP_SHA<<"\",\n  \"animation_hpp_sha256\":\""<<TH20_ANIMATION_HPP_SHA
           <<"\",\n  \"binding_cpp_sha256\":\""<<TH20_BINDING_CPP_SHA<<"\",\n  \"binding_hpp_sha256\":\""<<TH20_BINDING_HPP_SHA
           <<"\",\n  \"vertex_buffer_cpp_sha256\":\""<<TH20_VERTEX_BUFFER_SHA
           <<"\",\n  \"total\":"<<total<<",\n  \"failed\":"<<failed
           <<",\n  \"scope\":\"Original CPU routines vs new C++; complete vertex/texture record bytes, selected controller buffer fields, ordered COM calls and argument words\",\n"
           <<"  \"limitations\":[\"COM spies compare ABI interactions, not GPU-rendered pixels\",\"Controller full construction, ANM VM/pool and file loading are outside this test report\",\"Texture format outside original 0..8 table is rejected by source and not compared\"],\n  \"comparisons\":{";
        bool first=true;for(auto& row:counts){if(!first)out<<',';first=false;out<<'\n'<<th20::json_string(row.first)<<':'<<row.second;}
        out<<"\n  },\n  \"failure_examples\":[";first=true;for(auto& error:failures){if(!first)out<<',';first=false;out<<th20::json_string(error);}out<<"]\n}\n";
        if(!out)throw std::runtime_error("Cannot finish report");
        std::cout<<"Sprite comparisons: "<<total<<"; failed: "<<failed<<'\n';for(auto& error:failures)std::cout<<error<<'\n';return failed?1:0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 2;}
}
