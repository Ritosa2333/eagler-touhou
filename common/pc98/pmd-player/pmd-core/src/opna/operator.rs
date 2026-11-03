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

use super::envelope::Envelope;
use super::lfo::opn_phase_step;
use super::registers::OperatorConfig;
use super::tables::{abs_sin_attenuation, attenuation_to_volume};

const EG_QUIET: u16 = 0x380;

#[derive(Clone, Copy, Debug)]
pub(crate) struct Operator {
    phase: u32,
    phase_step: u32,
    block_freq: u32,
    detune: i32,
    multiple: u32,
    pm_sensitivity: u8,
    dynamic_phase: bool,
    envelope: Envelope,
    am_enabled: bool,
    total_level: u32,
    keyon_live: bool,
}

impl Operator {
    pub(crate) const fn new() -> Self {
        Self {
            phase: 0,
            phase_step: 0,
            block_freq: 0,
            detune: 0,
            multiple: 1,
            pm_sensitivity: 0,
            dynamic_phase: false,
            envelope: Envelope::new(),
            am_enabled: false,
            total_level: 0,
            keyon_live: false,
        }
    }

    pub(crate) fn reset(&mut self) {
        self.phase = 0;
        self.phase_step = 0;
        self.block_freq = 0;
        self.detune = 0;
        self.multiple = 1;
        self.pm_sensitivity = 0;
        self.dynamic_phase = false;
        self.envelope.reset();
        self.am_enabled = false;
        self.total_level = 0;
        self.keyon_live = false;
    }

    pub(crate) fn set_phase_step(&mut self, phase_step: u32) {
        self.phase_step = phase_step;
    }

    pub(crate) fn set_phase_raw(&mut self, phase: u32) {
        self.phase = phase;
    }

    pub(crate) fn keyonoff(&mut self, on: bool) {
        self.keyon_live = on;
    }

    pub(crate) fn prepare(&mut self, config: OperatorConfig, lfo_enabled: bool) -> bool {
        self.block_freq = config.block_freq;
        self.detune = config.detune;
        self.multiple = config.multiple;
        self.pm_sensitivity = config.pm_sensitivity;
        self.dynamic_phase = lfo_enabled && config.pm_sensitivity != 0;
        self.phase_step = opn_phase_step(
            self.block_freq,
            self.detune,
            self.multiple,
            self.pm_sensitivity,
            0,
        );
        self.envelope.set_rates(config.rates);
        self.envelope.set_sustain(config.sustain);
        self.envelope.set_ssg(config.ssg_enabled, config.ssg_mode);
        self.am_enabled = config.am_enabled;
        self.total_level = config.total_level;

        if self.envelope.set_key_state(self.keyon_live) {
            self.phase = 0;
        }
        self.envelope.state() != super::envelope::EnvelopeState::Release
            || self.envelope.attenuation() < EG_QUIET
    }

    pub(crate) fn envelope_mut(&mut self) -> &mut Envelope {
        &mut self.envelope
    }

    pub(crate) fn set_volume_state(
        &mut self,
        env_attenuation: u16,
        env_shift: u8,
        ssg_inverted: bool,
        am_enabled: bool,
        total_level: u32,
    ) {
        self.envelope
            .configure_volume_state(env_attenuation, env_shift, ssg_inverted);
        self.am_enabled = am_enabled;
        self.total_level = total_level;
    }

    pub(crate) fn phase(&self) -> u32 {
        self.phase >> 10
    }

    pub(crate) fn clock_phase(&mut self) {
        self.phase = self.phase.wrapping_add(self.phase_step);
    }

    pub(crate) fn clock(&mut self, env_counter: u32) {
        self.clock_with_lfo(env_counter, 0);
    }

    pub(crate) fn clock_with_phase_step(&mut self, env_counter: u32, phase_step: u32) {
        self.envelope.clock(env_counter);
        self.phase = self.phase.wrapping_add(phase_step);
    }

    pub(crate) fn clock_with_lfo(&mut self, env_counter: u32, raw_pm: i32) {
        let phase_step = if self.dynamic_phase {
            opn_phase_step(
                self.block_freq,
                self.detune,
                self.multiple,
                self.pm_sensitivity,
                raw_pm,
            )
        } else {
            self.phase_step
        };
        self.clock_with_phase_step(env_counter, phase_step);
    }

    pub(crate) fn compute_volume(&self, phase: u32, am_offset: u32) -> i32 {
        if self.envelope.attenuation() > EG_QUIET {
            return 0;
        }

        let mut sine = abs_sin_attenuation(phase & 0x3ff);
        if phase & 0x200 != 0 {
            sine |= 0x8000;
        }

        let envelope =
            self.envelope
                .effective_attenuation(self.am_enabled, am_offset, self.total_level)
                << 2;
        let result = attenuation_to_volume((sine & 0x7fff).wrapping_add(envelope));
        if sine & 0x8000 != 0 {
            -(result as i32)
        } else {
            result as i32
        }
    }
}

#[cfg(test)]
mod tests {
    use super::Operator;
    use crate::opna::registers::OperatorConfig;

    fn test_config() -> OperatorConfig {
        OperatorConfig {
            block_freq: 0x180,
            detune: 0,
            multiple: 1,
            pm_sensitivity: 0,
            total_level: 0,
            sustain: 0,
            rates: [0, 62, 0, 0, 15, 0],
            am_enabled: false,
            ssg_enabled: false,
            ssg_mode: 0,
        }
    }

    #[test]
    fn volume_matches_cpp_operator_vectors() {
        let cases = [
            (0x000, 0x000, 0, false, false, 0x000, 0x000, 25),
            (0x001, 0x020, 0, false, false, 0x000, 0x000, 53),
            (0x0ff, 0x080, 0, false, false, 0x000, 0x000, 2042),
            (0x100, 0x200, 0, false, false, 0x000, 0x000, 31),
            (0x1ff, 0x380, 0, false, false, 0x000, 0x000, 0),
            (0x200, 0x000, 1, true, false, 0x000, 0x000, 0),
            (0x2ff, 0x100, 0, false, true, 0x020, 0x000, -361),
            (0x300, 0x100, 0, false, true, 0x200, 0x000, -1),
            (0x3ff, 0x100, 0, false, false, 0x000, 0x040, 0),
            (0x155, 0x381, 0, false, false, 0x000, 0x000, 0),
        ];

        for (phase, env, shift, inverted, am, am_offset, total, expected) in cases {
            let mut operator = Operator::new();
            operator.set_volume_state(env, shift, inverted, am, total);
            assert_eq!(
                operator.compute_volume(phase, am_offset),
                expected,
                "phase={phase:#x}"
            );
        }
    }

    #[test]
    fn phase_accumulator_matches_cpp_uint32_wrap() {
        let steps = [0x12345, 0x00001, 0x3ffff, 0x20000, 0xffff_ffff];
        let expected = [
            (0x12345, 0x0000_0048),
            (0x12346, 0x0000_0048),
            (0x52345, 0x0000_0148),
            (0x72345, 0x0000_01c8),
            (0x72344, 0x0000_01c8),
        ];
        let mut operator = Operator::new();

        for (step, (expected_raw, expected_phase)) in steps.into_iter().zip(expected) {
            operator.set_phase_step(step);
            operator.clock_phase();
            assert_eq!(operator.phase, expected_raw);
            assert_eq!(operator.phase(), expected_phase);
        }
    }

    #[test]
    fn normal_key_on_resets_phase_after_release() {
        let config = test_config();
        let mut operator = Operator::new();
        assert!(!operator.prepare(config, false));
        operator.set_phase_step(0x12345);
        operator.clock_phase();
        assert_ne!(operator.phase, 0);

        operator.keyonoff(true);
        assert!(operator.prepare(config, false));
        assert_eq!(operator.phase, 0);
    }
}
