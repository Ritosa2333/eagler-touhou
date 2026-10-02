#pragma once
// Shared support for optional, locally supplied retail-data CPU diagnostics.
// No game resources are embedded. File access is read-only and all unsupported
// host/animation/render paths fail closed rather than silently approximating.
#include "../th10_web/cpp/platform/FileSystem.hpp"
#include "../th10_web/cpp/game/AnmResources.hpp"
#include "../th10_web/cpp/game/AnmRenderer.hpp"
#include "../th10_web/cpp/game/TexturePlatform.hpp"
#include <eagler/netplay/RollbackJournal.hpp>
#include <eagler/netplay/SparsePoolCapture.hpp>
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
namespace th10_test {
using namespace th10;
[[noreturn]] inline void fail(const char* expression,int line){std::fprintf(stderr,"Game resource check line %d: %s\n",line,expression);std::abort();}
#define CHECK(x) do{if(!(x))::th10_test::fail(#x,__LINE__);}while(false)
template<class T>T read_le(const u8* bytes){T value;std::memcpy(&value,bytes,sizeof(value));return value;}
inline u64 hash_bytes(const void* data,std::size_t length,u64 hash=14695981039346656037ull){const auto* bytes=static_cast<const u8*>(data);for(std::size_t i=0;i<length;++i)hash=(hash^bytes[i])*1099511628211ull;return hash;}
struct Host final:browser::FileHost {
    std::vector<FILE*> streams;
    u32 open(const char* name,bool write) override{CHECK(!write);CHECK(std::strcmp(name,"/input/th10.dat")==0);auto* stream=std::fopen(name,"rb");if(!stream)return ~u32(0);streams.push_back(stream);return streams.size()-1;}
    void close(u32 handle) override{CHECK(handle<streams.size()&&streams[handle]);std::fclose(streams[handle]);streams[handle]=nullptr;}
    u32 size(u32 handle) override{auto* file=streams.at(handle);const auto position=std::ftell(file);CHECK(!std::fseek(file,0,SEEK_END));const auto length=std::ftell(file);CHECK(length>=0);CHECK(!std::fseek(file,position,SEEK_SET));return length;}
    u32 seek(u32 handle,i32 offset,u32 origin) override{CHECK(origin<=2);auto* file=streams.at(handle);return std::fseek(file,offset,origin)?~u32(0):static_cast<u32>(std::ftell(file));}
    u32 read(u32 handle,u8* bytes,u32 length) override{return std::fread(bytes,1,length,streams.at(handle));}
    u32 write(u32,const u8*,u32) override{CHECK(false);return 0;}
    u32 list(const char*,const char*,u32,char*,u32) override{CHECK(false);return 0;}
};
struct Texture {TextureDescription dimensions;};
struct Textures final:TexturePlatform {
    u32 display=0,uploads=0;u64 upload_bytes=0,upload_hash=14695981039346656037ull;
    Textures(){display_flags=&display;}
    i32 create_surface_texture(AnmTexture& texture,u32 width,u32 height,u32 format) override{CHECK(width&&height);texture.handle=new Texture{{format,width,height}};return 0;}
    i32 decode_source_texture(AnmTexture&,u32,u32,u32,u32) override{CHECK(false);return -1;}
    void* get_surface(void* texture) override{return texture;}
    TextureDescription describe_surface(void* surface) override{return static_cast<Texture*>(surface)->dimensions;}
    TextureLock lock_surface(void*) override{CHECK(false);return {};}
    void unlock_surface(void*) override{CHECK(false);}
    void release_surface(void*) override{}
    i32 upload_surface(void* surface,const u8* bytes,u32 format,i32 pitch,const TextureRect& rect) override{
        const auto& target=static_cast<Texture*>(surface)->dimensions;CHECK(format==target.format);CHECK(rect.left==0&&rect.top==0&&rect.right>0&&rect.bottom>0);CHECK(u32(rect.right)<=target.width&&u32(rect.bottom)<=target.height);CHECK(pitch>0);
        const auto length=std::size_t(pitch)*rect.bottom;upload_hash=hash_bytes(bytes,length,upload_hash);upload_bytes+=length;++uploads;return 0;
    }
};
struct Resources final:AnmResourceEnvironment,AnmTextureEnvironment {
    browser::FileSystem& files;Textures textures;u32 flags=0;
    Resources(browser::FileSystem& f,AnmManager& manager):files(f){registry=&manager.registry;loader_flags=&flags;}
    AnmFile* allocate_file() override{return static_cast<AnmFile*>(std::malloc(sizeof(AnmFile)));}
    void* allocate_bytes(u32 bytes) override{auto* value=std::malloc(bytes);CHECK(value||!bytes);return value;}
    void release_file(AnmFile* value) override{std::free(value);}
    void release_bytes(void* value) override{std::free(value);}
    u8* read_file(const char* name,bool external,u32* size) override{CHECK(!external);return ResourceFiles{files}.load(name,size,external);}
    void release_texture(void* value) override{delete static_cast<Texture*>(value);}
    void report(AnmResourceError error) override{std::fprintf(stderr,"ANM error %d\n",int(error));CHECK(false);}
    i32 materialize(AnmFile& file,i32 texture,i32 sprite,i32 script,const AnmChunk* chunk) override{return file.materialize(texture,sprite,script,chunk,*this);}
    void wait_for_loading(AnmManager& manager,AnmFile&) override{CHECK(manager.process_loading(*this)==0);}
    i32 create_empty(AnmTexture& texture,i32 width,i32 height,i32 format) override{return texture.create_empty(width,height,format,textures);}
    i32 create_encoded(AnmTexture&,i32,i32,i32,u32) override{CHECK(false);return -1;}
    i32 create_embedded(AnmTexture& texture,const u8* data,i32 width,i32 height,i32 format) override{CHECK(read_le<u32>(data)==0x58544854);return texture.create_embedded(data,width,height,format,textures);}
    void set_priority(void*,u32) override{}
    void preload(void*) override{}
    AnmTextureDimensions dimensions(void* value) override{auto size=static_cast<Texture*>(value)->dimensions;return {size.width,size.height};}
    void texture_error(AnmResourceError error,const char*) override{report(error);}
};
struct Script {i32 index;u32 instructions=0;bool cpu_safe=true;u32 excluded_paths=0;std::vector<u32> geometry_counts;std::array<bool,94> opcodes{};};
inline std::vector<Script> inspect_anm(const u8* data,u32 length){
    std::vector<Script> scripts;u32 chunk_offset=0;u32 global_script=0;
    for(;;){
        CHECK(chunk_offset+sizeof(AnmChunk)<=length);const auto* bytes=data+chunk_offset;const auto& chunk=*reinterpret_cast<const AnmChunk*>(bytes);const u32 chunk_size=chunk.next_offset?chunk.next_offset:length-chunk_offset;
        CHECK(chunk_size>=sizeof(AnmChunk)&&chunk_size<=length-chunk_offset);CHECK(chunk.version==4&&chunk.sprite_count>=0&&chunk.script_count>=0);CHECK(chunk.width>0&&chunk.height>0);CHECK(sizeof(AnmChunk)+u64(chunk.sprite_count)*4+u64(chunk.script_count)*8<=chunk_size);
        CHECK(chunk.name_offset<chunk_size);CHECK(std::memchr(bytes+chunk.name_offset,0,chunk_size-chunk.name_offset));
        if(chunk.embedded_texture){CHECK(chunk.texture_offset+16<=chunk_size);const auto* texture=bytes+chunk.texture_offset;CHECK(read_le<u32>(texture)==0x58544854);const auto format=read_le<u16>(texture+6);CHECK(format<6);const auto width=read_le<u16>(texture+8),height=read_le<u16>(texture+10);constexpr u32 sizes[]={4,4,2,2,3,2};CHECK(u64(width)*height*sizes[format]<=chunk_size-chunk.texture_offset-16);}
        const auto* table=bytes+sizeof(AnmChunk);
        for(i32 sprite=0;sprite<chunk.sprite_count;++sprite){const auto offset=read_le<u32>(table+sprite*4);CHECK(offset<=chunk_size&&20<=chunk_size-offset);}
        table+=chunk.sprite_count*4;
        for(i32 script=0;script<chunk.script_count;++script){
            Script summary{};summary.index=static_cast<i32>(global_script++);u32 offset=read_le<u32>(table+script*8+4);bool ended=false;
            for(u32 steps=0;steps<100000;++steps){
                CHECK(offset<=chunk_size&&sizeof(AnmInstruction)<=chunk_size-offset);const auto* instruction=reinterpret_cast<const AnmInstruction*>(bytes+offset);const auto op=instruction->opcode;CHECK(op>=-1&&op<=92);summary.opcodes[op+1]=true;++summary.instructions;
                if(op==-1){ended=true;break;}CHECK(instruction->length>=8&&instruction->length<=chunk_size-offset);
                // CPU fixture deliberately excludes child/geometry/matrix paths.
                if(op==84)summary.excluded_paths|=1;
                if(op==88||op==90||op==91||op==92)summary.excluded_paths|=2;
                if(op==81)summary.excluded_paths|=4;
                if(op==84)summary.geometry_counts.push_back(instruction->is_reference(0)?~u32(0):instruction->argument<u32>(0));
                if(op==67&&instruction->argument<u32>(0)>3)summary.excluded_paths|=8;
                // Only writable VM references are allowed in a shared fixture.
                if((op>=6&&op<=27)||op==5||op==40||op==41||(op>=42&&op<=47)){
                    if(!instruction->is_reference(0))summary.excluded_paths|=16;
                    else if((op>=6&&op<=27&&(op&1))||op==41||(op>=42&&op<=47)){
                        const auto ref=instruction->argument<float>(0);if(!((ref>=10004&&ref<=10007)||(ref>=10013&&ref<=10015)))summary.excluded_paths|=16;
                    }else{const auto ref=instruction->argument<i32>(0);if(!((ref>=10000&&ref<=10003)||ref==10008||ref==10009))summary.excluded_paths|=16;}
                }
                offset+=instruction->length;
            }
            CHECK(ended);summary.cpu_safe=summary.excluded_paths==0;scripts.push_back(summary);
        }
        if(!chunk.next_offset)break;chunk_offset+=chunk.next_offset;
    }
    return scripts;
}
struct Animations final:AnmEnvironment {
    AnmFile& file;float speed=1;Rng rng{};Vec3 zero{};u32 binds=0;bool allow_geometry=false;std::vector<std::unique_ptr<u8[]>> geometry;std::vector<u32> geometry_sizes;
    explicit Animations(AnmFile& f):file(f){rate=&speed;visual_rng=script_rng=&rng;camera_delta=default_tangent=&zero;reference_positions[0]=reference_positions[1]=&zero;}
    void bind_sprite(AnmVm& vm,i32 index) override{CHECK(index>=0&&index<file.sprite_count);CHECK(file.bind_sprite(vm,index)==0);++binds;}
    void change_draw_mode(AnmVm&) override{CHECK(false);}
    void* allocate_geometry(u32 bytes) override{CHECK(allow_geometry&&bytes>0&&bytes<=65536);geometry.push_back(std::make_unique<u8[]>(bytes+16));std::memset(geometry.back().get()+bytes,0xa5,16);geometry_sizes.push_back(bytes);return geometry.back().get();}
    AnmVm* spawn_child(AnmVm&,i32,u32) override{CHECK(false);return nullptr;}
};
struct Backend final:AnmRenderEnvironment {
    PipelineState state{};AnmVertex vertices[4]{};RenderViewport view{0,0,640,480};unsigned calls=0,quads=0,textures=0;
    Backend(){quad=vertices;viewport=&view;for(auto& vertex:vertices)vertex.reciprocal_w=1;}
    void reset(){calls=quads=textures=0;state={};}
    PipelineState& pipeline() override{return state;}
    void set_texture(void*) override{++textures;}
    void vertex_format(LayoutParameter) override{}
    void draw_triangles(TopologyParameter,u32 count,const void*,u32 stride) override{CHECK(stride==sizeof(AnmVertex)&&count%2==0);++calls;quads+=count/2;}
    void set_transform(MatrixParameter,const Matrix4&) override{CHECK(false);}
    void stream_source(void*,u32) override{CHECK(false);}
    void draw_buffer(TopologyParameter,u32,u32) override{CHECK(false);}
    i32 special_draw(AnmManager&,AnmVm&,u32) override{CHECK(false);return 0;}
};
inline std::vector<u8> copy_bytes(const void* address,std::size_t size){const auto* bytes=static_cast<const u8*>(address);return {bytes,bytes+size};}
} // namespace th10_test
