#pragma once
#include "settings.hpp"
#include "pickup_core.hpp"
#include <algorithm>
#include <cctype>
#include <limits>
#include <vector>

namespace soew {
class NumericEditor {
public:
    std::string buffer;
    void begin(std::int64_t value,std::int64_t minimum,std::int64_t maximum){
        minimum_=minimum;maximum_=maximum;buffer=std::to_string(value);cursor_=buffer.size();selected_=true;active_=true;
    }
    bool active() const {return active_;}
    bool digit(unsigned value){return value<=9&&insert(std::string(1,static_cast<char>('0'+value)));}
    bool minus(){return minimum_<0&&insert("-");}
    void select_all(){if(active_)selected_=true;}
    void move_left(){if(active_){if(selected_)cursor_=0;else if(cursor_)--cursor_;selected_=false;}}
    void move_right(){if(active_){if(selected_)cursor_=buffer.size();else if(cursor_<buffer.size())++cursor_;selected_=false;}}
    void home(){if(active_){cursor_=0;selected_=false;}}
    void end(){if(active_){cursor_=buffer.size();selected_=false;}}
    void backspace(){if(!active_)return;if(selected_){buffer.clear();cursor_=0;selected_=false;}else if(cursor_){buffer.erase(--cursor_,1);}}
    void delete_forward(){if(!active_)return;if(selected_){buffer.clear();cursor_=0;selected_=false;}else if(cursor_<buffer.size())buffer.erase(cursor_,1);}
    bool paste(std::string_view text){
        if(!active_||text.size()>256)return false;
        while(!text.empty()&&std::isspace(static_cast<unsigned char>(text.front())))text.remove_prefix(1);
        while(!text.empty()&&std::isspace(static_cast<unsigned char>(text.back())))text.remove_suffix(1);
        std::int64_t value=0;
        if(!parse(text,value)||value<minimum_||value>maximum_)return false;
        buffer=std::to_string(value);cursor_=buffer.size();selected_=false;return true;
    }
    bool paste(std::wstring_view text){
        if(!active_||text.size()>256)return false;
        const auto space=[](wchar_t c){return c==L'\u3000'||(static_cast<std::uint32_t>(c)<=0x7f&&std::isspace(static_cast<unsigned char>(c)));};
        while(!text.empty()&&space(text.front()))text.remove_prefix(1);
        while(!text.empty()&&space(text.back()))text.remove_suffix(1);
        std::string normalized;normalized.reserve(text.size());
        for(const auto c:text){
            if(c>=L'\uff10'&&c<=L'\uff19')normalized.push_back(static_cast<char>('0'+c-L'\uff10'));
            else if(c==L'\uff0d')normalized.push_back('-');
            else if(static_cast<std::uint32_t>(c)<=0x7f)normalized.push_back(static_cast<char>(c));
            else return false;
        }
        return paste(std::string_view(normalized));
    }
    bool accept(std::int64_t &value){
        std::int64_t parsed=0;
        if(!active_||!parse(buffer,parsed)||parsed<minimum_||parsed>maximum_)return false;
        value=parsed;active_=false;selected_=false;return true;
    }
    void cancel(){active_=false;selected_=false;}
private:
    std::int64_t minimum_=0,maximum_=0;
    std::size_t cursor_=0;
    bool active_=false,selected_=false;
    static bool parse(std::string_view text,std::int64_t &value){
        if(text.empty()||text.size()>20)return false;
        const auto parsed=std::from_chars(text.data(),text.data()+text.size(),value);
        return parsed.ec==std::errc{}&&parsed.ptr==text.data()+text.size();
    }
    bool insert(std::string_view text){
        if(!active_)return false;
        auto next=selected_?std::string{}:buffer;
        const auto position=selected_?0:cursor_;
        next.insert(position,text);
        const auto low=std::to_string(minimum_),high=std::to_string(maximum_);
        const auto digits=[](const std::string &s){return s.size()-(s.front()=='-'?1u:0u);};
        const auto count=next.size()-(!next.empty()&&next.front()=='-'?1u:0u);
        if(count>std::max(digits(low),digits(high)))return false;
        std::int64_t value=0;
        if(next!="-"&&!parse(next,value))return false;
        if(next=="-"&&minimum_>=0)return false;
        buffer=std::move(next);cursor_=position+text.size();selected_=false;return true;
    }
};
using GeneralNumericEditor = NumericEditor;
// Section ID 4 was Road Only in saved version-4 recipes.
enum class PracticeSection { StageStart, Chapter, Midboss, Boss, Midboss2=5 };
inline constexpr int practice_stage_count=6;
inline constexpr int practice_extra_stage=5;
inline constexpr int practice_max_boss_phase=10;
enum class PracticeDialogue { Show, Skip };
enum class PracticeSeedMode { Fixed, Random };
struct PracticeSpec {
    using Section = PracticeSection;
    int stage=1;
    PracticeSection section=PracticeSection::Boss;
    int chapter_id=-1;
    int shot=1,rank=1,dynamic_rank=0,power=80,lives=2,bombs=3,respawn_bombs=3,boss_phase=0;
    PracticeDialogue dialogue=PracticeDialogue::Skip;
    std::uint32_t seed=1234; // Retained fixed seed, even when attempts use random seeds.
    PracticeSeedMode seed_mode=PracticeSeedMode::Fixed;
    bool operator==(const PracticeSpec&) const = default;
};
struct PracticeCapabilities {
    struct Chapter { int id; std::string name; };
    std::array<std::vector<PracticeSection>,practice_stage_count> allowed_sections;
    // Populate only from verified source metadata; IDs need not be contiguous.
    std::array<std::vector<Chapter>,practice_stage_count> chapters;
    std::array<std::vector<std::string>,practice_stage_count> boss_phases;
    bool allows(int stage,PracticeSection section) const {
        if(stage<0||stage>=practice_stage_count||static_cast<int>(section)==4)return false;
        const auto &sections=allowed_sections[stage];
        return std::find(sections.begin(),sections.end(),section)!=sections.end() &&
            (section!=PracticeSection::Chapter||!chapters[stage].empty());
    }
    bool has_chapter(int stage,int id) const {
        if(stage<0||stage>=practice_stage_count||id<0)return false;
        const auto &entries=chapters[stage];
        return std::any_of(entries.begin(),entries.end(),[&](const Chapter &x){return x.id==id;});
    }
    bool allows_phase(int stage,int phase) const {
        return stage>=0&&stage<practice_stage_count&&phase>=0&&phase<=static_cast<int>(boss_phases[stage].size());
    }
};
inline std::string validate_practice(const PracticeSpec &s) {
    if(s.stage<0||s.stage>=practice_stage_count)return "Stage unavailable";
    if(static_cast<int>(s.section)<0||s.section>PracticeSection::Midboss2||static_cast<int>(s.section)==4)return "Section invalid";
    if(s.section==PracticeSection::Chapter?s.chapter_id<0:s.chapter_id!=-1)return "Chapter ID invalid";
    if(s.boss_phase<0||s.boss_phase>practice_max_boss_phase||(s.section!=PracticeSection::Boss&&s.boss_phase!=0))return "Boss phase invalid";
    if(s.shot<0||s.shot>2)return "Shot invalid";
    if(s.stage==practice_extra_stage?s.rank!=4:(s.rank<0||s.rank>3))return "Difficulty invalid";
    if(s.dynamic_rank<-6||s.dynamic_rank>(s.rank==0?4:16))return "Dynamic rank invalid";
    if(s.power<1||s.power>80)return "Power invalid";
    if(s.lives<0||s.lives>5)return "Spare lives invalid";
    if(s.bombs<0||s.bombs>5)return "Bombs invalid";
    if(s.respawn_bombs<0||s.respawn_bombs>3)return "Respawn bombs invalid";
    if(s.dialogue!=PracticeDialogue::Show&&s.dialogue!=PracticeDialogue::Skip)return "Dialogue invalid";
    if(s.boss_phase>0&&s.dialogue!=PracticeDialogue::Skip)return "Phase entry skips dialogue";
    if(s.seed_mode!=PracticeSeedMode::Fixed&&s.seed_mode!=PracticeSeedMode::Random)return "Seed mode invalid";
    return {};
}
inline std::string validate_practice(const PracticeSpec &s,const PracticeCapabilities &caps) {
    if(auto error=validate_practice(s);!error.empty())return error;
    if(!caps.allows(s.stage,s.section))return "Target not implemented";
    if(s.section==PracticeSection::Chapter&&!caps.has_chapter(s.stage,s.chapter_id))return "Chapter unavailable";
    if(!caps.allows_phase(s.stage,s.boss_phase))return "Boss phase unavailable";
    return {};
}
inline int practice_power_step(int power,int delta) {
    const auto n=static_cast<long long>(power)+delta;
    return static_cast<int>(std::clamp(n,1LL,80LL));
}
struct PracticeMenu {
    enum class Action { None, Start, Back, EditNumber, EditSeed=EditNumber, NewSeed };
    enum class Row { Stage, Section, Chapter, Phase, Shot, Rank, DynamicRank, Power, Lives, Bombs, RespawnBombs, Dialogue, SeedMode, Seed, NewSeed, Start, Back };
    enum : unsigned { up=1,down=2,confirm=4,cancel=8,left=16,right=32 };
    PracticeSpec spec;
    PracticeCapabilities capabilities;
    NumericEditor number_editor;
    int selected=0;
    bool allowed=false;
    unsigned held=0;
    std::string warning;
    int regular_rank=1;
    bool number_editing() const {return number_editor.active();}
    bool seed_editing() const {return number_editing()&&editing_row==Row::Seed;}
    int row_count() const {return spec.section==PracticeSection::Chapter?17:16;}
    Row row(int index) const {
        if(index<0||index>=row_count())throw std::out_of_range("Practice row");
        return static_cast<Row>(index+(spec.section!=PracticeSection::Chapter&&index>=2?1:0));
    }
    struct SliderValue {int value,minimum,maximum;};
    std::optional<SliderValue> slider_value(Row which)const{
        switch(which){
        case Row::DynamicRank:return SliderValue{spec.dynamic_rank,-6,spec.rank==0?4:16};
        case Row::Power:return SliderValue{spec.power,1,80};
        case Row::Lives:return SliderValue{spec.lives,0,5};
        case Row::Bombs:return SliderValue{spec.bombs,0,5};
        case Row::RespawnBombs:return SliderValue{spec.respawn_bombs,0,3};
        default:return std::nullopt;
        }
    }
    void set_number(Row which,std::int64_t value){
        switch(which){
        case Row::DynamicRank:spec.dynamic_rank=static_cast<int>(value);break;
        case Row::Power:spec.power=static_cast<int>(value);break;
        case Row::Lives:spec.lives=static_cast<int>(value);break;
        case Row::Bombs:spec.bombs=static_cast<int>(value);break;
        case Row::RespawnBombs:spec.respawn_bombs=static_cast<int>(value);break;
        case Row::Seed:spec.seed=static_cast<std::uint32_t>(value);break;
        default:break;
        }
        warning.clear();
    }
    bool begin_number_edit(Row which){
        if(number_editing())return false;
        if(const auto slider=slider_value(which))number_editor.begin(slider->value,slider->minimum,slider->maximum);
        else if(which==Row::Seed)number_editor.begin(spec.seed,0,std::numeric_limits<std::uint32_t>::max());
        else return false;
        editing_row=which;allowed=false;warning.clear();return true;
    }
    bool number_digit(unsigned digit){return number_editor.digit(digit);}
    bool number_minus(){return number_editor.minus();}
    void number_backspace(){number_editor.backspace();}
    void number_delete(){number_editor.delete_forward();}
    void number_select_all(){number_editor.select_all();}
    bool number_paste(std::string_view text){return number_editor.paste(text);}
    bool number_paste(std::wstring_view text){return number_editor.paste(text);}
    bool number_enter(){
        std::int64_t value=0;
        // Recheck the current difficulty even if the host changed it during editing.
        if(number_editing()&&editing_row==Row::DynamicRank&&spec.rank==0){
            NumericEditor checked;checked.begin(0,-6,4);
            if(!checked.paste(number_editor.buffer)){warning="Number out of range";return false;}
        }
        if(!number_editor.accept(value)){warning="Number out of range";return false;}
        set_number(editing_row,value);allowed=false;return true;
    }
    void number_cancel(){number_editor.cancel();allowed=false;warning.clear();}
    void begin_seed_edit(){begin_number_edit(Row::Seed);}
    bool seed_digit(unsigned digit){return seed_editing()&&number_digit(digit);}
    void seed_backspace(){if(seed_editing())number_backspace();}
    void seed_enter(){if(seed_editing())number_enter();}
    void seed_cancel(){if(seed_editing())number_cancel();}
    void select_stage(int stage){
        if(stage==spec.stage)return;
        if(spec.stage!=practice_extra_stage)regular_rank=std::clamp(spec.rank,0,3);
        spec.stage=stage;
        spec.rank=stage==practice_extra_stage?4:regular_rank;
        if(spec.rank==0&&spec.dynamic_rank>4)spec.dynamic_rank=4;
        normalize_target();
    }
    int index_of(Row wanted)const{
        for(int i=0;i<row_count();++i)if(row(i)==wanted)return i;
        throw std::logic_error("Practice field is not available");
    }
    void change(int delta){
        if(number_editing())return;
        const auto current=row(selected);
        switch(current){
        case Row::Stage:
            select_stage(Menu::ring(spec.stage,delta,practice_stage_count));break;
        case Row::Section:{
            std::vector<PracticeSection> available;
            for(const auto x:capabilities.allowed_sections[spec.stage])if(capabilities.allows(spec.stage,x))available.push_back(x);
            if(available.empty())break;
            const auto it=std::find(available.begin(),available.end(),spec.section);
            const int at=it==available.end()?0:static_cast<int>(it-available.begin());
            spec.section=available[Menu::ring(at,delta,static_cast<int>(available.size()))];normalize_target();break;
        }
        case Row::Chapter:{
            const auto &entries=capabilities.chapters[spec.stage];if(entries.empty())break;
            const auto it=std::find_if(entries.begin(),entries.end(),[&](const auto &x){return x.id==spec.chapter_id;});
            const int at=it==entries.end()?0:static_cast<int>(it-entries.begin());
            spec.chapter_id=entries[Menu::ring(at,delta,static_cast<int>(entries.size()))].id;break;
        }
        case Row::Phase:
            if(spec.section==PracticeSection::Boss){spec.boss_phase=Menu::ring(spec.boss_phase,delta,int(capabilities.boss_phases[spec.stage].size())+1);if(spec.boss_phase>0)spec.dialogue=PracticeDialogue::Skip;}break;
        case Row::Shot:spec.shot=Menu::ring(spec.shot,delta,3);break;
        case Row::Rank:if(spec.stage!=practice_extra_stage){spec.rank=Menu::ring(spec.rank,delta,4);if(spec.rank==0&&spec.dynamic_rank>4){spec.dynamic_rank=4;warning="Easy dynamic rank capped at 4";}}break;
        case Row::DynamicRank:spec.dynamic_rank=static_cast<int>(std::clamp(static_cast<long long>(spec.dynamic_rank)+delta,-6LL,spec.rank==0?4LL:16LL));break;
        case Row::Power:spec.power=practice_power_step(spec.power,delta);break;
        case Row::Lives:spec.lives=Menu::ring(spec.lives,delta,6);break;
        case Row::Bombs:spec.bombs=Menu::ring(spec.bombs,delta,6);break;
        case Row::RespawnBombs:spec.respawn_bombs=Menu::ring(spec.respawn_bombs,delta,4);break;
        case Row::Dialogue:if(spec.boss_phase>0)break;spec.dialogue=spec.dialogue==PracticeDialogue::Show?PracticeDialogue::Skip:PracticeDialogue::Show;break;
        case Row::SeedMode:
            if(delta!=0){spec.seed_mode=spec.seed_mode==PracticeSeedMode::Fixed?PracticeSeedMode::Random:PracticeSeedMode::Fixed;warning.clear();}
            break;
        default:break;
        }
        selected=std::clamp(selected,0,row_count()-1);
    }
    Action update(unsigned input){
        held=input;
        if(!input){allowed=true;return Action::None;}
        if(!allowed)return Action::None;
        allowed=false;
        if(number_editing()){if(input&cancel)number_cancel();else if(input&confirm)number_enter();return Action::None;}
        if(input&cancel)return Action::Back;
        selected=std::clamp(selected,0,row_count()-1);
        if(input&up)selected=Menu::ring(selected,-1,row_count());
        if(input&down)selected=Menu::ring(selected,1,row_count());
        if(input&left)change(-1);
        if(input&right)change(1);
        if(!(input&confirm))return Action::None;
        if(begin_number_edit(row(selected)))return Action::EditNumber;
        switch(row(selected)){
        case Row::SeedMode:if(!(input&(left|right)))change(1);return Action::None;
        case Row::NewSeed:return Action::NewSeed;
        case Row::Start:warning=validate_practice(spec,capabilities);return warning.empty()?Action::Start:Action::None;
        case Row::Back:return Action::Back;
        default:return Action::None;
        }
    }
    std::string value(Row which) const {
        if(number_editing()&&which==editing_row)return number_editor.buffer;
        switch(which){
        case Row::Stage:return spec.stage==practice_extra_stage?"EXTRA":std::to_string(spec.stage+1);
        case Row::Section:{constexpr std::array<const char*,6> names={"FULL STAGE","CHAPTER","MIDBOSS","BOSS","UNAVAILABLE","MIDBOSS 2"};const int i=static_cast<int>(spec.section);return i>=0&&i<int(names.size())?names[i]:"UNAVAILABLE";}
        case Row::Chapter:if(spec.stage>=0&&spec.stage<practice_stage_count)for(const auto &c:capabilities.chapters[spec.stage])if(c.id==spec.chapter_id)return c.name;return "UNAVAILABLE";
        case Row::Phase:
            if(spec.section!=PracticeSection::Boss)return "NOT APPLICABLE";
            if(!capabilities.allows_phase(spec.stage,spec.boss_phase))return "UNAVAILABLE";
            return spec.boss_phase==0?"WHOLE BOSS":capabilities.boss_phases[spec.stage][spec.boss_phase-1];
        case Row::Shot:return std::to_string(spec.shot+1);
        case Row::Rank:return spec.stage==practice_extra_stage?"EXTRA":spec.rank>=0&&spec.rank<4?std::string(rank_names[spec.rank]):"UNAVAILABLE";
        case Row::DynamicRank:return std::to_string(spec.dynamic_rank);
        case Row::Power:if(spec.power<1||spec.power>80)return "UNAVAILABLE";return std::to_string(spec.power)+" LV "+std::to_string(PickupCore::levels.at(unsigned(spec.power)>>2));
        case Row::Lives:return std::to_string(spec.lives);
        case Row::Bombs:return std::to_string(spec.bombs);
        case Row::RespawnBombs:return std::to_string(spec.respawn_bombs);
        case Row::Dialogue:return spec.boss_phase>0?"SKIP AT PHASE":spec.dialogue==PracticeDialogue::Show?"SHOW":"SKIP";
        case Row::SeedMode:return spec.seed_mode==PracticeSeedMode::Random?"RANDOM":"FIXED";
        case Row::Seed:return std::to_string(spec.seed);
        default:return {};
        }
    }
private:
    Row editing_row=Row::Seed;
    void normalize_target(){
        if(!capabilities.allows(spec.stage,spec.section))for(const auto x:capabilities.allowed_sections[spec.stage])if(capabilities.allows(spec.stage,x)){spec.section=x;break;}
        if(spec.section!=PracticeSection::Chapter)spec.chapter_id=-1;
        else if(!capabilities.has_chapter(spec.stage,spec.chapter_id)&&!capabilities.chapters[spec.stage].empty())spec.chapter_id=capabilities.chapters[spec.stage].front().id;
        if(spec.section!=PracticeSection::Boss||!capabilities.allows_phase(spec.stage,spec.boss_phase))spec.boss_phase=0;
        if(spec.boss_phase>0)spec.dialogue=PracticeDialogue::Skip;
    }
};
inline std::wstring practice_value(const PracticeMenu& menu, PracticeMenu::Row row, Language lang) {
    using R=PracticeMenu::Row;
    const auto raw=menu.value(row);
    const auto wide=[](std::string_view s){return std::wstring(s.begin(),s.end());};
    if(menu.number_editing()&&raw==menu.number_editor.buffer)return wide(raw);
    if(raw=="UNAVAILABLE")return text::unavailable(lang);
    if(row==R::Stage&&menu.spec.stage==practice_extra_stage)return text::extra(lang);
    if(row==R::Rank)return menu.spec.stage==practice_extra_stage?text::extra(lang):text::difficulty.at(menu.spec.rank)(lang);
    if(lang==Language::English)return wide(raw);
    if(row==R::Section){
        constexpr std::array<LocalizedText,6> names={{{L"ステージ通し",L"",L"完整关卡",L"完整關卡"},{L"区間",L"",L"区段",L"區段"},{L"中ボス",L"",L"中途首领",L"中途頭目"},{L"ボス",L"",L"首领",L"頭目"},{L"選択不可",L"",L"不可选",L"無法選擇"},{L"中ボス２",L"",L"第二个中途首领",L"第二個中途頭目"}}};
        return names.at(static_cast<unsigned>(menu.spec.section))(lang);
    }
    const auto tr=[&](const wchar_t* ja,const wchar_t* hans,const wchar_t* hant){return localized(lang,ja,L"",hans,hant);};
    if(row==R::Dialogue)return menu.spec.boss_phase>0?tr(L"途中開始のため省略",L"从指定阶段开始时跳过",L"從指定階段開始時略過"):menu.spec.dialogue==PracticeDialogue::Show?tr(L"表示",L"显示",L"顯示"):tr(L"省略",L"跳过",L"略過");
    if(row==R::SeedMode)return menu.spec.seed_mode==PracticeSeedMode::Random?tr(L"ランダム",L"随机",L"隨機"):L"固定";
    if(row==R::Chapter){
        if(raw=="PRE BOSS")return tr(L"ボス直前",L"首领战前",L"頭目戰前");
        if(raw.starts_with("CHAPTER "))return tr(L"区間 ",L"区段 ",L"區段 ")+wide(std::string_view(raw).substr(8));
    }
    if(row==R::Phase){
        if(raw=="NOT APPLICABLE")return tr(L"対象外",L"不适用",L"不適用");
        if(raw=="WHOLE BOSS")return tr(L"ボス最初から",L"完整首领战",L"完整頭目戰");
        const auto phase=menu.spec.boss_phase;
        constexpr std::array<LocalizedText,5> stones={{{L"内側の魔石",L"",L"内侧魔石",L"內側魔石"},{L"外側の魔石",L"",L"外侧魔石",L"外側魔石"},{L"北の魔石：開く",L"",L"北方魔石：开启",L"北方魔石：開啟"},{L"北の魔石：閉じる",L"",L"北方魔石：闭合",L"北方魔石：閉合"},{L"北の魔石：最終",L"",L"北方魔石：最终阶段",L"北方魔石：最終階段"}}};
        constexpr std::array<LocalizedText,5> mima={{{L"第１攻撃",L"",L"第一轮攻击",L"第一輪攻擊"},{L"第２攻撃",L"",L"第二轮攻击",L"第二輪攻擊"},{L"第３攻撃",L"",L"第三轮攻击",L"第三輪攻擊"},{L"複合攻撃",L"",L"复合攻击",L"複合攻擊"},{L"最終形態",L"",L"最终形态",L"最終型態"}}};
        if(menu.spec.stage==2)return stones.at(phase-1)(lang);
        if(menu.spec.stage==3)return tr(L"ラウンド ",L"回合 ",L"回合 ")+std::to_wstring(phase);
        if(menu.spec.stage==4)return mima.at(phase-1)(lang);
        if(raw=="FINAL PHASE")return tr(L"最終段階",L"最终阶段",L"最終階段");
        return tr(L"段階 ",L"阶段 ",L"階段 ")+std::to_wstring(phase);
    }
    if(row==R::Power&&lang!=Language::Japanese&&menu.spec.power>=1&&menu.spec.power<=80)
        return std::to_wstring(menu.spec.power)+tr(L"",L" 等级 ",L" 等級 ")+std::to_wstring(PickupCore::levels.at(unsigned(menu.spec.power)>>2));
    return wide(raw);
}
inline std::wstring practice_warning(std::string_view message, Language lang) {
    if(lang==Language::English)return {message.begin(),message.end()};
    const auto tr=[&](const wchar_t* ja,const wchar_t* hans,const wchar_t* hant){return localized(lang,ja,L"",hans,hant);};
    if(message=="Number out of range")return tr(L"数値が範囲外です",L"数值超出范围",L"數值超出範圍");
    if(message=="Easy dynamic rank capped at 4")return tr(L"イージーの変動ランク上限は４です",L"简单难度的动态难度上限为 4",L"簡單難度的動態難度上限為 4");
    if(message=="Previous request is still cancelling")return tr(L"先ほどの処理を取り消しています",L"正在取消上次请求",L"正在取消上次請求");
    if(message=="Practice entry cancelled"||message=="Practice preparation cancelled")return tr(L"練習の準備を取り消しました",L"已取消练习准备",L"已取消練習準備");
    if(message=="Clipboard must contain an integer in range")return tr(L"範囲内の整数を貼り付けてください",L"请粘贴范围内的整数",L"請貼上範圍內的整數");
    if(message.find("config")!=std::string_view::npos)return tr(L"練習設定を読み書きできません。元のファイルは保持されます",L"无法读写练习设置，原文件已保留。",L"無法讀寫練習設定，原檔已保留。");
    return tr(L"選択した条件では練習を開始できません",L"无法按所选条件开始练习",L"無法依所選條件開始練習");
}

struct PracticeStore {
    std::filesystem::path path;
    PracticeSpec saved;
    bool writable=true;
    std::string warning;
    explicit PracticeStore(std::filesystem::path root):path(std::move(root)/"user"/"soew-practice.json"){}
    PracticeSpec load(int defaultRespawn=3){
        warning.clear();writable=true;loaded=true;exists=false;migrate=false;PracticeSpec out;
        out.respawn_bombs=std::clamp(defaultRespawn,0,3);const auto defaults=out;
        try{
            exists=std::filesystem::exists(path);
            if(!exists){saved=out;return out;}
            std::ifstream file(path,std::ios::binary|std::ios::ate);
            if(!file)throw std::runtime_error("Cannot read practice config; file protected");
            const auto size=file.tellg();if(size<2||size>16384)throw std::runtime_error("Invalid practice config size; file protected");
            std::string data(static_cast<std::size_t>(size),' ');file.seekg(0);
            if(!file.read(data.data(),static_cast<std::streamsize>(size)))throw std::runtime_error("Cannot read practice config; file protected");
            const auto fields=SettingsStore::parse(data);
            const auto version=fields.find("version");
            if(version==fields.end()||!version->second.integer||(version->second.text!="1"&&version->second.text!="2"&&version->second.text!="3"&&version->second.text!="4"&&version->second.text!="5"))throw std::runtime_error("Unsupported practice config version; file protected");
            const bool has_seed_mode=version->second.text=="5";
            const bool expanded=has_seed_mode||version->second.text=="4";
            const bool has_phase=expanded||version->second.text=="3";
            migrate=!has_seed_mode||fields.find("respawn_bombs")==fields.end()||fields.find("seed_mode")==fields.end();
            constexpr std::array<std::string_view,14> keys={"version","stage","section","chapter_id","boss_phase","shot","rank","dynamic_rank","power","lives","bombs","respawn_bombs","dialogue","seed"};
            for(const auto &[key,value]:fields){
                (void)value;
                if(std::find(keys.begin(),keys.end(),key)==keys.end()&&!(has_seed_mode&&key=="seed_mode"))invalid();
            }
            auto integer=[&](const char *key,int &target,int min,int max){
                const auto it=fields.find(key);if(it==fields.end())return;
                int value=0;const auto &x=it->second;const auto parsed=std::from_chars(x.text.data(),x.text.data()+x.text.size(),value);
                if(!x.integer||parsed.ec!=std::errc{}||parsed.ptr!=x.text.data()+x.text.size()||value<min||value>max){invalid();return;}target=value;
            };
            integer("stage",out.stage,0,expanded?practice_extra_stage:2);
            int section=static_cast<int>(out.section);integer("section",section,0,expanded?static_cast<int>(PracticeSection::Midboss2):3);
            if(section==4){out.section=PracticeSection::StageStart;migrate=true;}
            else out.section=static_cast<PracticeSection>(section);
            const bool extra=out.stage==practice_extra_stage;
            if(extra)out.rank=4;
            integer("chapter_id",out.chapter_id,-1,std::numeric_limits<int>::max());integer("shot",out.shot,0,2);integer("rank",out.rank,extra?4:0,extra?4:3);integer("power",out.power,1,80);integer("lives",out.lives,0,5);integer("bombs",out.bombs,0,5);
            if(version->second.text!="1")integer("dynamic_rank",out.dynamic_rank,-6,out.rank==0?4:16);
            if(has_phase){integer("respawn_bombs",out.respawn_bombs,0,3);integer("boss_phase",out.boss_phase,0,expanded?practice_max_boss_phase:3);}
            int dialogue=static_cast<int>(out.dialogue);integer("dialogue",dialogue,0,1);out.dialogue=static_cast<PracticeDialogue>(dialogue);
            if(has_seed_mode){
                int seed_mode=0;integer("seed_mode",seed_mode,0,1);out.seed_mode=static_cast<PracticeSeedMode>(seed_mode);
            }
            if(const auto it=fields.find("seed");it!=fields.end()){
                const auto &x=it->second;const auto &text=x.text;std::uint32_t seed=0;
                const auto result=std::from_chars(text.data(),text.data()+text.size(),seed);
                if(x.string||text.empty()||text.front()=='-'||result.ec!=std::errc{}||result.ptr!=text.data()+text.size())invalid();else out.seed=seed;
            }
            if(out.section==PracticeSection::Chapter&&out.chapter_id<0){invalid();out.section=PracticeSpec{}.section;out.chapter_id=-1;}
            if(out.section!=PracticeSection::Chapter&&out.chapter_id!=-1){invalid();out.chapter_id=-1;}
            if(out.section!=PracticeSection::Boss&&out.boss_phase!=0){invalid();out.boss_phase=0;}
            if(out.boss_phase>0&&out.dialogue!=PracticeDialogue::Skip){invalid();out.dialogue=PracticeDialogue::Skip;}
        }catch(const std::exception &e){warning=e.what();writable=false;out=defaults;}
        saved=out;return out;
    }
    bool save(const PracticeSpec &value){
        if(!loaded)load();
        if(!writable)return false;
        if(const auto error=validate_practice(value);!error.empty()){warning=error;return false;}
        if(exists&&value==saved&&!migrate)return true;
        std::filesystem::path temp;
        try{
            std::filesystem::create_directories(path.parent_path());
            static std::atomic<unsigned long long> sequence{0};temp=path;
            temp+=".tmp-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())+"-"+std::to_string(sequence++);
            const auto data=std::string("{\n  \"version\": 5,\n  \"stage\": ")+std::to_string(value.stage)+",\n  \"section\": "+std::to_string(static_cast<int>(value.section))+",\n  \"chapter_id\": "+std::to_string(value.chapter_id)+",\n  \"boss_phase\": "+std::to_string(value.boss_phase)+",\n  \"shot\": "+std::to_string(value.shot)+",\n  \"rank\": "+std::to_string(value.rank)+",\n  \"dynamic_rank\": "+std::to_string(value.dynamic_rank)+",\n  \"power\": "+std::to_string(value.power)+",\n  \"lives\": "+std::to_string(value.lives)+",\n  \"bombs\": "+std::to_string(value.bombs)+",\n  \"respawn_bombs\": "+std::to_string(value.respawn_bombs)+",\n  \"dialogue\": "+std::to_string(static_cast<int>(value.dialogue))+",\n  \"seed_mode\": "+std::to_string(static_cast<int>(value.seed_mode))+",\n  \"seed\": "+std::to_string(value.seed)+"\n}\n";
#ifdef _WIN32
            const auto file=CreateFileW(temp.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);
            if(file==INVALID_HANDLE_VALUE)throw std::runtime_error("Cannot create temporary practice config");
            DWORD written=0;
            const bool flushed=WriteFile(file,data.data(),static_cast<DWORD>(data.size()),&written,nullptr)&&written==data.size()&&FlushFileBuffers(file);
            const bool closed=CloseHandle(file)!=0;
            if(!flushed||!closed)throw std::runtime_error("Cannot flush practice config");
            if(!MoveFileExW(temp.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))throw std::runtime_error("Atomic practice config replace failed");
#else
            {std::ofstream file(temp,std::ios::binary|std::ios::trunc);if(!file)throw std::runtime_error("Cannot create temporary practice config");file<<data;file.flush();if(!file)throw std::runtime_error("Cannot flush practice config");file.close();if(!file)throw std::runtime_error("Cannot close practice config");}
            std::filesystem::rename(temp,path);
#endif
            saved=value;exists=true;migrate=false;warning.clear();return true;
        }catch(const std::exception &e){if(!temp.empty()){std::error_code ignored;std::filesystem::remove(temp,ignored);}warning=e.what();return false;}
    }
private:
    bool loaded=false,exists=false,migrate=false;
    void invalid(){writable=false;warning="Invalid or unknown practice config fields; defaults used; file protected";}
};
}
