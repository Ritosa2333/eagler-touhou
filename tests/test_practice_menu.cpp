#include "practice_targets.hpp"
#include "practice_panel.hpp"
#include "practice_assist.hpp"
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
void test_panel(){
    using namespace soew;
    using M=PracticeMenu;using P=PracticePanel::Page;using F=PracticePanel::Field;using A=M::Action;
    M menu;menu.capabilities=practice_capabilities();menu.spec.rank=2;
    Menu native;
    PracticePanel panel;
    const auto tap=[&](unsigned key){panel.update(menu,0);return panel.update(menu,key);};
    const auto native_tap=[&](unsigned key){native.update(0);return native.update(key);};
    const auto has=[&](F field){const auto fields=panel.fields(menu);return std::find(fields.begin(),fields.end(),field)!=fields.end();};
    const auto initial=menu.spec;
    native_tap(Menu::confirm);native_tap(Menu::confirm);
    check(native.shot_screen&&native.shot_confirmed&&!panel.open_from_shot(menu,native));
    native_tap(Menu::cancel);native.selected=Menu::practice_entry;
    check(native_tap(Menu::confirm)==Menu::practice_entry);
    native.begin_practice_shot_select(initial.shot);
    check(native.practice_shot_select()&&!native.extra_mode&&native.shot==initial.shot&&!panel.open_from_shot(menu,native));
    native_tap(Menu::right);check(native.shot==2&&menu.spec==initial);
    native_tap(Menu::confirm);
    check(panel.open_from_shot(menu,native)&&panel.page==P::Stages&&panel.shot==2&&menu.spec==initial);
    check(!native.shot_screen&&!native.shot_confirmed&&panel.update(menu,M::confirm)==A::None&&panel.page==P::Stages);
    tap(M::up);tap(M::up);check(panel.stage_cursor==practice_extra_stage&&menu.spec==initial);
    check(tap(M::cancel)==A::Back&&menu.spec==initial);
    native.begin_practice_shot_select(panel.shot);
    native_tap(Menu::cancel);
    check(!native.shot_screen&&native.selected==Menu::practice_entry);
    native.begin_practice_shot_select(panel.shot);native_tap(Menu::confirm);check(panel.open_from_shot(menu,native));
    tap(M::up);tap(M::up);tap(M::confirm);
    check(panel.page==P::Setup&&panel.focused(F::Start)&&menu.spec.shot==2&&menu.spec.stage==practice_extra_stage&&menu.spec.rank==4&&!has(F::Rank));
    check(has(F::Power)&&has(F::Lives)&&has(F::Bombs)&&has(F::SeedMode)&&has(F::Seed));
    tap(M::cancel);tap(M::up);tap(M::confirm);
    check(panel.page==P::Setup&&menu.spec.stage==4&&menu.spec.rank==2&&has(F::Rank)&&has(F::Phase)&&!has(F::Chapter));
    tap(M::down);check(panel.focused(F::Section));
    tap(M::right);tap(M::right);check(menu.spec.section==PracticeSection::Chapter&&has(F::Chapter)&&!has(F::Phase));
    tap(M::down);check(panel.focused(F::Chapter));
    const int first=menu.spec.chapter_id;
    tap(M::right);check(menu.spec.chapter_id!=first);
    tap(M::up);tap(M::left);tap(M::left);tap(M::down);check(panel.focused(F::Phase));
    tap(M::right);check(menu.spec.boss_phase==1);
    tap(M::down);tap(M::down);
    check(panel.page==P::Setup&&panel.focused(F::Power)&&!has(F::Dialogue));
    check(tap(M::confirm)==A::EditNumber&&menu.number_editing());
    check(menu.number_paste("42"));tap(M::confirm);
    check(menu.spec.power==42&&!menu.number_editing()&&panel.focused(F::Power));
    for(int i=0;i<5;++i)tap(M::down);
    check(panel.focused(F::SeedMode)&&has(F::Seed)&&has(F::NewSeed));
    tap(M::confirm);check(menu.spec.seed_mode==PracticeSeedMode::Random&&!has(F::Seed)&&!has(F::NewSeed));
    tap(M::down);check(panel.page==P::Setup&&panel.focused(F::Start));
    const auto recipe=menu.spec;
    check(tap(M::confirm)==A::Start&&menu.spec==recipe);
}
void test_panel_layout(){
    using namespace soew;
    using P=PracticePanel::Page;
    PracticeMenu menu;menu.capabilities=practice_capabilities();PracticePanel panel;
    std::vector<std::uint32_t> pixels(640*400);
    for(const auto lang:{Language::English,Language::Japanese,Language::ChineseSimplified,Language::ChineseTraditional}){
        for(const auto page:{P::Stages,P::Setup}){
            std::fill(pixels.begin(),pixels.end(),0xdec0a6);
            panel.open(menu,page);
            using F=PracticePanel::Field;
            const bool stages=page==P::Stages;
            const int left=stages?280:232,top=stages?140:44,bottom=stages?316:352;
            constexpr std::uint32_t selected=0xff88ff,unselected=0xaaaaaa;
            int stage_rows=0,setup_rows=0,boxes=0;
            const auto fields=panel.fields(menu);
            draw_practice_panel(pixels,panel,menu,lang,false,selected,unselected,
                [&](int l,int t,int r,int b,std::uint32_t color){
                    ++boxes;check(l==left&&t==top&&r==592&&b==bottom&&color==selected);
                },[&](std::wstring_view label,int x,int y,int r,int h,std::uint32_t color,PracticeText style,bool){
                check(!label.empty()&&x>=left+16&&x<r&&r<592&&y>top&&y+h<bottom);
                if(page==P::Stages){
                    check(style==PracticeText::Stage&&x==296&&y==160+24*stage_rows&&h==16);
                    check(label==practice_stage_name(lang,stage_rows));
                    check(color==(stage_rows==menu.spec.stage?selected:unselected));
                    ++stage_rows;
                }else if(x==248&&style==PracticeText::Body){
                    check(y==80+19*setup_rows&&label==practice_panel_labels[unsigned(fields[setup_rows])](lang));
                    check(color==(fields[setup_rows]==F::Start?selected:unselected));
                    ++setup_rows;
                }
            });
            check(boxes==1&&stage_rows==(stages?practice_stage_count:0)&&setup_rows==(stages?0:int(fields.size())));
            check(pixels[top*640+left-1]==0x6f6053u&&pixels[(bottom+9)*640+500]==0x6f6053u);
        }
    }
}
void test_floating_panels(){
    using namespace soew;
    using P=PracticePanel;
    using M=PracticeMenu;
    using L=Language;
    M menu;menu.capabilities=practice_capabilities();
    P panel;panel.open(menu);
    PracticeDrag drag;
    const PracticePoint grab{316,152};
    check(panel.in_header(grab)&&!panel.in_header({316,160}));
    drag.begin(panel.window,panel.base(),grab,P::shadow);
    drag.move({116,52});
    const auto moved=panel.window.bounds(panel.base());
    check(moved.left==80&&moved.top==40&&moved.right==392&&moved.bottom==216);
    check(panel.in_header({116,52})&&!panel.in_header(grab));
    drag.end();check(!drag.window);
    check(!panel.point(menu,{296,284}));
    const auto picked=panel.point(menu,{96,111});
    check(picked==M::confirm&&panel.stage_cursor==2);
    panel.update(menu,0);panel.update(menu,picked);
    check(panel.page==P::Page::Setup&&menu.spec.stage==2);
    const auto setup=panel.window.bounds(panel.base());
    check(setup.left==32&&setup.top==0);
    const auto start=PracticePoint{setup.left+16,setup.top+P::setup_y-P::setup_box.top+
        (int(P::fields(menu).size())-1)*P::setup_pitch};
    const auto begin=panel.point(menu,start);panel.update(menu,0);
    check(begin==M::confirm&&panel.update(menu,begin)==M::Action::Start);
    const auto fields=P::fields(menu);
    const int power_y=P::setup_y+panel.window.offset.y+
        int(std::find(fields.begin(),fields.end(),P::Field::Power)-fields.begin())*P::setup_pitch;
    check(!panel.point(menu,{setup.left+16,power_y})&&panel.focused(P::Field::Power));
    const auto edit=panel.point(menu,{setup.right-24,power_y});panel.update(menu,0);
    check(edit==M::confirm&&panel.update(menu,edit)==M::Action::EditNumber&&menu.number_editing());
    check(!panel.point(menu,start)&&panel.focused(P::Field::Power));menu.number_cancel();

    drag.begin(panel.window,panel.base(),{setup.left+16,setup.top+14},P::shadow);
    drag.move({2000,2000});drag.end();
    const auto edge=panel.window.bounds(panel.base());
    check(edge.right+P::shadow==640&&edge.bottom+P::shadow==400);
    std::vector<std::uint32_t> pixels(640*400,0);
    constexpr std::uint32_t active=0xff88ff;
    int boxes=0,labels=0;
    draw_practice_panel(pixels,panel,menu,L::ChineseTraditional,false,active,0xaaaaaa,
        [&](int l,int t,int r,int b,std::uint32_t){++boxes;check(l==edge.left&&t==edge.top&&r==edge.right&&b==edge.bottom);},
        [&](std::wstring_view label,int x,int y,int r,int h,std::uint32_t,PracticeText style,bool){
            check(x>=edge.left&&y>=edge.top&&r<=edge.right&&y+h<=edge.bottom);
            if(style==PracticeText::Stage){++labels;check(label==L"第3關");}
        });
    check(boxes==1&&labels==1);
    check(practice_stage_name(L::Japanese,0)==L"ステージ 1"&&practice_stage_name(L::ChineseSimplified,0)==L"第1关"&&practice_stage_name(L::English,5)==L"EXTRA");

    const PracticeBounds view{0,50,800,550};
    check(!practice_pointer(view,{400,49},false)&&!practice_pointer(view,{800,300},false));
    const auto center=practice_pointer(view,{400,300},false),outside=practice_pointer(view,{1000,-50},true);
    check(center&&center->x==320&&center->y==200&&outside&&outside->x==800&&outside->y==-80);
    const auto crt=practice_pointer({125,0,925,600},{525,300},false);
    check(crt&&crt->x==320&&crt->y==200);

    PracticeAssist assist;using A=PracticeAssist;
    drag.begin(assist.window,A::base,{50,18});drag.move({90,108});drag.end();
    check(assist.in_header({90,108})&&assist.row_at({90,108})<0);
    const int assist_row=assist.row_at({90,A::top+90+6*A::pitch});
    check(assist_row==6&&assist.row_at({50,18})<0);
    assist.toggle_row(assist_row);check(assist.enabled(PracticeTool::AutoBomb));

    PracticeLanguages language;Menu native;native.set_language_preference(L::English);
    check(!language.toggle(false,false,std::nullopt)&&!language.toggle(true,true,std::nullopt));
    check(language.toggle(true,false,std::nullopt)&&language.open&&language.focus==0);
    language.navigate(-1);check(language.focus==4);
    drag.begin(language.window,language.base,{56,240},language.shadow);drag.move({256,40});drag.end();
    const int row=language.row_at({256,language.rows_y-200+3*language.pitch});
    check(language.in_header({256,40})&&row==3);
    native.practice_language_preference=language.choices[row];
    check(native.practice_language()==L::ChineseSimplified&&native.language==L::English&&!native.chinese_available);
    check(!language.toggle(true,true,native.practice_language_preference)&&language.open);
    check(language.toggle(true,false,native.practice_language_preference)&&!language.open);
    check(language.toggle(true,false,native.practice_language_preference)&&language.focus==3);
    native.practice_language_preference=language.choices[0];check(native.practice_language()==native.language);
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
        test_editor();test_model();test_panel();test_panel_layout();test_floating_panels();test_store(root);
        {const auto &a=soew::stage3_practice_evidence;check(a.size()==8);check(a[0].id==0&&a[0].progress==0);check(a[1].id==3&&a[1].progress==53&&a[1].measure==17);check(a[2].id==15&&a[2].progress==103&&a[2].measure==34);check(a[7].id==55&&a[7].progress==265&&a[7].measure==88);for(std::size_t i=1;i<a.size();++i)check(a[i-1].progress<a[i].progress);check(soew::stage3_boss_entry_ids==std::array<int,6>{0x10,0x17,0x18,0x27,0x48,0x49});check(!soew::PracticeCapabilities{}.allows(2,soew::PracticeSection::Boss));}
        {soew::PracticeSpec spec;spec.stage=2;for(const auto &anchor:soew::stage3_practice_evidence){spec.section=anchor.id==0x0f?soew::PracticeSection::Midboss:soew::PracticeSection::Chapter;spec.chapter_id=anchor.id==0x0f?-1:anchor.id;check(soew::practice_music_measure(spec)==anchor.measure);}
         spec.section=soew::PracticeSection::Chapter;spec.chapter_id=48;check(soew::practice_music_measure(spec)==0);spec.stage=1;check(soew::practice_music_measure(spec)==58);}
        std::cout<<"practice_menu_checks="<<checks<<" failures=0\n";return 0;
    }catch(const std::exception &error){std::cerr<<error.what()<<'\n';return 1;}
}
