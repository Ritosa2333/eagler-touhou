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

use super::operator::Operator;
use super::registers::OperatorConfig;

const fn algorithm(
    op2_input: u16,
    op3_input: u16,
    op4_input: u16,
    op1_output: u16,
    op2_output: u16,
    op3_output: u16,
) -> u16 {
    op2_input
        | (op3_input << 1)
        | (op4_input << 4)
        | (op1_output << 7)
        | (op2_output << 8)
        | (op3_output << 9)
}

const ALGORITHM_OPS: [u16; 8] = [
    algorithm(1, 2, 3, 0, 0, 0),
    algorithm(0, 5, 3, 0, 0, 0),
    algorithm(0, 2, 6, 0, 0, 0),
    algorithm(1, 0, 7, 0, 0, 0),
    algorithm(1, 0, 3, 0, 1, 0),
    algorithm(1, 1, 1, 0, 1, 1),
    algorithm(1, 0, 0, 0, 1, 1),
    algorithm(0, 0, 0, 1, 1, 1),
];

#[derive(Clone, Copy, Debug)]
pub(crate) struct FmChannel {
    operators: [Operator; 4],
    feedback: [i16; 2],
    feedback_in: i16,
    algorithm: u8,
    feedback_level: u8,
    output_left: bool,
    output_right: bool,
}

impl FmChannel {
    pub(crate) const fn new() -> Self {
        Self {
            operators: [Operator::new(); 4],
            feedback: [0; 2],
            feedback_in: 0,
            algorithm: 0,
            feedback_level: 0,
            output_left: true,
            output_right: true,
        }
    }

    pub(crate) fn operator_mut(&mut self, index: usize) -> &mut Operator {
        &mut self.operators[index]
    }

    pub(crate) fn reset(&mut self) {
        self.feedback = [0; 2];
        self.feedback_in = 0;
        for operator in &mut self.operators {
            operator.reset();
        }
    }

    pub(crate) fn keyonoff(&mut self, states: u8) {
        for (index, operator) in self.operators.iter_mut().enumerate() {
            operator.keyonoff(states & (1 << index) != 0);
        }
    }

    pub(crate) fn prepare(&mut self, configs: [OperatorConfig; 4], lfo_enabled: bool) -> bool {
        let mut active = false;
        for (operator, config) in self.operators.iter_mut().zip(configs) {
            active |= operator.prepare(config, lfo_enabled);
        }
        active
    }

    pub(crate) fn set_algorithm(&mut self, algorithm: u8) {
        debug_assert!(algorithm < 8);
        self.algorithm = algorithm;
    }

    pub(crate) fn set_feedback_level(&mut self, feedback_level: u8) {
        debug_assert!(feedback_level < 8);
        self.feedback_level = feedback_level;
    }

    pub(crate) fn set_pan(&mut self, left: bool, right: bool) {
        self.output_left = left;
        self.output_right = right;
    }

    pub(crate) fn set_feedback_state(&mut self, feedback0: i16, feedback1: i16, feedback_in: i16) {
        self.feedback = [feedback0, feedback1];
        self.feedback_in = feedback_in;
    }

    pub(crate) fn clock(&mut self, env_counter: u32) {
        self.clock_with_lfo(env_counter, 0);
    }

    pub(crate) fn clock_with_lfo(&mut self, env_counter: u32, raw_pm: i32) {
        self.feedback[0] = self.feedback[1];
        self.feedback[1] = self.feedback_in;
        for operator in &mut self.operators {
            operator.clock_with_lfo(env_counter, raw_pm);
        }
    }

    pub(crate) fn clock_with_phase_steps(&mut self, env_counter: u32, phase_steps: [u32; 4]) {
        self.feedback[0] = self.feedback[1];
        self.feedback[1] = self.feedback_in;
        for (operator, phase_step) in self.operators.iter_mut().zip(phase_steps) {
            operator.clock_with_phase_step(env_counter, phase_step);
        }
    }

    pub(crate) fn render(&mut self, am_offset: u32, rshift: u32, clipmax: i32) -> [i32; 2] {
        let mut opmod = 0i32;
        if self.feedback_level != 0 {
            opmod = (i32::from(self.feedback[0]) + i32::from(self.feedback[1]))
                >> (10 - u32::from(self.feedback_level));
        }

        let mut opout = [0i16; 8];
        opout[1] = self.operators[0]
            .compute_volume(add_phase(self.operators[0].phase(), opmod), am_offset)
            as i16;
        self.feedback_in = opout[1];

        let algorithm_ops = ALGORITHM_OPS[self.algorithm as usize];

        opmod = i32::from(opout[(algorithm_ops & 1) as usize]) >> 1;
        opout[2] = self.operators[1]
            .compute_volume(add_phase(self.operators[1].phase(), opmod), am_offset)
            as i16;
        opout[5] = opout[1].wrapping_add(opout[2]);

        opmod = i32::from(opout[((algorithm_ops >> 1) & 7) as usize]) >> 1;
        opout[3] = self.operators[2]
            .compute_volume(add_phase(self.operators[2].phase(), opmod), am_offset)
            as i16;
        opout[6] = opout[1].wrapping_add(opout[3]);
        opout[7] = opout[2].wrapping_add(opout[3]);

        opmod = i32::from(opout[((algorithm_ops >> 4) & 7) as usize]) >> 1;
        let mut result = self.operators[3]
            .compute_volume(add_phase(self.operators[3].phase(), opmod), am_offset)
            >> rshift;

        let clipmin = -clipmax - 1;
        if algorithm_ops & (1 << 7) != 0 {
            result = clamp(result + (i32::from(opout[1]) >> rshift), clipmin, clipmax);
        }
        if algorithm_ops & (1 << 8) != 0 {
            result = clamp(result + (i32::from(opout[2]) >> rshift), clipmin, clipmax);
        }
        if algorithm_ops & (1 << 9) != 0 {
            result = clamp(result + (i32::from(opout[3]) >> rshift), clipmin, clipmax);
        }

        let mut output = [0; 2];
        if self.output_left {
            output[0] = result;
        }
        if self.output_right {
            output[1] = result;
        }
        output
    }
}

fn add_phase(phase: u32, modulation: i32) -> u32 {
    phase.wrapping_add(modulation as u32)
}

fn clamp(value: i32, min: i32, max: i32) -> i32 {
    value.clamp(min, max)
}

#[cfg(test)]
mod tests {
    use super::{FmChannel, ALGORITHM_OPS};

    fn configured_channel() -> FmChannel {
        let phases = [0x0ff, 0x155, 0x2aa, 0x3c0];
        let mut channel = FmChannel::new();
        for (operator, phase) in phases.into_iter().enumerate() {
            let operator = channel.operator_mut(operator);
            operator.set_phase_raw(phase << 10);
            operator.set_volume_state(0, 0, false, false, 0);
        }
        channel
    }

    #[test]
    fn algorithm_encoding_matches_ymfm() {
        assert_eq!(
            ALGORITHM_OPS,
            [0x035, 0x03a, 0x064, 0x071, 0x131, 0x313, 0x301, 0x380]
        );
    }

    #[test]
    fn algorithms_and_feedback_match_cpp_vectors() {
        let expected = [
            [-1828, 586, -3756, 1803, 4113, -1532, -1412, 2534],
            [-1828, 586, -3756, 1803, 4113, -1532, -1412, 2534],
            [-3100, -3666, -2642, 736, 4521, -1171, -1004, 2458],
        ];

        for (feedback_index, feedback_level) in [0u8, 3, 6].into_iter().enumerate() {
            for (algorithm, expected_value) in expected[feedback_index].into_iter().enumerate() {
                let mut channel = configured_channel();
                channel.set_feedback_level(feedback_level);
                channel.set_feedback_state(1200, -700, 333);
                channel.set_algorithm(algorithm as u8);
                assert_eq!(channel.render(0, 1, 32767)[0], expected_value);
            }
        }
    }

    #[test]
    fn channel_pan_is_applied_without_changing_the_mono_result() {
        let mut channel = configured_channel();
        channel.set_algorithm(7);
        channel.set_pan(true, false);
        assert_eq!(channel.render(0, 1, 32767), [2534, 0]);

        channel.set_pan(false, true);
        assert_eq!(channel.render(0, 1, 32767), [0, 2534]);

        channel.set_pan(false, false);
        assert_eq!(channel.render(0, 1, 32767), [0, 0]);
    }
}
