// SPDX-License-Identifier: AGPL-3.0-only

use alloc::vec::Vec;

use super::channel::{ChannelKind, CommandState, RegisterWrite, SequencerGlobals};
use super::commands::{dispatch, CommandError, CommandOutcome, PartCursor};
use super::effects::{self, VoiceFrame};
use super::registers::RegisterState;
use super::ParsedSong;

const MAX_ACTIONS_PER_TICK: usize = 4096;

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) enum StreamError {
    InvalidPartOffset {
        part: usize,
        offset: usize,
    },
    Truncated {
        part: usize,
        position: usize,
        needed: usize,
    },
    InvalidJump {
        part: usize,
        target: usize,
    },
    InvalidOperand {
        part: usize,
        opcode: u8,
        value: u8,
    },
    UnknownCommand {
        part: usize,
        opcode: u8,
    },
    ActionBudgetExceeded {
        part: usize,
        position: usize,
    },
}

impl StreamError {
    fn from_command(part: usize, error: CommandError) -> Self {
        match error {
            CommandError::Truncated { position, needed } => Self::Truncated {
                part,
                position,
                needed,
            },
            CommandError::InvalidJump { target } => Self::InvalidJump { part, target },
            CommandError::InvalidOperand { opcode, value } => Self::InvalidOperand {
                part,
                opcode,
                value,
            },
            CommandError::UnknownOpcode { opcode } => Self::UnknownCommand { part, opcode },
        }
    }
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) enum PartEvent {
    Note {
        part: usize,
        kind: ChannelKind,
        value: u8,
        length: u8,
        masked: bool,
    },
    Portamento {
        part: usize,
        kind: ChannelKind,
        length: u8,
    },

    KeyOff {
        part: usize,
        kind: ChannelKind,
    },

    Frame {
        part: usize,
        kind: ChannelKind,
        state: VoiceFrame,
    },
    Rhythm {
        part: usize,
        mask: u16,
        length: u8,
    },
    Ended {
        part: usize,
        kind: ChannelKind,
    },
}

#[derive(Clone, Copy, Debug, Default, Eq, PartialEq)]
struct PartTick {
    looped: bool,
    ended: bool,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct PartRunner {
    part: usize,
    kind: ChannelKind,
    start: usize,
    program_offset: usize,
    position: usize,
    remaining: i32,
    ended: bool,
    allow_loop: bool,
    rhythm_table: Option<usize>,
    rhythm_position: Option<usize>,
    state: CommandState,
}

impl PartRunner {
    fn new(
        part: usize,
        kind: ChannelKind,
        start: usize,
        data_len: usize,
        program_offset: usize,
    ) -> Result<Self, StreamError> {
        if start >= data_len {
            return Err(StreamError::InvalidPartOffset {
                part,
                offset: start,
            });
        }
        Ok(Self {
            part,
            kind,
            start,
            program_offset,
            position: start,
            remaining: 0,
            ended: false,
            allow_loop: true,
            rhythm_table: None,
            rhythm_position: None,
            state: CommandState::new(kind),
        })
    }

    fn reset(&mut self) {
        self.position = self.start;
        self.remaining = 0;
        self.ended = false;
        self.allow_loop = true;
        self.rhythm_position = None;
        self.state = CommandState::new(self.kind);
    }

    fn set_rhythm_table(&mut self, offset: usize) {
        self.rhythm_table = Some(offset);
    }

    fn set_allow_loop(&mut self, allow: bool) {
        self.allow_loop = allow;
    }

    fn initialize_external(&mut self, start: usize, data_len: usize) -> Result<(), StreamError> {
        if start >= data_len {
            return Err(StreamError::InvalidPartOffset {
                part: self.part,
                offset: start,
            });
        }
        self.start = start;
        self.position = start;
        self.remaining = 1;
        self.ended = false;
        self.allow_loop = true;
        self.rhythm_position = None;
        self.state = CommandState::new(self.kind);
        self.state.channel.keyoff_flag = -1;
        self.state.channel.mdspd = -1;
        self.state.channel.mdspd2 = -1;
        self.state.channel.mdc = -1;
        self.state.channel.mdc2 = -1;
        self.state.channel.part_mask |= match self.kind {
            ChannelKind::Fm => 0x20,
            ChannelKind::Ppz8 => 0,
            _ => 0,
        };
        Ok(())
    }

    fn external_offsets(&self) -> Option<ExternalOffsets> {
        match self.part {
            0..=2 => Some(ExternalOffsets::Fm3(self.state.channel.fm3_part_offsets)),

            9 => Some(ExternalOffsets::Ppz(
                self.state.channel.external_part_offsets,
            )),
            _ => None,
        }
    }

    pub(crate) fn position(&self) -> usize {
        self.position
    }

    pub(crate) fn ended(&self) -> bool {
        self.ended
    }

    pub(crate) fn channel(&self) -> &super::channel::ChannelState {
        &self.state.channel
    }

    #[allow(clippy::too_many_arguments)]
    fn tick(
        &mut self,
        data: &mut [u8],
        globals: &mut SequencerGlobals,
        writes: &mut Vec<RegisterWrite>,
        events: &mut Vec<PartEvent>,
        registers: &mut RegisterState,
        timer_a_delta: u32,
        ch3_mode: &mut u8,
    ) -> Result<PartTick, StreamError> {
        if self.ended {
            self.state.channel.loop_check = 3;

            let update = effects::advance_voice_with_timer_a(
                &mut self.state.channel,
                globals,
                timer_a_delta,
            );
            if update.force_volume_write || update.force_pitch_write || update.force_key_on {
                self.emit_frame(events, None, update, writes, registers, globals, *ch3_mode);
            }
            return Ok(PartTick {
                looped: false,
                ended: true,
            });
        }

        self.state.channel.loop_check = 0;
        let was_holding = self.remaining > 0;
        if was_holding {
            self.remaining -= 1;
        }
        if was_holding
            && self.state.channel.part_mask == 0
            && self.state.channel.keyoff_flag & 3 == 0
            && self.remaining <= self.state.channel.qdat
        {
            effects::key_off(&mut self.state.channel);
            self.state.channel.keyoff_flag = -1;
            let event = PartEvent::KeyOff {
                part: self.part,
                kind: self.kind,
            };
            registers.emit_key_off(&event, writes);
            events.push(event);
        }
        if self.remaining != 0 {
            let update = effects::advance_voice_with_timer_a(
                &mut self.state.channel,
                globals,
                timer_a_delta,
            );
            self.emit_frame(events, None, update, writes, registers, globals, *ch3_mode);
            return Ok(PartTick::default());
        }

        if was_holding {
            self.state.channel.lfoswi &= !8;
            self.state.channel.porta_enabled = false;
        }

        if self.kind == ChannelKind::Rhythm {
            return self.tick_rhythm(data, globals, writes, events, registers);
        }

        let mut looped = false;
        for _ in 0..MAX_ACTIONS_PER_TICK {
            let opcode = *data.get(self.position).ok_or(StreamError::Truncated {
                part: self.part,
                position: self.position,
                needed: 1,
            })?;

            if opcode == 0x80 {
                self.state.channel.loop_check = 3;
                if let Some(part_loop) = self.state.channel.part_loop {
                    if self.allow_loop {
                        self.position = part_loop;
                        self.state.channel.loop_check = 1;
                        looped = true;
                        continue;
                    }
                }
                self.ended = true;
                self.state.channel.onkai = 255;
                events.push(PartEvent::Ended {
                    part: self.part,
                    kind: self.kind,
                });
                let update = effects::advance_voice_with_timer_a(
                    &mut self.state.channel,
                    globals,
                    timer_a_delta,
                );
                if update.force_volume_write || update.force_pitch_write || update.force_key_on {
                    self.emit_frame(events, None, update, writes, registers, globals, *ch3_mode);
                }
                return Ok(PartTick {
                    looped,
                    ended: true,
                });
            }

            let is_portamento =
                opcode == 0xda && !matches!(self.kind, ChannelKind::Rhythm | ChannelKind::P86);
            let is_command = opcode > 0x80 && !is_portamento;
            let portamento_continues =
                is_portamento && data.get(self.position.saturating_add(4)).copied() == Some(0xc1);

            if is_command || is_portamento {
                let mut cursor = PartCursor::new(data);
                cursor
                    .set_position(self.position)
                    .map_err(|error| StreamError::from_command(self.part, error))?;

                self.state.globals.fm_select = fm_select(self.part, self.kind);
                self.state.fm_channel = fm_channel(self.part, self.kind);
                core::mem::swap(&mut self.state.globals, globals);
                let result = dispatch(&mut cursor, &mut self.state);
                core::mem::swap(&mut self.state.globals, globals);
                let outcome =
                    result.map_err(|error| StreamError::from_command(self.part, error))?;
                self.position = cursor.position();
                writes.append(&mut self.state.writes);
                for request in self.state.effect_requests.drain(..) {
                    registers.apply_effect_request(request, writes);
                }
                if opcode == 0xff
                    && self.kind == ChannelKind::Fm
                    && self.state.channel.part_mask == 0
                {
                    let fm3_alg_fb = matches!(self.part, 2 | 11..=13).then_some(globals.fm3_alg_fb);
                    if let Some(value) = registers.program_change(
                        self.part,
                        &mut self.state.channel,
                        data,
                        self.program_offset,
                        fm3_alg_fb,
                        writes,
                    ) {
                        globals.fm3_alg_fb = value;
                    }
                }
                if matches!(opcode, 0xbe | 0xba | 0xf1 | 0xcf | 0xc8 | 0xc7 | 0xc5) {
                    update_fm3_mode(
                        self.part,
                        self.kind,
                        &self.state.channel,
                        globals,
                        ch3_mode,
                        writes,
                    );
                }

                if is_portamento {
                    let length = self.state.channel.porta_length;
                    if length <= 0 {
                        return Err(StreamError::InvalidOperand {
                            part: self.part,
                            opcode,
                            value: 0,
                        });
                    }
                    if self.state.channel.part_mask == 0 {
                        let from = u8::try_from(self.state.channel.porta_from).unwrap_or(0);
                        let to = u8::try_from(self.state.channel.porta_to).unwrap_or(0);
                        effects::start_portamento_with_timer_a(
                            &mut self.state.channel,
                            globals,
                            from,
                            to,
                            length,
                            timer_a_delta,
                        )
                        .map_err(|_| StreamError::InvalidOperand {
                            part: self.part,
                            opcode,
                            value: 0,
                        })?;
                        if !portamento_continues {
                            effects::calculate_gate(&mut self.state.channel, globals, length);
                        }
                    }
                    let length = u8::try_from(length).unwrap_or(u8::MAX);
                    self.remaining = i32::from(length);
                    self.state.channel.key_on_count += 1;
                    if self.state.channel.vol_push != 0 && self.state.channel.onkai != 255 {
                        globals.volume_push_flag -= 1;
                        if globals.volume_push_flag != 0 {
                            globals.volume_push_flag = 0;
                            self.state.channel.vol_push = 0;
                        }
                    }
                    self.state.channel.keyoff_flag =
                        if data.get(self.position).copied() == Some(0xfb) {
                            2
                        } else {
                            0
                        };
                    globals.tie_flag = 0;
                    globals.volume_push_flag = 0;
                    events.push(PartEvent::Portamento {
                        part: self.part,
                        kind: self.kind,
                        length,
                    });
                    if self.state.channel.part_mask == 0 {
                        self.emit_frame(
                            events,
                            None,
                            effects::FrameUpdate {
                                force_volume_write: true,
                                force_pitch_write: true,
                                force_key_on: true,
                            },
                            writes,
                            registers,
                            globals,
                            *ch3_mode,
                        );
                    }
                    return Ok(PartTick {
                        looped,
                        ended: false,
                    });
                }

                if matches!(outcome, CommandOutcome::UnknownTerminated { .. }) {
                    continue;
                }
                continue;
            }

            let note = opcode;
            self.position += 1;
            let length_position = self.position;
            let length = *data.get(length_position).ok_or(StreamError::Truncated {
                part: self.part,
                position: length_position,
                needed: 1,
            })?;
            self.position += 1;
            let gate_continues = data.get(self.position).copied() == Some(0xc1);
            if gate_continues {
                self.position += 1;
            }

            self.remaining = i32::from(length);
            self.state.channel.key_on_count += 1;
            let masked = self.state.channel.part_mask != 0;
            if !masked {
                effects::start_note_with_timer_a(
                    &mut self.state.channel,
                    globals,
                    note,
                    timer_a_delta,
                );
                if gate_continues {
                    self.state.channel.qdat = 0;
                } else {
                    effects::calculate_gate(&mut self.state.channel, globals, i32::from(length));
                }
                if data.get(self.position).copied() == Some(0xfb) {
                    self.state.channel.keyoff_flag = 2;
                } else if self.state.channel.onkai == 255 {
                    self.state.channel.keyoff_flag = -1;
                } else {
                    self.state.channel.keyoff_flag = 0;
                }
                if self.state.channel.vol_push != 0 && self.state.channel.onkai != 255 {
                    globals.volume_push_flag -= 1;
                    if globals.volume_push_flag != 0 {
                        globals.volume_push_flag = 0;
                        self.state.channel.vol_push = 0;
                    }
                }
            } else {
                self.state.channel.keyoff_flag = -1;
            }
            globals.tie_flag = 0;
            globals.volume_push_flag = 0;
            events.push(PartEvent::Note {
                part: self.part,
                kind: self.kind,
                value: note,
                length,
                masked,
            });
            if !masked {
                self.emit_frame(
                    events,
                    Some(note),
                    effects::FrameUpdate::default(),
                    writes,
                    registers,
                    globals,
                    *ch3_mode,
                );
            }
            return Ok(PartTick {
                looped,
                ended: false,
            });
        }

        Err(StreamError::ActionBudgetExceeded {
            part: self.part,
            position: self.position,
        })
    }

    #[allow(clippy::too_many_arguments)]
    fn emit_frame(
        &self,
        events: &mut Vec<PartEvent>,
        note: Option<u8>,
        update: effects::FrameUpdate,
        writes: &mut Vec<RegisterWrite>,
        registers: &mut RegisterState,
        globals: &SequencerGlobals,
        ch3_mode: u8,
    ) {
        let mut state = effects::frame_with_update(&self.state.channel, self.remaining, update);
        if matches!(self.part, 2 | 11..=13) {
            state.slot_detune = globals.fm3_slot_detune;
        }
        state.fm_volume_down = globals.fm_volume_down;
        state.ssg_volume_down = globals.ssg_volume_down;
        let event = PartEvent::Frame {
            part: self.part,
            kind: self.kind,
            state,
        };
        if let PartEvent::Frame { state, .. } = event {
            registers.emit_frame(&event, note, state, globals.psg_noise, ch3_mode, writes);
        }
        events.push(event);
    }

    fn tick_rhythm(
        &mut self,
        data: &mut [u8],
        globals: &mut SequencerGlobals,
        writes: &mut Vec<RegisterWrite>,
        events: &mut Vec<PartEvent>,
        registers: &mut RegisterState,
    ) -> Result<PartTick, StreamError> {
        let mut looped = false;
        for _ in 0..MAX_ACTIONS_PER_TICK {
            if let Some(rhythm_position) = self.rhythm_position {
                let value = *data.get(rhythm_position).ok_or(StreamError::Truncated {
                    part: self.part,
                    position: rhythm_position,
                    needed: 1,
                })?;
                if value == 0xff {
                    self.rhythm_position = None;
                    continue;
                }

                let mut cursor = PartCursor::new(data);
                cursor
                    .set_position(rhythm_position)
                    .map_err(|error| StreamError::from_command(self.part, error))?;
                let first = cursor
                    .read_u8()
                    .map_err(|error| StreamError::from_command(self.part, error))?;
                if first & 0xc0 == 0xc0 {
                    cursor
                        .set_position(rhythm_position)
                        .map_err(|error| StreamError::from_command(self.part, error))?;
                    core::mem::swap(&mut self.state.globals, globals);
                    let result = dispatch(&mut cursor, &mut self.state);
                    core::mem::swap(&mut self.state.globals, globals);
                    let outcome =
                        result.map_err(|error| StreamError::from_command(self.part, error))?;
                    self.rhythm_position = Some(cursor.position());
                    writes.append(&mut self.state.writes);
                    for request in self.state.effect_requests.drain(..) {
                        registers.apply_effect_request(request, writes);
                    }
                    if matches!(outcome, CommandOutcome::UnknownTerminated { .. }) {
                        continue;
                    }
                    continue;
                }

                let mask = if first & 0x80 != 0 {
                    ((u16::from(first) << 8)
                        | u16::from(
                            cursor
                                .read_u8()
                                .map_err(|error| StreamError::from_command(self.part, error))?,
                        ))
                        & 0x3fff
                } else {
                    0
                };
                let length = cursor
                    .read_u8()
                    .map_err(|error| StreamError::from_command(self.part, error))?;
                self.rhythm_position = Some(cursor.position());
                self.remaining = i32::from(length);
                self.state.channel.key_on_count += 1;
                globals.tie_flag = 0;
                globals.volume_push_flag = 0;
                registers.emit_rhythm_effect(mask, writes);
                events.push(PartEvent::Rhythm {
                    part: self.part,
                    mask,
                    length,
                });
                return Ok(PartTick {
                    looped,
                    ended: false,
                });
            }

            let value = *data.get(self.position).ok_or(StreamError::Truncated {
                part: self.part,
                position: self.position,
                needed: 1,
            })?;
            if value == 0x80 {
                self.state.channel.loop_check = 3;
                if let Some(part_loop) = self.state.channel.part_loop {
                    if self.allow_loop {
                        self.position = part_loop;
                        self.state.channel.loop_check = 1;
                        looped = true;
                        continue;
                    }
                }
                self.ended = true;
                events.push(PartEvent::Ended {
                    part: self.part,
                    kind: self.kind,
                });
                return Ok(PartTick {
                    looped,
                    ended: true,
                });
            }

            if value > 0x80 {
                let mut cursor = PartCursor::new(data);
                cursor
                    .set_position(self.position)
                    .map_err(|error| StreamError::from_command(self.part, error))?;
                core::mem::swap(&mut self.state.globals, globals);
                let result = dispatch(&mut cursor, &mut self.state);
                core::mem::swap(&mut self.state.globals, globals);
                let outcome =
                    result.map_err(|error| StreamError::from_command(self.part, error))?;
                self.position = cursor.position();
                writes.append(&mut self.state.writes);
                if matches!(outcome, CommandOutcome::UnknownTerminated { .. }) {
                    continue;
                }
                continue;
            }

            let table = self.rhythm_table.ok_or(StreamError::InvalidJump {
                part: self.part,
                target: self.position,
            })?;
            let entry =
                table
                    .checked_add(usize::from(value).checked_mul(2).ok_or(
                        StreamError::InvalidJump {
                            part: self.part,
                            target: table,
                        },
                    )?)
                    .ok_or(StreamError::InvalidJump {
                        part: self.part,
                        target: table,
                    })?;
            let low = *data.get(entry).ok_or(StreamError::Truncated {
                part: self.part,
                position: entry,
                needed: 2,
            })?;
            let high_position = entry.checked_add(1).ok_or(StreamError::InvalidJump {
                part: self.part,
                target: entry,
            })?;
            let high = *data.get(high_position).ok_or(StreamError::Truncated {
                part: self.part,
                position: entry,
                needed: 2,
            })?;
            self.rhythm_position = Some(usize::from(u16::from_le_bytes([low, high])));
            self.position += 1;
        }

        Err(StreamError::ActionBudgetExceeded {
            part: self.part,
            position: self.position,
        })
    }
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
enum ExternalOffsets {
    Fm3([u16; 3]),
    Ppz([u16; 8]),
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct SongRunner {
    initial_data: Vec<u8>,
    data: Vec<u8>,
    program_offset: usize,
    parts: Vec<PartRunner>,
    globals: SequencerGlobals,
    writes: Vec<RegisterWrite>,
    timer_writes: Vec<RegisterWrite>,
    registers: RegisterState,
    last_timer_b: Option<u8>,
    ch3_mode: u8,
    loop_count: Option<u32>,
    loops_completed: u32,
    ended: bool,
}

impl SongRunner {
    pub(crate) fn from_song(song: &ParsedSong) -> Result<Option<Self>, StreamError> {
        Self::from_song_with_p86(song, false)
    }

    pub(crate) fn from_song_with_p86(
        song: &ParsedSong,
        use_p86: bool,
    ) -> Result<Option<Self>, StreamError> {
        let Some(layout) = song.layout.as_ref() else {
            return Ok(None);
        };
        let data = song.main.get(1..).ok_or(StreamError::Truncated {
            part: 0,
            position: 0,
            needed: 1,
        })?;
        let initial_data = data.to_vec();
        let mut parts = Vec::new();

        let order = [
            (6, ChannelKind::Ssg),
            (7, ChannelKind::Ssg),
            (8, ChannelKind::Ssg),
            (3, ChannelKind::Fm),
            (4, ChannelKind::Fm),
            (5, ChannelKind::Fm),
            (0, ChannelKind::Fm),
            (1, ChannelKind::Fm),
            (2, ChannelKind::Fm),
            (10, ChannelKind::Rhythm),
            (
                9,
                if use_p86 {
                    ChannelKind::P86
                } else {
                    ChannelKind::Adpcm
                },
            ),
        ];
        for (part, kind) in order {
            let offset = usize::from(layout.part_offsets[part]);
            if offset != 0 {
                if data.get(offset) == Some(&0x80) {
                    continue;
                }
                let mut runner = PartRunner::new(
                    part,
                    kind,
                    offset,
                    data.len(),
                    usize::from(layout.program_area_offset),
                )?;
                if kind == ChannelKind::Rhythm {
                    runner.set_rhythm_table(usize::from(layout.rhythm_table_offset));
                }
                parts.push(runner);
            }
        }

        let ended = parts.is_empty();
        Ok(Some(Self {
            initial_data,
            data: data.to_vec(),
            program_offset: usize::from(layout.program_area_offset),
            parts,
            globals: SequencerGlobals::new(),
            writes: Vec::new(),
            timer_writes: Vec::new(),
            registers: RegisterState::new(),

            last_timer_b: Some(SequencerGlobals::new().tempo.timer_b()),
            ch3_mode: 0x3f,
            loop_count: None,
            loops_completed: 0,
            ended,
        }))
    }

    pub(crate) fn reset(&mut self) {
        self.data.copy_from_slice(&self.initial_data);
        self.globals = SequencerGlobals::new();
        self.writes.clear();
        self.timer_writes.clear();
        self.registers.reset();

        self.last_timer_b = Some(self.globals.tempo.timer_b());
        self.ch3_mode = 0x3f;
        self.loops_completed = 0;
        self.parts.retain(|runner| runner.part < 11);
        self.ended = self.parts.is_empty();
        for part in &mut self.parts {
            part.reset();
        }
        self.apply_loop_limit();
    }

    pub(crate) fn loops_completed(&self) -> u32 {
        self.loops_completed
    }

    pub(crate) fn set_loop_count(&mut self, count: Option<u32>) {
        self.loop_count = count;
        self.loops_completed = 0;
        self.apply_loop_limit();
    }

    fn apply_loop_limit(&mut self) {
        let allow = match self.loop_count {
            Some(limit) => self.loops_completed < limit,
            None => true,
        };
        for part in &mut self.parts {
            part.set_allow_loop(allow);
        }
    }

    pub(crate) fn ended(&self) -> bool {
        self.ended
    }

    pub(crate) fn position(&self, part: usize) -> Option<usize> {
        self.parts
            .iter()
            .find(|runner| runner.part == part)
            .map(PartRunner::position)
    }

    pub(crate) fn tick(&mut self, events: &mut Vec<PartEvent>) -> Result<SongTick, StreamError> {
        self.tick_with_timer_a(events, 1)
    }

    pub(crate) fn tick_with_timer_a(
        &mut self,
        events: &mut Vec<PartEvent>,
        timer_a_delta: u32,
    ) -> Result<SongTick, StreamError> {
        events.clear();
        self.writes.clear();
        if self.ended {
            return Ok(SongTick {
                ended: true,
                looped: false,
                writes: 0,
            });
        }

        let mut looped = false;
        let mut all_ended = true;
        let mut index = 0;
        while index < self.parts.len() {
            let result = self.parts[index].tick(
                &mut self.data,
                &mut self.globals,
                &mut self.writes,
                events,
                &mut self.registers,
                timer_a_delta,
                &mut self.ch3_mode,
            )?;
            looped |= result.looped;
            all_ended &= result.ended;
            if let Some(offsets) = self.parts[index].external_offsets() {
                self.activate_external(offsets)?;
            }
            index += 1;
        }

        if looped {
            self.loops_completed = self.loops_completed.saturating_add(1);
            self.apply_loop_limit();
        }

        let timer_b = self.globals.tempo.timer_b();
        if self.last_timer_b != Some(timer_b) {
            self.writes.push(RegisterWrite {
                address: 0x26,
                value: timer_b,
            });
            self.last_timer_b = Some(timer_b);
        }
        self.writes.push(RegisterWrite {
            address: 0x27,
            value: self.ch3_mode | 0x30,
        });
        if all_ended {
            self.ended = true;
        }

        Ok(SongTick {
            ended: self.ended,
            looped,
            writes: self.writes.len(),
        })
    }

    pub(crate) fn take_writes(&mut self, output: &mut Vec<RegisterWrite>) {
        output.append(&mut self.writes);
    }

    pub(crate) fn timer_a_tick(&mut self) {
        self.registers.timer_a_tick(&mut self.timer_writes);
    }

    pub(crate) fn timer_b_value(&self) -> u8 {
        self.globals.tempo.timer_b()
    }

    pub(crate) fn ch3_mode(&self) -> u8 {
        self.ch3_mode
    }

    pub(crate) fn take_timer_writes(&mut self, output: &mut Vec<RegisterWrite>) {
        output.append(&mut self.timer_writes);
    }

    fn activate_external(&mut self, offsets: ExternalOffsets) -> Result<(), StreamError> {
        match offsets {
            ExternalOffsets::Fm3(offsets) => {
                for (slot, offset) in offsets.into_iter().enumerate() {
                    if offset != 0 {
                        self.activate_part(11 + slot, ChannelKind::Fm, usize::from(offset))?;
                    }
                }
            }
            ExternalOffsets::Ppz(offsets) => {
                for (slot, offset) in offsets.into_iter().enumerate() {
                    if offset != 0 {
                        self.activate_part(14 + slot, ChannelKind::Ppz8, usize::from(offset))?;
                    }
                }
            }
        }
        Ok(())
    }

    fn activate_part(
        &mut self,
        part: usize,
        kind: ChannelKind,
        offset: usize,
    ) -> Result<(), StreamError> {
        if let Some(existing) = self.parts.iter_mut().find(|runner| runner.part == part) {
            if existing.start != offset {
                existing.initialize_external(offset, self.data.len())?;
            }
            return Ok(());
        }

        let mut runner = PartRunner::new(part, kind, offset, self.data.len(), self.program_offset)?;
        runner.initialize_external(offset, self.data.len())?;
        let order = part_order(part);
        let insert_at = self
            .parts
            .iter()
            .position(|existing| part_order(existing.part) > order)
            .unwrap_or(self.parts.len());
        self.parts.insert(insert_at, runner);
        Ok(())
    }
}

fn part_order(part: usize) -> usize {
    match part {
        6..=8 => part - 6,
        3..=5 => part,
        0..=2 => part + 6,
        11..=13 => part - 2,
        10 => 12,
        9 => 13,
        14..=21 => part,
        _ => 100 + part,
    }
}

fn update_fm3_mode(
    part: usize,
    kind: ChannelKind,
    channel: &super::channel::ChannelState,
    globals: &mut SequencerGlobals,
    mode: &mut u8,
    writes: &mut Vec<RegisterWrite>,
) {
    if !matches!(part, 2 | 11..=13) || kind != ChannelKind::Fm {
        return;
    }

    let part_bit = match part {
        2 => 1,
        11 => 2,
        12 => 4,
        13 => 8,
        _ => return,
    };
    let clear_slot_flag = |globals: &mut SequencerGlobals| {
        globals.fm3_slot3_flag &= !part_bit;
        if globals.fm3_slot3_flag == 0 {
            if globals.fm3_slot_detune_flag != 1 {
                0x3f
            } else {
                0x7f
            }
        } else {
            0x7f
        }
    };
    let requested = if channel.slot_mask & 0xf0 == 0 {
        clear_slot_flag(globals)
    } else if channel.slot_mask != 0xf0 {
        globals.fm3_slot3_flag |= part_bit;
        0x7f
    } else if channel.volume_mask & 0x0f == 0 {
        clear_slot_flag(globals)
    } else if channel.lfoswi & 1 != 0 {
        globals.fm3_slot3_flag |= part_bit;
        0x7f
    } else if channel.secondary_volume_mask & 0x0f == 0 {
        clear_slot_flag(globals)
    } else if channel.lfoswi & 0x10 != 0 {
        globals.fm3_slot3_flag |= part_bit;
        0x7f
    } else {
        clear_slot_flag(globals)
    };
    if requested != *mode {
        *mode = requested;
        writes.push(RegisterWrite {
            address: 0x27,
            value: requested & 0xcf,
        });
    }
}

fn fm_select(part: usize, kind: ChannelKind) -> u16 {
    if !matches!(kind, ChannelKind::Fm | ChannelKind::Ppz8) {
        return 0;
    }
    match part {
        3..=5 => 0x100,
        11..=13 => 0,
        _ => 0,
    }
}

fn fm_channel(part: usize, kind: ChannelKind) -> u16 {
    if !matches!(kind, ChannelKind::Fm | ChannelKind::Ppz8) {
        return 0;
    }
    match part {
        0..=2 => part as u16,
        3..=5 => (part - 3) as u16,
        11..=13 => 2,
        _ => 0,
    }
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) struct SongTick {
    pub(crate) ended: bool,
    pub(crate) looped: bool,
    pub(crate) writes: usize,
}

#[cfg(test)]
mod tests {
    use super::{PartEvent, RegisterWrite, SongRunner, StreamError};
    use crate::pmd::channel::ChannelKind;
    use crate::pmd::{CompanionNames, MainFormat, MainLayout, ParsedSong};
    use crate::Metadata;
    use alloc::vec::Vec;

    fn song_with_stream(stream: &[u8]) -> ParsedSong {
        let mut main = vec![0; 1 + 0x1a + stream.len()];
        main[1..3].copy_from_slice(&[0x1a, 0]);
        main[1 + 0x1a..].copy_from_slice(stream);
        ParsedSong {
            main,
            flags: 0,
            format: MainFormat::V1a,
            layout: Some(MainLayout {
                part_offsets: [0x1a, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                rhythm_table_offset: 0,
                program_area_offset: 0,
            }),
            metadata: Metadata::default(),
            companions: CompanionNames::default(),
        }
    }

    #[test]
    fn note_length_is_decremented_before_the_next_note_is_read() {
        let song = song_with_stream(&[0x21, 2, 0x22, 1, 0x80]);
        let mut runner = SongRunner::from_song(&song)
            .expect("valid synthetic layout")
            .expect("layout exists");
        let mut events = Vec::new();

        let first = runner.tick(&mut events).expect("first tick");
        assert!(!first.ended);
        assert_eq!(
            events[0],
            PartEvent::Note {
                part: 0,
                kind: ChannelKind::Fm,
                value: 0x21,
                length: 2,
                masked: false,
            }
        );
        events.clear();
        let second = runner.tick(&mut events).expect("holding tick");
        assert!(!second.ended);
        assert!(matches!(events[0], PartEvent::Frame { .. }));
        let third = runner.tick(&mut events).expect("next note tick");
        assert!(!third.ended);
        assert_eq!(events[0].kind(), ChannelKind::Fm);
    }

    #[test]
    fn first_tick_does_not_rewrite_the_default_timer_b() {
        let song = song_with_stream(&[0x21, 1, 0x80]);
        let mut runner = SongRunner::from_song(&song)
            .expect("valid synthetic layout")
            .expect("layout exists");
        let mut events = Vec::new();
        runner.tick(&mut events).expect("first tick");

        let mut writes = Vec::new();
        runner.take_writes(&mut writes);
        assert!(!writes.iter().any(|write| write.address == 0x26));
        assert_eq!(
            writes.last(),
            Some(&RegisterWrite {
                address: 0x27,
                value: 0x3f,
            })
        );
    }

    #[test]
    fn q_gate_emits_keyoff_before_the_note_stream_expires() {
        let song = song_with_stream(&[0xfe, 5, 0x21, 4, 0x80]);
        let mut runner = SongRunner::from_song(&song)
            .expect("valid synthetic layout")
            .expect("layout exists");
        let mut events = Vec::new();

        runner.tick(&mut events).expect("note tick");
        assert!(matches!(
            events.as_slice(),
            [
                PartEvent::Note { .. },
                PartEvent::Frame {
                    state: super::super::effects::VoiceFrame { gate_length: 5, .. },
                    ..
                }
            ]
        ));
        runner.tick(&mut events).expect("gate keyoff tick");
        assert!(matches!(events[0], PartEvent::KeyOff { .. }));
        assert!(matches!(events[1], PartEvent::Frame { .. }));
    }

    #[test]
    fn portamento_frame_reaches_the_target_by_integer_steps() {
        let song = song_with_stream(&[0xda, 0x21, 0x31, 2, 0x80]);
        let mut runner = SongRunner::from_song(&song)
            .expect("valid synthetic layout")
            .expect("layout exists");
        let mut events = Vec::new();

        runner.tick(&mut events).expect("portamento start");
        assert!(matches!(events[0], PartEvent::Portamento { length: 2, .. }));
        let first_pitch = match events[1] {
            PartEvent::Frame { state, .. } => state.pitch,
            _ => panic!("missing initial portamento frame: {events:?}"),
        };
        runner.tick(&mut events).expect("portamento step");
        let second_pitch = match events[0] {
            PartEvent::Frame { state, .. } => state.pitch,
            _ => panic!("missing stepped portamento frame: {events:?}"),
        };
        assert_eq!(first_pitch, 0x128f);

        assert_eq!(second_pitch, 0x13c4);
    }

    #[test]
    fn part_loop_jumps_to_f6_and_optional_loop_limit_eventually_ends() {
        let song = song_with_stream(&[0xf6, 0x21, 1, 0x80]);
        let mut runner = SongRunner::from_song(&song)
            .expect("valid synthetic layout")
            .expect("layout exists");
        runner.set_loop_count(Some(1));
        let mut events = Vec::new();

        let first = runner.tick(&mut events).expect("first note");
        assert!(!first.ended);
        assert!(!first.looped);
        let second = runner.tick(&mut events).expect("one loop");
        assert!(second.looped);
        assert!(!second.ended);
        let third = runner.tick(&mut events).expect("end after limited loop");
        assert!(third.ended);
        assert!(events
            .iter()
            .any(|event| matches!(event, PartEvent::Ended { .. })));
    }

    #[test]
    fn command_jump_resumes_note_scanning_at_the_target() {
        let song = song_with_stream(&[0xf7, 0x1e, 0, 0, 2, 1, 0, 0, 0x21, 1, 0x80]);
        let mut runner = SongRunner::from_song(&song)
            .expect("valid synthetic layout")
            .expect("layout exists");
        let mut events = Vec::new();
        runner.tick(&mut events).expect("jump to note");
        assert_eq!(
            events[0],
            PartEvent::Note {
                part: 0,
                kind: ChannelKind::Fm,
                value: 0x21,
                length: 1,
                masked: false,
            }
        );
    }

    #[test]
    fn fm3_and_ppz_external_parts_activate_in_mmain_order_and_reset_cleanly() {
        let mut main = vec![0u8; 1 + 0x40];
        main[1..3].copy_from_slice(&[0x1a, 0]);

        let fm3 = [0xc6, 0x30, 0, 0, 0, 0, 0, 0x80];
        main[1 + 0x1a..1 + 0x1a + fm3.len()].copy_from_slice(&fm3);
        main[1 + 0x30..1 + 0x33].copy_from_slice(&[0x21, 1, 0x80]);
        let mut layout = MainLayout {
            part_offsets: [0; 11],
            rhythm_table_offset: 0,
            program_area_offset: 0,
        };
        layout.part_offsets[2] = 0x1a;
        let song = ParsedSong {
            main,
            flags: 0,
            format: MainFormat::V1a,
            layout: Some(layout),
            metadata: Metadata::default(),
            companions: CompanionNames::default(),
        };
        let mut runner = SongRunner::from_song(&song)
            .expect("valid synthetic layout")
            .expect("layout exists");
        let mut events = Vec::new();
        runner.tick(&mut events).expect("FM3 extension activation");
        assert!(events.iter().any(|event| {
            matches!(
                event,
                PartEvent::Note {
                    part: 11,
                    kind: ChannelKind::Fm,
                    masked: true,
                    ..
                }
            )
        }));
        assert!(runner.position(11).is_some());
        runner.reset();
        assert!(runner.position(11).is_none());

        let mut main = vec![0u8; 1 + 0x40];
        main[1..3].copy_from_slice(&[0x1a, 0]);

        main[1 + 0x1a] = 0xb4;
        main[1 + 0x1a + 1..1 + 0x1a + 3].copy_from_slice(&[0x30, 0]);
        main[1 + 0x1a + 0x11] = 0x80;
        main[1 + 0x30..1 + 0x33].copy_from_slice(&[0x21, 1, 0x80]);
        let mut layout = MainLayout {
            part_offsets: [0; 11],
            rhythm_table_offset: 0,
            program_area_offset: 0,
        };
        layout.part_offsets[9] = 0x1a;
        let song = ParsedSong {
            main,
            flags: 0,
            format: MainFormat::V1a,
            layout: Some(layout),
            metadata: Metadata::default(),
            companions: CompanionNames::default(),
        };
        let mut runner = SongRunner::from_song(&song)
            .expect("valid synthetic layout")
            .expect("layout exists");
        runner.tick(&mut events).expect("PPZ activation");
        assert!(events.iter().any(|event| {
            matches!(
                event,
                PartEvent::Note {
                    part: 14,
                    kind: ChannelKind::Ppz8,
                    masked: false,
                    ..
                }
            )
        }));
    }

    #[test]
    fn rhythm_part_uses_the_radtbl_indirection_and_subsequence_length() {
        let mut main = vec![0; 1 + 0x30];
        main[1..3].copy_from_slice(&[0x1a, 0]);
        main[1 + 0x14..1 + 0x16].copy_from_slice(&0x1c_u16.to_le_bytes());
        main[1 + 0x16..1 + 0x18].copy_from_slice(&0x20_u16.to_le_bytes());
        main[1 + 0x1a] = 0x80;
        main[1 + 0x1c] = 0;
        main[1 + 0x1d] = 0x80;
        main[1 + 0x20..1 + 0x22].copy_from_slice(&0x24_u16.to_le_bytes());
        main[1 + 0x24..1 + 0x27].copy_from_slice(&[0, 3, 0xff]);
        let song = ParsedSong {
            main,
            flags: 0,
            format: MainFormat::V1a,
            layout: Some(MainLayout {
                part_offsets: [0x1a, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x1c],
                rhythm_table_offset: 0x20,
                program_area_offset: 0,
            }),
            metadata: Metadata::default(),
            companions: CompanionNames::default(),
        };
        let mut runner = SongRunner::from_song(&song)
            .expect("valid synthetic layout")
            .expect("layout exists");
        let mut events = Vec::new();
        let tick = runner.tick(&mut events).expect("rhythm entry");
        assert!(!tick.ended);
        assert_eq!(
            events,
            [PartEvent::Rhythm {
                part: 10,
                mask: 0,
                length: 3,
            }]
        );
        runner.tick(&mut events).expect("rhythm hold");
        runner.tick(&mut events).expect("rhythm hold");
        let end = runner.tick(&mut events).expect("return to mml end");
        assert!(end.ended);
        assert!(events
            .iter()
            .any(|event| matches!(event, PartEvent::Ended { part: 10, .. })));
    }

    #[test]
    fn malformed_offsets_and_command_only_cycles_are_reported_without_hanging() {
        let mut song = song_with_stream(&[0x81]);
        song.layout.as_mut().expect("layout").part_offsets[10] = 0xfff0;
        assert_eq!(
            SongRunner::from_song(&song),
            Err(StreamError::InvalidPartOffset {
                part: 10,
                offset: 0xfff0
            })
        );

        let song = song_with_stream(&[0xf6, 0x80]);
        let mut runner = SongRunner::from_song(&song)
            .expect("valid synthetic layout")
            .expect("layout exists");
        let mut events = Vec::new();
        assert_eq!(
            runner.tick(&mut events),
            Err(StreamError::ActionBudgetExceeded {
                part: 0,
                position: 0x1b,
            })
        );
    }

    trait EventKind {
        fn kind(&self) -> ChannelKind;
    }

    impl EventKind for PartEvent {
        fn kind(&self) -> ChannelKind {
            match self {
                PartEvent::Note { kind, .. }
                | PartEvent::Portamento { kind, .. }
                | PartEvent::Ended { kind, .. } => *kind,
                PartEvent::Rhythm { .. } => ChannelKind::Rhythm,
                PartEvent::KeyOff { kind, .. } | PartEvent::Frame { kind, .. } => *kind,
            }
        }
    }
}
