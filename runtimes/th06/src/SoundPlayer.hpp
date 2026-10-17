#pragma once

#include "ZunResult.hpp"
#include "inttypes.hpp"
#include <SDL3/SDL.h>
#include <atomic>
#include <mutex>
#include <thread>

enum SoundIdx
{
    NO_SOUND = -1,
    SOUND_SHOOT = 0,
    SOUND_1 = 1,
    SOUND_2 = 2,
    SOUND_3 = 3,
    SOUND_PICHUN = 4,
    SOUND_5 = 5,
    SOUND_BOMB_REIMARI = 6,
    SOUND_7 = 7,
    SOUND_8 = 8,
    SOUND_SHOOT_BOSS = 9,
    SOUND_SELECT = 10,
    SOUND_BACK = 11,
    SOUND_MOVE_MENU = 12,
    SOUND_BOMB_REIMU_A = 13,
    SOUND_BOMB = 14,
    SOUND_F = 15,
    SOUND_BOSS_LASER = 16,
    SOUND_BOSS_LASER_2 = 17,
    SOUND_12 = 18,
    SOUND_BOMB_MARISA_B = 19,
    SOUND_TOTAL_BOSS_DEATH = 20,
    SOUND_15 = 21,
    SOUND_16 = 22,
    SOUND_17 = 23,
    SOUND_18 = 24,
    SOUND_WTF_IS_THAT_LMAO = 25,
    SOUND_1A = 26,
    SOUND_1B = 27,
    SOUND_1UP = 28,
    SOUND_1D = 29,
    SOUND_GRAZE = 30,
    SOUND_POWERUP = 31,
};

struct SoundBufferIdxVolume
{
    i32 bufferIdx;
    i16 volume;
};

struct SoundData
{
    i16 *samples;
    u32 pos;
    u32 len;
    bool isPlaying;
};

struct WavData
{
    SDL_IOStream *fileStream;
    i16 *ownedSamples;
    u32 dataStartOffset;
    u32 samples;
#ifdef __EMSCRIPTEN__
    void *oggDecoder;
#endif
};

struct MusicStream
{
    WavData srcWav;
    u32 pos;
    u32 loopStart;
    u32 loopEnd;
    u32 fadeoutLen;
    u32 fadeoutProgress;
};

struct SoundPlayer
{
    SoundPlayer();

    ZunResult InitializeDSound();
    ZunResult InitSoundBuffers();
    ZunResult Release(void);

    ZunResult LoadSound(i32 idx, const char *path, f32 volumeMultiplier);
    void PlaySounds();
    void PlaySoundByIdx(SoundIdx idx);
    ZunResult PlayBGM(bool isLooping);
    void StopBGM();
    void FadeOut(f32 seconds);

    ZunResult LoadWav(const char *path);
    ZunResult LoadPos(const char *path);

    void BackgroundMusicPlayerThread();
    bool MixAudio(u32 samples);
#ifdef __EMSCRIPTEN__
    void SetWebAudioWindowActive(bool active);
    void SetWebAudioBgmTransition(bool active);
    bool PumpWebAudio();
#endif

    SoundData soundBuffers[128];
    std::mutex soundBufMutex;
    SDL_AudioDeviceID audioDev;
    SDL_AudioStream *audioStream;
    std::thread backgroundMusicThreadHandle;
    std::atomic_bool terminateFlag;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    i32 soundBuffersToPlay[5];
#else
    i32 soundBuffersToPlay[3];
#endif
    MusicStream backgroundMusic;
    bool isLooping;
#ifdef __EMSCRIPTEN__
    void UpdateWebAudioPlaybackState();
    bool webAudioWindowActive;
    bool webAudioBgmTransition;
    bool webAudioPlaybackSuspended;
    bool webAudioRefilling;
    f64 webAudioLastDiagnosticMs;
    u32 webAudioMinQueuedFrames;
#endif
};

extern SoundPlayer g_SoundPlayer;
