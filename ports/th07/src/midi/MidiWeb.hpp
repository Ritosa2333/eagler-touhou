#pragma once

#include "ZunResult.hpp"
#include "inttypes.hpp"

// Browser MIDI transport. The game remains responsible for parsing and
// scheduling the original MIDI; the Web host chooses how to synthesize the
// emitted byte stream.
struct MidiDevice
{
  public:
    MidiDevice();
    ~MidiDevice();

    ZunResult Close();
    bool OpenDevice(u32 deviceId);
    bool SendShortMsg(u8 status, u8 data1, u8 data2);
    bool SendLongMsg(const u8 *data, u32 length);

  private:
    bool opened;
};
