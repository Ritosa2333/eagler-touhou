#include "stage_scene.hpp"
#include <iostream>
unsigned checks=0;void check(bool b){++checks;if(!b)throw std::runtime_error("Scroll/formation contract failure");}
int wmain(int argc,wchar_t**argv){try{
    if(argc!=2)throw std::runtime_error("Usage: progress-tests <assets>");
    for(int stage=0;stage<6;++stage){
        auto scene=soew::StageScene::load(argv[1],stage,0,stage==5?4:1,2,3);auto repeat=scene;std::vector<std::uint32_t> pixels(640*400);
        const unsigned expected_interval=stage==3||stage==4?1:stage==5?2:4,speed=stage==3?2:1;
        unsigned moves=0;for(unsigned tick=0;tick<1024;++tick){const auto before=scene.scroll.display_pixels;scene.tick(0);repeat.tick(0);if(tick%expected_interval==0)++moves;check(scene.scroll.display_pixels==moves*speed);check(scene.scroll.display_pixels-before==(tick%expected_interval==0?speed:0));check(scene.scroll.progress==scene.scroll.display_pixels/8);check(scene.scroll.line==int((400-scene.scroll.display_pixels%400)%400));check(scene.observed_requests==repeat.observed_requests);
            check(scene.scroll.display_pixels*4-soew::background_scroll_blend(scene.scroll)==(tick+1)*speed*4/expected_interval);
        }
        // Pixel sampling at s=24: y16 -> map row24/localy8; y24->row23/localy0.
        auto sample=soew::StageScene::load(argv[1],stage,0,1,2,3);sample.scroll.display_pixels=24;sample.render(pixels);
        for(auto y:{16u,24u}){unsigned row=(383-y+24)/16;auto section=sample.background.map.at(row/8);auto tile=sample.background.sections[(section*8+row%8)*24];auto index=sample.background.tiles[tile][((y+16-24%16)%16)*16];auto color=sample.background.palette[index];check(pixels[y*640+32]==((unsigned(color[0])<<16)|(unsigned(color[1])<<8)|color[2]));}
        // No reference beyond map, never wrap or invent continuation at endpoint.
        unsigned guard=0;while(!scene.scroll.done&&guard++<100000)scene.tick(0);check(scene.scroll.done);check(scene.scroll.display_pixels==(scene.background.map.size()*8-24)*16);scene.render(pixels);
        const auto frozen=scene.scroll.display_pixels;scene.tick(0);check(scene.scroll.display_pixels==frozen);scene.paused=true;const auto time=scene.ticks;scene.tick(soew::Right);check(scene.ticks==time);
        std::cout<<"STAGE"<<stage<<" pixels="<<frozen<<" requests_observed="<<scene.observed_requests<<" rows="<<scene.encounter_observer.cursor<<" timeline_incomplete_boundary="<<scene.encounter_observer.incomplete_boundary<<'\n';
    }
    // Equal triggers are NOT processed in a while-loop; next progress will stall
    // the duplicate row. Suppressed/over-capacity lanes still consume the row.
    soew::StageScript script;script.declared_formations=3;script.formations.resize(3);for(auto &r:script.formations)r.selectors.fill(255);script.formations[0].trigger=1;script.formations[1].trigger=1;script.formations[2].trigger=2;script.formations[0].selectors[0]=0;script.formations[0].selectors[47]=0;
    soew::FormationDispatch dispatch;unsigned callbacks=0;check(dispatch.dispatch(script,1,false,[&](auto request){check(request.x==24||request.x==400);check(request.lane==1||request.lane==48);++callbacks;return callbacks==1;}));check(dispatch.cursor==1&&dispatch.accepted==1&&dispatch.capacity_rejected==1);check(!dispatch.dispatch(script,2,false,[](auto){return true;}));check(dispatch.cursor==1);
    soew::FormationDispatch suppressed;check(suppressed.dispatch(script,1,true,[](auto){throw std::runtime_error("Suppressed spawn called");return false;}));check(suppressed.cursor==1&&suppressed.suppressed==2);
    // First 8-pixel progress is produced at tick29 and dispatched tick30
    // for speed1/interval4, not on the same update that advances the scroll.
    soew::StageScroll latency;for(unsigned i=0;i<28;++i){check(!latency.take_progress_event());latency.finish_tick(20);}check(latency.progress==0);check(!latency.take_progress_event());latency.finish_tick(20);check(latency.progress==1);check(latency.take_progress_event());check(!latency.take_progress_event());
    // Original slot policy belongs to callback: first free of25, never overwrite.
    soew::StageScript crowded;crowded.declared_formations=1;crowded.formations.resize(1);crowded.formations[0].trigger=1;crowded.formations[0].selectors.fill(0);
    std::array<bool,25> occupied{};soew::FormationDispatch pool;
    pool.dispatch(crowded,1,false,[&](auto){for(auto &slot:occupied)if(!slot){slot=true;return true;}return false;});check(pool.accepted==25&&pool.capacity_rejected==23&&pool.cursor==1);
    soew::StageScript short_script;short_script.declared_formations=1;soew::FormationDispatch incomplete;check(!incomplete.dispatch(short_script,1,false,[](auto){return true;}));check(incomplete.incomplete_boundary&&incomplete.cursor==0);
    std::cout<<"stage_progress_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
