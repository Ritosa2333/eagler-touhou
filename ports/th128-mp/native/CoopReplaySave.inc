// Original save interaction, bound to a separate presentation-only Wasm instance.
// The multiplayer tape and its IDB commit remain browser-owned; neither the
// co-op World nor the ordinary replay recorder is touched by this UI.
namespace {
std::unique_ptr<th128::record::NativeReplaySaveMenu> coopReplaySave;
}
extern "C" int th128_coop_replay_save_open(unsigned gameOver){
    if(gameOver>1||coopReplaySave)return 0;
    coopReplaySave=std::make_unique<th128::record::NativeReplaySaveMenu>();
    coopReplaySave->open(gameOver!=0,{});return 1;
}
extern "C" int th128_coop_replay_save_tick(unsigned pressed){
    if(!coopReplaySave||coopReplaySave->finished()||(pressed&~0x1f3u))return 0;
    coopReplaySave->tick(pressed);return 1;
}
extern "C" int th128_coop_replay_save_ack(unsigned serial,unsigned slot){
    if(!coopReplaySave||!coopReplaySave->pending()||coopReplaySave->serial()!=serial||coopReplaySave->slot()!=slot)return 0;
    coopReplaySave->acceptSaved(serial,slot);return 1;
}
extern "C" const char* th128_coop_replay_save_state(unsigned soundCursor){
    static std::string result;
    if(!coopReplaySave||soundCursor>coopReplaySave->sounds().size())return nullptr;
    const auto& menu=*coopReplaySave;
    std::ostringstream text;
    text<<"{\"phase\":"<<menu.phase()<<",\"slot\":"<<menu.slot()<<",\"selected\":"<<menu.selected()
        <<",\"serial\":"<<menu.serial()<<",\"age\":"<<menu.nameAge()<<",\"length\":"<<menu.length()<<",\"sounds\":[";
    const auto& sounds=menu.sounds();
    for(unsigned i=soundCursor;i<sounds.size();++i){if(i!=soundCursor)text<<',';text<<sounds[i];}
    text<<"],\"name\":[";
    for(unsigned i=0;i<menu.name().size();++i){if(i)text<<',';text<<unsigned(menu.name()[i]);}
    text<<"],\"glyphs\":[";
    if(menu.phase()==2){
        bool comma=false;
        for(const auto& glyph:th128::record::originalReplayNameGlyphs(menu)){
            if(comma)text<<',';comma=true;
            text<<'['<<glyph.sprite<<','<<glyph.x<<','<<glyph.y<<','<<glyph.color<<']';
        }
    }
    text<<"]}";result=text.str();return result.c_str();
}
