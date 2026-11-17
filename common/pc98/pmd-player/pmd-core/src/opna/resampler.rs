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

const FIXED_TIME_SHIFT: u32 = 48;
const FIXED_TIME_ONE: u64 = 1u64 << FIXED_TIME_SHIFT;
const SINC_BUFFER_SIZE: usize = 2048;
const SINC_TAPS: i32 = 128;

const SINC_PI: f64 = core::f64::consts::PI;

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
enum SsgRatio {
    Nop,
    OneToOne,
    TwoToOne,
    FourToOne,
    OneToThree,
    OneToSix,
    TwoToThree,
    FourToThree,
    TwoToNine,
}

#[derive(Clone, Debug)]
pub(crate) struct SsgResampler {
    sample_index: u32,
    ratio: SsgRatio,
    last: [i32; 3],
}

impl SsgResampler {
    pub(crate) fn new() -> Self {
        Self {
            sample_index: 0,
            ratio: SsgRatio::Nop,
            last: [0; 3],
        }
    }

    pub(crate) fn configure(&mut self, output_samples: u8, source_samples: u8) -> bool {
        let ratio = match (output_samples, source_samples) {
            (4, 1) => SsgRatio::FourToOne,
            (2, 1) => SsgRatio::TwoToOne,
            (4, 3) => SsgRatio::FourToThree,
            (1, 1) => SsgRatio::OneToOne,
            (2, 3) => SsgRatio::TwoToThree,
            (1, 3) => SsgRatio::OneToThree,
            (2, 9) => SsgRatio::TwoToNine,
            (1, 6) => SsgRatio::OneToSix,
            (0, 0) => SsgRatio::Nop,
            _ => return false,
        };
        self.ratio = ratio;
        true
    }

    pub(crate) fn sample_index(&self) -> u32 {
        self.sample_index
    }

    pub(crate) fn render<F>(&mut self, output: &mut [i32], mut clock_and_output: F)
    where
        F: FnMut() -> [i32; 3],
    {
        match self.ratio {
            SsgRatio::Nop => {
                self.sample_index = self.sample_index.wrapping_add(output.len() as u32);
            }
            SsgRatio::OneToOne => self.render_repeat(output, 1, &mut clock_and_output),
            SsgRatio::TwoToOne => self.render_repeat(output, 2, &mut clock_and_output),
            SsgRatio::FourToOne => self.render_repeat(output, 4, &mut clock_and_output),
            SsgRatio::OneToThree => self.render_average(output, 3, &mut clock_and_output),
            SsgRatio::OneToSix => self.render_average(output, 6, &mut clock_and_output),
            SsgRatio::TwoToThree => self.render_two_to_three(output, &mut clock_and_output),
            SsgRatio::FourToThree => self.render_four_to_three(output, &mut clock_and_output),
            SsgRatio::TwoToNine => self.render_two_to_nine(output, &mut clock_and_output),
        }
    }

    fn add_last(sum: &mut [i32; 3], last: &[i32; 3], scale: i32) {
        for index in 0..3 {
            sum[index] += last[index] * scale;
        }
    }

    fn clock_and_add<F>(&mut self, sum: &mut [i32; 3], clock_and_output: &mut F, scale: i32)
    where
        F: FnMut() -> [i32; 3],
    {
        self.last = clock_and_output();
        Self::add_last(sum, &self.last, scale);
    }

    fn write_output(&mut self, output: &mut [i32], index: usize, sum: [i32; 3], divisor: i32) {
        output[index] = (sum[0] + sum[1] + sum[2]) * 2 / (3 * divisor);
        self.sample_index = self.sample_index.wrapping_add(1);
    }

    fn render_repeat<F>(&mut self, output: &mut [i32], multiplier: u32, clock_and_output: &mut F)
    where
        F: FnMut() -> [i32; 3],
    {
        for destination in output.iter_mut() {
            if self.sample_index.is_multiple_of(multiplier) {
                self.last = clock_and_output();
            }
            *destination = (self.last[0] + self.last[1] + self.last[2]) * 2 / 3;
            self.sample_index = self.sample_index.wrapping_add(1);
        }
    }

    fn render_average<F>(&mut self, output: &mut [i32], divisor: i32, clock_and_output: &mut F)
    where
        F: FnMut() -> [i32; 3],
    {
        for index in 0..output.len() {
            let mut sum = [0; 3];
            for _ in 0..divisor {
                self.clock_and_add(&mut sum, clock_and_output, 1);
            }
            self.write_output(output, index, sum, divisor);
        }
    }

    fn render_two_to_three<F>(&mut self, output: &mut [i32], clock_and_output: &mut F)
    where
        F: FnMut() -> [i32; 3],
    {
        for index in 0..output.len() {
            let mut sum = [0; 3];
            if self.sample_index & 1 == 0 {
                self.clock_and_add(&mut sum, clock_and_output, 2);
                self.clock_and_add(&mut sum, clock_and_output, 1);
            } else {
                Self::add_last(&mut sum, &self.last, 1);
                self.clock_and_add(&mut sum, clock_and_output, 2);
            }
            self.write_output(output, index, sum, 3);
        }
    }

    fn render_four_to_three<F>(&mut self, output: &mut [i32], clock_and_output: &mut F)
    where
        F: FnMut() -> [i32; 3],
    {
        for index in 0..output.len() {
            let mut sum = [0; 3];
            let step = self.sample_index & 3;
            Self::add_last(&mut sum, &self.last, step as i32);
            if step != 3 {
                self.clock_and_add(&mut sum, clock_and_output, 3 - step as i32);
            }
            self.write_output(output, index, sum, 3);
        }
    }

    fn render_two_to_nine<F>(&mut self, output: &mut [i32], clock_and_output: &mut F)
    where
        F: FnMut() -> [i32; 3],
    {
        for index in 0..output.len() {
            let mut sum = [0; 3];
            if self.sample_index & 1 != 0 {
                Self::add_last(&mut sum, &self.last, 1);
            }
            for _ in 0..4 {
                self.clock_and_add(&mut sum, clock_and_output, 2);
            }
            if self.sample_index & 1 == 0 {
                self.clock_and_add(&mut sum, clock_and_output, 1);
            }
            self.write_output(output, index, sum, 9);
        }
    }
}

#[derive(Clone, Copy, Debug)]
pub(crate) struct OpnaScheduler {
    source_step: u64,
    output_step: u64,
    source_position: u64,
    output_position: u64,
    last: [i32; 3],
    generated_samples: u64,
}

impl OpnaScheduler {
    pub(crate) fn new(source_rate: u32, output_rate: u32) -> Option<Self> {
        if source_rate == 0 || output_rate == 0 {
            return None;
        }
        Some(Self {
            source_step: FIXED_TIME_ONE / u64::from(source_rate),
            output_step: FIXED_TIME_ONE / u64::from(output_rate),
            source_position: 0,
            output_position: 0,
            last: [0; 3],
            generated_samples: 0,
        })
    }

    pub(crate) fn render<F>(&mut self, output: &mut [[i32; 2]], mut generate: F)
    where
        F: FnMut() -> [i32; 3],
    {
        for destination in output {
            while self.source_position <= self.output_position {
                self.last = generate();
                self.source_position = self.source_position.wrapping_add(self.source_step);
                self.generated_samples = self.generated_samples.wrapping_add(1);
            }
            destination[0] += self.last[0] + self.last[2];
            destination[1] += self.last[1] + self.last[2];
            self.output_position = self.output_position.wrapping_add(self.output_step);
        }
    }

    pub(crate) fn generated_samples(&self) -> u64 {
        self.generated_samples
    }
}

#[inline]
fn clip16(value: i32) -> i32 {
    value.clamp(-32768, 32767)
}

pub(crate) fn scale_fm_and_clip(fm: &mut [i32; 2], volume: i32) {
    for value in fm {
        *value = clip16((*value * volume) / (65536 / 2));
    }
}

pub(crate) fn scale_ssg(ssg: &mut i32, volume: i32) {
    *ssg = (*ssg * volume) / 65536;
}

#[derive(Clone, Debug)]
pub(crate) struct SincResampler {
    source_rate: u32,
    output_rate: u32,
    ring: [[i32; 2]; SINC_BUFFER_SIZE],
    rest: f64,
    write_position: i32,
}

impl SincResampler {
    pub(crate) fn new(source_rate: u32, output_rate: u32) -> Option<Self> {
        if source_rate == 0 || output_rate == 0 {
            return None;
        }
        let mut resampler = Self {
            source_rate,
            output_rate,
            ring: [[0; 2]; SINC_BUFFER_SIZE],
            rest: 0.0,
            write_position: SINC_TAPS,
        };
        resampler.reset();
        Some(resampler)
    }

    pub(crate) fn reset(&mut self) {
        self.ring = [[0; 2]; SINC_BUFFER_SIZE];
        self.rest = 0.0;
        self.write_position = SINC_TAPS;
    }

    pub(crate) fn render<F>(&mut self, output: &mut [[i32; 2]], mut fill_source: F)
    where
        F: FnMut(&mut [[i32; 2]]),
    {
        let source_ratio = f64::from(self.source_rate) / f64::from(self.output_rate);
        for index in 0..output.len() {
            let integer_rest = self.rest as i32;
            if self.write_position - (integer_rest + SINC_TAPS) < 0 {
                let remaining = (output.len() - index - 1) as f64;
                let mut refill =
                    (self.rest + remaining * source_ratio) as i32 + SINC_TAPS - self.write_position;
                if self.write_position + refill - SINC_BUFFER_SIZE as i32 > integer_rest {
                    refill = integer_rest + SINC_BUFFER_SIZE as i32 - self.write_position;
                }
                self.refill(refill, &mut fill_source);
            }

            let mut sum = [0.0; 2];
            for tap in integer_rest..integer_rest + SINC_TAPS {
                let weight = sinc(f64::from(tap) - self.rest - f64::from(SINC_TAPS / 2) + 1.0);
                let frame = &self.ring[(tap as usize) % SINC_BUFFER_SIZE];
                sum[0] += weight * f64::from(frame[0]);
                sum[1] += weight * f64::from(frame[1]);
            }
            output[index][0] += clip16(sum[0] as i32);
            output[index][1] += clip16(sum[1] as i32);
            self.rest += source_ratio;
        }
    }

    fn refill<F>(&mut self, count: i32, fill_source: &mut F)
    where
        F: FnMut(&mut [[i32; 2]]),
    {
        if count <= 0 {
            return;
        }
        let mut remaining = count as usize;
        let mut position = (self.write_position as usize) % SINC_BUFFER_SIZE;
        while remaining != 0 {
            let chunk = remaining.min(SINC_BUFFER_SIZE - position);
            self.ring[position..position + chunk].fill([0; 2]);
            fill_source(&mut self.ring[position..position + chunk]);
            remaining -= chunk;
            position = 0;
        }
        self.write_position += count;
    }
}

#[inline]
fn sinc(value: f64) -> f64 {
    if value != 0.0 {
        let angle = SINC_PI * value;
        libm::sin(angle) / angle
    } else {
        1.0
    }
}

#[cfg(test)]
mod tests {
    use super::{OpnaScheduler, SincResampler, SsgResampler};

    fn source_sequence() -> impl FnMut() -> [i32; 3] {
        let mut index = 0;
        move || {
            let value = [100 + 7 * index, 200 + 11 * index, 500 + 13 * index];
            index += 1;
            value
        }
    }

    #[test]
    fn ssg_ratios_match_cpp_resampler_vectors() {
        let cases = [
            (4, 1, vec![533, 533, 533, 533, 554, 554, 554, 554]),
            (2, 1, vec![533, 533, 554, 554, 574, 574, 595, 595]),
            (1, 1, vec![533, 554, 574, 595, 616]),
            (4, 3, vec![533, 547, 560, 574, 595, 609, 622, 636]),
            (2, 3, vec![540, 567, 602, 629, 664, 691, 726, 753]),
            (1, 3, vec![554, 616, 678, 740, 802]),
            (2, 9, vec![570, 661, 756, 847, 942]),
            (1, 6, vec![585, 709, 833, 957]),
        ];

        for (output_samples, source_samples, expected) in cases {
            let mut resampler = SsgResampler::new();
            assert!(resampler.configure(output_samples, source_samples));
            let mut output = vec![0; expected.len()];
            resampler.render(&mut output, source_sequence());
            assert_eq!(output, expected);
        }
    }

    #[test]
    fn nop_resampler_only_advances_the_sample_index() {
        let mut resampler = SsgResampler::new();
        let mut output = [17, -23];
        resampler.render(&mut output, || [1, 2, 3]);
        assert_eq!(output, [17, -23]);
        assert_eq!(resampler.sample_index(), 2);
    }

    #[test]
    fn opna_source_scheduler_matches_fixed_time_vectors() {
        let mut scheduler = OpnaScheduler::new(4, 6).expect("valid rates");
        let mut source = 0;
        let mut output = [[0; 2]; 6];
        scheduler.render(&mut output, || {
            let value = [10 + source * 10, 20 + source * 20, 1 + source];
            source += 1;
            value
        });
        assert_eq!(
            output,
            [[11, 21], [11, 21], [22, 42], [22, 42], [33, 63], [44, 84]]
        );
        assert_eq!(scheduler.generated_samples(), 4);
    }

    #[test]
    fn fm_scaling_clips_but_ssg_scaling_does_not() {
        let mut fm = [20_000, -20_000];
        super::scale_fm_and_clip(&mut fm, 65_536);
        assert_eq!(fm, [32_767, -32_768]);

        let mut ssg = 12_345;
        super::scale_ssg(&mut ssg, 32_768);
        assert_eq!(ssg, 6_172);
    }

    #[test]
    fn sinc_resampler_matches_cpp_initial_ring_vectors() {
        let mut resampler = SincResampler::new(55_466, 44_100).expect("valid rates");
        let mut source = 0;
        let mut output = [[0; 2]; 12];
        resampler.render(&mut output, |frames| {
            for frame in frames {
                *frame = [1_000 + 17 * source, -500 - 9 * source];
                source += 1;
            }
        });
        assert_eq!(
            output,
            [
                [0, 0],
                [-3, 1],
                [0, 0],
                [-3, 1],
                [0, 0],
                [0, 0],
                [-5, 2],
                [0, 0],
                [0, 0],
                [-4, 2],
                [0, 0],
                [-3, 1],
            ]
        );
    }
}
