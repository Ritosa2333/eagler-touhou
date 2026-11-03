// SPDX-License-Identifier: AGPL-3.0-only

use super::timing::Tempo;
use alloc::vec::Vec;

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) enum ChannelKind {
    Fm,
    Ssg,
    Rhythm,
    Adpcm,
    P86,
    Ppz8,
}

impl ChannelKind {
    pub(crate) fn is_fm(self) -> bool {
        matches!(self, Self::Fm)
    }

    pub(crate) fn is_ssg_or_rhythm(self) -> bool {
        matches!(self, Self::Ssg | Self::Rhythm)
    }

    pub(crate) fn is_pcm(self) -> bool {
        matches!(self, Self::Adpcm | Self::P86 | Self::Ppz8)
    }
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) struct RegisterWrite {
    pub(crate) address: u16,
    pub(crate) value: u8,
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) enum EffectRequest {
    Start { index: u8, correction: u8 },
    Stop,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct ChannelState {
    pub(crate) kind: ChannelKind,
    pub(crate) volume: i32,
    pub(crate) qdata: i32,
    pub(crate) qdat2: i32,
    pub(crate) qdat3: i32,
    pub(crate) qdatb: i32,
    pub(crate) qdat: i32,
    pub(crate) fnum: i32,
    pub(crate) p86_fnum: u32,
    pub(crate) onkai: i32,
    pub(crate) onkai_def: i32,
    pub(crate) keyoff_flag: i32,
    pub(crate) detune: i32,
    pub(crate) shift: i32,
    pub(crate) shift_def: i32,
    pub(crate) mdspd: i32,
    pub(crate) mdspd2: i32,
    pub(crate) mdepth: i32,
    pub(crate) secondary_mdepth: i32,
    pub(crate) mdc: i32,
    pub(crate) mdc2: i32,
    pub(crate) secondary_mdc: i32,
    pub(crate) secondary_mdc2: i32,
    pub(crate) delay: i32,
    pub(crate) speed: i32,
    pub(crate) step: i32,
    pub(crate) time: i32,
    pub(crate) secondary_delay: i32,
    pub(crate) secondary_speed: i32,
    pub(crate) secondary_step: i32,
    pub(crate) secondary_time: i32,
    pub(crate) lfo_data: i32,
    pub(crate) secondary_lfo_data: i32,
    pub(crate) lfoswi: i32,
    pub(crate) lfo_wave: i32,
    pub(crate) secondary_lfo_wave: i32,
    pub(crate) extendmode: i32,
    pub(crate) hldelay: i32,
    pub(crate) fmpan: i32,
    pub(crate) reverse_pan: i32,
    pub(crate) psg_pattern: i32,
    pub(crate) pdr_switch: i32,
    pub(crate) voice_number: i32,
    pub(crate) part_mask: i32,
    pub(crate) slot_mask: i32,
    pub(crate) voice_mask: i32,
    pub(crate) volume_mask: i32,
    pub(crate) secondary_volume_mask: i32,
    pub(crate) carrier: i32,
    pub(crate) slot_tl: [i32; 4],
    pub(crate) slot_detune: [i32; 4],
    pub(crate) algorithm_feedback: i32,
    pub(crate) sdelay: i32,
    pub(crate) sdelay_counter: i32,
    pub(crate) sdelay_mask: i32,
    pub(crate) hard_lfo_pan: i32,
    pub(crate) vol_push: i32,
    pub(crate) key_on_count: i32,
    pub(crate) loop_check: i32,
    pub(crate) part_loop: Option<usize>,
    pub(crate) porta_from: i32,
    pub(crate) porta_to: i32,
    pub(crate) porta_length: i32,
    pub(crate) porta_num: i32,
    pub(crate) porta_num2: i32,
    pub(crate) porta_num3: i32,
    pub(crate) porta_delta: i32,
    pub(crate) porta_enabled: bool,
    pub(crate) env_flag: i32,
    pub(crate) env_count: i32,
    pub(crate) env_ar: i32,
    pub(crate) env_dr: i32,
    pub(crate) env_sr: i32,
    pub(crate) env_rr: i32,
    pub(crate) env_sl: i32,
    pub(crate) env_al: i32,
    pub(crate) env_arc: i32,
    pub(crate) env_drc: i32,
    pub(crate) env_src: i32,
    pub(crate) env_rrc: i32,
    pub(crate) env_volume: i32,
    pub(crate) repeat_start: i32,
    pub(crate) repeat_end: i32,
    pub(crate) release_start: i32,
    pub(crate) fm3_part_offsets: [u16; 3],
    pub(crate) external_part_offsets: [u16; 8],
    pub(crate) effect_value: i32,
    pub(crate) pcm86_volume: i32,
}

impl ChannelState {
    pub(crate) fn new(kind: ChannelKind) -> Self {
        let volume = match kind {
            ChannelKind::Fm => 108,
            ChannelKind::Ssg => 8,
            ChannelKind::Rhythm => 15,
            ChannelKind::Adpcm | ChannelKind::P86 | ChannelKind::Ppz8 => 128,
        };
        let fmpan = if matches!(kind, ChannelKind::Ppz8) {
            5
        } else {
            0xc0
        };
        Self {
            kind,
            volume,
            qdata: 0,
            qdat2: 0,
            qdat3: 0,
            qdatb: 0,
            qdat: 0,
            fnum: 0,
            p86_fnum: 0,
            onkai: 255,
            onkai_def: 255,
            keyoff_flag: -1,
            detune: 0,
            shift: 0,
            shift_def: 0,
            mdspd: 0,
            mdspd2: 0,
            mdepth: 0,
            secondary_mdepth: 0,
            mdc: -1,
            mdc2: -1,
            secondary_mdc: -1,
            secondary_mdc2: -1,
            delay: 0,
            speed: 0,
            step: 0,
            time: 0,
            secondary_delay: 0,
            secondary_speed: 0,
            secondary_step: 0,
            secondary_time: 0,
            lfo_data: 0,
            secondary_lfo_data: 0,
            lfoswi: 0,
            lfo_wave: 0,
            secondary_lfo_wave: 0,
            extendmode: 0,
            hldelay: 0,
            fmpan,
            reverse_pan: 1,
            psg_pattern: if matches!(kind, ChannelKind::Ssg) {
                7
            } else {
                0
            },
            pdr_switch: 0,
            voice_number: 0,
            part_mask: 0,
            slot_mask: 0xf0,
            voice_mask: if matches!(kind, ChannelKind::Fm) {
                0xff
            } else {
                0
            },
            volume_mask: 0,
            secondary_volume_mask: 0,

            carrier: 0,
            slot_tl: [0; 4],
            slot_detune: [0; 4],
            algorithm_feedback: 0,
            sdelay: 0,
            sdelay_counter: 0,
            sdelay_mask: 0,
            hard_lfo_pan: 0,
            vol_push: 0,
            key_on_count: 0,
            loop_check: 0,
            part_loop: None,
            porta_from: 0,
            porta_to: 0,
            porta_length: 0,
            porta_num: 0,
            porta_num2: 0,
            porta_num3: 0,
            porta_delta: 0,
            porta_enabled: false,
            env_flag: if matches!(kind, ChannelKind::Ssg) {
                3
            } else {
                0
            },
            env_count: 0,
            env_ar: 0,
            env_dr: 0,
            env_sr: 0,
            env_rr: 0,
            env_sl: 0,
            env_al: 0,
            env_arc: 0,
            env_drc: 0,
            env_src: 0,
            env_rrc: 0,
            env_volume: 0,
            repeat_start: 0,
            repeat_end: 0,
            release_start: 0x8000,
            fm3_part_offsets: [0; 3],
            external_part_offsets: [0; 8],
            effect_value: 0,
            pcm86_volume: 0,
        }
    }

    pub(crate) fn volume_limit(&self) -> i32 {
        if self.kind.is_fm() {
            127
        } else if self.kind.is_ssg_or_rhythm() {
            15
        } else {
            255
        }
    }

    pub(crate) fn reset_lfo_main(&mut self) {
        self.lfo_data = 0;
        self.delay = self.secondary_delay;
        self.speed = self.secondary_speed;
        self.step = self.secondary_step;
        self.time = self.secondary_time;
        self.mdc = self.mdc2;
        if self.lfo_wave == 2 || self.lfo_wave == 3 {
            self.speed = 1;
        } else {
            self.speed += 1;
        }
    }

    pub(crate) fn swap_lfo(&mut self) {
        core::mem::swap(&mut self.lfo_data, &mut self.secondary_lfo_data);
        self.lfoswi = ((self.lfoswi & 0x0f) << 4) | (self.lfoswi >> 4);
        self.extendmode = ((self.extendmode & 0x0f) << 4) | (self.extendmode >> 4);
        core::mem::swap(&mut self.delay, &mut self.secondary_delay);
        core::mem::swap(&mut self.speed, &mut self.secondary_speed);
        core::mem::swap(&mut self.step, &mut self.secondary_step);
        core::mem::swap(&mut self.time, &mut self.secondary_time);
        core::mem::swap(&mut self.volume_mask, &mut self.secondary_volume_mask);
        core::mem::swap(&mut self.mdepth, &mut self.secondary_mdepth);
        core::mem::swap(&mut self.mdspd, &mut self.mdspd2);
        core::mem::swap(&mut self.lfo_wave, &mut self.secondary_lfo_wave);
        core::mem::swap(&mut self.mdc, &mut self.secondary_mdc);
        core::mem::swap(&mut self.mdc2, &mut self.secondary_mdc2);
    }
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct SequencerGlobals {
    pub(crate) tempo: Tempo,
    pub(crate) tie_flag: i32,
    pub(crate) volume_push_flag: i32,
    pub(crate) measure_length: u32,
    pub(crate) status: i32,
    pub(crate) fadeout_flag: bool,
    pub(crate) fadeout_speed: i32,
    pub(crate) port22h: i32,
    pub(crate) fm3_alg_fb: i32,
    pub(crate) fm3_slot_detune: [i32; 4],
    pub(crate) fm3_slot_detune_flag: i32,
    pub(crate) fm3_slot3_flag: i32,
    pub(crate) psg_noise: i32,
    pub(crate) rhythm_mask: i32,
    pub(crate) rhythm_data: [i32; 6],
    pub(crate) rhythm_volume: i32,
    pub(crate) rhythm_volume_down: i32,
    pub(crate) pps_driver: bool,
    pub(crate) fm_select: u16,
    pub(crate) fm_volume_down: i32,
    pub(crate) ssg_volume_down: i32,
    pub(crate) pcm_volume_down: i32,
    pub(crate) rhythm_volume_down_saved: i32,
    pub(crate) pcm86_volume: i32,
    pub(crate) ppz_volume_down: i32,
    pub(crate) pcm_start: i32,
    pub(crate) pcm_stop: i32,
    pub(crate) pcm_repeat_start: i32,
    pub(crate) pcm_repeat_end: i32,
    pub(crate) pcm_release: i32,
    pub(crate) p86_pan_flag: i32,
    pub(crate) p86_pan_data: i32,
    pub(crate) random_seed: i32,
    pub(crate) effects: Vec<i32>,
}

impl SequencerGlobals {
    pub(crate) fn new() -> Self {
        Self {
            tempo: Tempo::new(),
            tie_flag: 0,
            volume_push_flag: 0,
            measure_length: 96,
            status: 0,
            fadeout_flag: false,
            fadeout_speed: 0,
            port22h: 0,
            fm3_alg_fb: 0,
            fm3_slot_detune: [0; 4],
            fm3_slot_detune_flag: 0,
            fm3_slot3_flag: 0,
            psg_noise: 0,
            rhythm_mask: 0xff,
            rhythm_data: [0xcf; 6],
            rhythm_volume: 0x3c,
            rhythm_volume_down: 0,
            pps_driver: false,
            fm_select: 0,
            fm_volume_down: 0,
            ssg_volume_down: 0,
            pcm_volume_down: 0,
            rhythm_volume_down_saved: 0,
            pcm86_volume: 0,
            ppz_volume_down: 0,
            pcm_start: 0,
            pcm_stop: 0,
            pcm_repeat_start: 0,
            pcm_repeat_end: 0,
            pcm_release: 0x8000,
            p86_pan_flag: 0,
            p86_pan_data: 0,
            random_seed: 0,
            effects: Vec::new(),
        }
    }
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct CommandState {
    pub(crate) channel: ChannelState,
    pub(crate) globals: SequencerGlobals,
    pub(crate) fm_channel: u16,
    pub(crate) writes: Vec<RegisterWrite>,
    pub(crate) effect_requests: Vec<EffectRequest>,
}

impl CommandState {
    pub(crate) fn new(kind: ChannelKind) -> Self {
        Self {
            channel: ChannelState::new(kind),
            globals: SequencerGlobals::new(),
            fm_channel: 0,
            writes: Vec::new(),
            effect_requests: Vec::new(),
        }
    }

    pub(crate) fn write_register(&mut self, address: u16, value: u8) {
        self.writes.push(RegisterWrite { address, value });
    }
}
