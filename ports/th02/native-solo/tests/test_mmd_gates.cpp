#include "mmd_preview_engine.hpp"
#include "mmd_stream.hpp"
#include <array>
#include <fstream>
#include <filesystem>
#include <vector>
#include <iostream>
#include <cmath>
using namespace th03::audio;
std::vector<std::byte> fixture(){
    // Same organ key repeats after 12ticks, gates last24; old single counter
    // lost the first deadline. Rest24 after second note ensures both expire.
    const std::array<unsigned char,17> events={0xec,0,19,0,60,12,24,100,60,24,24,100,0,120,0,0,0xfe};
    std::vector<std::byte>b(0x51+events.size());b[0]=std::byte{120};
    for(unsigned i=0;i<18;++i){b[2+i*4]=std::byte{0x51};b[5+i*4]=std::byte(i==0?0:255);}
    for(unsigned i=0;i<events.size();++i)b[0x51+i]=std::byte(events[i]);return b;
}
static std::vector<std::byte> checkpoint_fixture(){
    // Same key overlaps, the pedal holds a released voice, and playback crosses
    // a tempo change, controller and bend updates, and a loop boundary.
    const unsigned char events[]={
        0xec,0,19,0, 0xf9,0,0,0, 0xeb,0,64,127, 60,12,18,98,
        60,12,57,102, 0xe7,6,37,0, 0xeb,0,10,95, 0xee,7,0,72,
        0xeb,6,64,0, 0xeb,0,7,63, 67,72,70,88, 0xe7,7,64,0,
        0xf8,0,0,0, 0xfe};
    auto bytes=fixture();
    bytes.resize(0x51+sizeof(events));
    for(std::size_t i=0;i<sizeof(events);++i)bytes[0x51+i]=std::byte(events[i]);
    return bytes;
}

static void check_reference_and_checkpoint(std::span<const std::byte> sf){
    auto check=[](bool ok,const char* message){if(!ok)throw std::runtime_error(message);};
    const auto song=checkpoint_fixture();
    const auto start=[&] {
        auto engine=MmdSoundfontEngine::create({.soundfont=sf});
        check(engine && engine->load_song(song) && engine->set_user_volume(25) && engine->play(true),"Checkpoint test setup failed");
        return std::move(*engine);
    };
    constexpr std::uint64_t target=31;
    // Recreate the original reference seek using only the public renderer.
    // Tick 25 applies E7, so it takes the full first 25 intervals at 120 BPM.
    auto reference=start();
    std::array<PmdCanonicalStereoSample,256> scratch{};
    constexpr std::uint64_t threshold=48000ULL*60*64;
    std::uint64_t phase=0;
    for(std::uint64_t tick=0;tick<target;++tick){
        const auto increment=120ULL*(tick<25?64:37)*48;
        auto remaining=(threshold-phase+increment-1)/increment;
        phase+=remaining*increment-threshold;
        while(remaining){
            const auto chunk=std::min(remaining,256ULL);
            scratch.fill({});
            check(bool(reference.render_mix(std::span(scratch).first(chunk))),"Reference render failed");
            remaining-=chunk;
        }
        check(reference.status().elapsed_ticks==tick+1,"Reference tick diverged");
    }
    auto discard=start();
    check(bool(discard.advance_to_tick(target,100,48000)),"Discard seek failed");
    const auto at=reference.status();
    check(at.active_voices>=2&&at.pending_gates>=1&&at.note_offs>=1,"Expected sustained, overlapping voices at checkpoint");
    MmdCheckpointIdentity identity{};
    // The fixture verifies the codec's identity binding, not a file's hash provider.
    identity.song_sha256[0]=std::byte{1};identity.soundfont_sha256[0]=std::byte{2};
    const auto bytes=discard.save_checkpoint(identity);
    check(bool(bytes),"Checkpoint save failed");
    auto restored=start();
    auto wrong=identity;wrong.soundfont_sha256[0]=std::byte{3};
    check(!restored.restore_checkpoint(*bytes,wrong,target)&&!restored.restore_checkpoint(*bytes,identity,target+1),"Wrong checkpoint identity accepted");
    auto corrupt=*bytes;corrupt.back()^=std::byte{1};
    check(!restored.restore_checkpoint(corrupt,identity,target)&&restored.status().elapsed_ticks==0,"Corrupt checkpoint affected engine");
    check(bool(restored.restore_checkpoint(*bytes,identity,target)),"Checkpoint restore failed");
    const auto recovered=restored.status();
    check(at.elapsed_ticks==recovered.elapsed_ticks&&at.note_ons==recovered.note_ons&&at.note_offs==recovered.note_offs&&at.pending_gates==recovered.pending_gates&&at.active_voices==recovered.active_voices,"Checkpoint voice/gate counters differ");
    soew::MmdStream ref_stream(reference),discard_stream(discard),restored_stream(restored);
    for(unsigned slot=0;slot<180;++slot){
        std::array<PmdCanonicalStereoSample,960> a{},b{},c{};
        check(ref_stream.render(a)&&discard_stream.render(b)&&restored_stream.render(c),"Checkpoint refill failed");
        check(a==b&&a==c,"Checkpoint/discard PCM differs from old render-and-discard path");
    }
    check(restored.status().note_ons>at.note_ons&&restored.status().note_offs>at.note_offs,"Refill did not reach next notes and releases");
}

int wmain(int argc,wchar_t**argv){try{
    if(argc!=2)throw std::runtime_error("Usage: soew-mmd-gate-tests <sf2>");std::ifstream f(std::filesystem::path(argv[1]),std::ios::binary|std::ios::ate);if(!f)throw std::runtime_error("SF missing");auto n=f.tellg();std::vector<std::byte>sf(static_cast<std::size_t>(n));f.seekg(0);f.read(reinterpret_cast<char*>(sf.data()),n);
    unsigned checks=0;auto check=[&](bool b){++checks;if(!b)throw std::runtime_error("Gate/gain regression failed");};auto c=MmdSoundfontEngine::create({.soundfont=sf});check(bool(c));auto &e=*c;check(bool(e.load_song(fixture())));check(bool(e.play()));std::array<PmdCanonicalStereoSample,4800> pcm{};
    //0.6s=57ticks, bothnotesoff, reststillplaying.
    for(int i=0;i<6;++i){pcm.fill({});check(bool(e.render_mix(pcm)));}
    const auto s=e.status();check(s.note_ons==2&&s.note_offs==2&&s.pending_gates==0&&s.peak_gates==2);check(s.playing);
    // New looped sustained note holds while master volume changes. Applying
    // volume0 must silence ALREADY PLAYING voices, not just future NoteOns.
    auto sustained=fixture();sustained[0x51+5]=std::byte{240};sustained[0x51+6]=std::byte{240};check(bool(e.load_song(sustained)));check(bool(e.play()));pcm.fill({});check(bool(e.render_mix(pcm)));check(bool(e.set_user_volume(0)));pcm.fill({});check(bool(e.render_mix(pcm)));for(auto frame:pcm)check(frame.left==0&&frame.right==0);
    check(bool(e.set_user_volume(25)));pcm.fill({});check(bool(e.render_mix(pcm)));bool nonzero=false;for(auto p:pcm)nonzero|=p.left||p.right;check(nonzero);
    check_reference_and_checkpoint(sf);
    std::cout<<"gate_ons="<<s.note_ons<<" gate_offs="<<s.note_offs<<" pending="<<s.pending_gates<<" peak="<<s.peak_gates<<" checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
