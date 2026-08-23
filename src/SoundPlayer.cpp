#include "SoundPlayer.hpp"

#include "FileSystem.hpp"
#include "Supervisor.hpp"
#include "i18n.hpp"
#include "utils.hpp"

#include <SDL3/SDL.h>
#include <array>
#include <climits>
#include <cmath>
#include <cstring>
#include <new>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#define STB_VORBIS_HEADER_ONLY
#include "thirdparty/stb_vorbis.c"

// This would all be a lot easier with SDL_mixer, but SDL_mixer doesn't permit any way of doing custom
//   loop points that would be accurate to the sample like EoSD needs. So instead we get to read WAVs and
//   mix everything by hand. Yay

#define BACKGROUND_MUSIC_WAV_NUM_CHANNELS 2
#define BACKGROUND_MUSIC_WAV_SAMPLE_RATE 44100
#define BACKGROUND_MUSIC_WAV_BITS_PER_SAMPLE 16
#define BACKGROUND_MUSIC_WAV_BLOCK_ALIGN (BACKGROUND_MUSIC_WAV_BITS_PER_SAMPLE / 8 * BACKGROUND_MUSIC_WAV_NUM_CHANNELS)
#define BACKGROUND_MUSIC_WAV_BYTE_RATE (BACKGROUND_MUSIC_WAV_BLOCK_ALIGN * BACKGROUND_MUSIC_WAV_SAMPLE_RATE)

// DirectSound deals with volume by subtracting a number measured in hundredths of decibels from the source sound.
//   The scale is from 0 (no volume modification) to -10,000 (subtraction of 100 decibels, and basically silent).
//   20 decibels affects wave amplitude by a factor of 10

static const SoundBufferIdxVolume g_SoundBufferIdxVol[32] = {
    {0, -1500}, {0, -2000}, {1, -1200}, {1, -1400}, {2, -1000},  {3, -500},   {4, -500},   {5, -1700},
    {6, -1700}, {7, -1700}, {8, -1000}, {9, -1000}, {10, -1900}, {11, -1200}, {12, -900},  {5, -1500},
    {13, -900}, {14, -900}, {15, -600}, {16, -400}, {17, -1100}, {18, -900},  {5, -1800},  {6, -1800},
    {7, -1800}, {19, -300}, {20, -600}, {21, -800}, {22, -100},  {23, -500},  {24, -1000}, {25, -1000},
};
static const char *const g_SFXList[26] = {
    "data/wav/plst00.wav", "data/wav/enep00.wav",   "data/wav/pldead00.wav", "data/wav/power0.wav",
    "data/wav/power1.wav", "data/wav/tan00.wav",    "data/wav/tan01.wav",    "data/wav/tan02.wav",
    "data/wav/ok00.wav",   "data/wav/cancel00.wav", "data/wav/select00.wav", "data/wav/gun00.wav",
    "data/wav/cat00.wav",  "data/wav/lazer00.wav",  "data/wav/lazer01.wav",  "data/wav/enep01.wav",
    "data/wav/nep00.wav",  "data/wav/damage00.wav", "data/wav/item00.wav",   "data/wav/kira00.wav",
    "data/wav/kira01.wav", "data/wav/kira02.wav",   "data/wav/extend.wav",   "data/wav/timeout.wav",
    "data/wav/graze.wav",  "data/wav/powerup.wav",
};
SoundPlayer g_SoundPlayer;

static bool HasBackgroundMusicSource(const MusicStream &music)
{
#ifdef __EMSCRIPTEN__
    return music.srcWav.fileStream != NULL || music.srcWav.oggDecoder != NULL;
#else
    return music.srcWav.fileStream != NULL;
#endif
}

static u16 ReadU16LE(SDL_IOStream *stream)
{
    u16 value = 0;
    return SDL_ReadIO(stream, &value, sizeof(value)) == sizeof(value) ? SDL_Swap16LE(value) : 0;
}

static u32 ReadU32LE(SDL_IOStream *stream)
{
    u32 value = 0;
    return SDL_ReadIO(stream, &value, sizeof(value)) == sizeof(value) ? SDL_Swap32LE(value) : 0;
}

SoundPlayer::SoundPlayer()
{
    for (SoundData &buffer : this->soundBuffers)
    {
        buffer = {};
    }
    std::fill_n(this->soundBuffersToPlay, ARRAY_SIZE(this->soundBuffersToPlay), -1);
    this->audioDev = 0;
    this->audioStream = NULL;
    this->terminateFlag.store(false, std::memory_order_relaxed);
    this->backgroundMusic = {};
    this->isLooping = false;
#ifdef __EMSCRIPTEN__
    this->webAudioWindowActive = true;
    this->webAudioBgmTransition = false;
    this->webAudioPlaybackSuspended = false;
    this->webAudioRefilling = false;
    this->webAudioLastDiagnosticMs = 0.0;
    this->webAudioMinQueuedFrames = UINT_MAX;
#endif
}

#ifdef __EMSCRIPTEN__
void SoundPlayer::UpdateWebAudioPlaybackState()
{
    const bool shouldSuspend = !this->webAudioWindowActive || this->webAudioBgmTransition;
    if (shouldSuspend == this->webAudioPlaybackSuspended)
    {
        return;
    }

    if (shouldSuspend)
    {
        EM_ASM({
            const sdl = Module['SDL3'];
            const playback = sdl && sdl.audio_playback;
            const node = playback && playback.scriptProcessorNode;
            if (node) {
                try { node.disconnect(); } catch (_) {}
            }
        });
        if (this->audioDev != 0 && !SDL_AudioDevicePaused(this->audioDev))
        {
            SDL_PauseAudioDevice(this->audioDev);
        }
    }
    else
    {
        if (this->audioDev != 0 && SDL_AudioDevicePaused(this->audioDev))
        {
            SDL_ResumeAudioDevice(this->audioDev);
        }
        EM_ASM({
            const sdl = Module['SDL3'];
            const playback = sdl && sdl.audio_playback;
            const node = playback && playback.scriptProcessorNode;
            if (node && sdl.audioContext) {
                try { node.connect(sdl.audioContext.destination); } catch (_) {}
            }
        });
    }
    this->webAudioPlaybackSuspended = shouldSuspend;
}

void SoundPlayer::SetWebAudioWindowActive(bool active)
{
    this->webAudioWindowActive = active;
    this->UpdateWebAudioPlaybackState();
}

void SoundPlayer::SetWebAudioBgmTransition(bool active)
{
    this->webAudioBgmTransition = active;
    this->UpdateWebAudioPlaybackState();
}
#endif

ZunResult SoundPlayer::InitializeDSound()
{
    SDL_AudioSpec desiredAudio{};
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
    {
        goto fail;
    }

    desiredAudio.freq = 44100;
    desiredAudio.format = SDL_AUDIO_S16;
    desiredAudio.channels = 2;
    this->audioDev = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desiredAudio);

    if (this->audioDev == 0)
    {
        goto fail;
    }

    this->audioStream = SDL_CreateAudioStream(&desiredAudio, &desiredAudio);
    if (!this->audioStream || !SDL_BindAudioStream(this->audioDev, this->audioStream))
    {
        goto fail;
    }
    SDL_ResumeAudioDevice(this->audioDev);

    g_GameErrorContext.Log(TH_DBG_SOUNDPLAYER_INIT_SUCCESS);
    return ZUN_SUCCESS;

fail:
    if (this->audioStream != NULL)
    {
        SDL_DestroyAudioStream(this->audioStream);
        this->audioStream = NULL;
    }
    if (this->audioDev != 0)
    {
        SDL_CloseAudioDevice(this->audioDev);
        this->audioDev = 0;
    }
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "th06: audio initialization failed: %s", SDL_GetError());
    g_GameErrorContext.Log(TH_ERR_SOUNDPLAYER_FAILED_TO_INITIALIZE_OBJECT);
    return ZUN_ERROR;
}

ZunResult SoundPlayer::Release(void)
{
    StopBGM();

    for (int i = 0; i < ARRAY_SIZE_SIGNED(this->soundBuffers); i++)
    {
        if (this->soundBuffers[i].samples != NULL)
        {
            delete[] this->soundBuffers[i].samples;
            this->soundBuffers[i].samples = NULL;
            this->soundBuffers[i].isPlaying = false;
        }
    }

    if (this->audioDev != 0)
    {
        SDL_DestroyAudioStream(this->audioStream);
        this->audioStream = NULL;
        SDL_CloseAudioDevice(this->audioDev);
        this->audioDev = 0;
    }

    return ZUN_SUCCESS;
}

void SoundPlayer::StopBGM()
{
#ifdef __EMSCRIPTEN__
    // StopBGM is the output-owner boundary even if a prior EOF/fade already
    // released the source. Always discard any queued mixed tail before a new
    // BGM can be connected.
    if (this->audioStream != NULL)
        SDL_ClearAudioStream(this->audioStream);
#endif
    if (HasBackgroundMusicSource(this->backgroundMusic))
    {
        this->soundBufMutex.lock();
        if (this->backgroundMusic.srcWav.fileStream != NULL)
        {
            SDL_CloseIO(this->backgroundMusic.srcWav.fileStream);
            this->backgroundMusic.srcWav.fileStream = NULL;
        }
#ifdef __EMSCRIPTEN__
        if (this->backgroundMusic.srcWav.oggDecoder != NULL)
        {
            stb_vorbis_close(static_cast<stb_vorbis *>(this->backgroundMusic.srcWav.oggDecoder));
            this->backgroundMusic.srcWav.oggDecoder = NULL;
        }
#endif
        free(this->backgroundMusic.srcWav.ownedSamples);
        this->backgroundMusic.srcWav.ownedSamples = NULL;
        this->soundBufMutex.unlock();

        utils::DebugPrint2("stop BGM\n");
    }
}

void SoundPlayer::FadeOut(f32 seconds)
{
    if (HasBackgroundMusicSource(this->backgroundMusic))
    {
        this->backgroundMusic.fadeoutLen = seconds * 44100;
        this->backgroundMusic.fadeoutProgress = 0;
    }
}

ZunResult SoundPlayer::LoadWav(const char *path)
{
    SDL_IOStream *fileStream;
    char idBuf[4];
    u32 riffSize;
    u32 wavDataSize;
    Sint64 dataStart;
    Sint64 fileSize;

    if (this->audioDev == 0)
    {
        return ZUN_ERROR;
    }

    if (g_Supervisor.cfg.playSounds == 0)
    {
        return ZUN_ERROR;
    }

#ifdef __EMSCRIPTEN__
    // SDL's Emscripten backend feeds WebAudio through a ScriptProcessorNode
    // callback that runs on the browser main thread. Merely pausing the SDL
    // logical device cannot silence the last already-rendered quantum while a
    // BGM source is being replaced. Cut the WebAudio graph first, and leave it
    // disconnected until PlayBGM owns a fully prepared replacement source.
    struct WebBgmTransitionGuard
    {
        SoundPlayer *owner;
        bool keepTransition = false;
        ~WebBgmTransitionGuard()
        {
            if (!keepTransition)
                owner->SetWebAudioBgmTransition(false);
        }
    } webBgmTransitionGuard{this};
    this->SetWebAudioBgmTransition(true);
#endif

    this->StopBGM();

    utils::DebugPrint2("load BGM\n");

    fileStream = FileSystem::OpenFileStream(path, "rb");

    if (fileStream == NULL)
    {
        char oggPath[512];
        if (std::strlen(path) >= sizeof(oggPath))
        {
            return ZUN_ERROR;
        }
        std::strcpy(oggPath, path);
        char *extension = std::strrchr(oggPath, '.');
        if (extension == NULL)
        {
            return ZUN_ERROR;
        }
        std::strcpy(extension, ".ogg");

#ifdef __EMSCRIPTEN__
        const std::string fullPath = FileSystem::GetBasePath(oggPath);
        int error = 0;
        stb_vorbis *decoder = stb_vorbis_open_filename(fullPath.c_str(), &error, NULL);
        if (decoder == NULL)
        {
            utils::DebugPrint2("error : BGM file load error %s / %s\n", path, oggPath);
            return ZUN_ERROR;
        }

        const stb_vorbis_info info = stb_vorbis_get_info(decoder);
        const unsigned int frames = stb_vorbis_stream_length_in_samples(decoder);
        if (frames == 0 || info.channels != BACKGROUND_MUSIC_WAV_NUM_CHANNELS ||
            info.sample_rate != BACKGROUND_MUSIC_WAV_SAMPLE_RATE)
        {
            stb_vorbis_close(decoder);
            utils::DebugPrint2("error : BGM file load error %s / %s\n", path, oggPath);
            return ZUN_ERROR;
        }

        // Keep compressed OGG open on Web and decode only the small PCM slices
        // requested by MixAudio(). Native keeps the existing whole-track path.
        this->backgroundMusic.srcWav.fileStream = NULL;
        this->backgroundMusic.srcWav.ownedSamples = NULL;
        this->backgroundMusic.srcWav.oggDecoder = decoder;
#else
        SDL_IOStream *oggStream = FileSystem::OpenFileStream(oggPath, "rb");
        if (oggStream == NULL)
        {
            utils::DebugPrint2("error : BGM file load error %s / %s\n", path, oggPath);
            return ZUN_ERROR;
        }

        const Sint64 oggSize = SDL_GetIOSize(oggStream);
        if (oggSize <= 0 || oggSize > INT_MAX)
        {
            SDL_CloseIO(oggStream);
            return ZUN_ERROR;
        }

        std::vector<unsigned char> encoded(static_cast<size_t>(oggSize));
        const bool readSucceeded = SDL_ReadIO(oggStream, encoded.data(), encoded.size()) == encoded.size();
        SDL_CloseIO(oggStream);
        if (!readSucceeded)
        {
            return ZUN_ERROR;
        }

        int channels = 0;
        int sampleRate = 0;
        short *decoded = NULL;
        const int frames = stb_vorbis_decode_memory(encoded.data(), static_cast<int>(encoded.size()), &channels,
                                                    &sampleRate, &decoded);
        if (frames <= 0 || decoded == NULL || channels != BACKGROUND_MUSIC_WAV_NUM_CHANNELS ||
            sampleRate != BACKGROUND_MUSIC_WAV_SAMPLE_RATE)
        {
            free(decoded);
            utils::DebugPrint2("error : BGM file load error %s / %s\n", path, oggPath);
            return ZUN_ERROR;
        }

        fileStream = SDL_IOFromConstMem(decoded, static_cast<size_t>(frames) * channels * sizeof(i16));
        if (fileStream == NULL)
        {
            free(decoded);
            return ZUN_ERROR;
        }
        this->backgroundMusic.srcWav.fileStream = fileStream;
        this->backgroundMusic.srcWav.ownedSamples = decoded;
#endif
        this->backgroundMusic.srcWav.dataStartOffset = 0;
        this->backgroundMusic.srcWav.samples = static_cast<u32>(frames);
        this->backgroundMusic.loopStart = 0;
        this->backgroundMusic.loopEnd = static_cast<u32>(frames);
        this->backgroundMusic.fadeoutLen = 0;
        this->backgroundMusic.fadeoutProgress = 0;
        this->backgroundMusic.pos = 0;
#ifdef __EMSCRIPTEN__
        webBgmTransitionGuard.keepTransition = true;
#endif
        return ZUN_SUCCESS;
    }

    // Minimum size of RIFF header and chunk info preceeding the sample data
    if (SDL_GetIOSize(fileStream) < 44)
    {
        goto fail;
    }

    if (SDL_ReadIO(fileStream, idBuf, 4) != 4 || std::strncmp(idBuf, "RIFF", 4) != 0)
    {
        goto fail;
    }

    riffSize = ReadU32LE(fileStream);

    // Same bounds check done earlier on the total filesize
    if (riffSize < 36 || riffSize > SDL_GetIOSize(fileStream) - 8)
    {
        goto fail;
    }

    if (SDL_ReadIO(fileStream, idBuf, 4) != 4 || std::strncmp(idBuf, "WAVE", 4) != 0)
    {
        goto fail;
    }

    // Checks here are quite a bit less flexible than what WAV can represent. EoSD uses 44.1 kHz, stereo, 16-bit PCM
    //   so that's what we handle. We also assume that fmt and data are the only subchunks, which is definitely not
    //   a general guarantee, but it'll work fine with EoSD's WAV files.

    if (SDL_ReadIO(fileStream, idBuf, 4) != 4 || std::strncmp(idBuf, "fmt ", 4) != 0)
    {
        goto fail;
    }

    // Format subchunk size. Guaranteed 16 for PCM data
    if (ReadU32LE(fileStream) != 16)
    {
        goto fail;
    }

    // Audio format. 1 represents raw PCM samples
    if (ReadU16LE(fileStream) != 1)
    {
        goto fail;
    }

    // Number of channels. We expect stereo
    if (ReadU16LE(fileStream) != BACKGROUND_MUSIC_WAV_NUM_CHANNELS)
    {
        goto fail;
    }

    // Sample frequency rate
    if (ReadU32LE(fileStream) != BACKGROUND_MUSIC_WAV_SAMPLE_RATE)
    {
        goto fail;
    }

    // Byte rate
    if (ReadU32LE(fileStream) != BACKGROUND_MUSIC_WAV_BYTE_RATE)
    {
        goto fail;
    }

    // Block alignment
    if (ReadU16LE(fileStream) != BACKGROUND_MUSIC_WAV_BLOCK_ALIGN)
    {
        goto fail;
    }

    // Bits per sample
    if (ReadU16LE(fileStream) != BACKGROUND_MUSIC_WAV_BITS_PER_SAMPLE)
    {
        goto fail;
    }

    if (SDL_ReadIO(fileStream, idBuf, 4) != 4 || std::strncmp(idBuf, "data", 4) != 0)
    {
        goto fail;
    }

    wavDataSize = ReadU32LE(fileStream);

    dataStart = SDL_TellIO(fileStream);
    fileSize = SDL_GetIOSize(fileStream);
    if (wavDataSize > riffSize - 36 || dataStart < 0 || fileSize < dataStart ||
        static_cast<Uint64>(wavDataSize) > static_cast<Uint64>(fileSize - dataStart) ||
        wavDataSize % BACKGROUND_MUSIC_WAV_BLOCK_ALIGN != 0)
    {
        goto fail;
    }

    this->backgroundMusic.srcWav.samples = wavDataSize / BACKGROUND_MUSIC_WAV_BLOCK_ALIGN;

    if (this->backgroundMusic.srcWav.samples == 0)
    {
        goto fail;
    }

    this->backgroundMusic.srcWav.fileStream = fileStream;
    this->backgroundMusic.srcWav.ownedSamples = NULL;
    this->backgroundMusic.srcWav.dataStartOffset = static_cast<u32>(dataStart);
    this->backgroundMusic.loopStart = 0;
    this->backgroundMusic.loopEnd = this->backgroundMusic.srcWav.samples;
    this->backgroundMusic.fadeoutLen = 0;
    this->backgroundMusic.fadeoutProgress = 0;
    this->backgroundMusic.pos = 0;

#ifdef __EMSCRIPTEN__
    webBgmTransitionGuard.keepTransition = true;
#endif
    return ZUN_SUCCESS;

fail:
    SDL_CloseIO(fileStream);
    return ZUN_ERROR;
}

ZunResult SoundPlayer::LoadPos(const char *path)
{
    u8 *fileData;

    if (this->audioDev == 0 || g_Supervisor.cfg.playSounds == 0 ||
        !HasBackgroundMusicSource(this->backgroundMusic))
    {
        return ZUN_ERROR;
    }

    fileData = FileSystem::OpenPath(path, 0);

    if (fileData == NULL || g_LastFileSize < 8)
    {
        free(fileData);
        return ZUN_ERROR;
    }

    u32 loopStart = 0;
    u32 loopEnd = 0;
    std::memcpy(&loopStart, fileData, sizeof(loopStart));
    std::memcpy(&loopEnd, fileData + sizeof(loopStart), sizeof(loopEnd));
    this->backgroundMusic.loopStart = SDL_Swap32LE(loopStart);
    this->backgroundMusic.loopEnd = SDL_Swap32LE(loopEnd);

    free(fileData);

    if (this->backgroundMusic.loopStart >= this->backgroundMusic.loopEnd ||
        this->backgroundMusic.loopEnd > this->backgroundMusic.srcWav.samples)
    {
        this->backgroundMusic.loopStart = 0;
        this->backgroundMusic.loopEnd = this->backgroundMusic.srcWav.samples;

        return ZUN_ERROR;
    }

    return ZUN_SUCCESS;
}

ZunResult SoundPlayer::InitSoundBuffers()
{
    if (this->audioDev == 0)
    {
        return ZUN_ERROR;
    }

    std::fill_n(this->soundBuffersToPlay, ARRAY_SIZE(this->soundBuffersToPlay), -1);

    for (int idx = 0; idx < ARRAY_SIZE_SIGNED(g_SoundBufferIdxVol); idx++)
    {
        if (this->LoadSound(idx, g_SFXList[g_SoundBufferIdxVol[idx].bufferIdx],
                            1.0f / ZUN_POWF(10.0f, (float)g_SoundBufferIdxVol[idx].volume / -2000)) != ZUN_SUCCESS)
        {
            g_GameErrorContext.Log(TH_ERR_SOUNDPLAYER_FAILED_TO_LOAD_SOUND_FILE, g_SFXList[idx]);
            return ZUN_ERROR;
        }

        this->soundBuffers[idx].isPlaying = false;
        this->soundBuffers[idx].pos = 0;
    }

    return ZUN_SUCCESS;
}

ZunResult SoundPlayer::LoadSound(i32 idx, const char *path, f32 volumeMultiplier)
{
    SDL_AudioSpec wavFormat;
    SDL_AudioSpec targetFormat = {SDL_AUDIO_S16, 1, 44100};
    u8 *wavRawData;
    u8 *wavRawSamples;
    u32 wavRawSampleByteCount;
    u8 *convertedSamples = NULL;
    int convertedByteCount = 0;
    SDL_IOStream *wavIo = NULL;

    if (idx < 0 || idx >= ARRAY_SIZE_SIGNED(this->soundBuffers) || path == NULL)
    {
        return ZUN_ERROR;
    }

    soundBufMutex.lock();

    if (this->soundBuffers[idx].samples != NULL)
    {
        delete[] this->soundBuffers[idx].samples;
        this->soundBuffers[idx].samples = NULL;
    }

    wavRawData = (u8 *)FileSystem::OpenPath(path, 0);

    if (wavRawData == NULL)
    {
        goto fail;
    }

    wavIo = SDL_IOFromConstMem(wavRawData, g_LastFileSize);
    if (wavIo == NULL || !SDL_LoadWAV_IO(wavIo, true, &wavFormat, &wavRawSamples, &wavRawSampleByteCount))
    {
        std::free(wavRawData);
        wavRawData = NULL;
        g_GameErrorContext.Log(TH_ERR_NOT_A_WAV_FILE, path);
        goto fail;
    }
    std::free(wavRawData);
    wavRawData = NULL;

    // EoSD's sound files are all 22050 Hz, and some even use 8-bit samples. Converting them
    //   here only uses a few hundred extra kilobytes of RAM compared to the original code,
    //   but it might be worth looking into avoiding it for especially RAM-limited systems

    if (SDL_ConvertAudioSamples(&wavFormat, wavRawSamples, wavRawSampleByteCount, &targetFormat,
                                &convertedSamples, &convertedByteCount))
    {
        this->soundBuffers[idx].len = convertedByteCount / 2;
        this->soundBuffers[idx].samples = new (std::nothrow) i16[this->soundBuffers[idx].len];
        if (this->soundBuffers[idx].samples == NULL)
        {
            SDL_free(convertedSamples);
            SDL_free(wavRawSamples);
            goto fail;
        }
        std::memcpy(this->soundBuffers[idx].samples, convertedSamples, convertedByteCount);
        SDL_free(convertedSamples);
    }
    else
    {
        SDL_free(wavRawSamples);
        goto fail;
    }

    SDL_free(wavRawSamples);

    for (u32 i = 0; i < this->soundBuffers[idx].len; i++)
    {
        this->soundBuffers[idx].samples[i] *= volumeMultiplier;
    }

    this->soundBuffers[idx].pos = 0;
    this->soundBuffers[idx].isPlaying = false;

    soundBufMutex.unlock();

    return ZUN_SUCCESS;

fail:
    soundBufMutex.unlock();
    return ZUN_ERROR;
}

ZunResult SoundPlayer::PlayBGM(bool isLooping)
{
    utils::DebugPrint2("play BGM\n");

    if (!HasBackgroundMusicSource(this->backgroundMusic))
    {
#ifdef __EMSCRIPTEN__
        this->SetWebAudioBgmTransition(false);
#endif
        return ZUN_ERROR;
    }

    //    res = this->backgroundMusic->Reset();
    //    if (FAILED(res))
    //    {
    //        return ZUN_ERROR;
    //    }
    //
    //    buffer = this->backgroundMusic->GetBuffer(0);
    //    res = this->backgroundMusic->FillBufferWithSound(buffer, isLooping);
    //    if (FAILED(res))
    //    {
    //        return ZUN_ERROR;
    //    }
    //    res = this->backgroundMusic->Play(0, DSBPLAY_LOOPING);
    //    if (FAILED(res))
    //    {
    //        return ZUN_ERROR;
    //    }
    utils::DebugPrint2("comp\n");
    this->isLooping = isLooping;
#ifdef __EMSCRIPTEN__
    this->SetWebAudioBgmTransition(false);
#endif
    return ZUN_SUCCESS;
}

void SoundPlayer::PlaySounds()
{
    i32 idx;
    i32 sndBufIdx;

    if (this->audioDev == 0 || !g_Supervisor.cfg.playSounds)
    {
        return;
    }

    soundBufMutex.lock();

    for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->soundBuffersToPlay); idx++)
    {
        if (this->soundBuffersToPlay[idx] < 0)
        {
            break;
        }

        sndBufIdx = this->soundBuffersToPlay[idx];
        this->soundBuffersToPlay[idx] = -1;

        if (sndBufIdx < 0 || sndBufIdx >= ARRAY_SIZE_SIGNED(this->soundBuffers) ||
            this->soundBuffers[sndBufIdx].samples == NULL)
        {
            continue;
        }

        this->soundBuffers[sndBufIdx].pos = 0;
        this->soundBuffers[sndBufIdx].isPlaying = true;
    }

    soundBufMutex.unlock();

#ifndef __EMSCRIPTEN__
    while (SDL_GetAudioStreamQueued(this->audioStream) < 8192)
    {
        // If the stream can't accept data (device gone, stream unbound, ...),
        // SDL_PutAudioStreamData fails and the queue never fills; bail out
        // instead of spinning forever.
        if (!this->MixAudio(2048))
        {
            break;
        }
    }
#endif
}

#ifdef __EMSCRIPTEN__
bool SoundPlayer::PumpWebAudio()
{
    if (this->audioDev == 0 || !g_Supervisor.cfg.playSounds || this->audioStream == NULL ||
        this->webAudioPlaybackSuspended)
    {
        return true;
    }

    // Keep the verified low-latency streaming envelope. This function only
    // replenishes the SDL stream; playback timing remains owned by SDL/WebAudio.
    constexpr u32 FRAMES_PER_CHUNK = 1024;
    constexpr u32 LOW_WATER_FRAMES = 2048;
    constexpr u32 HIGH_WATER_FRAMES = 3072;
    constexpr int BYTES_PER_FRAME = BACKGROUND_MUSIC_WAV_BLOCK_ALIGN;

    const f64 nowMs = emscripten_get_now();
    const int queuedBytes = SDL_GetAudioStreamQueued(this->audioStream);
    if (queuedBytes < 0)
        return false;
    const u32 queuedFrames = static_cast<u32>(queuedBytes / BYTES_PER_FRAME);
    this->webAudioMinQueuedFrames = std::min(this->webAudioMinQueuedFrames, queuedFrames);

    if (this->webAudioLastDiagnosticMs == 0.0 || nowMs - this->webAudioLastDiagnosticMs >= 500.0)
    {
        const u32 minFrames = this->webAudioMinQueuedFrames == UINT_MAX ? queuedFrames : this->webAudioMinQueuedFrames;
        EM_ASM({
            globalThis.EaglerTouhouAudioHealth?.($0, $1, $2);
        }, static_cast<double>(queuedFrames) * 1000.0 / 44100.0,
           static_cast<double>(minFrames) * 1000.0 / 44100.0, 0);
        this->webAudioLastDiagnosticMs = nowMs;
        this->webAudioMinQueuedFrames = queuedFrames;
    }

    if (!this->webAudioRefilling)
    {
        if (queuedFrames >= LOW_WATER_FRAMES)
        {
            return true;
        }
        this->webAudioRefilling = true;
    }

    if (queuedFrames >= HIGH_WATER_FRAMES)
    {
        this->webAudioRefilling = false;
        return true;
    }

    const u32 framesToMix = std::min(FRAMES_PER_CHUNK, HIGH_WATER_FRAMES - queuedFrames);
    if (framesToMix == 0)
    {
        this->webAudioRefilling = false;
        return true;
    }
    return this->MixAudio(framesToMix * BACKGROUND_MUSIC_WAV_NUM_CHANNELS);
}
#endif

void SoundPlayer::PlaySoundByIdx(SoundIdx idx)
{
    u32 i;

    if (idx < 0)
    {
        return;
    }

    for (i = 0; i < ARRAY_SIZE(this->soundBuffersToPlay); i++)
    {
        if (this->soundBuffersToPlay[i] < 0)
        {
            break;
        }

        if (this->soundBuffersToPlay[i] == idx)
        {
            return;
        }
    }

    if (i >= 3)
    {
        return;
    }

    this->soundBuffersToPlay[i] = idx;
}

bool SoundPlayer::MixAudio(u32 samples)
{
    if (this->audioStream == NULL || samples == 0 || (samples & 1) != 0)
    {
        return false;
    }
    std::vector<i16> finalBuffer(samples);
    std::vector<i32> mixBuffer(samples);

    this->soundBufMutex.lock();

    for (int i = 0; i < ARRAY_SIZE_SIGNED(this->soundBuffers); i++)
    {
        if (!this->soundBuffers[i].isPlaying)
        {
            continue;
        }

        // Sounds are all mono, so we need to duplicate each sample for stereo output
        const u32 samplesToMix = std::min(samples / 2, this->soundBuffers[i].len - this->soundBuffers[i].pos);

        for (u32 j = 0; j < samplesToMix; j++)
        {
            mixBuffer[j * 2] += this->soundBuffers[i].samples[this->soundBuffers[i].pos + j];
            mixBuffer[j * 2 + 1] += this->soundBuffers[i].samples[this->soundBuffers[i].pos + j];
        }

        this->soundBuffers[i].pos += samplesToMix;

        if (this->soundBuffers[i].pos == this->soundBuffers[i].len)
        {
            this->soundBuffers[i].isPlaying = false;
        }
    }

    if (HasBackgroundMusicSource(this->backgroundMusic))
    {
        u32 samplesMixed = 0;
        f32 fadeoutMult;
#ifdef __EMSCRIPTEN__
        std::vector<i16> oggSamples;
        if (this->backgroundMusic.srcWav.oggDecoder != NULL)
        {
            oggSamples.resize(samples);
        }
#endif

        if (this->backgroundMusic.fadeoutLen != 0)
        {
            f32 fadeoutInterp =
                mapRange(this->backgroundMusic.fadeoutProgress, 0, this->backgroundMusic.fadeoutLen, 0, 5);
            fadeoutMult = 1.0f / ZUN_POWF(10.0f, fadeoutInterp / 2.0f);
        }
        else
        {
            fadeoutMult = 1.0f;
        }

        while (samplesMixed < samples / 2)
        {
            u32 samplesToMix =
                std::min((samples / 2) - samplesMixed, this->backgroundMusic.loopEnd - this->backgroundMusic.pos);

#ifdef __EMSCRIPTEN__
            if (this->backgroundMusic.srcWav.oggDecoder != NULL)
            {
                const int decodedFrames = stb_vorbis_get_samples_short_interleaved(
                    static_cast<stb_vorbis *>(this->backgroundMusic.srcWav.oggDecoder),
                    BACKGROUND_MUSIC_WAV_NUM_CHANNELS,
                    oggSamples.data() + samplesMixed * BACKGROUND_MUSIC_WAV_NUM_CHANNELS,
                    samplesToMix * BACKGROUND_MUSIC_WAV_NUM_CHANNELS);
                if (decodedFrames <= 0)
                {
                    this->backgroundMusic.pos = this->backgroundMusic.loopEnd;
                    samplesToMix = 0;
                }
                else
                {
                    samplesToMix = static_cast<u32>(decodedFrames);
                    for (u32 j = 0; j < samplesToMix; j++)
                    {
                        mixBuffer[(samplesMixed + j) * 2] +=
                            oggSamples[(samplesMixed + j) * 2] * fadeoutMult;
                        mixBuffer[(samplesMixed + j) * 2 + 1] +=
                            oggSamples[(samplesMixed + j) * 2 + 1] * fadeoutMult;
                    }
                }
            }
            else
#endif
            {
            for (u32 j = 0; j < samplesToMix; j++)
            {
                // samplesMixed counts stereo frames; each frame occupies two
                // slots in the interleaved mix buffer.
                mixBuffer[(samplesMixed + j) * 2] +=
                    ((i16)ReadU16LE(this->backgroundMusic.srcWav.fileStream)) * fadeoutMult;
                mixBuffer[(samplesMixed + j) * 2 + 1] +=
                    ((i16)ReadU16LE(this->backgroundMusic.srcWav.fileStream)) * fadeoutMult;
            }
            }

            this->backgroundMusic.pos += samplesToMix;
            samplesMixed += samplesToMix;

            if (this->backgroundMusic.pos == this->backgroundMusic.loopEnd)
            {
                if (this->isLooping)
                {
                    this->backgroundMusic.pos = this->backgroundMusic.loopStart;
#ifdef __EMSCRIPTEN__
                    if (this->backgroundMusic.srcWav.oggDecoder != NULL)
                    {
                        if (!stb_vorbis_seek(static_cast<stb_vorbis *>(this->backgroundMusic.srcWav.oggDecoder),
                                             this->backgroundMusic.loopStart))
                        {
                            stb_vorbis_close(
                                static_cast<stb_vorbis *>(this->backgroundMusic.srcWav.oggDecoder));
                            this->backgroundMusic.srcWav.oggDecoder = NULL;
                            break;
                        }
                    }
                    else
#endif
                    {
                    SDL_SeekIO(this->backgroundMusic.srcWav.fileStream,
                               this->backgroundMusic.srcWav.dataStartOffset + this->backgroundMusic.pos * 4,
                               SDL_IO_SEEK_SET);
                    }
                }
                else
                {
#ifdef __EMSCRIPTEN__
                    if (this->backgroundMusic.srcWav.oggDecoder != NULL)
                    {
                        stb_vorbis_close(
                            static_cast<stb_vorbis *>(this->backgroundMusic.srcWav.oggDecoder));
                        this->backgroundMusic.srcWav.oggDecoder = NULL;
                    }
#endif
                    if (this->backgroundMusic.srcWav.fileStream != NULL)
                    {
                        SDL_CloseIO(this->backgroundMusic.srcWav.fileStream);
                        this->backgroundMusic.srcWav.fileStream = NULL;
                    }
                    free(this->backgroundMusic.srcWav.ownedSamples);
                    this->backgroundMusic.srcWav.ownedSamples = NULL;

                    break;
                }
            }
        }

        if (this->backgroundMusic.fadeoutLen != 0)
        {
            this->backgroundMusic.fadeoutProgress += samplesMixed;

            if (this->backgroundMusic.fadeoutProgress >= this->backgroundMusic.fadeoutLen)
            {
#ifdef __EMSCRIPTEN__
                if (this->backgroundMusic.srcWav.oggDecoder != NULL)
                {
                    stb_vorbis_close(static_cast<stb_vorbis *>(this->backgroundMusic.srcWav.oggDecoder));
                    this->backgroundMusic.srcWav.oggDecoder = NULL;
                }
#endif
                if (this->backgroundMusic.srcWav.fileStream != NULL)
                {
                    SDL_CloseIO(this->backgroundMusic.srcWav.fileStream);
                    this->backgroundMusic.srcWav.fileStream = NULL;
                }
                free(this->backgroundMusic.srcWav.ownedSamples);
                this->backgroundMusic.srcWav.ownedSamples = NULL;
            }
        }

    }

    this->soundBufMutex.unlock();

    for (u32 i = 0; i < samples; i++)
    {
        // DirectSound mixed independent buffers at their configured gain. Preserve that gain
        // here and saturate only when their sum exceeds the signed 16-bit output range.
        finalBuffer[i] = static_cast<i16>(std::max(-32768, std::min(mixBuffer[i], 32767)));
    }

    return SDL_PutAudioStreamData(this->audioStream, finalBuffer.data(), samples * 2);
}

// EoSD originally just used this function to manage the streaming of the music WAV file.
//   We also use it to mix and queue audio, since we have to do that manually and doing it
//   in a thread keeps sound running continuously, even if the main thread runs into lag
void SoundPlayer::BackgroundMusicPlayerThread()
{
    // Kept as an ABI-compatible entry point for now. SDL3 audio is fed on the
    // main thread (PumpWebAudio() at presentation cadence on Web), so the
    // browser never creates or blocks on an audio producer thread.
    (void)this->MixAudio(2048);
}
