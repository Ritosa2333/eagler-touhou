#include "Sound.hpp"

namespace th16 {

void SoundEffects::reset() {
    queue_voice.fill(-1);
    queue_count.fill(0);
    for (auto& slot : queue_pan) slot.fill(0);
    for (i32 v = 0; v < kSoundVoiceCount; ++v) {
        Voice& voice = voices[size_t(v)];
        voice = Voice{};
        for (const SoundDefinition& row : kSoundDefinitions) {
            if (row.voice == v) {
                voice.definition = &row;
                break;
            }
        }
    }
}

void SoundRequests::push(i32 id, float x) {
    const i32 pan = SoundEffects::pan_for_x(x);
    ids.push_back(id);
    pans.push_back(pan);
    if (effects != nullptr) effects->play_pan(id, pan);
}

void SoundRequests::stop(i32 id) {
    if (effects != nullptr) effects->stop(id);
}

i32 SoundEffects::pan_for_x(float x) {
    return i32(float(x * 1000.0f) / 192.0f);
}

void SoundEffects::enqueue(i32 voice, i32 pan) {
    if (voice < 0 || voice >= kSoundVoiceCount) return;
    // 续期按请求号直接取行 +0xa（不经声部绑定）
    const i16 renew = kSoundDefinitions[size_t(voice)].renew;
    for (i32 i = 0; i < kSoundQueueSlots; ++i) {
        if (queue_voice[size_t(i)] < 0) {
            queue_voice[size_t(i)] = voice;
            queue_pan[size_t(i)][0] = pan;
            queue_count[size_t(i)] = 1;
            voices[size_t(voice)].renew = renew;
            return;
        }
        if (queue_voice[size_t(i)] == voice) {
            i32& count = queue_count[size_t(i)];
            if (count >= kSoundQueueDepth || count < 0) return;
            queue_pan[size_t(i)][size_t(count)] = pan;
            ++count;
            return;
        }
    }
}

void SoundEffects::play(i32 voice) { enqueue(voice, 0); }

void SoundEffects::play_at(i32 voice, float x) { enqueue(voice, pan_for_x(x)); }

void SoundEffects::halt(i32 voice) {
    Voice& v = voices[size_t(voice)];
    v.resume = false;
    if (output == nullptr || !output->available(voice)) return;
    v.resume = output->playing(voice);
    output->stop(voice);
}

void SoundEffects::stop(i32 voice) {
    if (voice < 0) {
        for (i32 v = 0; v < kSoundVoiceCount; ++v) halt(v);
        return;
    }
    for (i32 i = 0; i < kSoundQueueSlots; ++i) {
        if (queue_voice[size_t(i)] < 0) queue_voice[size_t(i)] = voice;
        else if (queue_voice[size_t(i)] != voice) continue;
        queue_count[size_t(i)] = -1;
        return;
    }
}

i32 SoundEffects::voice_volume(i16 volume, i32 master) {
    if (master == 0) return -10000;
    const float inverse = 1.0f - float(master) / 100.0f;
    const float gain = 1.0f - inverse * inverse * inverse;
    return i32(gain * float(i32(volume) + 5000)) - 5000;
}

void SoundEffects::restart(i32 voice, i32 pan) {
    if (output == nullptr || !output->available(voice)) return;
    Voice& v = voices[size_t(voice)];
    output->stop(voice);
    output->set_position(voice, 0);
    output->set_pan(voice, pan);
    v.pan = pan;
    output->set_volume(voice, voice_volume(v.definition != nullptr ? v.definition->volume : 0, master_volume));
    output->play(voice, v.definition != nullptr ? v.definition->flags : 0);
}

void SoundEffects::process() {
    if (!enabled) return;
    for (i32 i = 0; i < kSoundQueueSlots; ++i) {
        const i32 voice = queue_voice[size_t(i)];
        if (voice < 0) break;
        const i32 count = queue_count[size_t(i)];
        queue_voice[size_t(i)] = -1;
        queue_count[size_t(i)] = 0;
        if (count < 0) {
            halt(voice);
            continue;
        }
        i32 sum = 0;
        for (i32 k = 0; k < count; ++k) sum = i32(u32(sum) + u32(queue_pan[size_t(i)][size_t(k)]));
        restart(voice, count > 0 ? sum / count : 0);
    }
}

void SoundEffects::suspend() {
    queue_voice[0] = -1;
    for (i32 v = 0; v < kSoundVoiceCount; ++v) halt(v);
}

void SoundEffects::resume() {
    if (output == nullptr) return;
    for (i32 v = 0; v < kSoundVoiceCount; ++v) {
        const Voice& voice = voices[size_t(v)];
        if (output->available(v) && voice.resume) output->play(v, voice.definition != nullptr ? voice.definition->flags : 0);
    }
}

}  // namespace th16
