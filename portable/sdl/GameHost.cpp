#include "Game.hpp"
#include "RuntimeOptions.hpp"
#include "BinaryReader.hpp"
#include "game/GameState.hpp"
#include "wasm/Api.hpp"
#include <sstream>
namespace th12host {
std::vector<MusicTrackInfo> prepared_music_comments(){
    std::vector<u8> bytes;std::vector<MusicTrackInfo> tracks;
    if(!host::read_file(host::paths().data_file("native-music.bin"),bytes,4*1024*1024))return tracks;
    try{BinaryReader r(bytes);r.magic("TH12MUS1",8);const u32 count=r.word();if(count>256)throw std::runtime_error("Invalid music comment count");
        for(u32 i=0;i<count;++i){MusicTrackInfo track;track.id=r.text();track.label=decode_utf8(r.text());track.title=decode_utf8(r.text());const u32 lines=r.word();if(lines>256)throw std::runtime_error("Invalid music comment lines");for(u32 j=0;j<lines;++j)track.lines.push_back(decode_utf8(r.text()));tracks.push_back(std::move(track));}
    }catch(const std::exception& e){host::log("music comments: %s",e.what());tracks.clear();}
    return tracks;
}
const std::string& Game::host_state(){
    const auto* h=th12_hud();const bool playing=session&&menus&&menus->state.scene==MenuScene::Playing&&phase==Phase::Menu;
    const char* kind=!playing?"menu":session->replay?"replay":th12::messageVM.active?"dialogue":"game";
    std::ostringstream out;
    out<<"{\"scene\":\""<<phase_name()<<"\",\"kind\":\""<<kind<<"\",\"ready\":"<<(playing&&th12_player_state()==1?"true":"false")
       <<",\"stage\":"<<(session?session->stage:0)<<",\"character\":"<<(session?session->run.character:0)<<",\"shot\":"<<(session?session->run.shot:0)
       <<",\"cursor\":"<<(session?session->cursor:0)<<",\"instance\":\""<<(session?session->stage:0)<<':'<<th12_player_instance()<<"\""
       <<",\"x\":"<<int(std::lround(h[1]*128))<<",\"y\":"<<int(std::lround(h[2]*128))
       <<",\"fast\":"<<th12_player_speed(0)<<",\"slow\":"<<th12_player_speed(1)<<",\"paused\":"<<((external_paused||(menus&&(menus->state.scene==MenuScene::Pause||menus->state.scene==MenuScene::PauseConfirm)))?"true":"false")
       <<",\"capturing\":"<<(practice_ui.captures()?"true":"false")<<",\"track\":\""<<audio.music_name()<<"\",\"seeking\":"<<(seek_target>=0?"true":"false")<<"}";
    host_snapshot=out.str();return host_snapshot;
}
bool Game::playback(int action,const char* path,int value){
    if(action==0||action==5){
        if(!path||!*path||std::string(path).find("..")!=std::string::npos)return false;
        replay_file=path;pending_replay=replay_file;pending_replay_stage=std::clamp(value,1,7);if(action==0){external_paused=false;external_rate=1;seek_target=-1;}else{seek_resume=!external_paused;audio.pause_music(true);audio.stop_effects();}return true;
    }
    if(action==6&&!pending_replay.empty()&&value>=0){seek_target=value;return true;}
    if(!session||!session->replay)return false;
    if(action==1){external_paused=true;seek_resume=false;audio.pause_music(true);audio.pause_effects();return true;}
    if(action==2){external_paused=false;seek_resume=true;if(seek_target<0){audio.pause_music(false);audio.resume_effects();}return true;}
    if(action==3&&value>=0){pending_replay=replay_file;pending_replay_stage=session->stage;seek_resume=!external_paused;audio.pause_music(true);audio.stop_effects();seek_target=std::min(value,int(session->replay->stage(session->stage)->frames)-1);return true;}
    if(action==4&&value>=0&&value<=8000){external_rate=value?value/1000.:64;return true;}
    return false;
}
void Game::practice_key(int code,bool down){practice_ui.key(code,down);}
bool Game::practice_pointer(int type,int id,float x,float y){return practice_ui.pointer(type,id,x,y);}
void Game::practice_wheel(float x,float y,float horizontal,float vertical){practice_ui.wheel(x,y,horizontal,vertical);}
}
