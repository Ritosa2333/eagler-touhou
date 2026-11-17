// SPDX-License-Identifier: AGPL-3.0-only

use alloc::vec::Vec;

use super::channel::{ChannelKind, ChannelState, EffectRequest, RegisterWrite};
use super::effects::VoiceFrame;
use super::stream::PartEvent;

const CARRIER_TABLE: [i32; 8] = [0x80, 0x80, 0x80, 0x80, 0xa0, 0xe0, 0xe0, 0xf0];
const CARRIER_TL_MASK: [u8; 8] = [0xee, 0xee, 0xee, 0xee, 0xcc, 0x88, 0x88, 0x00];

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
struct EffectSegment {
    count: i32,
    tone: i32,
    noise: i32,
    mixer: i32,
    volume: i32,
    envelope: i32,
    pattern: i32,
    tone_step: i32,
    noise_step: i32,
}

const D_000: &[EffectSegment] = &[
    EffectSegment {
        count: 1,
        tone: 1500,
        noise: 31,
        mixer: 54,
        volume: 15,
        envelope: 0,
        pattern: 0,
        tone_step: 127,
        noise_step: 0,
    },
    EffectSegment {
        count: 8,
        tone: 1700,
        noise: 0,
        mixer: 62,
        volume: 16,
        envelope: 1200,
        pattern: 0,
        tone_step: 127,
        noise_step: 0,
    },
];
const D_001: &[EffectSegment] = &[EffectSegment {
    count: 14,
    tone: 400,
    noise: 7,
    mixer: 54,
    volume: 16,
    envelope: 3000,
    pattern: 0,
    tone_step: 93,
    noise_step: -14,
}];
const D_002: &[EffectSegment] = &[
    EffectSegment {
        count: 2,
        tone: 700,
        noise: 0,
        mixer: 54,
        volume: 15,
        envelope: 0,
        pattern: 0,
        tone_step: 100,
        noise_step: 0,
    },
    EffectSegment {
        count: 14,
        tone: 900,
        noise: 0,
        mixer: 54,
        volume: 16,
        envelope: 2500,
        pattern: 0,
        tone_step: 100,
        noise_step: 0,
    },
];
const D_003: &[EffectSegment] = &[
    EffectSegment {
        count: 2,
        tone: 500,
        noise: 5,
        mixer: 54,
        volume: 15,
        envelope: 0,
        pattern: 0,
        tone_step: 60,
        noise_step: 0,
    },
    EffectSegment {
        count: 14,
        tone: 620,
        noise: 0,
        mixer: 54,
        volume: 16,
        envelope: 2500,
        pattern: 0,
        tone_step: 60,
        noise_step: 0,
    },
];
const D_004: &[EffectSegment] = &[
    EffectSegment {
        count: 2,
        tone: 300,
        noise: 0,
        mixer: 54,
        volume: 15,
        envelope: 0,
        pattern: 0,
        tone_step: 50,
        noise_step: 0,
    },
    EffectSegment {
        count: 14,
        tone: 400,
        noise: 0,
        mixer: 54,
        volume: 16,
        envelope: 2500,
        pattern: 0,
        tone_step: 50,
        noise_step: 0,
    },
];
const D_005: &[EffectSegment] = &[EffectSegment {
    count: 2,
    tone: 55,
    noise: 0,
    mixer: 62,
    volume: 16,
    envelope: 300,
    pattern: 0,
    tone_step: 100,
    noise_step: 0,
}];
const D_006: &[EffectSegment] = &[EffectSegment {
    count: 16,
    tone: 0,
    noise: 15,
    mixer: 55,
    volume: 16,
    envelope: 3000,
    pattern: 0,
    tone_step: 0,
    noise_step: -15,
}];
const D_007: &[EffectSegment] = &[EffectSegment {
    count: 6,
    tone: 39,
    noise: 0,
    mixer: 54,
    volume: 16,
    envelope: 500,
    pattern: 0,
    tone_step: 0,
    noise_step: 0,
}];
const D_008: &[EffectSegment] = &[EffectSegment {
    count: 32,
    tone: 39,
    noise: 0,
    mixer: 54,
    volume: 16,
    envelope: 5000,
    pattern: 0,
    tone_step: 0,
    noise_step: 0,
}];
const D_009: &[EffectSegment] = &[EffectSegment {
    count: 31,
    tone: 40,
    noise: 31,
    mixer: 54,
    volume: 16,
    envelope: 5000,
    pattern: 0,
    tone_step: 0,
    noise_step: -15,
}];
const D_010: &[EffectSegment] = &[EffectSegment {
    count: 31,
    tone: 30,
    noise: 0,
    mixer: 54,
    volume: 16,
    envelope: 5000,
    pattern: 0,
    tone_step: 0,
    noise_step: 0,
}];

const EFFECT_TABLE: [&[EffectSegment]; 11] = [
    D_000, D_001, D_002, D_003, D_004, D_005, D_006, D_007, D_008, D_009, D_010,
];

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
struct ActiveEffect {
    segments: &'static [EffectSegment],
    segment: usize,
    remaining: i32,
    tone: i32,
    noise: i32,
    tone_step: i32,
    noise_step: i32,
    noise_counter: i32,
    correction: u8,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct RegisterState {
    ssg_mixer: u8,
    psg_noise: u8,
    ssg_frames: [Option<(i32, i32)>; 3],
    fm_frames: [Option<VoiceFrame>; 22],
    fm_keys: [[u8; 3]; 2],
    effect: Option<ActiveEffect>,
    effect_priority: u8,
}

impl RegisterState {
    pub(crate) fn new() -> Self {
        Self {
            ssg_mixer: 0xbf,
            psg_noise: 0,
            ssg_frames: [None; 3],
            fm_frames: [None; 22],
            fm_keys: [[0; 3]; 2],
            effect: None,
            effect_priority: 0,
        }
    }

    pub(crate) fn reset(&mut self) {
        *self = Self::new();
    }

    pub(crate) fn apply_effect_request(
        &mut self,
        request: EffectRequest,
        writes: &mut Vec<RegisterWrite>,
    ) {
        match request {
            EffectRequest::Start { index, correction } => {
                self.start_effect(usize::from(index), correction, writes);
            }
            EffectRequest::Stop => self.end_effect(writes),
        }
    }

    pub(crate) fn emit_rhythm_effect(&mut self, mask: u16, writes: &mut Vec<RegisterWrite>) {
        let Some(index) = (0..11).find(|index| mask & (1 << index) != 0) else {
            return;
        };
        self.start_effect(index, 3, writes);
    }

    pub(crate) fn timer_a_tick(&mut self, writes: &mut Vec<RegisterWrite>) {
        let Some(mut effect) = self.effect else {
            return;
        };
        if effect.remaining > 1 {
            effect.remaining -= 1;
            self.effect = Some(effect);
            self.sweep_effect(writes);
            return;
        }

        let next = effect.segment + 1;
        if next < effect.segments.len() {
            self.effect = Some(effect);
            self.start_effect_segment(next, writes);
        } else {
            self.end_effect(writes);
        }
    }

    fn start_effect(&mut self, index: usize, correction: u8, writes: &mut Vec<RegisterWrite>) {
        let Some(segments) = EFFECT_TABLE.get(index).copied() else {
            return;
        };
        let priority = 1;
        if self.effect_priority > priority {
            return;
        }
        self.effect_priority = priority;
        self.effect = Some(ActiveEffect {
            segments,
            segment: 0,
            remaining: 0,
            tone: 0,
            noise: 0,
            tone_step: 0,
            noise_step: 0,
            noise_counter: 0,
            correction,
        });
        self.start_effect_segment(0, writes);
    }

    fn start_effect_segment(&mut self, segment: usize, writes: &mut Vec<RegisterWrite>) {
        let Some(mut effect) = self.effect else {
            return;
        };
        let Some(data) = effect.segments.get(segment).copied() else {
            self.end_effect(writes);
            return;
        };
        effect.segment = segment;
        effect.remaining = data.count;
        effect.tone = data.tone;
        effect.noise = data.noise;
        effect.tone_step = data.tone_step;
        effect.noise_step = data.noise_step;
        effect.noise_counter = data.noise_step & 15;
        self.effect = Some(effect);

        write(writes, 0x04, data.tone as u8);
        write(writes, 0x05, (data.tone >> 8) as u8);
        write(writes, 0x06, data.noise as u8);
        self.psg_noise = data.noise.clamp(0, 31) as u8;
        let mixer = ((data.mixer << 2) & 0x24) as u8 | (self.ssg_mixer & 0xdb);
        self.ssg_mixer = mixer;
        write(writes, 0x07, mixer);
        write(writes, 0x0a, data.volume as u8);
        write(writes, 0x0b, data.envelope as u8);
        write(writes, 0x0c, (data.envelope >> 8) as u8);
        write(writes, 0x0d, data.pattern as u8);
    }

    fn sweep_effect(&mut self, writes: &mut Vec<RegisterWrite>) {
        let Some(mut effect) = self.effect else {
            return;
        };
        effect.tone += effect.tone_step;
        write(writes, 0x04, effect.tone as u8);
        write(writes, 0x05, (effect.tone >> 8) as u8);
        if effect.noise_step != 0 {
            effect.noise_counter -= 1;
            if effect.noise_counter == 0 {
                effect.noise_counter = effect.noise_step & 15;
                effect.noise += effect.noise_step >> 4;
                write(writes, 0x06, effect.noise as u8);
                self.psg_noise = effect.noise.clamp(0, 31) as u8;
            }
        }
        self.effect = Some(effect);
    }

    fn end_effect(&mut self, writes: &mut Vec<RegisterWrite>) {
        self.effect = None;
        self.effect_priority = 0;
        write(writes, 0x0a, 0);
        let mixer = self.ssg_mixer | 0x24;
        self.ssg_mixer = mixer;
        write(writes, 0x07, mixer);
    }

    pub(crate) fn program_change(
        &mut self,
        part: usize,
        channel: &mut ChannelState,
        data: &[u8],
        program_offset: usize,
        fm3_alg_fb: Option<i32>,
        writes: &mut Vec<RegisterWrite>,
    ) -> Option<i32> {
        let (base, channel_index) = fm_address(part)?;
        let voice = channel.voice_number.clamp(0, 255) as u8;
        let record = find_voice(data, program_offset, voice)?;

        self.silence_fm_part(base, channel_index, channel, writes);

        let record_algorithm_feedback = i32::from(record[25]);
        let is_fm3 = base == 0 && channel_index == 2;
        let algorithm_feedback = if is_fm3 && channel.slot_mask & 0x10 == 0 {
            (fm3_alg_fb.unwrap_or(0) & 0x38) | (record_algorithm_feedback & 7)
        } else {
            record_algorithm_feedback
        };
        let algorithm_feedback_byte = algorithm_feedback as u8;
        let algorithm = (algorithm_feedback & 7) as usize;
        channel.algorithm_feedback = algorithm_feedback;
        channel.carrier = CARRIER_TABLE[algorithm];
        if channel.volume_mask & 0x0f == 0 {
            channel.volume_mask = channel.carrier;
        }
        if channel.secondary_volume_mask & 0x0f == 0 {
            channel.secondary_volume_mask = channel.carrier;
        }

        channel.slot_tl = [
            i32::from(record[5]),
            i32::from(record[6]),
            i32::from(record[7]),
            i32::from(record[8]),
        ];

        let voice_mask = channel.voice_mask as u8;
        let tl_mask = CARRIER_TL_MASK[algorithm] & voice_mask;
        let channel_offset = channel_index as u16;
        write(
            writes,
            base + 0xb0 + channel_offset,
            algorithm_feedback_byte,
        );

        for (index, value) in record[1..5].iter().copied().enumerate() {
            let mask = 0x80 >> index;
            if voice_mask & mask != 0 {
                write(
                    writes,
                    base + 0x30 + channel_offset + (index as u16 * 4),
                    value,
                );
            }
        }
        for (index, value) in record[5..9].iter().copied().enumerate() {
            let mask = 0x80 >> index;
            if tl_mask & mask != 0 {
                write(
                    writes,
                    base + 0x40 + channel_offset + (index as u16 * 4),
                    value,
                );
            }
        }
        for (register_base, record_start) in [(0x50, 9), (0x60, 13), (0x70, 17), (0x80, 21)] {
            for (index, value) in record[record_start..].iter().copied().take(4).enumerate() {
                let mask = 0x80 >> index;
                if voice_mask & mask != 0 {
                    write(
                        writes,
                        base + register_base + channel_offset + (index as u16 * 4),
                        value,
                    );
                }
            }
        }
        if is_fm3 {
            Some(algorithm_feedback)
        } else {
            None
        }
    }

    pub(crate) fn emit_frame(
        &mut self,
        event: &PartEvent,
        note: Option<u8>,
        state: VoiceFrame,
        psg_noise: i32,
        ch3_mode: u8,
        writes: &mut Vec<RegisterWrite>,
    ) {
        match event {
            PartEvent::Frame {
                kind: ChannelKind::Ssg,
                ..
            } => self.emit_ssg_frame(event, note, state, psg_noise, writes),
            PartEvent::Frame {
                kind: ChannelKind::Fm,
                ..
            } => self.emit_fm_frame(event, note, state, ch3_mode, writes),
            _ => {}
        }
    }

    fn emit_ssg_frame(
        &mut self,
        event: &PartEvent,
        note: Option<u8>,
        state: VoiceFrame,
        psg_noise: i32,
        writes: &mut Vec<RegisterWrite>,
    ) {
        let PartEvent::Frame { part, .. } = event else {
            return;
        };
        let Some(index) = part.checked_sub(6).filter(|index| *index < 3) else {
            return;
        };
        let previous = self.ssg_frames[index];
        let is_new = note.is_some();
        let is_rest = note.is_some_and(|value| value & 0x0f == 0x0f);

        let reduced = ((256 - state.ssg_volume_down) * state.base_volume) >> 8;

        let (enveloped, suppress_lfo) = if reduced <= 0 {
            (0, true)
        } else if state.envelope_mode == -1 {
            if state.envelope == 0 {
                (0, true)
            } else {
                (
                    ((((reduced * (state.envelope + 1)) >> 3) + 1) >> 1).max(0),
                    false,
                )
            }
        } else {
            let envelope = if state.envelope_mode == 3 {
                0
            } else {
                state.envelope
            };
            let level = reduced + envelope;
            if level <= 0 {
                (0, true)
            } else {
                (level.min(15), false)
            }
        };
        let volume = if suppress_lfo {
            0
        } else {
            (enveloped + state.volume_lfo).clamp(0, 15)
        };
        let pitch = state.pitch.clamp(0, 0x0fff);
        let write_volume = state.volume_write_allowed
            && (is_new
                || state.force_volume_write
                || previous.is_none_or(|(_, old_volume)| old_volume != volume));

        let write_pitch = state.pitch_enabled && (is_new || state.force_pitch_write);

        let new_voice = is_new || state.force_key_on;
        if new_voice {
            if write_volume {
                write(writes, 0x08 + index as u16, volume as u8);
            }
            if write_pitch {
                write(writes, (index * 2) as u16, pitch as u8);
                write(writes, (index * 2 + 1) as u16, (pitch >> 8) as u8);
            }
        } else {
            if write_pitch {
                write(writes, (index * 2) as u16, pitch as u8);
                write(writes, (index * 2 + 1) as u16, (pitch >> 8) as u8);
            }
            if write_volume {
                write(writes, 0x08 + index as u16, volume as u8);
            }
        }

        if (is_new || state.force_key_on) && !is_rest {
            let key_mask = (1u8 << index) | (1u8 << (index + 3));
            let mixer = (self.ssg_mixer | key_mask)
                & !(key_mask & (state.psg_pattern.clamp(0, 0xff) as u8));
            self.ssg_mixer = mixer;
            write(writes, 0x07, mixer);

            let noise = psg_noise.clamp(0, 31) as u8;
            if self.effect.is_none() && noise != self.psg_noise {
                self.psg_noise = noise;
                write(writes, 0x06, noise);
            }
        }

        self.ssg_frames[index] = Some((state.pitch, volume));
    }

    fn emit_fm_frame(
        &mut self,
        event: &PartEvent,
        note: Option<u8>,
        state: VoiceFrame,
        ch3_mode: u8,
        writes: &mut Vec<RegisterWrite>,
    ) {
        let PartEvent::Frame { part, .. } = event else {
            return;
        };
        let Some((base, channel_index)) = fm_address(*part) else {
            return;
        };
        let channel_offset = channel_index as u16;
        let previous = self.fm_frames.get(*part).and_then(|frame| *frame);
        let is_new = note.is_some();

        if is_new
            || state.force_volume_write
            || previous.is_none_or(|old| old.volume != state.volume)
        {
            let volume = (((256 - state.fm_volume_down).max(0) * state.volume) >> 8).clamp(0, 127);

            for (slot_bit, index, address_index) in [
                (0x80_u8, 3_usize, 3_u16),
                (0x40, 1, 1),
                (0x20, 2, 2),
                (0x10, 0, 0),
            ] {
                if state.slot_mask as u8 & slot_bit == 0 {
                    continue;
                }
                let carrier_slot = state.carrier as u8 & slot_bit != 0;
                let first_lfo_slot =
                    state.lfoswi & 2 != 0 && state.volume_mask as u8 & slot_bit != 0;
                let second_lfo_slot =
                    state.lfoswi & 0x20 != 0 && state.secondary_volume_mask as u8 & slot_bit != 0;
                if !carrier_slot && !first_lfo_slot && !second_lfo_slot {
                    continue;
                }
                let mut volume_table = if carrier_slot { 255 - volume } else { 128 };
                if first_lfo_slot {
                    volume_table = (volume_table - state.lfo_data).clamp(0, 255);
                }
                if second_lfo_slot {
                    volume_table = (volume_table - state.secondary_lfo_data).clamp(0, 255);
                }

                let level = (state.slot_tl[index] + volume_table)
                    .min(255)
                    .saturating_sub(128);
                write(
                    writes,
                    base + 0x40 + channel_offset + address_index * 4,
                    level as u8,
                );
            }
        }

        if state.pitch_enabled && (is_new || state.force_pitch_write) {
            if matches!(*part, 2 | 11..=13) && base == 0 && ch3_mode != 0x3f {
                emit_fm3_special_pitch(state, writes);
            } else {
                let pitch = state.pitch.clamp(0, 0x3fff) as u16;
                write(writes, base + 0xa4 + channel_offset, (pitch >> 8) as u8);
                write(writes, base + 0xa0 + channel_offset, pitch as u8);
            }
        }

        if (is_new && note.is_some_and(|value| value & 0x0f != 0x0f)) || state.force_key_on {
            let key_mask = if state.sdelay_counter != 0 {
                state.sdelay_mask
            } else {
                state.slot_mask
            };
            let key_mask = key_mask.clamp(0, 0xff) as u8;
            let bank = usize::from(base != 0);
            let key = self.fm_keys[bank][channel_index] | key_mask;
            self.fm_keys[bank][channel_index] = key;
            write(
                writes,
                0x28,
                key | channel_index as u8 | if base != 0 { 4 } else { 0 },
            );
        }

        self.fm_frames[*part] = Some(state);
    }

    fn silence_fm_part(
        &mut self,
        base: u16,
        channel_index: usize,
        channel: &ChannelState,
        writes: &mut Vec<RegisterWrite>,
    ) {
        let channel_offset = channel_index as u16;
        let voice_mask = channel.voice_mask as u8;
        for (mask, offset) in [(0x80_u8, 0_u16), (0x40, 4), (0x20, 8), (0x10, 12)] {
            if voice_mask & mask != 0 {
                write(writes, base + 0x40 + channel_offset + offset, 127);
                write(writes, base + 0x80 + channel_offset + offset, 127);
            }
        }

        let bank = usize::from(base != 0);
        let previous = self.fm_keys[bank][channel_index];
        let key = previous & !(channel.slot_mask.clamp(0, 0xff) as u8);
        self.fm_keys[bank][channel_index] = key;
        write(
            writes,
            0x28,
            key | channel_index as u8 | if base != 0 { 4 } else { 0 },
        );
    }

    pub(crate) fn emit_key_off(&mut self, event: &PartEvent, writes: &mut Vec<RegisterWrite>) {
        let PartEvent::KeyOff { part, kind } = event else {
            return;
        };
        if !matches!(kind, ChannelKind::Fm) {
            return;
        }
        let Some((base, channel_index)) = fm_address(*part) else {
            return;
        };
        let bank = usize::from(base != 0);
        let previous = self.fm_keys[bank][channel_index];
        let mask = self
            .fm_frames
            .get(*part)
            .and_then(|frame| *frame)
            .map_or(0xf0, |frame| frame.slot_mask.clamp(0, 0xff) as u8);
        let key = previous & !mask;
        self.fm_keys[bank][channel_index] = key;
        write(
            writes,
            0x28,
            key | channel_index as u8 | if base != 0 { 4 } else { 0 },
        );
    }
}

fn fm_address(part: usize) -> Option<(u16, usize)> {
    match part {
        0..=2 => Some((0, part)),
        3..=5 => Some((0x100, part - 3)),
        11..=13 => Some((0, 2)),
        _ => None,
    }
}

fn find_voice(data: &[u8], program_offset: usize, voice: u8) -> Option<&[u8]> {
    let first = data.get(program_offset..program_offset.checked_add(26)?)?;
    let mut position = program_offset;
    while let Some(record) = data.get(position..position.checked_add(26)?) {
        if record[0] == voice {
            return Some(record);
        }
        position = position.checked_add(26)?;
    }
    Some(first)
}

fn write(writes: &mut Vec<RegisterWrite>, address: u16, value: u8) {
    writes.push(RegisterWrite { address, value });
}

fn emit_fm3_special_pitch(state: VoiceFrame, writes: &mut Vec<RegisterWrite>) {
    let first_mask = if state.volume_mask & 0x0f == 0 {
        0xf0
    } else {
        state.volume_mask as u8
    };
    let second_mask = if state.secondary_volume_mask & 0x0f == 0 {
        0xf0
    } else {
        state.secondary_volume_mask as u8
    };

    if state.slot_mask as u8 & 0x80 != 0 {
        let pitch = fm3_slot_pitch(state, 3, 0x80, first_mask, second_mask, true);
        write(writes, 0xa6, (pitch >> 8) as u8);
        write(writes, 0xa2, pitch as u8);
    }
    if state.slot_mask as u8 & 0x40 != 0 {
        let pitch = fm3_slot_pitch(state, 2, 0x40, first_mask, second_mask, true);
        write(writes, 0xac, (pitch >> 8) as u8);
        write(writes, 0xa8, pitch as u8);
    }
    if state.slot_mask as u8 & 0x20 != 0 {
        let pitch = fm3_slot_pitch(state, 1, 0x20, first_mask, second_mask, true);
        write(writes, 0xae, (pitch >> 8) as u8);
        write(writes, 0xaa, pitch as u8);
    }
    if state.slot_mask as u8 & 0x10 != 0 {
        let pitch = fm3_slot_pitch(state, 0, 0x10, first_mask, second_mask, true);
        write(writes, 0xad, (pitch >> 8) as u8);
        write(writes, 0xa9, pitch as u8);
    }
}

fn fm3_slot_pitch(
    state: VoiceFrame,
    slot_index: usize,
    slot_bit: u8,
    first_mask: u8,
    second_mask: u8,
    use_secondary_lfo: bool,
) -> u16 {
    let mut fnum = (state.fm_base_pitch & 0x07ff) + state.slot_detune[slot_index];
    if state.lfoswi & 1 != 0 && first_mask & slot_bit != 0 {
        fnum += state.lfo_data;
    }
    if use_secondary_lfo && state.lfoswi & 0x10 != 0 && second_mask & slot_bit != 0 {
        fnum += state.secondary_lfo_data;
    }
    fm_block_calc(state.fm_base_pitch & 0x3800, fnum) as u16
}

fn fm_block_calc(mut block: i32, fnum: i32) -> i32 {
    let mut fnum = fnum;
    while fnum >= 0x26a {
        if fnum < 0x26a * 2 {
            return block | fnum;
        }
        block += 0x800;
        if block != 0x4000 {
            fnum -= 0x26a;
        } else {
            block = 0x3800;
            if fnum >= 0x800 {
                fnum = 0x7ff;
            }
            return block | fnum;
        }
    }
    while fnum < 0x26a {
        block -= 0x800;
        if block >= 0 {
            fnum += 0x26a;
        } else {
            block = 0;
            if fnum < 8 {
                fnum = 8;
            }
            return block | fnum;
        }
    }
    block | fnum
}

#[cfg(test)]
mod tests {
    use super::RegisterState;
    use crate::pmd::channel::{ChannelKind, ChannelState, RegisterWrite};
    use crate::pmd::stream::PartEvent;
    use alloc::vec::Vec;

    #[test]
    fn fm_program_change_replays_silence_and_voice_record_in_cpp_order() {
        let program_offset = 0x20;
        let mut data = vec![0; program_offset + 26];
        data[program_offset..].copy_from_slice(&[
            3, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
            24, 0x3d,
        ]);

        let mut channel = ChannelState::new(ChannelKind::Fm);
        channel.voice_number = 3;
        let mut state = RegisterState::new();
        let mut writes = Vec::new();
        state.program_change(0, &mut channel, &data, program_offset, None, &mut writes);

        let expected = [
            (0x40, 0x7f),
            (0x80, 0x7f),
            (0x44, 0x7f),
            (0x84, 0x7f),
            (0x48, 0x7f),
            (0x88, 0x7f),
            (0x4c, 0x7f),
            (0x8c, 0x7f),
            (0x28, 0),
            (0xb0, 0x3d),
            (0x30, 1),
            (0x34, 2),
            (0x38, 3),
            (0x3c, 4),
            (0x40, 5),
            (0x50, 9),
            (0x54, 10),
            (0x58, 11),
            (0x5c, 12),
            (0x60, 13),
            (0x64, 14),
            (0x68, 15),
            (0x6c, 16),
            (0x70, 17),
            (0x74, 18),
            (0x78, 19),
            (0x7c, 20),
            (0x80, 21),
            (0x84, 22),
            (0x88, 23),
            (0x8c, 24),
        ];
        let actual: Vec<_> = writes
            .into_iter()
            .map(|RegisterWrite { address, value }| (address, value))
            .collect();
        assert_eq!(actual, expected);
    }

    #[test]
    fn ssg_effect_start_and_sweep_match_d001() {
        let mut state = RegisterState::new();
        let mut writes = Vec::new();
        state.start_effect(1, 1, &mut writes);
        let actual: Vec<_> = writes
            .iter()
            .map(|write| (write.address, write.value))
            .collect();
        assert_eq!(
            actual,
            vec![
                (0x04, 0x90),
                (0x05, 0x01),
                (0x06, 0x07),
                (0x07, 0x9b),
                (0x0a, 0x10),
                (0x0b, 0xb8),
                (0x0c, 0x0b),
                (0x0d, 0x00),
            ]
        );
        writes.clear();
        state.timer_a_tick(&mut writes);
        assert_eq!(
            writes
                .iter()
                .map(|write| (write.address, write.value))
                .collect::<Vec<_>>(),
            vec![(0x04, 0xed), (0x05, 0x01)]
        );
    }

    #[test]
    fn ssg_volume_down_precedes_envelope_and_rest_notes_write_volume() {
        let mut channel = ChannelState::new(ChannelKind::Ssg);
        channel.volume = 15;
        channel.env_flag = 2;
        channel.env_volume = -6;
        let mut frame = crate::pmd::effects::frame(&channel, 1);
        frame.ssg_volume_down = 28;
        let event = PartEvent::Frame {
            part: 7,
            kind: ChannelKind::Ssg,
            state: frame,
        };

        let mut state = RegisterState::new();
        let mut writes = Vec::new();
        state.emit_frame(&event, Some(0x0f), frame, 0, 0x3f, &mut writes);

        assert_eq!(
            writes,
            vec![RegisterWrite {
                address: 0x09,
                value: 7,
            }]
        );
    }

    #[test]
    fn ssg_clamps_normal_envelope_before_volume_lfo() {
        let mut channel = ChannelState::new(ChannelKind::Ssg);
        channel.volume = 15;
        channel.env_flag = 1;
        channel.env_volume = 3;
        channel.lfoswi = 2;
        channel.lfo_data = -1;
        let frame = crate::pmd::effects::frame(&channel, 1);
        let event = PartEvent::Frame {
            part: 7,
            kind: ChannelKind::Ssg,
            state: frame,
        };

        let mut state = RegisterState::new();
        let mut writes = Vec::new();
        state.emit_frame(&event, Some(0x01), frame, 0, 0x3f, &mut writes);

        assert_eq!(
            writes,
            vec![
                RegisterWrite {
                    address: 0x09,
                    value: 14,
                },
                RegisterWrite {
                    address: 0x07,
                    value: 0xbd,
                },
            ]
        );
    }

    #[test]
    fn ssg_rest_with_stale_portamento_does_not_write_pitch() {
        let mut channel = ChannelState::new(ChannelKind::Ssg);
        channel.fnum = 0;
        channel.porta_num = 15;
        channel.onkai = 255;
        let frame = crate::pmd::effects::frame(&channel, 1);
        assert!(!frame.pitch_enabled);
        let event = PartEvent::Frame {
            part: 6,
            kind: ChannelKind::Ssg,
            state: frame,
        };

        let mut state = RegisterState::new();
        let mut writes = Vec::new();
        state.emit_frame(&event, Some(0x0f), frame, 0, 0x3f, &mut writes);

        assert!(writes.is_empty());
    }
}
