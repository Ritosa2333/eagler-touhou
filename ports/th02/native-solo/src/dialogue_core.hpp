#pragma once
#include <array>
#include <vector>
#include <span>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
namespace soew {
// Host-only hold gate. Replays store the resulting fast-forward flag, not wall time.
struct DialogueHold {
    static constexpr std::uint64_t threshold_ms=300;
    std::optional<std::uint64_t> since;
    void clear(){since.reset();}
    bool poll(std::uint64_t now,bool eligible){
        if(!eligible){clear();return false;}
        if(!since||now<*since)since=now;
        return now-*since>=threshold_ms;
    }
};
// Original STAGEn.TXT:80-byte boxes, two36-byte CP932 lines+4padding.
// No content rewriting or newline inference. UI must decode each visible byte
// span via CP932 and handle source key_delay/box-slide separately.
struct StageDialogue {
    struct Line {std::array<std::byte,36> text;std::array<std::byte,4> padding;};
    struct Box {std::array<Line,2> lines;};
    std::vector<Box> boxes;
    bool trailing_space=false;
    unsigned stage_id=0,pre_count=16,total_count=24;
    void configure(unsigned id){
        constexpr std::array<unsigned,6> pre={16,24,8,35,44,25},total={24,35,11,52,57,31};
        if(id>=pre.size())throw std::runtime_error("Unsupported generic dialogue stage");
        if(boxes.size()<total[id]||(id>=2&&boxes.size()!=total[id]))throw std::runtime_error("Unexpected dialogue box count");
        stage_id=id;pre_count=pre[id];total_count=total[id];
    }
    unsigned face()const{
        switch(stage_id){case 0:return stage1_faces.at(current);case 1:return stage2_faces.at(current);case 2:return stage3_faces.at(current);case 3:return stage4_faces.at(current);case 4:return stage5_faces.at(current);default:return extra_faces.at(current);}
    }
    static constexpr unsigned stage4_intro_end=8,stage4_generic_end=24,stage4_common_post_end=39,stage4_no_continue_start=47;
    static constexpr unsigned extra_intro_end=3,extra_appearance_peak=18,extra_appearance_end=36;
    static constexpr unsigned face_col_0=255;
    // Presentation only: never apply the short Meira pulse to the 190-tick reveal.
    static constexpr unsigned appearance_white_mix(unsigned stage,unsigned box,unsigned frame,bool reduced){
        if(stage==1){
            const unsigned f=frame<32?frame:32;
            return (f<=16?f:32-f)*(reduced?64u:255u)/16;
        }
        if(stage==3&&box==stage4_generic_end&&frame<10){
            const unsigned envelope=frame<=5?frame:10-frame;
            return envelope*(reduced?48u:192u)/5;
        }
        // Sigma: one initial sync per fade, then 17 six-tone steps.
        if(stage==5&&box==extra_intro_end&&frame<extra_appearance_end){
            const unsigned tone=frame<2?0:frame<extra_appearance_peak?(frame-1)*6:
                frame<=extra_appearance_peak+1?100:100-(frame-extra_appearance_peak-1)*6;
            return tone*(reduced?64u:255u)/100;
        }
        return 0;
    }
    static StageDialogue decode(std::span<const std::byte> bytes){
        StageDialogue out;
        // Preserve original assets; accept only the known single-space trailers.
        if((bytes.size()==1921||bytes.size()==2801||bytes.size()==881||bytes.size()==4161||bytes.size()==4561)&&bytes.back()==std::byte{0x20}){out.trailing_space=true;bytes=bytes.first(bytes.size()-1);}
        if(bytes.empty()||bytes.size()%80||bytes.size()>64*80)throw std::runtime_error("Invalid stage dialogue record size");
        out.boxes.resize(bytes.size()/80);
        for(unsigned i=0;i<bytes.size();++i){const unsigned b=i/80,line=(i%80)/40,col=i%40;if(col<36)out.boxes[b].lines[line].text[col]=bytes[i];else out.boxes[b].lines[line].padding[col-36]=bytes[i];}return out;
    }
    static constexpr std::array<unsigned,24> stage1_faces={0,96,51,96,9,96,0,96,99,99,48,12,99,99,6,99,99,99,57,96,3,99,9,96};
    static constexpr std::array<unsigned,35> stage2_faces={0,96,102,102,57,60,102,102,60,9,150,54,102,102,96,54,102,60,9,102,9,150,57,150,9,54,57,9,96,150,57,96,54,96,12};
    // ReC98 th02_main.asm:26535-26542, 26616-26631 (8 pre / 3 post).
    static constexpr std::array<unsigned,11> stage3_faces={6,96,96,12,96,51,96,0,12,96,12};
    // th02_main.asm:26633-26657, 26699-26757; the extra face at
    // 26722 is not consumed by the 11-box loop in dialog.cpp:435-442.
    static constexpr std::array<unsigned,52> stage4_faces={
        57,96,96,57,96,57,96,57,
        105,6,96,12,6,6,105,105,12,96,6,6,105,105,105,105,
        57,144,6,144,144,144,6,144,96,0,96,
        147,0,96,0,
        96,54,144,144,12,96,57,144,
        96,96,57,96,57
    };
    // th02_main.asm:26759–26817 and th02/main/dialog/dialog.cpp:487–587.
    static constexpr std::array<unsigned,57> stage5_faces={
        6,105,6,105,
        105,57,105,6,6,105,105,6,105,6,6,96,6,105,57,105,0,96,6,6,105,105,
        105,0,105,6,105,105,105,105,105,6,105,105,105,6,6,105,105,
        105,
        105,105,6,105,57,
        108,6,108,12,
        105,105,105,105
    };
    static constexpr unsigned stage5_intro_end=2,stage5_winged=43,stage5_pre_final_start=49,stage5_final_start=53;
    // dialog.cpp:589–594 (3 intro) and th02_main.asm:26665–26695 (22 pre / 6 post).
    static constexpr std::array<unsigned,31> extra_faces={
        face_col_0,0,face_col_0,
        153,0,153,3,153,6,153,153,153,57,153,57,153,57,153,153,57,153,12,96,12,153,
        9,156,9,96,96,12
    };
    struct Reveal {
        unsigned cursor=6,loops=0;bool waiting=false;
        // Original tests <=80, then increments by2; at82 enters key_delay.
        void tick(bool any_key){if(waiting)return;++loops;if(loops%(any_key?1:3)==0)cursor+=2;if(cursor>80)waiting=true;}
        unsigned bytes(unsigned line)const{return line==0?(cursor<=36?cursor:36):(cursor>36?(cursor<=72?cursor-36:36):0);}
    };
    // Two samples separated by two logical host updates, release then press.
    // Caller supplies original groups5/3/0 translated to host keys, not any key.
    struct KeyDelay {
        bool waiting_press=false,sample=false;unsigned elapsed=0;
        void reset(bool group_key){waiting_press=false;sample=group_key;elapsed=0;}
        bool tick(bool group_key){if(++elapsed<2)return false;const bool detected=sample||group_key;elapsed=0;sample=group_key;if(!waiting_press){if(!detected)waiting_press=true;}else if(detected)return true;return false;}
    };
    enum class Phase {Dormant,SlideIn,Text,KeyDelay,IntroAppearance,SlideOut,Complete};
    Phase phase=Phase::Dormant;
    int box_left=400;
    KeyDelay key_delay;
    unsigned updates=0,end_box=16;
    bool postboss=false;
    unsigned continues_used=0;
    void continue_after_appearance(){if(phase!=Phase::IntroAppearance)throw std::runtime_error("Invalid appearance continuation");phase=Phase::Text;}
    bool active()const{return phase!=Phase::Dormant&&phase!=Phase::Complete;}
    void start_preboss(){if(boxes.size()<total_count)throw std::runtime_error("Stage dialogue records missing");current=0;end_box=pre_count;postboss=false;reveal=Reveal{};phase=Phase::SlideIn;box_left=400;updates=0;}
    void start_postboss(){
        if(boxes.size()<total_count||current!=pre_count)throw std::runtime_error("Invalid postboss dialogue cursor");
        end_box=total_count;
        if(stage_id==3&&continues_used){
            // dialog.cpp:444–484; modify the current run's copy, not the asset.
            end_box=stage4_no_continue_start;
            auto &line=boxes.at(42).lines[0].text;
            const auto digit=[&](unsigned pos,unsigned value){line[pos]=std::byte{0x82};line[pos+1]=std::byte(0x4f+value);};
            if(const auto tens=(continues_used/10)%10)digit(8,tens);
            digit(10,continues_used%10);
        }
        postboss=true;reveal=Reveal{};phase=Phase::SlideIn;box_left=400;updates=0;
    }
    void start_stage5_defeat(unsigned used,bool final_form){
        if(stage_id!=4||active()||current!=(final_form?stage5_final_start:44u))throw std::runtime_error("Invalid Mima defeat dialogue cursor");
        continues_used=used;postboss=true;
        if(final_form){current=stage5_final_start;end_box=57;box_left=32;phase=Phase::Text;}
        else{current=used?44:stage5_pre_final_start;end_box=used?stage5_pre_final_start:stage5_final_start;box_left=400;phase=Phase::SlideIn;}
        reveal=Reveal{};updates=0;
    }
    bool mima_pre_final_dialogue()const{
        return stage_id==4&&postboss&&active()&&end_box==stage5_final_start&&
            current>=stage5_pre_final_start&&current<stage5_final_start;
    }
    bool show_portrait()const{return stage_id!=4||!postboss||current<stage5_final_start;}
    // Reveal acceleration follows gameplay key_det; key_delay uses different
    // raw keyboard groups. A dedicated fast-forward key also advances the gate.
    bool tick(bool accelerate,bool group_key,bool fast_forward=false){
        if(!active())return false;
        ++updates;
        switch(phase){
        case Phase::SlideIn:box_left-=16;if(box_left<=32){box_left=32;phase=Phase::Text;}break;
        case Phase::Text:
            if(accelerate||fast_forward){while(!reveal.waiting)reveal.tick(true);}else reveal.tick(false);
            if(reveal.waiting){key_delay.reset(group_key);phase=Phase::KeyDelay;}
            if(!fast_forward)break;
            [[fallthrough]];
        case Phase::KeyDelay:
            if(!fast_forward&&!key_delay.tick(group_key))break;
            advance_after_key_delay();
            if(!postboss&&((stage_id==1&&current==2)||(stage_id==3&&(current==stage4_intro_end||current==stage4_generic_end))||
                (stage_id==4&&(current==stage5_intro_end||current==stage5_winged))||(stage_id==5&&current==extra_intro_end))){
                phase=Phase::IntroAppearance;return false;
            }
            if(current==end_box){if(postboss){phase=Phase::Complete;return true;}phase=Phase::SlideOut;}
            else phase=Phase::Text;
            break;
        case Phase::SlideOut:box_left-=16;if(box_left+384<=32){phase=Phase::Complete;return true;}break;
        default:break;
        }return false;
    }
    unsigned current=0;
    Reveal reveal;
    bool preboss_complete()const{return current==pre_count;}
    bool postboss_complete()const{return current==(postboss?end_box:total_count);}
    // Called ONLY after the separate key_delay state has completed.
    void advance_after_key_delay(){if(!reveal.waiting||current>=boxes.size())throw std::runtime_error("Invalid dialogue advance");++current;
        // Without Continue, the eight alternate records are skipped, not played.
        if(stage_id==3&&postboss&&!continues_used&&current==stage4_common_post_end)current=stage4_no_continue_start;
        if(stage_id==4&&!postboss){
            if(!continues_used&&current==4)current=26;
            else if(continues_used&&current==26)current=stage5_winged;
        }
        reveal=Reveal{};}
};

// Host-only physical Z gate. The simulation and its replays still receive
// ordinary reveal and key-delay inputs.
class MimaDialogueTap {
    const StageDialogue* dialogue_=nullptr;
    unsigned box_=0;
    bool held_=false,pending_=false;

    void bind(const StageDialogue* dialogue){
        using Phase=StageDialogue::Phase;
        if(!dialogue||!dialogue->mima_pre_final_dialogue()||
            (dialogue->phase!=Phase::Text&&dialogue->phase!=Phase::KeyDelay)){
            clear();return;
        }
        if(dialogue_!=dialogue||box_!=dialogue->current){
            dialogue_=dialogue;box_=dialogue->current;pending_=false;
        }
    }
public:
    struct Input {bool accelerate=false,group=false;};
    void clear(){dialogue_=nullptr;pending_=false;}
    void reset(bool z_held){clear();held_=z_held;}
    void event(const StageDialogue* dialogue,bool down,bool repeat=false){
        bind(dialogue);
        if(down&&!held_&&!repeat&&dialogue_)pending_=true;
        held_=down;
    }
    Input poll(const StageDialogue& dialogue){
        bind(&dialogue);
        if(!pending_)return {};
        if(dialogue.phase==StageDialogue::Phase::Text)return {true,false};
        // Let the original two-tick gate see release first. Pulse only on
        // the sample that consumes it, never on the next box's reveal.
        if(!dialogue.key_delay.waiting_press||dialogue.key_delay.elapsed!=1)return {};
        pending_=false;
        return {false,true};
    }
};
}
