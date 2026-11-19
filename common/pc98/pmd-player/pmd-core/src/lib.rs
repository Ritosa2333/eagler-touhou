// SPDX-License-Identifier: AGPL-3.0-only

#![forbid(unsafe_code)]
#![no_std]

#[allow(unused_imports)]
#[macro_use]
extern crate alloc;

pub mod opna;
mod player;
mod pmd;
mod visual;

pub use player::{Error, FileProvider, Metadata, Player, TickStatus};
pub use visual::{
    ChannelSnapshot, PlaybackStatus, VisualChannelKind, VoicePeaks, CHANNEL_COUNT, NO_NOTE,
};
