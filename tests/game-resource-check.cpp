// Optional, locally supplied retail-data check. No game resources are embedded.
// Compiles actual archive codec, resource loader, ANM interpreter and renderer.
// Texture/device I/O is a CPU test double; this is not browser or GPU gameplay.
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
using namespace th10;
namespace {
[[noreturn]] void fail(const char* expression,int line){std::fprintf(stderr,"Game resource check line %d: %s\n",line,expression);std::abort();}
#define CHECK(x) do{if(!(x))fail(#x,__LINE__);}while(false)
template<class T>T read_le(const u8* bytes){T value;std::memcpy(&value,bytes,sizeof(value));return value;}
u64 hash_bytes(const void* data,std::size_t length,u64 hash=14695981039346656037ull){const auto* bytes=static_cast<const u8*>(data);for(std::size_t i=0;i<length;++i)hash=(hash^bytes[i])*1099511628211ull;return hash;}
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
std::vector<Script> inspect_anm(const u8* data,u32 length){
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
void render(AnmManager& manager,Backend& backend,AnmRenderer& renderer,unsigned count){
    backend.reset();manager.current_texture=nullptr;std::memset(manager.cached_draw_state,0xff,sizeof(manager.cached_draw_state));
    manager.submitted_draws=manager.flushed_batches=0;renderer.begin_frame();
    for(unsigned i=0;i<count;++i){if(!manager.occupied[i])continue;const auto& vm=manager.pool[i];CHECK((vm.flags&3)!=3||!(vm.color>>24)||vm.sprite);renderer.draw(manager.pool[i]);}
    renderer.flush();CHECK(backend.quads==manager.submitted_draws&&backend.calls==manager.flushed_batches);
}
std::vector<u8> copy_bytes(const void* address,std::size_t size){const auto* bytes=static_cast<const u8*>(address);return {bytes,bytes+size};}
std::vector<i32> survey_scripts(AnmManager& manager,AnmFile& file,const std::vector<Script>& scripts){
    Animations environment(file);Backend backend;AnmRenderer renderer{manager,backend};std::vector<i32> selected;u64 quads=0,updates=0;u32 ended=0;
    for(const auto& script:scripts){if(!script.cpu_safe)continue;auto& vm=manager.pool[0];vm={};u32 started=0;environment.rng={0x1234,0,0};file.initialize_script(vm,script.index,environment,started);CHECK(started==1);vm.position={224,200,0};manager.occupied[0]=1;bool done=false;
        for(unsigned frame=0;frame<120;++frame){if(!done){done=vm.update(environment)!=0;manager.occupied[0]=!done;++updates;}render(manager,backend,renderer,1);quads+=backend.quads;}
        ended+=done;selected.push_back(script.index);
    }
    std::printf("{\"suite\":\"retail-script-survey\",\"file\":\"%s\",\"scripts\":%zu,\"frames_per_script\":120,\"updates\":%llu,\"terminated\":%u,\"quads\":%llu,\"sprite_binds\":%u}\n",file.name,selected.size(),(unsigned long long)updates,ended,(unsigned long long)quads,environment.binds);
    return selected;
}
void dense_scripts(AnmManager& manager,AnmFile& file,const std::vector<i32>& scripts,unsigned count,u32 file_size){
    constexpr unsigned Frames=12,Trials=5,WarmupPasses=2;
    using Clock=std::chrono::steady_clock;
    const auto elapsed=[](Clock::time_point begin){return std::chrono::duration<double,std::milli>(Clock::now()-begin).count();};
    CHECK(!scripts.empty()&&count<=4096);
    Animations environment(file);Backend backend;AnmRenderer renderer{manager,backend};
    std::memset(manager.pool,0,sizeof(manager.pool));std::memset(manager.occupied,0,sizeof(manager.occupied));
    for(unsigned i=0;i<count;++i){
        file.initialize_script(manager.pool[i],scripts[i%scripts.size()],environment,manager.started_scripts);
        manager.pool[i].position={32.f+float(i%32)*17.f+.125f,24.f+float((i/32)%24)*17.f+.25f,.25f};manager.occupied[i]=1;
    }
    manager.cursor=count%4096;
    const auto initial=copy_bytes(manager.pool,sizeof(manager.pool)),program=copy_bytes(file.loaded,file_size);
    const auto initial_occupied=copy_bytes(manager.occupied,sizeof(manager.occupied));const auto initial_rng=environment.rng;
    const auto active=[&](){return std::count(manager.occupied,manager.occupied+count,u8(1));};
    const auto update=[&](){unsigned calls=0;for(unsigned i=0;i<count;++i)if(manager.occupied[i]){
        ++calls;if(manager.pool[i].update(environment)){manager.occupied[i]=0;manager.pool[i].initialize();}
    }
    return calls;};
    Netplay::RollbackJournal journal;CHECK(journal.Reset({Frames+2,sizeof(manager.pool)+8192,4104,true,true}));
    Netplay::SparsePoolCapture<4096> capture;
    const auto snapshot=[&](unsigned frame){
        CHECK(journal.BeginFrame(frame));CHECK(journal.Touch(manager.occupied,sizeof(manager.occupied)));
        CHECK(journal.Touch(&manager.cursor,sizeof(manager.cursor)));CHECK(journal.Touch(&environment.rng,sizeof(environment.rng)));
        CHECK(capture.Capture(manager.pool,[&](const AnmVm& vm){return manager.occupied[&vm-manager.pool]!=0;},[&](void* p,std::size_t n){return journal.Touch(p,n);}));
    };
    const auto restored=[&](){CHECK(std::memcmp(manager.pool,initial.data(),initial.size())==0);CHECK(std::memcmp(manager.occupied,initial_occupied.data(),initial_occupied.size())==0);CHECK(std::memcmp(&initial_rng,&environment.rng,sizeof(initial_rng))==0);};
    struct Evidence{std::vector<u8> vertices;u32 quads,calls,textures,updates;};std::array<Evidence,Frames> frames;
    for(unsigned frame=0;frame<Frames;++frame){
        snapshot(frame);const auto calls=update();
        const auto before_render=copy_bytes(manager.pool,sizeof(manager.pool));render(manager,backend,renderer,count);
        CHECK(std::memcmp(manager.pool,before_render.data(),before_render.size())==0);
        frames[frame]={copy_bytes(manager.vertex_buffer,std::size_t(manager.vertex_write-manager.vertex_buffer)*sizeof(AnmVertex)),backend.quads,backend.calls,backend.textures,calls};CHECK(journal.EndFrame());
    }
    const auto final=copy_bytes(manager.pool,sizeof(manager.pool)),final_occupied=copy_bytes(manager.occupied,sizeof(manager.occupied));const auto final_rng=environment.rng;
    CHECK(journal.UndoTo(0));restored();u64 quads=0,vertex_hash=14695981039346656037ull;
    for(unsigned frame=0;frame<Frames;++frame){
        CHECK(update()==frames[frame].updates);render(manager,backend,renderer,count);const auto& expected=frames[frame];
        CHECK(std::size_t(manager.vertex_write-manager.vertex_buffer)*sizeof(AnmVertex)==expected.vertices.size());
        CHECK(std::memcmp(manager.vertex_buffer,expected.vertices.data(),expected.vertices.size())==0);
        CHECK(backend.quads==expected.quads&&backend.calls==expected.calls&&backend.textures==expected.textures);
        quads+=backend.quads;vertex_hash=hash_bytes(expected.vertices.data(),expected.vertices.size(),vertex_hash);
    }
    const auto replayed=[&](){CHECK(std::memcmp(manager.pool,final.data(),final.size())==0);CHECK(std::memcmp(manager.occupied,final_occupied.data(),final_occupied.size())==0);CHECK(std::memcmp(&final_rng,&environment.rng,sizeof(final_rng))==0);CHECK(std::memcmp(file.loaded,program.data(),program.size())==0);};
    replayed();
    std::printf("{\"suite\":\"retail-dense-replay\",\"file\":\"%s\",\"initial_instances\":%u,\"active_after\":%zu,\"unique_scripts\":%zu,\"frames\":12,\"quads\":%llu,\"rewind_byte_equal\":true,\"replay_byte_equal\":true,\"vertex_byte_equal\":true,\"render_vm_unchanged\":true,\"resource_bytes_unchanged\":true,\"vertex_hash\":\"%016llx\"}\n",file.name,count,std::size_t(active()),scripts.size(),(unsigned long long)quads,(unsigned long long)vertex_hash);
    // Every pass starts from the same explicit initial scene. Terminated VMs
    // retire and are not replenished within the measured 12-frame window.
    std::memcpy(manager.pool,initial.data(),initial.size());std::memcpy(manager.occupied,initial_occupied.data(),initial_occupied.size());environment.rng=initial_rng;
    for(unsigned pass=0;pass<WarmupPasses+Trials;++pass){
        double snapshot_ms=0,update_ms=0,render_ms=0;u64 measured_hash=14695981039346656037ull,measured_quads=0,snapshot_bytes=0,snapshot_blocks=0;
        std::array<unsigned,Frames> calls{};const auto growths=journal.ArenaGrowths();
        for(unsigned frame=0;frame<Frames;++frame){
            auto begin=Clock::now();snapshot(frame);snapshot_ms+=elapsed(begin);snapshot_bytes+=journal.BytesForFrame(frame);snapshot_blocks+=journal.BlocksForFrame(frame);
            begin=Clock::now();calls[frame]=update();update_ms+=elapsed(begin);
            begin=Clock::now();render(manager,backend,renderer,count);render_ms+=elapsed(begin);
            CHECK(journal.EndFrame());CHECK(calls[frame]==frames[frame].updates);
            const auto bytes=std::size_t(manager.vertex_write-manager.vertex_buffer)*sizeof(AnmVertex);
            CHECK(bytes==frames[frame].vertices.size());CHECK(std::memcmp(manager.vertex_buffer,frames[frame].vertices.data(),bytes)==0);
            measured_hash=hash_bytes(manager.vertex_buffer,bytes,measured_hash);measured_quads+=backend.quads;
        }
        CHECK(measured_hash==vertex_hash&&measured_quads==quads);replayed();const auto active_after=active();
        auto begin=Clock::now();CHECK(journal.UndoTo(0));const double undo_ms=elapsed(begin);restored();
        if(pass<WarmupPasses)continue;
        CHECK(journal.ArenaGrowths()==growths);
        std::printf("{\"suite\":\"retail-dense-timing\",\"file\":\"%s\",\"initial_instances\":%u,\"active_after\":%zu,\"trial\":%u,\"frames\":12,\"warmup_frames\":24,\"snapshot_ms\":%.6f,\"update_ms\":%.6f,\"render_ms\":%.6f,\"undo_ms\":%.6f,\"snapshot_bytes\":%llu,\"snapshot_blocks\":%llu,\"arena_growths\":0,\"quads\":%llu,\"vertex_hash\":\"%016llx\",\"updates_per_frame\":[",file.name,count,std::size_t(active_after),pass-WarmupPasses,snapshot_ms,update_ms,render_ms,undo_ms,(unsigned long long)snapshot_bytes,(unsigned long long)snapshot_blocks,(unsigned long long)quads,(unsigned long long)vertex_hash);
        for(unsigned frame=0;frame<Frames;++frame)std::printf("%s%u",frame?",":"",calls[frame]);std::puts("]}");
    }
    std::memset(manager.occupied,0,sizeof(manager.occupied));
}
void survey_geometry(AnmManager& manager,AnmFile& file,const std::vector<Script>& scripts){
    for(const auto& script:scripts){if(script.geometry_counts.empty())continue;
        CHECK(!script.opcodes[88+1]&&!script.opcodes[90+1]&&!script.opcodes[91+1]&&!script.opcodes[92+1]);
        auto& vm=manager.pool[0];vm={};Animations environment(file);environment.allow_geometry=true;u32 started=0;file.initialize_script(vm,script.index,environment,started);CHECK(started==1);vm.position={224,200,0};
        for(unsigned frame=0;frame<120;++frame)if(vm.update(environment))break;
        CHECK(!environment.geometry_sizes.empty());u64 hash=14695981039346656037ull;
        for(std::size_t i=0;i<environment.geometry.size();++i){
            const auto bytes=environment.geometry_sizes[i];for(u32 guard=0;guard<16;++guard)CHECK(environment.geometry[i][bytes+guard]==0xa5);CHECK(bytes%sizeof(AnmVertex)==0);const auto* vertices=reinterpret_cast<const AnmVertex*>(environment.geometry[i].get());for(u32 j=0;j<bytes/sizeof(AnmVertex);++j){CHECK(std::isfinite(vertices[j].position.x)&&std::isfinite(vertices[j].position.y)&&std::isfinite(vertices[j].position.z));}
            hash=hash_bytes(environment.geometry[i].get(),bytes,hash);
            std::printf("{\"suite\":\"retail-ring-allocation\",\"file\":\"%s\",\"script\":%d,\"allocation\":%zu,\"requested_bytes\":%u,\"pool_slot_bytes\":65536,\"geometry_hash\":\"%016llx\",\"rendered\":false}\n",file.name,script.index,i,bytes,(unsigned long long)hash);
        }
    }
}
void inspect_archive(browser::FileSystem& files){
    auto& archive=files.resources;CHECK(archive.count>0);u64 total=0,hash=14695981039346656037ull;unsigned animations=0;
    for(i32 i=0;i<archive.count;++i){const auto& entry=archive.entries[i];CHECK(entry.offset>=16&&entry.offset<=archive.entries[i+1].offset);CHECK(entry.size>0);std::vector<u8> bytes(entry.size);CHECK(archive.read(entry.name,bytes.data(),files.archives)==bytes.data());hash=hash_bytes(bytes.data(),bytes.size(),hash);total+=bytes.size();if(std::strstr(entry.name,".anm"))++animations;}
    std::printf("{\"suite\":\"retail-archive\",\"entries\":%d,\"decoded_bytes\":%llu,\"decoded_hash\":\"%016llx\",\"anm_files\":%u}\n",archive.count,(unsigned long long)total,(unsigned long long)hash,animations);
}
} // namespace
int main(){
    std::puts("{\"suite\":\"scope\",\"runtime\":\"wasm32-wasip1\",\"timing_informational\":true,\"gpu\":false,\"browser\":false,\"mobile\":false,\"full_game_fps\":false,\"filter_bits\":{\"geometry\":1,\"children\":2,\"resume\":4,\"special_draw\":8,\"writable_bytecode\":16}}");
    arithmetic_mode(Precision::Single,Rounding::NearestEven);Host host;browser::FileSystem files(host);CHECK(files.attach_archive("/input/th10.dat"));
    inspect_archive(files);auto manager=std::make_unique<AnmManager>();Resources resources(files,*manager);
    for(i32 i=0;i<files.resources.count;++i){const auto& entry=files.resources.entries[i];if(!std::strstr(entry.name,".anm"))continue;
        std::vector<u8> bytes(entry.size);CHECK(files.resources.read(entry.name,bytes.data(),files.archives)==bytes.data());auto scripts=inspect_anm(bytes.data(),bytes.size());
        auto* file=manager->load(0,entry.name,resources);CHECK(file&&manager->resources_ready());CHECK(file->script_count==i32(scripts.size()));
        unsigned safe=0,instructions=0;std::array<bool,94> ops{};for(const auto& script:scripts){safe+=script.cpu_safe;instructions+=script.instructions;for(unsigned n=0;n<ops.size();++n)ops[n]=ops[n]||script.opcodes[n];}
        std::printf("{\"suite\":\"retail-anm\",\"name\":\"%s\",\"textures\":%d,\"sprites\":%d,\"scripts\":%d,\"instructions\":%u,\"cpu_safe_scripts\":%u,\"opcodes\":[",entry.name,file->texture_count,file->sprite_count,file->script_count,instructions,safe);
        bool first=true;for(unsigned op=0;op<ops.size();++op)if(ops[op]){std::printf("%s%d",first?"":",",int(op)-1);first=false;}std::puts("]}");
        if(std::strcmp(entry.name,"bullet.anm")==0){
            std::printf("{\"suite\":\"retail-script-selection\",\"file\":\"bullet.anm\",\"total\":%zu,\"selected\":%u,\"excluded\":[",scripts.size(),safe);bool first_excluded=true;for(const auto& script:scripts)if(!script.cpu_safe){std::printf("%s{\"script\":%d,\"reason_bits\":%u}",first_excluded?"":",",script.index,script.excluded_paths);first_excluded=false;}std::puts("]}");
        }
        if(std::strcmp(entry.name,"bullet.anm")==0||std::strcmp(entry.name,"enemy.anm")==0||std::strcmp(entry.name,"pl00.anm")==0||std::strcmp(entry.name,"pl01.anm")==0){
            const auto before=copy_bytes(file->loaded,entry.size);auto selected=survey_scripts(*manager,*file,scripts);CHECK(std::memcmp(file->loaded,before.data(),before.size())==0);
            if(std::strcmp(entry.name,"bullet.anm")==0){dense_scripts(*manager,*file,selected,1200,entry.size);dense_scripts(*manager,*file,selected,4096,entry.size);survey_geometry(*manager,*file,scripts);CHECK(std::memcmp(file->loaded,before.data(),before.size())==0);}
        }
        manager->unload(0,resources);
    }
    std::printf("{\"suite\":\"retail-textures\",\"uploads\":%u,\"bytes\":%llu,\"hash\":\"%016llx\",\"gpu\":false}\n",resources.textures.uploads,(unsigned long long)resources.textures.upload_bytes,(unsigned long long)resources.textures.upload_hash);
    std::puts("{\"suite\":\"result\",\"passed\":true}");
}
