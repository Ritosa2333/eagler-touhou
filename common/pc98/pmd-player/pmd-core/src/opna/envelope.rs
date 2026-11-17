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

use super::tables::attenuation_increment;

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
#[repr(u8)]
pub(crate) enum EnvelopeState {
    Depress = 0,
    Attack = 1,
    Decay = 2,
    Sustain = 3,
    Release = 4,
    Reverb = 5,
}

#[derive(Clone, Copy, Debug)]
pub(crate) struct Envelope {
    attenuation: u16,
    state: EnvelopeState,
    rates: [u8; 6],
    sustain: u32,
    env_shift: u8,
    ssg_enabled: bool,
    ssg_mode: u8,
    inverted: bool,
    key_state: bool,
}

impl Envelope {
    pub(crate) const fn new() -> Self {
        Self {
            attenuation: 0x3ff,
            state: EnvelopeState::Release,
            rates: [0; 6],
            sustain: 0x3ff,
            env_shift: 0,
            ssg_enabled: false,
            ssg_mode: 0,
            inverted: false,
            key_state: false,
        }
    }

    pub(crate) fn set_rates(&mut self, rates: [u8; 6]) {
        self.rates = rates;
    }

    pub(crate) fn set_sustain(&mut self, sustain: u32) {
        self.sustain = sustain;
    }

    pub(crate) fn set_ssg(&mut self, enabled: bool, mode: u8) {
        self.ssg_enabled = enabled;
        self.ssg_mode = mode;
    }

    pub(crate) fn set_key_state(&mut self, on: bool) -> bool {
        if on == self.key_state {
            return false;
        }
        self.key_state = on;
        if on {
            self.start_attack(false);
        } else {
            self.start_release();
        }
        on
    }

    pub(crate) fn reset(&mut self) {
        self.attenuation = 0x3ff;
        self.state = EnvelopeState::Release;
        self.inverted = false;
        self.key_state = false;
    }

    pub(crate) fn state(&self) -> EnvelopeState {
        self.state
    }

    pub(crate) fn attenuation(&self) -> u16 {
        self.attenuation
    }

    pub(crate) fn inverted(&self) -> bool {
        self.inverted
    }

    pub(crate) fn configure_volume_state(
        &mut self,
        attenuation: u16,
        env_shift: u8,
        inverted: bool,
    ) {
        self.attenuation = attenuation;
        self.env_shift = env_shift;
        self.inverted = inverted;
    }

    pub(crate) fn effective_attenuation(
        &self,
        am_enabled: bool,
        am_offset: u32,
        total_level: u32,
    ) -> u32 {
        debug_assert!(self.env_shift < 32);
        let mut result = u32::from(self.attenuation) >> self.env_shift;

        if self.inverted {
            result = (0x200u32.wrapping_sub(result)) & 0x3ff;
        }
        if am_enabled {
            result = result.wrapping_add(am_offset);
        }
        result = result.wrapping_add(total_level);
        result.min(0x3ff)
    }

    pub(crate) fn clock(&mut self, env_counter: u32) {
        if self.ssg_enabled {
            self.clock_ssg_state();
        } else {
            self.inverted = false;
        }

        if env_counter & 3 == 0 {
            self.clock_envelope(env_counter >> 2);
        }
    }

    fn start_attack(&mut self, is_restart: bool) {
        if self.state == EnvelopeState::Attack {
            return;
        }
        self.state = EnvelopeState::Attack;
        if self.ssg_enabled && !is_restart {
            self.inverted = self.ssg_mode & 4 != 0;
        }
        if self.rates[EnvelopeState::Attack as usize] >= 62 {
            self.attenuation = 0;
        }
    }

    fn start_release(&mut self) {
        if (self.state as u8) >= EnvelopeState::Release as u8 {
            return;
        }
        self.state = EnvelopeState::Release;
        if self.ssg_enabled && self.inverted {
            self.attenuation = (0x200u32.wrapping_sub(u32::from(self.attenuation)) & 0x3ff) as u16;
            self.inverted = false;
        }
    }

    fn clock_ssg_state(&mut self) {
        if self.attenuation & 0x200 == 0 {
            return;
        }

        let mode = self.ssg_mode;
        if mode & 1 != 0 {
            self.inverted = ((mode >> 2) & 1) != ((mode >> 1) & 1);
            if self.state != EnvelopeState::Attack {
                self.attenuation = if self.inverted { 0x200 } else { 0x3ff };
            }
        } else {
            self.inverted ^= mode & 2 != 0;
            if matches!(self.state, EnvelopeState::Decay | EnvelopeState::Sustain) {
                self.start_attack(true);
            }
        }

        if self.state == EnvelopeState::Release {
            self.attenuation = 0x3ff;
        }
    }

    fn clock_envelope(&mut self, mut env_counter: u32) {
        if self.state == EnvelopeState::Attack && self.attenuation == 0 {
            self.state = EnvelopeState::Decay;
        }
        if self.state == EnvelopeState::Decay && u32::from(self.attenuation) >= self.sustain {
            self.state = EnvelopeState::Sustain;
        }

        let rate = u32::from(self.rates[self.state as usize]);
        let rate_shift = rate >> 2;
        env_counter <<= rate_shift;
        if env_counter & 0x7ff != 0 {
            return;
        }

        let relevant_bits = (env_counter >> if rate_shift <= 11 { 11 } else { rate_shift }) & 7;
        let increment = attenuation_increment(rate, relevant_bits);

        if self.state == EnvelopeState::Attack {
            if rate < 62 {
                let delta = (!(i32::from(self.attenuation)) * increment as i32) >> 4;
                self.attenuation = (i32::from(self.attenuation) + delta) as u16;
            }
        } else {
            if !self.ssg_enabled {
                self.attenuation = self.attenuation.wrapping_add(increment as u16);
            } else if self.attenuation < 0x200 {
                self.attenuation = self.attenuation.wrapping_add((4 * increment) as u16);
            }

            if self.attenuation >= 0x400 {
                self.attenuation = 0x3ff;
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::{Envelope, EnvelopeState};

    #[test]
    fn attack_clock_and_key_transitions_match_cpp_vectors() {
        let mut envelope = Envelope::new();
        envelope.set_rates([0, 20, 24, 16, 15, 0]);
        envelope.set_sustain(0x200);
        assert_eq!(
            (
                envelope.state(),
                envelope.attenuation(),
                envelope.inverted()
            ),
            (EnvelopeState::Release, 0x3ff, false)
        );

        envelope.set_key_state(true);
        assert_eq!(
            (envelope.state(), envelope.attenuation()),
            (EnvelopeState::Attack, 0x3ff)
        );
        envelope.clock(256);
        assert_eq!(
            (envelope.state(), envelope.attenuation()),
            (EnvelopeState::Attack, 0x3bf)
        );
        envelope.clock(512);
        assert_eq!(
            (envelope.state(), envelope.attenuation()),
            (EnvelopeState::Attack, 0x3bf)
        );
        envelope.clock(768);
        assert_eq!(
            (envelope.state(), envelope.attenuation()),
            (EnvelopeState::Attack, 0x383)
        );
    }

    #[test]
    fn fast_attack_transitions_to_decay_and_release() {
        let mut envelope = Envelope::new();
        envelope.set_rates([0, 62, 0, 0, 15, 0]);
        envelope.set_sustain(0);
        envelope.set_key_state(true);
        assert_eq!(
            (envelope.state(), envelope.attenuation()),
            (EnvelopeState::Attack, 0)
        );
        envelope.clock(0);
        assert_eq!(
            (envelope.state(), envelope.attenuation()),
            (EnvelopeState::Sustain, 0)
        );
        envelope.set_key_state(false);
        assert_eq!(
            (envelope.state(), envelope.attenuation()),
            (EnvelopeState::Release, 0)
        );
    }

    #[test]
    fn ssg_hold_and_restart_modes_match_cpp_vectors() {
        let mut envelope = Envelope::new();
        envelope.set_ssg(true, 1);
        envelope.state = EnvelopeState::Sustain;
        envelope.attenuation = 0x280;
        envelope.clock(0);
        assert_eq!(
            (
                envelope.state(),
                envelope.attenuation(),
                envelope.inverted()
            ),
            (EnvelopeState::Sustain, 0x3ff, false)
        );

        envelope.state = EnvelopeState::Sustain;
        envelope.attenuation = 0x280;
        envelope.set_ssg(true, 3);
        envelope.inverted = false;
        envelope.clock(0);
        assert_eq!(
            (
                envelope.state(),
                envelope.attenuation(),
                envelope.inverted()
            ),
            (EnvelopeState::Sustain, 0x200, true)
        );

        envelope.state = EnvelopeState::Sustain;
        envelope.attenuation = 0x280;
        envelope.set_ssg(true, 0);
        envelope.inverted = false;
        envelope.clock(0);
        assert_eq!(
            (
                envelope.state(),
                envelope.attenuation(),
                envelope.inverted()
            ),
            (EnvelopeState::Attack, 0x280, false)
        );
    }
}
