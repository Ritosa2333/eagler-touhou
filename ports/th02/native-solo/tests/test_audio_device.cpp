#include "audio_preview.hpp"
#include "ending_renderer.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
std::vector<std::byte> read(const std::filesystem::path&p){std::ifstream f(p,std::ios::binary|std::ios::ate);if(!f)throw std::runtime_error("Song missing");const auto n=f.tellg();if(n<1||n>65535)throw std::runtime_error("Song size invalid");std::vector<std::byte>b(static_cast<std::size_t>(n));f.seekg(0);if(!f.read(reinterpret_cast<char*>(b.data()),n))throw std::runtime_error("Song read failed");return b;}
int wmain(int argc,wchar_t**argv){try{
    const bool staff=argc>=3&&std::wstring_view(argv[argc-1])==L"--staff";
    argc-=staff;
    if(argc!=2&&argc!=3)throw std::runtime_error("Usage: soew-audio-device-tests <song> [SoundFont] [--staff]");
    auto bytes=read(argv[1]);std::vector<std::byte> sf;
    if(argc==3){std::ifstream file(argv[2],std::ios::binary|std::ios::ate);if(!file)throw std::runtime_error("SoundFont missing");auto n=file.tellg();if(n<1||n>40000000)throw std::runtime_error("SoundFont size invalid");sf.resize(static_cast<std::size_t>(n));file.seekg(0);if(!file.read(reinterpret_cast<char*>(sf.data()),n))throw std::runtime_error("SoundFont read failed");}
    unsigned checks=0;
    const auto check=[&](bool ok,const char *what){++checks;if(!ok)throw std::runtime_error(what);};
    AudioPreview audio;
    check(!audio.start(bytes,true,0xfffffffeu,sf),"Invalid device accepted");check(!audio.active(),"Failed-open active");
    for(int cycle=0;cycle<3;++cycle){
        const auto previous_generation=audio.playback_generation();
        if(!audio.start(bytes,true,WAVE_MAPPER,sf))throw std::runtime_error(audio.error());
        check(audio.queued()==4,"Prefill missing");
        check(!audio.completed_frames()&&!audio.completed_measure()&&audio.playback_generation()!=previous_generation,"Prefill advanced the playback clock");
        const auto begin=GetTickCount64();bool paused_once=false,resumed=false;std::uint64_t paused_count=0;
        while(GetTickCount64()-begin<700){
            const auto elapsed=GetTickCount64()-begin;
            if(!paused_once&&elapsed>=150){check(audio.pause(true),"Pause failed");paused_count=audio.submitted();paused_once=true;}
            if(paused_once&&!resumed&&elapsed>=300){check(audio.submitted()==paused_count,"Queue advanced while paused");check(audio.pause(false),"Resume failed");resumed=true;}
            const auto old_frames=audio.completed_frames();
            const auto old_measure=audio.completed_measure();
            const auto wait=WaitForSingleObject(audio.event(),20);check(wait!=WAIT_FAILED,"Event wait failed");check(audio.pump(),"Pump failed");
            check(audio.completed_frames()>=old_frames&&audio.completed_frames()<=audio.submitted(),"Invalid playback watermark");
            check(!old_measure||(audio.completed_measure()&&*audio.completed_measure()>=*old_measure),"Measure watermark regressed");
            if(audio.paused())check(audio.completed_frames()==old_frames&&audio.completed_measure()==old_measure,"Paused clock advanced");
        }
        check(audio.submitted()>3840,"Device queue did not recycle");std::cout<<"cycle="<<cycle<<" submitted="<<audio.submitted()<<" silent=true\n";
        check(audio.stop(),"Stop cleanup failed");check(!audio.active()&&audio.queued()==0,"Queue remains after stop");
    }
    {
        auto first=AudioPreview::prepare(bytes,true,sf);
        check(bool(first)&&audio.publish(std::move(first)),"Replacement test failed to start");
        const auto begin=GetTickCount64();
        while(!audio.completed_frames()){
            check(GetTickCount64()-begin<3000,"Replacement test has no initial completion");
            check(WaitForSingleObject(audio.event(),20)!=WAIT_FAILED&&audio.pump(),"Replacement initial pump failed");
        }
        const auto previous=audio.playback_generation();
        const auto event=audio.event();
        auto replacement=AudioPreview::prepare(bytes,true,sf);
        check(bool(replacement)&&audio.publish(std::move(replacement),WAVE_MAPPER,true,true),"Device reuse failed");
        check(audio.event()==event&&audio.playback_generation()!=previous,"Replacement did not establish a new playback generation");
        check(audio.pump()&&!audio.completed_frames()&&!audio.completed_measure(),"Returned buffers leaked into the new playback clock");
        check(audio.stop(),"Replacement cleanup failed");
    }
    if(staff){
        auto prepared=AudioPreview::prepare(bytes,true,sf);
        check(bool(prepared),"Staff preparation failed");
        check(audio.publish(std::move(prepared),WAVE_MAPPER,true),"Staff device publication failed");
        check(audio.pump()&&!audio.completed_measure(),"Paused prefill unlocked a Staff cue");
        check(audio.pause(false),"Staff device resume failed");
        const auto begin=GetTickCount64();
        using K=soew::EndingEventKind;
        soew::EndingScript script;
        constexpr std::array cues{5u,6u,8u,9u,13u,17u,21u,25u,29u,33u,37u,41u,45u,49u,53u,57u};
        for(unsigned cue:cues){
            script.events.push_back({K::WaitMeasure,{},int(cue),100});
            script.events.push_back({K::StaffVersion,{},int(cue)});
        }
        script.staff.end=script.events.size();
        soew::EndingPlayer player(std::move(script));
        soew::StaffMusicClock clock;
        auto previous=begin;
        unsigned emitted=0;
        while(emitted<cues.size()){
            check(GetTickCount64()-begin<240000,"Staff playback did not reach every cue");
            check(WaitForSingleObject(audio.event(),20)!=WAIT_FAILED&&audio.pump(),"Staff pump failed");
            const auto now=GetTickCount64();
            const auto measure=clock.update(audio.active(),audio.completed_frames(),audio.completed_measure(),unsigned(now-previous));
            previous=now;
            check(measure.has_value(),"Real Staff track unexpectedly fell back");
            player.advance_display_frame({false,false,measure},(now-begin)%4000<2000,[&](const auto &step){
                check(emitted<cues.size()&&step.event.kind==K::StaffVersion&&unsigned(step.event.a)==cues[emitted]&&*measure>=cues[emitted],"Staff cue fired early or out of order");
                std::cout<<"staff_device_cue="<<cues[emitted++]<<" completed_frames="<<audio.completed_frames()
                         <<" elapsed_ms="<<now-begin<<std::endl;
            });
        }
        check(audio.stop()&&!audio.completed_frames()&&!audio.completed_measure(),"Reset retained a Staff watermark");
    }
    const auto root=std::filesystem::path(argv[1]).parent_path();
    if(std::filesystem::exists(root/"HUUMA.EFC")){
        const auto bank=read(root/"HUUMA.EFC"),carrier=read(root/"OP.M");
        for(unsigned mode=0;mode<2;++mode){if((mode==1&&sf.empty())||(mode==0&&!sf.empty()))continue;
            check(audio.start(bytes,true,WAVE_MAPPER,mode?std::span<const std::byte>(sf):std::span<const std::byte>{},bank,carrier),"Mixed EFC start failed");
            const auto baseline=audio.pcm_hash();check(audio.effect(2),"Mixed death effect rejected");const auto begin=GetTickCount64();while(GetTickCount64()-begin<300){check(WaitForSingleObject(audio.event(),20)!=WAIT_FAILED,"Mix wait failed");check(audio.pump(),"Mix pump failed");}check(audio.pcm_hash()!=baseline&&audio.effect_calls()>0,"Mixed effect rendering did not advance");check(audio.stop(),"Mix stop failed");
        }
    }
    check(audio.stop(),"Idempotent stop failed");std::cout<<"device_checks="<<checks<<" failures=0 no_audible_samples=true\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
