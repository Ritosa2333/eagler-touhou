#include "midi/MidiWeb.hpp"

#include <emscripten.h>

MidiDevice::MidiDevice() : opened(false)
{
}

MidiDevice::~MidiDevice()
{
    Close();
}

bool MidiDevice::OpenDevice(u32 deviceId)
{
    (void)deviceId;
    opened = true;
    EM_ASM({ window.dispatchEvent(new CustomEvent("touhou-midi-open")); });
    return true;
}

ZunResult MidiDevice::Close()
{
    if (opened)
    {
        EM_ASM({ window.dispatchEvent(new CustomEvent("touhou-midi-close")); });
    }
    opened = false;
    return ZUN_SUCCESS;
}

bool MidiDevice::SendShortMsg(u8 status, u8 data1, u8 data2)
{
    if (!opened)
    {
        return false;
    }
    EM_ASM({
        window.dispatchEvent(new CustomEvent("touhou-midi", {
            detail: { bytes: [$0 & 255, $1 & 255, $2 & 255] }
        }));
    }, status, data1, data2);
    return true;
}

bool MidiDevice::SendLongMsg(const u8 *data, u32 length)
{
    if (!opened || data == nullptr || length == 0)
    {
        return false;
    }
    EM_ASM({
        const bytes = Array.from(HEAPU8.subarray($0, $0 + $1));
        window.dispatchEvent(new CustomEvent("touhou-midi", { detail: { bytes } }));
    }, data, length);
    return true;
}
