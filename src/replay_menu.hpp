#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include "locale.hpp"

namespace soew {
inline const wchar_t* replay_menu_error(std::string_view message, Language language) {
    if(message=="INVALID REPLAY") return localized(language,L"無効なリプレイです",L"Invalid replay",L"录像无效",L"錄影無效");
    if(message=="REPLAY SLOT PROTECTED") return localized(language,L"このスロットは上書きできません",L"Replay slot is protected",L"此槽位不可覆盖",L"此欄位不可覆寫");
    if(message=="REPLAY SAVE FAILED") return localized(language,L"リプレイを保存できませんでした",L"Could not save replay",L"无法保存录像",L"無法儲存錄影");
    if(message=="SEEK MISMATCH") return localized(language,L"指定した面までの内容が一致しません",L"Replay did not match before the selected stage",L"录像在指定关卡前已不一致",L"錄影在指定關卡前已不一致");
    if(message=="SEEK BUDGET") return localized(language,L"指定した面への準備に時間がかかりすぎました",L"Replay stage preparation exceeded its budget",L"准备从指定关卡播放时超时",L"準備從指定關卡播放時逾時");
    return localized(language,L"リプレイを読み込めませんでした",L"Could not load replay",L"无法载入录像",L"無法載入錄影");
}
inline constexpr std::size_t replay_menu_slot_count=100;
inline constexpr std::size_t replay_menu_rows=10;
inline constexpr std::size_t replay_menu_slot_pages=replay_menu_slot_count/replay_menu_rows;

enum class ReplaySlotState {Empty,Ready,Unavailable,ReadyProtected,UnavailableProtected};
struct ReplayMenuCatalog {
    std::array<ReplaySlotState,replay_menu_slot_count> slots{};
    std::size_t legacy_count=0; // Read-only loose v1 recordings, in adapter-defined stable order.
    std::span<const std::uint8_t> legacy_ready{}; // Empty when the adapter supplies only playable rows.
};

enum class ReplaySaveOrigin {Clear,GameOver,Pause,PracticeResult};
enum class ReplayMenuScreen {Closed,Browse,Details,SaveChoice,Name,SaveSlots,Overwrite,Writing,Starting};
enum class ReplayMenuAction {None,Back,Discard,Write,Saved,Play,Started,ToggleRecording,CancelPlay};
struct ReplayMenuEntry {
    bool legacy=false;
    std::size_t index=0; // Zero-based in its own collection.
    bool operator==(const ReplayMenuEntry&) const=default;
};
struct ReplayMenuCommand {
    ReplayMenuAction action=ReplayMenuAction::None;
    ReplayMenuEntry entry{};
    bool replace=false; // True only after an explicit Yes on an occupied numbered slot.
    int stage=-1; // -1 is the original start.
};

struct ReplayNameEditor {
    static constexpr std::size_t capacity=8,columns=17,rows=3;
    static constexpr std::size_t space_key=47,back_key=48,advance_key=49,end_key=50;

    // Preserve the reference grid coordinates; gaiji without ASCII equivalents
    // are skipped because native replay names store printable ASCII.
    static constexpr char character(std::size_t key){
        if(key<=25)return static_cast<char>('A'+key);
        if(key>=34&&key<=43)return static_cast<char>('0'+key-34);
        switch(key){
        case 26:return ',';
        case 27:return '.';
        case 28:return '!';
        case 29:return '?';
        case space_key:return ' ';
        default:return '\0';
        }
    }
    static constexpr bool selectable(std::size_t key){
        return character(key)!='\0'||key==back_key||key==advance_key||key==end_key;
    }
    static constexpr bool accepts(char c){
        return (c>='A'&&c<='Z')||(c>='0'&&c<='9')||c==' '||c=='.'||c==','||c=='!'||c=='?';
    }

    std::size_t selected_key()const{return key_;}
    std::size_t cursor()const{return cursor_;}
    std::string_view text()const{return {cells_.data(),length_};}
    const std::array<char,capacity>& cells()const{return cells_;}

private:
    friend class ReplayMenu;
    std::array<char,capacity> cells_{};
    std::size_t length_=0,cursor_=0,key_=0;

    void reset(std::string_view value){
        cells_.fill(' ');length_=0;cursor_=0;key_=0;
        for(char c:value){
            if(length_==capacity)break;
            if(c>='a'&&c<='z')c=static_cast<char>('A'+c-'a');
            if(accepts(c))cells_[length_++]=c;
        }
        cursor_=length_;
        if(cursor_==capacity)key_=end_key;
    }
    static std::size_t wrap(std::size_t n,int step,std::size_t count){
        if(step<0)return n==0?count-1:n-1;
        return n==count-1?0:n+1;
    }
    void move(int dx,int dy){
        auto x=key_%columns,y=key_/columns;
        do{
            if(dx)x=wrap(x,dx,columns);
            else y=wrap(y,dy,rows);
        }while(!selectable(y*columns+x));
        key_=y*columns+x;
    }
    void clear_cell(){
        if(cursor_>=capacity)return;
        cells_[cursor_]=' ';
        while(length_&&cells_[length_-1]==' ')--length_;
    }
    void erase(){
        clear_cell();
        if(cursor_)--cursor_;
    }
    bool select(){
        if(key_==end_key)return true;
        if(key_==back_key){
            if(cursor_)--cursor_;
            clear_cell();
        }else if(cursor_<capacity){
            if(key_!=advance_key){
                cells_[cursor_]=character(key_);
                if(length_<=cursor_)length_=cursor_+1;
            }
            if(++cursor_==capacity)key_=end_key;
        }
        return false;
    }
};

class ReplayMenu {
public:
    // Same input mask as the title Menu. A new press is accepted only after all keys are released.
    enum : unsigned {up=1,down=2,confirm=4,cancel=8,left=16,right=32,toggle_recording=64,erase=128};

    void begin_browse(){reset(ReplayMenuScreen::Browse);}
    void begin_save(ReplaySaveOrigin origin,std::string_view initial_name={}){
        origin_=origin;
        reset(origin==ReplaySaveOrigin::Clear?ReplayMenuScreen::SaveChoice:ReplayMenuScreen::Name);
        editor_.reset(initial_name);
        yes_=true; // Save rather than discard on the Clear prompt.
    }

    ReplayMenuScreen screen()const{return screen_;}
    ReplaySaveOrigin origin()const{return origin_;}
    bool active()const{return screen_!=ReplayMenuScreen::Closed;}
    std::size_t page()const{return page_;}
    std::size_t row()const{return row_;}
    bool choice_yes()const{return yes_;}
    int playback_stage()const{return playback_stage_;}
    std::uint8_t playback_stages()const{return playback_stages_;}
    void set_playback_stages(std::uint8_t stages){playback_stages_=stages&0x1f;playback_stage_=-1;}
    std::string_view error()const{return error_;}
    const ReplayNameEditor& name_editor()const{return editor_;}
    std::string_view name()const{
        const auto text=editor_.text();
        const auto last=text.find_last_not_of(' ');
        return last==std::string_view::npos?std::string_view("PLAYER"):text.substr(0,last+1);
    }
    std::size_t page_count(const ReplayMenuCatalog &catalog)const{
        const bool browse=screen_==ReplayMenuScreen::Browse||screen_==ReplayMenuScreen::Details||screen_==ReplayMenuScreen::Starting;
        return replay_menu_slot_pages+(browse?(catalog.legacy_count/replay_menu_rows+(catalog.legacy_count%replay_menu_rows!=0)):0);
    }
    ReplayMenuEntry selected_entry()const{
        if(page_<replay_menu_slot_pages)return {false,page_*replay_menu_rows+row_};
        return {true,(page_-replay_menu_slot_pages)*replay_menu_rows+row_};
    }

    ReplayMenuCommand update(unsigned input,const ReplayMenuCatalog &catalog){
        input&=up|down|confirm|cancel|left|right|toggle_recording|erase;
        if(!active())return {};
        if(!input){released_=true;return {};}
        if(!released_)return {};
        released_=false;
        const auto dx=int(bool(input&right))-int(bool(input&left));
        const auto dy=int(bool(input&down))-int(bool(input&up));
        if(input&cancel)return go_back();
        switch(screen_){
        case ReplayMenuScreen::Browse:
        case ReplayMenuScreen::SaveSlots: {
            const bool browse=screen_==ReplayMenuScreen::Browse;
            if(browse&&(input&toggle_recording))return {ReplayMenuAction::ToggleRecording};
            const auto pages=page_count(catalog);
            const auto last_page_rows=catalog.legacy_count%replay_menu_rows;
            const auto rows_at=[&](std::size_t page){
                return page>=replay_menu_slot_pages&&page==pages-1&&last_page_rows?last_page_rows:replay_menu_rows;
            };
            if(page_>=pages){page_=pages-1;row_=0;}
            if(dx)page_=ReplayNameEditor::wrap(page_,dx,pages);
            const auto rows=rows_at(page_);
            if(row_>=rows)row_=rows-1;
            if(dy<0){
                if(row_)--row_;
                else{page_=ReplayNameEditor::wrap(page_,-1,pages);row_=rows_at(page_)-1;}
            }else if(dy>0){
                if(row_+1<rows)++row_;
                else{page_=ReplayNameEditor::wrap(page_,1,pages);row_=0;}
            }
            if(dx||dy){error_={};return {};}
            if(!(input&confirm))return {};
            const auto entry=selected_entry();
            if(browse){
                const auto state=entry.legacy
                    ?(catalog.legacy_ready.empty()||(entry.index<catalog.legacy_ready.size()&&catalog.legacy_ready[entry.index])
                        ?ReplaySlotState::ReadyProtected:ReplaySlotState::UnavailableProtected)
                    :catalog.slots[entry.index];
                if(state==ReplaySlotState::Ready||state==ReplaySlotState::ReadyProtected){
                    pending_=entry;screen_=ReplayMenuScreen::Details;set_playback_stages(0);error_={};
                }else if(state!=ReplaySlotState::Empty){
                    error_="INVALID REPLAY";
                }
            }else{
                const auto state=catalog.slots[entry.index];
                if(state==ReplaySlotState::Empty){pending_=entry;return write(false);}
                if(state==ReplaySlotState::ReadyProtected||state==ReplaySlotState::UnavailableProtected){
                    error_="REPLAY SLOT PROTECTED";
                }else{
                    pending_=entry;yes_=false;error_={};screen_=ReplayMenuScreen::Overwrite;
                }
            }
            return {};
        }
        case ReplayMenuScreen::Details:
            if(playback_stages_&&(dx||dy)){
                const int direction=dx?dx:dy;
                do{playback_stage_=(playback_stage_+1+direction+6)%6-1;}
                while(playback_stage_>=0&&!(playback_stages_&(1u<<playback_stage_)));
                error_={};return {};
            }
            if(input&confirm){error_={};screen_=ReplayMenuScreen::Starting;return {ReplayMenuAction::Play,pending_,false,playback_stage_};}
            return {};
        case ReplayMenuScreen::SaveChoice:
            if(dx||dy){yes_=!yes_;return {};}
            if(input&confirm){
                if(!yes_){screen_=ReplayMenuScreen::Closed;return {ReplayMenuAction::Discard};}
                screen_=ReplayMenuScreen::Name;
            }
            return {};
        case ReplayMenuScreen::Name:
            if(dx||dy)editor_.move(dx,dy);
            else if(input&erase)editor_.erase();
            else if((input&confirm)&&editor_.select())screen_=ReplayMenuScreen::SaveSlots;
            return {};
        case ReplayMenuScreen::Overwrite:
            if(dx||dy){yes_=!yes_;return {};}
            if(input&confirm){
                if(yes_)return write(true);
                screen_=ReplayMenuScreen::SaveSlots;
            }
            return {};
        default:return {};
        }
    }

    ReplayMenuCommand complete_write(bool ok){
        if(screen_!=ReplayMenuScreen::Writing)return {};
        if(ok){screen_=ReplayMenuScreen::Closed;return {ReplayMenuAction::Saved,pending_};}
        error_="REPLAY SAVE FAILED";screen_=ReplayMenuScreen::SaveSlots;return {};
    }
    ReplayMenuCommand complete_play(bool ok,std::string_view error="REPLAY LOAD FAILED"){
        if(screen_!=ReplayMenuScreen::Starting)return {};
        if(ok){screen_=ReplayMenuScreen::Closed;return {ReplayMenuAction::Started,pending_};}
        error_=error;screen_=ReplayMenuScreen::Details;return {};
    }

private:
    ReplayMenuScreen screen_=ReplayMenuScreen::Closed;
    ReplaySaveOrigin origin_=ReplaySaveOrigin::Clear;
    bool yes_=false,released_=false;
    std::size_t page_=0,row_=0;
    int playback_stage_=-1;
    std::uint8_t playback_stages_=0;
    std::string_view error_{};
    ReplayNameEditor editor_{};
    ReplayMenuEntry pending_{};

    void reset(ReplayMenuScreen screen){
        screen_=screen;page_=0;row_=0;error_={};pending_={};yes_=false;released_=false;
    }
    ReplayMenuCommand write(bool replace){
        error_={};screen_=ReplayMenuScreen::Writing;return {ReplayMenuAction::Write,pending_,replace};
    }
    ReplayMenuCommand go_back(){
        error_={};
        switch(screen_){
        case ReplayMenuScreen::Browse:
        case ReplayMenuScreen::SaveChoice:
            screen_=ReplayMenuScreen::Closed;return {ReplayMenuAction::Back};
        case ReplayMenuScreen::Details:screen_=ReplayMenuScreen::Browse;break;
        case ReplayMenuScreen::Starting:
            screen_=ReplayMenuScreen::Details;return {ReplayMenuAction::CancelPlay};
        case ReplayMenuScreen::Name:
            if(origin_==ReplaySaveOrigin::Clear){screen_=ReplayMenuScreen::SaveChoice;yes_=true;}
            else{screen_=ReplayMenuScreen::Closed;return {ReplayMenuAction::Back};}
            break;
        case ReplayMenuScreen::SaveSlots:screen_=ReplayMenuScreen::Name;break;
        case ReplayMenuScreen::Overwrite:screen_=ReplayMenuScreen::SaveSlots;break;
        default:break;
        }
        return {};
    }
};
}
