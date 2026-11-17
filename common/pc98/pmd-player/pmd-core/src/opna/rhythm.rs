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

const RHYTHM_VOICES: usize = 6;
const RHYTHM_TL_POS: i32 = 32;
const RHYTHM_TL_TABLE: [u32; 160] = [
    0xfffff, 0xeac0b, 0xd744e, 0xc5671, 0xb504e, 0xa5fec, 0x9837e, 0x8b95b, 0x7ffff, 0x75605,
    0x6ba26, 0x62b38, 0x5a826, 0x52ff5, 0x4c1be, 0x45cad, 0x3ffff, 0x3ab02, 0x35d12, 0x3159b,
    0x2d412, 0x297fa, 0x260de, 0x22e56, 0x1ffff, 0x1d580, 0x1ae88, 0x18acd, 0x16a08, 0x14bfc,
    0x1306e, 0x1172a, 0xffff, 0xeabf, 0xd743, 0xc566, 0xb503, 0xa5fd, 0x9836, 0x8b94, 0x7fff,
    0x755f, 0x6ba1, 0x62b2, 0x5a81, 0x52fe, 0x4c1a, 0x45c9, 0x3fff, 0x3aaf, 0x35d0, 0x3158, 0x2d40,
    0x297e, 0x260c, 0x22e4, 0x1fff, 0x1d57, 0x1ae7, 0x18ab, 0x169f, 0x14be, 0x1305, 0x1171, 0xfff,
    0xeab, 0xd73, 0xc55, 0xb4f, 0xa5e, 0x982, 0x8b8, 0x7ff, 0x755, 0x6b9, 0x62a, 0x5a7, 0x52e,
    0x4c0, 0x45b, 0x3ff, 0x3aa, 0x35c, 0x314, 0x2d3, 0x296, 0x25f, 0x22d, 0x1ff, 0x1d4, 0x1ad,
    0x189, 0x169, 0x14a, 0x12f, 0x116, 0xff, 0xe9, 0xd6, 0xc4, 0xb4, 0xa4, 0x97, 0x8a, 0x7f, 0x74,
    0x6a, 0x61, 0x59, 0x51, 0x4b, 0x44, 0x3f, 0x39, 0x34, 0x30, 0x2c, 0x28, 0x25, 0x21, 0x1f, 0x1c,
    0x19, 0x17, 0x15, 0x13, 0x12, 0x10, 0xf, 0xd, 0xc, 0xb, 0xa, 0x9, 0x8, 0x7, 0x7, 0x6, 0x5, 0x5,
    0x4, 0x4, 0x3, 0x3, 0x3, 0x2, 0x2, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0,
];

#[derive(Clone, Debug)]
struct RhythmVoice {
    pan: u8,
    level: i8,
    volume: i32,
    sample: Option<Vec<i16>>,
    size: u32,
    pos: u32,
    step: u32,
    rate: u32,
}

impl RhythmVoice {
    fn new() -> Self {
        Self {
            pan: 0,
            level: 0,
            volume: 0,
            sample: None,
            size: 0,
            pos: 0,
            step: 0,
            rate: 0,
        }
    }

    fn clear_sample(&mut self) {
        self.sample = None;
        self.size = 0;
        self.pos = 0;
        self.step = 0;
        self.rate = 0;
    }
}

#[derive(Clone, Debug)]
pub(crate) struct RhythmEngine {
    voices: [RhythmVoice; RHYTHM_VOICES],
    total_volume: i32,
    total_level: i8,
    key_mask: u8,
    adpcm_rom_present: bool,

    peaks: [i32; RHYTHM_VOICES],
}

impl RhythmEngine {
    pub(crate) fn new() -> Self {
        Self {
            voices: core::array::from_fn(|_| RhythmVoice::new()),
            peaks: [0; RHYTHM_VOICES],
            total_volume: 0,
            total_level: 0,
            key_mask: 0,
            adpcm_rom_present: false,
        }
    }

    pub(crate) fn set_sample(
        &mut self,
        index: usize,
        sample_rate: u32,
        samples: &[i16],
        output_rate: u32,
    ) -> bool {
        if index >= RHYTHM_VOICES || samples.len() >= 0x100000 || output_rate == 0 {
            self.clear_samples();
            return false;
        }

        let voice = &mut self.voices[index];
        voice.sample = Some(samples.to_vec());
        voice.rate = sample_rate;
        voice.step = sample_rate.wrapping_mul(1024) / output_rate;
        voice.size = (samples.len() as u32).wrapping_mul(1024);
        voice.pos = voice.size;
        true
    }

    pub(crate) fn clear_samples(&mut self) {
        for voice in &mut self.voices {
            voice.clear_sample();
        }
        self.key_mask = 0;
    }

    pub(crate) fn set_adpcm_rom_present(&mut self, present: bool) {
        self.adpcm_rom_present = present;
    }

    pub(crate) fn write_register(&mut self, address: u8, data: u8) {
        if self.adpcm_rom_present {
            return;
        }

        match address {
            0x10 => {
                if data & 0x80 == 0 {
                    self.key_mask |= data & 0x3f;
                    for index in 0..RHYTHM_VOICES {
                        if data & (1 << index) != 0 {
                            self.voices[index].pos = 0;
                        }
                    }
                } else {
                    self.key_mask &= !data;
                }
            }
            0x11 => {
                self.total_level = (!data & 63) as i8;
            }
            0x18..=0x1d => {
                let voice = &mut self.voices[(address & 7) as usize];
                voice.pan = (data >> 6) & 3;
                voice.level = (!data & 31) as i8;
            }
            _ => {}
        }
    }

    pub(crate) fn set_volume(&mut self, index: usize, db: i32) {
        if let Some(voice) = self.voices.get_mut(index) {
            let db = db.min(20);
            voice.volume = -(db * 2 / 3);
        }
    }

    pub(crate) fn take_peaks(&mut self) -> [i32; RHYTHM_VOICES] {
        core::mem::take(&mut self.peaks)
    }

    pub(crate) fn render(&mut self, output: &mut [i32]) {
        if self.adpcm_rom_present
            || self.total_volume >= 128
            || self.key_mask & 0x3f == 0
            || !self.voices.iter().all(|voice| voice.sample.is_some())
        {
            return;
        }

        let frame_count = output.len() / 2;
        for index in 0..RHYTHM_VOICES {
            if self.key_mask & (1 << index) == 0 {
                continue;
            }

            let voice = &mut self.voices[index];
            let db =
                (self.total_level as i32 + self.total_volume + voice.level as i32 + voice.volume)
                    .clamp(-31, 127);
            let volume = (RHYTHM_TL_TABLE[(RHYTHM_TL_POS + db) as usize] >> 4) as i32;
            let mask_left = -(((voice.pan >> 1) & 1) as i32);
            let mask_right = -((voice.pan & 1) as i32);
            let samples = voice.sample.as_ref().expect("complete bank checked above");

            for frame in 0..frame_count {
                if voice.pos >= voice.size {
                    break;
                }
                let sample = (i32::from(samples[(voice.pos / 1024) as usize]) * volume) >> 12;
                let peak = &mut self.peaks[index];
                *peak = (*peak).max(sample.abs());
                voice.pos = voice.pos.wrapping_add(voice.step);
                let destination = frame * 2;

                output[destination] += sample & mask_left;
                output[destination + 1] += sample & mask_right;
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::RhythmEngine;

    fn complete_bank(engine: &mut RhythmEngine) {
        let samples = [1000, -2000, 3000, -4000];
        for index in 0..6 {
            assert!(engine.set_sample(index, 22_050, &samples, 44_100));
        }
    }

    #[test]
    fn table_and_default_level_match_cpp_vectors() {
        let mut engine = RhythmEngine::new();
        complete_bank(&mut engine);
        engine.write_register(0x18, 0x1f | 0xc0);
        engine.write_register(0x10, 0x01);

        let mut output = [0; 8];
        engine.render(&mut output);
        assert_eq!(output, [999, 999, 999, 999, -2000, -2000, -2000, -2000]);
    }

    #[test]
    fn pan_masks_match_cpp_vectors() {
        let expected = [
            (0, [0, 0, 0, 0]),
            (1, [0, 999, 0, 999]),
            (2, [999, 0, 999, 0]),
            (3, [999, 999, 999, 999]),
        ];

        for (pan, expected) in expected {
            let mut engine = RhythmEngine::new();
            complete_bank(&mut engine);
            engine.write_register(0x18, (pan << 6) | 0x1f);
            engine.write_register(0x10, 0x01);
            let mut output = [0; 4];
            engine.render(&mut output);
            assert_eq!(output, expected);
        }
    }

    #[test]
    fn level_and_per_voice_volume_match_cpp_vectors() {
        let mut engine = RhythmEngine::new();
        complete_bank(&mut engine);
        engine.write_register(0x18, 0xc0 | 0x1e);
        engine.write_register(0x10, 0x01);
        let mut output = [0; 4];
        engine.render(&mut output);
        assert_eq!(output, [916, 916, 916, 916]);

        engine.set_volume(0, 3);
        engine.write_register(0x18, 0xc0 | 0x1f);
        engine.write_register(0x10, 0x01);
        output = [0; 4];
        engine.render(&mut output);
        assert_eq!(output, [1188, 1188, 1188, 1188]);
    }

    #[test]
    fn fractional_step_and_retrigger_match_cpp_vectors() {
        let mut engine = RhythmEngine::new();
        complete_bank(&mut engine);
        engine.write_register(0x18, 0xc0 | 0x1f);
        engine.write_register(0x10, 0x01);

        let mut output = [0; 4];
        engine.render(&mut output);
        assert_eq!(output, [999, 999, 999, 999]);

        output = [0; 4];
        engine.render(&mut output);
        assert_eq!(output, [-2000, -2000, -2000, -2000]);

        engine.write_register(0x10, 0x01);
        output = [0; 4];
        engine.render(&mut output);
        assert_eq!(output, [999, 999, 999, 999]);
    }

    #[test]
    fn incomplete_bank_and_adpcm_rom_are_silent() {
        let mut engine = RhythmEngine::new();
        assert!(engine.set_sample(0, 22_050, &[1000], 44_100));
        engine.write_register(0x18, 0xc0 | 0x1f);
        engine.write_register(0x10, 0x01);
        let mut output = [7, -9, 11, -13];
        engine.render(&mut output);
        assert_eq!(output, [7, -9, 11, -13]);

        complete_bank(&mut engine);
        engine.set_adpcm_rom_present(true);
        engine.write_register(0x10, 0x01);
        output = [7, -9, 11, -13];
        engine.render(&mut output);
        assert_eq!(output, [7, -9, 11, -13]);
    }

    #[test]
    fn invalid_sample_configuration_clears_the_bank() {
        let mut engine = RhythmEngine::new();
        complete_bank(&mut engine);
        assert!(!engine.set_sample(0, 22_050, &[0; 0x100000], 44_100));
        engine.write_register(0x10, 0x01);
        let mut output = [0; 2];
        engine.render(&mut output);
        assert_eq!(output, [0, 0]);
    }
}
