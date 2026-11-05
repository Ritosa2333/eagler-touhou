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

use super::channel::FmChannel;
use super::registers::OpnaRegisters;

const CHANNELS: usize = 6;
const ALL_CHANNELS: u32 = (1 << CHANNELS) - 1;

#[derive(Clone, Debug)]
pub(crate) struct FmEngine {
    registers: OpnaRegisters,
    channels: [FmChannel; CHANNELS],
    env_counter: u32,
    active_channels: u32,
    modified_channels: u32,
    prepare_count: u32,

    peaks: [i32; CHANNELS],
}

impl FmEngine {
    pub(crate) fn new() -> Self {
        let mut engine = Self {
            registers: OpnaRegisters::new(),
            peaks: [0; CHANNELS],
            channels: [FmChannel::new(); CHANNELS],
            env_counter: 0,
            active_channels: ALL_CHANNELS,
            modified_channels: ALL_CHANNELS,
            prepare_count: 0,
        };
        engine.reset();
        engine
    }

    pub(crate) fn reset(&mut self) {
        self.registers.reset();
        self.env_counter = 0;
        self.active_channels = ALL_CHANNELS;
        self.modified_channels = ALL_CHANNELS;
        self.prepare_count = 0;
        for channel in &mut self.channels {
            channel.reset();
        }
    }

    pub(crate) fn registers(&self) -> &OpnaRegisters {
        &self.registers
    }

    pub(crate) fn write(&mut self, index: u16, value: u8) {
        self.modified_channels = ALL_CHANNELS;
        if let Some((channel, mask)) = self.registers.write(index, value) {
            self.channels[channel].keyonoff(mask);
        }
    }

    fn prepare(&mut self, channel_mask: u32) {
        self.active_channels = 0;
        let lfo_enabled = self.registers.lfo_enabled();
        for channel_index in 0..CHANNELS {
            if channel_mask & (1 << channel_index) == 0 {
                continue;
            }
            let channel_offset = OpnaRegisters::channel_offset(channel_index);
            let operator_offsets = OpnaRegisters::operator_offsets(channel_index);
            let configs = operator_offsets.map(|operator_offset| {
                self.registers
                    .operator_config(channel_offset, operator_offset)
            });
            let channel = &mut self.channels[channel_index];
            channel.set_algorithm(self.registers.channel_algorithm(channel_offset));
            channel.set_feedback_level(self.registers.channel_feedback(channel_offset));
            let (left, right) = self.registers.channel_pan(channel_offset);
            channel.set_pan(left, right);
            if channel.prepare(configs, lfo_enabled) {
                self.active_channels |= 1 << channel_index;
            }
        }
        self.modified_channels = 0;
        self.prepare_count = 0;
    }

    pub(crate) fn clock(&mut self, channel_mask: u32) -> u32 {
        if self.modified_channels != 0 || self.prepare_count >= 4096 {
            self.prepare(channel_mask);
        } else {
            self.prepare_count += 1;
        }

        self.env_counter = self.env_counter.wrapping_add(1);
        if self.env_counter & 3 == 3 {
            self.env_counter = self.env_counter.wrapping_add(1);
        }

        let raw_pm = self.registers.clock_lfo();
        for channel_index in 0..CHANNELS {
            if channel_mask & (1 << channel_index) != 0 {
                self.channels[channel_index].clock_with_lfo(self.env_counter, raw_pm);
            }
        }
        self.env_counter
    }

    pub(crate) fn output(&mut self, rshift: u32, clipmax: i32, channel_mask: u32) -> [i32; 2] {
        let mut output = [0; 2];
        let mask = channel_mask & self.active_channels;
        for channel_index in 0..CHANNELS {
            if mask & (1 << channel_index) == 0 {
                continue;
            }
            let channel_offset = OpnaRegisters::channel_offset(channel_index);
            let am_offset = self.registers.lfo_am_offset(channel_offset);
            let channel_output = self.channels[channel_index].render(am_offset, rshift, clipmax);

            let magnitude = channel_output[0].abs().max(channel_output[1].abs());
            let peak = &mut self.peaks[channel_index];
            *peak = (*peak).max(magnitude);
            output[0] += channel_output[0];
            output[1] += channel_output[1];
        }
        output
    }

    pub(crate) fn take_peaks(&mut self) -> [i32; CHANNELS] {
        core::mem::take(&mut self.peaks)
    }
}

#[cfg(test)]
mod tests {
    use super::{FmEngine, ALL_CHANNELS};

    fn write(engine: &mut FmEngine, register: u16, value: u8) {
        engine.write(register, value);
    }

    fn configure_carrier(engine: &mut FmEngine) {
        write(engine, 0xa4, 0x01);
        write(engine, 0xa0, 0x80);
        write(engine, 0xb0, 0x07);
        write(engine, 0xb4, 0xc0);
        for base in [0x00, 0x04, 0x08, 0x0c] {
            write(engine, 0x30 + base, 0x01);
            write(engine, 0x40 + base, 0x00);
            write(engine, 0x50 + base, 0x1f);
            write(engine, 0x60 + base, 0x00);
            write(engine, 0x70 + base, 0x00);
            write(engine, 0x80 + base, 0x0f);
        }
        write(engine, 0x28, 0xf0);
    }

    #[test]
    fn register_replay_produces_deterministic_fm_samples_after_key_on() {
        let mut first = FmEngine::new();
        configure_carrier(&mut first);
        let mut samples = [[0; 2]; 8];
        for sample in &mut samples {
            first.clock(ALL_CHANNELS);
            *sample = first.output(1, 32767, ALL_CHANNELS);
        }
        assert_eq!(
            samples,
            [
                [48, 48],
                [48, 48],
                [48, 48],
                [48, 48],
                [48, 48],
                [148, 148],
                [148, 148],
                [148, 148],
            ]
        );

        let mut second = FmEngine::new();
        configure_carrier(&mut second);
        for expected in samples {
            second.clock(ALL_CHANNELS);
            assert_eq!(second.output(1, 32767, ALL_CHANNELS), expected);
        }
        assert!(samples.iter().any(|sample| *sample != [0, 0]));
    }

    #[test]
    fn key_on_is_applied_on_the_first_clock_not_at_register_write() {
        let mut engine = FmEngine::new();
        configure_carrier(&mut engine);
        assert_eq!(engine.output(1, 32767, ALL_CHANNELS), [0, 0]);
        engine.clock(ALL_CHANNELS);
        assert_ne!(engine.output(1, 32767, ALL_CHANNELS), [0, 0]);
    }
}
