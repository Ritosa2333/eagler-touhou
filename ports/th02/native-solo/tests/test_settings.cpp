#include "settings.hpp"
#include "japanese.hpp"
#include "practice_tools_panel.hpp"
#include "../web/platform/sha256.hpp"
#include <iostream>

unsigned checks=0;
void check(bool v){++checks;if(!v)throw std::runtime_error("Settings check="+std::to_string(checks));}
void put(const std::filesystem::path&p,std::string_view s){
    std::ofstream f(p,std::ios::binary|std::ios::trunc);f<<s;if(!f)throw std::runtime_error("fixture write failed");
}

int main(int argc,char**argv){try{
    using O=Menu::Option;
    using L=soew::Language;
    if(argc!=2)throw std::runtime_error("Need isolated test directory");
    const auto hash_hex=[](std::string_view input){
        const auto digest=soew::web::sha256({reinterpret_cast<const std::uint8_t*>(input.data()),input.size()});
        std::string result;
        for(auto byte:digest){constexpr char hex[]="0123456789abcdef";result+=hex[byte>>4];result+=hex[byte&15];}
        return result;
    };
    check(hash_hex("")=="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    check(hash_hex("abc")=="ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    check(hash_hex("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq")=="248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
    auto root=std::filesystem::path(argv[1]);std::filesystem::create_directories(root);
    const auto p=root/"settings-contract.json";
    const auto missing=soew::SettingsStore(root/"missing-settings.json").load();
    check(missing.language==L::Japanese&&missing.stage_intro&&missing.smooth_background&&missing.focus_enabled);
    put(p,"{\"version\":1}");
    soew::SettingsStore a(p);check(a.load()==soew::Settings{}&&a.needs_migration);
    Menu m;m.rank=3;m.music=2;m.lives=4;m.bombs=0;m.reduced=m.autofire=true;
    const auto key=[&](unsigned input){m.update(0);m.update(input);};
    m.selected=3;key(Menu::confirm);key(Menu::tab);
    for(int i=0;i<3;++i)key(Menu::down);
    check(m.option()==O::Language&&m.language==L::Japanese);
    key(Menu::confirm);check(m.language==L::Japanese);
    key(Menu::right);check(m.option()==O::Language&&soew::Settings::from(m).language==L::English);
    key(Menu::right);check(m.language==L::ChineseSimplified);
    key(Menu::right);check(m.language==L::ChineseTraditional);
    key(Menu::right);check(m.language==L::Japanese);
    key(Menu::right);
    key(Menu::cancel);check(!m.options&&m.language==L::English);
    key(Menu::confirm);key(Menu::down);check(m.option()==O::Music);
    key(Menu::right);check(m.music==0&&soew::Settings::from(m).music==0);
    key(Menu::left);check(m.music==2&&soew::Settings::from(m).music==2);
    key(Menu::down);check(m.option()==O::MusicVolume);key(Menu::right);
    key(Menu::down);check(m.option()==O::EffectVolume);
    for(int i=0;i<6;++i)key(Menu::left);
    check(m.audio_levels.music==30&&m.audio_levels.effects==0);
    key(Menu::up);key(Menu::up);key(Menu::tab);
    check(m.option()==O::Fullscreen);key(Menu::right);check(m.fullscreen);
    key(Menu::down);check(m.option()==O::Hitboxes);key(Menu::left);check(m.hitboxes==soew::HitboxMode::All);
    using T=soew::PracticeToolsPanel;
    const T tools;
    check(tools.apply_menu_row(m,tools.row(T::first_setting))&&m.option_value(m.option())==0);
    key(Menu::left);check(m.hitboxes==soew::HitboxMode::All);
    key(Menu::down);key(Menu::down);check(m.option()==O::StageIntro&&m.stage_intro);
    key(Menu::left);check(!m.stage_intro);
    key(Menu::down);check(m.option()==O::SmoothBackground&&m.smooth_background);
    key(Menu::left);check(!m.smooth_background);
    key(Menu::down);check(m.option()==O::Focus&&m.focus_enabled);
    constexpr auto buttons=soew::Right|soew::Shot|soew::Focus;
    check(m.live_input(buttons)==buttons);
    key(Menu::left);check(!m.focus_enabled);
    check(m.live_input(buttons)==(soew::Right|soew::Shot));
    key(Menu::tab);check(m.option()==O::Autofire);
    check(tools.apply_menu_row(m,tools.row(T::first_setting+1),-1)&&m.option_value(m.option())==1);
    key(Menu::right);check(m.autofire&&m.fast_autofire);
    key(Menu::left);check(m.autofire&&!m.fast_autofire);
    check(a.save(soew::Settings::from(m)));
    soew::SettingsStore b(p);check(b.load()==soew::Settings::from(m)&&!b.needs_migration);
    Menu restored;b.saved.apply(restored);check(restored.language==L::English&&restored.autofire&&!restored.fast_autofire&&!restored.stage_intro&&!restored.smooth_background&&restored.rank==3);
    check(restored.audio_levels==m.audio_levels);
    check(!restored.focus_enabled&&restored.live_input(buttons)==(soew::Right|soew::Shot));
    key(Menu::down);check(m.option()==O::Reset);key(Menu::confirm);check(soew::Settings::from(m)==soew::Settings{});
    check(b.save(soew::Settings::from(m)));check(soew::SettingsStore(p).load()==soew::Settings{});

    {
        Menu web;
        web.akyu_available=true;
        web.adjust_option(O::Music,1);check(web.music==Menu::music_midi);
        web.adjust_option(O::Music,1);check(web.plays_akyu()&&web.replay_music()==Menu::music_fm);
        check(b.save(soew::Settings::from(web)));
        const auto saved=soew::SettingsStore(p).load();check(saved.music==Menu::music_akyu);
        Menu restored_web;restored_web.akyu_available=true;saved.apply(restored_web);check(restored_web.plays_akyu());
        Menu native;saved.apply(native);check(native.music==Menu::music_fm);
        web.adjust_option(O::Music,1);check(web.music==Menu::music_off);
        web.adjust_option(O::Music,-1);check(web.plays_akyu());
        web.cycle_preview_music();check(web.music==Menu::music_fm);
        web.cycle_preview_music();check(web.music==Menu::music_midi);
        web.cycle_preview_music();check(web.plays_akyu());
    }

    put(p,"{\"version\":1,\"rank\":99,\"music\":true,\"lives\":4,\"autofire\":true}");
    soew::SettingsStore c(p);const auto invalid=c.load();check(invalid.rank==1&&invalid.music==1&&invalid.lives==4&&invalid.autofire);
    put(p,"{\"version\":99,\"language\":\"en\"}");
    soew::SettingsStore d(p);check(d.load().language==L::Japanese&&!d.writable&&!d.save(soew::Settings::from(restored)));
    put(p,"bad json");soew::SettingsStore e(p);check(e.load()==soew::Settings{}&&!e.writable);
    const auto fail=root/"parent-is-file";put(fail,"x");soew::SettingsStore f(fail/"settings.json");check(!f.save(soew::Settings::from(restored)));
    for(const char* bad:{"\"wrong\"","[]","{}","1.5","null","-1","99"}){
        put(p,std::string("{\"version\":1,\"rank\":")+bad+",\"autofire\":true}");
        soew::SettingsStore store(p);const auto value=store.load();check(value.rank==1&&value.autofire&&store.writable);
    }
    put(p,"{\"version\":2,\"autofire\":true}");
    const auto old=soew::SettingsStore(p).load();check(old.language==L::Japanese&&old.autofire&&old.fast_autofire&&!old.fullscreen&&old.hitboxes==soew::HitboxMode::Off&&old.stage_intro&&old.smooth_background&&old.focus_enabled);
    {
        using A=soew::PracticeAdvanced;
        soew::SettingsStore store(p);auto value=store.load();
        value.practice_advanced.toggle(A::DisableBomb);value.practice_advanced.toggle(A::FastRetry);
        value.practice_advanced.free_fps=true;value.practice_advanced.speed_in_fps=true;value.practice_advanced.speed=120;
        check(store.save(value)&&soew::SettingsStore(p).load()==value);
        value.practice_advanced.free_fps=false;value.practice_advanced.speed_in_fps=false;value.practice_advanced.speed=7;
        check(store.save(value)&&soew::SettingsStore(p).load()==value);
    }
    for(const auto lang:{L::Japanese,L::English,L::ChineseSimplified,L::ChineseTraditional}){
        soew::SettingsStore store(p);auto value=store.load();value.language=lang;
        check(store.save(value));
        const auto loaded=soew::SettingsStore(p).load();check(loaded==value);
        Menu menu;loaded.apply(menu);check(menu.language==lang);
        check(soew::simulation_language(menu.language)==(lang==L::English?L::English:L::Japanese));
    }
    put(p,"{\"version\":2,\"language\":\"en\",\"practice_language\":\"zh-Hant\"}");
    soew::SettingsStore legacy_practice(p);const auto unified=legacy_practice.load();
    check(unified.language==L::English&&legacy_practice.writable&&legacy_practice.needs_migration&&legacy_practice.warning.empty());
    check(legacy_practice.save(unified));
    soew::SettingsStore migrated(p);check(migrated.load()==unified&&!migrated.needs_migration);
    for(const auto *bad:{"1","null","\"false\""}){
        put(p,std::string("{\"version\":2,\"stage_intro\":")+bad+"}");
        soew::SettingsStore store(p);check(store.load().stage_intro&&store.writable&&!store.warning.empty());
    }
    put(p,"{\"version\":2,\"language\":\"e\\u006e\"}");check(soew::SettingsStore(p).load().language==L::English);
    for(const char* bad:{"\"fr\"","\"EN\"","\"\"","0","true","null","[]","{}"}){
        put(p,std::string("{\"version\":2,\"rank\":2,\"language\":")+bad+"}");
        soew::SettingsStore store(p);const auto value=store.load();
        check(value.language==L::Japanese&&value.rank==2&&store.writable&&!store.warning.empty());
    }
    for(const char* bad:{"3","true"}){
        put(p,std::string("{\"version\":2,\"hitboxes\":")+bad+"}");check(soew::SettingsStore(p).load().hitboxes==soew::HitboxMode::Off);
    }
    for(const char* input:{"{\"version\":2,\"language\":\"en\",\"future_option\":true}",
                           "{\"version\":2,\"language\":\"en\",\"language\":\"ja\"}",
                           "{\"version\":2,\"language\":\"\\u006\"}"}){
        put(p,input);soew::SettingsStore store(p);store.load();check(!store.writable&&!store.save(soew::Settings{}));
        std::ifstream file(p,std::ios::binary);std::string contents(std::istreambuf_iterator<char>{file},{});check(contents==input);
    }
    namespace t=soew::text;
    for(const auto lang:{L::Japanese,L::English,L::ChineseSimplified,L::ChineseTraditional}){
        const auto width=soew::is_chinese(lang)?jp::cell_width:menu_text_width;
        for(const auto &row:Menu::option_rows)for(const auto option:row){
            const int count=Menu::option_count(option,true);
            const int label_width=width(t::option_labels[static_cast<unsigned>(option)](lang));
            if(!count)check(label_width<=280);
            for(int i=0;i<count;++i)check(label_width+width(t::option_value(lang,option,i))<=272);
        }
        for(const auto hint:{t::options_controls,t::options_music_hint,t::options_effects_hint})
            check(jp::cell_width(hint(lang))<=536);
        const auto warning=soew::settings_warning_text(lang,"Could not switch fullscreen mode");
        check(jp::cell_width(warning)<=536&&(lang==L::English)==(warning==L"Could not switch fullscreen mode"));
    }
    const auto menu_checks=test_menu();
    std::cout<<"settings_checks="<<checks<<" menu_checks="<<menu_checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
