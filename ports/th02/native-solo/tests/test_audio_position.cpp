#include "audio_preview.hpp"
#include "practice_music.hpp"
#include <set>
#include "th03/audio/pmd_opna.hpp"
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace th03::audio;
namespace {
unsigned checks=0;
void check(bool value,const char* message){++checks;if(!value)throw std::runtime_error(message);}
std::vector<std::byte> read(const std::filesystem::path& path){
    std::ifstream in(path,std::ios::binary|std::ios::ate);if(!in)throw std::runtime_error("Asset missing: "+path.string());
    const auto size=in.tellg();if(size<=0)throw std::runtime_error("Asset empty: "+path.string());
    std::vector<std::byte> bytes(static_cast<std::size_t>(size));in.seekg(0);if(!in.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size())))throw std::runtime_error("Asset read failed: "+path.string());return bytes;
}
std::vector<std::byte> midi_song(unsigned tempo){
    const std::array<unsigned char,13> events={0xec,0,19,0,60,240,240,100,0,240,0,0,0xfe};
    std::vector<std::byte> bytes(0x51+events.size());bytes[0]=std::byte(tempo);
    for(unsigned i=0;i<18;++i){bytes[2+i*4]=std::byte{0x51};bytes[5+i*4]=std::byte(i==0?0:255);}
    for(unsigned i=0;i<events.size();++i)bytes[0x51+i]=std::byte(events[i]);return bytes;
}
std::vector<std::byte> fm_song(unsigned bar){
    std::vector<std::byte> bytes(25);
    const auto word=[&](unsigned at,unsigned value){bytes[at]=std::byte(value&255);bytes[at+1]=std::byte(value>>8);};
    const unsigned lane=unsigned(bytes.size()-1);
    for(unsigned value:{0xdfu,bar,0x40u,240u,0x80u})bytes.push_back(std::byte(value));
    const unsigned empty=unsigned(bytes.size()-1);bytes.push_back(std::byte{0x80});
    for(unsigned part=0;part<11;++part)word(1+part*2,part==6?lane:empty);word(23,empty);return bytes;
}
}
int wmain(int argc,wchar_t** argv){try{
    const bool catalog=argc==4&&std::wstring_view(argv[3])==L"--require-catalog";
    check(argc==2||argc==3||catalog,"Usage: soew-audio-position-tests <sf2> [original-assets [--require-catalog]]");
    const auto sf=read(argv[1]);auto created=MmdSoundfontEngine::create({.soundfont=sf});check(bool(created),"MIDI create");auto &midi=*created;
    for(unsigned tempo:{60u,180u}){
        check(bool(midi.load_song(midi_song(tempo))),"MIDI load");check(bool(midi.play(true)),"MIDI original-first-tick play");
        check(midi.status().elapsed_ticks==0&&midi.status().note_ons==0,"MIDI tick-zero must not consume initial note");
        check(bool(midi.advance_to_tick(0,0,0)),"MIDI zero no-op");
        check(bool(midi.advance_to_tick(1,1,48000)),"MIDI first tick");check(midi.status().elapsed_ticks==1&&midi.status().note_ons==1,"MIDI note begins at raw tick one");
        check(bool(midi.advance_to_tick(192,192,48000*5)),"MIDI absolute measure one");auto state=midi.status();
        check(state.elapsed_ticks==192&&state.measure==1,"MIDI exact192 boundary independent of tempo");
        check(state.pending_gates==1&&state.active_voices!=0,"MIDI held note survives seek");
        check(state.music_volume_percent==100,"MIDI seek preserves gain");
        check(!midi.advance_to_tick(191,100,1000),"MIDI backward target rejected");
        check(!midi.advance_to_tick(194,1,48000),"MIDI tick budget rejected");check(midi.status().elapsed_ticks==192,"MIDI preflight failure leaves counter");
        check(!midi.advance_to_tick(193,1,0),"MIDI frame budget rejected");check(midi.status().elapsed_ticks==192,"MIDI frame failure leaves counter");
        check(bool(midi.advance_to_tick(241,100,96000)),"MIDI gate expiry");check(midi.status().pending_gates==0&&midi.status().note_offs==1,"MIDI original remaining gate expires once");
        check(bool(midi.play()),"MIDI legacy play preserved");check(midi.status().note_ons==1&&midi.status().elapsed_ticks==0,"MIDI default tick-zero behavior unchanged");
    }
    auto fm_created=PmdOpnaEngine::create({.input_clock_hz=7987200,.fidelity=PmdOpnaFidelity::minimum});check(bool(fm_created),"FM create");auto &fm=*fm_created;
    for(unsigned bar:{24u,96u}){
        check(bool(fm.load_song(fm_song(bar))),"FM load");check(bool(fm.play()),"FM play");check(bool(fm.advance_to_measure(0,0)),"FM zero no-op");check(fm.status().timer_b_count==0,"FM zero consumes no ticks");
        check(!fm.advance_to_measure(1,0),"FM bounded failure");check(fm.status().measure==0,"FM failure not reported as positioned");
        check(bool(fm.advance_to_measure(1,1000000)),"FM measure one");auto state=fm.status();check(state.measure==1&&state.timer_b_count==bar,"FM follows song DF measure length exactly");
        check(!fm.advance_to_measure(0,1000),"FM backward target rejected");
        check(bool(fm.play()),"FM fresh replay");check(bool(fm.advance_to_measure(1,1000000)),"FM repeated seek");check(fm.status().register_hash==state.register_hash&&fm.status().samples_rendered==state.samples_rendered,"FM repeat reconstructs identical state");
    }
    {
        auto prepared=AudioPreview::prepare(midi_song(180),true,sf,{}, {},false,1);
        check(bool(prepared),"Prepared MIDI rejected");check(prepared.positioned_measure()==1&&prepared.positioned_ticks()==192,"Prepared MIDI exact measure");
        auto moved=std::move(prepared);check(!prepared&&bool(moved),"Prepared ownership transfer");
        AudioPreview audio;check(audio.stop(),"Unpublished preparation stop");
        check(moved.positioned_ticks()==192,"Stopping UI must not mutate unpublished worker state");
        std::atomic_bool cancelled{true};auto stopped=AudioPreview::prepare(midi_song(180),true,sf,{}, {},false,1,&cancelled);check(!stopped&&stopped.error()=="Audio preparation cancelled","MIDI cancellation");
        auto stopped_fm=AudioPreview::prepare(fm_song(96),true,{}, {}, {},false,1,&cancelled);check(!stopped_fm&&stopped_fm.error()=="Audio preparation cancelled","FM cancellation");
        auto invalid=AudioPreview::prepare(midi_song(180),true,sf,{}, {},false,65536);
        check(!invalid&&!invalid.error().empty(),"Prepared out-of-range seek rejected");
        check(!audio.publish(std::move(moved),0xfffffffeu),"Prepared invalid device accepted");
        check(!audio.active()&&audio.queued()==0,"Prepared failed-open cleanup");
        check(!moved,"Published state cannot retain synth ownership");
    }
    if(argc>=3){
        const auto root=std::filesystem::path(argv[2]);
        const auto bank=read(root/"HUUMA.EFC"),carrier=read(root/"OP.M");
        std::set<std::pair<unsigned,unsigned>> checkpoint_targets={{0,83},{1,90},{2,88},{3,89},{5,93},{5,98},{5,104}};
        if(catalog){
            for(const auto &entry:soew::practice_chapters)
                if(entry.measure)checkpoint_targets.emplace(entry.stage,entry.measure);
            for(int stage=0;stage<soew::practice_stage_count;++stage){
                for(auto section:{soew::PracticeSection::Midboss,soew::PracticeSection::Midboss2}){
                    soew::PracticeSpec spec;spec.stage=stage;spec.section=section;
                    if(const auto measure=soew::practice_music_measure(spec))checkpoint_targets.emplace(stage,measure);
                }
            }
        }
        for(const auto &[stage,measure]:checkpoint_targets){
          const auto name="STAGE"+std::to_string(stage);
          if(!catalog&&stage==5&&(!std::filesystem::exists(root/(name+".M"))||!std::filesystem::exists(root/(name+".MMD"))))continue;
          const auto stage_fm=read(root/(name+".M")),stage_midi=read(root/(name+".MMD"));
          for(bool midi_mode:{false,true}){
            const auto song=midi_mode?std::span<const std::byte>(stage_midi):std::span<const std::byte>(stage_fm);
            const auto font=midi_mode?std::span<const std::byte>(sf):std::span<const std::byte>{};
            const auto candidate=AudioPreview::prepare(song,false,font,bank,stage_fm,false,measure);
            const auto reference=AudioPreview::prepare(song,false,font,bank,stage_fm,false,measure,nullptr,false);
            check(!reference.checkpoint_restored()&&candidate.same_prefill(reference),"Practice prefill differs from exact seek");
            if(catalog&&!candidate.checkpoint_restored())throw std::runtime_error("Catalog miss: "+name+" measure="+std::to_string(measure)+(midi_mode?" MIDI":" FM"));
            check(candidate.positioned_measure()==measure&&candidate.positioned_measure()==reference.positioned_measure()&&candidate.positioned_ticks()==reference.positioned_ticks(),"Practice position differs from exact seek");
            std::cout<<"practice_prefill_equivalent stage="<<stage+1<<" mode="<<(midi_mode?"midi":"fm")<<" checkpoint_restored="<<candidate.checkpoint_restored()<<'\n';
          }
        }
        // Same measure and MIDI data, but a different valid FM effects carrier:
        // the private checkpoint must not be treated as a hit for this asset set.
        {
            const auto song=read(root/"STAGE1.MMD");
            const auto candidate=AudioPreview::prepare(song,false,sf,bank,carrier,false,90);
            const auto reference=AudioPreview::prepare(song,false,sf,bank,carrier,false,90,nullptr,false);
            check(!candidate.checkpoint_restored()&&candidate.same_prefill(reference),"MIDI carrier mismatch did not use exact fallback");
            check(candidate.positioned_measure()==90&&candidate.positioned_ticks()==90u*192u,"Carrier-mismatch fallback did not position exactly");
            std::cout<<"practice_checkpoint_carrier_mismatch exact_fallback=1\n";
        }
        for(unsigned stage=0;stage<2;++stage){const auto base="STAGE"+std::to_string(stage);const auto song_fm=read(root/(base+".M")),song_midi=read(root/(base+".MMD"));
        const std::vector<unsigned> measures=stage==0?std::vector<unsigned>{19,37,38,66,83}:std::vector<unsigned>{10,18,26,33,58,74,90};
        for(unsigned measure:measures)for(bool use_midi:{false,true}){
            auto prepared=AudioPreview::prepare(use_midi?std::span<const std::byte>(song_midi):std::span<const std::byte>(song_fm),true,use_midi?std::span<const std::byte>(sf):std::span<const std::byte>{},bank,carrier,false,measure);
            if(!prepared)throw std::runtime_error(std::string(use_midi?"Real MIDI":"Real FM")+" measure="+std::to_string(measure)+" "+prepared.error());
            check(prepared.positioned_measure()==measure,"Real song exact positioned measure");
            if(use_midi)check(prepared.positioned_ticks()==std::uint64_t(measure)*192,"Real MIDI exact raw tick boundary");
            else check(prepared.positioned_ticks()!=0,"Real FM native ticks advanced");
            std::cout<<"real_position stage="<<stage+1<<" mode="<<(use_midi?"midi":"fm")<<" measure="<<measure<<" ticks="<<prepared.positioned_ticks()<<'\n';
        }}
    }
    std::cout<<"audio_position_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
