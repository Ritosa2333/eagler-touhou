// BSD 3-Clause License
//
// Copyright (c) 2021, Aaron Giles
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

use super::lfo::Lfo;

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) struct OperatorConfig {
    pub(crate) block_freq: u32,
    pub(crate) detune: i32,
    pub(crate) multiple: u32,
    pub(crate) pm_sensitivity: u8,
    pub(crate) total_level: u32,
    pub(crate) sustain: u32,
    pub(crate) rates: [u8; 6],
    pub(crate) am_enabled: bool,
    pub(crate) ssg_enabled: bool,
    pub(crate) ssg_mode: u8,
}

#[derive(Clone, Debug)]
pub(crate) struct OpnaRegisters {
    data: [u8; 0x200],
    lfo: Lfo,
}

impl OpnaRegisters {
    pub(crate) const fn new() -> Self {
        Self {
            data: [0; 0x200],
            lfo: Lfo::new(),
        }
    }

    pub(crate) fn reset(&mut self) {
        self.data = [0; 0x200];

        self.data[0x0b4] = 0xc0;
        self.data[0x0b5] = 0xc0;
        self.data[0x0b6] = 0xc0;
        self.data[0x1b4] = 0xc0;
        self.data[0x1b5] = 0xc0;
        self.data[0x1b6] = 0xc0;
    }

    pub(crate) fn read(&self, index: u16) -> u8 {
        self.data.get(index as usize).copied().unwrap_or(0)
    }

    pub(crate) fn write(&mut self, index: u16, value: u8) -> Option<(usize, u8)> {
        let index = index as usize;
        if index >= self.data.len() {
            return None;
        }

        if index & 0xf0 == 0xa0 {
            if index & 3 == 3 {
                return None;
            }
            let mut latch = 0x0b8 | ((index >> 3) & 1);
            if index & 0x100 != 0 {
                latch |= 0x100;
            }
            if index & 4 != 0 {
                self.data[latch] = value | 0x80;
            } else if self.data[latch] & 0x80 != 0 {
                self.data[index] = value;
                self.data[index | 4] = self.data[latch] & 0x3f;
                self.data[latch] = 0;
            }
            return None;
        }

        if index & 0xf8 == 0xb8 {
            return None;
        }

        self.data[index] = value;
        if index == 0x28 {
            let mut channel = usize::from(value & 3);
            if channel == 3 {
                return None;
            }
            if value & 4 != 0 {
                channel += 3;
            }
            return Some((channel, value >> 4));
        }
        None
    }

    pub(crate) fn clock_lfo(&mut self) -> i32 {
        self.lfo.clock(self.lfo_enabled(), self.lfo_rate())
    }

    pub(crate) fn lfo_am_offset(&self, channel_offset: usize) -> u32 {
        self.lfo.am_offset(self.channel_lfo_am(channel_offset))
    }

    pub(crate) fn lfo_enabled(&self) -> bool {
        self.data[0x22] & 0x08 != 0
    }

    fn lfo_rate(&self) -> u8 {
        self.data[0x22] & 7
    }

    pub(crate) fn channel_offset(channel: usize) -> usize {
        (channel % 3) + 0x100 * (channel / 3)
    }

    pub(crate) fn operator_offsets(channel: usize) -> [usize; 4] {
        let bank = 0x100 * (channel / 3);
        match channel % 3 {
            0 => [bank, bank + 8, bank + 4, bank + 12],
            1 => [bank + 1, bank + 9, bank + 5, bank + 13],
            _ => [bank + 2, bank + 10, bank + 6, bank + 14],
        }
    }

    pub(crate) fn channel_algorithm(&self, channel_offset: usize) -> u8 {
        self.data[0x0b0 + channel_offset] & 7
    }

    pub(crate) fn channel_feedback(&self, channel_offset: usize) -> u8 {
        (self.data[0x0b0 + channel_offset] >> 3) & 7
    }

    pub(crate) fn channel_pan(&self, channel_offset: usize) -> (bool, bool) {
        let value = self.data[0x0b4 + channel_offset];
        (value & 0x80 != 0, value & 0x40 != 0)
    }

    pub(crate) fn channel_lfo_am(&self, channel_offset: usize) -> u8 {
        (self.data[0x0b4 + channel_offset] >> 4) & 3
    }

    fn channel_lfo_pm(&self, channel_offset: usize) -> u8 {
        self.data[0x0b4 + channel_offset] & 7
    }

    fn channel_block_freq(&self, channel_offset: usize) -> u32 {
        (u32::from(self.data[0x0a4 + channel_offset] & 0x3f) << 8)
            | u32::from(self.data[0x0a0 + channel_offset])
    }

    fn multi_frequency(&self) -> bool {
        self.data[0x27] & 0xc0 != 0
    }

    fn multi_block_freq(&self, number: usize) -> u32 {
        (u32::from(self.data[0x0ac + number] & 0x3f) << 8) | u32::from(self.data[0x0a8 + number])
    }

    pub(crate) fn operator_config(
        &self,
        channel_offset: usize,
        operator_offset: usize,
    ) -> OperatorConfig {
        let mut block_freq = self.channel_block_freq(channel_offset);
        if self.multi_frequency() && channel_offset == 2 {
            block_freq = match operator_offset {
                2 => self.multi_block_freq(1),
                10 => self.multi_block_freq(2),
                6 => self.multi_block_freq(0),
                _ => block_freq,
            };
        }

        let keycode =
            ((block_freq >> 10) & 0x0f) << 1 | ((0xfe80u32 >> ((block_freq >> 7) & 0x0f)) & 1);
        let detune_raw = self.data[0x30 + operator_offset] >> 4 & 7;
        let detune = detune_adjustment(u32::from(detune_raw), keycode);
        let mut multiple = u32::from(self.data[0x30 + operator_offset] & 0x0f) * 2;
        if multiple == 0 {
            multiple = 1;
        }

        let ksr = keycode >> ((self.data[0x50 + operator_offset] >> 6 & 3) ^ 3);
        let attack = effective_rate(u32::from(self.data[0x50 + operator_offset] & 0x1f) * 2, ksr);
        let decay = effective_rate(u32::from(self.data[0x60 + operator_offset] & 0x1f) * 2, ksr);
        let sustain = effective_rate(u32::from(self.data[0x70 + operator_offset] & 0x1f) * 2, ksr);
        let release = effective_rate(
            u32::from(self.data[0x80 + operator_offset] & 0x0f) * 4 + 2,
            ksr,
        );

        let mut sustain_level = u32::from(self.data[0x80 + operator_offset] >> 4);
        sustain_level |= (sustain_level + 1) & 0x10;
        sustain_level <<= 5;

        OperatorConfig {
            block_freq,
            detune,
            multiple,
            pm_sensitivity: self.channel_lfo_pm(channel_offset),
            total_level: u32::from(self.data[0x40 + operator_offset] & 0x7f) << 3,
            sustain: sustain_level,
            rates: [0, attack, decay, sustain, release, 0],
            am_enabled: self.data[0x60 + operator_offset] & 0x80 != 0,
            ssg_enabled: self.data[0x90 + operator_offset] & 0x08 != 0,
            ssg_mode: self.data[0x90 + operator_offset] & 7,
        }
    }
}

fn effective_rate(raw_rate: u32, ksr: u32) -> u8 {
    if raw_rate == 0 {
        0
    } else {
        raw_rate.saturating_add(ksr).min(63) as u8
    }
}

fn detune_adjustment(detune: u32, keycode: u32) -> i32 {
    const TABLE: [[u8; 4]; 32] = [
        [0, 0, 1, 2],
        [0, 0, 1, 2],
        [0, 0, 1, 2],
        [0, 0, 1, 2],
        [0, 1, 2, 2],
        [0, 1, 2, 3],
        [0, 1, 2, 3],
        [0, 1, 2, 3],
        [0, 1, 2, 4],
        [0, 1, 3, 4],
        [0, 1, 3, 4],
        [0, 1, 3, 5],
        [0, 2, 4, 5],
        [0, 2, 4, 6],
        [0, 2, 4, 6],
        [0, 2, 5, 7],
        [0, 2, 5, 8],
        [0, 3, 6, 8],
        [0, 3, 6, 9],
        [0, 3, 7, 10],
        [0, 4, 8, 11],
        [0, 4, 8, 12],
        [0, 4, 9, 13],
        [0, 5, 10, 14],
        [0, 5, 11, 16],
        [0, 6, 12, 17],
        [0, 6, 13, 19],
        [0, 7, 14, 20],
        [0, 8, 16, 22],
        [0, 8, 16, 22],
        [0, 8, 16, 22],
        [0, 8, 16, 22],
    ];
    let value = i32::from(TABLE[(keycode & 31) as usize][(detune & 3) as usize]);
    if detune & 4 != 0 {
        -value
    } else {
        value
    }
}

#[cfg(test)]
mod tests {
    use super::OpnaRegisters;

    #[test]
    fn opna_channel_and_operator_maps_match_ymfm() {
        assert_eq!(OpnaRegisters::channel_offset(0), 0x000);
        assert_eq!(OpnaRegisters::channel_offset(3), 0x100);
        assert_eq!(OpnaRegisters::operator_offsets(0), [0, 8, 4, 12]);
        assert_eq!(OpnaRegisters::operator_offsets(2), [2, 10, 6, 14]);
        assert_eq!(
            OpnaRegisters::operator_offsets(5),
            [0x102, 0x10a, 0x106, 0x10e]
        );
    }

    #[test]
    fn frequency_latch_commits_upper_bits_only_after_lower_write() {
        let mut registers = OpnaRegisters::new();
        registers.reset();
        registers.write(0xa4, 0x12);
        assert_eq!(registers.read(0xa4), 0);
        registers.write(0xa0, 0x34);
        assert_eq!(registers.read(0xa0), 0x34);
        assert_eq!(registers.read(0xa4), 0x12 & 0x3f);
    }

    #[test]
    fn keyon_write_selects_bank_channel_and_operator_mask() {
        let mut registers = OpnaRegisters::new();
        assert_eq!(registers.write(0x28, 0xb6), Some((5, 0x0b)));
        assert_eq!(registers.write(0x28, 0x03), None);
    }
}
