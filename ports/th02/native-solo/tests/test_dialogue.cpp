#include "dialogue_renderer.hpp"
#include "native_replay.hpp"
#include "locale.hpp"
#include "shot_input.hpp"
#include <algorithm>
#include <iostream>
#include <tuple>
unsigned checks=0;void check(bool v){++checks;if(!v)throw std::runtime_error("dialogue contract failed="+std::to_string(checks));}

void test_autofire_isolation(){
    using namespace soew;
    using Phase=StageDialogue::Phase;
    for(const bool fast_autofire:{false,true}){
        StageDialogue d;
        d.boxes.resize(24);
        ShotInput shot;
        DialogueHold hold;
        const auto mode=[&]{return shot_input_mode(true,fast_autofire,d.active());};
        check(mode()==(fast_autofire?ShotInputMode::Fast:ShotInputMode::Autofire12Hz));
        shot.event(true,mode());
        check(shot.tick(mode()));
        d.start_preboss();
        const auto step=[&](std::uint64_t now,bool held){
            const bool z=shot.tick(mode());
            check(z==held);
            d.tick(z,held,hold.poll(now,d.active()&&held));
        };
        // An existing held key becomes a single physical hold during dialogue.
        // It cannot create a release/press pair to advance the original gate.
        while(d.phase==Phase::SlideIn)step(100,true);
        step(100,true);
        for(std::uint64_t now=117;now<399;now+=17)step(now,true);
        step(399,true);
        check(d.current==0&&d.phase==Phase::KeyDelay&&!d.key_delay.waiting_press);
        step(400,true);
        check(d.current==1);
        shot.event(false,mode());
        step(401,false);
        check(!hold.since);
    }
}

void test_mima_tap(){
    using D=soew::StageDialogue;
    using Phase=D::Phase;
    D d;
    d.boxes.resize(57);
    d.configure(4);
    d.current=d.pre_count;
    check(!d.mima_pre_final_dialogue());
    d.start_stage5_defeat(0,false);
    check(d.mima_pre_final_dialogue()&&d.current==49&&d.phase==Phase::SlideIn);
    const auto initial=d;
    soew::MimaDialogueTap tap;
    soew::NativeReplay recorded;
    recorded.config.stage=4;
    unsigned reveals=0,groups=0;
    const auto step=[&]{
        const auto input=tap.poll(d);
        reveals+=input.accelerate;
        groups+=input.group;
        recorded.events.push_back({soew::NativeReplayEventKind::Tick,
            std::uint16_t(input.accelerate?soew::Shot:0),input.group,false,soew::NativeReplayContext::Dialogue});
        d.tick(input.accelerate,input.group);
    };
    const auto advance=[&](unsigned box){
        for(unsigned i=0;i<6&&d.current==box;++i)step();
        check(d.current==box+1);
    };

    tap.event(nullptr,true); // Hold from combat, including repeated key-downs.
    while(d.phase==Phase::SlideIn){tap.event(&d,true,true);step();}
    for(unsigned i=0;i<60;++i){tap.event(&d,true,true);step();}
    check(reveals==0&&groups==0&&d.current==49&&d.phase==Phase::Text);
    tap.event(&d,false);
    tap.event(&d,true,true); // A repeat alone does not create an eligible edge.
    step();
    check(reveals==0);
    tap.event(&d,false);
    tap.event(&d,true);
    tap.event(&d,false); // The entire tap can happen between logical samples.
    tap.event(&d,true);
    tap.event(&d,false); // A second tap on the same box does not carry forward.
    step();
    check(reveals==1&&groups==0&&d.phase==Phase::KeyDelay);
    advance(49);
    check(reveals==1&&groups==1);
    for(unsigned i=0;i<6;++i)step();
    check(d.current==50&&d.phase==Phase::Text&&reveals==1&&groups==1);

    tap.event(&d,true);
    advance(50);
    for(unsigned i=0;i<6;++i){tap.event(&d,true,true);step();}
    check(d.current==51&&d.phase==Phase::Text&&reveals==2&&groups==2);
    tap.event(&d,false);
    for(unsigned i=0;i<120&&d.phase==Phase::Text;++i)step();
    check(d.phase==Phase::KeyDelay&&d.current==51&&groups==2);
    tap.event(&d,true); // Already fully revealed.
    tap.event(&d,false);
    advance(51);
    check(reveals==2&&groups==3);

    tap.event(&d,true);
    step(); // Reveal, then lose focus before the confirmation pulse.
    tap.reset(true);
    for(unsigned i=0;i<8;++i){tap.event(&d,true,true);step();}
    check(d.current==52&&d.phase==Phase::KeyDelay&&reveals==3&&groups==3);
    tap.event(&d,false);
    tap.event(&d,true);
    tap.event(&d,false);
    advance(52);
    check(d.phase==Phase::Complete&&!d.mima_pre_final_dialogue()&&reveals==3&&groups==4);

    recorded.summary.emplace();
    auto &summary=*recorded.summary;
    summary.final_stage=4;
    summary.ticks=static_cast<std::uint32_t>(recorded.events.size());
    summary.stage_scores[4]=summary.final_score;
    summary.stage_checks[4]=soew::NativeReplayBoundary{summary.ticks,summary.state_hash};
    using namespace soew::native_replay_detail;
    const auto decoded=decode(encode(recorded));
    check(decoded==recorded);
    auto replay=initial;
    unsigned confirmations=0;
    for(const auto &event:decoded.events){
        const auto box=replay.current;
        replay.tick(event.input!=0,event.dialogue_group,event.dialogue_fast_forward);
        check((replay.current==box+1)==event.dialogue_group);
        confirmations+=event.dialogue_group;
    }
    check(replay.current==d.current&&replay.phase==d.phase&&replay.updates==d.updates&&confirmations==4);
    d.start_stage5_defeat(0,true);
    check(!d.mima_pre_final_dialogue());

    d.phase=Phase::Complete;
    d.current=d.pre_count;
    d.start_stage5_defeat(1,false);
    check(!d.mima_pre_final_dialogue());
    d.phase=Phase::Complete;
    d.current=d.pre_count;
    d.start_stage5_defeat(0,false);
    tap.event(&d,true);
    tap.event(&d,false); // A tap during SlideIn must not queue.
    while(d.phase==Phase::SlideIn)d.tick(false,false);
    check(!tap.poll(d).accelerate);
    tap.event(&d,true);
    tap.event(&d,false);
    tap.clear(); // Pause / scene reset drops even fully released taps.
    check(!tap.poll(d).accelerate);
    tap.event(&d,true);
    tap.event(&d,false);
    auto replacement=d;
    check(!tap.poll(replacement).accelerate); // Same box in a different dialogue instance.
}

void test_localized_dialogue(const std::filesystem::path& root){
    using namespace soew;
    using D=StageDialogue;
    using Phase=D::Phase;
    validate_locale_assets(root);
    check(has_locale_assets(root));
    for(unsigned stage=0;stage<6;++stage){
        const auto name="STAGE"+std::to_string(stage)+".TXT";
        const auto read=[&](Language lang){
            auto d=D::decode(StageScene::read(locale_asset(root,lang,name)));d.configure(stage);return d;
        };
        for(unsigned used:{0u,1u,12u}){
            auto ja=read(Language::Japanese),en=read(Language::English);
            check(ja.boxes.size()==en.boxes.size()&&ja.boxes.front().lines[0].text!=en.boxes.front().lines[0].text);
            ja.continues_used=en.continues_used=used;
            const auto advance=[&]{
                unsigned steps=0;
                for(;ja.active()&&steps<10000;++steps){
                    check(en.active()&&ja.phase==en.phase&&ja.current==en.current&&ja.reveal.cursor==en.reveal.cursor);
                    if(ja.current<ja.end_box)check(ja.face()==en.face());
                    if(ja.phase==Phase::IntroAppearance){ja.continue_after_appearance();en.continue_after_appearance();}
                    const bool press=ja.phase==Phase::KeyDelay&&ja.key_delay.waiting_press;
                    check(ja.tick(false,press)==en.tick(false,press));
                }
                check(!ja.active()&&!en.active()&&ja.updates==en.updates&&steps<10000);
            };
            ja.start_preboss();en.start_preboss();advance();
            if(stage==4){
                ja.start_stage5_defeat(used,false);en.start_stage5_defeat(used,false);advance();
                if(!used){ja.start_stage5_defeat(used,true);en.start_stage5_defeat(used,true);advance();}
            }else{
                ja.start_postboss();en.start_postboss();
                if(stage==3&&used){
                    for(const auto* d:{&ja,&en}){
                        const auto& row=d->boxes[42].lines[0].text;
                        check(row[8]==std::byte(used==12?0x82:0x81));
                        check(row[9]==std::byte(used==12?0x50:0x40));
                        check(row[10]==std::byte{0x82}&&row[11]==std::byte(0x4f+used%10));
                    }
                }
                advance();
            }
        }
    }
}

int main(int argc,char**argv){try{if(argc<2)throw std::runtime_error("need resource root");if(argc==3)test_localized_dialogue(argv[2]);const auto raw=soew::StageScene::read(std::filesystem::path(argv[1])/"STAGE0.TXT");auto d=soew::StageDialogue::decode(raw);check(d.boxes.size()>=24);for(unsigned b=0;b<d.boxes.size();++b)for(unsigned line=0;line<2;++line){for(unsigned i=0;i<36;++i)check(d.boxes[b].lines[line].text[i]==raw[b*80+line*40+i]);for(unsigned i=0;i<4;++i)check(d.boxes[b].lines[line].padding[i]==raw[b*80+line*40+36+i]);}
 for(unsigned box=0;box<24;++box){check(d.current==box&&d.reveal.cursor==6&&d.reveal.bytes(0)==6&&d.reveal.bytes(1)==0);unsigned ticks=0;while(!d.reveal.waiting){d.reveal.tick(false);++ticks;}check(ticks==114&&d.reveal.bytes(0)==36&&d.reveal.bytes(1)==36);d.advance_after_key_delay();if(box==15)check(d.preboss_complete());}check(d.postboss_complete());
 {soew::DialogueHold hold;check(!hold.poll(100,true));check(!hold.poll(399,true));check(hold.poll(400,true));
  check(!hold.poll(401,false));check(!hold.poll(500,true));check(!hold.poll(501,false));
  check(!hold.poll(800,true));check(!hold.poll(1099,true));check(hold.poll(1100,true));hold.clear();check(!hold.poll(1101,true));}
 test_autofire_isolation();
 test_mima_tap();
 auto raw3=soew::StageScene::read(std::filesystem::path(argv[1])/"STAGE2.TXT");
 check(raw3.size()==881&&raw3.back()==std::byte{0x20});
 auto third=soew::StageDialogue::decode(raw3);third.configure(2);
 check(third.trailing_space&&third.boxes.size()==11&&third.pre_count==8&&third.total_count==11);
 constexpr std::array<unsigned,11> faces={6,96,96,12,96,51,96,0,12,96,12};
 const auto play=[&](unsigned first,unsigned end){
     for(unsigned frame=0;third.active()&&frame<1000;++frame){
         if(third.current<end){check(third.current>=first&&third.face()==faces[third.current]);}
         check(third.phase!=soew::StageDialogue::Phase::IntroAppearance);
         const bool press=third.phase==soew::StageDialogue::Phase::KeyDelay&&third.key_delay.waiting_press;
         third.tick(true,press);
     }
     check(!third.active()&&third.current==end);
 };
 third.start_preboss();play(0,8);check(third.preboss_complete());
 third.start_postboss();play(8,11);check(third.postboss_complete());
 raw3.back()=std::byte{0};bool bad3=false;try{soew::StageDialogue::decode(raw3);}catch(const std::runtime_error&){bad3=true;}check(bad3);
 auto raw4=soew::StageScene::read(std::filesystem::path(argv[1])/"STAGE3.TXT");
 check(raw4.size()==4161&&raw4.back()==std::byte{0x20});
 auto fourth=soew::StageDialogue::decode(raw4);fourth.configure(3);
 check(fourth.trailing_space&&fourth.boxes.size()==52&&fourth.pre_count==35);
 using Phase=soew::StageDialogue::Phase;
 std::vector<unsigned> pauses,visited;
 const auto run4=[&]{
     for(unsigned frame=0;fourth.active()&&frame<1000;++frame){
         if(fourth.phase==Phase::IntroAppearance){
             pauses.push_back(fourth.current);
             fourth.tick(true,true);check(fourth.phase==Phase::IntroAppearance);
             fourth.continue_after_appearance();
         }
         if(fourth.phase==Phase::Text)visited.push_back(fourth.current);
         const bool press=fourth.phase==Phase::KeyDelay&&fourth.key_delay.waiting_press;
         fourth.tick(true,press);
     }
     check(!fourth.active());
 };
 fourth.start_preboss();run4();check(fourth.preboss_complete());
 check(pauses==std::vector<unsigned>{8,24}&&visited.size()==35);
 for(unsigned i=0;i<35;++i)check(visited[i]==i);
 visited.clear();fourth.start_postboss();run4();
 check(fourth.postboss_complete()&&visited==std::vector<unsigned>{35,36,37,38,47,48,49,50,51});
 for(const auto [box,face]:std::array<std::array<unsigned,2>,7>{{{0,57},{8,105},{25,144},{34,96},{35,147},{47,96},{51,57}}}){fourth.current=box;check(fourth.face()==face);}
 fourth.current=35;fourth.continues_used=2;visited.clear();fourth.start_postboss();run4();
 check(fourth.postboss_complete()&&visited==std::vector<unsigned>{35,36,37,38,39,40,41,42,43,44,45,46});
 check(fourth.boxes[42].lines[0].text[10]==std::byte{0x82}&&fourth.boxes[42].lines[0].text[11]==std::byte{0x51});
 raw4.back()=std::byte{0};bool bad4=false;try{soew::StageDialogue::decode(raw4);}catch(const std::runtime_error&){bad4=true;}check(bad4);
 using D=soew::StageDialogue;
 check(D::appearance_white_mix(1,2,16,false)==255&&D::appearance_white_mix(1,2,33,false)==0);
 for(unsigned f=0;f<191;++f){check(D::appearance_white_mix(3,8,f,false)==0);check(D::appearance_white_mix(3,24,f,true)<=48);}
 check(D::appearance_white_mix(3,24,5,false)==192&&D::appearance_white_mix(3,24,10,false)==0);
 for(unsigned used:{0u,1u}){
     auto fifth=D::decode(soew::StageScene::read(std::filesystem::path(argv[1])/"STAGE4.TXT"));fifth.configure(4);
     check(fifth.trailing_space&&fifth.boxes.size()==57);fifth.continues_used=used;
     const auto run5=[&]{
         std::vector<unsigned> shown;
         for(unsigned f=0;f<2000&&fifth.active();++f){
             if(fifth.phase==Phase::IntroAppearance)fifth.continue_after_appearance();
             if(fifth.phase==Phase::Text)shown.push_back(fifth.current);
             fifth.tick(true,fifth.phase==Phase::KeyDelay&&fifth.key_delay.waiting_press);
         }
         check(!fifth.active());return shown;
     };
     fifth.start_preboss();auto shown=run5();
     std::vector<unsigned> expected={0,1,2,3};for(unsigned i=used?4:26;i<(used?26:43);++i)expected.push_back(i);expected.push_back(43);
     check(shown==expected&&fifth.preboss_complete());
     fifth.start_stage5_defeat(used,false);shown=run5();
     check(shown==(used?std::vector<unsigned>{44,45,46,47,48}:std::vector<unsigned>{49,50,51,52}));
     if(!used){fifth.start_stage5_defeat(1,true);check(!fifth.show_portrait());check(run5()==std::vector<unsigned>{53,54,55,56});}
 }
 const auto raw_extra=soew::StageScene::read(std::filesystem::path(argv[1])/"STAGE5.TXT");
 auto extra=D::decode(raw_extra);extra.configure(5);
 check(raw_extra.size()==2480&&!extra.trailing_space&&extra.pre_count==25&&extra.total_count==31);
 for(unsigned b=0;b<31;++b)for(unsigned l=0;l<2;++l){
     for(unsigned i=0;i<36;++i)check(extra.boxes[b].lines[l].text[i]==raw_extra[b*80+l*40+i]);
     for(unsigned i=0;i<4;++i)check(extra.boxes[b].lines[l].padding[i]==raw_extra[b*80+l*40+36+i]);
 }
 constexpr std::array<unsigned,31> ex_faces={255,0,255,153,0,153,3,153,6,153,153,153,57,153,57,153,57,153,153,57,153,12,96,12,153,9,156,9,96,96,12};
 auto oversize=raw_extra;oversize.push_back(std::byte{0x20});
 bool bad_extra=false;try{D::decode(oversize);}catch(const std::runtime_error&){bad_extra=true;}check(bad_extra);
 bad_extra=false;try{auto short_dialogue=D::decode(std::span<const std::byte>(raw_extra).first(30*80));short_dialogue.configure(5);}catch(const std::runtime_error&){bad_extra=true;}check(bad_extra);
 const auto run_extra=[&](unsigned begin,unsigned end){
     unsigned shown=begin,appearances=0;
     for(unsigned frame=0;extra.active()&&frame<1000;++frame){
         if(extra.phase==Phase::IntroAppearance){
             ++appearances;check(begin==0&&extra.current==D::extra_intro_end);
             extra.tick(true,true);check(extra.phase==Phase::IntroAppearance);
             extra.continue_after_appearance();
         }
         if(extra.phase==Phase::Text){check(shown<end&&extra.current==shown&&extra.face()==ex_faces[shown]);++shown;}
         extra.tick(true,extra.phase==Phase::KeyDelay&&extra.key_delay.waiting_press);
     }
     check(!extra.active()&&shown==end&&appearances==(begin==0?1u:0u));
 };
 extra.continues_used=2; // Extra's script does not take regular-stage Continue branches.
 extra.start_preboss();run_extra(0,25);check(extra.preboss_complete());
 extra.start_postboss();run_extra(25,31);check(extra.postboss_complete());
 check(D::extra_appearance_peak==18&&D::extra_appearance_end==36);
 check(D::appearance_white_mix(5,3,0,false)==0&&D::appearance_white_mix(5,3,18,false)==255);
 check(D::appearance_white_mix(5,3,17,false)==244&&D::appearance_white_mix(5,3,20,false)==239);
 check(D::appearance_white_mix(5,3,36,false)==0&&D::appearance_white_mix(5,4,18,false)==0);
 for(unsigned i=0;i<=D::extra_appearance_end;++i)check(D::appearance_white_mix(5,3,i,true)<=64);
 // Use a non-black index 0 and nonempty portrait tiles so the blank slot can
 // only pass by explicitly clearing the 48x48 rectangle with palette index 0.
 soew::DialogueRenderer portrait;portrait.tiles.resize(256);
 for(auto &tile:portrait.tiles)tile.fill(1);
 soew::StageBackground bg;bg.palette[0]={0x11,0x22,0x33};bg.palette[1]={0xaa,0xbb,0xcc};
 soew::StageSprites bank;for(unsigned id:{34,35,36,37,38,42,43,44,45,46})bank.sprites[id]=soew::StageSprites::Sprite{};
 std::vector<std::uint32_t> pixels(640*400,0xffffff);
 extra.start_preboss();extra.box_left=32;
 for(const auto [box,phase,color]:std::array<std::tuple<unsigned,Phase,std::uint32_t>,4>{{
     {1,Phase::Text,0xaabbcc},{0,Phase::Text,0x112233},{2,Phase::KeyDelay,0x112233},{3,Phase::IntroAppearance,0x112233}}}){
     extra.current=box;extra.phase=phase;std::fill(pixels.begin(),pixels.end(),0xffffff);
     portrait.render(pixels,bg,bank,extra);
     for(unsigned y=328;y<376;++y)for(unsigned x=40;x<88;++x)check(pixels[y*640+x]==color);
     check(pixels[327*640+40]==0xffffff&&pixels[328*640+39]==0xffffff&&pixels[375*640+88]==0xffffff&&pixels[376*640+87]==0xffffff);
 }
 soew::StageDialogue::Reveal fast;unsigned ticks=0;while(!fast.waiting){fast.tick(true);++ticks;}check(ticks==38);bool bad=false;try{soew::StageDialogue::decode(std::span<const std::byte>(raw).first(79));}catch(const std::runtime_error&){bad=true;}check(bad);std::cout<<"dialogue_checks="<<checks<<" boxes="<<d.boxes.size()<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
