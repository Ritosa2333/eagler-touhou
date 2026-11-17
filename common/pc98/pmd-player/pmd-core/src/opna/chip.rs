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

use super::adpcm::{AdpcmAEngine, AdpcmBEngine};
use super::fm::FmEngine;
use super::resampler::SsgResampler;
use super::rhythm::RhythmEngine;
use super::ssg::Ssg;
use crate::VoicePeaks;

const ALL_CHANNELS: u32 = 0x3f;

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
enum Fidelity {
    Max,
    Min,
    Med,
}

#[derive(Clone, Debug)]
pub(crate) struct Ym2608 {
    clock_hz: u32,
    fidelity: Fidelity,
    prescale: u8,
    fm_samples_per_output: u8,
    fm: FmEngine,
    ssg: Ssg,
    ssg_resampler: SsgResampler,
    adpcm_a: AdpcmAEngine,
    adpcm_b: AdpcmBEngine,
    rhythm: RhythmEngine,
    adpcm_rom_present: bool,
    irq_enable: u8,
    fm_volume: i32,
    psg_volume: i32,
    last_fm: [i32; 2],
    fm_mute_mask: u32,
    ssg_mute_mask: u8,
    adpcm_muted: bool,
    rhythm_muted: bool,

    ssg_peaks: [i32; 3],

    adpcm_b_peak: i32,

    adpcm_a_peak: i32,
}

impl Ym2608 {
    pub(crate) fn new(clock_hz: u32) -> Self {
        let mut chip = Self {
            clock_hz,
            fidelity: Fidelity::Max,
            prescale: 6,
            fm_samples_per_output: 18,
            fm: FmEngine::new(),
            ssg: Ssg::new(),
            ssg_resampler: SsgResampler::new(),
            adpcm_a: AdpcmAEngine::new(),
            adpcm_b: AdpcmBEngine::new(),
            rhythm: RhythmEngine::new(),
            adpcm_rom_present: false,
            irq_enable: 0x1f,
            fm_volume: 65536,
            psg_volume: 65536,
            last_fm: [0; 2],
            fm_mute_mask: 0,
            ssg_mute_mask: 0,
            adpcm_muted: false,
            rhythm_muted: false,
            ssg_peaks: [0; 3],
            adpcm_b_peak: 0,
            adpcm_a_peak: 0,
        };
        chip.configure_prescale(6);
        chip.reset();
        chip
    }

    pub(crate) fn clock_hz(&self) -> u32 {
        self.clock_hz
    }

    pub(crate) fn internal_sample_rate(&self) -> u32 {
        match self.fidelity {
            Fidelity::Max => self.clock_hz / 8,
            Fidelity::Med => self.clock_hz / 24,
            Fidelity::Min => self.clock_hz / 48,
        }
    }

    pub(crate) fn reset(&mut self) {
        self.fm.reset();
        self.ssg.reset();
        self.adpcm_a.reset();
        self.adpcm_b.reset();
        self.last_fm = [0; 2];
        self.irq_enable = 0x1f;
        self.adpcm_a.set_start_end(0, 0x0000, 0x01bf);
        self.adpcm_a.set_start_end(1, 0x01c0, 0x043f);
        self.adpcm_a.set_start_end(2, 0x0440, 0x1b7f);
        self.adpcm_a.set_start_end(3, 0x1b80, 0x1cff);
        self.adpcm_a.set_start_end(4, 0x1d00, 0x1f7f);
        self.adpcm_a.set_start_end(5, 0x1f80, 0x1fff);
    }

    pub(crate) fn set_fidelity_max(&mut self) {
        self.fidelity = Fidelity::Max;
        self.configure_prescale(self.prescale);
    }

    pub(crate) fn set_fidelity_medium(&mut self) {
        self.fidelity = Fidelity::Med;
        self.configure_prescale(self.prescale);
    }

    pub(crate) fn set_fidelity_min(&mut self) {
        self.fidelity = Fidelity::Min;
        self.configure_prescale(self.prescale);
    }

    pub(crate) fn set_fm_volume(&mut self, volume: i32) {
        self.fm_volume = volume;
    }

    pub(crate) fn set_psg_volume(&mut self, volume: i32) {
        self.psg_volume = volume;
    }

    pub(crate) fn set_channel_mute(&mut self, channel: usize, muted: bool) -> bool {
        match channel {
            0..=5 => set_fm_bit(&mut self.fm_mute_mask, channel, muted),
            6..=8 => set_ssg_bit(&mut self.ssg_mute_mask, channel - 6, muted),
            9 => {
                self.adpcm_muted = muted;
                true
            }
            10 => {
                self.rhythm_muted = muted;
                true
            }
            _ => false,
        }
    }

    pub(crate) fn set_adpcm_a_memory(&mut self, memory: &[u8]) {
        self.adpcm_a.set_memory(memory);
        self.adpcm_rom_present = true;
        self.rhythm.set_adpcm_rom_present(true);
    }

    pub(crate) fn set_adpcm_b_memory(&mut self, memory: &[u8]) {
        self.adpcm_b.set_memory(memory);
    }

    pub(crate) fn set_rhythm_sample(
        &mut self,
        index: usize,
        sample_rate: u32,
        samples: &[i16],
        output_rate: u32,
    ) -> bool {
        self.rhythm
            .set_sample(index, sample_rate, samples, output_rate)
    }

    pub(crate) fn render_rhythm(&mut self, output: &mut [i32; 2]) {
        let mut rhythm = [0; 2];
        self.rhythm.render(&mut rhythm);
        if !self.rhythm_muted {
            output[0] += rhythm[0];
            output[1] += rhythm[1];
        }
    }

    pub(crate) fn write(&mut self, address: u16, value: u8) {
        match address {
            0x00..=0x0f => self.ssg.write(address as usize, value),
            0x10..=0x1f if self.adpcm_rom_present => {
                self.adpcm_a.write((address & 0x0f) as usize, value)
            }
            0x10..=0x1f => self.rhythm.write_register(address as u8, value),
            0x29 => self.irq_enable = value,
            0x2d => self.configure_prescale(6),
            0x2e if self.prescale == 6 => self.configure_prescale(3),
            0x2f => self.configure_prescale(2),
            0x20..=0xff => self.fm.write(address, value),
            0x100..=0x10f => self.adpcm_b.write((address & 0x0f) as usize, value),
            0x110 => {}
            0x111..=0x1ff => self.fm.write(address, value),
            _ => {}
        }
    }

    fn configure_prescale(&mut self, prescale: u8) {
        self.prescale = prescale;
        match self.fidelity {
            Fidelity::Min => match prescale {
                3 => {
                    self.fm_samples_per_output = 0;
                    self.ssg_resampler.configure(1, 3);
                }
                2 => {
                    self.fm_samples_per_output = 1;
                    self.ssg_resampler.configure(1, 6);
                }
                _ => {
                    self.fm_samples_per_output = 3;
                    self.ssg_resampler.configure(2, 3);
                }
            },
            Fidelity::Med => match prescale {
                3 => {
                    self.fm_samples_per_output = 3;
                    self.ssg_resampler.configure(2, 3);
                }
                2 => {
                    self.fm_samples_per_output = 2;
                    self.ssg_resampler.configure(1, 3);
                }
                _ => {
                    self.fm_samples_per_output = 6;
                    self.ssg_resampler.configure(4, 3);
                }
            },
            Fidelity::Max => match prescale {
                3 => {
                    self.fm_samples_per_output = 9;
                    self.ssg_resampler.configure(2, 1);
                }
                2 => {
                    self.fm_samples_per_output = 6;
                    self.ssg_resampler.configure(1, 1);
                }
                _ => {
                    self.fm_samples_per_output = 18;
                    self.ssg_resampler.configure(4, 1);
                }
            },
        }
    }

    fn clock_fm_and_adpcm(&mut self) {
        let fm_mask = if self.irq_enable & 0x80 != 0 {
            ALL_CHANNELS
        } else {
            0x07
        };
        let env_counter = self.fm.clock(ALL_CHANNELS);
        if env_counter & 3 == 0 {
            let adpcm_mask = if env_counter & 4 != 0 {
                0x0f
            } else {
                ALL_CHANNELS
            };
            self.adpcm_a.clock(adpcm_mask);
        }
        self.adpcm_b.clock();

        let output_mask = fm_mask & !self.fm_mute_mask;
        let mut output = self.fm.output(1, 32767, output_mask);

        let before_a = output;
        let adpcm_mask = if self.adpcm_muted { 0 } else { ALL_CHANNELS };
        self.adpcm_a.output(&mut output, adpcm_mask);
        let a_delta = (output[0] - before_a[0])
            .abs()
            .max((output[1] - before_a[1]).abs());
        self.adpcm_a_peak = self.adpcm_a_peak.max(a_delta);
        let before_b = output;
        self.adpcm_b
            .output(&mut output, if self.adpcm_muted { 0 } else { 1 });
        let b_delta = (output[0] - before_b[0])
            .abs()
            .max((output[1] - before_b[1]).abs());
        self.adpcm_b_peak = self.adpcm_b_peak.max(b_delta);
        for sample in &mut output {
            *sample = clip16((*sample * self.fm_volume) / (65536 / 2));
        }
        self.last_fm = output;
    }

    pub(crate) fn take_voice_peaks(&mut self) -> VoicePeaks {
        const FULL_SCALE: f32 = 32_767.0;

        let fm_gain = self.fm_volume as f32 / (65_536.0 / 2.0) / FULL_SCALE;
        let ssg_gain = self.psg_volume as f32 / 65_536.0 / FULL_SCALE;
        let fm_peaks = self.fm.take_peaks();
        let rhythm_peaks = self.rhythm.take_peaks();
        let ssg_peaks = core::mem::take(&mut self.ssg_peaks);
        VoicePeaks {
            fm: core::array::from_fn(|i| (fm_peaks[i] as f32 * fm_gain).clamp(0.0, 1.0)),
            ssg: core::array::from_fn(|i| (ssg_peaks[i] as f32 * ssg_gain).clamp(0.0, 1.0)),

            rhythm: core::array::from_fn(|i| (rhythm_peaks[i] as f32 / FULL_SCALE).clamp(0.0, 1.0)),
            adpcm: (core::mem::take(&mut self.adpcm_b_peak) as f32 * fm_gain).clamp(0.0, 1.0),
            extension: (core::mem::take(&mut self.adpcm_a_peak) as f32 * fm_gain).clamp(0.0, 1.0),
        }
    }

    pub(crate) fn generate_internal(&mut self) -> [i32; 3] {
        let sample_index = self.ssg_resampler.sample_index();
        let fm;
        if self.fm_samples_per_output != 0 {
            if sample_index.is_multiple_of(u32::from(self.fm_samples_per_output)) {
                self.clock_fm_and_adpcm();
            }
            fm = self.last_fm;
        } else {
            let step = sample_index % 3;
            if step == 0 {
                self.clock_fm_and_adpcm();
            }
            let mut selected = self.last_fm;
            if step == 1 {
                self.clock_fm_and_adpcm();
                selected[0] = (selected[0] + self.last_fm[0]) / 2;
                selected[1] = (selected[1] + self.last_fm[1]) / 2;
            }
            fm = selected;
        }

        let ssg = &mut self.ssg;
        let ssg_peaks = &mut self.ssg_peaks;
        let mut ssg_output = [0];
        self.ssg_resampler.render(&mut ssg_output, || {
            ssg.clock();
            let mut output = [0; 3];
            ssg.output(&mut output);

            for (peak, sample) in ssg_peaks.iter_mut().zip(&output) {
                *peak = (*peak).max(sample.abs());
            }
            output
        });
        for (channel, sample) in ssg_output.iter_mut().enumerate() {
            if self.ssg_mute_mask & (1 << channel) != 0 {
                *sample = 0;
            }
        }
        let ssg_sample = (ssg_output[0] * self.psg_volume) / 65536;
        [fm[0], fm[1], ssg_sample]
    }
}

fn set_fm_bit(mask: &mut u32, bit: usize, enabled: bool) -> bool {
    if enabled {
        *mask |= 1 << bit;
    } else {
        *mask &= !(1 << bit);
    }
    true
}

fn set_ssg_bit(mask: &mut u8, bit: usize, enabled: bool) -> bool {
    if enabled {
        *mask |= 1 << bit;
    } else {
        *mask &= !(1 << bit);
    }
    true
}

#[inline]
fn clip16(value: i32) -> i32 {
    value.clamp(-32768, 32767)
}

#[cfg(test)]
mod tests {
    use super::Ym2608;

    fn configure_carrier(chip: &mut Ym2608) {
        chip.write(0xa4, 0x01);
        chip.write(0xa0, 0x80);
        chip.write(0xb0, 0x07);
        chip.write(0xb4, 0xc0);
        for base in [0x00, 0x04, 0x08, 0x0c] {
            chip.write(0x30 + base, 0x01);
            chip.write(0x40 + base, 0x00);
            chip.write(0x50 + base, 0x1f);
            chip.write(0x60 + base, 0x00);
            chip.write(0x70 + base, 0x00);
            chip.write(0x80 + base, 0x0f);
        }
        chip.write(0x28, 0xf0);
    }

    #[test]
    fn internal_output_matches_the_pinned_ymfm_synthetic_vector() {
        let mut chip = Ym2608::new(8_000_000);
        chip.set_fm_volume(32768);
        configure_carrier(&mut chip);
        let mut samples = [[0; 3]; 8];
        for frame in 0..(samples.len() * 18) {
            let sample = chip.generate_internal();
            if frame % 18 == 0 {
                samples[frame / 18] = sample;
            }
        }
        assert_eq!(
            samples,
            [
                [48, 48, 0],
                [48, 48, 0],
                [48, 48, 0],
                [48, 48, 0],
                [48, 48, 0],
                [148, 148, 0],
                [148, 148, 0],
                [148, 148, 0],
            ]
        );
    }

    #[test]
    fn fm_channel_mute_suppresses_output_without_rejecting_the_channel() {
        let mut chip = Ym2608::new(8_000_000);
        chip.set_fm_volume(32768);
        configure_carrier(&mut chip);
        assert!(chip.set_channel_mute(0, true));
        let mut sample = [0; 3];
        for _ in 0..18 {
            sample = chip.generate_internal();
        }
        assert_eq!(sample[0], 0);
        assert_eq!(sample[1], 0);
        assert!(chip.set_channel_mute(0, false));
    }

    #[test]
    fn ssg_mixed_output_matches_the_pinned_ymfm_vector() {
        let mut chip = Ym2608::new(8_000_000);
        chip.set_fm_volume(0);
        chip.write(0x07, 0x08);
        chip.write(0x08, 0x0f);
        chip.write(0x00, 2);
        let mut samples = [[0; 3]; 12];
        for sample in &mut samples {
            *sample = chip.generate_internal();
        }
        assert_eq!(
            samples,
            [
                [0, 0, 0],
                [0, 0, 0],
                [0, 0, 0],
                [0, 0, 0],
                [0, 0, 10921],
                [0, 0, 10921],
                [0, 0, 10921],
                [0, 0, 10921],
                [0, 0, 10921],
                [0, 0, 10921],
                [0, 0, 10921],
                [0, 0, 10921],
            ]
        );
    }
}
