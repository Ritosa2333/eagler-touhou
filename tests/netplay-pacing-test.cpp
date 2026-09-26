#include "../portable/sdl/FrameCadence.hpp"
#include <eagler/netplay/SessionPacing.hpp>
#include <cassert>
#include <cstdio>
int main(){
    for(unsigned hz:{60u,120u,144u,165u}){
        touhou::sdl::FrameCadence clock[2];Netplay::SessionPacing pacing[2];
        unsigned frame[2]{108,100};
        for(unsigned callback=0;callback<hz*60;++callback){
            for(unsigned seat=0;seat<2;++seat){
                const auto peer=1-seat;Netplay::InputPacket packet{};
                packet.senderPlayer=peer;packet.senderFrame=frame[peer]-3;
                packet.frameAdvantage=short(int(frame[peer])-int(frame[seat])+3);
                pacing[seat].Observe(frame[seat],packet);
            }
            for(unsigned seat=0;seat<2;++seat){
                const auto ticks=clock[seat].advance((1./hz)/pacing[seat].IntervalScale());
                assert(ticks<=1);frame[seat]+=ticks;
                assert(clock[seat].interpolation_alpha()>=0&&clock[seat].interpolation_alpha()<=1);
            }
        }
        const auto difference=int(frame[0])-int(frame[1]);
        assert(difference>=-2&&difference<=2);
        assert(frame[0]>3600&&frame[1]>3600);
    }
    std::puts("TH10 netplay display cadence: PASS");
}
