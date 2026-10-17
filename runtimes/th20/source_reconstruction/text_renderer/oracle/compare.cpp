// Original mapping and render-boundary interception exist only in this oracle.
#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../text.hpp"
#include "../bitmap.hpp"
#include "../raster.hpp"
#include "../deferred_queue.hpp"
#include "../../program_entry/program_entry.hpp"
#include "../../sprite_renderer/pool.hpp"
#include "../../sprite_renderer/binding.hpp"
#include "../../sprite_renderer/anm_vm.hpp"
#include "../../sprite_renderer/quad.hpp"
#include "../../options_system/options.hpp"
#include "../../options_system/data.hpp"
#include "../../key_config/key_config.hpp"
#include "../../key_config/data.hpp"
#include "../menu_style.hpp"
namespace s=th20::source::sprite;namespace t=th20::source::text;namespace pe=th20::source::program_entry;
namespace {
s::Controller* controller=nullptr;float clock_value=1;
template<class R,class... A>R cpu(std::uint32_t va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
[[noreturn]]void unexpected(const char* name){throw std::runtime_error(std::string("Unexercised test dependency: ")+name);}
struct Draw {std::uint32_t kind;s::Animation animation;};std::vector<Draw> draws;
void __fastcall capture_axis(void*,void*,s::Animation* a){draws.push_back({0,*a});}
void __fastcall capture_rotated(void*,void*,s::Animation* a){draws.push_back({1,*a});}
void intercept(std::uint32_t va,void* function){auto* at=reinterpret_cast<std::uint8_t*>(mapped_image_base+va-0x400000);at[0]=0xe9;const auto delta=reinterpret_cast<std::uint32_t>(function)-reinterpret_cast<std::uint32_t>(at)-5;std::memcpy(at+1,&delta,4);FlushInstructionCache(GetCurrentProcess(),at,5);}
struct Surface {void** table;std::vector<std::uint8_t> pixels;RECT rectangle;unsigned locks=0,unlocks=0,releases=0;};
struct Texture {void** table;Surface* surface;unsigned gets=0;};
HRESULT WINAPI get_surface(Texture* texture,UINT level,IDirect3DSurface9** out){if(level)unexpected("surface level");++texture->gets;*out=reinterpret_cast<IDirect3DSurface9*>(texture->surface);return S_OK;}
HRESULT WINAPI lock_surface(Surface* surface,D3DLOCKED_RECT* out,const RECT* rectangle,DWORD flags){if(flags)unexpected("lock flags");surface->rectangle=*rectangle;++surface->locks;out->pBits=surface->pixels.data();out->Pitch=4352;return S_OK;}
HRESULT WINAPI unlock_surface(Surface* surface){++surface->unlocks;return S_OK;}
ULONG WINAPI release_surface(Surface* surface){return ++surface->releases;}
}
#include "test_environment.hpp"
namespace th20::source::program_entry {WindowStatePrefix window_state{};GraphicsStatePrefix graphics_state{};SpriteController* sprite_controller=nullptr;}
namespace th20::source::platform_window {HFONT fonts[22]{};std::uint8_t font_available[3]{};}
namespace th20::source::platform_window::unrecovered {void draw_frame_rate_text(float);}
namespace th20::source::text {Renderer* renderer=nullptr;}
namespace th20::source::sprite {
void draw_axis_aligned_sprite(Controller&,Animation& a,bool snap){if(snap)unexpected("snap");draws.push_back({0,a});}
void draw_rotated_sprite(Controller&,Animation& a){draws.push_back({1,a});}
}
int wmain(int argc,wchar_t** argv){try{
    AddVectoredExceptionHandler(1,[](PEXCEPTION_POINTERS p)->LONG{if(p->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION)std::cerr<<"access violation ip="<<std::hex<<p->ContextRecord->Eip<<" originalVA="<<(p->ContextRecord->Eip-mapped_image_base+0x400000)<<" address="<<p->ExceptionRecord->ExceptionInformation[1]<<" returnVA="<<(*reinterpret_cast<std::uint32_t*>(p->ContextRecord->Esp)-mapped_image_base+0x400000)<<std::endl;return EXCEPTION_CONTINUE_SEARCH;});
    if(argc!=3)throw std::runtime_error("Usage: th20_text_cpu_compare ORIGINAL.exe OUTPUT.json");
    const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");const auto pe_info=th20::parse_pe(bytes);Mapping image(bytes,pe_info);mapped_image_base=image.address();
    for(const auto& imported:pe_info.imports){FARPROC address=nullptr;for(const auto* library:{L"kernel32.dll",L"gdi32.dll",L"user32.dll"})if(auto module=GetModuleHandleW(library))if(auto entry=GetProcAddress(module,imported.name.c_str())){address=entry;break;}if(address)*reinterpret_cast<FARPROC*>(mapped_image_base+imported.iat_rva)=address;}
    *reinterpret_cast<HANDLE*>(mapped_image_base+0x1e5990)=GetProcessHeap();cpu<void>(0x452e00,reinterpret_cast<void*>(mapped_image_base+0x1c0240));
    intercept(0x43f550,reinterpret_cast<void*>(&capture_axis));intercept(0x43f630,reinterpret_cast<void*>(&capture_rotated));
    controller=static_cast<s::Controller*>(VirtualAlloc(nullptr,sizeof(s::Controller),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));pe::sprite_controller=controller;
    if(!controller)throw std::bad_alloc();*reinterpret_cast<void**>(mapped_image_base+0x1c0028)=controller;
    // A raw original-layout fixture avoids invoking unrelated renderer startup.
    auto* fixture=static_cast<t::Renderer*>(VirtualAlloc(nullptr,sizeof(t::Renderer),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));if(!fixture)throw std::bad_alloc();
    auto& renderer=*fixture;std::vector<s::SpriteData> sprites(2048);s::AnimationFile file{};file.sprites=sprites.data();controller->files[0]=&file;std::cerr<<"fixtures ready\n";
    std::mt19937 rng(0x46d300);for(auto& sprite:sprites){sprite.u0=float(rng()%1000)/8192.f;sprite.u1=sprite.u0+float(rng()%100)/1024.f;sprite.v0=float(rng()%1000)/8192.f;sprite.v1=sprite.v0+float(rng()%100)/1024.f;sprite.extent_48=float(5+rng()%200)/5.f;sprite.extent_4c=float(5+rng()%300)/11.f;}
    unsigned passed=0,failed=0;std::vector<std::string> failures;
    auto check=[&](const char* name,unsigned test,const void* a,const void* b,std::size_t size){if(!std::memcmp(a,b,size)){++passed;return;}++failed;if(failures.size()<30){std::ostringstream text;text<<name<<" test="<<test;for(std::size_t i=0;i<size;++i)if(static_cast<const unsigned char*>(a)[i]!=static_cast<const unsigned char*>(b)[i]){text<<" +0x"<<std::hex<<i<<" expected="<<unsigned(static_cast<const unsigned char*>(a)[i])<<" actual="<<unsigned(static_cast<const unsigned char*>(b)[i]);break;}failures.push_back(text.str());}};
    for(unsigned test=0;test<512;++test){t::Line original,source;std::memset(&original,0xa7,sizeof(original));source=original;cpu<void>(0x46aba0,&original);t::construct_line(source);check("line_constructor",test,&original,&source,sizeof(source));}
    for(unsigned test=0;test<512;++test){for(auto& line:renderer.lines){auto* raw=reinterpret_cast<std::uint32_t*>(&line);for(unsigned i=0;i<sizeof(line)/4;++i)raw[i]=rng();line.frames=test%2?static_cast<std::int32_t>(rng()%8)-2:static_cast<std::int32_t>(rng());}
        renderer.line_count=int(rng()%350);const auto count=renderer.line_count;std::vector<t::Line> before(renderer.lines,renderer.lines+320);cpu<void>(0x46b7f0,&renderer);std::vector<t::Line> expected(renderer.lines,renderer.lines+320);const auto expected_count=renderer.line_count;
        std::memcpy(renderer.lines,before.data(),sizeof(renderer.lines));renderer.line_count=count;renderer.compact_lines();check("line_compaction",test,expected.data(),renderer.lines,sizeof(renderer.lines));check("line_compaction_count",test,&expected_count,&renderer.line_count,4);}
    for(unsigned test=0;test<1024;++test){s::Animation a{},b{};for(auto& point:a.base.vectors_378)point={7,9};a.base.fields_10_28[3]=0;b=a;const auto index=int(rng()%sprites.size());cpu<void>(0x470ed0,&a,index);t::set_sprite(*controller,b,index);check("glyph_sprite_uv",test,&a,&b,sizeof(a));}
    for(unsigned test=0;test<1512;++test){auto& a=renderer.animations[0];std::memset(&a,0,sizeof(a));s::construct_animation(a);a.base.fields_10_28[3]=0;a.base.fields_10_28[4]=rng()%500;
        for(auto& word:a.base.flags)word=rng();a.base.field_490=rng();a.base.field_494=rng();a.vector_5bc={3,7,9};renderer.font_width=9+test%11;
        const float scale[]={.75f,1.f,1.5f,2.f,2.5f,1.7f};pe::window_state.scale=scale[test%6];*reinterpret_cast<float*>(mapped_image_base+0x1b8818)=pe::window_state.scale;
        t::Line line{};line.font=test%14;line.align_x=(test/14)%3;line.align_y=(test/42)%3;
        line.scale_x=test%5==0?1.f:float(5+rng()%20)/11.f;line.scale_y=float(5+rng()%20)/13.f;line.rotation=test%3==0?0.f:float(int(rng()%100)-50)/31.f;
        line.color=rng();line.blend=rng();line.shadow=test%2;line.position={float(int(rng()%1000)-500)/17.f,float(int(rng()%1000)-500)/21.f,float(test%3)};
        const char* samples[]{"Score 0123456789","AbcXYZ./:-*%$+","1,234.56s* /","a\nb", "\x01\x10\x19\x7f\x81\xc2\xff", ""};strcpy_s(line.text,samples[(test/126)%6]);
        const s::Animation initial=a;draws.clear();FloatingEnvironment::prepare();cpu<void>(0x46d300,&renderer,&line);const s::Animation expected=a;const auto expected_draws=draws;std::memcpy(&a,&initial,sizeof(a));draws.clear();FloatingEnvironment::prepare();renderer.draw_line(line);
        check("ascii_state",test,&expected,&a,sizeof(a));const auto expected_count=expected_draws.size(),actual_count=draws.size();check("ascii_draw_count",test,&expected_count,&actual_count,sizeof(actual_count));
        if(expected_count==actual_count)check("ascii_draw_boundary_trace",test,expected_draws.data(),draws.data(),draws.size()*sizeof(Draw));
    }

    for(unsigned test=0;test<4096;++test){std::int32_t v[8];for(auto& value:v)value=static_cast<std::int32_t>(rng());using F=unsigned char(__cdecl*)(int,int,int,int,int,int,int,int);const auto original=reinterpret_cast<F>(mapped_image_base+0x70920)(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7]);const auto source=static_cast<unsigned char>(t::rectangles_overlap(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7]));check("rectangle_overlap_wrap",test,&original,&source,1);}
    auto* jobs=static_cast<t::Job*>(VirtualAlloc(nullptr,32*sizeof(t::Job),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));if(!jobs)throw std::bad_alloc();
    for(unsigned test=0;test<512;++test){th20::source::scheduler::initialize_list(renderer.jobs);renderer.texture_width=128;renderer.texture_height=128;
        for(unsigned i=0;i<24;++i){auto& job=jobs[i];std::memset(&job,0,sizeof(job));job.rectangle[0]=(i%6)*20;job.rectangle[1]=(i/6)*22;job.rectangle[2]=8+rng()%12;job.rectangle[3]=7+rng()%11;th20::source::scheduler::initialize_link(job.link,reinterpret_cast<th20::source::scheduler::Node*>(&job));th20::source::scheduler::append(renderer.jobs,job.link);}
        const int width=1+rng()%140,height=1+rng()%140;t::Point expected{};cpu<void>(0x470680,&renderer,&expected,width,height);const auto actual=renderer.find_atlas_position(width,height);check("atlas_position",test,&expected,&actual,sizeof(actual));
    }
    for(unsigned test=0;test<512;++test){auto& job=jobs[0];std::memset(&job,0xa7,sizeof(job));std::memset(&job.link,0,sizeof(job.link));
        for(auto& word:renderer.fields_1a1d4)word=rng();renderer.fields_1a1d4[7]=test%3==0?0:rng();renderer.color=rng();renderer.shadow_color=rng();renderer.field_1a1c8=rng();renderer.scale_x=.3f;renderer.scale_y=2.1f;renderer.rotation=.7f;
        th20::source::scheduler::initialize_list(renderer.jobs);const auto before_list=static_cast<th20::source::scheduler::List>(renderer.jobs);const auto initial_flags=renderer.animations[0].base.flags[3];std::array<std::uint8_t,sizeof(t::Job)> before;std::memcpy(before.data(),&job,before.size());
        const s::Vec3 position{12.7f,-45.1f,3};FloatingEnvironment::prepare();cpu<void>(0x46d170,&renderer,&position,&job);std::array<std::uint8_t,sizeof(t::Job)> expected;std::memcpy(expected.data(),&job,expected.size());const auto after_list=static_cast<th20::source::scheduler::List>(renderer.jobs);const auto expected_flags=renderer.animations[0].base.flags[3];
        std::memcpy(&job,before.data(),before.size());static_cast<th20::source::scheduler::List&>(renderer.jobs)=before_list;renderer.animations[0].base.flags[3]=initial_flags;FloatingEnvironment::prepare();renderer.register_job(position,job);check("register_job_state",test,expected.data(),&job,sizeof(job));check("register_job_list",test,&after_list,&renderer.jobs,sizeof(after_list));check("register_job_flags",test,&expected_flags,&renderer.animations[0].base.flags[3],4);
    }
    for(int font=0;font<22;++font){auto handle=CreateFontW(12+font*2,0,0,0,400,0,0,0,SHIFTJIS_CHARSET,0,0,2,0x11,L"ＭＳ ゴシック");th20::source::platform_window::fonts[font]=handle;reinterpret_cast<HFONT*>(mapped_image_base+0x1b66f0)[font]=handle;
        for(const char* string:{"Loading","score 0123456789","", "\x82\xa0\x82\xa2\x82\xa4", "long narrow lllll and WWW"}){using F=std::uint64_t(__cdecl*)(const char*,int);const auto expected=reinterpret_cast<F>(mapped_image_base+0x156e0)(string,font);const auto actual=t::measure_text(string,font);check("gdi_text_extent",font,&expected,&actual,8);}DeleteObject(handle);
    }
    for(unsigned test=0;test<2048;++test){renderer.line_count=rng()%325;renderer.color=rng();renderer.shadow_color=rng();renderer.scale_x=.5f;renderer.scale_y=1.3f;renderer.rotation=.17f;for(auto& word:renderer.fields_1a1d4)word=rng();renderer.fields_1a1d4[3]=test%14;
        const s::Vec3 position{.71f,-32.1f,7.f};const std::uint32_t bits=rng();float value;std::memcpy(&value,&bits,4);const int precision=test%7;const char* suffix=test%3==0?"fps":"%";const auto length=static_cast<std::uint32_t>(std::strlen(suffix));
        std::vector<std::uint8_t> before(sizeof(renderer));std::memcpy(before.data(),&renderer,before.size());FloatingEnvironment::prepare();cpu<void>(0x46ce00,&renderer,&position,value,suffix,length,precision);std::vector<std::uint8_t> expected(sizeof(renderer));std::memcpy(expected.data(),&renderer,expected.size());std::memcpy(&renderer,before.data(),before.size());FloatingEnvironment::prepare();renderer.write_float(position,value,suffix,precision);check("format_and_commit",test,expected.data(),&renderer,expected.size());
    }
    t::renderer=&renderer;*reinterpret_cast<void**>(mapped_image_base+0x1c0698)=&renderer;
    for(unsigned test=0;test<512;++test){renderer.line_count=rng()%325;renderer.color=rng();renderer.fields_1a1d4[3]=rng()%14;const auto scene=test%26;pe::graphics_state.field_0b0c=scene;*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c584c)=scene;
        std::uint32_t frame[16]{};float value;if(test%5==0){const auto bits=rng();std::memcpy(&value,&bits,4);}else value=float(rng()%10000)/100.f;std::memcpy(frame+14,&value,4);
        std::vector<std::uint8_t> before(sizeof(renderer));std::memcpy(before.data(),&renderer,before.size());FloatingEnvironment::prepare();cpu<void>(0x4abf30,frame);std::vector<std::uint8_t> expected(sizeof(renderer));std::memcpy(expected.data(),&renderer,expected.size());std::memcpy(&renderer,before.data(),before.size());FloatingEnvironment::prepare();th20::source::platform_window::unrecovered::draw_frame_rate_text(value);check("fps_scene_color_format",test,expected.data(),&renderer,expected.size());
    }
    std::cerr<<"public text formatting tests\n";
    for(unsigned test=0;test<4096;++test){renderer.line_count=rng()%325;renderer.color=rng();renderer.shadow_color=rng();for(auto& word:renderer.fields_1a1d4)word=rng();renderer.fields_1a1d4[3]=test%14;const s::Vec3 position{7.21f,-41.35f,2.1f};const auto value=static_cast<std::int32_t>(rng());const int width=static_cast<int>(rng()%32)-4;const char padding=test%3?'0':' ';const std::string_view prefix="score:",suffix="pts";const char character=static_cast<char>(rng());std::vector<std::uint8_t> before(sizeof(renderer));std::memcpy(before.data(),&renderer,before.size());
        switch(test%8){case 0:cpu<void>(0x46cc30,&renderer,&position,character);break;case 1:cpu<void>(0x46cc80,&renderer,&position,value);break;case 2:cpu<void>(0x46cd40,&renderer,&position,value,width,padding);break;case 3:cpu<void>(0x46cd80,&renderer,&position,value,suffix.data(),std::uint32_t(suffix.size()));break;case 4:cpu<void>(0x46ce90,&renderer,&position,"raw text",std::uint32_t(8));break;case 5:cpu<void>(0x46cf00,&renderer,&position,prefix.data(),std::uint32_t(prefix.size()),value,suffix.data(),std::uint32_t(suffix.size()));break;case 6:cpu<void>(0x46cfa0,&renderer,&position,prefix.data(),std::uint32_t(prefix.size()),value);break;case 7:{using F=void(__cdecl*)(t::Renderer*,const s::Vec3*,const char*,...);reinterpret_cast<F>(mapped_image_base+0x6c990)(&renderer,&position,"%s %+011d %.3f","fmt",value,double(.123f));break;}}
        std::vector<std::uint8_t> expected(sizeof(renderer));std::memcpy(expected.data(),&renderer,expected.size());std::memcpy(&renderer,before.data(),before.size());
        switch(test%8){case 0:renderer.write_character(position,character);break;case 1:renderer.write_integer(position,value);break;case 2:renderer.write_padded_integer(position,value,width,padding);break;case 3:renderer.write_integer_suffix(position,value,suffix);break;case 4:renderer.write_ascii(position,"raw text");break;case 5:renderer.write_affixed_integer(position,prefix,value,suffix);break;case 6:renderer.write_prefixed_integer(position,prefix,value);break;case 7:renderer.write_ascii_format(position,"%s %+011d %.3f","fmt",value,double(.123f));break;}check("public_format_and_line_commit",test,expected.data(),&renderer,expected.size());
    }
    std::cerr<<"grouped score formatting tests\n";
    for(unsigned test=0;test<4096;++test){
        const std::int64_t edges[]{0,9,10,99,999,1000,999999,1000000,999999999,1000000000,999999999999LL,1000000000000LL,99999999999999LL};
        const std::int64_t magnitude=test<26?edges[test/2]:static_cast<std::int64_t>((std::uint64_t(rng())<<32|rng())%100000000000000ULL);const auto value=test%2?-magnitude:magnitude;
        char original[256],source[256];std::memset(original,0xa6,256);std::memcpy(source,original,256);using F=void(__cdecl*)(char*,int,std::int64_t);reinterpret_cast<F>(mapped_image_base+0x53500)(original,256,value);t::format_grouped_integer(source,256,value);check("grouped_integer",test,original,source,256);
        renderer.line_count=rng()%325;renderer.fields_1a1d4[3]=test%14;const s::Vec3 position{7.21f,-41.35f,2.1f};const std::uint64_t score=static_cast<std::uint64_t>(magnitude/10);const int digit=int(test%10);std::vector<std::uint8_t> before(sizeof(renderer));std::memcpy(before.data(),&renderer,before.size());cpu<void>(0x46d020,&renderer,&position,score,digit);std::vector<std::uint8_t> expected(sizeof(renderer));std::memcpy(expected.data(),&renderer,expected.size());std::memcpy(&renderer,before.data(),before.size());renderer.write_grouped_score(position,score,digit);check("grouped_score_line",test,expected.data(),&renderer,expected.size());
    }
    std::cerr<<"bitmap format tests\n";
    for(int format=-2;format<33;++format){using F=const t::BitmapFormat*(__cdecl*)(int);auto* expected=reinterpret_cast<F>(mapped_image_base+0x15640)(format);auto* actual=t::bitmap_format(format);const bool e=expected!=nullptr,a=actual!=nullptr;check("bitmap_format_found",format,&e,&a,1);if(e&&a)check("bitmap_format",format,expected,actual,sizeof(*actual));}
    for(unsigned test=0;test<128;++test){t::Bitmap original,source;std::memset(&original,0xa7,sizeof(original));cpu<void>(0x414200,&original);check("bitmap_constructor",test,&original,&source,sizeof(source));}
    std::cerr<<"bitmap create tests\n";
    for(unsigned test=0;test<384;++test){t::Bitmap original,source;const int formats[]{21,22,23,24,25,26,-1,27};const int format=formats[test%8],width=test/8%16,height=test/128*3;const auto va=test%2?0x415120u:0x4153d0u;
        const auto expected=cpu<unsigned char>(va,&original,width,height,format);const auto actual=static_cast<unsigned char>(test%2?source.create(width,height,format):source.create_with_fallback(width,height,format));check("bitmap_create_result",test,&expected,&actual,1);check("bitmap_create_fields",test,&original.format,&source.format,20);
        if(expected&&actual){check("bitmap_create_pixels",test,original.pixels,source.pixels,source.image_bytes);}
        const auto released=cpu<unsigned char>(0x416360,&original);const auto source_released=static_cast<unsigned char>(source.release());check("bitmap_release_result",test,&released,&source_released,1);check("bitmap_release_state",test,&original,&source,sizeof(source));}
    std::cerr<<"bitmap invert tests\n";
    for(unsigned test=0;test<768;++test){t::Bitmap original,source;const int formats[]{21,22,23,24,25,26};original.format=source.format=formats[test%6];original.width=source.width=1+rng()%25;original.height=source.height=1+rng()%25;original.pitch=source.pitch=((original.width*(original.format<=22?4:2))+3)/4*4;const auto pixel_bytes=original.pitch*(original.height+1);std::vector<std::uint8_t> a(pixel_bytes),b;for(auto& pixel:a)pixel=static_cast<std::uint8_t>(rng());b=a;original.pixels=a.data();source.pixels=b.data();const int x=rng()%original.width,columns=rng()%(original.width-x+1),rows=rng()%(original.height+1);
        const auto expected=cpu<unsigned char>(0x417ab0,&original,rows,x,columns,0);const auto actual=static_cast<unsigned char>(source.invert_alpha(rows,x,columns));check("bitmap_invert_result",test,&expected,&actual,1);check("bitmap_invert_pixels",test,a.data(),b.data(),a.size());}
    std::cerr<<"bitmap outline tests\n";
    for(unsigned test=0;test<512;++test){t::Bitmap original,source;original.format=source.format=test%2?21:26;original.width=source.width=1+rng()%17;original.height=source.height=1+rng()%17;original.pitch=source.pitch=((original.width*(original.format==21?4:2))+3)/4*4;const auto pixel_bytes=original.pitch*original.height;std::vector<std::uint8_t> a(pixel_bytes),b;for(auto& pixel:a)pixel=test%3?static_cast<std::uint8_t>(rng()):0xff;b=a;original.pixels=a.data();source.pixels=b.data();const int x=rng()%original.width,end=x+rng()%(original.width-x+1);const auto color=rng();const float radii[]{0,1,1.5f,2,2.5f,3,4,-1};const float radius=radii[test%8];FloatingEnvironment::prepare();cpu<void>(0x416420,&original,original.height,x,end,color,radius);FloatingEnvironment::prepare();source.outline(source.height,x,end,color,radius);check("bitmap_outline_pixels",test,a.data(),b.data(),a.size());}
    std::cerr<<"GDI raster and deferred upload tests\n";
    cpu<void>(0x414170,reinterpret_cast<void*>(mapped_image_base+0x1b66e0));
    void* surface_table[15]{};surface_table[2]=reinterpret_cast<void*>(&release_surface);surface_table[13]=reinterpret_cast<void*>(&lock_surface);surface_table[14]=reinterpret_cast<void*>(&unlock_surface);
    void* texture_table[19]{};texture_table[18]=reinterpret_cast<void*>(&get_surface);
    Surface surface{surface_table,std::vector<std::uint8_t>(4352*128),{}};Texture texture{texture_table,&surface};
    struct EmptyCallback {std::uint32_t words[10]{};};
    using Queue=std::uint64_t(__cdecl*)(const RECT*,int,std::uint32_t,std::uint32_t,const char*,Texture*,int,int,int,std::uint8_t*,EmptyCallback);
    for(unsigned test=0;test<352;++test){const auto font=test%22;auto handle=CreateFontW(12+font*2,0,0,0,400,0,0,0,SHIFTJIS_CHARSET,0,0,2,0x11,L"ＭＳ ゴシック");th20::source::platform_window::fonts[font]=handle;reinterpret_cast<HFONT*>(mapped_image_base+0x1b66f0)[font]=handle;
        const auto available=static_cast<std::uint8_t>((test/22)%2);th20::source::platform_window::font_available[0]=available;*reinterpret_cast<std::uint8_t*>(mapped_image_base+0x1b66ec)=available;
        const RECT destination{7,9,test%11==0?1097:100,65};const char* strings[]{"Loading 0123", "\x82\xa0\x82\xa2\x82\xa4", "", "outlined \x81\x9a"};const char* string=strings[(test/44)%4];const int spacing=test%3?0:9,x=test%4;const auto foreground=rng(),background=rng();const bool outline=test%2!=0;const float scale=test%5==0?1.5f:1.f;
        std::uint8_t ready=0;std::fill(surface.pixels.begin(),surface.pixels.end(),std::uint8_t(0x97));*reinterpret_cast<float*>(mapped_image_base+0x1ae120)=scale;
        const auto extent=reinterpret_cast<Queue>(mapped_image_base+0x15830)(&destination,x,foreground,background,string,&texture,font,spacing,outline,&ready,{});
        auto* list=reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1b66e0);auto* first=*reinterpret_cast<std::uint32_t**>(list[1]);auto* implementation=reinterpret_cast<std::uint8_t*>(first[11]);auto* capture=reinterpret_cast<std::uint32_t*>(implementation+8);auto* bitmap=reinterpret_cast<t::Bitmap*>(capture[0]);
        t::next_outline_scale=scale;auto raster=t::rasterize_text(destination,x,foreground,background,string,font,spacing,outline);check("gdi_raster_extent",test,&extent,&raster.extent,8);check("gdi_raster_bitmap_fields",test,&bitmap->format,&raster.bitmap->format,20);check("gdi_raster_pixels",test,bitmap->pixels,raster.bitmap->pixels,raster.bitmap->image_bytes);check("gdi_raster_destination",test,capture+2,&raster.destination,sizeof(RECT));check("gdi_raster_source",test,capture+6,&raster.source,sizeof(RECT));
        using Run=void(__cdecl*)();reinterpret_cast<Run>(mapped_image_base+0x16140)();const auto expected_pixels=surface.pixels;const auto expected_ready=ready;const auto expected_rect=surface.rectangle;const auto expected_gets=texture.gets,expected_locks=surface.locks,expected_unlocks=surface.unlocks,expected_releases=surface.releases;
        std::fill(surface.pixels.begin(),surface.pixels.end(),std::uint8_t(0x97));ready=0;texture.gets=surface.locks=surface.unlocks=surface.releases=0;t::next_outline_scale=scale;
        const auto source_extent=t::queue_text(destination,x,foreground,background,string,*reinterpret_cast<IDirect3DTexture9*>(&texture),font,spacing,outline,&ready,{});t::process_one_deferred_task();
        check("gdi_deferred_extent",test,&extent,&source_extent,8);check("gdi_upload_pixels",test,expected_pixels.data(),surface.pixels.data(),surface.pixels.size());check("gdi_upload_ready",test,&expected_ready,&ready,1);check("gdi_upload_rect",test,&expected_rect,&surface.rectangle,sizeof(RECT));check("gdi_upload_gets",test,&expected_gets,&texture.gets,4);check("gdi_upload_locks",test,&expected_locks,&surface.locks,4);check("gdi_upload_unlocks",test,&expected_unlocks,&surface.unlocks,4);check("gdi_upload_releases",test,&expected_releases,&surface.releases,4);
        texture.gets=surface.locks=surface.unlocks=surface.releases=0;DeleteObject(handle);
    }
    std::cerr<<"text rectangle and cached job tests\n";
    for(unsigned test=0;test<1024;++test){s::Animation a{},b{};s::construct_animation(a);a.base.fields_10_28[3]=0;a.base.fields_10_28[4]=test%sprites.size();b=a;auto& data=sprites[test%sprites.size()];data.texture_extent_1c=17.3f;data.texture_extent_20=713.7f;data.scale_50=12.2f;data.scale_54=9.9f;const int x=rng(),y=rng(),width=rng(),height=rng();FloatingEnvironment::prepare();cpu<void>(0x470b80,&a,x,y,width,height);FloatingEnvironment::prepare();t::set_text_rectangle(*controller,b,x,y,width,height);check("text_rectangle",test,&a,&b,sizeof(a));}
    using WriteText=void(__cdecl*)(t::Renderer*,const s::Vec3*,const char*,std::uint32_t,...);
    auto& cached=*::new(static_cast<void*>(jobs))t::Job;cached.text="Cached text";renderer.animation_file=&file;pe::graphics_state.surface_animation=&file;*reinterpret_cast<void**>(mapped_image_base+0x1c5880)=&file;
    for(unsigned test=0;test<128;++test){th20::source::scheduler::initialize_list(renderer.jobs);th20::source::scheduler::initialize_link(cached.link,reinterpret_cast<th20::source::scheduler::Node*>(&cached));th20::source::scheduler::append(renderer.jobs,cached.link);cached.frames=0;cached.fields_628[1]=renderer.fields_1a1d4[4]=test%22;renderer.color=rng();renderer.shadow_color=rng();renderer.field_1a1c8=rng();const s::Vec3 position{13.17f,-41.23f,5.5f};std::array<std::uint8_t,sizeof(t::Job)> before,expected;std::memcpy(before.data(),&cached,before.size());FloatingEnvironment::prepare();reinterpret_cast<WriteText>(mapped_image_base+0x6c210)(&renderer,&position,"Cached text",11);std::memcpy(expected.data(),&cached,expected.size());std::memcpy(&cached,before.data(),before.size());FloatingEnvironment::prepare();renderer.write_text_literal(position,"Cached text");check("reuse_cached_text_job",test,expected.data(),&cached,sizeof(cached));}
    cached.~Job();
#include "menu_compare.inl"
    std::cerr<<"dynamic job create, clone, raster and cancel tests\n";
    ::new(static_cast<void*>(&renderer.pending_tasks))std::list<std::function<void()>>;
    s::TextureRecord texture_record{};texture_record.texture=reinterpret_cast<IDirect3DTexture9*>(&texture);file.textures=&texture_record;file.texture_count=1;std::uint8_t file_byte=0;file.bytes=&file_byte;
    for(auto& sprite:sprites){sprite.texture_id=0;sprite.texture_extent_1c=sprite.texture_extent_20=2048.f;sprite.scale_50=sprite.scale_54=256.f;}
    auto snapshot_jobs=[&]{std::vector<std::uint8_t> result;auto append=[&](const void* data,std::size_t bytes){auto* first=static_cast<const std::uint8_t*>(data);result.insert(result.end(),first,first+bytes);};unsigned count=0;for(th20::source::scheduler::Iterator it(renderer.jobs.sentinel.next);it.current;it.advance()){auto& job=*reinterpret_cast<t::Job*>(it.current->value);auto animation=static_cast<s::Animation>(job.animation);for(auto& link:animation.links)if(link.value==&job.animation)link.value=reinterpret_cast<s::Animation*>(0xfffffff0);append(&animation,sizeof(animation));append(&job.position,0x67c-0x614);const bool external=job.external_ready!=nullptr;append(&external,1);const bool canceled=job.canceled.load();append(&canceled,1);const auto length=job.text.size();append(&length,4);append(job.text.data(),length);++count;}append(&count,4);return result;};
    auto clear_jobs=[&](bool original){while(renderer.jobs.sentinel.next){auto* job=reinterpret_cast<t::Job*>(renderer.jobs.sentinel.next->value);if(original){cpu<void>(0x46a100,nullptr,job);}else t::destroy_job(job);}renderer.pending_tasks.clear();};
    for(unsigned test=0;test<88;++test){const int font=test%22;auto handle=CreateFontW(12+font,0,0,0,400,0,0,0,SHIFTJIS_CHARSET,0,0,2,0x11,L"ＭＳ ゴシック");th20::source::platform_window::fonts[font]=handle;reinterpret_cast<HFONT*>(mapped_image_base+0x1b66f0)[font]=handle;
        renderer.texture_width=renderer.texture_height=2048;renderer.color=0xff887766;renderer.shadow_color=0xff111122;renderer.field_1a1c8=0xffefcfaf;renderer.scale_x=1.3f;renderer.scale_y=.7f;std::memset(renderer.fields_1a1d4,0,sizeof(renderer.fields_1a1d4));renderer.fields_1a1d4[4]=font;renderer.fields_1a1d4[5]=test%2?0:7;renderer.fields_1a1d4[8]=renderer.fields_1a1d4[9]=1;const s::Vec3 position{8.7f,4.5f,0};const char* string=test%2?"Dynamic":"\x82\xa0\x82\xa2\x82\xa4";
        th20::source::scheduler::initialize_list(renderer.jobs);std::fill(surface.pixels.begin(),surface.pixels.end(),std::uint8_t(0x97));FloatingEnvironment::prepare();reinterpret_cast<WriteText>(mapped_image_base+0x6c210)(&renderer,&position,string,static_cast<std::uint32_t>(std::strlen(string)));const auto original_new=snapshot_jobs();const auto original_pending=renderer.pending_tasks.size();
        if(test%4==0)reinterpret_cast<t::Job*>(renderer.jobs.sentinel.next->value)->canceled.store(true);
        if(test%4!=0)reinterpret_cast<WriteText>(mapped_image_base+0x6c210)(&renderer,&position,string,static_cast<std::uint32_t>(std::strlen(string)));const auto original_clone=snapshot_jobs();
        while(!renderer.pending_tasks.empty()){auto function=renderer.pending_tasks.front();renderer.pending_tasks.pop_front();function();}using Run=void(__cdecl*)();reinterpret_cast<Run>(mapped_image_base+0x16140)();const auto original_uploaded=snapshot_jobs();const auto original_pixels=surface.pixels;clear_jobs(true);
        th20::source::scheduler::initialize_list(renderer.jobs);std::fill(surface.pixels.begin(),surface.pixels.end(),std::uint8_t(0x97));FloatingEnvironment::prepare();renderer.write_text_literal(position,string);const auto source_new=snapshot_jobs();const auto source_pending=renderer.pending_tasks.size();check("dynamic_new_job",test,original_new.data(),source_new.data(),std::min(original_new.size(),source_new.size()));const auto original_size=original_new.size(),source_size=source_new.size();check("dynamic_new_job_size",test,&original_size,&source_size,sizeof(source_size));
        check("dynamic_pending_count",test,&original_pending,&source_pending,sizeof(source_pending));if(test%4==0)reinterpret_cast<t::Job*>(renderer.jobs.sentinel.next->value)->canceled.store(true);if(test%4!=0)renderer.write_text_literal(position,string);const auto source_clone=snapshot_jobs();const auto original_clone_size=original_clone.size(),source_clone_size=source_clone.size();check("dynamic_clone_size",test,&original_clone_size,&source_clone_size,sizeof(source_clone_size));check("dynamic_clone_jobs",test,original_clone.data(),source_clone.data(),std::min(original_clone.size(),source_clone.size()));
        while(!renderer.pending_tasks.empty()){auto function=renderer.pending_tasks.front();renderer.pending_tasks.pop_front();function();}t::process_one_deferred_task();const auto source_uploaded=snapshot_jobs();const auto original_uploaded_size=original_uploaded.size(),source_uploaded_size=source_uploaded.size();check("dynamic_upload_size",test,&original_uploaded_size,&source_uploaded_size,sizeof(source_uploaded_size));check("dynamic_upload_jobs",test,original_uploaded.data(),source_uploaded.data(),std::min(original_uploaded.size(),source_uploaded.size()));check("dynamic_upload_pixels",test,original_pixels.data(),surface.pixels.data(),surface.pixels.size());clear_jobs(false);DeleteObject(handle);
    }
    renderer.pending_tasks.~list();
    VirtualFree(jobs,0,MEM_RELEASE);
    std::ofstream report(argv[2]);report<<"{\n  \"passed\": "<<passed<<",\n  \"failed\": "<<failed<<",\n  \"original_sha256\": \""<<expected_sha<<"\",\n  \"render_boundary_interception\": [\"0x0043f550\", \"0x0043f630\"],\n  \"full_game_equivalence\": false,\n  \"failures\": [";
    for(unsigned i=0;i<failures.size();++i)report<<(i?", ":"")<<'"'<<failures[i]<<'"';report<<"]\n}\n";
    for(const auto& failure:failures)std::cerr<<failure<<'\n';std::cout<<passed<<" passed, "<<failed<<" failed\n";VirtualFree(fixture,0,MEM_RELEASE);VirtualFree(controller,0,MEM_RELEASE);return failed?1:0;
}catch(const std::exception& ex){std::cerr<<ex.what()<<'\n';return 2;}}
