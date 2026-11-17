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

use alloc::vec::Vec;

const ADPCM_A_CHANNELS: usize = 6;
const ADPCM_A_REGISTERS: usize = 0x30;
const ADPCM_B_REGISTERS: usize = 0x11;

#[inline]
fn clamp_i32(value: i32, minimum: i32, maximum: i32) -> i32 {
    value.clamp(minimum, maximum)
}

#[derive(Clone, Debug)]
struct AdpcmARegisters {
    data: [u8; ADPCM_A_REGISTERS],
}

impl AdpcmARegisters {
    fn new() -> Self {
        let mut registers = Self {
            data: [0; ADPCM_A_REGISTERS],
        };
        registers.reset();
        registers
    }

    fn reset(&mut self) {
        self.data = [0; ADPCM_A_REGISTERS];

        self.data[0x08..=0x0d].fill(0xdf);
    }

    fn write(&mut self, index: usize, value: u8) {
        if let Some(register) = self.data.get_mut(index) {
            *register = value;
        }
    }

    fn dump(&self) -> bool {
        self.data[0x00] & 0x80 != 0
    }

    fn dump_mask(&self) -> u32 {
        u32::from(self.data[0x00] & 0x3f)
    }

    fn total_level(&self) -> u32 {
        u32::from(self.data[0x01] & 0x3f)
    }

    fn ch_pan_left(&self, channel: usize) -> bool {
        self.data[channel + 0x08] & 0x80 != 0
    }

    fn ch_pan_right(&self, channel: usize) -> bool {
        self.data[channel + 0x08] & 0x40 != 0
    }

    fn ch_instrument_level(&self, channel: usize) -> u32 {
        u32::from(self.data[channel + 0x08] & 0x1f)
    }

    fn ch_start(&self, channel: usize) -> u32 {
        u32::from(self.data[channel + 0x10]) | (u32::from(self.data[channel + 0x18]) << 8)
    }

    fn ch_end(&self, channel: usize) -> u32 {
        u32::from(self.data[channel + 0x20]) | (u32::from(self.data[channel + 0x28]) << 8)
    }

    fn write_start(&mut self, channel: usize, address: u16) {
        self.write(channel + 0x10, address as u8);
        self.write(channel + 0x18, (address >> 8) as u8);
    }

    fn write_end(&mut self, channel: usize, address: u16) {
        self.write(channel + 0x20, address as u8);
        self.write(channel + 0x28, (address >> 8) as u8);
    }
}

#[derive(Clone, Debug)]
struct AdpcmAChannel {
    channel: usize,
    address_shift: u32,
    playing: bool,
    current_nibble: u32,
    current_byte: u8,
    current_address: u32,
    accumulator: i32,
    step_index: i32,
}

impl AdpcmAChannel {
    fn new(channel: usize, address_shift: u32) -> Self {
        Self {
            channel,
            address_shift,
            playing: false,
            current_nibble: 0,
            current_byte: 0,
            current_address: 0,
            accumulator: 0,
            step_index: 0,
        }
    }

    fn reset(&mut self) {
        self.playing = false;
        self.current_nibble = 0;
        self.current_byte = 0;
        self.current_address = 0;
        self.accumulator = 0;
        self.step_index = 0;
    }

    fn key_on_off(&mut self, on: bool, registers: &AdpcmARegisters) {
        self.playing = on;
        if on {
            self.current_address = registers.ch_start(self.channel) << self.address_shift;
            self.current_nibble = 0;
            self.current_byte = 0;
            self.accumulator = 0;
            self.step_index = 0;
        }
    }

    fn clock(&mut self, registers: &AdpcmARegisters, memory: &[u8]) -> bool {
        if !self.playing {
            self.accumulator = 0;
            return false;
        }

        let data = if self.current_nibble == 0 {
            let end = registers
                .ch_end(self.channel)
                .wrapping_add(1)
                .wrapping_shl(self.address_shift);
            if (self.current_address ^ end) & 0x0f_ffff == 0 {
                self.playing = false;
                self.accumulator = 0;
                return true;
            }

            self.current_byte = memory
                .get(self.current_address as usize)
                .copied()
                .unwrap_or(0);
            self.current_address = self.current_address.wrapping_add(1);
            self.current_nibble = 1;
            self.current_byte >> 4
        } else {
            self.current_nibble = 0;
            self.current_byte & 0x0f
        };

        const STEPS: [i32; 49] = [
            16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107,
            118, 130, 143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494, 544,
            598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552,
        ];
        let mut delta = (2 * i32::from(data & 7) + 1) * STEPS[self.step_index as usize] / 8;
        if data & 8 != 0 {
            delta = -delta;
        }

        self.accumulator = (self.accumulator + delta) & 0x0fff;

        const STEP_INCREMENT: [i32; 8] = [-1, -1, -1, -1, 2, 5, 7, 9];
        self.step_index = clamp_i32(self.step_index + STEP_INCREMENT[(data & 7) as usize], 0, 48);
        false
    }

    fn output(&self, registers: &AdpcmARegisters, output: &mut [i32; 2]) {
        let volume =
            (registers.ch_instrument_level(self.channel) ^ 0x1f) + (registers.total_level() ^ 0x3f);
        if volume >= 63 {
            return;
        }

        let multiplier = 15 - (volume & 7) as i32;
        let shift = 5 + (volume >> 3);

        let signed_accumulator = ((self.accumulator << 4) as i16) as i32;
        let value = ((signed_accumulator * multiplier) >> shift) & !3;

        if registers.ch_pan_left(self.channel) {
            output[0] += value;
        }
        if registers.ch_pan_right(self.channel) {
            output[1] += value;
        }
    }
}

#[derive(Clone, Debug)]
pub(crate) struct AdpcmAEngine {
    registers: AdpcmARegisters,
    channels: [AdpcmAChannel; ADPCM_A_CHANNELS],
    memory: Vec<u8>,
}

impl AdpcmAEngine {
    pub(crate) fn new() -> Self {
        let mut engine = Self {
            registers: AdpcmARegisters::new(),
            channels: core::array::from_fn(|channel| AdpcmAChannel::new(channel, 0)),
            memory: Vec::new(),
        };
        engine.reset();
        engine
    }

    pub(crate) fn reset(&mut self) {
        self.registers.reset();
        for channel in &mut self.channels {
            channel.reset();
        }
    }

    pub(crate) fn set_memory(&mut self, memory: &[u8]) {
        self.memory.clear();
        self.memory.extend_from_slice(memory);
    }

    pub(crate) fn set_start_end(&mut self, channel: usize, start: u16, end: u16) {
        if channel < ADPCM_A_CHANNELS {
            self.registers.write_start(channel, start);
            self.registers.write_end(channel, end);
        }
    }

    pub(crate) fn write(&mut self, register: usize, data: u8) {
        self.registers.write(register, data);
        if register == 0x00 {
            for channel in 0..ADPCM_A_CHANNELS {
                if data & (1 << channel) != 0 {
                    self.channels[channel].key_on_off(data & 0x80 == 0, &self.registers);
                }
            }
        }
    }

    pub(crate) fn clock(&mut self, channel_mask: u32) -> u32 {
        let mut finished = 0;
        for channel in 0..ADPCM_A_CHANNELS {
            if channel_mask & (1 << channel) != 0
                && self.channels[channel].clock(&self.registers, &self.memory)
            {
                finished |= 1 << channel;
            }
        }
        finished
    }

    pub(crate) fn output(&self, output: &mut [i32; 2], channel_mask: u32) {
        for channel in 0..ADPCM_A_CHANNELS {
            if channel_mask & (1 << channel) != 0 {
                self.channels[channel].output(&self.registers, output);
            }
        }
    }

    #[cfg(test)]
    fn dump_register(&self) -> bool {
        self.registers.dump()
    }

    #[cfg(test)]
    fn dump_mask(&self) -> u32 {
        self.registers.dump_mask()
    }
}

#[derive(Clone, Debug)]
struct AdpcmBRegisters {
    data: [u8; ADPCM_B_REGISTERS],
}

impl AdpcmBRegisters {
    fn new() -> Self {
        let mut registers = Self {
            data: [0; ADPCM_B_REGISTERS],
        };
        registers.reset();
        registers
    }

    fn reset(&mut self) {
        self.data = [0; ADPCM_B_REGISTERS];

        self.data[0x0c] = 0xff;
        self.data[0x0d] = 0xff;
    }

    fn write(&mut self, index: usize, value: u8) {
        if let Some(register) = self.data.get_mut(index) {
            *register = value;
        }
    }

    fn execute(&self) -> bool {
        self.data[0x00] & 0x80 != 0
    }

    fn record(&self) -> bool {
        self.data[0x00] & 0x40 != 0
    }

    fn external(&self) -> bool {
        self.data[0x00] & 0x20 != 0
    }

    fn repeat(&self) -> bool {
        self.data[0x00] & 0x10 != 0
    }

    fn pan_left(&self) -> bool {
        self.data[0x01] & 0x80 != 0
    }

    fn pan_right(&self) -> bool {
        self.data[0x01] & 0x40 != 0
    }

    fn reset_flag(&self) -> bool {
        self.data[0x00] & 1 != 0
    }

    fn dram_8bit(&self) -> bool {
        self.data[0x01] & 2 != 0
    }

    fn rom_ram(&self) -> bool {
        self.data[0x01] & 1 != 0
    }

    fn start(&self) -> u32 {
        u32::from(self.data[0x02]) | (u32::from(self.data[0x03]) << 8)
    }

    fn end(&self) -> u32 {
        u32::from(self.data[0x04]) | (u32::from(self.data[0x05]) << 8)
    }

    fn delta_n(&self) -> u32 {
        u32::from(self.data[0x09]) | (u32::from(self.data[0x0a]) << 8)
    }

    fn level(&self) -> u32 {
        u32::from(self.data[0x0b])
    }

    fn limit(&self) -> u32 {
        u32::from(self.data[0x0c]) | (u32::from(self.data[0x0d]) << 8)
    }

    fn cpudata(&self) -> u8 {
        self.data[0x08]
    }
}

const ADPCM_B_STATUS_EOS: u32 = 0x01;
const ADPCM_B_STATUS_BRDY: u32 = 0x02;
const ADPCM_B_STATUS_PLAYING: u32 = 0x04;

#[derive(Clone, Debug)]
struct AdpcmBChannel {
    address_shift_constant: u32,
    status: u32,
    current_nibble: u32,
    current_byte: u8,
    dummy_read: u32,
    position: u32,
    current_address: u32,
    accumulator: i32,
    previous_accumulator: i32,
    adpcm_step: i32,
}

impl AdpcmBChannel {
    fn new(address_shift_constant: u32) -> Self {
        Self {
            address_shift_constant,
            status: ADPCM_B_STATUS_BRDY,
            current_nibble: 0,
            current_byte: 0,
            dummy_read: 0,
            position: 0,
            current_address: 0,
            accumulator: 0,
            previous_accumulator: 0,
            adpcm_step: 127,
        }
    }

    fn reset(&mut self) {
        self.status = ADPCM_B_STATUS_BRDY;
        self.current_nibble = 0;
        self.current_byte = 0;
        self.dummy_read = 0;
        self.position = 0;
        self.current_address = 0;
        self.accumulator = 0;
        self.previous_accumulator = 0;
        self.adpcm_step = 127;
    }

    fn address_shift(&self, registers: &AdpcmBRegisters) -> u32 {
        if self.address_shift_constant != 0 {
            self.address_shift_constant
        } else if registers.rom_ram() || registers.dram_8bit() {
            5
        } else {
            2
        }
    }

    fn at_limit(&self, registers: &AdpcmBRegisters) -> bool {
        let end = registers
            .limit()
            .wrapping_add(1)
            .wrapping_shl(self.address_shift(registers))
            .wrapping_sub(1);
        self.current_address == end
    }

    fn at_end(&self, registers: &AdpcmBRegisters) -> bool {
        let end = registers
            .end()
            .wrapping_add(1)
            .wrapping_shl(self.address_shift(registers))
            .wrapping_sub(1);
        self.current_address == end
    }

    fn load_start(&mut self, registers: &AdpcmBRegisters) {
        self.status = (self.status & !ADPCM_B_STATUS_EOS) | ADPCM_B_STATUS_PLAYING;
        self.current_address = if registers.external() {
            registers
                .start()
                .wrapping_shl(self.address_shift(registers))
        } else {
            0
        };
        self.current_nibble = 0;
        self.current_byte = 0;
        self.position = 0;
        self.accumulator = 0;
        self.previous_accumulator = 0;
        self.adpcm_step = 127;
    }

    fn clock(&mut self, registers: &AdpcmBRegisters, memory: &[u8]) {
        if !registers.execute() || registers.record() || self.status & ADPCM_B_STATUS_PLAYING == 0 {
            self.status &= !ADPCM_B_STATUS_PLAYING;
            return;
        }

        let position = self.position + registers.delta_n();
        self.position = position as u16 as u32;
        if position < 0x1_0000 {
            return;
        }

        if self.current_nibble == 0 && registers.external() {
            self.current_byte = memory
                .get(self.current_address as usize)
                .copied()
                .unwrap_or(0);
        }

        let data = if self.current_nibble == 0 {
            self.current_byte >> 4
        } else {
            self.current_byte & 0x0f
        };
        self.current_nibble ^= 1;

        if self.current_nibble == 0 {
            if registers.external() {
                if self.at_end(registers) {
                    if registers.repeat() {
                        self.load_start(registers);
                    } else {
                        self.accumulator = 0;
                        self.previous_accumulator = 0;
                        self.status = (self.status & !ADPCM_B_STATUS_PLAYING) | ADPCM_B_STATUS_EOS;
                        return;
                    }
                } else if self.at_limit(registers) {
                    self.current_address = 0;
                } else {
                    self.current_address = (self.current_address + 1) & 0x00ff_ffff;
                }
            } else {
                self.current_byte = registers.cpudata();
                self.status |= ADPCM_B_STATUS_BRDY;
            }
        }

        self.previous_accumulator = self.accumulator;
        let mut delta = (2 * i32::from(data & 7) + 1) * self.adpcm_step / 8;
        if data & 8 != 0 {
            delta = -delta;
        }
        self.accumulator = clamp_i32(self.accumulator + delta, -32768, 32767);

        const STEP_SCALE: [i32; 8] = [57, 57, 57, 57, 77, 102, 128, 153];
        self.adpcm_step = clamp_i32(
            (self.adpcm_step * STEP_SCALE[(data & 7) as usize]) / 64,
            127,
            24576,
        );
    }

    fn output(&self, registers: &AdpcmBRegisters, output: &mut [i32; 2], right_shift: u32) {
        let interpolation_weight = (self.position ^ 0xffff).wrapping_add(1) as i32;
        let mut result = (self.previous_accumulator * interpolation_weight
            + self.accumulator * self.position as i32)
            >> 16;
        result = (result * registers.level() as i32) >> (8 + right_shift);

        if registers.pan_left() {
            output[0] += result;
        }
        if registers.pan_right() {
            output[1] += result;
        }
    }

    fn read(&mut self, registers: &AdpcmBRegisters, memory: &[u8], register: usize) -> u8 {
        if register != 0x08 || registers.execute() || registers.record() || !registers.external() {
            return 0;
        }

        if self.dummy_read != 0 {
            self.load_start(registers);
            self.dummy_read -= 1;
            return 0;
        }

        let result = memory
            .get(self.current_address as usize)
            .copied()
            .unwrap_or(0);
        self.current_address = self.current_address.wrapping_add(1);
        if self.at_end(registers) {
            self.status = ADPCM_B_STATUS_EOS | ADPCM_B_STATUS_BRDY;
        } else {
            self.status = ADPCM_B_STATUS_BRDY;
        }
        if self.at_limit(registers) {
            self.current_address = 0;
        }
        result
    }

    fn write(
        &mut self,
        registers: &AdpcmBRegisters,
        memory: &mut Vec<u8>,
        register: usize,
        value: u8,
    ) {
        if register == 0x00 {
            if registers.execute() {
                self.load_start(registers);
            } else {
                self.status &= !ADPCM_B_STATUS_EOS;
            }
            if registers.reset_flag() {
                self.reset();
            }
            if registers.external() {
                self.dummy_read = 2;
            }
        } else if register == 0x08 {
            if registers.execute() && !registers.record() && !registers.external() {
                self.status &= !ADPCM_B_STATUS_BRDY;
            } else if !registers.execute() && registers.record() && registers.external() {
                if self.dummy_read != 0 {
                    self.load_start(registers);
                    self.dummy_read = 0;
                }

                if self.at_end(registers) {
                    self.status = ADPCM_B_STATUS_EOS | ADPCM_B_STATUS_BRDY;
                } else {
                    let address = self.current_address as usize;
                    if address >= memory.len() {
                        memory.resize(address + 1, 0);
                    }
                    memory[address] = value;
                    self.current_address = self.current_address.wrapping_add(1);
                    self.status = ADPCM_B_STATUS_BRDY;
                }
            }
        }
    }
}

#[derive(Clone, Debug)]
pub(crate) struct AdpcmBEngine {
    registers: AdpcmBRegisters,
    channel: AdpcmBChannel,
    memory: Vec<u8>,
}

impl AdpcmBEngine {
    pub(crate) fn new() -> Self {
        let mut engine = Self {
            registers: AdpcmBRegisters::new(),
            channel: AdpcmBChannel::new(0),
            memory: Vec::new(),
        };
        engine.reset();
        engine
    }

    pub(crate) fn reset(&mut self) {
        self.registers.reset();
        self.channel.reset();
    }

    pub(crate) fn set_memory(&mut self, memory: &[u8]) {
        self.memory.clear();
        self.memory.extend_from_slice(memory);
    }

    pub(crate) fn read(&mut self, register: usize) -> u8 {
        self.channel.read(&self.registers, &self.memory, register)
    }

    pub(crate) fn write(&mut self, register: usize, data: u8) {
        self.registers.write(register, data);
        self.channel
            .write(&self.registers, &mut self.memory, register, data);
    }

    pub(crate) fn clock(&mut self) {
        self.channel.clock(&self.registers, &self.memory);
    }

    pub(crate) fn output(&self, output: &mut [i32; 2], right_shift: u32) {
        self.channel.output(&self.registers, output, right_shift);
    }

    pub(crate) fn status(&self) -> u8 {
        self.channel.status as u8
    }
}

#[cfg(test)]
mod tests {
    use super::{AdpcmAEngine, AdpcmBEngine, ADPCM_B_STATUS_BRDY, ADPCM_B_STATUS_PLAYING};
    use alloc::vec::Vec;

    #[test]
    fn adpcm_a_decoding_pan_and_inclusive_end_match_cpp_vectors() {
        let mut engine = AdpcmAEngine::new();
        engine.set_memory(&[0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc]);
        engine.write(0x01, 0x3f);
        engine.write(0x08, 0xdf);
        engine.set_start_end(0, 0, 2);
        engine.write(0x00, 0x01);

        let expected = [
            [44, 44],
            [120, 120],
            [224, 224],
            [360, 360],
            [552, 552],
            [928, 928],
            [0, 0],
        ];
        for expected in expected {
            engine.clock(1);
            let mut output = [0; 2];
            engine.output(&mut output, 1);
            assert_eq!(output, expected);
        }
    }

    #[test]
    fn adpcm_a_pan_end_and_silent_total_level_match_cpp_vectors() {
        let mut engine = AdpcmAEngine::new();
        engine.set_memory(&[0x12, 0x34]);
        engine.write(0x01, 0x3f);
        engine.write(0x08, 0x5f);
        engine.set_start_end(0, 0, 0);
        engine.write(0x00, 0x01);

        for expected in [[0, 44], [0, 120], [0, 0]] {
            engine.clock(1);
            let mut output = [0; 2];
            engine.output(&mut output, 1);
            assert_eq!(output, expected);
        }

        engine.reset();
        engine.set_memory(&[0x12]);
        engine.write(0x01, 0x00);
        engine.write(0x08, 0xdf);
        engine.set_start_end(0, 0, 0);
        engine.write(0x00, 0x01);
        engine.clock(1);
        let mut output = [0; 2];
        engine.output(&mut output, 1);
        assert_eq!(output, [0, 0]);
    }

    #[test]
    fn adpcm_a_register_control_and_empty_memory_are_safe() {
        let mut engine = AdpcmAEngine::new();
        engine.set_start_end(0, 0, 0);
        engine.write(0x00, 0x81);
        assert!(engine.dump_register());
        assert_eq!(engine.dump_mask(), 1);
        engine.clock(1);
        let mut output = [0; 2];
        engine.output(&mut output, 1);
        assert_eq!(output, [0, 0]);

        engine.write(0x00, 0x01);
        engine.clock(1);
        engine.output(&mut output, 1);
        assert_eq!(output, [0, 0]);
    }

    #[test]
    fn signed_nibbles_and_wrapping_or_clamped_accumulators_match_cpp_vectors() {
        let mut adpcm_a = AdpcmAEngine::new();
        adpcm_a.set_memory(&[0x8f, 0x71, 0xf1]);
        adpcm_a.write(0x01, 0x3f);
        adpcm_a.write(0x08, 0xdf);
        adpcm_a.set_start_end(0, 0, 2);
        adpcm_a.write(0x00, 0x01);
        let expected_a = [
            [-16, -16],
            [-240, -240],
            [276, 276],
            [524, 524],
            [-600, -600],
        ];
        for expected in expected_a {
            adpcm_a.clock(1);
            let mut output = [0; 2];
            adpcm_a.output(&mut output, 1);
            assert_eq!(output, expected);
        }

        let mut adpcm_b = AdpcmBEngine::new();
        adpcm_b.set_memory(&[0x8f, 0x7f, 0xf1, 0x08]);
        configure_adpcm_b(&mut adpcm_b, 0xa0, 0xff, 0xc0);
        let expected_b = [
            [0, 0],
            [-8, -8],
            [-127, -127],
            [156, 156],
            [-519, -519],
            [-2135, -2135],
        ];
        for expected in expected_b {
            adpcm_b.clock();
            let mut output = [0; 2];
            adpcm_b.output(&mut output, 1);
            assert_eq!(output, expected);
        }
    }

    fn configure_adpcm_b(engine: &mut AdpcmBEngine, control: u8, level: u8, pan: u8) {
        engine.write(0x01, pan | 0x03);
        engine.write(0x02, 0x00);
        engine.write(0x03, 0x00);
        engine.write(0x04, 0x00);
        engine.write(0x05, 0x00);
        engine.write(0x09, 0xff);
        engine.write(0x0a, 0xff);
        engine.write(0x0b, level);
        engine.write(0x00, control);
    }

    #[test]
    fn adpcm_b_interpolation_pan_and_status_match_cpp_vectors() {
        let mut engine = AdpcmBEngine::new();
        engine.set_memory(&(0..32).map(|value| 0x10 + value).collect::<Vec<_>>());
        configure_adpcm_b(&mut engine, 0xa0, 0xff, 0xc0);

        let expected = [
            [0, 0],
            [22, 22],
            [30, 30],
            [53, 53],
            [77, 77],
            [100, 100],
            [139, 139],
            [163, 163],
            [218, 218],
            [242, 242],
            [312, 312],
            [341, 341],
        ];
        for expected in expected {
            engine.clock();
            let mut output = [0; 2];
            engine.output(&mut output, 1);
            assert_eq!(output, expected);
            assert_eq!(
                engine.status(),
                (ADPCM_B_STATUS_BRDY | ADPCM_B_STATUS_PLAYING) as u8
            );
        }
    }

    #[test]
    fn adpcm_b_pan_and_level_match_cpp_vectors() {
        let mut engine = AdpcmBEngine::new();
        engine.set_memory(&(0..32).map(|value| 0x10 + value).collect::<Vec<_>>());
        configure_adpcm_b(&mut engine, 0xa0, 0x80, 0x80);

        for expected in [[0, 0], [11, 0], [15, 0], [27, 0], [38, 0], [50, 0]] {
            engine.clock();
            let mut output = [0; 2];
            engine.output(&mut output, 1);
            assert_eq!(output, expected);
        }
    }

    #[test]
    fn adpcm_b_repeat_and_missing_memory_keep_the_reference_state_machine() {
        let mut engine = AdpcmBEngine::new();
        engine.set_memory(&(0..32).map(|value| 0x10 + value).collect::<Vec<_>>());
        configure_adpcm_b(&mut engine, 0xb0, 0xff, 0xc0);
        for _ in 0..130 {
            engine.clock();
        }
        assert_eq!(
            engine.status(),
            (ADPCM_B_STATUS_BRDY | ADPCM_B_STATUS_PLAYING) as u8
        );

        let mut output = [0; 2];
        engine.clock();
        engine.output(&mut output, 1);
        assert_eq!(output, [-119, -119]);

        let mut empty = AdpcmBEngine::new();
        configure_adpcm_b(&mut empty, 0xa0, 0xff, 0xc0);
        for _ in 0..4 {
            empty.clock();
        }
        output = [0; 2];
        empty.output(&mut output, 1);

        assert_eq!(output, [21, 21]);
    }

    #[test]
    fn adpcm_b_cpu_mode_read_and_write_are_in_memory() {
        let mut engine = AdpcmBEngine::new();

        engine.write(0x01, 0xc1);
        engine.write(0x02, 0x00);
        engine.write(0x03, 0x00);
        engine.write(0x04, 0x00);
        engine.write(0x05, 0x00);
        engine.write(0x00, 0x20);
        assert_eq!(engine.read(0x08), 0);
        assert_eq!(engine.read(0x08), 0);
        engine.write(0x00, 0x40);
        engine.write(0x08, 0xa5);
        engine.write(0x08, 0x5a);

        engine.write(0x00, 0x00);
        engine.clock();
        assert_eq!(engine.status() & ADPCM_B_STATUS_PLAYING as u8, 0);
    }
}
