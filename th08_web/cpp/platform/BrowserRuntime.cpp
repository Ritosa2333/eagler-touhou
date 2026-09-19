#include "BrowserRuntime.hpp"
#ifdef TH_NATIVE_PLATFORM
#include "../sdl/ThpracUi.hpp"
#include "Renderer.hpp"
#endif
#include "PlatformDevices.hpp"
#include "GameAudioManager.hpp"
#include "../game/Presentation.hpp"
#include <algorithm>
#include <cstdio>
#ifdef TH_SDL3
#include "../sdl/GraphicsHost.hpp"
#endif
#ifdef TH_NATIVE_PLATFORM
#include "../sdl/PlatformHost.hpp"
#endif
namespace th08 {
namespace {
u32 ptr(const void* p){return reinterpret_cast<uintptr_t>(p);}
PixelSurface surface(TextureRecord& r){auto& i=r.image;return {i.format,i.width,i.height,i.width*TexturePixels::describe(i.format).bytes,i.pixels.data()};}
}
BrowserRuntime::BrowserRuntime():app(*this,graphics){audio=std::make_unique<GameAudioManager>(*this);text=std::make_unique<AnmText>(app.textures,fonts);app.textures.prepare_callback=[](void*,u32 handle){graphics_device().prepare_texture(handle);};}
BrowserRuntime::~BrowserRuntime(){app.shutdown();flush();if(pending_capture.target)app.textures.release(pending_capture.target);audio->shutdown();for(auto& s:surfaces)app.textures.release(s.second);app.textures.release(back);}
std::string BrowserRuntime::path(const char* p){return ResourceManager::path(p);}
bool BrowserRuntime::put(const char* p,const u8* b,u32 n){return resources_.put(p,b,n);}
bool BrowserRuntime::put_archive(const u8* b,u32 n){return !prepared&&resources_.put_archive(b,n);}
bool BrowserRuntime::put_font(i32 kind,const u8* b,u32 size){return kind==0?fonts.encoding.load(b,size):kind==1?fonts.load_blend(b,size):fonts.load_font(b,size);}
bool BrowserRuntime::put_image(const char* name,u32 w,u32 h,const u8* b,u32 size){if(!name||!b||!w||!h||w>4096||h>4096||u64(w)*h*4!=size)return false;TexturePixels image;if(!image.create(w,h,21))return false;for(u32 i=0;i<size;i+=4){image.pixels[i]=b[i+2];image.pixels[i+1]=b[i+1];image.pixels[i+2]=b[i];image.pixels[i+3]=b[i+3];}
#ifdef TH_NATIVE_PLATFORM
    const auto key=path(name);images.erase(key);image_use.erase(key);size_t total=size;for(const auto& entry:images)total+=entry.second.pixels.size();
    while(total>8*1024*1024&&!images.empty()){auto oldest=image_use.begin();for(auto it=image_use.begin();it!=image_use.end();++it)if(it->second<oldest->second)oldest=it;
        total-=images.find(oldest->first)->second.pixels.size();images.erase(oldest->first);image_use.erase(oldest);}
    image_use[key]=++cache_clock;
#endif
    images[path(name)]=std::move(image);return true;}
std::vector<u8> BrowserRuntime::read(const char* p){return resources_.read(p);}
std::vector<u8> BrowserRuntime::read_prefix(const char* p,u32 size){return resources_.read_prefix(p,size);}
const std::vector<u8>& BrowserRuntime::file(const char* p){return resources_.file(p);}
bool BrowserRuntime::write(const char* p,const u8* b,u32 size){
    const auto name=path(p);std::vector<u8> extended;
    // Upstream th08_save_replay appends the thprac 'USER'/'PRAC' block right
    // after the vanilla file; the touch movement trailer stays last so .rpyx
    // detection keeps working. Upstream saves whenever thPracParam.mode is set
    // (advanced practice), with no assist/cheat gate, so assisted runs still
    // carry their parameters. Original-mode runs carry no PRAC block, and a
    // config that cannot be serialized simply saves a vanilla replay.
    if(name.find("replay/")==0&&name.size()>4&&name.substr(name.size()-4)==".rpy"&&app.session.practice.active&&!app.session.practice.replay){
        const auto tail=practice_replay_block(app.session.practice.run);
        if(!tail.empty()){extended.assign(b,b+size);extended.insert(extended.end(),tail.begin(),tail.end());b=extended.data();size=extended.size();}
    }
    if(name.find("replay/")==0&&name.size()>4&&name.substr(name.size()-4)==".rpy"&&motion.used()&&!motion.playing){
        const auto tail=motion.trailer(8);if(tail.empty())return false;
        if(extended.empty())extended.assign(b,b+size);extended.insert(extended.end(),tail.begin(),tail.end());b=extended.data();size=extended.size();
    }
    if(!put(p,b,size))return false;return file_device().save(p,b,size);
}
bool BrowserRuntime::player_motion(const PlayerMovementState& state,float speed,const FrameTiming& timing,float& x,float& y){
    const auto& g=app.game;if((g.globals.game_flags&516)!=4)return false;
    if(motion.playing)return motion.playback(g.globals.stage,x,y);
    const bool enabled=motion.active&&!g.dialogue.present()&&!g.paused&&!g.menus.context.pause_state&&!g.player_state.context.game_over;
    if(enabled){
        const float sx=state.multiplier.x*timing.rate,sy=state.multiplier.y*timing.rate;
        x=sx?(motion.target_x-state.position.x)/sx:0;y=sy?(motion.target_y-state.position.y)/sy:0;
        if(!motion.unlimited)touhou::input::limit_vector(x,y,speed);
        motion.mark_cheat_movement(x,y);
    }
    motion.record(g.globals.stage,enabled,x,y);return enabled;
}
i32 BrowserRuntime::replay_touch_points(ReplayTouchPoint* points,i32 capacity){
    if(!points||capacity<=0)return 0;touhou::input::MotionTrack::TouchPoint source[10];const i32 limit=std::min<i32>(capacity,10),count=motion.replay_points(source,limit);
    for(i32 i=0;i<count;++i)points[i]={source[i].x,source[i].y};return count;
}
std::vector<std::string> BrowserRuntime::user_replays(){return resources_.user_replays();}
void BrowserRuntime::calendar(char date[6],char stamp[20]){file_device().calendar(date,stamp);}
u32 BrowserRuntime::milliseconds(){return file_device().milliseconds();}
u64 BrowserRuntime::performance_counter(){return u64(milliseconds())*1000;}
u16 BrowserRuntime::poll_input(){const auto touch=u16(file_device().supplemental_input());return input.controller(InputController::keyboard(keys,false)|touch,pad,app.session.display_config);}
void BrowserRuntime::Graphics::bind_texture(u32 h){graphics_device().texture(h);}
#ifdef TH_NATIVE_PLATFORM
PipelineState& BrowserRuntime::Graphics::pipeline(){return graphics_device().pipeline();}
void BrowserRuntime::Graphics::destination_blend(BlendParameter b){set_destination_blend(b);}
void BrowserRuntime::Graphics::write_depth(bool e){set_depth_mask(e);}
#else
void BrowserRuntime::Graphics::destination_blend(BlendParameter b){render_state(20,b);}
void BrowserRuntime::Graphics::write_depth(bool e){render_state(14,e);}
#endif
void BrowserRuntime::Graphics::triangles(const SpriteVertex* v,u32 n){
    // Original AnmManager::FlushVertexBuffer (00462e40) restores both
    // diffuse arguments on every sprite batch. A preceding 3D background
    // uses TFACTOR, which can be black/transparent at the end of stage 3's
    // scenery. Keeping that state hides the player, bullets and HUD.
    RenderCommands(*this).SetDiffuseArg(TextureArg::Diffuse);
    draw(Primitive::Triangles,VertexFormat::Screen,v,n);
}
void BrowserRuntime::Graphics::transform(MatrixParameter k,const Matrix4& m){graphics_device().transform(k,m);}
void BrowserRuntime::Graphics::set_viewport(const Viewport& v){graphics_device().viewport(v);}
#ifdef TH_NATIVE_PLATFORM
void BrowserRuntime::Graphics::texture_factor(u32 c){set_texture_factor(c);}
#else
void BrowserRuntime::Graphics::texture_factor(u32 c){render_state(60,c);}
void BrowserRuntime::Graphics::stage_state(u32 s,u32 v){graphics_device().stage_state(s,v);}
void BrowserRuntime::Graphics::render_state(u32 s,u32 v){graphics_device().render_state(s,v);}
#endif
void BrowserRuntime::Graphics::draw(Primitive p,VertexFormat f,const void* v,u32 n){graphics_device().draw(p,f,v,n);}
void BrowserRuntime::Graphics::clear_target(u32 flags,u32 color,float depth,u32 stencil){graphics_device().clear(flags,color,depth,stencil);}
void BrowserRuntime::flush(){app.renderer.flush();graphics_device().flush();}
void BrowserRuntime::readback(u32 h){flush();graphics_device().read(h);}
const BrowserTexture* BrowserRuntime::texture(u32 h){const auto* r=app.textures.get(h);if(!r)return nullptr;const auto& i=r->image;texture_result={h,i.width,i.height,i.format,i.width*TexturePixels::describe(i.format).bytes,ptr(i.pixels.data()),u32(i.pixels.size()),r->revision};return &texture_result;}
void BrowserRuntime::begin_frame(){}
bool BrowserRuntime::present(){
    flush();
#ifdef TH_NATIVE_PLATFORM
    if(auto* renderer=touhou::sdl::current())ThpracUi::render(*this,*renderer);
#endif
    if(presentation::render_only)return graphics_device().present(back)&&!capture_failed;
    captured=false;const bool presented=graphics_device().present(back);finish_capture();return presented&&!capture_failed;
}
void BrowserRuntime::discard_graphics(){app.renderer.clear();graphics_device().discard();}
void BrowserRuntime::reset_device(){
    flush();const auto options=app.session.display_config.options;
#ifdef TH_NATIVE_PLATFORM
    graphics.set_viewport({});graphics.configure_game(4);graphics.set_depth_test(!(options&64));graphics.set_fog(!(options&1024));
    if(options&256)RenderCommands(graphics).SetColorOp(ColorOp::SelectFirst);
    if(options&2)RenderCommands(graphics).SetDiffuseArg(TextureArg::Diffuse);
    app.renderer.fog_enabled=!(options&1024);
#else
    graphics.set_viewport({});graphics.render_state(7,!(options&64));
    const u32 states[][2]={{137,0},{22,1},{27,1},{9,2},{19,5},{20,6},{23,8},{15,1},{24,4},{25,7},{38,0x3f800000},{35,0},{140,3},{34,0xffa0a0a0},{36,0x447a0000},{37,0x459c4000},{40,0},{161,0}};
    for(auto& s:states)graphics.render_state(s[0],s[1]);graphics.render_state(28,!(options&1024));app.renderer.fog_enabled=!(options&1024);
    graphics.stage_state(4,options&256?2:4);graphics.stage_state(1,options&256?2:4);
    const u32 stages[][2]={{5,2},{6,3},{2,2},{3,3},{18,0},{16,2},{17,2},{24,2},{25,3},{13,1},{14,1}};
    for(auto& s:stages)graphics.stage_state(s[0],s[1]);if(options&2){graphics.stage_state(6,0);graphics.stage_state(3,0);}
#endif
    app.renderer.begin_background();app.renderer.screen_camera();
}
bool BrowserRuntime::load_surface(i32 slot,const char* p){const auto key=path(p);auto found=images.find(key);if(found==images.end()){const auto slash=key.rfind('/');if(slash!=std::string::npos)found=images.find(key.substr(slash+1));}
#ifdef TH_NATIVE_PLATFORM
    if(found==images.end()){auto bytes=read(p);if(!sdl_decode_image(*this,key.c_str(),bytes))return false;found=images.find(key);}
    image_use[found->first]=++cache_clock;
#endif
    if(found==images.end())return false;release_surface(slot);auto copy=found->second;surfaces[slot]=app.textures.insert(std::move(copy));return true;}
void BrowserRuntime::release_surface(i32 slot){auto found=surfaces.find(slot);if(found!=surfaces.end()){flush();app.textures.release(found->second);surfaces.erase(found);}}
bool BrowserRuntime::has_surface(i32 slot){return surfaces.count(slot);}
void BrowserRuntime::draw_surface(i32 slot,i32 x,i32 y){auto found=surfaces.find(slot);if(found==surfaces.end())return;auto* r=app.textures.get(found->second);if(!r)return;flush();const TextureRect region{x,y,std::min<i32>(x+640,r->image.width),std::min<i32>(y+480,r->image.height)};if(region.right>x&&region.bottom>y){graphics_device().copy(found->second,region,back,0,0);app.textures.changed(back);}}
void BrowserRuntime::capture_screen(i32 slot){release_surface(slot);TexturePixels copy;auto* source=app.textures.get(back);if(!source||!copy.create(640,480,source->image.format))return;const auto h=app.textures.insert(std::move(copy),0,true);surfaces[slot]=h;flush();graphics_device().copy(back,{0,0,640,480},h,0,0);app.textures.changed(h);captured=true;}
bool BrowserRuntime::capture(u32 target,const TextureRect& src,const TextureRect& dst,bool triangle){
    if(pending_capture.target)return true; // The original keeps the first queued capture.
    auto* a=app.textures.get(back);auto* b=app.textures.get(target);if(!a||!b||!TextureResample::valid(surface(*a),src)||!TextureResample::valid(surface(*b),dst))return false;
    app.textures.retain(target);pending_capture={target,src,dst,triangle};return true;
}
void BrowserRuntime::finish_capture(){
    if(!pending_capture.target)return;const auto request=pending_capture;pending_capture={};
    auto* a=app.textures.get(back);auto* b=app.textures.get(request.target);
    if(!a||!b)capture_failed=true;
    else if(graphics_device().resample(back,request.source,request.target,request.destination,request.triangle)){app.textures.changed(request.target);}
    else{readback(back);readback(request.target);auto from=surface(*a),to=surface(*b);const auto result=request.triangle?TextureResample::triangle(to,request.destination,from,request.source):TextureResample::point(to,request.destination,from,request.source);capture_failed|=result<0;if(result>=0)app.textures.changed(request.target);}
    app.textures.release(request.target);
}
bool BrowserRuntime::capture_arcade(const AnmLoadedSprite& s){const i32 x=number(s.startPixelInclusive.x).truncate_int(),y=number(s.startPixelInclusive.y).truncate_int();return capture(s.texture,{32,16,416,464},{x,y,x+number(s.widthPx).truncate_int(),y+number(s.heightPx).truncate_int()},true);}
bool BrowserRuntime::capture_texture(const TextureCaptureRequest& r){const auto* a=app.library.get(r.index);if(!a||!a->spriteCount)return false;return capture(a->sprites[0].texture,{r.source_x,r.source_y,r.source_x+r.source_width,r.source_y+r.source_height},{r.dest_x,r.dest_y,r.dest_x+r.dest_width,r.dest_y+r.dest_height},true);}
bool BrowserRuntime::draw(AnmVm& v,TextAlignment a,u32 c,u32 o,const char* s){if(v.loadedSprite)readback(v.loadedSprite->texture);return text->draw(v,a,c,o,s);}
void BrowserRuntime::begin(bool disable_fog){if(disable_fog)app.renderer.set_fog(false);}
void BrowserRuntime::rectangle(const OverlayRect& r,u32 c){const u32 colors[4]{c,c,c,c};app.renderer.draw_rectangle(r.left,r.top,r.right,r.bottom,colors);}
bool BrowserRuntime::initialize(){
    if(prepared||!fonts.encoding.loaded())return false;arithmetic_mode(Precision::Single,Rounding::NearestEven);
    if(!audio->prepare_formats())return false;
    TexturePixels pixels;if(!pixels.create(640,480,22))return false;back=app.textures.insert(std::move(pixels),0,true);
    reset_device();if(!audio->prepare_samples())return false;
    prepared=true;if(!app.initialize(1000000))return false;reset_device();return true;
}
bool BrowserRuntime::step(bool render){return prepared&&app.update()&&(!render||app.draw())&&!capture_failed;}
i32 BrowserRuntime::status(i32 field)const{switch(field){case 0:return app.supervisor.state.active;case 1:return app.active();case 2:return app.invalid()||capture_failed;case 3:return app.game.globals.stage;case 4:return app.game.faults()|(u32(capture_failed)<<9);case 5:return app.textures.live_count();case 6:return app.game.enemies.state.frames;case 7:return app.loading_game();case 8:return app.title.menus.state.currentScreen;case 9:return app.title.menus.state.cursor;case 10:return app.title.modal();default:return -1;}}
void BrowserRuntime::sound(i32 i,i32 m,float x,bool p){audio->sound(i,m,x,p);}
bool BrowserRuntime::play_music(i32 i,i32 s){return audio->play_music(i,s);}
void BrowserRuntime::load_music(i32 i,const char* p){audio->load_music(i,p);}
void BrowserRuntime::play_audio(const char* p,i32 s){audio->play_audio(p,s);}
void BrowserRuntime::stop_audio(){audio->stop_audio();}
void BrowserRuntime::fade_music(float s){audio->fade_music(s);}
void BrowserRuntime::menu_music(MenuMusic m,float s){audio->menu_music(m,s);}
void BrowserRuntime::midi_reset(){audio->midi_reset();}
void BrowserRuntime::start_bgm(){audio->start_bgm();}
void BrowserRuntime::process_sounds(){audio->process_sounds();}
void BrowserRuntime::update_audio_fades(){audio->update_audio_fades();}
void BrowserRuntime::apply_volume(const GameConfiguration& c){audio->apply_volume(c);}
bool BrowserRuntime::audio_tick(u32 now){return audio->audio_tick(now);}
void BrowserRuntime::replay_error(){file_device().replay_error();}
}
