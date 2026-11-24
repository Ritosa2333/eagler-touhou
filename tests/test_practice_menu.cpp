#include "practice_targets.hpp"
#include "stage3_practice_evidence.hpp"
#include "practice_music.hpp"
#include <iostream>
#include <source_location>

namespace {
unsigned checks=0;
void check(bool condition,const std::source_location at=std::source_location::current()){
    ++checks;
    if(!condition)throw std::runtime_error("Practice menu check="+std::to_string(checks)+" line="+std::to_string(at.line()));
}
void put(const std::filesystem::path &path,std::string_view data){
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path,std::ios::binary|std::ios::trunc);file<<data;
    if(!file)throw std::runtime_error("Cannot write isolated practice fixture");
}
std::string read(const std::filesystem::path &path){
    std::ifstream file(path,std::ios::binary);
    if(!file)throw std::runtime_error("Cannot read isolated practice fixture");
    return {std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>()};
}
void tap(soew::PracticeMenu &menu,unsigned input){menu.update(0);menu.update(input);}
int row_index(const soew::PracticeMenu &menu,soew::PracticeMenu::Row wanted){
    for(int i=0;i<menu.row_count();++i)if(menu.row(i)==wanted)return i;
    throw std::runtime_error("Missing practice row");
}
void test_editor(){
    using namespace soew;
    NumericEditor editor;
    std::int64_t value=987;
    check(!editor.digit(1)&&!editor.paste("1")&&!editor.accept(value)&&value==987);
    editor.begin(80,1,80);check(editor.active()&&editor.buffer=="80");
    check(editor.digit(9)&&editor.buffer=="9");check(editor.digit(9)&&editor.buffer=="99");
    check(!editor.accept(value)&&value==987&&editor.active());
    check(!editor.paste("81")&&editor.buffer=="99");
    for(const auto bad:{""," ","-","+1","1e1","1.0","1 2","12x","0","-1","9223372036854775808"})check(!editor.paste(bad)&&editor.buffer=="99");
    check(!editor.paste(std::string(257,' '))&&editor.buffer=="99");
    check(editor.paste(" \t80\r\n")&&editor.accept(value)&&value==80&&!editor.active());
    editor.begin(16,-6,16);check(editor.minus()&&editor.buffer=="-");
    check(!editor.accept(value)&&value==80);check(editor.digit(6)&&editor.buffer=="-6");
    check(editor.accept(value)&&value==-6);
    editor.begin(16,-6,16);editor.select_all();editor.backspace();check(editor.buffer.empty());
    check(editor.digit(1)&&editor.digit(6));editor.home();editor.delete_forward();check(editor.buffer=="6");
    editor.end();editor.backspace();check(editor.buffer.empty());
    editor.select_all();check(editor.paste("-5"));editor.move_left();editor.delete_forward();check(editor.buffer=="-");
    editor.home();check(!editor.minus());editor.end();check(editor.digit(4));editor.move_left();editor.move_right();check(editor.accept(value)&&value==-4);
    editor.begin(0,0,std::numeric_limits<std::uint32_t>::max());check(!editor.minus());
    check(editor.paste("4294967295"));check(!editor.paste("4294967296")&&editor.buffer=="4294967295");
    check(editor.accept(value)&&value==4294967295LL);
    editor.begin(0,std::numeric_limits<std::int64_t>::min(),std::numeric_limits<std::int64_t>::max());
    check(editor.paste("-9223372036854775808")&&editor.accept(value)&&value==std::numeric_limits<std::int64_t>::min());
    editor.begin(0,std::numeric_limits<std::int64_t>::min(),std::numeric_limits<std::int64_t>::max());
    check(editor.paste(L"\u3000\t\uff0d\uff19\uff12\uff12\uff13\uff13\uff17\uff12\uff10\uff13\uff16\uff18\uff15\uff14\uff17\uff17\uff15\uff18\uff10\uff18\u3000")&&editor.buffer=="-9223372036854775808");
    for(const auto bad:{L"",L"\u3000",L"\uff0d",L"\uff11\u3000\uff12",L"\uff11x",L"\uff0b\uff11",L"\uff11\uff0d\uff12",L"\uff19\uff12\uff12\uff13\uff13\uff17\uff12\uff10\uff13\uff16\uff18\uff15\uff14\uff17\uff17\uff15\uff18\uff10\uff18",L"\uff0d\uff19\uff12\uff12\uff13\uff13\uff17\uff12\uff10\uff13\uff16\uff18\uff15\uff14\uff17\uff17\uff15\uff18\uff10\uff19"})check(!editor.paste(bad)&&editor.buffer=="-9223372036854775808");
    check(!editor.paste(std::wstring(257,L' '))&&editor.buffer=="-9223372036854775808");
    check(editor.paste(L" \uff11\uff12 3\t")==false&&editor.buffer=="-9223372036854775808");
    check(editor.paste(L" \uff11\uff123\t")&&editor.accept(value)&&value==123);
    check(!editor.paste(L"\uff11"));
    editor.begin(0,0,5);check(editor.digit(9));editor.cancel();check(!editor.active()&&!editor.accept(value));
}
void test_model(){
    using namespace soew;
    using Row=PracticeMenu::Row;using Action=PracticeMenu::Action;
    const PracticeSpec defaults;
    check(defaults.stage==1&&defaults.section==PracticeSection::Boss&&defaults.chapter_id==-1);
    check(defaults.shot==1&&defaults.rank==1&&defaults.dynamic_rank==0&&defaults.power==80&&defaults.lives==2&&defaults.bombs==3);
    check(defaults.dialogue==PracticeDialogue::Skip&&defaults.seed==1234&&defaults.respawn_bombs==3&&defaults.boss_phase==0);
    check(defaults.seed_mode==PracticeSeedMode::Fixed);
    auto caps=practice_capabilities();check(validate_practice(defaults,caps).empty());
    auto invalid=defaults;invalid.stage=6;check(!validate_practice(invalid,caps).empty());
    invalid=defaults;invalid.seed_mode=static_cast<PracticeSeedMode>(2);check(!validate_practice(invalid,caps).empty());
    caps.chapters[1].clear();
    invalid=defaults;invalid.section=PracticeSection::Chapter;invalid.chapter_id=0;check(!validate_practice(invalid,caps).empty());
    caps.allowed_sections[1].push_back(PracticeSection::Chapter);check(!validate_practice(invalid,caps).empty());
    // Sparse synthetic metadata, not game chapter IDs.
    caps.chapters[1]={{7,"FIXTURE A"},{42,"FIXTURE B"}};
    invalid.chapter_id=7;check(validate_practice(invalid,caps).empty());
    invalid.chapter_id=8;check(!validate_practice(invalid,caps).empty());
    invalid=defaults;invalid.chapter_id=7;check(!validate_practice(invalid,caps).empty());
    for(int rank=0;rank<4;++rank)for(int shot=0;shot<3;++shot){auto valid=defaults;valid.rank=rank;valid.shot=shot;check(validate_practice(valid,caps).empty());}
    for(int rank=0;rank<4;++rank){
        auto valid=defaults;valid.rank=rank;
        for(int dynamic=-6;dynamic<=(rank==0?4:16);++dynamic){valid.dynamic_rank=dynamic;check(validate_practice(valid,caps).empty());}
        valid.dynamic_rank=-7;check(!validate_practice(valid,caps).empty());valid.dynamic_rank=rank==0?5:17;check(!validate_practice(valid,caps).empty());
    }
    for(int lives=0;lives<=5;++lives)for(int bombs=0;bombs<=5;++bombs)for(int respawn=0;respawn<=3;++respawn){auto valid=defaults;valid.lives=lives;valid.bombs=bombs;valid.respawn_bombs=respawn;check(validate_practice(valid,caps).empty());}
    for(int phase=0;phase<=3;++phase){auto valid=defaults;valid.boss_phase=phase;check(validate_practice(valid,caps).empty());}
    invalid=defaults;invalid.power=0;check(!validate_practice(invalid,caps).empty());
    invalid=defaults;invalid.power=81;check(!validate_practice(invalid,caps).empty());
    invalid=defaults;invalid.lives=6;check(!validate_practice(invalid,caps).empty());
    invalid=defaults;invalid.bombs=6;check(!validate_practice(invalid,caps).empty());
    invalid=defaults;invalid.respawn_bombs=4;check(!validate_practice(invalid,caps).empty());
    invalid=defaults;invalid.boss_phase=4;check(!validate_practice(invalid,caps).empty());
    invalid=defaults;invalid.boss_phase=1;invalid.dialogue=PracticeDialogue::Show;check(!validate_practice(invalid,caps).empty());
    invalid=defaults;invalid.boss_phase=1;invalid.section=PracticeSection::Midboss;check(!validate_practice(invalid,caps).empty());
    check(practice_power_step(80,1)==80&&practice_power_step(1,-1)==1);
    check(practice_power_step(50,1)==51&&practice_power_step(50,-1)==49);
    check(practice_power_step(1,std::numeric_limits<int>::max())==80&&practice_power_step(80,std::numeric_limits<int>::min())==1);
    PracticeMenu m;m.capabilities=practice_capabilities();m.capabilities.chapters[1].clear();check(m.row_count()==16&&m.row(2)==Row::Phase);
    check(m.update(PracticeMenu::down)==Action::None&&m.selected==0);
    tap(m,PracticeMenu::down);check(m.selected==1);m.update(PracticeMenu::down);check(m.selected==1);
    tap(m,PracticeMenu::up);check(m.selected==0);tap(m,PracticeMenu::up);check(m.row(m.selected)==Row::Back);
    m.update(0);check(m.update(PracticeMenu::confirm)==Action::Back);m.update(0);check(m.update(PracticeMenu::cancel)==Action::Back);
    m.selected=1;for(int i=0;i<12;++i){tap(m,PracticeMenu::right);check(m.spec.section!=PracticeSection::Chapter);}
    m.capabilities=caps;m.spec=defaults;m.spec.section=PracticeSection::StageStart;
    tap(m,PracticeMenu::left);check(m.spec.section==PracticeSection::Chapter&&m.spec.chapter_id==7&&m.row_count()==17&&m.row(2)==Row::Chapter);
    m.selected=2;tap(m,PracticeMenu::right);check(m.spec.chapter_id==42);tap(m,PracticeMenu::right);check(m.spec.chapter_id==7);
    for(int i=0;i<m.row_count();++i){m.selected=i;check(m.visible_row_count()<=12&&m.first_visible_row()<=i&&i<m.first_visible_row()+m.visible_row_count());}
    m.capabilities.chapters[0].clear();m.selected=0;tap(m,PracticeMenu::left);check(m.spec.stage==0&&m.spec.section!=PracticeSection::Chapter&&m.spec.chapter_id==-1&&m.row_count()==16);
    m.spec=defaults;m.selected=row_index(m,Row::Phase);m.spec.dialogue=PracticeDialogue::Show;
    tap(m,PracticeMenu::right);check(m.spec.boss_phase==1&&m.spec.dialogue==PracticeDialogue::Skip);
    m.selected=row_index(m,Row::Dialogue);tap(m,PracticeMenu::right);check(m.spec.dialogue==PracticeDialogue::Skip);
    m.capabilities.boss_phases[0].clear();m.selected=0;tap(m,PracticeMenu::left);check(m.spec.stage==0&&m.spec.boss_phase==0);
    m.selected=row_index(m,Row::Phase);tap(m,PracticeMenu::right);check(m.spec.boss_phase==0);
    invalid=defaults;invalid.stage=0;invalid.boss_phase=1;check(!validate_practice(invalid,m.capabilities).empty());
    m.spec=defaults;m.selected=row_index(m,Row::Power);tap(m,PracticeMenu::right);check(m.spec.power==80);
    tap(m,PracticeMenu::left);check(m.spec.power==79);m.spec.power=1;tap(m,PracticeMenu::left);check(m.spec.power==1);
    for(const auto r:{Row::Shot,Row::Rank,Row::Lives,Row::Bombs,Row::RespawnBombs,Row::Dialogue}){
        m.selected=row_index(m,r);for(int i=0;i<20;++i){tap(m,PracticeMenu::left);check(validate_practice(m.spec,m.capabilities).empty());}
    }
    m.selected=row_index(m,Row::Start);m.update(0);check(m.update(PracticeMenu::confirm)==Action::Start);
    m.capabilities.allowed_sections[m.spec.stage].clear();m.update(0);check(m.update(PracticeMenu::confirm)==Action::None&&!m.warning.empty());
    m.capabilities=caps;m.spec=defaults;
    const auto before=m.spec.seed;
    m.selected=row_index(m,Row::SeedMode);check(m.value(Row::SeedMode)=="FIXED");
    tap(m,PracticeMenu::right);check(m.value(Row::SeedMode)=="RANDOM"&&m.value(Row::Seed)==std::to_string(before));
    check(m.begin_number_edit(Row::Seed)&&m.number_paste("4321")&&m.number_enter());
    check(m.spec.seed_mode==PracticeSeedMode::Random&&m.spec.seed==4321);
    tap(m,PracticeMenu::confirm);check(m.value(Row::SeedMode)=="FIXED"&&m.spec.seed==4321);
    tap(m,PracticeMenu::left);check(m.value(Row::SeedMode)=="RANDOM"&&m.spec.seed==4321);
    m.spec=defaults;
    m.selected=row_index(m,Row::NewSeed);m.update(0);check(m.update(PracticeMenu::confirm)==Action::NewSeed&&m.spec.seed==before);
    for(const auto r:{Row::DynamicRank,Row::Power,Row::Lives,Row::Bombs,Row::RespawnBombs,Row::Seed}){
        m.selected=row_index(m,r);m.update(0);check(m.update(PracticeMenu::confirm)==Action::EditNumber&&m.number_editing());
        const auto original=m.spec;check(m.number_digit(9)&&m.spec==original);const int selected=m.selected;
        tap(m,PracticeMenu::down|PracticeMenu::right);check(m.selected==selected&&m.spec==original);
        tap(m,PracticeMenu::cancel);check(!m.number_editing()&&m.spec==original);
    }
    check(m.begin_number_edit(Row::Lives));check(m.number_digit(6));check(!m.number_enter()&&m.number_editing()&&m.spec.lives==2);
    m.number_select_all();m.number_delete();check(!m.number_enter()&&m.spec.lives==2);
    check(!m.number_paste(L"\uff16")&&m.spec.lives==2);
    check(m.number_paste(L"\u3000\uff15\u3000")&&m.spec.lives==2&&m.number_enter()&&m.spec.lives==5);
    check(m.begin_number_edit(Row::DynamicRank));check(m.number_paste("16"));m.spec.rank=0;
    check(!m.number_enter()&&m.spec.dynamic_rank==0&&m.number_editing());check(m.number_paste("-6")&&m.number_enter()&&m.spec.dynamic_rank==-6);
    m.spec.rank=1;m.begin_seed_edit();check(m.seed_editing());check(m.number_paste("4294967295")&&m.spec.seed==before);
    check(!m.number_paste("4294967296"));m.seed_enter();check(m.spec.seed==4294967295u&&!m.seed_editing());
    m.begin_seed_edit();check(m.seed_digit(0)&&m.spec.seed==4294967295u);m.seed_enter();check(m.spec.seed==0);
    m.begin_seed_edit();m.seed_digit(9);m.seed_cancel();check(m.spec.seed==0&&!m.seed_editing());
    m.begin_seed_edit();m.seed_digit(8);tap(m,PracticeMenu::confirm);check(m.spec.seed==8&&m.value(Row::Seed)=="8");
    m.spec.dynamic_rank=16;m.selected=row_index(m,Row::Rank);tap(m,PracticeMenu::left);check(m.spec.rank==0&&m.spec.dynamic_rank==4);
    m.capabilities=practice_capabilities();m.spec=defaults;m.spec.stage=4;m.spec.rank=3;m.selected=row_index(m,Row::Stage);
    tap(m,PracticeMenu::right);check(m.spec.stage==5&&m.spec.rank==4&&m.value(Row::Stage)=="EXTRA");
    m.selected=row_index(m,Row::Rank);tap(m,PracticeMenu::left);check(m.spec.rank==4);
    m.selected=row_index(m,Row::Stage);tap(m,PracticeMenu::right);check(m.spec.stage==0&&m.spec.rank==3);
    for(int stage=0;stage<practice_stage_count;++stage){
        auto full=defaults;full.stage=stage;full.rank=stage==5?4:1;
        check(!m.capabilities.allows(stage,static_cast<PracticeSection>(4)));
        for(const auto section:{PracticeSection::StageStart,PracticeSection::Boss}){
            full.section=section;check(validate_practice(full,m.capabilities).empty());
        }
    }
}
void test_font(){
    using namespace soew;
    th03::assets::BfntSheet font{};
    font.header.first_code=0;font.header.last_code=255;
    for(int i=0;i<256;++i){
        th03::assets::MonochromeBitmap1 glyph{};glyph.bits.resize(32);glyph.bits[0]=0x80;
        font.glyphs.push_back({static_cast<std::uint8_t>(i),std::move(glyph)});
    }
    PracticeMenu menu;menu.capabilities=practice_capabilities();
    const auto title_x=[](Language lang){return 320-menu_text_width(text::practice_start(lang))/2;};
    check(title_x(Language::English)==208&&title_x(Language::Japanese)==288);
    std::vector<std::uint32_t> pixels(640*400);
    for(const auto lang:{Language::English,Language::Japanese}){
        bool title_fallback=false,first_value_fallback=false;
        draw_practice_menu(pixels,menu,lang,[&](std::wstring_view label,int x,int y,std::uint32_t color){
            check(x>=0&&x+menu_text_width(label)<=640);
            draw_menu_text(pixels,font,label,x,y,color,[&](std::wstring_view run,int left,int top,std::uint32_t){
                if(top==16)title_fallback=run==L"練習開始"&&left==title_x(lang);
                if(top==50&&left>=352)first_value_fallback=true;
            });
        });
        check(title_fallback==(lang==Language::Japanese)&&!first_value_fallback);
        check(pixels[50*640+352]==0xff172b43u&&pixels[50*640+353]==0xffd6eaffu);
        if(lang==Language::English)check(pixels[16*640+title_x(lang)]==0xff18304cu);
    }
    check(menu_gaiji_code(L'm')==glyph_code('M')&&menu_gaiji_code(L'日')<0);
}
void test_store(const std::filesystem::path &root){
    using namespace soew;
    const auto fixture=root/("practice-menu-contract-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    if(std::filesystem::exists(fixture))throw std::runtime_error("Practice fixture already exists; supply a fresh isolated test root");
    std::filesystem::create_directories(fixture);
    PracticeStore fresh(fixture/"new-config");check(fresh.path==fixture/"new-config"/"user"/"soew-practice.json");
    check(!std::filesystem::exists(fresh.path));check(fresh.load()==PracticeSpec{}&&fresh.writable);
    check(fresh.save(PracticeSpec{})&&std::filesystem::exists(fresh.path));check(PracticeStore(fixture/"new-config").load()==PracticeSpec{});
    auto changed=PracticeSpec{};changed.stage=0;changed.section=PracticeSection::Midboss;changed.shot=2;changed.rank=3;
    changed.power=1;changed.lives=5;changed.bombs=5;changed.respawn_bombs=0;changed.seed=4294967295u;
    if(!fresh.save(changed))throw std::runtime_error("save changed: "+fresh.warning);
    const auto roundtrip=PracticeStore(fixture/"new-config").load(1);if(!(roundtrip==changed))std::cerr<<"roundtrip phase="<<roundtrip.boss_phase<<" respawn="<<roundtrip.respawn_bombs<<" seed="<<roundtrip.seed<<" shot="<<roundtrip.shot<<" lives="<<roundtrip.lives<<" bombs="<<roundtrip.bombs<<'\n';check(roundtrip==changed);
    const auto valid=read(fresh.path);changed.power=0;check(!fresh.save(changed)&&read(fresh.path)==valid);
    PracticeStore host_defaults(fixture/"host-defaults");check(host_defaults.load(1).respawn_bombs==1);
    PracticeStore typed(fixture/"typed");
    put(typed.path,"{\"version\":1,\"rank\":99,\"power\":40,\"seed\":4294967295}");
    const auto partial=typed.load();check(partial.rank==1&&partial.power==40&&partial.seed==4294967295u&&!typed.writable);check(!typed.save(PracticeSpec{}));
    for(const char *bad:{"\"wrong\"","\"1234\"","[]","{}","1.5","null","true","-1","4294967296","1e2"}){
        const auto data=std::string("{\"version\":3,\"seed\":")+bad+",\"rank\":3}";
        put(typed.path,data);const auto value=typed.load();check(value.seed==1234&&value.rank==3&&!typed.writable&&!typed.save(PracticeSpec{}));check(read(typed.path)==data);
    }
    for(const char *bad:{"\"wrong\"","[]","{}","1.5","null","true","-1","99"}){
        put(typed.path,std::string("{\"version\":1,\"rank\":")+bad+",\"power\":40}");const auto value=typed.load();check(value.rank==1&&value.power==40&&!typed.writable);
    }
    for(const char *data:{"bad json","{\"version\":99}","{\"version\":true}","{}","{\"version\":1,\"rank\":2,\"rank\":3}","{\"version\":1,\"future\":{\"x\":[true]}}","{\"version\":1,\"rank\":3,}","{\"version\":1} trailing"}){
        put(typed.path,data);typed.load();check(!typed.writable&&!typed.save(PracticeSpec{}));check(read(typed.path)==data);
    }
    for(int version=1;version<=2;++version){
        put(typed.path,"{\"version\":"+std::to_string(version)+",\"shot\":2,\"lives\":5,\"bombs\":5,\"seed\":4294967295}");
        const auto legacy=typed.load(1);check(legacy.shot==2&&legacy.lives==5&&legacy.bombs==5&&legacy.seed==4294967295u&&legacy.seed_mode==PracticeSeedMode::Fixed&&legacy.dynamic_rank==0&&legacy.respawn_bombs==1&&legacy.boss_phase==0&&typed.writable);
        check(typed.save(legacy)&&read(typed.path).find("\"version\": 5")!=std::string::npos);
        check(typed.load(3).respawn_bombs==1);const auto bytes=read(typed.path);const auto stamp=std::filesystem::last_write_time(typed.path);
        check(typed.save(typed.saved)&&read(typed.path)==bytes&&std::filesystem::last_write_time(typed.path)==stamp);
    }
    put(typed.path,"{\"version\":3,\"respawn_bombs\":0,\"boss_phase\":3,\"dialogue\":1}");
    auto phase=typed.load(2);check(phase.respawn_bombs==0&&phase.boss_phase==3&&typed.writable);check(typed.save(phase)&&typed.load(3)==phase);
    put(typed.path,"{\"version\":3}");auto missing=typed.load(2);check(missing.respawn_bombs==2&&typed.save(missing)&&typed.load(0).respawn_bombs==2);
    for(const auto field:{"\"respawn_bombs\":4","\"respawn_bombs\":-1","\"respawn_bombs\":\"2\"","\"boss_phase\":4","\"boss_phase\":\"1\"","\"boss_phase\":1,\"section\":0","\"boss_phase\":1,\"dialogue\":0"}){
        const auto data=std::string("{\"version\":3,")+field+"}";put(typed.path,data);typed.load(2);check(!typed.writable&&!typed.save(PracticeSpec{})&&read(typed.path)==data);
    }
    put(typed.path,"{\"version\":1}");check(typed.load()==PracticeSpec{}&&typed.writable);
    put(typed.path,"{\"version\":1,\"section\":1,\"chapter_id\":42,\"seed\":0}");
    const auto chapter=typed.load();check(chapter.section==PracticeSection::Chapter&&chapter.chapter_id==42&&chapter.seed==0&&typed.writable);check(!validate_practice(chapter,PracticeCapabilities{}).empty());
    put(typed.path,"{\"version\":1,\"section\":1}");check(typed.load().section==PracticeSection::Boss&&!typed.writable);
    put(typed.path,"{\"version\":1,\"chapter_id\":42}");check(typed.load().chapter_id==-1&&!typed.writable);
    const auto blocked=fixture/"parent-is-file";put(blocked,"fixture");PracticeStore failure(blocked);check(!failure.save(PracticeSpec{})&&read(blocked)=="fixture");
    const auto normal=fixture/"normal"/"user"/"soew-settings.json";put(normal,"normal settings sentinel");
    PracticeStore independent(fixture/"normal");check(independent.save(PracticeSpec{}));check(read(normal)=="normal settings sentinel");
    auto ranked=PracticeSpec{};put(typed.path,"{\"version\":2}");typed.load();ranked.dynamic_rank=-6;check(typed.save(ranked));check(typed.load()==ranked);ranked.dynamic_rank=16;check(typed.save(ranked));check(typed.load().dynamic_rank==16);
    put(typed.path,"{\"version\":4,\"stage\":5,\"rank\":4,\"section\":4,\"respawn_bombs\":1,\"dialogue\":0,\"seed\":42}");
    auto extra=PracticeSpec{};extra.stage=5;extra.rank=4;extra.section=PracticeSection::StageStart;
    extra.respawn_bombs=1;extra.dialogue=PracticeDialogue::Show;extra.seed=42;
    check(typed.load()==extra&&typed.writable&&typed.warning.empty());
    check(typed.save(extra)&&typed.load()==extra&&read(typed.path).find("\"section\": 0")!=std::string::npos);
    check(read(typed.path).find("\"seed_mode\": 0")!=std::string::npos);
    put(typed.path,"{\"version\":4,\"respawn_bombs\":3,\"seed\":4294967295}");
    const auto v4=typed.load();
    check(v4.seed_mode==PracticeSeedMode::Fixed&&v4.seed==4294967295u&&typed.writable);
    check(typed.save(v4)&&typed.load()==v4&&read(typed.path).find("\"version\": 5")!=std::string::npos);
    auto random=v4;random.seed_mode=PracticeSeedMode::Random;
    check(typed.save(random)&&read(typed.path).find("\"seed_mode\": 1")!=std::string::npos);
    check(PracticeStore(fixture/"typed").load()==random);
    for(const auto bad:{"-1","2","0.5","null","true","\"1\""}){
        const auto data=std::string("{\"version\":5,\"seed\":42,\"seed_mode\":")+bad+"}";
        put(typed.path,data);const auto partial=typed.load();
        check(partial.seed_mode==PracticeSeedMode::Fixed&&partial.seed==42&&!typed.writable);
        check(!typed.save(random)&&read(typed.path)==data);
    }
    auto invalid=extra;invalid.section=static_cast<PracticeSection>(4);check(!typed.save(invalid));
    PracticeStore protect_without_load(fixture/"typed");put(protect_without_load.path,"{\"version\":99}");check(!protect_without_load.save(PracticeSpec{})&&read(protect_without_load.path)=="{\"version\":99}");
}
}
int main(int argc,char **argv){
    try{
        if(argc!=2)throw std::runtime_error("Need an explicit fresh isolated test root");
        const auto root=std::filesystem::path(argv[1]);if(!root.is_absolute())throw std::runtime_error("Isolated test root must be absolute");
        test_editor();test_model();test_font();test_store(root);
        {const auto &a=soew::stage3_practice_evidence;check(a.size()==8);check(a[0].id==0&&a[0].progress==0);check(a[1].id==3&&a[1].progress==53&&a[1].measure==17);check(a[2].id==15&&a[2].progress==103&&a[2].measure==34);check(a[7].id==55&&a[7].progress==265&&a[7].measure==88);for(std::size_t i=1;i<a.size();++i)check(a[i-1].progress<a[i].progress);check(soew::stage3_boss_entry_ids==std::array<int,6>{0x10,0x17,0x18,0x27,0x48,0x49});check(!soew::PracticeCapabilities{}.allows(2,soew::PracticeSection::Boss));}
        {soew::PracticeSpec spec;spec.stage=2;for(const auto &anchor:soew::stage3_practice_evidence){spec.section=anchor.id==0x0f?soew::PracticeSection::Midboss:soew::PracticeSection::Chapter;spec.chapter_id=anchor.id==0x0f?-1:anchor.id;check(soew::practice_music_measure(spec)==anchor.measure);}
         spec.section=soew::PracticeSection::Chapter;spec.chapter_id=48;check(soew::practice_music_measure(spec)==0);spec.stage=1;check(soew::practice_music_measure(spec)==58);}
        std::cout<<"practice_menu_checks="<<checks<<" failures=0\n";return 0;
    }catch(const std::exception &error){std::cerr<<error.what()<<'\n';return 1;}
}
