#include "MidiOutput.hpp"
#include "FileSystem.hpp"
#include "GameErrorContext.hpp"
#include "Supervisor.hpp"
#include "inttypes.hpp"
#include "utils.hpp"

#include <SDL3/SDL.h>
#include <cstdlib>
#include <cstring>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

static u16 ReadMidiBE16(const u8 *data)
{
    return (static_cast<u16>(data[0]) << 8) | data[1];
}

static u32 ReadMidiBE32(const u8 *data)
{
    return (static_cast<u32>(data[0]) << 24) | (static_cast<u32>(data[1]) << 16) |
           (static_cast<u32>(data[2]) << 8) | data[3];
}

void MidiOutput::StartTimer(u32 delay, SDL_TimerCallback cb, void *data)
{
    this->StopTimer();

    this->lastTimerTicks = SDL_GetTicks();
    this->timerActive.store(true, std::memory_order_release);
    this->timerPaused.store(false, std::memory_order_release);

    if (cb != NULL)
    {
        this->timerId = SDL_AddTimer(delay, cb, data);
    }
    else
    {
        this->timerId = SDL_AddTimer(delay, MidiOutput::DefaultTimerCallback, this);
    }

    if (this->timerId == 0)
    {
        this->timerActive.store(false, std::memory_order_release);
    }
}

void MidiOutput::SetPaused(bool value)
{
    std::lock_guard<std::mutex> lock(this->timerMutex);
    this->timerPaused.store(value, std::memory_order_release);
    this->lastTimerTicks = SDL_GetTicks();
}

i32 MidiOutput::StopTimer()
{
    this->timerActive.store(false, std::memory_order_release);
    if (this->timerId != 0)
    {
        SDL_RemoveTimer(this->timerId);
    }

    this->timerId = 0;

    // SDL_RemoveTimer only cancels future callbacks; it does not join one
    // which is already executing. Synchronize with the callback before any
    // MUSIC_MIDI tracks or the MidiOutput itself can be released.
    std::lock_guard<std::mutex> lock(this->timerMutex);

    return 1;
}

u32 SDLCALL MidiOutput::DefaultTimerCallback(void *userdata, SDL_TimerID timerID, u32 interval)
{
    (void)timerID;

    MidiOutput *timer = (MidiOutput *)userdata;
    std::lock_guard<std::mutex> lock(timer->timerMutex);
    if (!timer->timerActive.load(std::memory_order_acquire))
    {
        return 0;
    }
    if (timer->timerPaused.load(std::memory_order_acquire))
    {
        timer->lastTimerTicks = SDL_GetTicks();
        return interval;
    }
    timer->OnTimerElapsed();

    return interval; // Reschedules with same interval
}

bool MidiOutput::ReadVariableLength(u8 **curTrackDataCursor, const u8 *end, u32 *value)
{
    u32 length = 0;

    // A Standard MUSIC_MIDI variable-length quantity is at most four bytes.
    for (i32 count = 0; count < 4; ++count)
    {
        if (*curTrackDataCursor >= end)
        {
            return false;
        }
        const u8 tmp = *(*curTrackDataCursor)++;
        length = length * 0x80 + (tmp & 0x7f);
        if ((tmp & 0x80) == 0)
        {
            *value = length;
            return true;
        }
    }

    return false;
}

MidiOutput::MidiOutput()
{
    this->timerId = 0;
    this->timerActive.store(false, std::memory_order_relaxed);
    this->timerPaused.store(false, std::memory_order_relaxed);

    this->tracks = NULL;
    this->divisions = 0;
    this->tempo = 0;
    this->numTracks = 0;
    this->fadeOutVolumeMultiplier = 0;
    this->fadeOutLastSetVolume = 0;
    this->fadeOutFlag = false;

    for (int i = 0; i < static_cast<i32>(ARRAY_SIZE(this->midiFileData)); i++)
    {
        this->midiFileData[i] = NULL;
        this->midiFileSizes[i] = 0;
    }
}

MidiOutput::~MidiOutput()
{
    this->StopTimer();

    this->StopPlayback();
    this->ClearTracks();
    for (i32 i = 0; i < 32; i++)
    {
        this->ReleaseFileData(i);
    }
}

ZunResult MidiOutput::ReadFileData(u32 idx, const char *path)
{
    if (idx >= ARRAY_SIZE(this->midiFileData))
    {
        return ZUN_ERROR;
    }
    bool allowRead = g_Supervisor.cfg.musicMode == MUSIC_MIDI;
#ifdef __EMSCRIPTEN__
    allowRead = allowRead || EM_ASM_INT({ return Module.touhouMusicMode === 'ogg'; });
#endif
    if (!allowRead)
    {
        return ZUN_SUCCESS;
    }

    this->StopPlayback();
    this->ReleaseFileData(idx);

    this->midiFileData[idx] = FileSystem::OpenFile(path, false);

    if (this->midiFileData[idx] == NULL)
    {
        g_GameErrorContext.Log("error : MIDI file could not be read %s\n", path);
        return ZUN_ERROR;
    }

    this->midiFileSizes[idx] = g_LastFileSize;

    return ZUN_SUCCESS;
}

void MidiOutput::ReleaseFileData(u32 idx)
{
    if (idx >= ARRAY_SIZE(this->midiFileData))
    {
        return;
    }
    std::free(this->midiFileData[idx]);

    this->midiFileData[idx] = NULL;
    this->midiFileSizes[idx] = 0;
}

void MidiOutput::ClearTracks()
{
    i32 trackIndex;
    u8 *data;
    MidiTrack *tracks;

    for (trackIndex = 0; this->tracks != NULL && trackIndex < this->numTracks; trackIndex++)
    {
        data = this->tracks[trackIndex].trackData;
        std::free(data);
    }

    tracks = this->tracks;
    std::free(tracks);
    this->tracks = NULL;
    this->numTracks = 0;
}

ZunResult MidiOutput::ParseFile(i32 fileIdx)
{
    u8 hdrRaw[8];
    u32 trackLength;
    const u8 *currentCursor;
    const u8 *currentCursorTrack;
    const u8 *endOfHeaderPointer;
    i32 trackIdx;
    u32 hdrLength;

    this->ClearTracks();
    if (fileIdx < 0 || fileIdx >= static_cast<i32>(ARRAY_SIZE(this->midiFileData)) ||
        this->midiFileSizes[fileIdx] < 14)
    {
        return ZUN_ERROR;
    }
    currentCursor = this->midiFileData[fileIdx];
    if (currentCursor == NULL)
    {
        Supervisor::DebugPrint("error : MIDI playback requested before loading\n");
        return ZUN_ERROR;
    }

    // Read midi header chunk
    // First, read the header len
    const u8 *fileEnd = currentCursor + this->midiFileSizes[fileIdx];
    std::memcpy(&hdrRaw, currentCursor, 8);
    if (std::memcmp(hdrRaw, "MThd", 4) != 0)
    {
        return ZUN_ERROR;
    }

    // Get a pointer to the end of the header chunk
    currentCursor += sizeof(hdrRaw);
    hdrLength = ReadMidiBE32(hdrRaw + 4);

    if (hdrLength < 6 || static_cast<size_t>(fileEnd - currentCursor) < hdrLength)
    {
        return ZUN_ERROR;
    }

    endOfHeaderPointer = currentCursor;
    currentCursor += hdrLength;

    // Read the format. Only three values of format are specified:
    //  0: the file contains a single multi-channel track
    //  1: the file contains one or more simultaneous tracks (or MUSIC_MIDI outputs) of a
    //  sequence
    //  2: the file contains one or more sequentially independent single-track
    //  patterns
    this->format = ReadMidiBE16(endOfHeaderPointer);

    // Read the divisions in this track. Note that this doesn't appear to support
    // "negative SMPTE format", which happens when the MSB is set.
    this->divisions = ReadMidiBE16(endOfHeaderPointer + 4);
    // Read the number of tracks in this midi file.
    this->numTracks = ReadMidiBE16(endOfHeaderPointer + 2);

    if (this->format > 2 || this->divisions <= 0 || this->numTracks <= 0 || this->numTracks > 256)
    {
        this->numTracks = 0;
        return ZUN_ERROR;
    }

    // Allocate this->divisions * 32 bytes.
    this->tracks = (MidiTrack *)std::malloc(sizeof(MidiTrack) * this->numTracks);
    if (this->tracks == NULL)
    {
        this->numTracks = 0;
        return ZUN_ERROR;
    }
    std::memset(this->tracks, 0, sizeof(MidiTrack) * this->numTracks);
    for (trackIdx = 0; trackIdx < this->numTracks; trackIdx++)
    {
        currentCursorTrack = currentCursor;
        if (fileEnd - currentCursor < 8 || std::memcmp(currentCursorTrack, "MTrk", 4) != 0)
        {
            this->ClearTracks();
            return ZUN_ERROR;
        }
        currentCursor += 8;

        // Read a track (MTrk) chunk.
        //
        // First, read the length of the chunk
        trackLength = ReadMidiBE32(currentCursorTrack + 4);
        if (trackLength == 0 || static_cast<size_t>(fileEnd - currentCursor) < trackLength)
        {
            this->ClearTracks();
            return ZUN_ERROR;
        }
        this->tracks[trackIdx].trackLength = trackLength;
        this->tracks[trackIdx].trackData = (u8 *)std::malloc(trackLength);
        if (this->tracks[trackIdx].trackData == NULL)
        {
            this->ClearTracks();
            return ZUN_ERROR;
        }
        this->tracks[trackIdx].trackPlaying = 1;
        std::memcpy(this->tracks[trackIdx].trackData, currentCursor, trackLength);
        this->tracks[trackIdx].trackDataEnd = this->tracks[trackIdx].trackData + trackLength;
        currentCursor += trackLength;
    }
    this->tempo = 1'000'000;
    return ZUN_SUCCESS;
}

ZunResult MidiOutput::LoadFile(const char *midiPath)
{
    if (this->ReadFileData(0x1f, midiPath) != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }

    const ZunResult result = this->ParseFile(0x1f);
    this->ReleaseFileData(0x1f);

    return result;
}

void MidiOutput::LoadTracks()
{
    i32 trackIndex;
    MidiTrack *track = this->tracks;

    this->fadeOutVolumeMultiplier = 1.0;
    this->fadeOutFlag = false;
    this->elapsedMS = 0;
    this->tickBase = 0;

    for (trackIndex = 0; trackIndex < this->numTracks; trackIndex++, track++)
    {
        track->curTrackDataCursor = track->trackData;
        track->loopPointTarget = track->curTrackDataCursor;
        track->trackPlaying = true;
        if (!MidiOutput::ReadVariableLength(&track->curTrackDataCursor, track->trackDataEnd,
                                            &track->nextMessageTimePos))
        {
            track->trackPlaying = false;
        }
    }
}

ZunResult MidiOutput::Play()
{
    if (this->tracks == NULL)
    {
        return ZUN_ERROR;
    }

    this->LoadTracks();
    if (!this->midiOutDev.OpenDevice(0xFFFF'FFFF))
    {
        return ZUN_ERROR;
    }
    this->StartTimer(1, NULL, NULL);

    if (this->timerId == 0)
    {
        this->midiOutDev.Close();
        return ZUN_ERROR;
    }

    return ZUN_SUCCESS;
}

ZunResult MidiOutput::StopPlayback()
{
    this->StopTimer();
    this->midiOutDev.Close();
    return ZUN_SUCCESS;
}

u32 MidiOutput::SetFadeOut(u32 ms)
{
    this->fadeOutVolumeMultiplier = 0.0;
    this->fadeOutInterval = ms;
    this->fadeOutElapsedMS = 0;
    this->fadeOutFlag = true;

    return 0;
}

// Windows EoSD relies solely on the number of times this function is called for timing,
//   assuming that there is exactly 1 ms between calls. In my testing, the time between
//   calls with the SDL timer actually ends up averaging to 1.08 ms and the MUSIC_MIDI playback
//   ends up noticeably slow, so the timing mechanism has been replaced with getting a
//   delta from SDL_GetTicks instead.
void MidiOutput::OnTimerElapsed()
{
    u64 timePos;
    i32 trackIndex;
    bool trackLoaded;

    trackLoaded = false;
    if (this->tempo <= 0 || this->divisions <= 0 || this->tracks == NULL)
    {
        return;
    }
    timePos = this->tickBase + (this->elapsedMS * this->divisions * 1000) / this->tempo;
    if (this->fadeOutFlag)
    {
        if (this->fadeOutElapsedMS < this->fadeOutInterval)
        {
            this->fadeOutVolumeMultiplier = 1.0f - (f32)this->fadeOutElapsedMS / (f32)this->fadeOutInterval;
            if ((u32)(this->fadeOutVolumeMultiplier * 128.0f) != this->fadeOutLastSetVolume)
            {
                this->FadeOutSetVolume(0);
            }
            this->fadeOutLastSetVolume = this->fadeOutVolumeMultiplier * 128.0f;
            this->fadeOutElapsedMS = this->fadeOutElapsedMS + 1;
        }
        else
        {
            this->fadeOutVolumeMultiplier = 0.0;
            return;
        }
    }

    for (trackIndex = 0; trackIndex < this->numTracks; trackIndex++)
    {
        if (this->tracks[trackIndex].trackPlaying)
        {
            trackLoaded = true;
            while (this->tracks[trackIndex].trackPlaying)
            {
                if (this->tracks[trackIndex].nextMessageTimePos <= timePos)
                {
                    this->ProcessMsg(&this->tracks[trackIndex]);
                    timePos = this->tickBase + (this->elapsedMS * this->divisions * 1000 / this->tempo);
                    continue;
                }
                break;
            }
        }
    }

    u32 curTicks = SDL_GetTicks();
    this->elapsedMS += curTicks - this->lastTimerTicks;
    this->lastTimerTicks = curTicks;

    if (!trackLoaded)
    {
        this->LoadTracks();
    }
}

void MidiOutput::ProcessMsg(MidiTrack *track)
{
    i32 curTrackLength;
    u8 arg1 = 0, arg2 = 0;
    u8 opcode, opcodeHigh, opcodeLow;
    u8 metaEventID;
    i32 idx;
    u8 *sysExData;

    if (track == NULL || track->curTrackDataCursor >= track->trackDataEnd)
    {
        if (track != NULL)
        {
            track->trackPlaying = false;
        }
        return;
    }

    opcode = *track->curTrackDataCursor;
    if (opcode < MIDI_OPCODE_NOTE_OFF)
    {
        opcode = track->opcode;
        if (opcode < MIDI_OPCODE_NOTE_OFF)
        {
            track->trackPlaying = false;
            return;
        }
    }
    else
    {
        track->curTrackDataCursor++;
    }

    // we AND the opcode to filter out the channel
    opcodeHigh = opcode & 0xf0;
    opcodeLow = opcode & 0x0f;
    switch (opcodeHigh)
    {
    case MIDI_OPCODE_SYSTEM_EXCLUSIVE:
        if (opcode == MIDI_OPCODE_SYSTEM_EXCLUSIVE)
        {
            u32 messageLength = 0;
            if (!MidiOutput::ReadVariableLength(&track->curTrackDataCursor, track->trackDataEnd, &messageLength) ||
                messageLength > static_cast<u32>(track->trackDataEnd - track->curTrackDataCursor))
            {
                track->trackPlaying = false;
                return;
            }
            curTrackLength = static_cast<i32>(messageLength);

            sysExData = (u8 *)std::malloc(curTrackLength + 1);
            if (sysExData == NULL)
            {
                track->trackPlaying = false;
                return;
            }
            sysExData[0] = MIDI_OPCODE_SYSTEM_EXCLUSIVE;

            std::memcpy(sysExData + 1, track->curTrackDataCursor, curTrackLength);

            this->midiOutDev.SendLongMsg(sysExData, curTrackLength + 1);

            track->curTrackDataCursor += curTrackLength;

            std::free(sysExData);
        }
        else if (opcode == MIDI_OPCODE_SYSTEM_RESET)
        {
            // Meta-Event. In a MUSIC_MIDI file, SYSTEM_RESET gets reused as a
            // sort of escape code to introducde its own meta-events system,
            // which are events that make sense in the context of a MUSIC_MIDI
            // file, but not in the context of the MUSIC_MIDI protocol itself.
            if (track->curTrackDataCursor >= track->trackDataEnd)
            {
                track->trackPlaying = false;
                return;
            }
            metaEventID = *track->curTrackDataCursor;
            track->curTrackDataCursor++;
            u32 messageLength = 0;
            if (!MidiOutput::ReadVariableLength(&track->curTrackDataCursor, track->trackDataEnd, &messageLength) ||
                messageLength > static_cast<u32>(track->trackDataEnd - track->curTrackDataCursor))
            {
                track->trackPlaying = false;
                return;
            }
            curTrackLength = static_cast<i32>(messageLength);

            // End of Track meta-event.
            if (metaEventID == 0x2f)
            {
                track->trackPlaying = false;
                return;
            }

            // Set Tempo meta-event.
            if (metaEventID == 0x51)
            {
                this->tickBase += (this->elapsedMS * this->divisions * 1000 / this->tempo);
                this->elapsedMS = 0;
                this->tempo = 0;

                for (idx = 0; idx < curTrackLength; idx++)
                {
                    this->tempo = this->tempo * 0x100 + *track->curTrackDataCursor;
                    track->curTrackDataCursor++;
                }

                if (this->tempo <= 0)
                {
                    track->trackPlaying = false;
                    return;
                }

                break;
            }

            track->curTrackDataCursor += curTrackLength;
        }
        break;
    case MIDI_OPCODE_NOTE_OFF:
    case MIDI_OPCODE_NOTE_ON:
    case MIDI_OPCODE_POLYPHONIC_AFTERTOUCH:
    case MIDI_OPCODE_MODE_CHANGE:
    case MIDI_OPCODE_PITCH_BEND_CHANGE:
        if (track->trackDataEnd - track->curTrackDataCursor < 2)
        {
            track->trackPlaying = false;
            return;
        }
        arg1 = *track->curTrackDataCursor;
        track->curTrackDataCursor++;
        arg2 = *track->curTrackDataCursor;
        track->curTrackDataCursor++;
        break;
    case MIDI_OPCODE_PROGRAM_CHANGE:
    case MIDI_OPCODE_CHANNEL_AFTERTOUCH:
        if (track->curTrackDataCursor >= track->trackDataEnd)
        {
            track->trackPlaying = false;
            return;
        }
        arg1 = *track->curTrackDataCursor;
        track->curTrackDataCursor++;
        arg2 = 0;
        break;
    }

    if ((opcodeHigh >= MIDI_OPCODE_NOTE_OFF && opcodeHigh <= MIDI_OPCODE_PITCH_BEND_CHANGE) &&
        (arg1 >= 0x80 || arg2 >= 0x80))
    {
        track->trackPlaying = false;
        return;
    }

    switch (opcodeHigh)
    {
    case MIDI_OPCODE_NOTE_ON:
        if (arg2 != 0)
        {
            this->channels[opcodeLow].keyPressedFlags[arg1 >> 3] |= (1u << (arg1 & 7));
            break;
        }

        SDL_FALLTHROUGH;
    case MIDI_OPCODE_NOTE_OFF:
        this->channels[opcodeLow].keyPressedFlags[arg1 >> 3] &= ~(1u << (arg1 & 7));
        break;
    case MIDI_OPCODE_PROGRAM_CHANGE:
        // Program Change
        this->channels[opcodeLow].instrument = arg1;
        break;
    case MIDI_OPCODE_MODE_CHANGE:
        switch (arg1)
        {
        case 0:
            // Bank Select
            this->channels[opcodeLow].instrumentBank = arg2;
            break;
        case 7:
            // Channel Volume
            this->channels[opcodeLow].channelVolume = arg2;
            break;
        case 91:
            // Effects 1 Depth
            this->channels[opcodeLow].effectOneDepth = arg2;
            break;
        case 93:
            // Effects 3 Depth
            this->channels[opcodeLow].effectThreeDepth = arg2;
            break;
        case 10:
            // Pan
            this->channels[opcodeLow].pan = arg2;
            break;

        // EoSD doesn't actually use these last two for their intended purpose, instead
        //   using the breath controller to identify the target of a loop within the file and
        //   the foot controller to identify the loop point. Why did Zun do it like this
        //   instead of adding a meta event? Who knows...
        case 2:
            // Breath control
            for (i32 i = 0; i < this->numTracks; i++)
            {
                this->tracks[i].loopPointTarget = this->tracks[i].curTrackDataCursor;
                this->tracks[i].loopPointTimePos = this->tracks[i].nextMessageTimePos;
            }
            this->loopPointTempo = this->tempo;
            this->loopPointMSCount = this->elapsedMS;
            this->loopPointBaseTicks = this->tickBase;

            break;
        case 4:
            // Foot controller
            for (i32 i = 0; i < this->numTracks; i++)
            {
                this->tracks[i].curTrackDataCursor = this->tracks[i].loopPointTarget;
                this->tracks[i].nextMessageTimePos = this->tracks[i].loopPointTimePos;
            }
            this->tempo = this->loopPointTempo;
            this->elapsedMS = this->loopPointMSCount;
            this->tickBase = this->loopPointBaseTicks;

            break;
        }
        break;
    }

    if (opcode < MIDI_OPCODE_SYSTEM_EXCLUSIVE)
    {
        this->midiOutDev.SendShortMsg(opcode, arg1, arg2);
    }

    track->opcode = opcode;
    u32 delta = 0;
    if (!MidiOutput::ReadVariableLength(&track->curTrackDataCursor, track->trackDataEnd, &delta) ||
        UINT32_MAX - track->nextMessageTimePos < delta)
    {
        track->trackPlaying = false;
        return;
    }
    track->nextMessageTimePos += delta;
}

void MidiOutput::FadeOutSetVolume(i32 volume)
{
    i32 idx;
    i32 volumeClamped;

    for (idx = 0; idx < static_cast<i32>(ARRAY_SIZE(this->channels)); idx++)
    {
        volumeClamped = (i32)(this->channels[idx].channelVolume * this->fadeOutVolumeMultiplier) + volume;

        if (volumeClamped < 0)
        {
            volumeClamped = 0;
        }
        else if (volumeClamped > 127)
        {
            volumeClamped = 127;
        }

        // 7: Controller value number for volume (with range 0 - 127)
        this->midiOutDev.SendShortMsg(MIDI_OPCODE_MODE_CHANGE | idx, 7, volumeClamped);
    }
}

void Supervisor::StopMidiTimer(MidiTimer *timer)
{
    if (timer) timer->StopTimer();
}
