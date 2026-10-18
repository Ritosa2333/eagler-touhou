#include "MidiWin32.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <cstring>

MidiDevice::MidiDevice()
{
    this->handle = NULL;
    this->deviceId = 0;

    for (int i = 0; i < static_cast<int>(ARRAY_SIZE(this->midiHeaders)); i++)
    {
        this->midiHeaders[i] = NULL;
    }

    this->midiHeadersCursor = 0;
}

MidiDevice::~MidiDevice()
{
    this->Close();
}

bool MidiDevice::OpenDevice(u32 uDeviceId)
{
    if (this->handle != 0)
    {
        if (this->deviceId != uDeviceId)
        {
            this->Close();
        }
        else
        {
            return true;
        }
    }

    this->deviceId = uDeviceId;

    // TODO: Write callback function. Windows EoSD used WndProc for this, but we obviously can't do that here
    return midiOutOpen(&this->handle, uDeviceId, (DWORD_PTR)NULL, (DWORD_PTR)NULL, CALLBACK_NULL) == MMSYSERR_NOERROR;
}

ZunResult MidiDevice::Close()
{
    if (this->handle == 0)
    {
        return ZUN_ERROR;
    }

    midiOutReset(this->handle);

    for (i32 i = 0; i < static_cast<i32>(ARRAY_SIZE(this->midiHeaders)); i++)
    {
        if (this->midiHeaders[i] != NULL)
        {
            this->UnprepareHeader(this->midiHeaders[i]);
        }
    }

    midiOutClose(this->handle);
    this->handle = 0;

    return ZUN_SUCCESS;
}

union MidiShortMsg {
    struct
    {
        u8 midiStatus;
        i8 firstByte;
        i8 secondByte;
        i8 unused;
    } msg;
    u32 dwMsg;
};

bool MidiDevice::SendLongMsg(const u8 *buf, u32 len)
{
    if (this->handle == 0)
    {
        return true;
    }

    // Reclaim completed asynchronous messages before looking for a slot.
    for (i32 i = 0; i < static_cast<i32>(ARRAY_SIZE(this->midiHeaders)); ++i)
    {
        MIDIHDR *header = this->midiHeaders[i];
        if (header != NULL && (header->dwFlags & MHDR_DONE) != 0)
        {
            this->UnprepareHeader(header);
        }
    }

    i32 slot = -1;
    for (i32 offset = 0; offset < static_cast<i32>(ARRAY_SIZE(this->midiHeaders)); ++offset)
    {
        const i32 candidate = (this->midiHeadersCursor + offset) % static_cast<i32>(ARRAY_SIZE(this->midiHeaders));
        if (this->midiHeaders[candidate] == NULL)
        {
            slot = candidate;
            break;
        }
    }
    if (slot < 0 || buf == NULL || len == 0)
    {
        return false;
    }

    MIDIHDR *midiHdr = (MIDIHDR *)std::calloc(1, sizeof(MIDIHDR));
    if (midiHdr == NULL)
    {
        return false;
    }
    midiHdr->lpData = (LPSTR)std::malloc(len);
    if (midiHdr->lpData == NULL)
    {
        std::free(midiHdr);
        return false;
    }
    std::memcpy(midiHdr->lpData, buf, len);
    midiHdr->dwBufferLength = len;

    if (midiOutPrepareHeader(this->handle, midiHdr, sizeof(*midiHdr)) != MMSYSERR_NOERROR)
    {
        std::free(midiHdr->lpData);
        std::free(midiHdr);
        return false;
    }

    this->midiHeaders[slot] = midiHdr;
    this->midiHeadersCursor = (slot + 1) % ARRAY_SIZE(this->midiHeaders);

    if (midiOutLongMsg(this->handle, midiHdr, sizeof(*midiHdr)) != MMSYSERR_NOERROR)
    {
        this->UnprepareHeader(midiHdr);
        return false;
    }
    return true;
}

bool MidiDevice::SendShortMsg(u8 midiStatus, u8 firstByte, u8 secondByte)
{
    MidiShortMsg pkt{};

    if (this->handle == 0)
    {
        return true;
    }
    else
    {
        pkt.msg.midiStatus = midiStatus;
        pkt.msg.firstByte = firstByte;
        pkt.msg.secondByte = secondByte;
        return midiOutShortMsg(this->handle, pkt.dwMsg) == MMSYSERR_NOERROR;
    }
}

ZunResult MidiDevice::UnprepareHeader(LPMIDIHDR pmh)
{
    if (pmh == NULL || this->handle == 0)
    {
        return ZUN_ERROR;
    }

    // The reason for this weird linear search here is that this is supposed to be able
    //   to run after Windows sends an MM_MOM_DONE message indicating that a long message
    //   was sent. To save ourselves from a possible double free we have to make sure that
    //   the header hasn't yet been freed.

    for (i32 i = 0; i < static_cast<i32>(ARRAY_SIZE(this->midiHeaders)); i++)
    {
        if (this->midiHeaders[i] == pmh)
        {
            MMRESULT res = midiOutUnprepareHeader(this->handle, pmh, sizeof(*pmh));
            if (res != MMSYSERR_NOERROR)
            {
                // In particular, MIDIERR_STILLPLAYING means the driver still
                // owns this memory. Keep it registered and try again later.
                return ZUN_ERROR;
            }

            this->midiHeaders[i] = NULL;
            std::free(pmh->lpData);
            std::free(pmh);
            return ZUN_SUCCESS;
        }
    }

    return ZUN_ERROR;
}
