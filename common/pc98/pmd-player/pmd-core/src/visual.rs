// SPDX-License-Identifier: AGPL-3.0-only

pub const CHANNEL_COUNT: usize = 11;

pub const NO_NOTE: u8 = 0xff;

#[derive(Clone, Copy, Debug, Default, Eq, PartialEq)]
pub enum VisualChannelKind {
    #[default]
    Fm,

    Ssg,

    Adpcm,

    Rhythm,
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub struct ChannelSnapshot {
    pub kind: VisualChannelKind,

    pub active: bool,

    pub releasing: bool,

    pub note: u8,

    pub voice: u8,

    pub level: u8,

    pub pan: u8,

    pub pitch: u32,

    pub lfo: i16,

    pub envelope: i16,

    pub remaining: u16,

    pub operator_mask: u8,

    pub voice_mask: u16,

    pub trigger: u32,
}

impl Default for ChannelSnapshot {
    fn default() -> Self {
        Self {
            kind: VisualChannelKind::Fm,
            active: false,
            releasing: false,
            note: NO_NOTE,
            voice: NO_NOTE,
            level: 0,
            pan: 128,
            pitch: 0,
            lfo: 0,
            envelope: 0,
            remaining: 0,
            operator_mask: 0,
            voice_mask: 0,
            trigger: 0,
        }
    }
}

#[derive(Clone, Copy, Debug, Default, PartialEq)]
pub struct VoicePeaks {
    pub fm: [f32; 6],

    pub ssg: [f32; 3],

    pub rhythm: [f32; 6],

    pub adpcm: f32,

    pub extension: f32,
}

#[derive(Clone, Copy, Debug, Default, Eq, PartialEq)]
pub struct PlaybackStatus {
    pub ticks: u64,

    pub loops_completed: u32,

    pub loop_limit: Option<u32>,

    pub timer_b: u8,

    pub tempo_quarter: u8,

    pub ended: bool,
}

pub(crate) fn empty_channel_snapshots() -> [ChannelSnapshot; CHANNEL_COUNT] {
    let mut snapshots = [ChannelSnapshot::default(); CHANNEL_COUNT];
    for (index, snapshot) in snapshots.iter_mut().enumerate() {
        snapshot.kind = match index {
            0..=5 => VisualChannelKind::Fm,
            6..=8 => VisualChannelKind::Ssg,
            9 => VisualChannelKind::Adpcm,
            _ => VisualChannelKind::Rhythm,
        };
    }
    snapshots
}
