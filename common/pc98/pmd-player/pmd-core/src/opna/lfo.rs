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

const LFO_MAX_COUNT: [u8; 8] = [109, 78, 72, 68, 63, 45, 9, 6];

const PM_SHIFTS: [[u8; 8]; 8] = [
    [0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77],
    [0x77, 0x77, 0x77, 0x77, 0x72, 0x72, 0x72, 0x72],
    [0x77, 0x77, 0x77, 0x72, 0x72, 0x72, 0x17, 0x17],
    [0x77, 0x77, 0x72, 0x72, 0x17, 0x17, 0x12, 0x12],
    [0x77, 0x77, 0x72, 0x17, 0x17, 0x17, 0x12, 0x07],
    [0x77, 0x77, 0x17, 0x12, 0x07, 0x07, 0x02, 0x01],
    [0x77, 0x77, 0x17, 0x12, 0x07, 0x07, 0x02, 0x01],
    [0x77, 0x77, 0x17, 0x12, 0x07, 0x07, 0x02, 0x01],
];

#[derive(Clone, Copy, Debug)]
pub(crate) struct Lfo {
    counter: u32,
    am: u8,
}

impl Lfo {
    pub(crate) const fn new() -> Self {
        Self { counter: 0, am: 0 }
    }

    pub(crate) fn reset(&mut self) {
        self.counter = 0;
    }

    pub(crate) fn set_counter(&mut self, counter: u32) {
        self.counter = counter;
    }

    pub(crate) fn counter(&self) -> u32 {
        self.counter
    }

    pub(crate) fn am(&self) -> u8 {
        self.am
    }

    pub(crate) fn clock(&mut self, enabled: bool, rate: u8) -> i32 {
        if !enabled {
            self.counter = 0;

            self.am = 0x3f;
            return 0;
        }

        debug_assert!(rate < 8);
        let subcount = self.counter as u8;
        self.counter = self.counter.wrapping_add(1);
        if subcount >= LFO_MAX_COUNT[rate as usize] {
            self.counter = self.counter.wrapping_add(0x101 - u32::from(subcount));
        }

        self.am = ((self.counter >> 8) & 0x3f) as u8;
        if self.counter & (1 << 14) == 0 {
            self.am ^= 0x3f;
        }

        let mut pm = ((self.counter >> 10) & 7) as i32;
        if self.counter & (1 << 13) != 0 {
            pm ^= 7;
        }
        if self.counter & (1 << 14) != 0 {
            -pm
        } else {
            pm
        }
    }

    pub(crate) fn am_offset(&self, sensitivity: u8) -> u32 {
        debug_assert!(sensitivity < 4);
        let am_shift = (1u32 << (u32::from(sensitivity) ^ 3)) - 1;
        (u32::from(self.am) << 1) >> am_shift
    }
}

pub(crate) fn opn_lfo_pm_phase_adjustment(fnum_bits: u32, sensitivity: u8, raw_pm: i32) -> i32 {
    debug_assert!(sensitivity < 8);
    let abs_pm = if raw_pm < 0 { -raw_pm } else { raw_pm } as u32;
    let shifts = PM_SHIFTS[sensitivity as usize][(abs_pm & 7) as usize];
    let mut adjust = (fnum_bits >> (shifts & 0x0f)) + (fnum_bits >> (shifts >> 4));
    if sensitivity > 5 {
        adjust <<= sensitivity - 5;
    }
    let adjust = (adjust >> 2) as i32;
    if raw_pm < 0 {
        -adjust
    } else {
        adjust
    }
}

pub(crate) fn opn_phase_step(
    block_freq: u32,
    detune: i32,
    multiple: u32,
    pm_sensitivity: u8,
    raw_pm: i32,
) -> u32 {
    let mut fnum = (block_freq & 0x7ff) << 1;
    if pm_sensitivity != 0 {
        fnum = fnum.wrapping_add(opn_lfo_pm_phase_adjustment(
            (block_freq >> 4) & 0x7f,
            pm_sensitivity,
            raw_pm,
        ) as u32);
        fnum &= 0xfff;
    }

    let block = (block_freq >> 11) & 7;
    let phase_step = ((fnum << block) >> 2).wrapping_add(detune as u32) & 0x1ffff;
    (phase_step * multiple) >> 1
}

#[cfg(test)]
mod tests {
    use super::{opn_lfo_pm_phase_adjustment, opn_phase_step, Lfo};

    #[test]
    fn disabled_and_divider_quirk_match_cpp_vectors() {
        let mut lfo = Lfo::new();
        lfo.set_counter(123);
        assert_eq!(lfo.clock(false, 0), 0);
        assert_eq!((lfo.counter(), lfo.am()), (0, 0x3f));

        lfo.set_counter(108);
        assert_eq!(lfo.clock(true, 0), 0);
        assert_eq!((lfo.counter(), lfo.am()), (0x6d, 0x3f));
        assert_eq!(lfo.clock(true, 0), 0);
        assert_eq!((lfo.counter(), lfo.am()), (0x102, 0x3e));

        lfo.set_counter(5);
        assert_eq!(lfo.clock(true, 7), 0);
        assert_eq!((lfo.counter(), lfo.am()), (6, 0x3f));
        assert_eq!(lfo.clock(true, 7), 0);
        assert_eq!((lfo.counter(), lfo.am()), (0x102, 0x3e));
    }

    #[test]
    fn am_wave_and_sensitivity_match_cpp_vectors() {
        let cases = [
            (0x000, 0x3f, 0, [0, 15, 63, 126]),
            (0x100, 0x3e, 0, [0, 15, 62, 124]),
            (0x3ff, 0x3b, 1, [0, 14, 59, 118]),
            (0x1fff, 0x1f, 7, [0, 7, 31, 62]),
            (0x3fff, 0x00, 0, [0, 0, 0, 0]),
            (0x4000, 0x00, 0, [0, 0, 0, 0]),
            (0x7fff, 0x3f, 0, [0, 15, 63, 126]),
        ];
        for (counter, expected_am, expected_pm, offsets) in cases {
            let mut lfo = Lfo::new();
            lfo.set_counter(counter);
            assert_eq!(lfo.clock(true, 0), expected_pm);
            assert_eq!(lfo.am(), expected_am);
            for (sensitivity, expected) in offsets.into_iter().enumerate() {
                assert_eq!(lfo.am_offset(sensitivity as u8), expected);
            }
        }
    }

    #[test]
    fn phase_modulation_table_matches_cpp_vectors() {
        let expected = [
            [0, -5, -10, -15, -21, -31, -63, -127],
            [0, -5, -5, -10, -10, -21, -42, -85],
            [0, 0, 0, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0, 0, 0],
            [0, 5, 5, 10, 10, 21, 42, 85],
            [0, 5, 10, 15, 21, 31, 63, 127],
        ];
        for (raw_pm, expected_row) in [-7, -4, -1, 0, 4, 7].into_iter().zip(expected) {
            for (sensitivity, expected_value) in expected_row.into_iter().enumerate() {
                assert_eq!(
                    opn_lfo_pm_phase_adjustment(0x55, sensitivity as u8, raw_pm),
                    expected_value
                );
            }
        }
    }

    #[test]
    fn phase_step_with_pm_matches_cpp_vectors() {
        assert_eq!(opn_phase_step(0x3456, -3, 3, 5, -7), 0xcdc3);
        assert_eq!(opn_phase_step(0x3456, -3, 3, 5, 0), 0xd01b);
        assert_eq!(opn_phase_step(0x3456, -3, 3, 5, 7), 0xd273);
    }
}
