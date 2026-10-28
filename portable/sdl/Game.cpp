#include "Game.hpp"
#include "RuntimeOptions.hpp"
#include "BinaryReader.hpp"
#include "ResourceImages.hpp"
#include "game/GameState.hpp"
#include "game/StageFlow.hpp"
#include "game/Practice.hpp"
#include "game/PracticeTools.hpp"
#include "wasm/Api.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>

// Exported by core/wasm/Exports.cpp but not declared in Api.hpp.
extern "C" const int32_t* th12_practice_input();

namespace th12host {
std::vector<MusicTrackInfo> prepared_music_comments();
namespace {
std::string two(i32 n){char b[8];std::snprintf(b,sizeof b,"%02d",n);return b;}
// StageInfo +10 (road theme) per stage, for continues (stage-loader.mjs stageMusic).
constexpr const char* stage_music[8]{"","th12_00","th12_04","th12_07","th12_09","th12_13","th12_16","th12_18"};
// The core's spell history JSON ({"records":[[[a,c],...],...]}) as 1792 words.
std::vector<i32> core_spell_history(){
    std::vector<i32> words;const char* p=th12_spell_history();const int n=th12_spell_history_size();
    for(int i=0;i<n&&words.size()<1792;++i)if(p[i]>='0'&&p[i]<='9'){i64 v=0;while(i<n&&p[i]>='0'&&p[i]<='9')v=v*10+(p[i++]-'0');words.push_back(i32(std::min<i64>(v,99999)));}
    return words;
}
}
Game::Game()=default;
Game::~Game(){shutdown();}
void Game::shutdown(){
    if(!scene)return;
    // Closing from the HOME menu mid-run keeps play time, unlocks and card history.
    if(menus)save_profile();
    prefetch.reset();practice_ui.shutdown(gpu);scene->drop_text_cache();
    for(auto& b:banks.slots)if(b){Renderer::release(*b);b.reset();}
    ending.reset();title.reset();menus.reset();signature.reset();indicator.reset();retiring.clear();scene.reset();gpu.shutdown();
}
bool Game::read(const std::string& name,std::vector<u8>& out){if(archive.read(name,out))return true;error=archive.error;return false;}
bool Game::load_bank(i32 bank,const std::string& name,bool optional){
    const std::string file=name+".anm";
    if(optional&&!archive.find(file)){if(banks.slots[size_t(bank)]){Renderer::release(*banks.slots[size_t(bank)]);banks.slots[size_t(bank)].reset();}files.erase("bank"+std::to_string(bank));return true;}
    std::vector<u8> bytes;if(!read(file,bytes))return false;
    auto anm=std::make_shared<AnmFile>();std::string why;if(!parse_anm(name,bytes,*anm,why)||!apply_texture_overrides(*anm,why)){error=why;return false;}
    gpu.upload(*anm);
    if(auto& old=banks.slots[size_t(bank)])Renderer::release(*old);
    banks.slots[size_t(bank)]=anm;files["bank"+std::to_string(bank)]=std::move(bytes);return true;
}

bool Game::boot(){
    const auto& paths=host::paths();
    load_translations();
    if(!archive.open(host::exists(paths.data_file("th12.dat"))?paths.data_file("th12.dat"):paths.data)){error="th12.dat: "+archive.error;return false;}
    std::vector<u8> exe_bytes;
    if(host::read_file(paths.data_file("th12.exe"),exe_bytes,64*1024*1024)){
        if(!exe.load(exe_bytes)){error=exe.error;return false;}
    }else if(!host::read_file(paths.data_file("native-tables.bin"),exe_bytes,1024*1024)||!exe.load_prepared(exe_bytes)){
        error="Original EXE tables are missing: supply th12.exe or the prepared platform tables";return false;
    }
    host::log("th12.exe: %s (%s)",exe.version.empty()?"unknown version, tables look valid":exe.version.c_str(),exe.sha256.c_str());
    if(!gpu.initialize(2,error)){error="renderer: "+error;return false;}
    if(!text.initialize())host::log("text: no Japanese font found; dialogue text is blank");
    host::log("text fonts: %s",text.font_names().c_str());
    if(!audio.initialize(archive,exe))host::log("audio: %s",audio.error.c_str());
    practice_ui.initialize(gpu);
    profile.load(paths.save);audio.set_volumes(profile.bgm_volume(),profile.se_volume());
    std::vector<u8> comments;if(archive.read("musiccmt.txt",comments))music_tracks=parse_music_comments(comments);else music_tracks=prepared_music_comments();
    scene=std::make_unique<Scene>(gpu,text,banks,exe);
    // 43f320: the original sig.anm splash first, then ascii.anm's indicator.
    if(!load_bank(BankSig,"sig")||!load_bank(BankAscii,"ascii"))return false;
    loading_sampler.files=[this](i32 bank){return banks.get(bank);};loading_sampler.display_random32=[this]{return next_loading_random();};
    ending_sampler.files=loading_sampler.files;ending_sampler.display_random32=loading_sampler.display_random32;
    signature=std::make_unique<MenuAnimation>(loading_sampler,banks.get(BankSig),BankSig,0);signature->tick();
    show_indicator();
    for(const auto& [bank,name]:std::initializer_list<std::pair<i32,const char*>>{{BankBullet,"bullet"},{BankEnemy,"enemy"},{BankPlayer,"pl00"},{BankPlayer+1,"pl01"},{BankPlayer+2,"pl02"},{BankFront,"front"},{BankText,"text"},{BankTitle,"title"},{BankTitleV,"title_v"}})
        jobs.push_back([this,bank=bank,name=std::string(name)]{return load_bank(bank,name);});
    jobs.push_back([this]{
        for(const char* name:{"default.ecl","pl00a.sht","pl00b.sht","pl01a.sht","pl01b.sht","pl02a.sht","pl02b.sht"})if(!read(name,files[name]))return false;
        return true;});
    after_jobs=[this]{startup_ready=true;};
    phase=Phase::Startup;return true;
}
// Upstream clearKeys: no input carries over a scene change as a new press.
void Game::clear_keys(){
    if(menus)menus->clear_keys(previous_ui);if(session)session->previous=previous_ui&0x2ff;
}
void Game::save_profile(){
    // A replay's cards are not the player's history.
    if(session&&!session->replay)profile.set_spell_history(core_spell_history());
    profile.save();
}
u32 Game::next_loading_random(){
    const u32 a=((loading_seed^0x9630)-0x6553)&65535,b=((a<<2)|(a>>14))&65535,c=((b^0x9630)-0x6553)&65535;
    loading_seed=((c<<2)|(c>>14))&65535;return (a<<16)|c;
}
// 411b80 binds ascii 18 at (480,392); 411b00 sends IRQ 1 and clears its handle.
void Game::show_indicator(){if(!indicator&&banks.get(BankAscii)){indicator=std::make_unique<MenuAnimation>(loading_sampler,banks.get(BankAscii),BankAscii,18);indicator->tick();}}
void Game::retire_indicator(){if(!indicator)return;indicator->interrupt(1);retiring.push_back(std::move(indicator));}
void Game::tick_loading(){
    if(signature)signature->tick();if(indicator)indicator->tick();
    for(auto& vm:retiring)vm->tick();retiring.erase(std::remove_if(retiring.begin(),retiring.end(),[](auto& vm){return !vm->alive();}),retiring.end());
}

void Game::queue_stage(i32 stage){
    const std::string n=two(stage);
    jobs.push_back([this,stage,n]{
        std::vector<std::string> names{"stage"+n+".ecl","stage"+n+".std"};if(stage==7)names.push_back("stage07boss.ecl");
        for(i32 i=0;i<6;++i)names.push_back("st"+n+"_0"+std::to_string(i>>1)+(i%2?"b":"a")+".msg");
        for(const auto& name:names)if(!read(name,files[name]))return false;
        StdGeometry g;std::string why;if(!g.parse(files["stage"+n+".std"],why)){error="stage"+n+".std: "+why;return false;}
        scene->set_geometry(stage,std::move(g));return true;});
    jobs.push_back([this,n]{return load_bank(BankStageEnemy,"stgenm"+n);});
    jobs.push_back([this,n]{return load_bank(BankStageEnemyM,"stgenm"+n+"m",true);});
    jobs.push_back([this,n]{return load_bank(BankLogo,"st"+n+"logo");});
    jobs.push_back([this,n]{return load_bank(BankStage,"stage"+n);});
}
// Upstream StageLoader.load: select the stage, then hand the core every file.
bool Game::load_core_stage(i32 stage){
    const std::string n=two(stage);th12_select_stage(stage);
    const bool alternate=files.count("bank"+std::to_string(BankStageEnemyM))>0;if(!alternate)th12_unload_anm(3);
    const auto give=[&](const std::string& key,const std::function<int(const u8*,int)>& load){
        const auto it=files.find(key);if(it==files.end()||load(it->second.data(),int(it->second.size()))!=1){error="invalid original resource "+key;return false;}return true;};
    std::vector<std::string> ecl{"default.ecl","stage"+n+".ecl"};if(stage==7)ecl.push_back("stage07boss.ecl");
    for(const auto& e:ecl)if(!give(e,th12_load_ecl))return false;
    if(!give("stage"+n+".std",th12_load_std))return false;
    for(i32 i=0;i<6;++i){const std::string tail=std::to_string(i>>1)+(i%2?"b":"a");
        if(!give("pl0"+tail+".sht",[i](const u8* p,int s){return th12_load_sht(i,p,s);}))return false;
        if(!give("st"+n+"_0"+tail+".msg",[i](const u8* p,int s){return th12_load_msg(i,p,s);}))return false;}
    std::vector<i32> order{0,1,2,4,5,6,7,8,11,9,10};if(alternate)order.push_back(3);
    for(i32 bank:order)if(!give("bank"+std::to_string(bank),[bank](const u8* p,int s){return th12_load_anm(bank,p,s);}))return false;
    return true;
}


// Upstream begin(): a fresh run starts at stage 1; Practice keeps its stage;
// Extra is stage 7; a replay or the demo starts at its chosen stage.
void Game::begin(MenuRun run,std::shared_ptr<const Replay> replay){
    const bool playback=bool(replay);
    const i32 stage=playback?run.stage:run.mode==ModePractice?run.stage:run.mode==ModeExtra?7:1;
    const i32 difficulty=playback?replay->info.difficulty:run.mode==ModeExtra?4:run.difficulty;
    if(session&&!session->replay)save_profile();
    if(run.mode!=ModePractice)custom_practice=false;
    prefetch.reset();
    phase=Phase::Loading;held_frame=false;show_indicator();audio.stop_effects();ending.reset();pending_result.reset();
    queue_stage(stage);
    after_jobs=[this,run,stage,difficulty,replay]{
        translation_context("st"+two(stage)+"_0"+std::to_string(run.character)+(run.shot?"b":"a")+".msg");
        th12_practice_reset(0);th12_practice_replay(replay?1:0);
        th12_practice_damage_begin(replay?1:0,run.mode==ModePractice?1:0,run.character,run.shot);
        if(!load_core_stage(stage))return;
        const auto words=profile.spell_history();
        if(th12_load_spell_history(words.data(),int(words.size()))!=1)host::log("spell history rejected");
        Session n;n.run=run;n.run.difficulty=difficulty;n.run.stage=stage;n.stage=stage;n.replay=replay;
        if(replay){
            const ReplayStage* part=replay->stage(stage);if(!part||!start_replay_stage(*replay,*part,run.mode!=ModeDemo,false)){error="replay stage could not start";return;}
        }else{
            const u32 seed=fixed_seed>=0?u32(fixed_seed)&65535:u32(std::chrono::steady_clock::now().time_since_epoch().count())&65535;
            const bool custom=run.mode==ModePractice&&custom_practice&&runtime_options.thprac;
            const bool prepared=!custom||th12_practice_configure(practice_params.data(),int(practice_params.size()))==1;
            const int started=prepared?(custom?th12_practice_start(run.character,run.shot,difficulty,int(seed)):th12_start_full_game(run.character,run.shot,difficulty,int(seed),run.mode==ModePractice?1:0)):0;
            if(started!=1){error="full game initialisation failed";return;}
            if(!custom||practice_params[1]==0)n.recorder=std::make_unique<ReplayRecorder>(run.character,run.shot,difficulty,run.mode==ModePractice,profile.config_bytes());
            profile.record_play_start(run.character,run.shot);
        }
        // The HUD's HI SCORE starts from the score file's best (Practice: that stage's).
        if(run.mode==ModePractice)n.high_score=double(profile.practice_score(run.character,run.shot,difficulty,stage))*10;
        else n.high_score=double(profile.rankings(run.character,run.shot,difficulty)[0].score());
        n.music_serial=th12_music_serial();session=std::move(n);
        if(!replay)record_stage();
        menus->run=session->run; // upstream model.run: Replay Again restarts from this stage
        scene->reset_presentation();clear_keys();
        // 4221bd / 422928: the demo keeps the title theme playing.
        if(run.mode!=ModeDemo){audio.play_music(th12_music_track());if(const auto id=audio.music_name();!replay&&!id.empty())profile.unlock_music(id);}
        retire_indicator();phase=Phase::Menu;captured=false;
        host::log("start: mode %d character %d shot %d difficulty %d stage %d",run.mode,run.character,run.shot,difficulty,stage);
    };
}
// upstream startReplayStage: the stage header's 17 words and 21 spell times.
bool Game::start_replay_stage(const Replay& replay,const ReplayStage& stage,bool flow,bool fade_in){
    std::array<i32,38> words{};const auto initial=stage.initial.words();
    std::copy(initial.begin(),initial.end(),words.begin());std::copy(stage.spell_times.begin(),stage.spell_times.end(),words.begin()+17);
    if(th12_start_replay(replay.info.character,replay.info.shot,replay.info.difficulty,stage.seed,words.data(),38)!=1)return false;
    if(flow&&!replay.info.practice)th12_replay_stage_flow(0,fade_in?1:0);
    return true;
}
// The stage boundary the core recorded before the main routine's birth.
void Game::record_stage(){
    const auto& f=th12::fullGameFlow;if(!session||!session->recorder||!f.recordHeaderReady)return;
    const auto& h=f.recordHeader;const auto& r=h.resources;ReplayInitial in;
    in.score=r.scoreStored;in.power=r.power;in.point_value=r.pointValueStored;in.lives=r.lives;in.life_fragments=r.lifeFragments;in.bombs=r.bombs;in.bomb_fragments=r.bombFragments;
    in.rank=r.rank;in.x=h.x;in.y=h.y;for(int i=0;i<3;++i)in.ufo[size_t(i)]=h.inventory.colors[size_t(i)];for(int i=0;i<4;++i)in.globals[size_t(i)]=h.nativeGlobals[size_t(i)];
    session->recorder->begin_stage(f.stage,h.seed,in);session->result_key.clear();session->spell_count=0;
}
void Game::play_replay(const std::string& file,i32 stage,bool demo){
    std::vector<u8> bytes;auto replay=std::make_shared<Replay>();std::string why;
    const bool found=demo?archive.read(file,bytes):host::read_file(menus->replay_folder()+"/"+file,bytes,64u*1024*1024);
    if(!found||!parse_replay(bytes,*replay,why)){host::log("replay %s: %s",file.c_str(),found?why.c_str():"missing");menus->return_title();actions();return;}
    MenuRun run;run.mode=demo?ModeDemo:ModeReplay;run.character=replay->info.character;run.shot=replay->info.shot;run.difficulty=replay->info.difficulty;
    run.stage=replay->stage(stage)?stage:replay->stages.front().number;run.replay_file=file;
    begin(run,std::move(replay));
}
i32 Game::stage_after()const{
    const auto& s=*session;const ReplayStage* part=s.replay?s.replay->after(s.stage):nullptr;
    return s.replay?(part?part->number:0):th12::fullGameFlow.nextStage;
}
void Game::next_stage(){
    auto& s=*session;
    if(s.run.mode==ModeDemo){menus->return_title();return;}
    if(!s.replay&&s.run.mode!=ModePractice)profile.record_stage_clear(s.run.character,s.run.shot,s.run.difficulty,s.stage,i64(th12_score_stored())*10);
    if(s.recorder)s.recorder->finish_stage();
    const i32 stage=stage_after();
    if(!stage){prefetch.reset();complete_run(true);return;}
    if(prefetch&&prefetch->next.stage==stage){
        // Read during the clear: switch over now, like the original.
        const double t0=host::seconds();const bool waited=!prefetch->done;
        if(prefetch->loader.joinable())prefetch->loader.join();
        StageFiles next=std::move(prefetch->next);prefetch.reset();
        if(next.error.empty()){install_stage(next);enter_stage(stage);host::log("  switched in %.0f ms%s",(host::seconds()-t0)*1000,waited?" (waited for the read)":"");return;}
        host::log("stage %d: %s; loading it again",stage,next.error.c_str());
    }
    prefetch.reset();
    phase=Phase::Loading;held_frame=true;show_indicator();
    queue_stage(stage);
    after_jobs=[this,stage]{enter_stage(stage);};
}
void Game::enter_stage(i32 stage){
    if(session)translation_context("st"+two(stage)+"_0"+std::to_string(session->run.character)+(session->run.shot?"b":"a")+".msg");
    if(!load_core_stage(stage))return;
    auto& s=*session;scene->reset_presentation();s.stage=stage;s.previous=0;s.cursor=0;s.result_key.clear();s.spell_count=0;
    if(s.replay){if(!start_replay_stage(*s.replay,*s.replay->stage(stage),s.run.mode!=ModeDemo,true)){error="replay stage could not start";return;}}
    else{if(th12_start_next_stage()!=1){error="stage carry-over failed";return;}record_stage();save_profile();}
    s.music_serial=th12_music_serial();audio.play_music(th12_music_track());if(const auto id=audio.music_name();!s.replay&&!id.empty())profile.unlock_music(id);
    retire_indicator();held_frame=false;phase=Phase::Menu;captured=false;
    host::log("stage %d",stage);
}
// The same files and banks as queue_stage's jobs, without the renderer.
bool Game::read_stage(Archive& archive,i32 stage,StageFiles& out){
    const std::string n=two(stage);std::string why;out.stage=stage;
    std::vector<std::string> names{"stage"+n+".ecl","stage"+n+".std"};if(stage==7)names.push_back("stage07boss.ecl");
    for(i32 i=0;i<6;++i)names.push_back("st"+n+"_0"+std::to_string(i>>1)+(i%2?"b":"a")+".msg");
    for(const auto& name:names)if(!archive.read(name,out.files[name])){out.error="cannot read "+name;return false;}
    if(!out.geometry.parse(out.files["stage"+n+".std"],why)){out.error="stage"+n+".std: "+why;return false;}
    const struct {i32 slot;std::string name;bool optional;} banks[]{{BankStageEnemy,"stgenm"+n,false},{BankStageEnemyM,"stgenm"+n+"m",true},{BankLogo,"st"+n+"logo",false},{BankStage,"stage"+n,false}};
    for(const auto& b:banks){
        StageFiles::Bank bank;bank.slot=b.slot;const std::string file=b.name+".anm";
        if(b.optional&&!archive.find(file))bank.present=false;
        else{if(!archive.read(file,bank.bytes)){out.error="cannot read "+file;return false;}
            bank.anm=std::make_shared<AnmFile>();if(!parse_anm(b.name,bank.bytes,*bank.anm,why)||!apply_texture_overrides(*bank.anm,why)){out.error=why;return false;}}
        out.banks.push_back(std::move(bank));
    }
    return true;
}
void Game::install_stage(StageFiles& next){
    for(auto& b:next.banks){
        auto& slot=banks.slots[size_t(b.slot)];const std::string key="bank"+std::to_string(b.slot);
        if(!b.present){if(slot){Renderer::release(*slot);slot.reset();}files.erase(key);continue;}
        if(!b.uploaded){gpu.upload(*b.anm);b.uploaded=true;}
        if(slot)Renderer::release(*slot);slot=std::move(b.anm);files[key]=std::move(b.bytes);
    }
    next.banks.clear();
    for(auto& [name,bytes]:next.files)files[name]=std::move(bytes);
    scene->set_geometry(next.stage,std::move(next.geometry));
}
Game::Prefetch::~Prefetch(){
    if(loader.joinable())loader.join();
    for(auto& b:next.banks)if(b.anm)Renderer::release(*b.anm); // never installed; textures uploaded so far
}
// Once per frame of play: start reading the next stage when a clear begins,
// then upload its textures one per frame.
void Game::watch_prefetch(){
    if(!session||phase!=Phase::Menu||session->run.mode==ModeDemo)return;
    if(!prefetch){
        if(th12::fullGameFlow.status!=th12::FullGameStatus::StageClear)return;
        const i32 stage=stage_after();if(!stage)return;
        prefetch=std::make_unique<Prefetch>();prefetch->next.stage=stage;
        Prefetch* p=prefetch.get();
#ifdef __EMSCRIPTEN__
        read_stage(archive,stage,p->next);p->done=true;
#else
        p->loader=std::thread([this,p,stage]{read_stage(archive,stage,p->next);p->done=true;});
#endif
        return;
    }
    if(!prefetch->done)return;
    for(auto& b:prefetch->next.banks)if(b.present&&!b.uploaded){if(gpu.upload_next(*b.anm))break;b.uploaded=true;}
}
MenuContext Game::result_data(bool clear){
    MenuContext c;if(!session)return c;const auto& f=th12::fullGameFlow;
    c.run=session->run;c.stage=session->stage;c.clear=clear;c.continues=session->replay?0:f.continueCount;
    c.can_continue=!session->replay&&th12::canContinueFullGame();
    c.can_save_replay=!session->replay&&session->recorder&&f.continueCount==0;
    c.ranked=!session->replay&&f.continueCount==0;c.score_stored=th12_score_stored();c.timestamp=host::unix_time();c.slowdown=0;
    return c;
}
// upstream recordClear: the stage clear and the clear count, once a run.
void Game::record_clear(const MenuContext& r){
    if(!session||session->replay||r.run.mode==ModePractice||session->clear_recorded||!r.clear)return;session->clear_recorded=true;
    profile.record_stage_clear(r.run.character,r.run.shot,r.run.difficulty,session->stage,i64(r.score_stored)*10);
    profile.record_clear(r.run.character,r.run.shot,r.run.difficulty,r.continues,session->stage);
}
// Upstream completeRun: commit the clear, then the result flow.
void Game::complete_run(bool clear,bool save_only){
    if(!session)return;pending_result=result_data(clear);record_clear(*pending_result);
    if(session->recorder)session->recorder->finish_stage(true);
    audio.stop_effects();save_profile();
    if(save_only)menus->open_replay_save(*pending_result,profile.last_name(),NameContext::Pause);else menus->finish(*pending_result);
    clear_keys();
}
// upstream beginEnding: e00..e11.msg, then staff.msg, with their ANM files.
void Game::begin_ending(){
    auto& s=*session;const MenuContext result=result_data(true);pending_result=result;
    ending_id=select_ending(s.run.character,s.run.shot,result.continues>0);
    // 410da2 / 4115dd: a first viewing cannot be skipped with Ctrl.
    const bool seen=profile.ending_seen(ending_id);
    record_clear(result);if(s.recorder)s.recorder->finish_stage(true);audio.stop_effects();save_profile();
    menus->enter(MenuScene::Ending,0,[](MenuState& st){st.ready_at=0;});
    char name[16];std::snprintf(name,sizeof name,"e%02d.msg",ending_id);
    auto message=std::make_shared<std::vector<EndingCommand>>(),credits=std::make_shared<std::vector<EndingCommand>>();
    std::vector<u8> bytes;std::string why;
    if(!read(name,bytes)||!decode_ending_message(bytes,*message,why)||!read("staff.msg",bytes)||!decode_ending_message(bytes,*credits,why)){
        host::log("ending: %s %s",name,why.c_str());error.clear();menus->finish(result);clear_keys();return;}
    // The ANM files both messages load (op 7) go into the ending bank slots.
    ending_files.clear();
    for(const auto* list:{message.get(),credits.get()})for(const auto& c:*list)if(c.op==7&&std::find(ending_files.begin(),ending_files.end(),c.file)==ending_files.end()&&ending_files.size()<size_t(BankEndingCount))ending_files.push_back(c.file);
    phase=Phase::Loading;held_frame=false;show_indicator();
    for(size_t i=0;i<ending_files.size();++i){std::string stem=ending_files[i];if(const auto dot=stem.rfind('.');dot!=std::string::npos)stem.erase(dot);
        jobs.push_back([this,i,stem]{return load_bank(BankEnding+i32(i),stem);});}
    after_jobs=[this,message,credits,seen,difficulty=s.run.difficulty]{
        EndingTimeline::Hooks hooks;
        hooks.bank=[this](const std::string& file){for(size_t i=0;i<ending_files.size();++i)if(ending_files[i]==file)return BankEnding+i32(i);return -1;};
        hooks.music=[this](const std::string& track){play_music(track);};
        hooks.fade_music=[this]{audio.fade_music(3);};
        hooks.sound=[this](i32 id){audio.effect(id,0,false);};
        ending=std::make_unique<EndingTimeline>(ending_sampler,BankText,*message,*credits,difficulty,seen,hooks);
        retire_indicator();phase=Phase::Menu;clear_keys();host::log("ending %d",ending_id);
    };
}
// upstream recordRun: the Score Ranking row (or a Practice best) and the name.
void Game::record_run(const MenuContext& r,const std::string& name){
    if(!session||session->replay||session->result_recorded)return;session->result_recorded=true;
    const i32 d=r.run.mode==ModeExtra?4:r.run.difficulty;
    if(r.run.mode==ModePractice){if(!r.continues)profile.record_practice_score(r.run.character,r.run.shot,d,r.stage,r.score());}
    else if(r.ranked&&!r.continues){
        const i32 rank=profile.ranking_place(r.run.character,r.run.shot,d,r.score());
        if(rank>=0){Profile::RankingRow row;row.score_stored=r.score_stored;row.continues=r.continues;row.final_stage=r.clear?8:r.stage;row.name=name;row.timestamp=r.timestamp;row.slowdown=r.slowdown;
            profile.insert_ranking(r.run.character,r.run.shot,d,rank,row);}
    }
    if(!name.empty())profile.set_last_name(name);
    save_profile();
}
// replay/th12_NN.rpy, as the PC game names its 25 slots.
void Game::save_replay(i32 slot,const std::string& name,const MenuContext& r){
    if(!session||!session->recorder||slot<1||slot>25)return;
    const auto bytes=session->recorder->encode(name,r.timestamp,r.score_stored,r.continues,r.clear?8:r.stage,r.slowdown);
    if(bytes.empty())return;
    const std::string folder=menus->replay_folder();host::make_directory(folder);char file[16];std::snprintf(file,sizeof file,"th12_%02d.rpy",slot);
    const bool ok=host::write_file(folder+"/"+file,bytes);host::log("replay %s: %s (%zu bytes)",file,ok?"saved":"not saved",bytes.size());
    profile.set_last_name(name);profile.save();menus->rescan_replays();
}
void Game::return_title(){
    prefetch.reset();
    if(session&&session->run.mode!=ModeDemo)audio.stop_music();
    show_indicator();if(session&&!session->replay)save_profile();session.reset();ending.reset();pending_result.reset();audio.stop_effects();clear_keys();
    retire_indicator();
}
void Game::play_music(const std::string& name){
    audio.play_music(name);if(session&&!session->replay&&!name.empty())profile.unlock_music(name);
}
void Game::game_tick(u32 held,double now){
    auto& s=*session;u32 pressed=held&~s.previous;
    if(s.replay){
        // 43b9ce: the stage's stream ends at its record count or an all-ffff record.
        const auto recorded=s.replay->input(s.stage,s.cursor);
        if(!recorded||recorded->end()){next_stage();return;}
        held=recorded->held;pressed=recorded->pressed;++s.cursor;
    }
    th12_motion(s.replay?0:motion_mode,motion_x,motion_y);th12_tick(int(held),int(pressed));++s.hud_age;
    if(!s.replay){
        // The retail key stream cannot represent adapter movement or tool
        // mutations. Keep those runs playable without writing a false replay.
        if(th12_motion_result()[2]||th12::practice::assisted())s.recorder.reset();
        const int32_t* resolved=th12_practice_input();held=u32(resolved[0]);pressed=u32(resolved[1]);
        if(s.recorder)s.recorder->push(held,pressed,s.previous&~held,std::clamp(fps_value>0?fps_value:60.f,0.f,60.f));
        th12_set_wall_time(now);
    }
    s.previous=held;
    const i32 events=th12_sound_events();const auto* e=static_cast<const th12::sound_system::Event*>(th12_sound_events_ptr());
    for(i32 i=0;i<events;++i)audio.effect(e[i].id,e[i].pan,e[i].stop!=0);
    const u32 serial=th12_music_serial();
    if(serial!=s.music_serial){s.music_serial=serial;if(s.run.mode!=ModeDemo){audio.play_music(th12_music_track());if(const auto id=audio.music_name();!s.replay&&!id.empty())profile.unlock_music(id);}}
    if(s.replay)return;
    profile.record_play_frame(s.run.character,s.run.shot);
    // A declared card's title goes to the score file (Player Data shows it);
    // each result's real time goes to the stage's replay header.
    const auto& spell=th12::spellState;const i32 player=s.run.character*2+s.run.shot;
    if(spell.active()&&!spell.title.empty()&&spell.title!=s.card_title){s.card_title=spell.title;profile.set_spell_title(spell.cardId,player,spell.title);}
    const auto& r=spell.lastResult;
    if(r.kind!=th12::spell_system::ResultKind::None){
        const std::string key=std::to_string(r.cardId)+":"+std::to_string(int(r.kind))+":"+std::to_string(r.elapsedTicks)+":"+std::to_string(r.bonusDisplayed);
        if(key!=s.result_key){s.result_key=key;if(s.spell_count<20&&s.recorder)s.recorder->set_spell_time(s.spell_count++,r.realTimeEncoded);}
    }
}
void Game::handle_flow(){
    if(!session||session->replay)return; // a replay ends with its input stream
    using th12::FullGameStatus;const auto status=th12::fullGameFlow.status;
    if(status==FullGameStatus::NextStage)next_stage();
    else if(status==FullGameStatus::Ending)begin_ending();
    else if(status==FullGameStatus::Result)complete_run(true);
    else if(status==FullGameStatus::GameOver){
        if(session->recorder)session->recorder->finish_stage(true);
        if(th12::canContinueFullGame()){menus->open_continue(result_data(false));clear_keys();}else complete_run(false);
    }
}
// Upstream actions(): the menu's requests.
void Game::actions(){
    for(const auto& a:menus->drain()){
        switch(a.type){
        case MenuAction::Start:begin(a.run);break;
        case MenuAction::Retry:audio.stop_music();begin(a.run);break;
        case MenuAction::PlayReplay:play_replay(a.run.replay_file,a.stage,false);break;
        case MenuAction::Demo:play_replay(a.run.replay_file,1,true);break;
        case MenuAction::Continue:
            audio.stop_effects();
            if(session&&th12_continue_game()==1){
                // A continued run is no longer recorded (it cannot be saved).
                session->recorder.reset();clear_keys();session->music_serial=th12_music_serial();captured=false;play_music(stage_music[std::clamp(session->stage,1,7)]);}
            break;
        case MenuAction::ReturnTitle:return_title();break;
        case MenuAction::Quit:if(session&&!session->replay)save_profile();else profile.save();error.clear();quit=true;break;
        case MenuAction::MenuMusic:
            if(a.music_room){profile.unlock_music(a.track);profile.save();}
            if(a.restart||a.music_room||a.track!=audio.music_name()||audio.music_paused())play_music(a.track);break;
        case MenuAction::MusicWarning:audio.stop_music();break;
        case MenuAction::FadeMusic:audio.fade_music(a.seconds);break;
        case MenuAction::Sound:audio.effect(a.id,0,false);break;
        case MenuAction::Pause:audio.pause_effects();audio.pause_music(true);break;
        case MenuAction::Resume:clear_keys();audio.resume_effects();audio.pause_music(false);break;
        case MenuAction::StopEffects:audio.stop_effects();break;
        case MenuAction::LoadingIndicator:show_indicator();break;
        case MenuAction::Result:if(!a.needs_name)record_run(a.result,profile.last_name());break;
        case MenuAction::SaveName:record_run(a.result,a.name);break;
        case MenuAction::SaveReplay:save_replay(a.slot,a.name,a.result);break;
        case MenuAction::SaveExit:audio.stop_music();complete_run(false,true);break;
        case MenuAction::Settings:{
            const auto& st=menus->settings;profile.set_volumes(st.bgm_volume,st.se_volume);profile.set_joy_buttons(st.joy_buttons);
            audio.set_volumes(st.bgm_volume,st.se_volume);profile.save();break;}
        }
    }
}
// Switch buttons as the game's joystick buttons (Key Config), plus the
// directions. X and Y keep upstream's Enter and Retry keys unless assigned.
u32 Game::map_buttons(const PadSample& pad)const{
    const auto joy=menus?menus->settings.joy_buttons:profile.joy_buttons();static constexpr u32 bits[5]{InShot,InBomb,InFocus,InPause,InSkip};
    u32 held=pad.directions&0xf0;bool x_bound=false,y_bound=false;
    for(int i=0;i<5;++i){const i32 b=joy[size_t(i)];if(b>=0&&b<32&&(pad.buttons>>b&1))held|=bits[i];x_bound|=b==5;y_bound|=b==6;}
    if(!x_bound&&(pad.buttons>>5&1))held|=InEnter;if(!y_bound&&(pad.buttons>>6&1))held|=InRetry;
    return held;
}

double Game::logical_interval()const{
    return runtime_options.thprac&&session&&!session->replay&&menus&&menus->state.scene==MenuScene::Playing&&!practice_ui.captures()?th12_practice_interval(0,0,0,0):1./60.;
}
bool Game::frame(const PadSample& pad,u32 ticks,double now,float alpha){
    presentation_alpha=runtime_options.limit_presentation||!session||session->replay||phase!=Phase::Menu||menus->state.scene!=MenuScene::Playing?1:std::clamp(alpha,0.f,1.f);
    // Upstream createFrameRateMeter: displayed frames over wall time, once a second.
    frame_timing[0]=frame_timing[1]=frame_timing[2]=0;
    const double interval=last_frame>=0?now-last_frame:0;u32 game_ticks=0;
    if(fps_since<0||now<fps_since){fps_since=now;fps_frames=0;}else{++fps_frames;if(now-fps_since>=1000){fps_value=float(fps_frames*1000/(now-fps_since));fps_since=now;fps_frames=0;}}
    if(last_frame>=0)audio.update(std::clamp(now-last_frame,0.,250.)/1000);last_frame=now;
    audio.pump(); // keep the output queue filled (~0.1-0.14 s ahead)
    // Resource jobs run between frames so the loading screen keeps moving.
    if(!jobs.empty()){const double start=host::seconds();
        while(!jobs.empty()&&host::seconds()-start<.012){auto job=std::move(jobs.front());jobs.pop_front();
            if(!job()){host::log("load failed: %s",error.c_str());jobs.clear();after_jobs=nullptr;return false;}}
        if(jobs.empty()&&after_jobs){auto done=std::move(after_jobs);after_jobs=nullptr;done();if(!error.empty())return false;}
        audio.pump();}
    // Key Config capture (upstream padInput): wait for release, then take the next button.
    const u32 new_buttons=ticks?(pad.buttons&~previous_buttons):0;if(ticks)previous_buttons=pad.buttons;bool capturing=false;
    if(ticks&&menus&&phase==Phase::Menu){
        if(capture_release){if(!pad.buttons)capture_release=false;capturing=true;}
        else if(menus->state.capture_joy>=0){
            capturing=true;
            if(!menus->state.capture_ready){if(!pad.buttons)menus->state.capture_ready=true;}
            else if(new_buttons){for(i32 b=0;b<32;++b)if(new_buttons>>b&1){if(menus->capture_button(b))capture_release=true;break;}}
        }
    }
    const u32 ui=capturing?0:(map_buttons(pad)|pad.keyboard);
    if(!pending_replay.empty()&&menus&&phase==Phase::Menu){
        auto file=std::move(pending_replay);pending_replay.clear();menus->enter(MenuScene::Playing);play_replay(file,pending_replay_stage,false);
    }
    const int context=menus&&phase==Phase::Menu?(menus->state.scene==MenuScene::Practice?1:menus->state.scene==MenuScene::Playing?2:menus->state.scene==MenuScene::Pause||menus->state.scene==MenuScene::PauseConfirm?3:0):0;
    if(ticks){
        const int action=practice_ui.tick(context,menus&&menus->ready(),session&&bool(session->replay),menus?menus->run.difficulty:1,ui,practice_params);
        if(action==1&&context==1){custom_practice=true;MenuRun run=menus->run;run.mode=ModePractice;run.stage=practice_params[2]+1;if(run.stage==7)run.difficulty=4;menus->enter(MenuScene::Playing);begin(run);ticks=0;}
        else if(action==2&&context==1){menus->enter(MenuScene::Shot,menus->run.shot);ticks=0;}
        else if(action==3&&session){begin(session->run);ticks=0;}
        if(practice_ui.captures()){if(context==1)for(u32 n=0;n<ticks;++n){menus->tick(0);title->tick(menus->state,*menus);}ticks=0;}
    }
    if(session&&session->replay){
        if(external_paused)ticks=0;
        else if(external_rate!=1){rate_debt+=ticks*external_rate;ticks=u32(rate_debt);rate_debt-=ticks;}
        if(seek_target>=0&&phase==Phase::Menu&&menus->state.scene==MenuScene::Playing){
            const double deadline=host::seconds()+.008;
            for(int n=0;n<1200&&session&&int(session->cursor)<seek_target&&phase==Phase::Menu&&host::seconds()<deadline;++n){game_tick(0,now);handle_flow();captured=false;}
            if(session&&int(session->cursor)>=seek_target){seek_target=-1;external_paused=true;}
            ticks=0;
        }
    }
    // Replay playback (native 4x / quarter speed): skip or shot fast-forwards,
    // focus slows down.
    if(session&&session->run.mode==ModeReplay&&menus&&menus->state.scene==MenuScene::Playing&&phase==Phase::Menu){
        if(ui&(InSkip|InShot))ticks=std::min(16u,ticks*4);
        else if(ui&InFocus){const u32 total=slow_frames+ticks;ticks=total/4;slow_frames=total%4;}
        else slow_frames=0;
    }
    for(u32 n=0;n<ticks&&phase!=Phase::Loading&&!quit;++n){
        if(phase==Phase::Startup){tick_loading();continue;}
        tick_loading(); // the indicator outlives its load while it retires
        const u32 pressed=n==0?ui&~previous_ui:0;if(n==0)previous_ui=ui;
        auto& state=menus->state;
        if(ending){
            ending->tick(ui,pressed);
            if(ending->finished()){
                if(!ending->error.empty())host::log("ending: %s",ending->error.c_str());
                profile.record_ending_seen(ending_id);profile.save();ending.reset();
                for(i32 b=BankEnding;b<BankEnding+BankEndingCount;++b)if(auto& slot=banks.slots[size_t(b)]){Renderer::release(*slot);slot.reset();}
                menus->finish(*pending_result);clear_keys();title->tick(menus->state,*menus);actions();break;
            }
            continue;
        }
        if(state.scene==MenuScene::Playing&&(pressed&(InPause|InRetry))){
            menus->open_pause(result_data(false));clear_keys();
            if(pressed&InRetry){menus->clear_keys(previous_ui&~u32(InRetry));menus->tick(InRetry);}
            title->tick(menus->state,*menus);actions();break;
        }
        menus->tick(ui);title->tick(menus->state,*menus);
        // Upstream: a queued start/continue/retry/resume/return replaces or
        // reinitialises the session, so the old one is not ticked this tick;
        // any action ends the frame's ticks.
        const bool acted=menus->has_actions();bool blocked=false;
        if(acted){blocked=menus->blocks_session();actions();if(phase==Phase::Loading||quit)break;}
        if(!blocked&&(menus->state.scene==MenuScene::Playing||menus->state.scene==MenuScene::Demo)&&session&&phase==Phase::Menu){
            {const double t0=host::seconds();game_tick(ui&0x2ff&~u32(InPause),now);const double t=host::seconds()-t0;timing[0]+=t;frame_timing[0]+=t*1000;++game_ticks;}
            handle_flow();captured=false;if(menus->has_actions()){actions();break;}
            if(phase==Phase::Loading)break;
        }
        if(acted)break;
    }
    if(phase==Phase::Startup){
        startup_frames+=i32(ticks);
        // 43f320 waits for at least 180 start-up frames.
        if(startup_ready&&startup_frames>=180){
            signature.reset();Renderer::release(*banks.slots[BankSig]);banks.slots[BankSig].reset();retire_indicator();
            menus=std::make_unique<TitleScenes>(profile,music_tracks,host::paths().save+"/replay");
            std::vector<std::u32string> warning;for(const auto& line:exe.music_warning)warning.push_back(exe.warning_utf8?decode_utf8(line):decode_cp932(line));
            title=std::make_unique<TitlePresentation>(*scene,banks,profile,std::move(warning));
            phase=Phase::Menu;actions();
        }
    }
    if(quit)return false;
    watch_prefetch();
    // One capture per displayed frame; the sampler simulates any skipped ages.
    const bool in_game=session&&phase==Phase::Menu;
    const bool hide_game=in_game&&menus->state.scene==MenuScene::Playing&&th12::practice::toolSettings().disable_master_display;
    if(in_game&&!captured&&!hide_game){const double t0=host::seconds();
        scene->capture({fps_value,session->hud_age,session->replay?0:th12::fullGameFlow.continueCount,session->run.mode==ModePractice||(session->replay&&session->replay->info.practice),
            th12::fullGameFlow.status==th12::FullGameStatus::StageClear,session->high_score});
        captured=true;const double t=host::seconds()-t0;timing[1]+=t;frame_timing[1]=t*1000;}
    if(session&&!session->replay&&phase!=Phase::Loading)th12_set_wall_time(now);
    {const double t0=host::seconds();if(hide_game)gpu.begin({0,0,0,1});else draw();practice_ui.render(gpu);const double t=host::seconds()-t0;timing[2]+=t;frame_timing[2]=t*1000;}
    // Once a minute of play: average CPU time per frame and frame pacing for the log.
    const bool continued=timed_frame;timed_frame=in_game&&menus->state.scene==MenuScene::Playing;
    if(timed_frame){
        const double cpu=frame_timing[0]+frame_timing[1]+frame_timing[2];
        if(continued&&interval>1000/60.0*1.5){++late_frames;if(last_cpu>14)++late_after_work;}
        if(game_ticks>1)catch_up+=game_ticks-1;
        if(cpu>worst[0]){worst[0]=cpu;worst[1]=frame_timing[0];worst[2]=frame_timing[1];worst[3]=frame_timing[2];}
        last_cpu=cpu;
    }
    if(timed_frame&&++timed_frames==3600){
        host::log("timing (ms/frame): ticks %.2f, capture %.2f, draw %.2f, present %.2f, fps %.1f, stage %d",timing[0]*1000/timed_frames,timing[1]*1000/timed_frames,timing[2]*1000/timed_frames,present_ms/timed_frames,fps_value,th12::stageNumber);
        host::log("  late frames %u (%u after over 14 ms of work), catch-up ticks %u; slowest frame %.1f ms (ticks %.1f, capture %.1f, draw %.1f), slowest present %.1f ms",
            late_frames,late_after_work,catch_up,worst[0],worst[1],worst[2],worst[3],worst_present);
        timing[0]=timing[1]=timing[2]=0;timed_frames=0;present_ms=worst_present=0;for(double& w:worst)w=0;late_frames=late_after_work=catch_up=0;}
    return true;
}
void Game::presented(double ms){if(timed_frame){present_ms+=ms;worst_present=std::max(worst_present,ms);}}
const char* Game::phase_name()const{
    if(phase==Phase::Startup)return "startup";if(phase==Phase::Loading)return "loading";
    switch(menus->state.scene){
    case MenuScene::Title:return "title";case MenuScene::Difficulty:return "difficulty";case MenuScene::Character:return "character";case MenuScene::Shot:return "shot";
    case MenuScene::Practice:return "practice";case MenuScene::Options:return "options";case MenuScene::KeyConfig:return "key-config";case MenuScene::MusicRoom:return "music-room";
    case MenuScene::Replay:return "replay";case MenuScene::ReplayStage:return "replay-stage";case MenuScene::PlayerData:return "player-data";
    case MenuScene::Quit:return "quit";case MenuScene::Closed:return "closed";case MenuScene::Playing:return session&&session->replay?"replay-playing":"playing";
    case MenuScene::Demo:return "demo";case MenuScene::Pause:return "paused";case MenuScene::Name:return "name";case MenuScene::ReplaySave:return "replay-save";
    case MenuScene::PauseConfirm:return "pause-confirm";case MenuScene::Continue:return "continue";case MenuScene::ResultMenu:return "result-menu";case MenuScene::Ending:return "ending";}
    return "?";
}
void Game::focus_lost(){
    // Upstream visibilitychange: a running game opens its pause menu.
    if(phase==Phase::Menu&&menus&&menus->state.scene==MenuScene::Playing&&session){menus->open_pause(result_data(false));title->tick(menus->state,*menus);actions();}
}
void Game::draw_loading(){
    std::vector<Appearance> poses;
    if(held_frame){scene->render();const float field[4]{32,16,416,464};scene->render_menu({},0,0,true,1,field);}
    else{if(signature)signature->poses(poses);scene->render_menu(poses,0,0,false);}
}
void Game::draw(){
    if(phase==Phase::Startup||phase==Phase::Loading)draw_loading();
    else{
        const auto& state=menus->state;
        if(ending){
            std::vector<Appearance> poses;ending->poses(poses);scene->render_menu(poses,0,0,false,0,nullptr,{0,0,0,1},nullptr,fps_value);
            if(const float shade=ending->shade();shade>0)scene->render_menu({},0,0,true,shade);
        }else if((state.scene==MenuScene::Playing||state.scene==MenuScene::Demo)&&session){
            scene->render(presentation_alpha);
            // 422af3: the demo's last second fades to black.
            if(state.scene==MenuScene::Demo&&state.age>=3540)scene->render_menu({},0,0,true,std::min(1.f,float(state.age-3540)/60.f));
        }else{
            if(session&&title->popup(state))scene->render();
            title->render(state,*menus,fps_value);
        }
    }
    // The loading indicator sits over everything (renderOverlay).
    std::vector<Appearance> poses;if(indicator)indicator->poses(poses);for(const auto& vm:retiring)vm->poses(poses);
    if(!poses.empty())scene->render_menu(poses,480,392,true);
}

bool Game::snapshot(){
    const u8* p=gpu.read_pixels();if(!p)return false;const u32 s=gpu.scale(),w=640,h=480,sw=w*s;
    const auto dir=host::paths().save_file("snapshot");host::make_directory(dir);std::string path;
    for(u32 n=0;n<1000;++n){char file[32];std::snprintf(file,sizeof file,"/th12_%03u.bmp",n);path=dir+file;if(!host::exists(path))break;}
    const u32 row=w*3,size=54+row*h;std::vector<u8> bmp(size,0);
    auto put=[&](u32 at,u32 v,u32 n){for(u32 i=0;i<n;++i)bmp[at+i]=u8(v>>(8*i));};
    bmp[0]='B';bmp[1]='M';put(2,size,4);put(10,54,4);put(14,40,4);put(18,w,4);put(22,h,4);put(26,1,2);put(28,24,2);put(34,size-54,4);
    // Box-filter the scaled frame back to the original 640x480 (bottom-up BGR).
    for(u32 y=0;y<h;++y)for(u32 x=0;x<w;++x){u32 sum[3]{};for(u32 j=0;j<s;++j)for(u32 i=0;i<s;++i){const u8* q=p+(size_t(y*s+j)*sw+x*s+i)*4;sum[0]+=q[0];sum[1]+=q[1];sum[2]+=q[2];}
        u8* d=bmp.data()+54+(h-1-y)*row+x*3;d[0]=u8(sum[2]/(s*s));d[1]=u8(sum[1]/(s*s));d[2]=u8(sum[0]/(s*s));}
    const bool ok=host::write_file(path,bmp);host::log("snapshot %s: %s",path.c_str(),ok?"ok":"failed");return ok;
}
}
