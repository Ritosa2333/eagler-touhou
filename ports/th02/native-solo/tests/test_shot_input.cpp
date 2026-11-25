#include "shot_input.hpp"
#include "player_shots.hpp"
#include "sound_effects.hpp"
#include <algorithm>
#include <iostream>
#include <unordered_map>
unsigned checks=0;void check(bool v){++checks;if(!v)throw std::runtime_error("Shot input check="+std::to_string(checks));}

// Original TH02 loc_F336..loc_F426, abstracting away projectile allocation.
// Dead cooldowns are overwritten on rearm. B states 2 and 4 are equivalent.
struct SourceGate {
    unsigned a=255,b=3,ca=0,cb=0;
    unsigned pack()const{return (a<<12)|(b<<8)|(ca<<4)|cb;}
    static SourceGate unpack(unsigned key){return {key>>12,(key>>8)&15,(key>>4)&15,key&15};}
    unsigned sample(bool z){
        if(z){if(a==255){a=0;ca=0;}else if(b==3){b=0;cb=0;}}
        else if(a>=3){a=255;b=2;}else if(b>=2)b=3;
        unsigned sent=0;
        if(a!=255&&!ca){ca=8;a=(a+1)&255;sent=1;}
        else {if(a!=255)--ca;if(b<2){if(!cb){cb=8;++b;sent=1;}else --cb;}}
        if(a==255)ca=0;
        if(b>=2)cb=0;
        return sent;
    }
};

int main(){try{
 using enum soew::ShotInputMode;
 {
    soew::ShotSoundCadence cadence;
    check(cadence.requests(2,false)==2&&cadence.requests(0,true)==0);
    cadence.set_autofire(true);
    for(unsigned volley=0;volley<7;++volley){
        check(cadence.requests(1,false)==0);
        cadence.set_autofire(true); // The host sets the mode every tick.
        check(cadence.requests(volley%2,true)==(volley%3==0));
    }
    cadence.reset();check(cadence.requests(0,true)==1);
    cadence.set_autofire(false);check(cadence.requests(1,false)==1);
    cadence.set_autofire(true);check(cadence.requests(0,true)==1);
 }
 {soew::ShotInput s;s.event(true,false);s.event(false,false);check(s.tick(false));check(!s.tick(false));check(!s.tick(false));}
 for(auto mode:{Autofire12Hz,Fast}){soew::ShotInput s;s.event(true,mode);unsigned rises=0,falls=0;bool prior=false;for(unsigned i=0;i<500;++i){bool v=s.tick(mode);if(v&&!prior)++rises;if(!v&&prior)++falls;prior=v;}const unsigned expected=mode==Fast?125u:102u;check(rises==expected&&falls==expected);s.event(false,mode);for(unsigned i=0;i<30;++i)check(!s.tick(mode));s.event(true,mode);check(s.tick(mode));s.clear();for(unsigned i=0;i<30;++i)check(!s.tick(mode));s.event(true,mode);check(s.tick(mode));s.event(false,mode);s.event(true,mode);check(s.tick(mode));}
 // At Lv0 each invocation allocates one projectile; count volleys over 30.6 s
 // of base ticks. The old Auto waveform remains useful as a replay baseline.
 for(unsigned type=0;type<3;++type){
    soew::ShotInput input,legacy,manual;input.event(true,Fast);legacy.event(true,true);manual.event(true,false);
    soew::PlayerShots held,old_auto,new_auto,manual_pulse;
    for(auto* s:{&held,&old_auto,&new_auto,&manual_pulse})s->shot_type=type;
    for(unsigned f=0;f<1800;++f){
        const unsigned page=f&1;
        const auto tick=[&](soew::PlayerShots &s,bool z){s.begin_frame(page);s.input(z,page,208,336);s.update(page,f);};
        const bool key=input.tick(Fast),expected=f%4<2,legacy_key=legacy.tick(true);
        check(key==expected&&legacy_key==(f*408u)%2000u<1000u);
        tick(held,manual.tick(false));tick(old_auto,legacy_key);tick(new_auto,key);tick(manual_pulse,expected);
        check(new_auto.fired==manual_pulse.fired&&new_auto.burst_a==manual_pulse.burst_a&&new_auto.burst_b==manual_pulse.burst_b&&new_auto.cool_a==manual_pulse.cool_a&&new_auto.cool_b==manual_pulse.cool_b);
    }
    check(held.fired==202&&old_auto.fired==451&&new_auto.fired==540);
    input.event(false,Fast);for(unsigned f=0;f<60;++f)check(!input.tick(Fast));
    std::cout<<"shot="<<type<<" hold="<<held.fired<<" old_auto="<<old_auto.fired<<" new_auto="<<new_auto.fired<<'\n';
 }
 // Take both next key states from every reachable gate state. Keep the
 // largest number of fire calls per state, since future calls depend only on it.
 struct Edge{unsigned next,sent;};
 std::vector<unsigned> keys{SourceGate{}.pack()};
 std::unordered_map<unsigned,unsigned> indices{{keys[0],0}};
 std::vector<std::array<Edge,2>> graph;
 for(unsigned i=0;i<keys.size();++i){
    std::array<Edge,2> outgoing{};
    for(bool z:{false,true}){
        auto gate=SourceGate::unpack(keys[i]);
        soew::PlayerShots port;port.burst_a=gate.a;port.burst_b=gate.b==3?255:gate.b;port.cool_a=gate.ca;port.cool_b=gate.cb;
        port.input(z,0,208,336);
        const auto sent=gate.sample(z);
        const auto port_b=port.burst_b==255?3u:std::min<unsigned>(port.burst_b,2);
        check(port.fired==sent&&gate.a==port.burst_a&&gate.b==port_b&&(gate.a==255||gate.ca==port.cool_a)&&(gate.b>=2||gate.cb==port.cool_b));
        const auto [where,added]=indices.try_emplace(gate.pack(),keys.size());
        if(added)keys.push_back(gate.pack());
        outgoing[z]={where->second,sent};
    }
    graph.push_back(outgoing);
 }
 std::vector<int> best(graph.size(),-1801);best[0]=0;
 for(unsigned f=1;f<=1800;++f){
    std::vector<int> next(graph.size(),-1801);
    for(unsigned i=0;i<graph.size();++i)if(best[i]>=0)for(const auto e:graph[i])next[e.next]=std::max(next[e.next],best[i]+int(e.sent));
    best=std::move(next);
    if(f==60||f==1800){const auto maximum=*std::max_element(best.begin(),best.end());check(maximum==(f==60?18:540));std::cout<<"source_bound ticks="<<f<<" volleys="<<maximum<<'\n';}
 }
 // Find a potential with 10*volleys - 3*ticks <= P(end)-P(start).
 // A fixed point proves the upper bound 0.3 volley/tick for every cycle,
 // not just for the two finite test windows above. Fast attains it.
 std::vector<int> potential(graph.size());bool fixed=false;
 for(unsigned f=0;f<graph.size();++f){
    auto next=potential;
    for(unsigned i=0;i<graph.size();++i)for(auto e:graph[i])next[e.next]=std::max(next[e.next],potential[i]+10*int(e.sent)-3);
    if(next==potential){fixed=true;break;}
    potential=std::move(next);
 }
 check(fixed);
 std::cout<<"source_sustained_bound=3/10 volleys_per_tick states="<<graph.size()<<" transient_upper_bound="<<*std::max_element(potential.begin(),potential.end())-potential[0]<<"/10 volley\n";
 std::cout<<"shot_input_checks="<<checks<<" failures=0\n";return 0;}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
