// SPDX-License-Identifier: AGPL-3.0-only

use alloc::borrow::ToOwned;
use alloc::string::String;
use alloc::vec::Vec;

use crate::FileProvider;

use super::CompanionNames;

const ADPCM_DATA_START_BLOCK: u16 = 0x26;
const ADPCM_BLOCK_BYTES: usize = 32;
const PVI_ADPCM_HEADER_SIZE: usize = 0x210;
const PPC_HEADER_SIZE: usize = 30 + 4 * 256 + 2;
const P86_HEADER_SIZE: usize = 12 + 1 + 3 + 6 * 256;
const PPS_HEADER_SIZE: usize = 6 * 14;
const PPZ_PZI_HEADER_SIZE: usize = 2336;
const PPZ_PVI_HEADER_SIZE: usize = 528;
const MAX_COMPANION_DATA: usize = 64 * 1024 * 1024;
const PPC_HEADER: &[u8] = b"ADPCM DATA for  PMD ver.4.4-  ";

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) enum CompanionLoadError {
    Missing,
    Invalid,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct CompanionBanks {
    pub(crate) adpcm: Option<AdpcmBank>,
    pub(crate) pps: Option<PpsBank>,
    pub(crate) p86: Option<P86Bank>,
    pub(crate) ppz: [Option<PpzBank>; 2],
}

impl CompanionBanks {
    pub(crate) fn empty() -> Self {
        Self {
            adpcm: None,
            pps: None,
            p86: None,
            ppz: [None, None],
        }
    }
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) enum AdpcmFormat {
    Pvi8,
    Ppc,
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) struct AdpcmSpan {
    pub(crate) start_block: u16,
    pub(crate) end_block: u16,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct AdpcmBank {
    pub(crate) format: AdpcmFormat,
    pub(crate) pcm_end: u16,
    pub(crate) entries: Vec<AdpcmSpan>,

    pub(crate) data: Vec<u8>,
}

impl AdpcmBank {
    fn parse(bytes: &[u8]) -> Result<Self, ()> {
        if bytes.starts_with(b"PVI") && bytes.get(10) == Some(&2) {
            Self::parse_pvi(bytes)
        } else if bytes.starts_with(PPC_HEADER) {
            Self::parse_ppc(bytes)
        } else {
            Err(())
        }
    }

    fn parse_pvi(bytes: &[u8]) -> Result<Self, ()> {
        if bytes.len() < PVI_ADPCM_HEADER_SIZE {
            return Err(());
        }

        let mut entries = Vec::with_capacity(256);
        let mut pcm_end = 0u16;
        for index in 0..128usize {
            let offset = 0x10 + index * 4;
            let raw_start = read_u16(bytes, offset).ok_or(())?;
            let raw_end = read_u16(bytes, offset + 2).ok_or(())?;
            let span = if raw_end == 0 {
                AdpcmSpan {
                    start_block: raw_start,
                    end_block: 0,
                }
            } else {
                AdpcmSpan {
                    start_block: raw_start.checked_add(ADPCM_DATA_START_BLOCK).ok_or(())?,
                    end_block: raw_end.checked_add(ADPCM_DATA_START_BLOCK).ok_or(())?,
                }
            };
            if span.end_block != 0 {
                if span.end_block < span.start_block {
                    return Err(());
                }
                pcm_end = pcm_end.max(span.end_block.checked_add(1).ok_or(())?);
            }
            entries.push(span);
        }
        entries.extend((128..256).map(|_| AdpcmSpan {
            start_block: 0,
            end_block: 0,
        }));

        let data = copy_adpcm_payload(bytes, PVI_ADPCM_HEADER_SIZE, pcm_end)?;
        Ok(Self {
            format: AdpcmFormat::Pvi8,
            pcm_end,
            entries,
            data,
        })
    }

    fn parse_ppc(bytes: &[u8]) -> Result<Self, ()> {
        if bytes.len() < PPC_HEADER_SIZE {
            return Err(());
        }

        let pcm_end = read_u16(bytes, 30).ok_or(())?;
        if pcm_end != 0 && pcm_end < ADPCM_DATA_START_BLOCK {
            return Err(());
        }

        let mut entries = Vec::with_capacity(256);
        for index in 0..256usize {
            let offset = 32 + index * 4;
            let start_block = read_u16(bytes, offset).ok_or(())?;
            let end_block = read_u16(bytes, offset + 2).ok_or(())?;
            if end_block != 0
                && (start_block < ADPCM_DATA_START_BLOCK
                    || end_block < start_block
                    || end_block > pcm_end)
            {
                return Err(());
            }
            entries.push(AdpcmSpan {
                start_block,
                end_block,
            });
        }

        let data = copy_adpcm_payload(bytes, PPC_HEADER_SIZE, pcm_end)?;
        Ok(Self {
            format: AdpcmFormat::Ppc,
            pcm_end,
            entries,
            data,
        })
    }

    pub(crate) fn sample_bytes(&self, index: usize) -> Option<&[u8]> {
        let span = self.entries.get(index)?;
        if span.end_block <= span.start_block || span.start_block < ADPCM_DATA_START_BLOCK {
            return None;
        }
        let start = usize::from(span.start_block - ADPCM_DATA_START_BLOCK)
            .checked_mul(ADPCM_BLOCK_BYTES)?;
        let length =
            usize::from(span.end_block - span.start_block).checked_mul(ADPCM_BLOCK_BYTES)?;
        self.data.get(start..start.checked_add(length)?)
    }
}

fn copy_adpcm_payload(bytes: &[u8], offset: usize, pcm_end: u16) -> Result<Vec<u8>, ()> {
    if pcm_end <= ADPCM_DATA_START_BLOCK {
        return Ok(Vec::new());
    }
    let blocks = usize::from(pcm_end - ADPCM_DATA_START_BLOCK);
    let length = blocks.checked_mul(ADPCM_BLOCK_BYTES).ok_or(())?;
    if length > MAX_COMPANION_DATA {
        return Err(());
    }
    let end = offset.checked_add(length).ok_or(())?;
    Ok(bytes.get(offset..end).ok_or(())?.to_vec())
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) struct PpsEntry {
    pub(crate) address: u16,
    pub(crate) length: u16,
    pub(crate) tone_offset: u8,
    pub(crate) volume_offset: u8,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct PpsBank {
    pub(crate) entries: Vec<PpsEntry>,

    pub(crate) samples: Vec<i32>,
}

impl PpsBank {
    fn parse(bytes: &[u8]) -> Result<Self, ()> {
        if bytes.len() < PPS_HEADER_SIZE {
            return Err(());
        }

        let mut entries = Vec::with_capacity(14);
        for index in 0..14usize {
            let offset = index * 6;
            entries.push(PpsEntry {
                address: read_u16(bytes, offset).ok_or(())?,
                length: read_u16(bytes, offset + 2).ok_or(())?,
                tone_offset: *bytes.get(offset + 4).ok_or(())?,
                volume_offset: *bytes.get(offset + 5).ok_or(())?,
            });
        }

        let payload = bytes.get(PPS_HEADER_SIZE..).ok_or(())?;
        if payload.len() > MAX_COMPANION_DATA / 2 {
            return Err(());
        }
        let mut samples = Vec::with_capacity(payload.len().saturating_mul(2));
        for byte in payload {
            samples.push(i32::from(byte >> 4));
            samples.push(i32::from(byte & 0x0f));
        }

        for entry in &entries {
            if entry.address == 0 || entry.length == 0 {
                continue;
            }
            let base = usize::from(entry.address)
                .checked_sub(PPS_HEADER_SIZE)
                .ok_or(())?
                .checked_mul(2)
                .ok_or(())?;
            let end = base
                .checked_add(usize::from(entry.length).checked_mul(2).ok_or(())?)
                .ok_or(())?;
            if end > samples.len() {
                return Err(());
            }

            let fade_start = base.max(end.saturating_sub(160));
            let fade_length = end - fade_start;
            if fade_length != 0 {
                for (position, sample) in samples
                    .iter_mut()
                    .enumerate()
                    .skip(fade_start)
                    .take(fade_length)
                {
                    let attenuation = ((position - fade_start) * 16) / fade_length;
                    let value = *sample - i32::try_from(attenuation).map_err(|_| ())?;
                    *sample = value.max(0);
                }
            }
        }

        Ok(Self { entries, samples })
    }

    pub(crate) fn sample_values(&self, index: usize) -> Option<&[i32]> {
        let entry = self.entries.get(index)?;
        if entry.address == 0 {
            return None;
        }
        let start = usize::from(entry.address)
            .checked_sub(PPS_HEADER_SIZE)?
            .checked_mul(2)?;
        let length = usize::from(entry.length).checked_mul(2)?;
        self.samples.get(start..start.checked_add(length)?)
    }
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) struct P86Entry {
    pub(crate) start: u32,
    pub(crate) size: u32,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct P86Bank {
    pub(crate) entries: Vec<P86Entry>,
    pub(crate) samples: Vec<u8>,
}

impl P86Bank {
    fn parse(bytes: &[u8]) -> Result<Self, ()> {
        if bytes.len() < P86_HEADER_SIZE {
            return Err(());
        }

        let sample_data = bytes.get(P86_HEADER_SIZE..).ok_or(())?;
        if sample_data.len() > MAX_COMPANION_DATA {
            return Err(());
        }
        let sample_len = u32::try_from(sample_data.len()).map_err(|_| ())?;
        let mut entries = Vec::with_capacity(256);
        for index in 0..256usize {
            let offset = 0x10 + index * 6;
            let absolute_start = read_u24(bytes, offset).ok_or(())?;
            let size = read_u24(bytes, offset + 3).ok_or(())?;
            if size == 0 {
                entries.push(P86Entry { start: 0, size: 0 });
                continue;
            }
            if absolute_start < P86_HEADER_SIZE as u32 {
                return Err(());
            }
            let start = absolute_start - P86_HEADER_SIZE as u32;
            if start > sample_len || size > sample_len - start {
                return Err(());
            }
            entries.push(P86Entry { start, size });
        }

        Ok(Self {
            entries,
            samples: sample_data.to_vec(),
        })
    }

    pub(crate) fn sample_bytes(&self, index: usize) -> Option<&[u8]> {
        let entry = self.entries.get(index)?;
        if entry.size == 0 {
            return None;
        }
        let start = usize::try_from(entry.start).ok()?;
        let size = usize::try_from(entry.size).ok()?;
        self.samples.get(start..start.checked_add(size)?)
    }
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) struct PpzEntry {
    pub(crate) start: u32,
    pub(crate) size: u32,
    pub(crate) loop_start: u32,
    pub(crate) loop_end: u32,
    pub(crate) rate: u16,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct PpzBank {
    pub(crate) from_pvi: bool,
    pub(crate) entries: Vec<PpzEntry>,
    pub(crate) samples: Vec<u8>,
}

impl PpzBank {
    fn parse_pzi(bytes: &[u8]) -> Result<Self, ()> {
        if bytes.len() < PPZ_PZI_HEADER_SIZE || !bytes.starts_with(b"PZI") {
            return Err(());
        }
        let sample_data = bytes.get(PPZ_PZI_HEADER_SIZE..).ok_or(())?;
        if sample_data.len() > MAX_COMPANION_DATA {
            return Err(());
        }

        let mut entries = Vec::with_capacity(128);
        for index in 0..128usize {
            let offset = 0x20 + index * 18;
            let entry = PpzEntry {
                start: read_u32(bytes, offset).ok_or(())?,
                size: read_u32(bytes, offset + 4).ok_or(())?,
                loop_start: read_u32(bytes, offset + 8).ok_or(())?,
                loop_end: read_u32(bytes, offset + 12).ok_or(())?,
                rate: read_u16(bytes, offset + 16).ok_or(())?,
            };
            let start = usize::try_from(entry.start).map_err(|_| ())?;
            let size = usize::try_from(entry.size).map_err(|_| ())?;
            if size != 0 && (start > sample_data.len() || size > sample_data.len() - start) {
                return Err(());
            }
            entries.push(entry);
        }

        Ok(Self {
            from_pvi: false,
            entries,
            samples: sample_data.to_vec(),
        })
    }

    fn parse_pvi(bytes: &[u8]) -> Result<Self, ()> {
        if bytes.len() < PPZ_PVI_HEADER_SIZE || !bytes.starts_with(b"PVI") {
            return Err(());
        }
        let count = usize::from(*bytes.get(0x0b).ok_or(())?);
        if count > 128 {
            return Err(());
        }
        let source = bytes.get(PPZ_PVI_HEADER_SIZE..).ok_or(())?;
        let mut entries = Vec::with_capacity(128);
        let mut output_size = 0usize;

        for index in 0..128usize {
            let offset = 0x10 + index * 4;
            let start_address = u32::from(read_u16(bytes, offset).ok_or(())?) << 6;
            let end_address = read_u16(bytes, offset + 2).ok_or(())?;
            let raw_start = read_u16(bytes, offset).ok_or(())?;
            let (start, size) = if index < count {
                if end_address < raw_start {
                    return Err(());
                }
                let size = u32::from(end_address - raw_start + 1)
                    .checked_shl(6)
                    .ok_or(())?;
                (start_address, size)
            } else {
                (0, 0)
            };
            output_size = output_size
                .checked_add(usize::try_from(size).map_err(|_| ())?)
                .ok_or(())?;
            entries.push(PpzEntry {
                start,
                size,
                loop_start: 0xffff,
                loop_end: 0xffff,
                rate: 16000,
            });
        }

        if output_size > MAX_COMPANION_DATA {
            return Err(());
        }
        let source_size = output_size.checked_div(2).ok_or(())?;
        if source.len() < source_size {
            return Err(());
        }

        let samples = decode_pvi_samples(source, output_size)?;

        Ok(Self {
            from_pvi: true,
            entries,
            samples,
        })
    }

    pub(crate) fn sample_bytes(&self, index: usize) -> Option<&[u8]> {
        let entry = self.entries.get(index)?;
        if entry.size == 0 {
            return None;
        }
        let start = usize::try_from(entry.start).ok()?;
        let size = usize::try_from(entry.size).ok()?;
        self.samples.get(start..start.checked_add(size)?)
    }
}

fn decode_pvi_samples(source: &[u8], output_size: usize) -> Result<Vec<u8>, ()> {
    const TABLE1: [i32; 16] = [1, 3, 5, 7, 9, 11, 13, 15, -1, -3, -5, -7, -9, -11, -13, -15];
    const TABLE2: [i32; 16] = [
        57, 57, 57, 57, 77, 102, 128, 153, 57, 57, 57, 57, 77, 102, 128, 153,
    ];
    let source_size = output_size.checked_div(2).ok_or(())?;
    if source.len() < source_size {
        return Err(());
    }
    let mut x_n = 0x80i32;
    let mut delta_n = 127i32;
    let mut output = Vec::with_capacity(output_size);
    for byte in source.iter().take(source_size) {
        for nibble in [byte >> 4, byte & 0x0f] {
            let index = usize::from(nibble);
            x_n = (x_n + TABLE1[index] * delta_n / 8).clamp(-32768, 32767);
            delta_n = (delta_n * TABLE2[index] / 64).clamp(127, 24576);
            output.push((x_n / (32768 / 128) + 128) as u8);
        }
    }
    Ok(output)
}

fn read_u16(bytes: &[u8], offset: usize) -> Option<u16> {
    let pair = bytes.get(offset..offset.checked_add(2)?)?;
    Some(u16::from_le_bytes([pair[0], pair[1]]))
}

fn read_u24(bytes: &[u8], offset: usize) -> Option<u32> {
    let triple = bytes.get(offset..offset.checked_add(3)?)?;
    Some(u32::from(triple[0]) | (u32::from(triple[1]) << 8) | (u32::from(triple[2]) << 16))
}

fn read_u32(bytes: &[u8], offset: usize) -> Option<u32> {
    let quad = bytes.get(offset..offset.checked_add(4)?)?;
    Some(u32::from_le_bytes([quad[0], quad[1], quad[2], quad[3]]))
}

fn normalize_name(name: &str) -> String {
    let mut normalized = String::with_capacity(name.len());
    for character in name.chars() {
        normalized.push(character.to_ascii_uppercase());
    }
    normalized
}

fn normalize_lower_name(name: &str) -> String {
    let mut normalized = String::with_capacity(name.len());
    for character in name.chars() {
        normalized.push(character.to_ascii_lowercase());
    }
    normalized
}

fn lookup(files: &dyn FileProvider, name: &str) -> Option<Vec<u8>> {
    if let Some(bytes) = files.get(name) {
        return Some(bytes.to_vec());
    }
    let normalized = normalize_name(name);
    if normalized != name {
        if let Some(bytes) = files.get(&normalized) {
            return Some(bytes.to_vec());
        }
    }
    let lower = normalize_lower_name(name);
    if lower != name {
        if let Some(bytes) = files.get(&lower) {
            return Some(bytes.to_vec());
        }
    }

    let separator = name.rfind(['/', '\\']);
    let separator = separator?;
    let basename = &name[separator + 1..];
    if let Some(bytes) = files.get(basename) {
        return Some(bytes.to_vec());
    }
    let normalized_basename = normalize_name(basename);
    if let Some(bytes) = files.get(&normalized_basename) {
        return Some(bytes.to_vec());
    }
    let lower_basename = normalize_lower_name(basename);
    files.get(&lower_basename).map(ToOwned::to_owned)
}

fn extension(name: &str) -> Option<&str> {
    let basename = name
        .rsplit_once(['/', '\\'])
        .map_or(name, |(_, basename)| basename);
    basename.rsplit_once('.').map(|(_, extension)| extension)
}

fn extension_is(name: &str, expected: &str) -> bool {
    extension(name).is_some_and(|value| value.eq_ignore_ascii_case(expected))
}

fn has_extension(name: &str) -> bool {
    extension(name).is_some_and(|value| !value.is_empty())
}

fn replace_extension(name: &str, new_extension: &str) -> String {
    let separator = name.rfind(['/', '\\']);
    let dot = name.rfind('.');
    let prefix_end = match (separator, dot) {
        (Some(separator), Some(dot)) if dot > separator => dot,
        (None, Some(dot)) => dot,
        _ => name.len(),
    };
    let mut result = String::with_capacity(prefix_end + new_extension.len());
    result.push_str(&name[..prefix_end]);
    result.push_str(new_extension);
    result
}

fn load_adpcm(
    files: &dyn FileProvider,
    name: &str,
) -> Result<Option<AdpcmBank>, CompanionLoadError> {
    let Some(bytes) = lookup(files, name) else {
        return Ok(None);
    };
    AdpcmBank::parse(&bytes)
        .map(Some)
        .map_err(|_| CompanionLoadError::Invalid)
}

fn load_p86(files: &dyn FileProvider, name: &str) -> Result<Option<P86Bank>, CompanionLoadError> {
    let Some(bytes) = lookup(files, name) else {
        return Ok(None);
    };
    P86Bank::parse(&bytes)
        .map(Some)
        .map_err(|_| CompanionLoadError::Invalid)
}

fn load_pps(files: &dyn FileProvider, name: &str) -> Result<Option<PpsBank>, CompanionLoadError> {
    let Some(bytes) = lookup(files, name) else {
        return Ok(None);
    };
    PpsBank::parse(&bytes)
        .map(Some)
        .map_err(|_| CompanionLoadError::Invalid)
}

fn load_ppz(files: &dyn FileProvider, name: &str) -> Result<Option<PpzBank>, CompanionLoadError> {
    let Some(bytes) = lookup(files, name) else {
        return Ok(None);
    };
    let result = if extension_is(name, "PZI") {
        PpzBank::parse_pzi(&bytes)
    } else {
        PpzBank::parse_pvi(&bytes)
    };
    result.map(Some).map_err(|_| CompanionLoadError::Invalid)
}

pub(crate) fn load_companions(
    names: &CompanionNames,
    files: &dyn FileProvider,
) -> Result<CompanionBanks, CompanionLoadError> {
    let mut banks = CompanionBanks::empty();

    if let Some(name) = names.pcm.as_deref() {
        if extension_is(name, "P86") {
            let fallback = replace_extension(name, ".PPC");
            match load_p86(files, name) {
                Ok(Some(bank)) => banks.p86 = Some(bank),
                Ok(None) => {
                    banks.adpcm = load_adpcm(files, &fallback)?;
                    if banks.adpcm.is_none() {
                        return Err(CompanionLoadError::Missing);
                    }
                }
                Err(CompanionLoadError::Invalid) => match load_adpcm(files, &fallback) {
                    Ok(Some(bank)) => banks.adpcm = Some(bank),
                    Ok(None) | Err(CompanionLoadError::Missing) => {
                        return Err(CompanionLoadError::Invalid)
                    }
                    Err(CompanionLoadError::Invalid) => return Err(CompanionLoadError::Invalid),
                },
                Err(CompanionLoadError::Missing) => {
                    banks.adpcm = load_adpcm(files, &fallback)?;
                    if banks.adpcm.is_none() {
                        return Err(CompanionLoadError::Missing);
                    }
                }
            }
        } else {
            match load_adpcm(files, name) {
                Ok(Some(bank)) => banks.adpcm = Some(bank),
                Ok(None) => {
                    let fallback = replace_extension(name, ".P86");
                    match load_p86(files, &fallback) {
                        Ok(Some(bank)) => banks.p86 = Some(bank),
                        Ok(None) => return Err(CompanionLoadError::Missing),
                        Err(CompanionLoadError::Invalid) => {
                            return Err(CompanionLoadError::Invalid)
                        }
                        Err(CompanionLoadError::Missing) => {
                            return Err(CompanionLoadError::Missing)
                        }
                    }
                }
                Err(CompanionLoadError::Invalid) => {
                    let fallback = replace_extension(name, ".P86");
                    match load_p86(files, &fallback) {
                        Ok(Some(bank)) => banks.p86 = Some(bank),
                        Ok(None) | Err(CompanionLoadError::Missing) => {
                            return Err(CompanionLoadError::Invalid)
                        }
                        Err(CompanionLoadError::Invalid) => {
                            return Err(CompanionLoadError::Invalid)
                        }
                    }
                }
                Err(CompanionLoadError::Missing) => {
                    let fallback = replace_extension(name, ".P86");
                    match load_p86(files, &fallback) {
                        Ok(Some(bank)) => banks.p86 = Some(bank),
                        Ok(None) | Err(CompanionLoadError::Missing) => {
                            return Err(CompanionLoadError::Missing)
                        }
                        Err(CompanionLoadError::Invalid) => {
                            return Err(CompanionLoadError::Invalid)
                        }
                    }
                }
            }
        }
    }

    if let Some(name) = names.pps.as_deref() {
        let pps_name = if extension_is(name, "PPS") {
            name.to_owned()
        } else {
            replace_extension(name, ".PPS")
        };
        match load_pps(files, &pps_name)? {
            Some(bank) => banks.pps = Some(bank),
            None if !extension_is(name, "PPS") => match load_pps(files, name)? {
                Some(bank) => banks.pps = Some(bank),
                None => return Err(CompanionLoadError::Missing),
            },
            None => return Err(CompanionLoadError::Missing),
        }
    }

    if let Some(names) = names.ppz.as_deref() {
        if extension_is(names, "PMB") {
            return Ok(banks);
        }
        let mut pieces = names.split(',');
        let first = pieces.next().ok_or(CompanionLoadError::Missing)?;
        let second = pieces.next();
        if pieces.next().is_some() || first.is_empty() || second.is_some_and(str::is_empty) {
            return Err(CompanionLoadError::Invalid);
        }
        for (slot, piece) in [Some(first), second].into_iter().flatten().enumerate() {
            let ppz_name = if has_extension(piece) {
                piece.to_owned()
            } else {
                replace_extension(piece, ".PZI")
            };
            banks.ppz[slot] = Some(load_ppz(files, &ppz_name)?.ok_or(CompanionLoadError::Missing)?);
        }
    }

    Ok(banks)
}

#[cfg(test)]
mod tests {
    use super::{
        load_companions, AdpcmBank, AdpcmFormat, P86Bank, PpsBank, PpzBank, PPC_HEADER_SIZE,
        PPS_HEADER_SIZE, PPZ_PVI_HEADER_SIZE, PPZ_PZI_HEADER_SIZE, PVI_ADPCM_HEADER_SIZE,
    };
    use crate::pmd::CompanionNames;
    use crate::FileProvider;
    use alloc::vec;
    use alloc::vec::Vec;

    struct Files {
        name: &'static str,
        bytes: Vec<u8>,
    }

    impl FileProvider for Files {
        fn get(&self, name: &str) -> Option<&[u8]> {
            (name == self.name).then_some(self.bytes.as_slice())
        }
    }

    fn set_u16(bytes: &mut [u8], offset: usize, value: u16) {
        bytes[offset..offset + 2].copy_from_slice(&value.to_le_bytes());
    }

    fn set_u24(bytes: &mut [u8], offset: usize, value: u32) {
        bytes[offset..offset + 3].copy_from_slice(&[
            value as u8,
            (value >> 8) as u8,
            (value >> 16) as u8,
        ]);
    }

    #[test]
    fn parses_ppc_and_pvi_adpcm_block_addresses() {
        let mut ppc = vec![0u8; PPC_HEADER_SIZE + 64];
        ppc[..30].copy_from_slice(b"ADPCM DATA for  PMD ver.4.4-  ");
        set_u16(&mut ppc, 30, 0x28);
        set_u16(&mut ppc, 32, 0x26);
        set_u16(&mut ppc, 34, 0x28);
        let ppc_bank = AdpcmBank::parse(&ppc).expect("synthetic PPC");
        assert_eq!(ppc_bank.format, AdpcmFormat::Ppc);
        assert_eq!(ppc_bank.sample_bytes(0).expect("sample").len(), 64);

        let mut pvi = vec![0u8; PVI_ADPCM_HEADER_SIZE + 96];
        pvi[..4].copy_from_slice(b"PVI2");
        pvi[10] = 2;
        set_u16(&mut pvi, 0x10, 0);
        set_u16(&mut pvi, 0x12, 2);
        let pvi_bank = AdpcmBank::parse(&pvi).expect("synthetic PVI");
        assert_eq!(pvi_bank.format, AdpcmFormat::Pvi8);
        assert_eq!(pvi_bank.sample_bytes(0).expect("sample").len(), 64);
    }

    #[test]
    fn parses_pps_and_p86_payloads_without_filesystem_access() {
        let mut pps = vec![0u8; PPS_HEADER_SIZE + 1];
        set_u16(&mut pps, 0, PPS_HEADER_SIZE as u16);
        set_u16(&mut pps, 2, 1);
        pps[PPS_HEADER_SIZE] = 0xab;
        let pps_bank = PpsBank::parse(&pps).expect("synthetic PPS");
        assert_eq!(pps_bank.sample_values(0).expect("sample"), &[10, 3]);

        let mut p86 = vec![0u8; 1552 + 3];
        set_u24(&mut p86, 0x10, 1552);
        set_u24(&mut p86, 0x13, 3);
        p86[1552..].copy_from_slice(&[1, 2, 3]);
        let p86_bank = P86Bank::parse(&p86).expect("synthetic P86");
        assert_eq!(p86_bank.sample_bytes(0), Some(&[1, 2, 3][..]));
    }

    #[test]
    fn parses_pzi_and_pvi_ppz_banks() {
        let mut pzi = vec![0u8; PPZ_PZI_HEADER_SIZE + 3];
        pzi[..4].copy_from_slice(b"PZI1");
        pzi[0x0b] = 1;
        pzi[0x20..0x24].copy_from_slice(&0u32.to_le_bytes());
        pzi[0x24..0x28].copy_from_slice(&3u32.to_le_bytes());
        pzi[0x30..0x32].copy_from_slice(&16000u16.to_le_bytes());
        pzi[PPZ_PZI_HEADER_SIZE..].copy_from_slice(&[1, 2, 3]);
        let pzi_bank = PpzBank::parse_pzi(&pzi).expect("synthetic PZI");
        assert!(!pzi_bank.from_pvi);
        assert_eq!(pzi_bank.sample_bytes(0), Some(&[1, 2, 3][..]));

        let mut pvi = vec![0u8; PPZ_PVI_HEADER_SIZE + 32];
        pvi[..4].copy_from_slice(b"PVI2");
        pvi[0x0b] = 1;
        set_u16(&mut pvi, 0x10, 0);
        set_u16(&mut pvi, 0x12, 0);
        let pvi_bank = PpzBank::parse_pvi(&pvi).expect("synthetic PPZ PVI");
        assert!(pvi_bank.from_pvi);
        assert_eq!(pvi_bank.sample_bytes(0).expect("sample").len(), 64);
    }

    #[test]
    fn companion_lookup_normalizes_case_and_replaces_extensions() {
        let mut ppc = vec![0u8; PPC_HEADER_SIZE];
        ppc[..30].copy_from_slice(b"ADPCM DATA for  PMD ver.4.4-  ");
        set_u16(&mut ppc, 30, 0x26);
        let files = Files {
            name: "BANK.PPC",
            bytes: ppc,
        };
        let banks = load_companions(
            &CompanionNames {
                pcm: Some("bank.P86".into()),
                ..CompanionNames::default()
            },
            &files,
        )
        .expect("case-insensitive PPC fallback");
        assert_eq!(banks.adpcm.expect("ADPCM bank").format, AdpcmFormat::Ppc);

        let files = Files {
            name: "bank.ppc",
            bytes: empty_ppc_for_lookup(),
        };
        let banks = load_companions(
            &CompanionNames {
                pcm: Some("BANK.PPC".into()),
                ..CompanionNames::default()
            },
            &files,
        )
        .expect("reverse case-insensitive lookup");
        assert!(banks.adpcm.is_some());
    }

    fn empty_ppc_for_lookup() -> Vec<u8> {
        let mut ppc = vec![0u8; PPC_HEADER_SIZE];
        ppc[..30].copy_from_slice(b"ADPCM DATA for  PMD ver.4.4-  ");
        set_u16(&mut ppc, 30, 0x26);
        ppc
    }
}
