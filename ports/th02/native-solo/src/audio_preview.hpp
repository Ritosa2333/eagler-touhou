#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <mmsystem.h>
#include "th03/audio/pmd_opna.hpp"
#include "th03/audio/fixed_resampler.hpp"
#include "mmd_stream.hpp"
#include "audio_levels.hpp"
#include <array>
#include <memory>
#include <span>
#include <string>
#include <optional>
#include <atomic>

// WinMM is UI-thread-owned. Preparation has exclusive synth ownership and may
// run on a worker; publishing transfers that ownership without moving engines.
class AudioPreview {
    static constexpr std::size_t frames=960;
    struct Slot {
        WAVEHDR header{};
        std::array<std::int16_t,frames*2> pcm{};
        std::uint64_t end_frame=0;
        unsigned measure_before_fill=0;
        bool prepared=false;
    };
    struct Data {
        std::array<Slot,4> slots{};
        std::optional<th03::audio::PmdOpnaEngine> engine;
        std::optional<th03::audio::MmdSoundfontEngine> midi;
        std::unique_ptr<soew::MmdStream> midi_stream;
        th03::audio::PmdFixedResampler resampler;
        std::array<th03::audio::PmdOpnaStereoSample,1024> native{};
        std::size_t native_at=0,native_size=0;
        std::uint32_t song_hash=2166136261u,pcm_hash=2166136261u;
        unsigned requested_measure=0,positioned_measure=0;
        std::uint64_t positioned_ticks=0;
        bool silent=false, checkpoint_restored=false;
        bool fm_music=false, midi_music=false, effects=false;
    };
public:
    class Prepared {
    public:
        Prepared()=default;
        Prepared(Prepared&&) noexcept=default;
        Prepared& operator=(Prepared&&) noexcept=default;
        Prepared(const Prepared&)=delete;
        Prepared& operator=(const Prepared&)=delete;
        explicit operator bool()const{return data_!=nullptr&&error_.empty();}
        const std::string& error()const{return error_;}
        unsigned positioned_measure()const{return data_?data_->positioned_measure:0;}
        std::uint64_t positioned_ticks()const{return data_?data_->positioned_ticks:0;}
        bool checkpoint_restored()const{return data_&&data_->checkpoint_restored;}
        // Testable before submitting to a device; compares the actual prefill buffers.
        bool same_prefill(const Prepared &other)const{
            if(!*this||!other||data_->pcm_hash!=other.data_->pcm_hash)return false;
            for(std::size_t i=0;i<data_->slots.size();++i)
                if(data_->slots[i].pcm!=other.data_->slots[i].pcm)return false;
            return true;
        }
    private:
        friend class AudioPreview;
        std::unique_ptr<Data> data_;
        std::string error_;
    };
    AudioPreview()=default;
    ~AudioPreview();
    AudioPreview(const AudioPreview&)=delete;
    AudioPreview& operator=(const AudioPreview&)=delete;
    // Input spans must remain valid until prepare returns. No device is opened.
    static Prepared prepare(std::span<const std::byte> song,bool silent_device_test=false,std::span<const std::byte> soundfont={},std::span<const std::byte> effects={},std::span<const std::byte> fm_carrier={},bool music_off=false,unsigned start_measure=0,const std::atomic_bool* cancelled=nullptr,bool allow_checkpoint=true,bool mute_fm_bgm=false,soew::AudioLevels levels={});
    bool publish(Prepared&& prepared,UINT device=WAVE_MAPPER,bool initially_paused=false,bool reuse_output=false);
    bool start(std::span<const std::byte> song,bool silent_device_test=false,UINT device=WAVE_MAPPER,std::span<const std::byte> soundfont={},std::span<const std::byte> effects={},std::span<const std::byte> fm_carrier={},bool music_off=false,unsigned start_measure=0,bool mute_fm_bgm=false,soew::AudioLevels levels={});
    bool set_levels(soew::AudioLevels levels){return apply_levels(*data_,levels,error_);}
    bool effect(unsigned id,bool force=false);
    bool mute_music();
    unsigned effect_calls()const{return effect_calls_;}
    unsigned requested_measure()const{return data_->requested_measure;}
    unsigned positioned_measure()const{return data_->positioned_measure;}
    std::uint64_t positioned_ticks()const{return data_->positioned_ticks;}
    bool pump();
    bool pause(bool paused);
    bool stop();
    bool active()const{return device_!=nullptr&&(data_->engine.has_value()||data_->midi.has_value());}
    bool paused()const{return paused_;}
    std::uint64_t submitted()const{return submitted_;}
    // UI-thread watermark from completed buffers, not synthesized/submitted PCM.
    std::uint64_t completed_frames()const{return completed_;}
    std::optional<unsigned> completed_measure()const{return completed_measure_;}
    std::uint64_t playback_generation()const{return generation_;}
    bool checkpoint_restored()const{return data_->checkpoint_restored;}
    unsigned queued()const;
    std::uint32_t song_hash()const{return data_->song_hash;}
    std::uint32_t pcm_hash()const{return data_->pcm_hash;}
    const std::string &error()const{return error_;}
    HANDLE event()const{return event_;}
private:
    HWAVEOUT device_=nullptr;
    HANDLE event_=nullptr;
    std::unique_ptr<Data> data_=std::make_unique<Data>();
    std::uint64_t submitted_=0,completed_=0,generation_=0;
    std::optional<unsigned> completed_measure_;
    unsigned effect_calls_=0;
    bool paused_=false;
    std::string error_;
    UINT device_id_=WAVE_MAPPER;
    bool release_buffers();
    bool fail(const char *operation,MMRESULT code);
    static bool fill(Data& data,Slot& slot,std::string& error);
    static bool apply_levels(Data& data,soew::AudioLevels levels,std::string& error);
};
