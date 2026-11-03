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

use super::chip::Ym2608;
use super::resampler::SincResampler;
use alloc::collections::VecDeque;

const FIXED_TIME_SHIFT: u32 = 48;
const FIXED_TIME_ONE: u64 = 1u64 << FIXED_TIME_SHIFT;
const SOURCE_RATE: u32 = 55_466;

#[derive(Clone, Copy, Debug)]
struct ChipScheduler {
    source_step: u64,
    output_step: u64,
    source_position: u64,
    output_position: u64,
    last: [i32; 2],
}

impl ChipScheduler {
    fn new(chip_rate: u32, output_rate: u32) -> Option<Self> {
        if chip_rate == 0 || output_rate == 0 {
            return None;
        }
        Some(Self {
            source_step: FIXED_TIME_ONE / u64::from(chip_rate),
            output_step: FIXED_TIME_ONE / u64::from(output_rate),
            source_position: 0,
            output_position: 0,
            last: [0; 2],
        })
    }

    fn render<F>(&mut self, output: &mut [[i32; 2]], mut generate: F)
    where
        F: FnMut() -> [i32; 2],
    {
        for destination in output {
            while self.source_position <= self.output_position {
                self.last = generate();
                self.source_position = self.source_position.wrapping_add(self.source_step);
            }
            destination[0] += self.last[0];
            destination[1] += self.last[1];
            self.output_position = self.output_position.wrapping_add(self.output_step);
        }
    }
}

#[derive(Clone, Debug)]
pub struct ChipRenderer {
    chip: Ym2608,
    scheduler: ChipScheduler,
    interpolation: bool,
    target_rate: u32,
    sinc: Option<SincResampler>,
}

impl ChipRenderer {
    pub fn new(clock_hz: u32, target_rate: u32, interpolation: bool) -> Option<Self> {
        if clock_hz == 0 || target_rate == 0 {
            return None;
        }
        let chip = Ym2608::new(clock_hz);
        let scheduler_rate = if interpolation {
            SOURCE_RATE
        } else {
            target_rate
        };
        let scheduler = ChipScheduler::new(chip.internal_sample_rate(), scheduler_rate)?;
        let sinc = if interpolation && target_rate != SOURCE_RATE {
            SincResampler::new(SOURCE_RATE, target_rate)
        } else {
            None
        };
        Some(Self {
            chip,
            scheduler,
            interpolation,
            target_rate,
            sinc,
        })
    }

    pub fn write(&mut self, address: u16, value: u8) {
        self.chip.write(address, value);
    }

    pub fn set_fm_volume(&mut self, volume: i32) {
        self.chip.set_fm_volume(volume);
    }

    pub fn set_psg_volume(&mut self, volume: i32) {
        self.chip.set_psg_volume(volume);
    }

    pub(crate) fn chip_mut(&mut self) -> &mut Ym2608 {
        &mut self.chip
    }

    pub(crate) fn set_target_rate(&mut self, target_rate: u32) -> bool {
        if target_rate == 0 || self.interpolation {
            return false;
        }
        self.target_rate = target_rate;
        self.scheduler.output_step = FIXED_TIME_ONE / u64::from(target_rate);
        true
    }

    pub fn take_voice_peaks(&mut self) -> crate::VoicePeaks {
        self.chip.take_voice_peaks()
    }

    pub(crate) fn set_channel_mute(&mut self, channel: usize, muted: bool) -> bool {
        self.chip.set_channel_mute(channel, muted)
    }

    pub fn target_rate(&self) -> u32 {
        self.target_rate
    }

    pub fn interpolation(&self) -> bool {
        self.interpolation
    }

    pub(crate) fn clear_buffer(&mut self) {
        if let Some(sinc) = self.sinc.as_mut() {
            sinc.reset();
        }
    }

    pub fn render(&mut self, output: &mut [[i32; 2]]) {
        let mut empty = VecDeque::new();
        self.render_with_prefill(output, &mut empty);
    }

    pub(crate) fn render_with_prefill(
        &mut self,
        output: &mut [[i32; 2]],
        prebuffer: &mut VecDeque<[i32; 2]>,
    ) {
        let Self {
            chip,
            scheduler,
            sinc,
            ..
        } = self;
        let mut generate_source = || {
            let sample = chip.generate_internal();
            let mut stereo = [sample[0] + sample[2], sample[1] + sample[2]];
            let mut rhythm = [stereo[0], stereo[1]];
            chip.render_rhythm(&mut rhythm);
            stereo = rhythm;
            stereo
        };

        let mut fill_source = |source: &mut [[i32; 2]]| {
            let mut index = 0;
            while index < source.len() {
                let Some(sample) = prebuffer.pop_front() else {
                    break;
                };
                source[index] = sample;
                index += 1;
            }
            if index < source.len() {
                scheduler.render(&mut source[index..], &mut generate_source);
            }
        };

        if let Some(sinc) = sinc {
            sinc.render(output, &mut fill_source);
        } else {
            fill_source(output);
        }
    }

    pub(crate) fn render_source(&mut self, output: &mut [[i32; 2]]) {
        let Self {
            chip, scheduler, ..
        } = self;
        let mut generate_source = || {
            let sample = chip.generate_internal();
            let mut stereo = [sample[0] + sample[2], sample[1] + sample[2]];
            let mut rhythm = [stereo[0], stereo[1]];
            chip.render_rhythm(&mut rhythm);
            stereo = rhythm;
            stereo
        };
        scheduler.render(output, &mut generate_source);
    }
}

#[cfg(test)]
mod tests {
    use super::{ChipRenderer, ChipScheduler, FIXED_TIME_ONE, SOURCE_RATE};

    fn configure_carrier(renderer: &mut ChipRenderer) {
        renderer.write(0xa4, 0x01);
        renderer.write(0xa0, 0x80);
        renderer.write(0xb0, 0x07);
        renderer.write(0xb4, 0xc0);
        for base in [0x00, 0x04, 0x08, 0x0c] {
            renderer.write(0x30 + base, 0x01);
            renderer.write(0x40 + base, 0x00);
            renderer.write(0x50 + base, 0x1f);
            renderer.write(0x60 + base, 0x00);
            renderer.write(0x70 + base, 0x00);
            renderer.write(0x80 + base, 0x0f);
        }
        renderer.write(0x28, 0xf0);
    }

    #[test]
    fn scheduler_matches_floor_boundary_and_last_sample_semantics() {
        let mut scheduler = ChipScheduler::new(4, 6).expect("valid rates");
        let mut source = 0;
        let mut output = [[0; 2]; 6];
        scheduler.render(&mut output, || {
            source += 1;
            [source * 10, source * 20]
        });
        assert_eq!(
            output,
            [[10, 20], [10, 20], [20, 40], [20, 40], [30, 60], [40, 80]]
        );
    }

    #[test]
    fn renderer_uses_the_reference_55466_hz_source_path() {
        let renderer = ChipRenderer::new(8_000_000, 44_100, true).expect("valid renderer");
        assert_eq!(renderer.target_rate(), 44_100);
        assert!(renderer.interpolation());
        assert_eq!(SOURCE_RATE, 55_466);
    }

    #[test]
    fn renderer_uses_target_rate_for_the_direct_path() {
        let renderer = ChipRenderer::new(8_000_000, 44_100, false).expect("valid renderer");
        assert_eq!(renderer.scheduler.output_step, FIXED_TIME_ONE / 44_100);
    }

    #[test]
    fn direct_rate_change_preserves_scheduler_position() {
        let mut renderer = ChipRenderer::new(8_000_000, 44_100, false).expect("valid renderer");
        let mut output = [[0; 2]; 3];
        renderer.render(&mut output);
        let source_position = renderer.scheduler.source_position;
        let output_position = renderer.scheduler.output_position;

        assert!(renderer.set_target_rate(55_467));
        assert_eq!(renderer.target_rate(), 55_467);
        assert_eq!(renderer.scheduler.source_position, source_position);
        assert_eq!(renderer.scheduler.output_position, output_position);
        assert_eq!(renderer.scheduler.output_step, FIXED_TIME_ONE / 55_467);
    }

    #[test]
    fn renderer_rejects_zero_rates() {
        assert!(ChipRenderer::new(0, 44_100, false).is_none());
        assert!(ChipRenderer::new(8_000_000, 0, false).is_none());
        assert!(ChipRenderer::new(8_000_000, 0, true).is_none());
    }

    #[test]
    fn source_rate_frames_match_the_pinned_ymfm_scheduler_vector() {
        let mut renderer =
            ChipRenderer::new(8_000_000, SOURCE_RATE, false).expect("valid renderer");
        renderer.chip_mut().set_fm_volume(32768);
        configure_carrier(&mut renderer);
        let mut output = [[0; 2]; 12];
        renderer.render(&mut output);
        assert_eq!(
            output,
            [
                [48, 48],
                [48, 48],
                [48, 48],
                [48, 48],
                [48, 48],
                [148, 148],
                [148, 148],
                [148, 148],
                [148, 148],
                [148, 148],
                [248, 248],
                [248, 248],
            ]
        );
    }
}
