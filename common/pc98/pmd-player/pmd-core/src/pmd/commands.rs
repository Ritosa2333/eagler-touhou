// SPDX-License-Identifier: AGPL-3.0-only

use super::channel::{ChannelKind, CommandState, EffectRequest};
use super::timing::Tempo;

pub(crate) struct PartCursor<'a> {
    data: &'a mut [u8],
    position: usize,
}

impl<'a> PartCursor<'a> {
    pub(crate) fn new(data: &'a mut [u8]) -> Self {
        Self { data, position: 0 }
    }

    pub(crate) fn position(&self) -> usize {
        self.position
    }

    pub(crate) fn len(&self) -> usize {
        self.data.len()
    }

    pub(crate) fn peek(&self) -> Option<u8> {
        self.data.get(self.position).copied()
    }

    pub(crate) fn byte_at(&self, position: usize) -> Option<u8> {
        self.data.get(position).copied()
    }

    pub(crate) fn read_u8(&mut self) -> Result<u8, CommandError> {
        let position = self.position;
        let value = self
            .data
            .get(position)
            .copied()
            .ok_or(CommandError::Truncated {
                position,
                needed: 1,
            })?;
        self.position += 1;
        Ok(value)
    }

    pub(crate) fn read_i8(&mut self) -> Result<i8, CommandError> {
        Ok(self.read_u8()? as i8)
    }

    pub(crate) fn read_u16_le(&mut self) -> Result<u16, CommandError> {
        let low = self.read_u8()?;
        let high = self.read_u8()?;
        Ok(u16::from_le_bytes([low, high]))
    }

    pub(crate) fn read_i16_le(&mut self) -> Result<i16, CommandError> {
        Ok(self.read_u16_le()? as i16)
    }

    pub(crate) fn skip(&mut self, count: usize) -> Result<(), CommandError> {
        let end = self
            .position
            .checked_add(count)
            .ok_or(CommandError::Truncated {
                position: self.position,
                needed: count,
            })?;
        if end > self.data.len() {
            return Err(CommandError::Truncated {
                position: self.position,
                needed: count,
            });
        }
        self.position = end;
        Ok(())
    }

    pub(crate) fn set_position(&mut self, position: usize) -> Result<(), CommandError> {
        if position > self.data.len() {
            return Err(CommandError::InvalidJump { target: position });
        }
        self.position = position;
        Ok(())
    }

    pub(crate) fn write_at(&mut self, position: usize, value: u8) -> Result<(), CommandError> {
        let slot = self
            .data
            .get_mut(position)
            .ok_or(CommandError::InvalidJump { target: position })?;
        *slot = value;
        Ok(())
    }
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) enum CommandError {
    Truncated { position: usize, needed: usize },
    InvalidJump { target: usize },
    InvalidOperand { opcode: u8, value: u8 },
    UnknownOpcode { opcode: u8 },
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) enum CommandOutcome {
    EndOfPart {
        position: usize,
    },
    Executed {
        opcode: u8,
        start: usize,
        end: usize,
    },
    UnknownTerminated {
        opcode: u8,
        position: usize,
    },
}

pub(crate) fn dispatch(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
) -> Result<CommandOutcome, CommandError> {
    let start = cursor.position();
    let opcode = cursor.read_u8()?;
    if opcode == 0x80 {
        return Ok(CommandOutcome::EndOfPart { position: start });
    }

    let result = match state.channel.kind {
        ChannelKind::Fm => dispatch_fm(opcode, cursor, state),
        ChannelKind::Ssg => dispatch_ssg(opcode, cursor, state),
        ChannelKind::Rhythm => dispatch_rhythm(opcode, cursor, state),
        ChannelKind::Adpcm => dispatch_adpcm(opcode, cursor, state),
        ChannelKind::P86 => dispatch_p86(opcode, cursor, state),
        ChannelKind::Ppz8 => dispatch_ppz8(opcode, cursor, state),
    };

    match result {
        Ok(()) => Ok(CommandOutcome::Executed {
            opcode,
            start,
            end: cursor.position(),
        }),
        Err(CommandError::UnknownOpcode { opcode }) => {
            cursor.write_at(start, 0x80)?;
            cursor.set_position(start)?;
            Ok(CommandOutcome::UnknownTerminated {
                opcode,
                position: start,
            })
        }
        Err(error) => Err(error),
    }
}

fn dispatch_fm(
    opcode: u8,
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
) -> Result<(), CommandError> {
    dispatch_common(opcode, cursor, state, CommandProfile::Fm)
}

fn dispatch_ssg(
    opcode: u8,
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
) -> Result<(), CommandError> {
    dispatch_common(opcode, cursor, state, CommandProfile::Ssg)
}

fn dispatch_rhythm(
    opcode: u8,
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
) -> Result<(), CommandError> {
    dispatch_common(opcode, cursor, state, CommandProfile::Rhythm)
}

fn dispatch_adpcm(
    opcode: u8,
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
) -> Result<(), CommandError> {
    dispatch_common(opcode, cursor, state, CommandProfile::Adpcm)
}

fn dispatch_p86(
    opcode: u8,
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
) -> Result<(), CommandError> {
    dispatch_common(opcode, cursor, state, CommandProfile::P86)
}

fn dispatch_ppz8(
    opcode: u8,
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
) -> Result<(), CommandError> {
    dispatch_common(opcode, cursor, state, CommandProfile::Ppz8)
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
enum CommandProfile {
    Fm,
    Ssg,
    Rhythm,
    Adpcm,
    P86,
    Ppz8,
}

impl CommandProfile {
    fn kind(self) -> ChannelKind {
        match self {
            Self::Fm => ChannelKind::Fm,
            Self::Ssg => ChannelKind::Ssg,
            Self::Rhythm => ChannelKind::Rhythm,
            Self::Adpcm => ChannelKind::Adpcm,
            Self::P86 => ChannelKind::P86,
            Self::Ppz8 => ChannelKind::Ppz8,
        }
    }

    fn is_fm(self) -> bool {
        matches!(self, Self::Fm)
    }

    fn is_ssg(self) -> bool {
        matches!(self, Self::Ssg)
    }

    fn is_rhythm(self) -> bool {
        matches!(self, Self::Rhythm)
    }

    fn is_pcm(self) -> bool {
        matches!(self, Self::Adpcm | Self::P86 | Self::Ppz8)
    }

    fn supports_lfo(self) -> bool {
        !matches!(self, Self::Rhythm | Self::P86)
    }

    fn volume_limit(self) -> i32 {
        if self.is_fm() {
            127
        } else if self.is_ssg() || self.is_rhythm() {
            15
        } else {
            255
        }
    }
}

fn dispatch_common(
    opcode: u8,
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
    profile: CommandProfile,
) -> Result<(), CommandError> {
    debug_assert_eq!(state.channel.kind, profile.kind());
    match opcode {
        0xff => program_change(cursor, state),
        0xfe => {
            state.channel.qdata = i32::from(cursor.read_u8()?);
            if !profile.is_rhythm() && !profile.is_pcm() {
                state.channel.qdat3 = 0;
            }
            Ok(())
        }
        0xfd => {
            state.channel.volume = i32::from(cursor.read_u8()?);
            Ok(())
        }
        0xfc => tempo_change(cursor, &mut state.globals.tempo),
        0xfb => {
            state.globals.tie_flag |= 1;
            Ok(())
        }
        0xfa => {
            state.channel.detune = i32::from(cursor.read_i16_le()?);
            Ok(())
        }
        0xf9 => start_loop(cursor),
        0xf8 => end_loop(cursor, &mut state.channel),
        0xf7 => loop_exit(cursor, &mut state.channel),
        0xf6 => {
            state.channel.part_loop = Some(cursor.position());
            Ok(())
        }
        0xf5 => {
            if profile.is_rhythm() {
                cursor.skip(1)
            } else {
                state.channel.shift = i32::from(cursor.read_i8()?);
                Ok(())
            }
        }
        0xf4 => {
            if profile.is_fm() {
                state.channel.volume = (state.channel.volume + 4).min(127);
            } else if profile.is_ssg() || profile.is_rhythm() {
                if state.channel.volume < 15 {
                    state.channel.volume += 1;
                }
            } else {
                if state.channel.volume < 255 - 16 {
                    state.channel.volume += 16;
                } else {
                    state.channel.volume = 255;
                }
            }
            Ok(())
        }
        0xf3 => {
            if profile.is_fm() {
                state.channel.volume = (state.channel.volume - 4).max(0);
            } else if profile.is_ssg() || profile.is_rhythm() {
                if state.channel.volume > 0 {
                    state.channel.volume -= 1;
                }
            } else {
                state.channel.volume = if state.channel.volume < 16 {
                    0
                } else {
                    state.channel.volume - 16
                };
            }
            Ok(())
        }
        0xf2 => {
            if profile.is_rhythm() {
                cursor.skip(4)
            } else {
                set_lfo(cursor, &mut state.channel)
            }
        }
        0xf1 => {
            if profile.is_rhythm() {
                state.channel.pdr_switch = i32::from(cursor.read_u8()?);
                Ok(())
            } else {
                set_lfo_switch(cursor, &mut state.channel)
            }
        }
        0xf0 => {
            if profile.is_fm() || profile.is_rhythm() {
                cursor.skip(4)
            } else {
                set_psg_envelope(cursor, &mut state.channel)
            }
        }
        0xef => raw_register(cursor, state, profile),
        0xee => {
            if profile.is_ssg() {
                state.globals.psg_noise = i32::from(cursor.read_u8()?);
                Ok(())
            } else {
                cursor.skip(1)
            }
        }
        0xed => {
            if profile.is_ssg() {
                state.channel.psg_pattern = i32::from(cursor.read_u8()?);
                Ok(())
            } else {
                cursor.skip(1)
            }
        }
        0xec => pan_set(cursor, state, profile),
        0xeb => rhythm_key(cursor, state),
        0xea => rhythm_voice_volume(cursor, state),
        0xe9 => rhythm_voice_pan(cursor, state),
        0xe8 => rhythm_master_volume(cursor, state),
        0xe7 => {
            if profile.is_rhythm() {
                cursor.skip(1)
            } else {
                state.channel.shift += i32::from(cursor.read_i8()?);
                Ok(())
            }
        }
        0xe6 => rhythm_master_volume_shift(cursor, state),
        0xe5 => rhythm_voice_volume_shift(cursor, state),
        0xe4 => {
            if profile.is_fm() {
                state.channel.hldelay = i32::from(cursor.read_u8()?);
                Ok(())
            } else {
                cursor.skip(1)
            }
        }
        0xe3 => volume_add(cursor, state, profile),
        0xe2 => volume_subtract(cursor, state, profile),
        0xe1 => {
            if profile.is_fm() {
                state.channel.hard_lfo_pan = i32::from(cursor.read_u8()?);
                Ok(())
            } else {
                cursor.skip(1)
            }
        }
        0xe0 => {
            if profile.is_fm() {
                let value = cursor.read_u8()?;
                state.globals.port22h = i32::from(value);
                state.write_register(0x22, value);
                Ok(())
            } else {
                cursor.skip(1)
            }
        }
        0xdf => {
            state.globals.measure_length = u32::from(cursor.read_u8()?);
            Ok(())
        }
        0xde => volume_one_up(cursor, state, profile),
        0xdd => volume_one_down(cursor, state, profile),
        0xdc => {
            state.globals.status = i32::from(cursor.read_u8()?);
            Ok(())
        }
        0xdb => {
            state.globals.status += i32::from(cursor.read_u8()?);
            Ok(())
        }
        0xda => portamento(cursor, state, profile),
        0xd9 | 0xd8 | 0xd7 | 0xd1 => cursor.skip(1),
        0xd6 => {
            if profile.is_rhythm() {
                cursor.skip(2)
            } else {
                let speed = cursor.read_u8()?;
                state.channel.mdspd = i32::from(speed);
                state.channel.mdspd2 = i32::from(speed);
                state.channel.mdepth = i32::from(cursor.read_i8()?);
                Ok(())
            }
        }
        0xd5 => {
            state.channel.detune += i32::from(cursor.read_i16_le()?);
            Ok(())
        }
        0xd4 | 0xd3 => {
            let value = cursor.read_u8()?;
            state.channel.effect_value = i32::from(value);
            if opcode == 0xd4 && state.channel.part_mask == 0 {
                if value == 0 {
                    state.effect_requests.push(EffectRequest::Stop);
                } else {
                    state.effect_requests.push(EffectRequest::Start {
                        index: value & 0x7f,
                        correction: 1,
                    });
                }
            }
            Ok(())
        }
        0xd2 => {
            state.globals.fadeout_flag = true;
            state.globals.fadeout_speed = i32::from(cursor.read_u8()?);
            Ok(())
        }
        0xd0 => {
            if profile.is_ssg() {
                state.globals.psg_noise =
                    (state.globals.psg_noise + i32::from(cursor.read_i8()?)).clamp(0, 31);
                Ok(())
            } else {
                cursor.skip(1)
            }
        }
        0xcf => {
            if profile.is_fm() {
                slot_mask(cursor, state)
            } else {
                cursor.skip(1)
            }
        }
        0xce => {
            if profile.is_pcm() {
                repeat_set(cursor, state)
            } else {
                cursor.skip(6)
            }
        }
        0xcd => {
            if profile.is_rhythm() || profile.is_fm() {
                cursor.skip(5)
            } else {
                set_extended_psg_envelope(cursor, &mut state.channel)
            }
        }
        0xcc => {
            if profile.is_ssg() {
                state.channel.extendmode =
                    (state.channel.extendmode & !1) | i32::from(cursor.read_u8()? & 1);
                Ok(())
            } else {
                cursor.skip(1)
            }
        }
        0xcb => {
            if profile.is_rhythm() {
                cursor.skip(1)
            } else {
                state.channel.lfo_wave = i32::from(cursor.read_u8()?);
                Ok(())
            }
        }
        0xca => {
            if profile.is_rhythm() {
                cursor.skip(1)
            } else {
                set_extend_bit(cursor, &mut state.channel, 1)
            }
        }
        0xc9 => {
            if profile.is_rhythm() || profile.is_fm() {
                cursor.skip(1)
            } else {
                set_extend_bit(cursor, &mut state.channel, 2)
            }
        }
        0xc8 => {
            if profile.is_fm() {
                slot_detune(cursor, state, false)
            } else {
                cursor.skip(3)
            }
        }
        0xc7 => {
            if profile.is_fm() {
                slot_detune(cursor, state, true)
            } else {
                cursor.skip(3)
            }
        }
        0xc6 => {
            if profile.is_fm() {
                for offset in &mut state.channel.fm3_part_offsets {
                    *offset = cursor.read_u16_le()?;
                }
                Ok(())
            } else {
                cursor.skip(6)
            }
        }
        0xc5 => {
            if profile.is_fm() {
                volume_mask(cursor, &mut state.channel, false)
            } else {
                cursor.skip(1)
            }
        }
        0xc4 => {
            if profile.is_rhythm() {
                cursor.skip(1)
            } else {
                state.channel.qdatb = i32::from(cursor.read_u8()?);
                Ok(())
            }
        }
        0xc3 => extended_pan(cursor, state, profile),
        0xc2 => {
            if profile.is_rhythm() {
                cursor.skip(1)
            } else {
                state.channel.delay = i32::from(cursor.read_u8()?);
                state.channel.secondary_delay = state.channel.delay;
                state.channel.reset_lfo_main();
                Ok(())
            }
        }
        0xc1 => Ok(()),
        0xc0 => part_mask(cursor, state, profile),
        0xbf => {
            if profile.supports_lfo() {
                state.channel.swap_lfo();
                set_lfo(cursor, &mut state.channel)?;
                state.channel.swap_lfo();
                Ok(())
            } else {
                cursor.skip(4)
            }
        }
        0xbe => switch_secondary_lfo(cursor, state, profile),
        0xbd => {
            if profile.supports_lfo() {
                state.channel.swap_lfo();
                let speed = cursor.read_u8()?;
                state.channel.mdspd = i32::from(speed);
                state.channel.mdspd2 = i32::from(speed);
                state.channel.mdepth = i32::from(cursor.read_i8()?);
                state.channel.swap_lfo();
                Ok(())
            } else {
                cursor.skip(2)
            }
        }
        0xbc => {
            if profile.supports_lfo() {
                state.channel.swap_lfo();
                state.channel.lfo_wave = i32::from(cursor.read_u8()?);
                state.channel.swap_lfo();
                Ok(())
            } else {
                cursor.skip(1)
            }
        }
        0xbb => {
            if profile.supports_lfo() {
                state.channel.swap_lfo();
                set_extend_bit(cursor, &mut state.channel, 1)?;
                state.channel.swap_lfo();
                Ok(())
            } else {
                cursor.skip(1)
            }
        }
        0xba => {
            if matches!(
                profile,
                CommandProfile::Fm | CommandProfile::Adpcm | CommandProfile::Ppz8
            ) {
                volume_mask(cursor, &mut state.channel, true)
            } else {
                cursor.skip(1)
            }
        }
        0xb9 => secondary_lfo_delay(cursor, state, profile),
        0xb8 => {
            if profile.is_fm() {
                total_level_set(cursor, state)
            } else {
                cursor.skip(2)
            }
        }
        0xb7 => mdepth_count(cursor, &mut state.channel),
        0xb6 => {
            if profile.is_fm() {
                feedback_set(cursor, state)
            } else {
                cursor.skip(1)
            }
        }
        0xb5 => {
            if profile.is_rhythm() || profile.is_fm() {
                if profile.is_fm() {
                    state.channel.sdelay_mask = i32::from(!cursor.read_u8()? << 4);
                    state.channel.sdelay = i32::from(cursor.read_u8()?);
                    state.channel.sdelay_counter = state.channel.sdelay;
                    Ok(())
                } else {
                    cursor.skip(2)
                }
            } else {
                cursor.skip(2)
            }
        }
        0xb4 => {
            if profile.is_pcm() && !matches!(profile, CommandProfile::Ppz8) {
                for offset in &mut state.channel.external_part_offsets {
                    *offset = cursor.read_u16_le()?;
                }
                Ok(())
            } else {
                cursor.skip(16)
            }
        }
        0xb3 => {
            if profile.is_rhythm() {
                cursor.skip(1)
            } else {
                state.channel.qdat2 = i32::from(cursor.read_u8()?);
                Ok(())
            }
        }
        0xb2 => {
            if profile.is_rhythm() {
                cursor.skip(1)
            } else {
                state.channel.shift_def = i32::from(cursor.read_i8()?);
                Ok(())
            }
        }
        0xb1 => {
            if profile.is_rhythm() {
                cursor.skip(1)
            } else {
                state.channel.qdat3 = i32::from(cursor.read_u8()?);
                Ok(())
            }
        }
        _ => Err(CommandError::UnknownOpcode { opcode }),
    }
}

fn program_change(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
) -> Result<(), CommandError> {
    state.channel.voice_number = i32::from(cursor.read_u8()?);
    Ok(())
}

fn tempo_change(cursor: &mut PartCursor<'_>, tempo: &mut Tempo) -> Result<(), CommandError> {
    let selector = cursor.read_u8()?;
    match selector {
        0..=250 => tempo.set_timer_b(selector),
        0xff => tempo.set_quarter(cursor.read_u8()?.max(18)),
        0xfe => tempo.adjust_timer_b(cursor.read_i8()?),
        _ => tempo.adjust_quarter(cursor.read_i8()?),
    }
    Ok(())
}

fn start_loop(cursor: &mut PartCursor<'_>) -> Result<(), CommandError> {
    let target = usize::from(cursor.read_u16_le()?);
    let marker = target
        .checked_add(1)
        .ok_or(CommandError::InvalidJump { target })?;
    cursor.write_at(marker, 0)?;
    Ok(())
}

fn end_loop(
    cursor: &mut PartCursor<'_>,
    channel: &mut super::channel::ChannelState,
) -> Result<(), CommandError> {
    let count_position = cursor.position();
    let count = cursor.read_u8()?;
    if count != 0 {
        let current_position = cursor.position();
        let current = cursor
            .byte_at(current_position)
            .ok_or(CommandError::Truncated {
                position: current_position,
                needed: 1,
            })?;
        cursor.write_at(current_position, current.wrapping_add(1))?;
        let compare = cursor.read_u8()?;
        if count == compare {
            cursor.skip(2)?;
            return Ok(());
        }
    } else {
        channel.loop_check = 1;
        cursor.skip(1)?;
    }
    let target =
        usize::from(cursor.read_u16_le()?)
            .checked_add(2)
            .ok_or(CommandError::InvalidJump {
                target: count_position,
            })?;
    cursor.set_position(target)
}

fn loop_exit(
    cursor: &mut PartCursor<'_>,
    _channel: &mut super::channel::ChannelState,
) -> Result<(), CommandError> {
    let pair = usize::from(cursor.read_u16_le()?);
    let count = cursor
        .byte_at(pair)
        .ok_or(CommandError::InvalidJump { target: pair })?;
    let current = cursor
        .byte_at(
            pair.checked_add(1)
                .ok_or(CommandError::InvalidJump { target: pair })?,
        )
        .ok_or(CommandError::InvalidJump { target: pair })?;
    if i32::from(count) - 1 == i32::from(current) {
        cursor.set_position(
            pair.checked_add(4)
                .ok_or(CommandError::InvalidJump { target: pair })?,
        )?;
    }
    Ok(())
}

fn set_lfo(
    cursor: &mut PartCursor<'_>,
    channel: &mut super::channel::ChannelState,
) -> Result<(), CommandError> {
    channel.delay = i32::from(cursor.read_u8()?);
    channel.secondary_delay = channel.delay;
    channel.speed = i32::from(cursor.read_u8()?);
    channel.secondary_speed = channel.speed;
    channel.step = i32::from(cursor.read_i8()?);
    channel.secondary_step = channel.step;
    channel.time = i32::from(cursor.read_u8()?);
    channel.secondary_time = channel.time;
    channel.reset_lfo_main();
    Ok(())
}

fn set_lfo_switch(
    cursor: &mut PartCursor<'_>,
    channel: &mut super::channel::ChannelState,
) -> Result<(), CommandError> {
    let mut value = cursor.read_u8()?;
    if value & 0xf8 != 0 {
        value = 1;
    }
    channel.lfoswi = (channel.lfoswi & !7) | i32::from(value & 7);
    channel.reset_lfo_main();
    Ok(())
}

fn set_psg_envelope(
    cursor: &mut PartCursor<'_>,
    channel: &mut super::channel::ChannelState,
) -> Result<(), CommandError> {
    channel.env_ar = i32::from(cursor.read_u8()?);
    channel.env_arc = channel.env_ar;
    channel.env_dr = i32::from(cursor.read_i8()?);
    channel.env_sr = i32::from(cursor.read_u8()?);
    channel.env_src = channel.env_sr;
    channel.env_rr = i32::from(cursor.read_u8()?);
    channel.env_rrc = channel.env_rr;
    if channel.env_flag == -1 {
        channel.env_flag = 2;
        channel.env_volume = -15;
    }
    Ok(())
}

fn set_extended_psg_envelope(
    cursor: &mut PartCursor<'_>,
    channel: &mut super::channel::ChannelState,
) -> Result<(), CommandError> {
    channel.env_ar = i32::from(cursor.read_u8()? & 0x1f);
    channel.env_dr = i32::from(cursor.read_u8()? & 0x1f);
    channel.env_sr = i32::from(cursor.read_u8()? & 0x1f);
    let release_and_sustain = cursor.read_u8()?;
    channel.env_rr = i32::from(release_and_sustain & 0x0f);
    channel.env_sl = i32::from((release_and_sustain >> 4) ^ 0x0f);
    channel.env_al = i32::from(cursor.read_u8()? & 0x0f);
    if channel.env_flag != -1 {
        channel.env_flag = -1;
        channel.env_count = 4;
        channel.env_volume = 0;
    }
    Ok(())
}

fn raw_register(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
    profile: CommandProfile,
) -> Result<(), CommandError> {
    let address = u16::from(cursor.read_u8()?);
    let value = cursor.read_u8()?;
    let base = match profile {
        CommandProfile::Fm | CommandProfile::Ppz8 => state.globals.fm_select,
        CommandProfile::Ssg | CommandProfile::Rhythm => 0,
        CommandProfile::Adpcm | CommandProfile::P86 => 0x100,
    };
    state.write_register(base + address, value);
    Ok(())
}

fn pan_set(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
    profile: CommandProfile,
) -> Result<(), CommandError> {
    let value = cursor.read_u8()?;
    match profile {
        CommandProfile::Fm => {
            state.channel.fmpan = (state.channel.fmpan & 0x3f) | ((i32::from(value) << 6) & 0xc0);
        }
        CommandProfile::Adpcm => state.channel.fmpan = i32::from(value & 3) << 6,
        CommandProfile::P86 => {
            let (flag, data) = match value {
                1 => (2, 1),
                2 => (1, 0),
                3 => (3, 0),
                _ => (7, 0),
            };
            state.globals.p86_pan_flag = flag;
            state.globals.p86_pan_data = data;
        }
        CommandProfile::Ppz8 => state.channel.fmpan = i32::from(value),
        CommandProfile::Ssg | CommandProfile::Rhythm => {}
    }
    Ok(())
}

fn extended_pan(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
    profile: CommandProfile,
) -> Result<(), CommandError> {
    let value = cursor.read_i8()?;
    let reverse = cursor.read_u8()?;
    match profile {
        CommandProfile::Fm => {
            state.channel.fmpan = if value > 0 {
                0x80
            } else if value == 0 {
                0xc0
            } else {
                0x40
            };
        }
        CommandProfile::Adpcm => {
            state.channel.fmpan = if value == 0 {
                0xc0
            } else if value < 0 {
                0x40
            } else {
                0x80
            };
        }
        CommandProfile::P86 => {
            state.channel.fmpan = i32::from(value);
            state.channel.reverse_pan = i32::from(reverse);
        }
        CommandProfile::Ppz8 => {
            state.channel.fmpan = (i32::from(value)).clamp(-4, 4) + 5;
        }
        CommandProfile::Ssg | CommandProfile::Rhythm => {}
    }
    Ok(())
}

fn rhythm_key(cursor: &mut PartCursor<'_>, state: &mut CommandState) -> Result<(), CommandError> {
    let value = cursor.read_u8()?;
    let mask = value & (state.globals.rhythm_mask as u8);
    if mask == 0 {
        return Ok(());
    }
    if mask < 0x80 {
        for index in 0..state.globals.rhythm_data.len() {
            if mask & (1 << index) != 0 {
                let data = state.globals.rhythm_data[index] as u8;
                state.write_register(0x18 + index as u16, data);
            }
        }
    }
    state.write_register(0x10, mask);
    Ok(())
}

fn rhythm_voice_volume(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
) -> Result<(), CommandError> {
    let value = cursor.read_u8()?;
    let voice = usize::from(value >> 5);
    if !(1..=6).contains(&voice) {
        return Err(CommandError::InvalidOperand {
            opcode: 0xea,
            value,
        });
    }
    let index = voice - 1;
    let data = (value & 0x1f) | (state.globals.rhythm_data[index] as u8 & 0xc0);
    state.globals.rhythm_data[index] = i32::from(data);
    state.write_register(0x18 + index as u16, data);
    Ok(())
}

fn rhythm_voice_pan(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
) -> Result<(), CommandError> {
    let value = cursor.read_u8()?;
    let voice = usize::from((value >> 5) & 7);
    if !(1..=6).contains(&voice) {
        return Err(CommandError::InvalidOperand {
            opcode: 0xe9,
            value,
        });
    }
    let index = voice - 1;
    let data = ((value & 3) << 6) | (state.globals.rhythm_data[index] as u8 & 0x1f);
    state.globals.rhythm_data[index] = i32::from(data);
    state.write_register(0x18 + index as u16, data);
    Ok(())
}

fn rhythm_master_volume(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
) -> Result<(), CommandError> {
    let value = cursor.read_u8()?;
    state.globals.rhythm_volume = i32::from(value);
    state.write_register(
        0x11,
        rhythm_output_volume(state, state.globals.rhythm_volume),
    );
    Ok(())
}

fn rhythm_master_volume_shift(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
) -> Result<(), CommandError> {
    let value = cursor.read_i8()?;
    let next = state.globals.rhythm_volume + i32::from(value);
    state.globals.rhythm_volume = if next >= 64 {
        if next & 0x80 != 0 {
            0
        } else {
            63
        }
    } else {
        next
    };
    state.write_register(
        0x11,
        rhythm_output_volume(state, state.globals.rhythm_volume),
    );
    Ok(())
}

fn rhythm_output_volume(state: &CommandState, value: i32) -> u8 {
    let corrected = if state.globals.rhythm_volume_down != 0 {
        ((256 - state.globals.rhythm_volume_down) * value) >> 8
    } else {
        value
    };
    corrected.clamp(0, 255) as u8
}

fn rhythm_voice_volume_shift(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
) -> Result<(), CommandError> {
    let voice = usize::from(cursor.read_u8()?);
    if !(1..=6).contains(&voice) {
        return Err(CommandError::InvalidOperand {
            opcode: 0xe5,
            value: voice as u8,
        });
    }
    let shift = i32::from(cursor.read_i8()?);
    let index = voice - 1;
    let volume = (state.globals.rhythm_data[index] & 0x1f) + shift;
    let volume = volume.clamp(0, 31);
    state.globals.rhythm_data[index] = (state.globals.rhythm_data[index] & 0xe0) | volume;
    state.write_register(0x18 + index as u16, state.globals.rhythm_data[index] as u8);
    Ok(())
}

fn volume_add(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
    profile: CommandProfile,
) -> Result<(), CommandError> {
    let amount = i32::from(cursor.read_u8()?);
    if profile.is_rhythm() {
        if state.channel.volume + amount < 16 {
            state.channel.volume += amount;
        }
    } else {
        state.channel.volume = (state.channel.volume + amount).min(profile.volume_limit());
    }
    Ok(())
}

fn volume_subtract(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
    _profile: CommandProfile,
) -> Result<(), CommandError> {
    let amount = i32::from(cursor.read_u8()?);
    if state.channel.volume < amount {
        state.channel.volume = 0;
    } else {
        state.channel.volume -= amount;
    }
    Ok(())
}

fn volume_one_up(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
    profile: CommandProfile,
) -> Result<(), CommandError> {
    let amount = i32::from(cursor.read_u8()?);
    let value = if profile.is_fm() {
        (state.channel.volume + 1 + amount).min(128)
    } else {
        let limit_before_increment = if profile.is_pcm() {
            254
        } else {
            profile.volume_limit()
        };
        (state.channel.volume + amount).min(limit_before_increment) + 1
    };
    state.channel.vol_push = value;
    state.globals.volume_push_flag = 1;
    Ok(())
}

fn volume_one_down(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
    _profile: CommandProfile,
) -> Result<(), CommandError> {
    let amount = i32::from(cursor.read_u8()?);
    let mut value = state.channel.volume - amount;
    if value < 0 {
        value = 0;
    } else if value >= 255 {
        value = 254;
    }
    state.channel.vol_push = value + 1;
    state.globals.volume_push_flag = 1;
    Ok(())
}

fn portamento(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
    profile: CommandProfile,
) -> Result<(), CommandError> {
    if profile.is_rhythm() || matches!(profile, CommandProfile::P86) {
        return cursor.skip(1);
    }
    let from = i32::from(cursor.read_u8()?);
    let to = i32::from(cursor.read_u8()?);
    let length = i32::from(cursor.read_u8()?);
    if length == 0 {
        return Err(CommandError::InvalidOperand {
            opcode: 0xda,
            value: 0,
        });
    }
    if cursor.peek() == Some(0xc1) {
        cursor.skip(1)?;
        state.channel.qdat = 0;
    }
    state.channel.porta_from = from;
    state.channel.porta_to = to;
    state.channel.porta_length = length;
    let difference = to - from;
    state.channel.porta_delta = if matches!(profile, CommandProfile::Ppz8) {
        difference / 16
    } else {
        difference
    };
    state.channel.porta_enabled = true;
    Ok(())
}

fn repeat_set(cursor: &mut PartCursor<'_>, state: &mut CommandState) -> Result<(), CommandError> {
    state.channel.repeat_start = i32::from(cursor.read_i16_le()?);
    state.channel.repeat_end = i32::from(cursor.read_i16_le()?);
    state.channel.release_start = i32::from(cursor.read_i16_le()? as u16);
    state.globals.pcm_repeat_start = state.channel.repeat_start;
    state.globals.pcm_repeat_end = state.channel.repeat_end;
    state.globals.pcm_release = state.channel.release_start;
    Ok(())
}

fn set_extend_bit(
    cursor: &mut PartCursor<'_>,
    channel: &mut super::channel::ChannelState,
    bit: u32,
) -> Result<(), CommandError> {
    let value = cursor.read_u8()?;
    let mask = 1_i32 << bit;
    channel.extendmode = (channel.extendmode & !mask) | (i32::from(value & 1) << bit);
    Ok(())
}

fn slot_mask(cursor: &mut PartCursor<'_>, state: &mut CommandState) -> Result<(), CommandError> {
    let value = cursor.read_u8()?;
    let low = value & 0x0f;
    if low != 0 {
        state.channel.carrier = i32::from(low << 4);
    }
    if low == 0 && state.fm_channel == 2 && state.globals.fm_select == 0 {
        state.channel.carrier = carrier_for_algorithm(state.globals.fm3_alg_fb & 7);
    }
    state.channel.slot_mask = i32::from(value & 0xf0);
    if state.channel.slot_mask == 0 {
        state.channel.part_mask |= 0x20;
    } else {
        state.channel.part_mask &= !0x20;
    }
    state.channel.voice_mask = if state.channel.slot_mask & 0x80 != 0 {
        0x11
    } else {
        0
    };
    if state.channel.slot_mask & 0x40 != 0 {
        state.channel.voice_mask |= 0x44;
    }
    if state.channel.slot_mask & 0x20 != 0 {
        state.channel.voice_mask |= 0x22;
    }
    if state.channel.slot_mask & 0x10 != 0 {
        state.channel.voice_mask |= 0x88;
    }
    Ok(())
}

fn carrier_for_algorithm(algorithm: i32) -> i32 {
    [0x80, 0x80, 0x80, 0x80, 0xa0, 0xe0, 0xe0, 0xf0]
        .get(usize::try_from(algorithm.clamp(0, 7)).unwrap_or(0))
        .copied()
        .unwrap_or(0x80)
}

fn slot_detune(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
    relative: bool,
) -> Result<(), CommandError> {
    let mask = cursor.read_u8()?;
    let value = i32::from(cursor.read_i16_le()?);
    let is_fm3 = state.fm_channel == 2 && state.globals.fm_select == 0;
    for (index, detune) in state.channel.slot_detune.iter_mut().enumerate() {
        if mask & (1 << index) != 0 {
            if relative {
                *detune += value;
            } else {
                *detune = value;
            }
        }
    }
    if is_fm3 {
        for (index, detune) in state.globals.fm3_slot_detune.iter_mut().enumerate() {
            if mask & (1 << index) != 0 {
                if relative {
                    *detune += value;
                } else {
                    *detune = value;
                }
            }
        }
        state.globals.fm3_slot_detune_flag = if state
            .globals
            .fm3_slot_detune
            .iter()
            .any(|detune| *detune != 0)
        {
            1
        } else {
            0
        };
    }
    Ok(())
}

fn volume_mask(
    cursor: &mut PartCursor<'_>,
    channel: &mut super::channel::ChannelState,
    secondary: bool,
) -> Result<(), CommandError> {
    let value = cursor.read_u8()? & 0x0f;
    let mask = if value == 0 {
        channel.carrier
    } else {
        i32::from((value << 4) | 0x0f)
    };
    if secondary {
        channel.secondary_volume_mask = mask;
    } else {
        channel.volume_mask = mask;
    }
    Ok(())
}

fn secondary_lfo_delay(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
    profile: CommandProfile,
) -> Result<(), CommandError> {
    if !profile.supports_lfo() {
        return cursor.skip(1);
    }
    state.channel.swap_lfo();
    state.channel.delay = i32::from(cursor.read_u8()?);
    state.channel.secondary_delay = state.channel.delay;
    state.channel.reset_lfo_main();
    if profile.is_ssg() {
    } else {
        state.channel.swap_lfo();
    }
    Ok(())
}

fn switch_secondary_lfo(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
    profile: CommandProfile,
) -> Result<(), CommandError> {
    if profile.is_rhythm() || profile == CommandProfile::P86 {
        return cursor.skip(1);
    }
    let value = cursor.read_u8()?;
    state.channel.lfoswi = (state.channel.lfoswi & 0x8f) | (i32::from(value & 7) << 4);
    state.channel.swap_lfo();
    state.channel.reset_lfo_main();
    state.channel.swap_lfo();
    Ok(())
}

fn mdepth_count(
    cursor: &mut PartCursor<'_>,
    channel: &mut super::channel::ChannelState,
) -> Result<(), CommandError> {
    let value = cursor.read_u8()?;
    if value & 0x80 != 0 {
        let count = if value & 0x7f == 0 {
            255
        } else {
            i32::from(value & 0x7f)
        };
        channel.secondary_mdc = count;
        channel.secondary_mdc2 = count;
    } else {
        let count = if value == 0 { 255 } else { i32::from(value) };
        channel.mdc = count;
        channel.mdc2 = count;
    }
    Ok(())
}

fn total_level_set(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
) -> Result<(), CommandError> {
    let selector = cursor.read_i8()?;
    let value = i32::from(cursor.read_i8()?);
    let slot_selector = (selector as u8 & 0x0f)
        & ((state.channel.slot_mask as u8 >> 4) | (state.channel.slot_mask as u8).wrapping_shl(4));
    let channel_offset = state.fm_channel;
    for (selector_bit, slot_index, register_offset) in
        [(1_u8, 0_usize, 0_u16), (2, 2, 8), (4, 1, 4), (8, 3, 12)]
    {
        if slot_selector & selector_bit == 0 {
            continue;
        }
        let level = if selector >= 0 {
            value & 0x7f
        } else {
            let candidate = state.channel.slot_tl[slot_index] + value;
            if candidate < 0 {
                0
            } else if value >= 0 && candidate > 127 {
                127
            } else {
                candidate
            }
        };
        state.channel.slot_tl[slot_index] = level;
        if state.channel.part_mask == 0 {
            state.write_register(
                state.globals.fm_select + 0x40 + channel_offset + register_offset,
                level as u8,
            );
        }
    }
    Ok(())
}

fn feedback_set(cursor: &mut PartCursor<'_>, state: &mut CommandState) -> Result<(), CommandError> {
    let input = i32::from(cursor.read_i8()?);
    let is_fm3 = state.fm_channel == 2 && state.globals.fm_select == 0;
    let current = if is_fm3 {
        state.globals.fm3_alg_fb
    } else {
        state.channel.algorithm_feedback
    };

    if is_fm3 && state.channel.slot_mask & 0x10 == 0 {
        return Ok(());
    }
    let mut feedback = input;
    if feedback >= 0 {
        feedback = ((feedback << 3) & 0xff) | (feedback >> 5);
    } else {
        if feedback & 0x40 == 0 {
            feedback &= 7;
        }
        feedback += (current >> 3) & 7;
        if feedback >= 0 {
            if feedback >= 8 {
                feedback = 0x38;
            } else {
                feedback = ((feedback << 3) & 0xff) | (feedback >> 5);
            }
        } else {
            feedback = 0;
        }
    }
    let value = (current & 7) | feedback;
    if is_fm3 {
        state.globals.fm3_alg_fb = value;
    }
    state.channel.algorithm_feedback = value;
    state.write_register(
        state.globals.fm_select + 0xb0 + state.fm_channel,
        value as u8,
    );
    Ok(())
}

fn part_mask(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
    profile: CommandProfile,
) -> Result<(), CommandError> {
    let value = cursor.read_u8()?;
    if value >= 2 {
        return special_part_mask(cursor, state, value);
    }
    if value != 0 {
        state.channel.part_mask |= 0x40;
    } else {
        state.channel.part_mask &= !0x40;
    }
    if profile.is_fm() && value == 1 {
        state.channel.part_mask |= 0x40;
    }
    Ok(())
}

fn special_part_mask(
    cursor: &mut PartCursor<'_>,
    state: &mut CommandState,
    opcode: u8,
) -> Result<(), CommandError> {
    let operand_position = cursor.position();
    let value = cursor.read_u8()?;
    let signed = i32::from(value as i8);
    match opcode {
        0xff => state.globals.fm_volume_down = i32::from(value),
        0xfe => state.globals.fm_volume_down += signed,
        0xfd => state.globals.ssg_volume_down = i32::from(value),
        0xfc => state.globals.ssg_volume_down += signed,
        0xfb => state.globals.pcm_volume_down = i32::from(value),
        0xfa => state.globals.pcm_volume_down += signed,
        0xf9 => state.globals.rhythm_volume_down = i32::from(value),
        0xf8 => state.globals.rhythm_volume_down_saved += signed,
        0xf7 => state.globals.pcm86_volume = i32::from(value & 1),
        0xf6 => state.globals.ppz_volume_down = i32::from(value),
        0xf5 => state.globals.ppz_volume_down += signed,
        _ => {
            cursor.write_at(operand_position, 0x80)?;
            cursor.set_position(operand_position)?;
        }
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::{dispatch, CommandError, CommandOutcome, PartCursor};
    use crate::pmd::channel::{ChannelKind, CommandState};

    fn execute(bytes: &mut [u8], kind: ChannelKind) -> (CommandOutcome, CommandState) {
        let mut cursor = PartCursor::new(bytes);
        let mut state = CommandState::new(kind);
        let outcome = dispatch(&mut cursor, &mut state).expect("synthetic command is valid");
        (outcome, state)
    }

    #[test]
    fn cursor_reads_little_endian_signed_operands_and_rejects_truncation() {
        let mut bytes = [0x34, 0x12, 0xfe];
        let mut cursor = PartCursor::new(&mut bytes);
        assert_eq!(cursor.read_u16_le(), Ok(0x1234));
        assert_eq!(cursor.read_i8(), Ok(-2));
        assert_eq!(
            cursor.read_u8(),
            Err(CommandError::Truncated {
                position: 3,
                needed: 1
            })
        );
        assert_eq!(cursor.len(), 3);
    }

    #[test]
    fn fm_commands_update_state_and_keep_raw_register_effects() {
        let mut bytes = [
            0xfd, 100, 0xf4, 0xfa, 0x34, 0x12, 0xf2, 2, 3, 0xfe, 0xef, 0x22, 0xa5,
        ];
        let (outcome, state) = execute(&mut bytes, ChannelKind::Fm);
        assert_eq!(
            outcome,
            CommandOutcome::Executed {
                opcode: 0xfd,
                start: 0,
                end: 2
            }
        );
        assert_eq!(state.channel.volume, 100);

        let mut bytes = [0xf2, 2, 3, 0xfe, 0xef, 0x22, 0xa5];
        let (outcome, mut state) = execute(&mut bytes, ChannelKind::Fm);
        assert_eq!(
            outcome,
            CommandOutcome::Executed {
                opcode: 0xf2,
                start: 0,
                end: 5
            }
        );
        let mut cursor = PartCursor::new(&mut bytes[4..]);
        let _ = dispatch(&mut cursor, &mut state).expect("raw register command");
        assert_eq!(
            state.writes,
            [super::super::channel::RegisterWrite {
                address: 0x22,
                value: 0xa5
            }]
        );
    }

    #[test]
    fn volume_steps_preserve_cpp_ssg_limits_and_pcm_stride() {
        let mut ssg_state = CommandState::new(ChannelKind::Ssg);
        let mut set_ssg = [0xfd, 16];
        dispatch(&mut PartCursor::new(&mut set_ssg), &mut ssg_state).expect("SSG volume set");
        let mut up_ssg = [0xf4];
        dispatch(&mut PartCursor::new(&mut up_ssg), &mut ssg_state).expect("SSG volume up");
        assert_eq!(ssg_state.channel.volume, 16);
        let mut down_ssg = [0xf3];
        dispatch(&mut PartCursor::new(&mut down_ssg), &mut ssg_state).expect("SSG volume down");
        assert_eq!(ssg_state.channel.volume, 15);

        let mut adpcm_state = CommandState::new(ChannelKind::Adpcm);
        let mut set_adpcm = [0xfd, 128];
        dispatch(&mut PartCursor::new(&mut set_adpcm), &mut adpcm_state).expect("ADPCM volume set");
        let mut up_adpcm = [0xf4];
        dispatch(&mut PartCursor::new(&mut up_adpcm), &mut adpcm_state).expect("ADPCM volume up");
        assert_eq!(adpcm_state.channel.volume, 144);
        let mut down_adpcm = [0xf3];
        dispatch(&mut PartCursor::new(&mut down_adpcm), &mut adpcm_state)
            .expect("ADPCM volume down");
        assert_eq!(adpcm_state.channel.volume, 128);
    }

    #[test]
    fn regular_lfo_consumes_four_parameters_like_reference_driver() {
        let mut bytes = [0xf2, 2, 3, 0xfe, 0x81, 0xef, 0x22, 0xa5];
        let (outcome, state) = execute(&mut bytes, ChannelKind::Fm);
        assert_eq!(
            outcome,
            CommandOutcome::Executed {
                opcode: 0xf2,
                start: 0,
                end: 5,
            }
        );
        assert_eq!(state.channel.delay, 2);
        assert_eq!(state.channel.secondary_delay, 2);
        assert_eq!(state.channel.speed, 4);
        assert_eq!(state.channel.secondary_speed, 3);
        assert_eq!(state.channel.step, -2);
        assert_eq!(state.channel.secondary_step, -2);
        assert_eq!(state.channel.time, 0x81);
        assert_eq!(state.channel.secondary_time, 0x81);
    }

    #[test]
    fn tempo_command_uses_the_existing_integer_tempo_conversion() {
        let mut bytes = [0xfc, 0xff, 80];
        let (_, state) = execute(&mut bytes, ChannelKind::Fm);
        assert_eq!(state.globals.tempo.quarter(), 80);

        let mut bytes = [0xfc, 0xfe, 0xfb];
        let (_, state) = execute(&mut bytes, ChannelKind::Fm);
        assert_eq!(state.globals.tempo.timer_b(), 195);
    }

    #[test]
    fn rhythm_master_volume_applies_cpp_volume_down_at_write_time() {
        let mut bytes = [0xe8, 60];
        let mut cursor = PartCursor::new(&mut bytes);
        let mut state = CommandState::new(ChannelKind::Rhythm);
        state.globals.rhythm_volume_down = 10;
        dispatch(&mut cursor, &mut state).expect("valid rhythm volume command");
        assert_eq!(state.globals.rhythm_volume, 60);
        assert_eq!(state.writes[0].address, 0x11);
        assert_eq!(state.writes[0].value, 57);
    }

    #[test]
    fn loop_start_marks_the_original_pointer_and_unknown_rewinds_as_cpp_does() {
        let mut bytes = [0xf9, 2, 0, 0xaa];
        let (outcome, _) = execute(&mut bytes, ChannelKind::Fm);
        assert_eq!(
            outcome,
            CommandOutcome::Executed {
                opcode: 0xf9,
                start: 0,
                end: 3
            }
        );
        assert_eq!(bytes, [0xf9, 2, 0, 0]);

        let mut bytes = [0xa0];
        let (outcome, _) = execute(&mut bytes, ChannelKind::Fm);
        assert_eq!(
            outcome,
            CommandOutcome::UnknownTerminated {
                opcode: 0xa0,
                position: 0
            }
        );
        assert_eq!(bytes[0], 0x80);
    }

    #[test]
    fn command_profiles_preserve_the_documented_operand_lengths() {
        let mut bytes = [0xf2, 0, 0, 0, 0, 0, 0, 0, 0];
        let (outcome, _) = execute(&mut bytes, ChannelKind::Rhythm);
        assert_eq!(
            outcome,
            CommandOutcome::Executed {
                opcode: 0xf2,
                start: 0,
                end: 5
            }
        );

        let mut bytes = [0xda, 0x42];
        let (outcome, _) = execute(&mut bytes, ChannelKind::P86);
        assert_eq!(
            outcome,
            CommandOutcome::Executed {
                opcode: 0xda,
                start: 0,
                end: 2
            }
        );

        let mut bytes = [0xef, 0x10, 0x7f];
        let (_, state) = execute(&mut bytes, ChannelKind::Adpcm);
        assert_eq!(
            state.writes,
            [super::super::channel::RegisterWrite {
                address: 0x110,
                value: 0x7f
            }]
        );
    }
}
