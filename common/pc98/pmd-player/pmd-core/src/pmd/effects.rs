// SPDX-License-Identifier: AGPL-3.0-only

use super::channel::{ChannelKind, ChannelState, SequencerGlobals};

const FM_FNUM: [i32; 15] = [
    0x026a, 0x028f, 0x02b6, 0x02df, 0x030b, 0x0339, 0x036a, 0x039e, 0x03d5, 0x0410, 0x044e, 0x048f,
    0x0ee8, 0x0e12, 0x0d48,
];

const PSG_TUNE: [i32; 15] = [
    0x0ee8, 0x0e12, 0x0d48, 0x0c89, 0x0bd5, 0x0b2b, 0x0a8a, 0x09f3, 0x0964, 0x08dd, 0x085e, 0x07e6,
    0x8080, 0x8080, 0xe0a0,
];

const PCM_TUNE: [i32; 15] = [
    0x6264, 0x6840, 0x6e74, 0x7506, 0x7bfc, 0x835e, 0x8b2e, 0x9376, 0x9c3c, 0xa588, 0xaf62, 0xb9d0,
    0x3e80, 0x3c73, 0x7400,
];

const P86_TUNE: [u32; 101] = [
    0xff002ab7, 0xff002d41, 0xff002ff2, 0xff0032cb, 0xff0035d1, 0xff003904, 0xff003c68, 0xff003fff,
    0xff0043ce, 0xff0047d6, 0xff004c1b, 0xff0050a2, 0xff00556e, 0xff005a82, 0xff005fe4, 0xff006597,
    0xff006ba2, 0xff007209, 0xff0078d0, 0xff007fff, 0xff00879c, 0xff008fac, 0xff009837, 0xff00a145,
    0xff00aadc, 0xff00b504, 0xff00bfc8, 0xff00cb2f, 0xff00d744, 0xff00e412, 0xff00f1a1, 0xff010000,
    0xff20cb6b, 0xff20d783, 0xff20e454, 0xff20f1e7, 0xff40aadc, 0xff40b504, 0xff40bfc8, 0xff40cb2f,
    0xff40d744, 0xff40e412, 0xff40f1a1, 0xff410000, 0xff60cb6b, 0xff60d783, 0xff60e454, 0xff60f1e7,
    0xff80aadc, 0xff80b504, 0xff80bfc8, 0xff80cb2f, 0xff80d744, 0xff80e412, 0xff80f1a1, 0xff810000,
    0xffa0cb6b, 0xffa0d783, 0xffa0e454, 0xffa0f1e7, 0xffc0aadc, 0xffc0b504, 0xffc0bfc8, 0xffc0cb2f,
    0xffc0d744, 0xffc0e412, 0xffc0f1a1, 0xffc10000, 0xffe0cb6b, 0xffe0d783, 0xffe0e454, 0xffe0f1e7,
    0xffe1004a, 0xffe10f87, 0xffe11fac, 0xffe130c7, 0xffe142e7, 0xffe1561c, 0xffe16a72, 0xffe18000,
    0xffe196d6, 0xffe1af06, 0xffe1c8a8, 0xffe1e3cf, 0xffe20094, 0xffe21f0e, 0xffe23f59, 0xffe2618f,
    0xffe285ce, 0xffe2ac38, 0xffe2d4e5, 0xffe30000, 0xffe32dac, 0xffe35e0d, 0xffe39150, 0xffe3c79e,
    0xff80463e, 0xff407400, 0xff11800c, 0xff387606, 0xff0d76a2,
];

const PPZ_TUNE: [i32; 15] = [
    0x8000, 0x87a6, 0x8fb3, 0x9838, 0xa146, 0xaade, 0xb4ff, 0xbfcc, 0xcb34, 0xd747, 0xe418, 0xf1a5,
    0x3e80, 0x42de, 0x7400,
];

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) struct VoiceFrame {
    pub(crate) pitch: i32,

    pub(crate) fm_base_pitch: i32,

    pub(crate) pitch_enabled: bool,

    pub(crate) p86_pitch: u32,

    pub(crate) pitch_lfo: i32,

    pub(crate) portamento: i32,

    pub(crate) detune: i32,

    pub(crate) volume: i32,

    pub(crate) base_volume: i32,

    pub(crate) volume_lfo: i32,

    pub(crate) lfo_data: i32,
    pub(crate) secondary_lfo_data: i32,

    pub(crate) envelope: i32,

    pub(crate) gate_length: i32,

    pub(crate) remaining: i32,

    pub(crate) psg_pattern: i32,

    pub(crate) slot_mask: i32,

    pub(crate) volume_mask: i32,
    pub(crate) secondary_volume_mask: i32,

    pub(crate) lfoswi: i32,

    pub(crate) sdelay_mask: i32,
    pub(crate) sdelay_counter: i32,
    pub(crate) carrier: i32,

    pub(crate) slot_tl: [i32; 4],

    pub(crate) slot_detune: [i32; 4],

    pub(crate) algorithm_feedback: i32,

    pub(crate) fmpan: i32,

    pub(crate) voice_number: i32,

    pub(crate) fm_volume_down: i32,

    pub(crate) ssg_volume_down: i32,

    pub(crate) force_volume_write: bool,

    pub(crate) force_pitch_write: bool,

    pub(crate) force_key_on: bool,

    pub(crate) volume_write_allowed: bool,

    pub(crate) envelope_mode: i32,
}

#[derive(Clone, Copy, Debug, Default, Eq, PartialEq)]
pub(crate) struct FrameUpdate {
    pub(crate) force_volume_write: bool,
    pub(crate) force_pitch_write: bool,
    pub(crate) force_key_on: bool,
}

pub(crate) fn start_note(channel: &mut ChannelState, globals: &mut SequencerGlobals, raw_note: u8) {
    start_note_with_timer_a(channel, globals, raw_note, 1);
}

pub(crate) fn start_note_with_timer_a(
    channel: &mut ChannelState,
    globals: &mut SequencerGlobals,
    raw_note: u8,
    timer_a_delta: u32,
) {
    let mut note = i32::from(raw_note);
    if note & 0x0f == 0x0c {
        note = channel.onkai_def;
    }
    channel.onkai_def = note;
    channel.pcm86_volume = globals.pcm86_volume;

    if note & 0x0f == 0x0f {
        channel.onkai = 255;
        if channel.lfoswi & 0x11 == 0 {
            channel.fnum = 0;
            channel.p86_fnum = 0;
        }
        if !channel.kind.is_fm() {
            advance_envelope(channel, globals, envelope_ticks(channel, timer_a_delta));
        }
        lfo_exit(channel, globals, timer_a_delta);
        return;
    }

    channel.porta_num = 0;
    if channel.kind.is_fm() {
        if globals.tie_flag & 1 == 0 {
            lfo_entry(channel, globals, timer_a_delta);
        } else {
            lfo_exit(channel, globals, timer_a_delta);
        }
    } else if globals.tie_flag & 1 == 0 {
        let extended_envelope = channel.env_flag == -1;
        initialize_envelope(channel);
        if extended_envelope {
            advance_envelope(channel, globals, 1);
        }
        lfo_entry(channel, globals, timer_a_delta);
    } else {
        advance_envelope(channel, globals, envelope_ticks(channel, timer_a_delta));
        lfo_exit(channel, globals, timer_a_delta);
    }

    let shifted = shifted_note(note, channel.shift + channel.shift_def);
    match channel.kind {
        ChannelKind::Adpcm => {
            let (canonical, pitch) = adpcm_pitch(shifted);
            channel.onkai = canonical;
            channel.fnum = pitch;
        }
        ChannelKind::P86 => {
            let (canonical, pitch) = p86_pitch(shifted, channel.pcm86_volume != 0);
            channel.onkai = canonical;
            channel.p86_fnum = pitch;
            channel.fnum = 0;
        }
        _ => {
            channel.onkai = shifted;
            channel.fnum = base_pitch(channel.kind, shifted);
        }
    }
}

pub(crate) fn start_portamento(
    channel: &mut ChannelState,
    globals: &mut SequencerGlobals,
    from: u8,
    to: u8,
    length: i32,
) -> Result<(), ()> {
    start_portamento_with_timer_a(channel, globals, from, to, length, 1)
}

pub(crate) fn start_portamento_with_timer_a(
    channel: &mut ChannelState,
    globals: &mut SequencerGlobals,
    from: u8,
    to: u8,
    length: i32,
    timer_a_delta: u32,
) -> Result<(), ()> {
    if length <= 0 {
        return Err(());
    }

    start_note_with_timer_a(channel, globals, from, timer_a_delta);
    let from_pitch = channel.fnum;
    let from_onkai = channel.onkai;
    let from_p86_pitch = channel.p86_fnum;

    set_pitch_only(channel, to);
    let target_pitch = channel.fnum;

    channel.onkai = from_onkai;
    channel.fnum = from_pitch;
    channel.p86_fnum = from_p86_pitch;
    channel.porta_from = i32::from(from);
    channel.porta_to = i32::from(to);
    channel.porta_length = length;
    let difference = if channel.kind == ChannelKind::Fm {
        let block_delta = ((target_pitch / 256) & 0x38) - ((from_pitch / 256) & 0x38);
        let octave_delta = if block_delta == 0 {
            0
        } else {
            (block_delta / 8) * 0x26a
        };
        octave_delta + (target_pitch & 0x7ff) - (from_pitch & 0x7ff)
    } else {
        target_pitch - from_pitch
    };
    channel.porta_num = 0;
    channel.porta_num2 = if channel.kind == ChannelKind::Ppz8 {
        difference / 16 / length
    } else {
        difference / length
    };
    channel.porta_num3 = if channel.kind == ChannelKind::Ppz8 {
        difference / 16 % length
    } else {
        difference % length
    };
    channel.porta_delta = difference;
    channel.porta_enabled = true;
    channel.lfoswi |= 8;
    Ok(())
}

fn set_pitch_only(channel: &mut ChannelState, raw_note: u8) {
    let note = i32::from(raw_note);
    if note & 0x0f == 0x0f {
        channel.onkai = 255;
        if channel.lfoswi & 0x11 == 0 {
            channel.fnum = 0;
            channel.p86_fnum = 0;
        }
        return;
    }

    let shifted = shifted_note(note, channel.shift + channel.shift_def);
    match channel.kind {
        ChannelKind::Adpcm => {
            let (canonical, pitch) = adpcm_pitch(shifted);
            channel.onkai = canonical;
            channel.fnum = pitch;
        }
        ChannelKind::P86 => {
            let (canonical, pitch) = p86_pitch(shifted, channel.pcm86_volume != 0);
            channel.onkai = canonical;
            channel.p86_fnum = pitch;
            channel.fnum = 0;
        }
        _ => {
            channel.onkai = shifted;
            channel.fnum = base_pitch(channel.kind, shifted);
        }
    }
}

pub(crate) fn calculate_gate(
    channel: &mut ChannelState,
    globals: &mut SequencerGlobals,
    length: i32,
) {
    let mut gate = channel.qdata;
    if channel.qdatb != 0 {
        gate += (length * channel.qdatb) >> 8;
    }
    if channel.qdat3 != 0 {
        let random = next_random(globals, (channel.qdat3 & 0x7f) + 1);
        if channel.qdat3 & 0x80 == 0 {
            gate += random;
        } else {
            gate -= random;
            if gate < 0 {
                gate = 0;
            }
        }
    }
    if channel.qdat2 != 0 {
        let minimum = length - channel.qdat2;
        channel.qdat = if minimum < 0 { 0 } else { gate.min(minimum) };
    } else {
        channel.qdat = gate;
    }
}

pub(crate) fn advance_voice(
    channel: &mut ChannelState,
    globals: &mut SequencerGlobals,
) -> FrameUpdate {
    advance_voice_with_timer_a(channel, globals, 1)
}

pub(crate) fn advance_voice_with_timer_a(
    channel: &mut ChannelState,
    globals: &mut SequencerGlobals,
    timer_a_delta: u32,
) -> FrameUpdate {
    let mut update = FrameUpdate::default();

    if channel.kind.is_fm() && channel.sdelay_counter > 0 {
        channel.sdelay_counter -= 1;
        if channel.sdelay_counter == 0 && channel.keyoff_flag & 1 == 0 {
            update.force_key_on = true;
        }
    }
    if channel.lfoswi != 0 {
        let lfo_switch = advance_lfo(channel, globals, timer_a_delta);
        update.force_pitch_write = lfo_switch & 0x19 != 0;
        update.force_volume_write = lfo_switch & 0x22 != 0;
    }
    if !channel.kind.is_fm() {
        update.force_volume_write |=
            advance_envelope(channel, globals, envelope_ticks(channel, timer_a_delta));
    }
    update
}

pub(crate) fn key_off(channel: &mut ChannelState) {
    if channel.kind.is_fm() {
        channel.keyoff_flag = -1;
    } else if channel.env_flag != -1 {
        channel.env_flag = 2;
    } else {
        channel.env_count = 4;
    }
}

pub(crate) fn frame(channel: &ChannelState, remaining: i32) -> VoiceFrame {
    frame_with_update(channel, remaining, FrameUpdate::default())
}

pub(crate) fn frame_with_update(
    channel: &ChannelState,
    remaining: i32,
    update: FrameUpdate,
) -> VoiceFrame {
    let pitch_lfo = tone_lfo(channel);
    let volume_lfo = volume_lfo(channel);
    let pitch = effective_pitch(channel, pitch_lfo);
    let volume = effective_volume(channel, volume_lfo);
    VoiceFrame {
        pitch,
        fm_base_pitch: if channel.kind == ChannelKind::Fm {
            fm_effective_pitch(channel, 0)
        } else {
            0
        },
        pitch_enabled: channel.fnum != 0,
        p86_pitch: effective_p86_pitch(channel),
        pitch_lfo,
        portamento: channel.porta_num,
        detune: channel.detune,
        volume,
        base_volume: if channel.vol_push != 0 {
            channel.vol_push - 1
        } else {
            channel.volume
        },
        volume_lfo,
        lfo_data: channel.lfo_data,
        secondary_lfo_data: channel.secondary_lfo_data,
        envelope: channel.env_volume,
        gate_length: channel.qdat,
        remaining,
        psg_pattern: channel.psg_pattern,
        slot_mask: channel.slot_mask,
        volume_mask: channel.volume_mask,
        secondary_volume_mask: channel.secondary_volume_mask,
        lfoswi: channel.lfoswi,
        sdelay_mask: channel.sdelay_mask,
        sdelay_counter: channel.sdelay_counter,
        carrier: channel.carrier,
        slot_tl: channel.slot_tl,
        slot_detune: channel.slot_detune,
        algorithm_feedback: channel.algorithm_feedback,
        fmpan: channel.fmpan,
        voice_number: channel.voice_number,
        fm_volume_down: 0,
        ssg_volume_down: 0,
        force_volume_write: update.force_volume_write,
        force_pitch_write: update.force_pitch_write,
        force_key_on: update.force_key_on,
        volume_write_allowed: channel.env_flag != 3
            && !(channel.env_flag == -1 && channel.env_count == 0),
        envelope_mode: channel.env_flag,
    }
}

fn base_pitch(kind: ChannelKind, note: i32) -> i32 {
    let key = (note & 0x0f) as usize;
    match kind {
        ChannelKind::Fm => {
            let block = ((note >> 1) & 0x38) << 8;
            FM_FNUM[key] | block
        }
        ChannelKind::Ssg => {
            let mut value = PSG_TUNE[key];
            let octave = (note >> 4) & 0x0f;
            if octave > 0 {
                value >>= octave - 1;
                let carry = value & 1;
                value = (value >> 1) + carry;
            }
            value
        }
        ChannelKind::Adpcm => adpcm_pitch(note).1,
        ChannelKind::Rhythm | ChannelKind::P86 => 0,
        ChannelKind::Ppz8 => ppz_pitch(note),
    }
}

fn adpcm_pitch(note: i32) -> (i32, i32) {
    let key = (note & 0x0f) as usize;
    let mut octave = (note >> 4) & 0x0f;
    let mut pitch = PCM_TUNE[key];
    if octave > 5 {
        octave = 5;
        if pitch < 0x8000 {
            pitch *= 2;
            octave = 6;
        }
        return ((octave << 4) | (note & 0x0f), pitch);
    }
    pitch >>= 5 - octave;
    (note, pitch)
}

fn p86_pitch(note: i32, pcm86_volume: bool) -> (i32, u32) {
    let key = note & 0x0f;
    let mut canonical = note;
    if pcm86_volume && note >= 0x65 {
        canonical = if key < 5 { 0x60 | key } else { 0x50 | key };
    }
    let index = (((canonical & 0xf0) >> 4) * 12 + (canonical & 0x0f)) as usize;
    (canonical, P86_TUNE.get(index).copied().unwrap_or(0))
}

fn ppz_pitch(note: i32) -> i32 {
    let key = (note & 0x0f) as usize;
    let octave = ((note >> 4) & 0x0f) - 4;
    let value = i64::from(PPZ_TUNE[key]);
    if octave < 0 {
        (value >> -octave).min(i64::from(i32::MAX)) as i32
    } else {
        (value << octave).min(i64::from(i32::MAX)) as i32
    }
}

fn shifted_note(note: i32, shift: i32) -> i32 {
    if note & 0x0f == 0x0f || shift == 0 {
        return note;
    }
    let mut key = note & 0x0f;
    let mut octave = (note >> 4) & 0x0f;
    if shift < 0 {
        key += shift;
        while key < 0 {
            octave -= 1;
            key += 12;
        }
    } else {
        key += shift;
        while key >= 12 {
            octave += 1;
            key -= 12;
        }
    }
    octave = octave.clamp(0, 7);
    (octave << 4) | key
}

fn lfo_entry(channel: &mut ChannelState, globals: &mut SequencerGlobals, timer_a_delta: u32) {
    channel.sdelay_counter = channel.sdelay;
    if channel.lfoswi & 3 != 0 {
        if channel.lfoswi & 4 == 0 {
            channel.reset_lfo_main();
        }
        lfo_once(channel, globals, timer_a_delta);
    }
    if channel.lfoswi & 0x30 != 0 {
        let synchronized = channel.lfoswi & 0x40 != 0;
        channel.swap_lfo();
        if !synchronized {
            channel.reset_lfo_main();
        }
        lfo_once(channel, globals, timer_a_delta);
        channel.swap_lfo();
    }
}

fn lfo_exit(channel: &mut ChannelState, globals: &mut SequencerGlobals, timer_a_delta: u32) {
    if channel.lfoswi & 3 != 0 {
        lfo_once(channel, globals, timer_a_delta);
    }
    if channel.lfoswi & 0x30 != 0 {
        channel.swap_lfo();
        lfo_once(channel, globals, timer_a_delta);
        channel.swap_lfo();
    }
}

fn advance_lfo(
    channel: &mut ChannelState,
    globals: &mut SequencerGlobals,
    timer_a_delta: u32,
) -> i32 {
    let mut lfo_switch = channel.lfoswi & 8;
    if channel.lfoswi & 3 != 0 && lfo_once(channel, globals, timer_a_delta) {
        lfo_switch |= channel.lfoswi & 3;
    }
    if channel.lfoswi & 0x30 != 0 {
        channel.swap_lfo();
        if lfo_once(channel, globals, timer_a_delta) {
            lfo_switch |= channel.lfoswi & 0x30;
        }
        channel.swap_lfo();
    }
    if channel.lfoswi & 8 != 0 {
        channel.porta_num += channel.porta_num2;
        if channel.porta_num3 > 0 {
            channel.porta_num3 -= 1;
            channel.porta_num += 1;
        } else if channel.porta_num3 < 0 {
            channel.porta_num3 += 1;
            channel.porta_num -= 1;
        }
    }
    lfo_switch
}

fn lfo_once(
    channel: &mut ChannelState,
    globals: &mut SequencerGlobals,
    timer_a_delta: u32,
) -> bool {
    if channel.delay != 0 {
        channel.delay -= 1;
        return false;
    }
    let before = channel.lfo_data;
    let count = if channel.extendmode & 2 != 0 {
        timer_a_delta
    } else {
        1
    };
    for _ in 0..count {
        lfo_main(channel, globals);
    }
    before != channel.lfo_data
}

fn lfo_main(channel: &mut ChannelState, globals: &mut SequencerGlobals) {
    if channel.speed != 1 {
        if channel.speed != 255 {
            channel.speed -= 1;
        }
        return;
    }
    channel.speed = channel.secondary_speed;

    match channel.lfo_wave {
        0 | 4 | 5 => {
            let step = if channel.lfo_wave == 5 {
                channel.step.abs() * channel.step
            } else {
                channel.step
            };
            channel.lfo_data += step;
            if channel.lfo_data == 0 {
                depth_increment(channel);
            }
            let mut time = channel.time;
            if time != 255 {
                time -= 1;
                if time == 0 {
                    time = channel.secondary_time;
                    if channel.lfo_wave != 4 {
                        time += time;
                    }
                    channel.time = time;
                    channel.step = -channel.step;
                    return;
                }
            }
            channel.time = time;
        }
        2 => {
            channel.lfo_data = channel.step * channel.time;
            depth_increment(channel);
            channel.step = -channel.step;
        }
        6 => {
            if channel.time != 0 {
                if channel.time != 255 {
                    channel.time -= 1;
                }
                channel.lfo_data += channel.step;
            }
        }
        1 => {
            channel.lfo_data += channel.step;
            let mut time = channel.time;
            if time != -1 {
                time -= 1;
                if time == 0 {
                    channel.lfo_data = -channel.lfo_data;
                    depth_increment(channel);
                    time = channel.secondary_time * 2;
                }
            }
            channel.time = time;
        }
        _ => {
            let max = channel.step.abs() * channel.time;
            channel.lfo_data = max - next_random(globals, max * 2);
            depth_increment(channel);
        }
    }
}

fn depth_increment(channel: &mut ChannelState) {
    channel.mdspd -= 1;
    if channel.mdspd != 0 {
        return;
    }
    channel.mdspd = channel.mdspd2;
    if channel.mdc == 0 {
        return;
    }
    if channel.mdc <= 127 {
        channel.mdc -= 1;
    }
    if channel.step < 0 {
        let value = channel.mdepth - channel.step;
        if value < 128 {
            channel.step = -value;
        } else if channel.mdepth < 0 {
            channel.step = 0;
        } else {
            channel.step = -127;
        }
    } else {
        let value = channel.step + channel.mdepth;
        if value < 128 {
            channel.step = value;
        } else if channel.mdepth < 0 {
            channel.step = 0;
        } else {
            channel.step = 127;
        }
    }
}

fn initialize_envelope(channel: &mut ChannelState) {
    if channel.env_flag != -1 {
        channel.env_flag = 0;
        channel.env_volume = 0;
        channel.env_ar = channel.env_arc;
        if channel.env_ar == 0 {
            channel.env_flag = 1;
            channel.env_volume = channel.env_dr;
        }
        channel.env_sr = channel.env_src;
        channel.env_rr = channel.env_rrc;
    } else {
        channel.env_arc = channel.env_ar - 16;
        channel.env_drc = if channel.env_dr < 16 {
            (channel.env_dr - 16) * 2
        } else {
            channel.env_dr - 16
        };
        channel.env_src = if channel.env_sr < 16 {
            (channel.env_sr - 16) * 2
        } else {
            channel.env_sr - 16
        };
        channel.env_rrc = channel.env_rr * 2 - 16;
        channel.env_volume = channel.env_al;
        channel.env_count = 1;
    }
}

fn advance_envelope(
    channel: &mut ChannelState,
    _globals: &mut SequencerGlobals,
    ticks: i32,
) -> bool {
    let mut changed = false;
    let count = ticks.max(0);
    for _ in 0..count {
        if channel.env_flag == -1 {
            changed |= extended_envelope_step(channel);
        } else {
            changed |= normal_envelope_step(channel);
        }
    }
    changed
}

fn envelope_ticks(channel: &ChannelState, timer_a_delta: u32) -> i32 {
    if channel.extendmode & 4 != 0 {
        timer_a_delta.min(i32::MAX as u32) as i32
    } else {
        1
    }
}

fn normal_envelope_step(channel: &mut ChannelState) -> bool {
    let before = channel.env_volume;
    if channel.env_flag == 0 {
        channel.env_ar -= 1;
        if channel.env_ar != 0 {
            return false;
        }
        channel.env_flag = 1;
        channel.env_volume = channel.env_dr;

        return before != channel.env_volume;
    }
    if channel.env_flag != 2 {
        if channel.env_sr == 0 {
            return false;
        }
        channel.env_sr -= 1;
        if channel.env_sr != 0 {
            return false;
        }
        channel.env_sr = channel.env_src;
        channel.env_volume -= 1;

        if channel.env_volume >= -15 || channel.env_volume < 15 {
            return before != channel.env_volume;
        }
        channel.env_volume = -15;
        return before != channel.env_volume;
    }
    if channel.env_rr == 0 {
        channel.env_volume = -15;
        return before != channel.env_volume;
    }
    channel.env_rr -= 1;
    if channel.env_rr != 0 {
        return false;
    }
    channel.env_rr = channel.env_rrc;
    channel.env_volume -= 1;
    if channel.env_volume >= -15 && channel.env_volume < 15 {
        return before != channel.env_volume;
    }
    channel.env_volume = -15;
    before != channel.env_volume
}

fn extended_envelope_step(channel: &mut ChannelState) -> bool {
    let before = channel.env_volume;
    if channel.env_count == 0 {
        return false;
    }
    match channel.env_count {
        1 => {
            if channel.env_arc > 0 {
                channel.env_volume += channel.env_arc;
                if channel.env_volume < 15 {
                    channel.env_arc = channel.env_ar - 16;
                    return before != channel.env_volume;
                }
                channel.env_volume = 15;
                channel.env_count += 1;
                if channel.env_sl != 15 {
                    return before != channel.env_volume;
                }
                channel.env_count += 1;
            } else if channel.env_ar != 0 {
                channel.env_arc += 1;
            }
        }
        2 => {
            if channel.env_drc > 0 {
                channel.env_volume -= channel.env_drc;
                if channel.env_volume < 0 || channel.env_volume < channel.env_sl {
                    channel.env_volume = channel.env_sl;
                    channel.env_count += 1;
                    return before != channel.env_volume;
                }
                channel.env_drc = if channel.env_dr < 16 {
                    (channel.env_dr - 16) * 2
                } else {
                    channel.env_dr - 16
                };
            } else if channel.env_dr != 0 {
                channel.env_drc += 1;
            }
        }
        3 => {
            if channel.env_src > 0 {
                channel.env_volume -= channel.env_src;
                if channel.env_volume < 0 {
                    channel.env_volume = 0;
                }
                channel.env_src = if channel.env_sr < 16 {
                    (channel.env_sr - 16) * 2
                } else {
                    channel.env_sr - 16
                };
            } else if channel.env_sr != 0 {
                channel.env_src += 1;
            }
        }
        _ => {
            if channel.env_rrc > 0 {
                channel.env_volume -= channel.env_rrc;
                if channel.env_volume < 0 {
                    channel.env_volume = 0;
                }
                channel.env_rrc = channel.env_rr * 2 - 16;
            } else if channel.env_rr != 0 {
                channel.env_rrc += 1;
            }
        }
    }
    before != channel.env_volume
}

fn tone_lfo(channel: &ChannelState) -> i32 {
    let mut value = 0;
    if channel.lfoswi & 1 != 0 {
        value += channel.lfo_data;
    }
    if channel.lfoswi & 0x10 != 0 {
        value += channel.secondary_lfo_data;
    }
    value
}

fn volume_lfo(channel: &ChannelState) -> i32 {
    let mut value = 0;
    if channel.lfoswi & 2 != 0 {
        value += channel.lfo_data;
    }
    if channel.lfoswi & 0x20 != 0 {
        value += channel.secondary_lfo_data;
    }
    value
}

fn effective_pitch(channel: &ChannelState, lfo: i32) -> i32 {
    match channel.kind {
        ChannelKind::Fm => fm_effective_pitch(channel, lfo),
        ChannelKind::Ssg => {
            let base = channel.fnum + channel.porta_num;
            if channel.extendmode & 1 == 0 {
                base - channel.detune - lfo
            } else {
                let detune = signed_product_shift(base, channel.detune);
                let lfo = signed_product_shift(base, lfo);
                base - detune - lfo
            }
        }
        ChannelKind::Adpcm => channel.fnum + channel.porta_num + channel.detune + lfo * 4,
        ChannelKind::P86 => 0,
        ChannelKind::Ppz8 => {
            let scaled = (i64::from(channel.fnum) / 256) * i64::from(lfo + channel.detune);
            (i64::from(channel.fnum) + i64::from(channel.porta_num) * 16 + scaled)
                .clamp(0, i64::from(i32::MAX)) as i32
        }
        ChannelKind::Rhythm => 0,
    }
}

fn fm_effective_pitch(channel: &ChannelState, lfo: i32) -> i32 {
    let mut block = channel.fnum & 0x3800;
    let mut fnum = (channel.fnum & 0x7ff) + channel.porta_num + channel.detune + lfo;

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

fn effective_p86_pitch(channel: &ChannelState) -> u32 {
    if channel.kind != ChannelKind::P86 || channel.p86_fnum == 0 {
        return 0;
    }
    let block = channel.p86_fnum & 0x0e00_0000;
    let mut frequency = channel.p86_fnum & 0x001f_ffff;
    if channel.pcm86_volume == 0 && channel.detune != 0 {
        frequency =
            ((((frequency >> 5) as i64) + i64::from(channel.detune)).clamp(1, 65_535) as u32) << 5;
    }
    block | frequency
}

fn signed_product_shift(value: i32, factor: i32) -> i32 {
    let product = (i64::from(value) * i64::from(factor)) >> 12;
    if product >= 0 {
        (product + 1) as i32
    } else {
        (product - 1) as i32
    }
}

fn effective_volume(channel: &ChannelState, lfo: i32) -> i32 {
    let base = if channel.vol_push != 0 {
        channel.vol_push - 1
    } else {
        channel.volume
    };
    if channel.kind.is_fm() {
        return base.clamp(0, 127);
    }
    if channel.env_flag == -1 {
        if channel.env_volume == 0 {
            return 0;
        }
        let scaled = ((((base * (channel.env_volume + 1)) >> 3) + 1) >> 1).max(0);
        return (scaled + lfo).clamp(0, channel.volume_limit());
    }
    let envelope = if channel.env_flag == 3 {
        0
    } else {
        channel.env_volume
    };
    (base + envelope + lfo).clamp(0, channel.volume_limit())
}

fn next_random(globals: &mut SequencerGlobals, maximum: i32) -> i32 {
    globals.random_seed = (259 * globals.random_seed + 3) & 0x7fff;
    if maximum <= 0 {
        return 0;
    }
    globals.random_seed * maximum / 32767
}

#[cfg(test)]
mod tests {
    use super::{calculate_gate, fm_effective_pitch, frame, shifted_note, start_note};
    use crate::pmd::channel::{ChannelKind, ChannelState, SequencerGlobals};

    #[test]
    fn fm_and_ssg_note_tables_and_shift_match_pmd_integer_rules() {
        let mut fm = ChannelState::new(ChannelKind::Fm);
        let mut globals = SequencerGlobals::new();
        start_note(&mut fm, &mut globals, 0x21);
        assert_eq!(fm.fnum, 0x128f);
        assert_eq!(shifted_note(0x01, -2), 0x0b);
        assert_eq!(shifted_note(0x76, 2), 0x78);

        let mut ssg = ChannelState::new(ChannelKind::Ssg);
        start_note(&mut ssg, &mut globals, 0x21);
        assert_eq!(ssg.fnum, 0x0385);
    }

    #[test]
    fn pcm_note_tables_preserve_pmd_octave_clamping_and_packed_p86_values() {
        let mut globals = SequencerGlobals::new();

        let mut adpcm = ChannelState::new(ChannelKind::Adpcm);
        start_note(&mut adpcm, &mut globals, 0x21);
        assert_eq!(adpcm.onkai, 0x21);
        assert_eq!(adpcm.fnum, 0x0d08);
        start_note(&mut adpcm, &mut globals, 0x61);
        assert_eq!(adpcm.onkai, 0x61);
        assert_eq!(adpcm.fnum, 0xd080);

        let mut ppz = ChannelState::new(ChannelKind::Ppz8);
        start_note(&mut ppz, &mut globals, 0x41);
        assert_eq!(ppz.fnum, 0x87a6);

        let mut p86 = ChannelState::new(ChannelKind::P86);
        start_note(&mut p86, &mut globals, 0x21);
        assert_eq!(p86.onkai, 0x21);
        assert_eq!(p86.p86_fnum, 0xff00b504);
        p86.detune = 1;
        let packed = frame(&p86, 1).p86_pitch;
        assert_eq!(packed & 0x001f_ffff, 0x00b520);
    }

    #[test]
    fn q_gate_uses_q_q_percent_and_minimum_in_the_cpp_order() {
        let mut channel = ChannelState::new(ChannelKind::Fm);
        let mut globals = SequencerGlobals::new();
        channel.qdata = 3;
        channel.qdatb = 128;
        channel.qdat2 = 4;
        calculate_gate(&mut channel, &mut globals, 10);
        assert_eq!(channel.qdat, 6);

        channel.qdat3 = 0x80 | 1;
        calculate_gate(&mut channel, &mut globals, 2);
        assert_eq!(channel.qdat, 0);
    }

    #[test]
    fn fm_pitch_normalizes_the_cpp_octave_boundary() {
        let mut channel = ChannelState::new(ChannelKind::Fm);
        channel.fnum = 0x2adf;
        channel.porta_num = 0x250 - (channel.fnum & 0x7ff);
        assert_eq!(fm_effective_pitch(&channel, 0), 0x24ba);
    }

    #[test]
    fn triangular_lfo_and_portamento_frames_are_integer_steps() {
        let mut channel = ChannelState::new(ChannelKind::Fm);
        let mut globals = SequencerGlobals::new();
        channel.fnum = 100;
        channel.lfoswi = 1;
        channel.secondary_speed = 1;
        channel.speed = 1;
        channel.step = 2;
        channel.secondary_step = 2;
        channel.time = 2;
        channel.secondary_time = 2;
        channel.mdspd = 1;
        channel.mdspd2 = 1;
        channel.mdc = 0;
        channel.mdc2 = 0;
        start_note(&mut channel, &mut globals, 0x21);
        let first = frame(&channel, 3);
        assert_eq!(first.pitch_lfo, 0);
        super::advance_voice(&mut channel, &mut globals);
        assert_eq!(frame(&channel, 3).pitch_lfo, 2);
        super::advance_voice(&mut channel, &mut globals);
        assert_eq!(first.remaining, 3);
        assert_eq!(frame(&channel, 3).pitch_lfo, 4);
        assert_eq!(frame(&channel, 3).pitch, channel.fnum + 4);
        assert_eq!(first.remaining, 3);
        channel.porta_num2 = 3;
        channel.porta_num3 = 1;
        channel.lfoswi = 8;
        super::advance_voice(&mut channel, &mut globals);
        assert_eq!(channel.porta_num, 4);
        let second = frame(&channel, 2);
        assert_eq!(second.portamento, 4);
    }

    #[test]
    fn normal_envelope_attack_decay_and_release_follow_state_numbers() {
        let mut channel = ChannelState::new(ChannelKind::Ssg);
        channel.env_arc = 2;
        channel.env_dr = 4;
        channel.env_src = 1;
        channel.env_rrc = 1;
        channel.env_ar = 2;
        channel.env_sr = 1;
        channel.env_rr = 1;
        let mut globals = SequencerGlobals::new();
        start_note(&mut channel, &mut globals, 0x21);
        super::advance_envelope(&mut channel, &mut globals, 1);
        assert_eq!(channel.env_flag, 0);
        super::advance_envelope(&mut channel, &mut globals, 1);
        assert_eq!(channel.env_flag, 1);
        assert_eq!(channel.env_volume, 4);
        super::key_off(&mut channel);
        super::advance_envelope(&mut channel, &mut globals, 1);
        assert_eq!(channel.env_flag, 2);
        assert_eq!(channel.env_volume, 3);
    }

    #[test]
    fn envelope_change_forces_the_cpp_volume_write() {
        let mut channel = ChannelState::new(ChannelKind::Ssg);
        let mut globals = SequencerGlobals::new();
        channel.env_flag = 2;
        channel.env_volume = -7;
        channel.env_rr = 1;
        channel.env_rrc = 1;

        let update = super::advance_voice(&mut channel, &mut globals);

        assert!(update.force_volume_write);
        assert_eq!(channel.env_volume, -8);
    }
}
