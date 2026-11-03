// SPDX-License-Identifier: AGPL-3.0-only

use alloc::collections::VecDeque;
use alloc::string::String;
use alloc::vec;
use alloc::vec::Vec;

use crate::opna::{ChipRenderer, RegisterWrite, PMD_PSG_MIX_GAIN};
use crate::pmd;
use crate::pmd::channel::ChannelKind;
use crate::visual::{
    empty_channel_snapshots, ChannelSnapshot, PlaybackStatus, VisualChannelKind, CHANNEL_COUNT,
    NO_NOTE,
};

const OPNA_CLOCK_HZ: u32 = 7_987_200;
const DEFAULT_REGISTER_WAIT_NS: u64 = 30_000;
const OPNA_SOURCE_RATE: u64 = 55_466;

const END_TAIL_FRAMES: usize = 1024;

fn startup_timer_b_delta(sample_rate: u32) -> u8 {
    if sample_rate == 55_467 {
        12
    } else {
        11
    }
}

#[derive(Debug)]
struct DriverEvent {
    frame: u64,
    time_us: u64,
    writes: Vec<RegisterWrite>,
    ended: bool,
    snapshot: Option<[ChannelSnapshot; CHANNEL_COUNT]>,
}

pub trait FileProvider {
    fn get(&self, name: &str) -> Option<&[u8]>;
}

#[derive(Clone, Debug, Default, Eq, PartialEq)]
pub struct Metadata {
    pub title: Option<String>,

    pub composer: Option<String>,

    pub arranger: Option<String>,

    pub comment: Option<String>,
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub struct TickStatus {
    pub ended: bool,

    pub looped: bool,

    pub writes: usize,
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
#[non_exhaustive]
pub enum Error {
    InvalidSampleRate,

    EmptyMainFile,

    TruncatedFile,

    FileTooLarge,

    InvalidHeader,

    InvalidStream,

    MissingCompanion,

    InvalidCompanion,
}

pub struct Player {
    sample_rate: u32,
    metadata: Metadata,
    position_samples: u64,
    loop_count: Option<u32>,
    song: Option<pmd::ParsedSong>,
    #[allow(dead_code)]
    companion_banks: pmd::banks::CompanionBanks,
    runner: Option<pmd::stream::SongRunner>,
    stream_events: Vec<pmd::stream::PartEvent>,
    stream_writes: Vec<pmd::channel::RegisterWrite>,
    stream_trace: Vec<RegisterWrite>,
    timing: pmd::timing::PmdTiming,
    chip: Option<ChipRenderer>,
    pending_events: VecDeque<DriverEvent>,
    driver_timer_times: Vec<u64>,
    scheduled_time_us: u64,
    scheduled_output_frame: u64,
    render_block: Vec<[i32; 2]>,
    wait_samples: VecDeque<[i32; 2]>,
    output_samples: VecDeque<[i32; 2]>,
    generated_output_frame: u64,
    register_wait_fractional: u64,
    sequencer_ended: bool,
    end_tail_frames: usize,
    channel_mutes: [bool; 11],
    channel_snapshots: [ChannelSnapshot; CHANNEL_COUNT],

    tick_counter: u64,
}

impl Player {
    pub fn new(sample_rate: u32) -> Self {
        Self {
            sample_rate,
            metadata: Metadata::default(),
            position_samples: 0,
            loop_count: None,
            song: None,
            companion_banks: pmd::banks::CompanionBanks::empty(),
            runner: None,
            stream_events: Vec::new(),
            stream_writes: Vec::new(),
            stream_trace: Vec::new(),
            timing: pmd::timing::PmdTiming::new(OPNA_CLOCK_HZ)
                .expect("the fixed OPNA clock is valid"),
            chip: None,
            pending_events: VecDeque::with_capacity(16),
            driver_timer_times: Vec::with_capacity(9),
            scheduled_time_us: 0,
            scheduled_output_frame: 0,
            render_block: Vec::with_capacity(1024),
            wait_samples: VecDeque::with_capacity(256),
            output_samples: VecDeque::with_capacity(1024),
            generated_output_frame: 0,
            register_wait_fractional: 0,
            sequencer_ended: true,
            end_tail_frames: 0,
            channel_mutes: [false; 11],
            channel_snapshots: empty_channel_snapshots(),
            tick_counter: 0,
        }
    }

    pub fn load(&mut self, main: &[u8], files: &dyn FileProvider) -> Result<(), Error> {
        if self.sample_rate == 0 {
            return Err(Error::InvalidSampleRate);
        }
        if main.is_empty() {
            return Err(Error::EmptyMainFile);
        }
        let song = pmd::parse_song(main)?;
        let companion_banks =
            pmd::banks::load_companions(&song.companions, files).map_err(|error| match error {
                pmd::banks::CompanionLoadError::Missing => Error::MissingCompanion,
                pmd::banks::CompanionLoadError::Invalid => Error::InvalidCompanion,
            })?;
        let runner =
            pmd::stream::SongRunner::from_song_with_p86(&song, companion_banks.p86.is_some())
                .map_err(|_| Error::InvalidStream)?;
        self.metadata = song.metadata.clone();
        self.song = Some(song);
        self.companion_banks = companion_banks;
        self.runner = runner;
        if let Some(runner) = self.runner.as_mut() {
            runner.set_loop_count(self.loop_count);
        }
        self.position_samples = 0;
        self.channel_snapshots = empty_channel_snapshots();
        self.timing
            .start_with_timer_b_delta(startup_timer_b_delta(self.sample_rate));
        self.reset_renderer();
        Ok(())
    }

    pub fn render(&mut self, out: &mut [f32]) {
        if self.chip.is_none() {
            out.fill(0.0);
            return;
        }

        let frame_count = out.len() / 2;
        self.ensure_output_samples(frame_count);
        for frame in 0..frame_count {
            let sample = self.output_samples.pop_front().unwrap_or([0; 2]);
            out[frame * 2] = sample[0] as f32 / 32_768.0;
            out[frame * 2 + 1] = sample[1] as f32 / 32_768.0;
            self.position_samples = self.position_samples.saturating_add(1);
        }

        if let Some(last) = out.get_mut(frame_count * 2..) {
            last.fill(0.0);
        }
    }

    pub fn metadata(&self) -> &Metadata {
        &self.metadata
    }

    pub fn position_samples(&self) -> u64 {
        self.position_samples
    }

    pub fn channel_snapshots(&self) -> [ChannelSnapshot; CHANNEL_COUNT] {
        self.channel_snapshots
    }

    pub fn playback_status(&self) -> PlaybackStatus {
        let Some(runner) = self.runner.as_ref() else {
            return PlaybackStatus {
                loop_limit: self.loop_count,
                ended: true,
                ..PlaybackStatus::default()
            };
        };
        let timer_b = runner.timer_b_value();
        PlaybackStatus {
            ticks: self.tick_counter,
            loops_completed: runner.loops_completed(),
            loop_limit: self.loop_count,
            timer_b,
            tempo_quarter: pmd::timing::timer_b_to_quarter(timer_b),
            ended: self.sequencer_ended,
        }
    }

    pub fn set_loop_count(&mut self, count: Option<u32>) {
        self.loop_count = count;
        if let Some(runner) = self.runner.as_mut() {
            runner.set_loop_count(count);
        }
    }

    pub fn voice_peaks(&mut self) -> crate::VoicePeaks {
        self.chip
            .as_mut()
            .map(ChipRenderer::take_voice_peaks)
            .unwrap_or_default()
    }

    pub fn set_channel_mute(&mut self, channel: usize, muted: bool) -> bool {
        let Some(slot) = self.channel_mutes.get_mut(channel) else {
            return false;
        };
        *slot = muted;
        if let Some(chip) = self.chip.as_mut() {
            chip.set_channel_mute(channel, muted);
        }
        true
    }

    pub fn channel_muted(&self, channel: usize) -> Option<bool> {
        self.channel_mutes.get(channel).copied()
    }

    pub fn reset(&mut self) {
        self.position_samples = 0;
        self.tick_counter = 0;
        self.channel_snapshots = empty_channel_snapshots();
        if let Some(runner) = self.runner.as_mut() {
            runner.reset();
        }
        self.stream_events.clear();
        self.stream_writes.clear();
        self.stream_trace.clear();
        self.pending_events.clear();
        self.driver_timer_times.clear();
        self.scheduled_time_us = 0;
        self.scheduled_output_frame = 0;
        self.generated_output_frame = 0;
        self.timing
            .start_with_timer_b_delta(startup_timer_b_delta(self.sample_rate));
        if self.song.is_some() {
            self.reset_renderer();
        } else {
            self.chip = None;
            self.pending_events.clear();
            self.driver_timer_times.clear();
            self.scheduled_time_us = 0;
            self.scheduled_output_frame = 0;
            self.generated_output_frame = 0;
            self.wait_samples.clear();
            self.output_samples.clear();
            self.register_wait_fractional = 0;
            self.sequencer_ended = true;
            self.end_tail_frames = 0;
        }
    }

    #[allow(dead_code)]
    pub(crate) fn advance_tick(&mut self) -> Result<pmd::stream::SongTick, Error> {
        let Some(runner) = self.runner.as_mut() else {
            return Ok(pmd::stream::SongTick {
                ended: true,
                looped: false,
                writes: 0,
            });
        };
        let timer_tick = self.timing.timer_b_tick();
        self.driver_timer_times.clear();
        self.driver_timer_times.extend(
            timer_tick.timer_a_times[..usize::from(timer_tick.timer_a_count)]
                .iter()
                .copied(),
        );
        self.driver_timer_times.push(timer_tick.time_us);
        self.stream_writes.clear();
        self.stream_events.clear();
        for index in 0..usize::from(timer_tick.timer_a_count) {
            if let Some(&time_us) = timer_tick.timer_a_times.get(index) {
                runner.timer_a_tick();
                let start = self.stream_writes.len();
                runner.take_timer_writes(&mut self.stream_writes);
                self.stream_trace
                    .extend(
                        self.stream_writes[start..]
                            .iter()
                            .map(|write| RegisterWrite {
                                time_us,
                                tick: timer_tick.tick.saturating_sub(1),
                                address: write.address,
                                value: write.value,
                            }),
                    );

                if time_us != timer_tick.time_us {
                    self.stream_trace.push(RegisterWrite {
                        time_us,
                        tick: timer_tick.tick.saturating_sub(1),
                        address: 0x27,
                        value: runner.ch3_mode() | 0x30,
                    });
                }
            }
        }

        self.stream_writes.clear();
        let tick = runner
            .tick_with_timer_a(&mut self.stream_events, timer_tick.timer_a_delta)
            .map_err(|_| Error::InvalidStream)?;
        self.timing.timers.set_timer_b_value(runner.timer_b_value());
        self.tick_counter = self.tick_counter.saturating_add(1);
        runner.take_writes(&mut self.stream_writes);
        self.stream_trace
            .extend(self.stream_writes.iter().map(|write| RegisterWrite {
                time_us: timer_tick.time_us,
                tick: timer_tick.tick,
                address: write.address,
                value: write.value,
            }));
        Ok(tick)
    }

    pub fn tick(&mut self) -> Result<TickStatus, Error> {
        let tick = self.advance_tick()?;
        Ok(TickStatus {
            ended: tick.ended,
            looped: tick.looped,
            writes: tick.writes,
        })
    }

    #[allow(dead_code)]
    pub fn take_sequencer_trace(&mut self, output: &mut Vec<RegisterWrite>) {
        output.append(&mut self.stream_trace);
    }

    fn reset_renderer(&mut self) {
        let mut chip = ChipRenderer::new(OPNA_CLOCK_HZ, 44_100, false)
            .expect("sample rate was validated before renderer creation");
        chip.set_psg_volume(PMD_PSG_MIX_GAIN);
        if let Some(bank) = self.companion_banks.adpcm.as_ref() {
            let memory = adpcm_memory_image(bank);
            chip.chip_mut().set_adpcm_a_memory(&memory);
        }

        self.chip = Some(chip);
        if let Some(chip) = self.chip.as_mut() {
            for (channel, &muted) in self.channel_mutes.iter().enumerate() {
                chip.set_channel_mute(channel, muted);
            }
        }
        self.pending_events.clear();
        self.driver_timer_times.clear();
        self.scheduled_time_us = 0;
        self.scheduled_output_frame = 0;
        self.generated_output_frame = 0;
        self.wait_samples.clear();
        self.output_samples.clear();
        self.register_wait_fractional = 0;
        self.sequencer_ended = false;
        self.end_tail_frames = 0;
        self.apply_initial_driver_init();
        if let Some(chip) = self.chip.as_mut() {
            assert!(chip.set_target_rate(self.sample_rate));
        }

        self.apply_silence();
        self.apply_silence();

        self.apply_silence();
        self.apply_silence();
        self.apply_music_start();
    }

    fn apply_initial_driver_init(&mut self) {
        for (address, value) in [
            (0x029, 0x00),
            (0x024, 0x00),
            (0x025, 0x00),
            (0x026, 0x00),
            (0x027, 0x3f),
        ] {
            self.apply_startup_write(address, value);
        }
        self.clear_output_buffer();
        self.apply_opn_init_registers();
        self.apply_startup_write(0x007, 0xbf);
        self.apply_silence();
        for (address, value) in [
            (0x026, 0xc8),
            (0x025, 0x00),
            (0x024, 0x00),
            (0x027, 0x3f),
            (0x029, 0x83),
        ] {
            self.apply_startup_write(address, value);
        }
    }

    fn apply_silence(&mut self) {
        for address in [
            0x080, 0x081, 0x082, 0x084, 0x085, 0x086, 0x088, 0x089, 0x08a, 0x08c, 0x08d, 0x08e,
            0x180, 0x181, 0x184, 0x185, 0x188, 0x189, 0x18c, 0x18d, 0x182, 0x186, 0x18a, 0x18e,
        ] {
            self.apply_startup_write(address, 0xff);
        }
        for value in [0x00, 0x01, 0x02, 0x04, 0x05, 0x06] {
            self.apply_startup_write(0x28, value);
        }
        for (address, value) in [
            (0x007, 0xbf),
            (0x008, 0x00),
            (0x009, 0x00),
            (0x00a, 0x00),
            (0x010, 0xff),
            (0x101, 0x02),
            (0x100, 0x01),
            (0x110, 0x80),
            (0x110, 0x18),
        ] {
            self.apply_startup_write(address, value);
        }
    }

    fn apply_music_start(&mut self) {
        self.apply_startup_write(0x26, 0x00);
        self.apply_startup_write(0x27, 0x00);
        self.apply_silence();
        self.clear_output_buffer();
        self.apply_opn_init_registers();
        self.apply_setint();

        self.apply_startup_write(0x027, 0x3f);
        self.timing
            .start_with_timer_b_delta(startup_timer_b_delta(self.sample_rate));
    }

    fn apply_opn_init_registers(&mut self) {
        self.apply_startup_write(0x029, 0x83);
        self.apply_startup_write(0x006, 0x00);

        for base in [0x090, 0x190] {
            for offset in [0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14] {
                self.apply_startup_write(base + offset, 0x00);
            }
        }
        for address in [0x0b4, 0x0b5, 0x0b6, 0x1b4, 0x1b5, 0x1b6] {
            self.apply_startup_write(address, 0xc0);
        }
        for (address, value) in [
            (0x022, 0x00),
            (0x010, 0xff),
            (0x011, 0x30),
            (0x10c, 0xff),
            (0x10d, 0xff),
        ] {
            self.apply_startup_write(address, value);
        }
    }

    fn apply_setint(&mut self) {
        for (address, value) in [(0x026, 0xc8), (0x025, 0x00), (0x024, 0x00), (0x027, 0x3f)] {
            self.apply_startup_write(address, value);
        }
    }

    fn apply_startup_write(&mut self, address: u16, value: u8) {
        self.apply_register_wait();
        if let Some(chip) = self.chip.as_mut() {
            chip.write(address, value);
        }
    }

    fn clear_output_buffer(&mut self) {
        self.wait_samples.clear();
        if let Some(chip) = self.chip.as_mut() {
            chip.clear_buffer();
        }
    }

    fn ensure_output_samples(&mut self, needed: usize) {
        while self.output_samples.len() < needed {
            if self.pending_events.is_empty() && !self.sequencer_ended {
                self.queue_next_tick_events();
            }

            let Some(next_frame) = self.pending_events.front().map(|event| event.frame) else {
                if self.sequencer_ended && self.end_tail_frames > 0 {
                    let frame_count =
                        (needed - self.output_samples.len()).min(self.end_tail_frames);
                    self.render_output_block(frame_count);
                    self.generated_output_frame = self
                        .generated_output_frame
                        .saturating_add(frame_count as u64);
                    self.end_tail_frames -= frame_count;
                }
                break;
            };

            if self.generated_output_frame < next_frame {
                let frame_count = next_frame
                    .saturating_sub(self.generated_output_frame)
                    .min(usize::MAX as u64) as usize;
                self.render_output_block(frame_count);
                self.generated_output_frame = self
                    .generated_output_frame
                    .saturating_add(frame_count as u64);
                continue;
            }

            let event = self.pending_events.pop_front().expect("front was present");
            self.apply_driver_event(event);
        }
    }

    fn apply_driver_event(&mut self, event: DriverEvent) {
        let DriverEvent {
            frame,
            writes,
            ended,
            snapshot,
            ..
        } = event;
        debug_assert!(frame <= self.generated_output_frame);
        for write in writes {
            self.apply_register_wait();
            if let Some(chip) = self.chip.as_mut() {
                chip.write(write.address, write.value);
            }
        }
        if let Some(snapshot) = snapshot {
            self.channel_snapshots = snapshot;
        }
        if ended {
            self.sequencer_ended = true;
            self.end_tail_frames = END_TAIL_FRAMES;
        }
    }

    fn render_output_block(&mut self, frame_count: usize) {
        if frame_count == 0 {
            return;
        }
        self.render_block.resize(frame_count, [0; 2]);
        self.render_block.fill([0; 2]);
        if let Some(chip) = self.chip.as_mut() {
            chip.render_with_prefill(&mut self.render_block, &mut self.wait_samples);
        } else {
            self.render_block.fill([0; 2]);
        }
        self.output_samples
            .extend(self.render_block.iter().copied());
    }

    fn queue_next_tick_events(&mut self) {
        if self.sequencer_ended {
            return;
        }

        let trace_start = self.stream_trace.len();
        let tick = self
            .advance_tick()
            .expect("a PMD stream accepted by load must remain traversable");
        let visual_snapshot =
            channel_snapshot_after_events(self.channel_snapshots, &self.stream_events);
        let new_writes = self.stream_trace[trace_start..].to_vec();

        self.stream_trace.truncate(trace_start);
        let mut write_index = 0usize;
        let mut event_index = 0usize;
        while event_index < self.driver_timer_times.len() {
            let time_us = self.driver_timer_times[event_index];
            let mut writes = Vec::new();
            while let Some(write) = new_writes.get(write_index) {
                if write.time_us != time_us {
                    break;
                }
                writes.push(*write);
                write_index += 1;
            }

            let frame_delta = time_us
                .saturating_sub(self.scheduled_time_us)
                .saturating_mul(u64::from(self.sample_rate))
                / 1_000_000;
            self.scheduled_output_frame = self.scheduled_output_frame.saturating_add(frame_delta);
            self.scheduled_time_us = time_us;

            if let Some(previous) = self.pending_events.back_mut() {
                if previous.time_us == time_us {
                    previous.writes.extend(writes);
                    if event_index + 1 == self.driver_timer_times.len() {
                        previous.ended = tick.ended;
                        previous.snapshot = Some(visual_snapshot);
                    }
                    event_index += 1;
                    continue;
                }
            }
            self.pending_events.push_back(DriverEvent {
                frame: self.scheduled_output_frame,
                time_us,
                writes,
                ended: event_index + 1 == self.driver_timer_times.len() && tick.ended,
                snapshot: (event_index + 1 == self.driver_timer_times.len())
                    .then_some(visual_snapshot),
            });
            event_index += 1;
        }

        if self.driver_timer_times.is_empty() {
            self.pending_events.push_back(DriverEvent {
                frame: self.scheduled_output_frame,
                time_us: self.scheduled_time_us,
                writes: new_writes.clone(),
                ended: tick.ended,
                snapshot: Some(visual_snapshot),
            });
        } else if write_index < new_writes.len() {
            let writes = new_writes[write_index..].to_vec();
            self.pending_events.push_back(DriverEvent {
                frame: self.scheduled_output_frame,
                time_us: self.scheduled_time_us,
                writes,
                ended: tick.ended,
                snapshot: Some(visual_snapshot),
            });
        }
    }

    fn apply_register_wait(&mut self) {
        let wait_rate = self
            .chip
            .as_ref()
            .map_or(u64::from(self.sample_rate), |chip| {
                if chip.interpolation() {
                    OPNA_SOURCE_RATE
                } else {
                    u64::from(chip.target_rate())
                }
            });
        let wait_count = DEFAULT_REGISTER_WAIT_NS.saturating_mul(wait_rate) / 1_000_000;
        let mut frame_count = wait_count / 1_000;
        self.register_wait_fractional += wait_count % 1_000;
        if self.register_wait_fractional > 1_000 {
            frame_count += 1;
            self.register_wait_fractional -= 1_000;
        }
        for _ in 0..frame_count {
            let sample = self.render_chip_one();
            self.wait_samples.push_back(sample);
        }
    }

    fn render_chip_one(&mut self) -> [i32; 2] {
        let mut output = [[0i32; 2]; 1];
        if let Some(chip) = self.chip.as_mut() {
            chip.render_source(&mut output);
        }
        output[0]
    }
}

fn adpcm_memory_image(bank: &pmd::banks::AdpcmBank) -> Vec<u8> {
    const ADPCM_MEMORY_SIZE: usize = 0x40000;
    const DATA_OFFSET: usize = 0x26 * 32;
    let mut memory = vec![0x08; ADPCM_MEMORY_SIZE];
    let length = bank
        .data
        .len()
        .min(ADPCM_MEMORY_SIZE.saturating_sub(DATA_OFFSET));
    memory[DATA_OFFSET..DATA_OFFSET + length].copy_from_slice(&bank.data[..length]);
    memory
}

fn channel_snapshot_after_events(
    mut snapshots: [ChannelSnapshot; CHANNEL_COUNT],
    events: &[pmd::stream::PartEvent],
) -> [ChannelSnapshot; CHANNEL_COUNT] {
    for event in events {
        match event {
            pmd::stream::PartEvent::Note {
                part,
                kind,
                value,
                length,
                masked,
            } => {
                let Some(index) = visual_channel_index(*part) else {
                    continue;
                };
                let snapshot = &mut snapshots[index];
                snapshot.kind = visual_channel_kind(*kind);
                snapshot.active = !masked;
                snapshot.releasing = false;
                snapshot.note = if *masked || *value == NO_NOTE || value & 0x0f == 0x0c {
                    NO_NOTE
                } else {
                    *value
                };
                snapshot.remaining = u16::from(*length);
                snapshot.level = if *masked { 0 } else { 255 };
                if *part == 2 {
                    snapshot.voice_mask |= event_voice_mask(*part);
                } else {
                    snapshot.voice_mask = event_voice_mask(*part);
                }
                snapshot.trigger = snapshot.trigger.wrapping_add(1);
            }
            pmd::stream::PartEvent::Portamento { part, kind, length } => {
                let Some(index) = visual_channel_index(*part) else {
                    continue;
                };
                let snapshot = &mut snapshots[index];
                snapshot.kind = visual_channel_kind(*kind);
                snapshot.active = true;
                snapshot.releasing = false;
                snapshot.remaining = u16::from(*length);
            }
            pmd::stream::PartEvent::KeyOff { part, kind } => {
                let Some(index) = visual_channel_index(*part) else {
                    continue;
                };
                let snapshot = &mut snapshots[index];
                snapshot.kind = visual_channel_kind(*kind);
                snapshot.releasing = true;
                snapshot.remaining = 0;
            }
            pmd::stream::PartEvent::Frame { part, kind, state } => {
                let Some(index) = visual_channel_index(*part) else {
                    continue;
                };
                let snapshot = &mut snapshots[index];
                snapshot.kind = visual_channel_kind(*kind);
                snapshot.active |= state.force_key_on;
                snapshot.level = visual_level(*kind, state.volume);
                snapshot.pitch = if state.p86_pitch != 0 {
                    state.p86_pitch
                } else {
                    state.pitch.max(0) as u32
                };
                snapshot.lfo = state
                    .pitch_lfo
                    .clamp(i32::from(i16::MIN), i32::from(i16::MAX))
                    as i16;
                snapshot.envelope = state
                    .envelope
                    .clamp(i32::from(i16::MIN), i32::from(i16::MAX))
                    as i16;
                snapshot.remaining = state.remaining.max(0).min(i32::from(u16::MAX)) as u16;
                snapshot.operator_mask = state.slot_mask.clamp(0, i32::from(u8::MAX)) as u8;
                snapshot.voice = if state.voice_number < 0 {
                    NO_NOTE
                } else {
                    state.voice_number.min(i32::from(u8::MAX)) as u8
                };
                snapshot.pan = visual_pan(*kind, state.fmpan);
                merge_voice_mask(snapshot, *part);
                if state.force_key_on {
                    snapshot.trigger = snapshot.trigger.wrapping_add(1);
                }
            }
            pmd::stream::PartEvent::Rhythm { part, mask, length } => {
                let Some(index) = visual_channel_index(*part) else {
                    continue;
                };
                let snapshot = &mut snapshots[index];
                snapshot.kind = VisualChannelKind::Rhythm;
                snapshot.active = true;
                snapshot.releasing = false;
                snapshot.note = NO_NOTE;
                snapshot.voice = NO_NOTE;
                snapshot.level = mask.count_ones().saturating_mul(32).min(255) as u8;
                snapshot.remaining = u16::from(*length);
                snapshot.voice_mask = *mask;
                snapshot.trigger = snapshot.trigger.wrapping_add(1);
            }
            pmd::stream::PartEvent::Ended { part, kind } => {
                let Some(index) = visual_channel_index(*part) else {
                    continue;
                };
                let snapshot = &mut snapshots[index];
                snapshot.kind = visual_channel_kind(*kind);
                clear_voice_mask(snapshot, *part);
                snapshot.active = snapshot.voice_mask != 0;
                snapshot.releasing = true;
                snapshot.remaining = 0;
            }
        }
    }
    snapshots
}

fn visual_channel_index(part: usize) -> Option<usize> {
    match part {
        0..=10 => Some(part),
        11..=13 => Some(2),
        14..=21 => Some(9),
        _ => None,
    }
}

fn visual_channel_kind(kind: ChannelKind) -> VisualChannelKind {
    match kind {
        ChannelKind::Fm => VisualChannelKind::Fm,
        ChannelKind::Ssg => VisualChannelKind::Ssg,
        ChannelKind::Rhythm => VisualChannelKind::Rhythm,
        ChannelKind::Adpcm | ChannelKind::P86 | ChannelKind::Ppz8 => VisualChannelKind::Adpcm,
    }
}

fn visual_level(kind: ChannelKind, volume: i32) -> u8 {
    let (maximum, scale) = match kind {
        ChannelKind::Fm => (127, 2),
        ChannelKind::Ssg => (15, 17),
        ChannelKind::Rhythm => (15, 17),
        ChannelKind::Adpcm | ChannelKind::P86 | ChannelKind::Ppz8 => (255, 1),
    };
    volume.clamp(0, maximum).saturating_mul(scale) as u8
}

fn visual_pan(kind: ChannelKind, fmpan: i32) -> u8 {
    if !matches!(kind, ChannelKind::Fm) {
        return 128;
    }
    match fmpan & 0xc0 {
        0x40 => 0,
        0x80 => 255,
        _ => 128,
    }
}

fn event_voice_mask(part: usize) -> u16 {
    if part < CHANNEL_COUNT {
        1
    } else if part < 27 {
        1u16 << (part - 10)
    } else {
        0
    }
}

fn merge_voice_mask(snapshot: &mut ChannelSnapshot, part: usize) {
    let mask = event_voice_mask(part);
    if part < CHANNEL_COUNT && part != 2 {
        snapshot.voice_mask = mask;
    } else {
        snapshot.voice_mask |= mask;
    }
}

fn clear_voice_mask(snapshot: &mut ChannelSnapshot, part: usize) {
    let mask = event_voice_mask(part);
    snapshot.voice_mask &= !mask;
}

#[cfg(test)]
mod tests {
    use super::{
        channel_snapshot_after_events, Error, FileProvider, Metadata, Player, OPNA_CLOCK_HZ,
    };
    use crate::opna::ChipRenderer;
    use crate::pmd;
    use crate::pmd::channel::ChannelKind;
    use crate::pmd::stream::PartEvent;
    use crate::visual::{empty_channel_snapshots, VisualChannelKind, CHANNEL_COUNT};
    use alloc::vec::Vec;

    struct EmptyFiles;

    impl FileProvider for EmptyFiles {
        fn get(&self, _name: &str) -> Option<&[u8]> {
            None
        }
    }

    struct OneFile {
        name: &'static str,
        bytes: Vec<u8>,
    }

    impl FileProvider for OneFile {
        fn get(&self, name: &str) -> Option<&[u8]> {
            (name == self.name).then_some(self.bytes.as_slice())
        }
    }

    fn song_with_pcm_memo() -> Vec<u8> {
        let mut main = vec![0u8; 0x100];
        main[..3].copy_from_slice(&[0, 0x1a, 0]);
        main[1 + 0x18..1 + 0x1a].copy_from_slice(&0x40u16.to_le_bytes());
        main[1 + 0x3c..1 + 0x3e].copy_from_slice(&0x50u16.to_le_bytes());
        main[1 + 0x3e] = 0x40;
        main[1 + 0x50..1 + 0x52].copy_from_slice(&0x70u16.to_le_bytes());
        main[1 + 0x70..1 + 0x79].copy_from_slice(b"bank.PPC\0");
        main
    }

    fn empty_ppc() -> Vec<u8> {
        let mut ppc = vec![0u8; 1056];
        ppc[..30].copy_from_slice(b"ADPCM DATA for  PMD ver.4.4-  ");
        ppc[30..32].copy_from_slice(&0x26u16.to_le_bytes());
        ppc
    }

    #[test]
    fn public_player_boundary_owns_only_byte_provider_state() {
        let mut player = Player::new(44_100);
        let files = EmptyFiles;

        assert_eq!(player.metadata(), &Metadata::default());
        assert_eq!(player.position_samples(), 0);
        player.set_loop_count(Some(2));
        player.reset();
        assert_eq!(player.position_samples(), 0);
        assert_eq!(player.load(&[0], &files), Err(Error::TruncatedFile));
    }

    #[test]
    fn channel_mute_state_is_checked_and_survives_reset() {
        let mut player = Player::new(44_100);
        assert_eq!(player.channel_muted(0), Some(false));
        assert!(player.set_channel_mute(0, true));
        assert_eq!(player.channel_muted(0), Some(true));
        assert!(!player.set_channel_mute(11, true));
        player.reset();
        assert_eq!(player.channel_muted(0), Some(true));
    }

    #[test]
    fn channel_snapshots_cover_physical_and_dynamic_voice_rows() {
        let initial = empty_channel_snapshots();
        assert_eq!(initial.len(), CHANNEL_COUNT);
        assert_eq!(initial[0].kind, VisualChannelKind::Fm);
        assert_eq!(initial[6].kind, VisualChannelKind::Ssg);
        assert_eq!(initial[9].kind, VisualChannelKind::Adpcm);
        assert_eq!(initial[10].kind, VisualChannelKind::Rhythm);

        let after_note = channel_snapshot_after_events(
            initial,
            &[PartEvent::Note {
                part: 11,
                kind: ChannelKind::Fm,
                value: 0x42,
                length: 8,
                masked: false,
            }],
        );
        assert!(after_note[2].active);
        assert_eq!(after_note[2].note, 0x42);
        assert_eq!(after_note[2].voice_mask, 2);

        let after_end = channel_snapshot_after_events(
            after_note,
            &[PartEvent::Ended {
                part: 11,
                kind: ChannelKind::Fm,
            }],
        );
        assert!(!after_end[2].active);
        assert!(after_end[2].releasing);
        assert_eq!(after_end[2].voice_mask, 0);
    }

    #[test]
    fn load_rejects_invalid_input_before_parser_is_called() {
        let files = EmptyFiles;
        assert_eq!(
            Player::new(0).load(&[0], &files),
            Err(Error::InvalidSampleRate)
        );
        assert_eq!(
            Player::new(44_100).load(&[], &files),
            Err(Error::EmptyMainFile)
        );
    }

    #[test]
    fn load_accepts_a_valid_header_before_playback_layout_is_needed() {
        let files = EmptyFiles;
        let mut player = Player::new(44_100);
        assert_eq!(player.load(&[0, 0x1a, 0], &files), Ok(()));
        assert_eq!(player.metadata(), &Metadata::default());
    }

    #[test]
    fn render_is_stateful_across_frontend_chunks() {
        let files = EmptyFiles;
        let main = [0, 0x1a, 0];
        let mut contiguous = Player::new(44_100);
        contiguous
            .load(&main, &files)
            .expect("header-only PMD loads");
        let mut expected = vec![0.0; 128];
        contiguous.render(&mut expected);

        let mut chunked = Player::new(44_100);
        chunked.load(&main, &files).expect("header-only PMD loads");
        let mut actual = vec![0.0; 128];
        chunked.render(&mut actual[..64]);
        chunked.render(&mut actual[64..]);

        assert_eq!(chunked.position_samples(), 64);
        assert_eq!(actual, expected);
        assert!(actual.iter().all(|sample| sample.is_finite()));
    }

    #[test]
    fn reused_render_block_does_not_accumulate_previous_pcm() {
        let mut player = Player::new(44_100);
        player.chip =
            Some(ChipRenderer::new(OPNA_CLOCK_HZ, 44_100, false).expect("valid renderer"));
        player.render_block = vec![[123, -321]; 2];

        player.render_output_block(1);

        assert_eq!(player.output_samples.pop_front(), Some([0, 0]));
    }

    #[test]
    fn render_does_not_retain_a_full_song_sequencer_trace() {
        let files = EmptyFiles;
        let mut main = vec![0; 1 + 0x1a + 3];
        main[..3].copy_from_slice(&[0, 0x1a, 0]);
        main[1 + 0x1a..].copy_from_slice(&[0x21, 1, 0x80]);

        let mut player = Player::new(44_100);
        player
            .load(&main, &files)
            .expect("synthetic PMD should load");
        let mut output = [0.0; 2 * 1024];
        player.render(&mut output);

        assert!(player.stream_trace.is_empty());
        assert!(player.stream_events.len() <= 2);
    }

    #[test]
    fn scheduled_frames_floor_each_timer_interval() {
        let first = 15_956u64 * 44_100 / 1_000_000;
        let second = (18_462u64 - 15_956) * 44_100 / 1_000_000;
        assert_eq!(first, 703);
        assert_eq!(second, 110);
        assert_eq!(first + second, 813);
    }

    #[test]
    fn load_copies_a_case_insensitive_companion_bank_from_the_provider() {
        let files = OneFile {
            name: "BANK.PPC",
            bytes: empty_ppc(),
        };
        let mut player = Player::new(44_100);
        assert_eq!(player.load(&song_with_pcm_memo(), &files), Ok(()));
        assert_eq!(
            player
                .companion_banks
                .adpcm
                .as_ref()
                .map(|bank| bank.data.len()),
            Some(0)
        );
    }

    #[test]
    fn load_reports_missing_and_malformed_named_companions() {
        let main = song_with_pcm_memo();
        assert_eq!(
            Player::new(44_100).load(&main, &EmptyFiles),
            Err(Error::MissingCompanion)
        );
        let malformed = OneFile {
            name: "BANK.PPC",
            bytes: vec![0; 30],
        };
        assert_eq!(
            Player::new(44_100).load(&main, &malformed),
            Err(Error::InvalidCompanion)
        );
    }

    #[test]
    fn playback_status_counts_ticks_and_clears_on_reset() {
        let files = EmptyFiles;
        let mut main = vec![0; 1 + 0x1a + 3];
        main[..3].copy_from_slice(&[0, 0x1a, 0]);
        main[1 + 0x1a..].copy_from_slice(&[0x21, 1, 0x80]);

        let mut player = Player::new(44_100);
        let idle = player.playback_status();
        assert!(idle.ended, "an unloaded player reports no song");
        assert_eq!(idle.ticks, 0);
        assert_eq!(player.load(&main, &files), Ok(()));
        assert_eq!(player.playback_status().ticks, 0);

        for expected in 1..=3 {
            assert!(player.advance_tick().is_ok());
            assert_eq!(player.playback_status().ticks, expected);
        }
        let status = player.playback_status();
        assert_eq!(status.loops_completed, 0);
        assert_eq!(status.loop_limit, None);
        assert!(status.timer_b > 0, "a loaded song publishes its tempo");
        assert_eq!(
            status.tempo_quarter,
            pmd::timing::timer_b_to_quarter(status.timer_b)
        );

        player.reset();
        assert_eq!(player.playback_status().ticks, 0);
    }

    #[test]
    fn load_wires_part_streams_and_reset_restarts_their_cursor() {
        let files = EmptyFiles;
        let mut main = vec![0; 1 + 0x1a + 3];
        main[..3].copy_from_slice(&[0, 0x1a, 0]);
        main[1 + 0x1a..].copy_from_slice(&[0x21, 1, 0x80]);

        let mut player = Player::new(44_100);
        assert_eq!(player.load(&main, &files), Ok(()));
        assert_eq!(
            player.advance_tick(),
            Ok(pmd::stream::SongTick {
                ended: false,
                looped: false,
                writes: 4,
            })
        );
        assert_eq!(
            player.stream_events.len(),
            2,
            "events: {:?}",
            player.stream_events
        );
        player.reset();
        assert_eq!(
            player.advance_tick(),
            Ok(pmd::stream::SongTick {
                ended: false,
                looped: false,
                writes: 4,
            })
        );
        assert_eq!(player.stream_events.len(), 2);
    }

    #[test]
    fn sequencer_trace_attaches_player_startup_timer_tick_and_time() {
        let files = EmptyFiles;
        let mut main = vec![0; 1 + 0x1a + 4];
        main[..3].copy_from_slice(&[0, 0x1a, 0]);
        main[1 + 0x1a..].copy_from_slice(&[0xe0, 0x5a, 0x80, 0x80]);

        let mut player = Player::new(44_100);
        player
            .load(&main, &files)
            .expect("synthetic PMD should load");
        player.advance_tick().expect("command tick should run");

        let mut trace = Vec::new();
        player.take_sequencer_trace(&mut trace);
        assert_eq!(
            trace,
            [
                crate::opna::RegisterWrite {
                    time_us: 15_956,
                    tick: 1,
                    address: 0x22,
                    value: 0x5a,
                },
                crate::opna::RegisterWrite {
                    time_us: 15_956,
                    tick: 1,
                    address: 0x27,
                    value: 0x3f,
                },
            ]
        );
    }

    #[test]
    fn load_rejects_a_nonzero_part_offset_outside_the_work_area() {
        let files = EmptyFiles;
        let mut main = vec![0; 1 + 0x1a];
        main[..3].copy_from_slice(&[0, 0x1a, 0]);
        main[1 + 0x14..1 + 0x16].copy_from_slice(&0xfff0_u16.to_le_bytes());
        let mut player = Player::new(44_100);
        assert_eq!(player.load(&main, &files), Err(Error::InvalidStream));
    }
}
