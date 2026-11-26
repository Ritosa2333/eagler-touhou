#include "audio_preview.hpp"
#include <algorithm>
#include <vector>
using namespace th03::audio;
#if defined(SOEW_PRIVATE_CHECKPOINT_CATALOG) || defined(SOEW_PRIVATE_EXTRA93_MIDI_CHECKPOINT) || defined(SOEW_PRIVATE_EXTRA98_MIDI_CHECKPOINT) || defined(SOEW_PRIVATE_MIDI_CHECKPOINT) || defined(SOEW_PRIVATE_EXTRA_MIDI_CHECKPOINT) || defined(SOEW_PRIVATE_STAGE4_MIDI_CHECKPOINT) || defined(SOEW_PRIVATE_STAGE1_MIDI_CHECKPOINT) || defined(SOEW_PRIVATE_STAGE3_MIDI_CHECKPOINT)
#define SOEW_HAS_PRIVATE_MIDI_CHECKPOINTS
#endif
#if defined(SOEW_PRIVATE_CHECKPOINT_CATALOG) || defined(SOEW_PRIVATE_EXTRA93_FM_CHECKPOINT) || defined(SOEW_PRIVATE_EXTRA98_FM_CHECKPOINT) || defined(SOEW_PRIVATE_FM_CHECKPOINT) || defined(SOEW_PRIVATE_EXTRA_FM_CHECKPOINT) || defined(SOEW_PRIVATE_STAGE4_FM_CHECKPOINT) || defined(SOEW_PRIVATE_STAGE1_FM_CHECKPOINT) || defined(SOEW_PRIVATE_STAGE3_FM_CHECKPOINT)
#define SOEW_HAS_PRIVATE_FM_CHECKPOINTS
#endif
#if defined(SOEW_HAS_PRIVATE_MIDI_CHECKPOINTS) || defined(SOEW_HAS_PRIVATE_FM_CHECKPOINTS)
#include <bcrypt.h>
#include <bit>
namespace {
#ifdef SOEW_PRIVATE_CHECKPOINT_CATALOG
struct CatalogCheckpoint {unsigned measure; WORD fm_id,midi_id;};
constexpr CatalogCheckpoint checkpoint_catalog[]={
#include "catalog_entries.inc"
};
bool has_catalog_checkpoint(unsigned measure){
    return std::any_of(std::begin(checkpoint_catalog),std::end(checkpoint_catalog),
        [measure](const auto &entry){return entry.measure==measure;});
}
// A measure may occur in several songs. Only the decoder's full identity check can select a hit.
bool try_catalog_checkpoint(unsigned measure,bool midi,auto&& restore){
    const auto module=GetModuleHandleW(nullptr);
    for(const auto &entry:checkpoint_catalog){
        if(entry.measure!=measure)continue;
        const auto resource=FindResource(module,MAKEINTRESOURCE(midi?entry.midi_id:entry.fm_id),RT_RCDATA);
        if(!resource)continue;
        const auto loaded=LoadResource(module,resource);
        const auto bytes=loaded?static_cast<const std::byte*>(LockResource(loaded)):nullptr;
        if(bytes&&restore(std::span(bytes,SizeofResource(module,resource))))return true;
    }
    return false;
}
#endif
#ifdef SOEW_PRIVATE_MIDI_CHECKPOINT
constexpr std::byte private_midi_checkpoint[]={
#include "soew_private_midi_checkpoint.inc"
};
#endif
#ifdef SOEW_PRIVATE_FM_CHECKPOINT
constexpr std::byte private_fm_checkpoint[]={
#include "soew_private_fm_checkpoint.inc"
};
#endif
std::optional<std::array<std::byte,32>> checkpoint_digest(std::span<const std::byte> bytes){
    if(bytes.size()>ULONG_MAX)return {};
    struct Provider { BCRYPT_ALG_HANDLE handle=nullptr; ~Provider(){if(handle)BCryptCloseAlgorithmProvider(handle,0);} } p;
    if(BCryptOpenAlgorithmProvider(&p.handle,BCRYPT_SHA256_ALGORITHM,nullptr,0)<0)return {};
    std::array<std::byte,32> result{};
    struct Hash { BCRYPT_HASH_HANDLE handle=nullptr; ~Hash(){if(handle)BCryptDestroyHash(handle);} } hash;
    if(BCryptCreateHash(p.handle,&hash.handle,nullptr,0,nullptr,0,0)<0||
       (!bytes.empty()&&BCryptHashData(hash.handle,reinterpret_cast<PUCHAR>(const_cast<std::byte*>(bytes.data())),static_cast<ULONG>(bytes.size()),0)<0)||
       BCryptFinishHash(hash.handle,reinterpret_cast<PUCHAR>(result.data()),static_cast<ULONG>(result.size()),0)<0)return {};
    return result;
}
#ifdef SOEW_PRIVATE_STAGE1_MIDI_CHECKPOINT
constexpr std::byte stage1_midi_checkpoint[]={
#include "SOEW_PRIVATE_STAGE1_MIDI_CHECKPOINT.inc"
};
#endif
#ifdef SOEW_PRIVATE_STAGE1_FM_CHECKPOINT
constexpr std::byte stage1_fm_checkpoint[]={
#include "SOEW_PRIVATE_STAGE1_FM_CHECKPOINT.inc"
};
#endif
#ifdef SOEW_PRIVATE_STAGE3_MIDI_CHECKPOINT
constexpr std::byte stage3_midi_checkpoint[]={
#include "SOEW_PRIVATE_STAGE3_MIDI_CHECKPOINT.inc"
};
#endif
#ifdef SOEW_PRIVATE_STAGE3_FM_CHECKPOINT
constexpr std::byte stage3_fm_checkpoint[]={
#include "SOEW_PRIVATE_STAGE3_FM_CHECKPOINT.inc"
};
#endif
#ifdef SOEW_PRIVATE_STAGE4_MIDI_CHECKPOINT
constexpr std::byte stage4_midi_checkpoint[]={
#include "SOEW_PRIVATE_STAGE4_MIDI_CHECKPOINT.inc"
};
#endif
#ifdef SOEW_PRIVATE_STAGE4_FM_CHECKPOINT
constexpr std::byte stage4_fm_checkpoint[]={
#include "SOEW_PRIVATE_STAGE4_FM_CHECKPOINT.inc"
};
#endif
#ifdef SOEW_PRIVATE_EXTRA93_MIDI_CHECKPOINT
constexpr std::byte extra93_midi_checkpoint[]={
#include "SOEW_PRIVATE_EXTRA93_MIDI_CHECKPOINT.inc"
};
#endif
#ifdef SOEW_PRIVATE_EXTRA98_MIDI_CHECKPOINT
constexpr std::byte extra98_midi_checkpoint[]={
#include "SOEW_PRIVATE_EXTRA98_MIDI_CHECKPOINT.inc"
};
#endif
#ifdef SOEW_PRIVATE_EXTRA93_FM_CHECKPOINT
constexpr std::byte extra93_fm_checkpoint[]={
#include "SOEW_PRIVATE_EXTRA93_FM_CHECKPOINT.inc"
};
#endif
#ifdef SOEW_PRIVATE_EXTRA98_FM_CHECKPOINT
constexpr std::byte extra98_fm_checkpoint[]={
#include "SOEW_PRIVATE_EXTRA98_FM_CHECKPOINT.inc"
};
#endif
#ifdef SOEW_PRIVATE_EXTRA_MIDI_CHECKPOINT
constexpr std::byte extra_midi_checkpoint[]={
#include "SOEW_PRIVATE_EXTRA_MIDI_CHECKPOINT.inc"
};
#endif
#ifdef SOEW_PRIVATE_EXTRA_FM_CHECKPOINT
constexpr std::byte extra_fm_checkpoint[]={
#include "SOEW_PRIVATE_EXTRA_FM_CHECKPOINT.inc"
};
#endif
#ifdef SOEW_HAS_PRIVATE_FM_CHECKPOINTS
std::optional<FmCheckpoint> decode_private_fm(std::span<const std::byte> blob,const std::array<std::byte,32>& song,const std::array<std::byte,32>& effects,unsigned measure){
    if(blob.size()<177)return {};
    const auto b=blob.first(blob.size()-32);
    std::size_t pos=0;
    const auto read=[&](unsigned n){
        if(n>b.size()-pos)throw std::runtime_error("Truncated FM checkpoint");
        std::uint64_t v=0;
        for(unsigned i=0;i<n;++i)v|=std::uint64_t(std::to_integer<unsigned>(b[pos++]))<<(8*i);
        return v;
    };
    if(read(4)!=0x50434d46||read(4)!=1||read(4)!=7987200||read(4)!=0||read(4)!=25||read(4)!=25||read(4)!=measure)return {};
    if(!std::equal(song.begin(),song.end(),b.begin()+pos))return {};
    pos+=32;
    if(!std::equal(effects.begin(),effects.end(),b.begin()+pos))return {};
    pos+=32;
    FmCheckpoint c{};c.measure=static_cast<std::uint16_t>(measure);
    c.native_samples=read(8);c.writes=read(8);c.write_hash=static_cast<std::uint32_t>(read(4));c.ticks=static_cast<std::uint32_t>(read(4));
    for(auto &d:c.deadlines)d=std::bit_cast<std::int64_t>(read(8));
    const auto irq=read(1);if(irq>1)return {};c.irq=irq!=0;
    const auto events=read(4),statuses=read(4),chip=read(4);
    if(events*8+statuses+chip!=b.size()-pos)return {};
    c.interrupts.reserve(events);std::uint64_t clock=0;
    for(std::uint64_t i=0;i<events;++i){clock+=read(4);c.interrupts.push_back({clock,static_cast<std::uint32_t>(read(4))});}
    c.statuses.reserve(statuses);for(std::uint64_t i=0;i<statuses;++i)c.statuses.push_back(static_cast<std::uint8_t>(read(1)));
    c.chip.reserve(chip);for(std::uint64_t i=0;i<chip;++i)c.chip.push_back(static_cast<std::uint8_t>(read(1)));
    return c;
}
#endif
}
#endif
bool AudioPreview::fail(const char *op,MMRESULT code){error_=std::string(op)+" code="+std::to_string(code);return false;}
AudioPreview::~AudioPreview(){
    if(!stop()){
        // Failed driver cleanup: retain submitted storage until process exit.
        // Never destroy memory still owned by a driver.
        if(device_){(void)data_.release();}
    }
}
AudioPreview::Prepared AudioPreview::prepare(std::span<const std::byte> song,bool silent_device_test,std::span<const std::byte> soundfont,std::span<const std::byte> effects,std::span<const std::byte> fm_carrier,bool music_off,unsigned start_measure,const std::atomic_bool* cancelled,[[maybe_unused]] bool allow_checkpoint,bool mute_fm_bgm,soew::AudioLevels levels){
    Prepared prepared;prepared.data_=std::make_unique<Data>();auto &data=*prepared.data_;
    auto &error=prepared.error_;
    for(auto byte:song){data.song_hash^=std::to_integer<std::uint8_t>(byte);data.song_hash*=16777619u;}
    data.silent=silent_device_test;data.resampler.reset();data.requested_measure=start_measure;
    data.fm_music=soundfont.empty()&&!music_off&&!mute_fm_bgm;
    data.midi_music=!soundfont.empty()&&!music_off;
    data.effects=!effects.empty();
    if(!music_off&&start_measure>UINT16_MAX){error="Audio measure out of range";return prepared;}
    if(!soundfont.empty()){
        auto created=MmdSoundfontEngine::create({.soundfont=soundfont});if(!created){error="MIDI SoundFont rejected";return prepared;}
        data.midi=std::move(*created);
        const auto loaded=data.midi->load_song(song);
        if(!loaded||!data.midi->set_user_volume(25)||!data.midi->play(start_measure!=0&&!music_off)){error="MIDI song rejected";return prepared;}
        data.midi_stream=std::make_unique<soew::MmdStream>(*data.midi);
    }
    if(!data.midi||!effects.empty()){
        auto created=PmdOpnaEngine::create({.input_clock_hz=7987200,.fidelity=PmdOpnaFidelity::minimum});
        if(!created){error="PMD create failed";return prepared;}
        data.engine=std::move(*created);
        if(!data.engine->set_user_volumes(data.midi||music_off||mute_fm_bgm?0:25,effects.empty()?0:25)||!data.engine->load_song(data.midi?fm_carrier:song)||(!effects.empty()&&!data.engine->load_effect_bank(effects))||!data.engine->play()){error="PMD song/effect bank rejected";return prepared;}
        if(data.midi||music_off)data.engine->stop();
    }
    if(music_off&&data.midi)data.midi->stop();
    if(!music_off&&start_measure!=0){
        // Musical counters determine position; sample/tick limits only bound work.
        constexpr std::uint64_t maximum_ticks=200000;
        constexpr std::uint64_t maximum_midi_frames=48000ULL*600;
        constexpr std::uint64_t maximum_fm_samples=166400ULL*600;
        if(data.midi){
            const auto target=std::uint64_t(start_measure)*mmd_ticks_per_measure;
#ifdef SOEW_HAS_PRIVATE_MIDI_CHECKPOINTS
            std::span<const std::byte> blob;
#ifdef SOEW_PRIVATE_EXTRA93_MIDI_CHECKPOINT
            if(start_measure==93)blob=extra93_midi_checkpoint;
#endif
#ifdef SOEW_PRIVATE_EXTRA98_MIDI_CHECKPOINT
            if(start_measure==98)blob=extra98_midi_checkpoint;
#endif
#ifdef SOEW_PRIVATE_STAGE1_MIDI_CHECKPOINT
            if(start_measure==83)blob=stage1_midi_checkpoint;
#endif
#ifdef SOEW_PRIVATE_STAGE3_MIDI_CHECKPOINT
            if(start_measure==88)blob=stage3_midi_checkpoint;
#endif
#ifdef SOEW_PRIVATE_STAGE4_MIDI_CHECKPOINT
            if(start_measure==89)blob=stage4_midi_checkpoint;
#endif
#ifdef SOEW_PRIVATE_MIDI_CHECKPOINT
            if(start_measure==90)blob=private_midi_checkpoint;
#endif
#ifdef SOEW_PRIVATE_EXTRA_MIDI_CHECKPOINT
            if(start_measure==104)blob=extra_midi_checkpoint;
#endif
            bool has_checkpoint=!blob.empty();
#ifdef SOEW_PRIVATE_CHECKPOINT_CATALOG
            has_checkpoint|=has_catalog_checkpoint(start_measure);
#endif
            if(allow_checkpoint&&has_checkpoint&&!(cancelled&&cancelled->load(std::memory_order_relaxed))){
                const auto s=checkpoint_digest(song),f=checkpoint_digest(soundfont),c=checkpoint_digest(fm_carrier),e=checkpoint_digest(effects);
                if(s&&f&&c&&e){
                    const MmdCheckpointIdentity identity{*s,*f,*c,*e};
                    const auto restore=[&](auto bytes){return bool(data.midi->restore_checkpoint(bytes,identity,target));};
                    data.checkpoint_restored=!blob.empty()&&restore(blob);
#ifdef SOEW_PRIVATE_CHECKPOINT_CATALOG
                    if(!data.checkpoint_restored)data.checkpoint_restored=try_catalog_checkpoint(start_measure,true,restore);
#endif
                }
            }
#endif
            while(data.midi->status().elapsed_ticks<target){if(cancelled&&cancelled->load(std::memory_order_relaxed)){error="Audio preparation cancelled";return prepared;}const auto next=std::min(target,data.midi->status().elapsed_ticks+192);const auto positioned=data.midi->advance_to_tick(next,maximum_ticks,maximum_midi_frames);if(!positioned){error="MIDI measure seek failed code="+std::to_string(unsigned(positioned.error().code));return prepared;}}
            const auto state=data.midi->status();
            if(state.elapsed_ticks!=target||state.measure!=start_measure){error="MIDI seek did not reach the exact target";return prepared;}
            data.positioned_measure=state.measure;data.positioned_ticks=state.elapsed_ticks;
            data.midi_stream->reset();
        }else{
#ifdef SOEW_HAS_PRIVATE_FM_CHECKPOINTS
            std::span<const std::byte> blob;
#ifdef SOEW_PRIVATE_FM_CHECKPOINT
            if(start_measure==90)blob=private_fm_checkpoint;
#endif
#ifdef SOEW_PRIVATE_EXTRA_FM_CHECKPOINT
            if(start_measure==104)blob=extra_fm_checkpoint;
#endif
#ifdef SOEW_PRIVATE_EXTRA93_FM_CHECKPOINT
            if(start_measure==93)blob=extra93_fm_checkpoint;
#endif
#ifdef SOEW_PRIVATE_EXTRA98_FM_CHECKPOINT
            if(start_measure==98)blob=extra98_fm_checkpoint;
#endif
#ifdef SOEW_PRIVATE_STAGE4_FM_CHECKPOINT
            if(start_measure==89)blob=stage4_fm_checkpoint;
#endif
#ifdef SOEW_PRIVATE_STAGE1_FM_CHECKPOINT
            if(start_measure==83)blob=stage1_fm_checkpoint;
#endif
#ifdef SOEW_PRIVATE_STAGE3_FM_CHECKPOINT
            if(start_measure==88)blob=stage3_fm_checkpoint;
#endif
            bool has_checkpoint=!blob.empty();
#ifdef SOEW_PRIVATE_CHECKPOINT_CATALOG
            has_checkpoint|=has_catalog_checkpoint(start_measure);
#endif
            if(allow_checkpoint&&!mute_fm_bgm&&has_checkpoint&&!effects.empty()&&!(cancelled&&cancelled->load(std::memory_order_relaxed))){
                const auto s=checkpoint_digest(song),e=checkpoint_digest(effects);
                if(s&&e){
                    const auto restore=[&](auto bytes){
                        auto checkpoint=decode_private_fm(bytes,*s,*e,start_measure);
                        if(!checkpoint)return false;
                        // Restoration can mutate the candidate before detecting a mismatch.
                        // Keep the original fresh engine intact for the exact fallback.
                        auto candidate=PmdOpnaEngine::create({.input_clock_hz=7987200,.fidelity=PmdOpnaFidelity::minimum});
                        if(!candidate||!candidate->set_user_volumes(25,25)||!candidate->load_song(song)||
                           !candidate->load_effect_bank(effects)||!candidate->play()||!candidate->restore_checkpoint(*checkpoint)||
                           candidate->status().measure!=start_measure)return false;
                        data.engine=std::move(*candidate);
                        return true;
                    };
                    data.checkpoint_restored=!blob.empty()&&restore(blob);
#ifdef SOEW_PRIVATE_CHECKPOINT_CATALOG
                    if(!data.checkpoint_restored)data.checkpoint_restored=try_catalog_checkpoint(start_measure,false,restore);
#endif
                }
            }
#endif
            while(data.engine->status().measure<start_measure){if(cancelled&&cancelled->load(std::memory_order_relaxed)){error="Audio preparation cancelled";return prepared;}const auto next=std::uint16_t(data.engine->status().measure+1);const auto positioned=data.engine->advance_to_measure(next,maximum_fm_samples);if(!positioned){error="FM measure seek failed code="+std::to_string(unsigned(positioned.error().code));return prepared;}}
            const auto state=data.engine->status();data.positioned_measure=state.measure;data.positioned_ticks=state.timer_b_count;
        }
    }
    // Private checkpoints store the reference levels; apply the player's
    // levels after positioning and before the first submitted PCM buffer.
    if(!apply_levels(data,levels,error))return prepared;
    for(auto &slot:data.slots){
        if(cancelled&&cancelled->load(std::memory_order_relaxed)){error="Audio preparation cancelled";return prepared;}
        if(!fill(data,slot,error))return prepared;
    }
    return prepared;
}
bool AudioPreview::start(std::span<const std::byte> song,bool silent_device_test,UINT device,std::span<const std::byte> soundfont,std::span<const std::byte> effects,std::span<const std::byte> fm_carrier,bool music_off,unsigned start_measure,bool mute_fm_bgm,soew::AudioLevels levels){
    if(!stop())return false;
    return publish(prepare(song,silent_device_test,soundfont,effects,fm_carrier,music_off,start_measure,nullptr,true,mute_fm_bgm,levels),device);
}
bool AudioPreview::apply_levels(Data& data,soew::AudioLevels levels,std::string& error){
    if(data.engine&&!data.engine->set_user_volumes(data.fm_music?levels.music:0,data.effects?levels.effects:0)){
        error="PMD volume rejected";return false;
    }
    if(data.midi&&!data.midi->set_user_volume(data.midi_music?levels.music:0)){
        error="MIDI volume rejected";return false;
    }
    return true;
}
bool AudioPreview::publish(Prepared&& prepared,UINT device,bool initially_paused,bool reuse_output){
    // Only the fixed-format output handle survives a retry, never synth/PCM state.
    if(!prepared){error_=prepared.error().empty()?"Audio preparation unavailable":prepared.error();return false;}
    if(reuse_output&&device_&&device_id_==device){if(!release_buffers())return false;}
    else if(!stop())return false;
    ++generation_;
    data_=std::move(prepared.data_);error_.clear();submitted_=0;
    if(!device_){
        event_=CreateEventW(nullptr,FALSE,FALSE,nullptr);if(!event_){error_="Audio event failed";stop();return false;}
        WAVEFORMATEX format{};format.wFormatTag=WAVE_FORMAT_PCM;format.nChannels=2;format.nSamplesPerSec=48000;format.wBitsPerSample=16;format.nBlockAlign=4;format.nAvgBytesPerSec=192000;
        const auto result=waveOutOpen(&device_,device,&format,reinterpret_cast<DWORD_PTR>(event_),0,CALLBACK_EVENT);
        if(result!=MMSYSERR_NOERROR){device_=nullptr;fail("waveOutOpen",result);stop();return false;}
        device_id_=device;
    }
    if(waveOutPause(device_)!=MMSYSERR_NOERROR){error_="Initial audio pause failed";stop();return false;}
    for(auto &slot:data_->slots){slot.header={};slot.header.lpData=reinterpret_cast<LPSTR>(slot.pcm.data());slot.header.dwBufferLength=static_cast<DWORD>(slot.pcm.size()*sizeof(std::int16_t));
        const auto code=waveOutPrepareHeader(device_,&slot.header,sizeof(WAVEHDR));if(code){fail("Prepare",code);stop();return false;}slot.prepared=true;
    }
    paused_=false;
    for(auto &slot:data_->slots){
        const auto code=waveOutWrite(device_,&slot.header,sizeof(WAVEHDR));if(code){fail("Write",code);stop();return false;}submitted_+=frames;slot.end_frame=submitted_;
    }
    if(initially_paused){paused_=true;}else{const auto restart=waveOutRestart(device_);if(restart){fail("Restart",restart);stop();return false;}}
    return true;
}
bool AudioPreview::fill(Data &data,Slot &slot,std::string &error){
    slot.measure_before_fill=data.midi?static_cast<unsigned>(data.midi->status().elapsed_ticks/mmd_ticks_per_measure):
        data.engine?data.engine->status().measure:0;
    std::array<PmdCanonicalStereoSample,frames> output{};std::size_t done=0;
    while(data.engine&&done<frames){
        if(data.native_at==data.native_size){data.engine->render_native(data.native);data.native_at=0;data.native_size=data.native.size();}
        const auto progress=data.resampler.process(std::span(data.native).subspan(data.native_at,data.native_size-data.native_at),std::span(output).subspan(done));
        if(!progress||(!progress->input_consumed&&!progress->output_written)){error="Audio resampler stalled";return false;}
        data.native_at+=progress->input_consumed;done+=progress->output_written;
    }
    if(data.midi_stream){
        std::array<PmdCanonicalStereoSample,frames> music{};
        if(!data.midi_stream->render(music)){error="MIDI mix failed";return false;}
        const auto mix=[](std::int32_t fm,std::int32_t midi){
            return static_cast<std::int32_t>(std::clamp<std::int64_t>(std::int64_t(fm)+std::int64_t(double(midi)*soew::midi_output_trim),INT32_MIN,INT32_MAX));
        };
        for(std::size_t i=0;i<frames;++i)output[i]={mix(output[i].left,music[i].left),mix(output[i].right,music[i].right)};
    }
    // Diagnostic hash before test-only silence, so routing tests inspect real synthesis.
    for(const auto &frame:output)for(auto sample:{frame.left,frame.right})for(unsigned shift=0;shift<32;shift+=8){data.pcm_hash^=(static_cast<std::uint32_t>(sample)>>shift)&255;data.pcm_hash*=16777619u;}
    for(std::size_t i=0;i<frames;++i){slot.pcm[2*i]=data.silent?0:static_cast<std::int16_t>(std::clamp<std::int64_t>(std::int64_t(output[i].left)/65536,-32768,32767));slot.pcm[2*i+1]=data.silent?0:static_cast<std::int16_t>(std::clamp<std::int64_t>(std::int64_t(output[i].right)/65536,-32768,32767));}
    return true;
}
bool AudioPreview::effect(unsigned id,bool force){
    if(!active()||!data_->engine||id>=127)return false;
    if(force)data_->engine->stop_effect();
    if(!data_->engine->play_effect(std::uint8_t(id))){error_="EFC effect rejected";return false;}
    ++effect_calls_;return true;
}
bool AudioPreview::mute_music(){if(data_->engine){if(!data_->engine->set_user_volumes(0,25))return false;data_->engine->stop();data_->engine->stop_effect();}if(data_->midi)data_->midi->stop();return true;}
unsigned AudioPreview::queued()const{unsigned count=0;for(const auto &slot:data_->slots)count+=(slot.header.dwFlags&WHDR_INQUEUE)!=0;return count;}
bool AudioPreview::pump(){
    if(!active()||paused_)return true;
    // Retire in submission order before refilling a slot. A later completion
    // cannot advance past an older buffer that the device still owns.
    for(;;){
        const auto next=std::find_if(data_->slots.begin(),data_->slots.end(),[&](const Slot &slot){
            return slot.end_frame==completed_+frames;
        });
        if(next==data_->slots.end()||(next->header.dwFlags&(WHDR_INQUEUE|WHDR_DONE))!=WHDR_DONE)break;
        completed_=next->end_frame;
        // One completed 20 ms slot exceeds the synthesizers' prior lookahead:
        // 1024 native FM samples plus FIR history (<7 ms), or 256 MIDI frames.
        if(!completed_measure_||next->measure_before_fill>*completed_measure_)
            completed_measure_=next->measure_before_fill;
    }
    for(auto &slot:data_->slots)if(slot.end_frame&&slot.end_frame<=completed_&&!(slot.header.dwFlags&WHDR_INQUEUE)){
        if(!fill(*data_,slot,error_))return false;
        const auto code=waveOutWrite(device_,&slot.header,sizeof(WAVEHDR));if(code)return fail("Write",code);
        submitted_+=frames;slot.end_frame=submitted_;
    }
    return true;
}
bool AudioPreview::pause(bool paused){
    if(!active()||paused_==paused)return true;
    const auto code=paused?waveOutPause(device_):waveOutRestart(device_);if(code)return fail("Pause/restart",code);paused_=paused;return true;
}
bool AudioPreview::release_buffers(){
    // Reset also returns unplayed buffers as DONE. Invalidate the watermark
    // and their identities first, including when reset or unprepare fails.
    completed_=0;completed_measure_.reset();
    for(auto &slot:data_->slots)slot.end_frame=0;
    const auto reset=waveOutReset(device_);if(reset)return fail("Reset",reset);
    for(auto &slot:data_->slots)if(slot.prepared){const auto code=waveOutUnprepareHeader(device_,&slot.header,sizeof(WAVEHDR));if(code)return fail("Unprepare",code);slot.prepared=false;}
    return true;
}
bool AudioPreview::stop(){
    if(device_){if(!release_buffers())return false;
        const auto closed=waveOutClose(device_);if(closed)return fail("Close",closed);device_=nullptr;
    }
    if(event_){CloseHandle(event_);event_=nullptr;}data_->midi_stream.reset();data_->midi.reset();data_->engine.reset();paused_=false;data_->native_at=data_->native_size=0;data_->resampler.reset();return true;
}
