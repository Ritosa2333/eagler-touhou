#include "SoundPlayer.hpp"

#include "FileSystem.hpp"
#include "Supervisor.hpp"
#include "i18n.hpp"
#include "utils.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL.h>
#include <array>
#include <cmath>
#include <cstring>
#include <new>
#include <vector>

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
    // Note: memset of an std::mutex crashes on windows
    // std::memset(this, 0, sizeof(SoundPlayer));
}

ZunResult SoundPlayer::InitializeDSound()
{
    SDL_AudioSpec desiredAudio;
    if (SDL_InitSubSystem(SDL_INIT_AUDIO))
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
    if (this->backgroundMusic.srcWav.fileStream != NULL)
    {
        this->soundBufMutex.lock();
        SDL_CloseIO(this->backgroundMusic.srcWav.fileStream);
        this->backgroundMusic.srcWav.fileStream = NULL;
        this->soundBufMutex.unlock();

        utils::DebugPrint2("stop BGM\n");
    }
}

void SoundPlayer::FadeOut(f32 seconds)
{
    if (this->backgroundMusic.srcWav.fileStream != NULL)
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

    if (this->audioDev == 0)
    {
        return ZUN_ERROR;
    }

    if (g_Supervisor.cfg.playSounds == 0)
    {
        return ZUN_ERROR;
    }

    this->StopBGM();

    utils::DebugPrint2("load BGM\n");

    fileStream = SDL_IOFromFile(path, "rb");

    if (fileStream == NULL)
    {
        utils::DebugPrint2("error : wav file load error %s\n", path);
        return ZUN_ERROR;
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

    if (wavDataSize > riffSize - 44)
    {
        goto fail;
    }

    this->backgroundMusic.srcWav.samples = wavDataSize / BACKGROUND_MUSIC_WAV_BLOCK_ALIGN;

    if (this->backgroundMusic.srcWav.samples == 0)
    {
        goto fail;
    }

    this->backgroundMusic.srcWav.fileStream = fileStream;
    this->backgroundMusic.srcWav.dataStartOffset = SDL_TellIO(fileStream);
    this->backgroundMusic.loopStart = 0;
    this->backgroundMusic.loopEnd = this->backgroundMusic.srcWav.samples;
    this->backgroundMusic.fadeoutLen = 0;
    this->backgroundMusic.fadeoutProgress = 0;
    this->backgroundMusic.pos = 0;

    return ZUN_SUCCESS;

fail:
    SDL_CloseIO(fileStream);
    return ZUN_ERROR;
}

ZunResult SoundPlayer::LoadPos(const char *path)
{
    u8 *fileData;

    if (this->audioDev == 0 || g_Supervisor.cfg.playSounds == 0 || backgroundMusic.srcWav.fileStream == NULL)
    {
        return ZUN_ERROR;
    }

    fileData = FileSystem::OpenPath(path, 0);

    if (fileData == NULL)
    {
        return ZUN_ERROR;
    }

    this->backgroundMusic.loopStart = SDL_Swap32LE(*((u32 *)fileData));
    this->backgroundMusic.loopEnd = SDL_Swap32LE(*(u32 *)(fileData + 4));

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

    if (!SDL_LoadWAV_IO(SDL_IOFromConstMem(wavRawData, g_LastFileSize), true, &wavFormat, &wavRawSamples,
                        &wavRawSampleByteCount))
    {
        g_GameErrorContext.Log(TH_ERR_NOT_A_WAV_FILE, path);
        goto fail;
    }

    // EoSD's sound files are all 22050 Hz, and some even use 8-bit samples. Converting them
    //   here only uses a few hundred extra kilobytes of RAM compared to the original code,
    //   but it might be worth looking into avoiding it for especially RAM-limited systems

    if (SDL_ConvertAudioSamples(&wavFormat, wavRawSamples, wavRawSampleByteCount, &targetFormat,
                                &convertedSamples, &convertedByteCount))
    {
        this->soundBuffers[idx].len = convertedByteCount / 2;
        this->soundBuffers[idx].samples = new i16[this->soundBuffers[idx].len];
        std::memcpy(this->soundBuffers[idx].samples, convertedSamples, convertedByteCount);
        SDL_free(convertedSamples);
    }
    else
    {
        this->soundBuffers[idx].len = wavRawSampleByteCount / 2;
        this->soundBuffers[idx].samples = new i16[this->soundBuffers[idx].len];
        std::memcpy(this->soundBuffers[idx].samples, wavRawSamples, wavRawSampleByteCount);
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

    if (this->backgroundMusic.srcWav.fileStream == NULL)
    {
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

        if (this->soundBuffers[sndBufIdx].samples == NULL)
        {
            continue;
        }

        this->soundBuffers[sndBufIdx].pos = 0;
        this->soundBuffers[sndBufIdx].isPlaying = true;
    }

    soundBufMutex.unlock();

    while (SDL_GetAudioStreamQueued(this->audioStream) < 8192)
    {
        this->MixAudio(2048);
    }
}

void SoundPlayer::PlaySoundByIdx(SoundIdx idx)
{
    u32 i;

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

void SoundPlayer::MixAudio(u32 samples)
{
    std::vector<i16> finalBuffer(samples);
    std::vector<i32> mixBuffer(samples);
    u8 playingChannels = 0;

    this->soundBufMutex.lock();

    for (int i = 0; i < ARRAY_SIZE_SIGNED(this->soundBuffers); i++)
    {
        if (!this->soundBuffers[i].isPlaying)
        {
            continue;
        }

        playingChannels++;

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

    if (this->backgroundMusic.srcWav.fileStream != NULL)
    {
        u32 samplesMixed = 0;
        f32 fadeoutMult;

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
            const u32 samplesToMix =
                std::min((samples / 2) - samplesMixed, this->backgroundMusic.loopEnd - this->backgroundMusic.pos);

            for (u32 j = 0; j < samplesToMix; j++)
            {
                mixBuffer[samplesMixed + j * 2] +=
                    ((i16)ReadU16LE(this->backgroundMusic.srcWav.fileStream)) * fadeoutMult;
                mixBuffer[samplesMixed + j * 2 + 1] +=
                    ((i16)ReadU16LE(this->backgroundMusic.srcWav.fileStream)) * fadeoutMult;
            }

            this->backgroundMusic.pos += samplesToMix;
            samplesMixed += samplesToMix;

            if (this->backgroundMusic.pos == this->backgroundMusic.loopEnd)
            {
                if (this->isLooping)
                {
                    this->backgroundMusic.pos = this->backgroundMusic.loopStart;
                    SDL_SeekIO(this->backgroundMusic.srcWav.fileStream,
                               this->backgroundMusic.srcWav.dataStartOffset + this->backgroundMusic.pos * 4,
                               SDL_IO_SEEK_SET);
                }
                else
                {
                    SDL_CloseIO(this->backgroundMusic.srcWav.fileStream);
                    this->backgroundMusic.srcWav.fileStream = NULL;

                    break;
                }
            }
        }

        if (this->backgroundMusic.fadeoutLen != 0)
        {
            this->backgroundMusic.fadeoutProgress += samplesMixed;

            if (this->backgroundMusic.fadeoutProgress >= this->backgroundMusic.fadeoutLen)
            {
                SDL_CloseIO(this->backgroundMusic.srcWav.fileStream);
                this->backgroundMusic.srcWav.fileStream = NULL;
            }
        }

        playingChannels++;
    }

    this->soundBufMutex.unlock();

    // DirectSound supports playing from an arbitrary number of buffers at once, but that's kind of
    //   difficult to get right as it turns out. Instead we use 8 as an assumption of the
    //   max number of channels that could possibly be playing at once. If more channels end up in use,
    //   the input volume of each channel will start scaling down, which isn't correct, but would
    //   likely be imperceptible with that many channels anyway.

    const int mixDivisor = std::max(8, (int)playingChannels);

    for (u32 i = 0; i < samples; i++)
    {
        // Integer division like this doesn't get optimized at all by the compiler. If it becomes
        //   a problem, it could be a good idea to convert to float, or to do the division as
        //   fixed point multiplication by the inverse of mixDivisor, depending on what's faster
        //   on any particular platform
        finalBuffer[i] = mixBuffer[i] / mixDivisor;
    }

    SDL_PutAudioStreamData(this->audioStream, finalBuffer.data(), samples * 2);
}

// EoSD originally just used this function to manage the streaming of the music WAV file.
//   We also use it to mix and queue audio, since we have to do that manually and doing it
//   in a thread keeps sound running continuously, even if the main thread runs into lag
void SoundPlayer::BackgroundMusicPlayerThread()
{
    // Kept as an ABI-compatible entry point for now. SDL3 audio is fed from
    // PlaySounds(), once per application iteration, so the browser main thread
    // never creates or blocks on an audio producer thread.
    this->MixAudio(2048);
}
