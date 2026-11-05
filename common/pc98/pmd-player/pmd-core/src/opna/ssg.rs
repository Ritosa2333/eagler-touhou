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

const AMPLITUDES: [i32; 32] = [
    0, 32, 78, 141, 178, 222, 262, 306, 369, 441, 509, 585, 701, 836, 965, 1112, 1334, 1595, 1853,
    2146, 2576, 3081, 3576, 4135, 5000, 6006, 7023, 8155, 9963, 11976, 14132, 16382,
];

#[derive(Clone, Copy, Debug)]
pub(crate) struct Ssg {
    registers: [u8; 16],
    tone_count: [u32; 3],
    tone_state: [u32; 3],
    envelope_count: u32,
    envelope_state: u32,
    noise_count: u32,
    noise_state: u32,
}

impl Ssg {
    pub(crate) const fn new() -> Self {
        Self {
            registers: [0; 16],
            tone_count: [0; 3],
            tone_state: [0; 3],
            envelope_count: 0,
            envelope_state: 0,
            noise_count: 0,
            noise_state: 1,
        }
    }

    pub(crate) fn reset(&mut self) {
        self.registers = [0; 16];
        self.tone_count = [0; 3];
        self.tone_state = [0; 3];
        self.envelope_count = 0;
        self.envelope_state = 0;
        self.noise_count = 0;
        self.noise_state = 1;
    }

    pub(crate) fn read(&self, register: usize) -> u8 {
        self.registers[register]
    }

    pub(crate) fn write(&mut self, register: usize, data: u8) {
        self.registers[register] = data;
        if register == 0x0d {
            self.envelope_state = 0;
        }
    }

    pub(crate) fn clock(&mut self) {
        for channel in 0..3 {
            self.tone_count[channel] = self.tone_count[channel].wrapping_add(1);
            if self.tone_count[channel] >= self.channel_tone_period(channel) {
                self.tone_state[channel] ^= 1;
                self.tone_count[channel] = 0;
            }
        }

        self.noise_count = self.noise_count.wrapping_add(1);
        if (self.noise_count >> 1) >= self.noise_period() && self.noise_count != 1 {
            let feedback = (self.noise_state & 1) ^ ((self.noise_state >> 3) & 1);
            self.noise_state ^= feedback << 17;
            self.noise_state >>= 1;
            self.noise_count = 0;
        }

        self.envelope_count = self.envelope_count.wrapping_add(1);
        if self.envelope_count >= self.envelope_period() {
            self.envelope_state = self.envelope_state.wrapping_add(1);
            self.envelope_count = 0;
        }
    }

    pub(crate) fn output(&mut self, output: &mut [i32; 3]) {
        let envelope_volume = self.envelope_volume();
        for (channel, sample) in output.iter_mut().enumerate() {
            let noise_on = self.channel_noise_enable_n(channel) | self.noise_state;
            let tone_on = self.channel_tone_enable_n(channel) | self.tone_state[channel];

            let volume = if noise_on & tone_on == 0 {
                0
            } else if self.channel_envelope_enable(channel) {
                envelope_volume
            } else {
                let mut volume = self.channel_amplitude(channel) * 2;
                if volume != 0 {
                    volume |= 1;
                }
                volume
            };
            *sample = AMPLITUDES[volume as usize];
        }
    }

    pub(crate) fn tone_state(&self, channel: usize) -> u32 {
        self.tone_state[channel]
    }

    pub(crate) fn noise_state(&self) -> u32 {
        self.noise_state
    }

    pub(crate) fn envelope_state(&self) -> u32 {
        self.envelope_state
    }

    fn channel_tone_period(&self, channel: usize) -> u32 {
        u32::from(self.registers[channel * 2])
            | (u32::from(self.registers[channel * 2 + 1] & 0x0f) << 8)
    }

    fn noise_period(&self) -> u32 {
        u32::from(self.registers[0x06] & 0x1f)
    }

    fn envelope_period(&self) -> u32 {
        u32::from(self.registers[0x0b]) | (u32::from(self.registers[0x0c]) << 8)
    }

    fn channel_noise_enable_n(&self, channel: usize) -> u32 {
        u32::from((self.registers[0x07] >> (3 + channel)) & 1)
    }

    fn channel_tone_enable_n(&self, channel: usize) -> u32 {
        u32::from((self.registers[0x07] >> channel) & 1)
    }

    fn channel_envelope_enable(&self, channel: usize) -> bool {
        self.registers[0x08 + channel] & 0x10 != 0
    }

    fn channel_amplitude(&self, channel: usize) -> u32 {
        u32::from(self.registers[0x08 + channel] & 0x0f)
    }

    fn envelope_volume(&mut self) -> u32 {
        let continue_flag = u32::from((self.registers[0x0d] >> 3) & 1);
        let mut attack = u32::from((self.registers[0x0d] >> 2) & 1);
        let alternate = u32::from((self.registers[0x0d] >> 1) & 1);
        let hold = u32::from(self.registers[0x0d] & 1);

        if (hold | (continue_flag ^ 1)) != 0 && self.envelope_state >= 32 {
            self.envelope_state = 32;
            if (attack ^ alternate) & continue_flag != 0 {
                31
            } else {
                0
            }
        } else {
            if alternate != 0 {
                attack ^= (self.envelope_state >> 5) & 1;
            }
            (self.envelope_state & 31) ^ if attack != 0 { 0 } else { 31 }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::Ssg;

    #[test]
    fn register_masks_and_periods_match_cpp() {
        let mut ssg = Ssg::new();
        ssg.reset();
        ssg.write(0x00, 0x34);
        ssg.write(0x01, 0x0f);
        ssg.write(0x06, 0xff);
        assert_eq!(ssg.read(0x01), 0x0f);
        assert_eq!(ssg.channel_tone_period(0), 0x0f34);
        assert_eq!(ssg.noise_period(), 0x1f);
    }

    #[test]
    fn tone_divider_and_output_match_cpp_vectors() {
        let mut ssg = Ssg::new();
        ssg.write(0x00, 2);
        ssg.write(0x06, 31);
        ssg.write(0x07, 0x08);
        ssg.write(0x08, 0x0f);
        let mut output = [0; 3];
        let expected = [0, 0, 16382, 16382, 0, 0];
        for expected_sample in expected {
            ssg.output(&mut output);
            assert_eq!(output[0], expected_sample);
            ssg.clock();
        }
    }

    #[test]
    fn noise_lfsr_and_gate_match_cpp_vectors() {
        let mut ssg = Ssg::new();
        ssg.write(0x07, 0x01);
        ssg.write(0x06, 1);
        ssg.write(0x08, 0x0f);
        let expected = [
            (0x00000001, 16382),
            (0x00000001, 16382),
            (0x00010000, 0),
            (0x00010000, 0),
            (0x00008000, 0),
            (0x00008000, 0),
            (0x00004000, 0),
            (0x00004000, 0),
        ];
        let mut output = [0; 3];
        for (expected_state, expected_sample) in expected {
            ssg.output(&mut output);
            assert_eq!(
                (ssg.noise_state(), output[0]),
                (expected_state, expected_sample)
            );
            ssg.clock();
        }
    }

    #[test]
    fn envelope_shapes_match_cpp_vectors() {
        let cases = [
            (
                0,
                [
                    (0, 16382),
                    (1, 14132),
                    (2, 11976),
                    (3, 9963),
                    (31, 0),
                    (32, 0),
                ],
            ),
            (
                4,
                [(0, 0), (1, 32), (2, 78), (3, 141), (31, 16382), (32, 0)],
            ),
            (
                8,
                [
                    (0, 16382),
                    (1, 14132),
                    (2, 11976),
                    (3, 9963),
                    (31, 0),
                    (32, 16382),
                ],
            ),
            (
                12,
                [(0, 0), (1, 32), (2, 78), (3, 141), (31, 16382), (32, 0)],
            ),
        ];
        for (shape, expected) in cases {
            let mut ssg = Ssg::new();
            ssg.write(0x07, 0x3f);
            ssg.write(0x08, 0x10);
            ssg.write(0x0b, 1);
            ssg.write(0x0d, shape);
            let mut output = [0; 3];
            for (index, (expected_state, expected_sample)) in expected.into_iter().enumerate() {
                while ssg.envelope_state() != expected_state {
                    ssg.clock();
                }
                ssg.output(&mut output);
                assert_eq!(
                    (ssg.envelope_state(), output[0]),
                    (expected_state, expected_sample),
                    "shape={shape} index={index}"
                );
            }
        }
    }

    #[test]
    fn envelope_shape_write_resets_state() {
        let mut ssg = Ssg::new();
        ssg.write(0x0b, 1);
        for _ in 0..4 {
            ssg.clock();
        }
        assert_eq!(ssg.envelope_state(), 4);
        ssg.write(0x0d, 0);
        assert_eq!(ssg.envelope_state(), 0);
    }
}
