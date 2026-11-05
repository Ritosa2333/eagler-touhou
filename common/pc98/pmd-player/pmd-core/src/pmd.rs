// SPDX-License-Identifier: AGPL-3.0-only

use alloc::string::String;
use alloc::vec::Vec;

use encoding_rs::SHIFT_JIS;

use crate::{Error, Metadata};

#[allow(dead_code)]
pub(crate) mod banks;
#[allow(dead_code)]
pub(crate) mod channel;
#[allow(dead_code)]
pub(crate) mod commands;
#[allow(dead_code)]
pub(crate) mod effects;
#[allow(dead_code)]
pub(crate) mod registers;
#[allow(dead_code)]
pub(crate) mod stream;
#[allow(dead_code)]
pub(crate) mod timing;

pub(crate) const MAX_MAIN_FILE_SIZE: usize = 64 * 1024;
const PART_COUNT: usize = 11;

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub(crate) enum MainFormat {
    V18,
    V1a,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct MainLayout {
    pub(crate) part_offsets: [u16; PART_COUNT],
    pub(crate) rhythm_table_offset: u16,
    pub(crate) program_area_offset: u16,
}

#[derive(Clone, Debug, Default, Eq, PartialEq)]
pub(crate) struct CompanionNames {
    pub(crate) pcm: Option<String>,

    pub(crate) pps: Option<String>,

    pub(crate) ppz: Option<String>,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct ParsedSong {
    pub(crate) main: Vec<u8>,
    pub(crate) flags: u8,
    pub(crate) format: MainFormat,

    pub(crate) layout: Option<MainLayout>,
    pub(crate) metadata: Metadata,
    pub(crate) companions: CompanionNames,
}

pub(crate) fn parse_song(main: &[u8]) -> Result<ParsedSong, Error> {
    if main.len() > MAX_MAIN_FILE_SIZE {
        return Err(Error::FileTooLarge);
    }
    if main.len() < 3 {
        return Err(Error::TruncatedFile);
    }

    let flags = main[0];
    if (flags > 0x0f && flags != 0xff) || (main[1] != 0x18 && main[1] != 0x1a) || main[2] != 0 {
        return Err(Error::InvalidHeader);
    }

    let format = if main[1] == 0x18 {
        MainFormat::V18
    } else {
        MainFormat::V1a
    };
    let mmlbuf = &main[1..];
    let layout = parse_layout(mmlbuf);

    let (metadata, companions) = if format == MainFormat::V1a {
        let program_area_offset = read_u16(mmlbuf, 0x18);
        (
            Metadata {
                title: memo_string(mmlbuf, program_area_offset, 1, true),
                composer: memo_string(mmlbuf, program_area_offset, 2, true),
                arranger: memo_string(mmlbuf, program_area_offset, 3, true),
                comment: memo_string(mmlbuf, program_area_offset, 4, true),
            },
            CompanionNames {
                pcm: memo_string(mmlbuf, program_area_offset, 0, false),
                pps: memo_string(mmlbuf, program_area_offset, -1, false),
                ppz: memo_string(mmlbuf, program_area_offset, -2, false),
            },
        )
    } else {
        (Metadata::default(), CompanionNames::default())
    };

    Ok(ParsedSong {
        main: main.to_vec(),
        flags,
        format,
        layout,
        metadata,
        companions,
    })
}

fn parse_layout(mmlbuf: &[u8]) -> Option<MainLayout> {
    if mmlbuf.len() < 0x1a {
        return None;
    }

    let mut part_offsets = [0u16; PART_COUNT];
    for (index, offset) in part_offsets.iter_mut().enumerate() {
        *offset = read_u16(mmlbuf, index * 2)?;
    }
    Some(MainLayout {
        part_offsets,
        rhythm_table_offset: read_u16(mmlbuf, 0x16)?,
        program_area_offset: read_u16(mmlbuf, 0x18)?,
    })
}

fn memo_bytes(mmlbuf: &[u8], program_area_offset: u16, selector: i32) -> Option<&[u8]> {
    if mmlbuf.len() < 0x19 || mmlbuf[0] != 0x1a || mmlbuf[1] != 0 {
        return None;
    }

    let program_area_offset = usize::from(program_area_offset);
    let preamble = program_area_offset.checked_sub(4)?;
    let preamble_end = preamble.checked_add(4)?;
    if preamble_end > mmlbuf.len() {
        return None;
    }

    let marker = mmlbuf[preamble + 2];
    if marker != 0x40 && (mmlbuf[preamble + 3] != 0xfe || marker < 0x41) {
        return None;
    }

    let mut selected = selector;
    if marker >= 0x42 {
        selected += 1;
    }
    if marker >= 0x48 {
        selected += 1;
    }
    if selected < 0 {
        return None;
    }

    let table = usize::from(read_u16(mmlbuf, preamble)?);
    let selected = usize::try_from(selected).ok()?;
    let selected_pointer = table.checked_add(selected.checked_mul(2)?)?;
    if selected_pointer.checked_add(2)? > mmlbuf.len() {
        return None;
    }

    for index in 0..=selected {
        let pointer = table.checked_add(index.checked_mul(2)?)?;
        let data_offset = usize::from(read_u16(mmlbuf, pointer)?);
        if data_offset == 0 || data_offset >= mmlbuf.len() {
            return None;
        }
        if mmlbuf[data_offset] == b'/' {
            return None;
        }
        if index == selected {
            let end = mmlbuf[data_offset..]
                .iter()
                .position(|byte| *byte == 0)
                .map_or(mmlbuf.len(), |length| data_offset + length);
            return Some(&mmlbuf[data_offset..end]);
        }
    }

    None
}

fn memo_string(
    mmlbuf: &[u8],
    program_area_offset: Option<u16>,
    selector: i32,
    display_cleanup: bool,
) -> Option<String> {
    let bytes = memo_bytes(mmlbuf, program_area_offset?, selector)?;
    let normalized = if display_cleanup {
        zen2tohan(bytes)
    } else {
        bytes.to_vec()
    };
    let normalized = if display_cleanup {
        delesc(&normalized)
    } else {
        normalized
    };
    if normalized.is_empty() {
        return None;
    }

    let (decoded, _, _) = SHIFT_JIS.decode(&normalized);
    Some(decoded.into_owned())
}

fn read_u16(bytes: &[u8], offset: usize) -> Option<u16> {
    let pair = bytes.get(offset..offset.checked_add(2)?)?;
    Some(u16::from_le_bytes([pair[0], pair[1]]))
}

fn is_sjis_lead(byte: u8) -> bool {
    (0x81..=0x9f).contains(&byte) || (0xe0..=0xfc).contains(&byte)
}

fn zen2tohan(bytes: &[u8]) -> Vec<u8> {
    let mut output = Vec::with_capacity(bytes.len());
    let mut index = 0;
    while index < bytes.len() {
        let byte = bytes[index];
        if byte == 0x85 && index + 1 < bytes.len() {
            let second = bytes[index + 1];
            if (0x40..=0xfc).contains(&second) {
                match second {
                    0x40..=0x7e => output.push(second - 0x1f),
                    0x7f => {}
                    0x80..=0x9d => output.push(second - 0x20),
                    0x9e => output.extend_from_slice(&[0x85, 0x9e]),
                    0x9f..=0xdd => output.push(second - 0x9f + 0xa1),
                    0xde | 0xdf => output.extend_from_slice(&[0x85, second]),
                    0xe0 => output.push(0xdc),
                    0xe1 => output.push(0xb6),
                    0xe2 => output.push(0xb9),
                    0xe3 => output.extend_from_slice(&[0xb3, 0xde]),
                    0xe4 => output.extend_from_slice(&[0xb6, 0xde]),
                    0xe5 => output.extend_from_slice(&[0xb7, 0xde]),
                    0xe6 => output.extend_from_slice(&[0xb8, 0xde]),
                    0xe7 => output.extend_from_slice(&[0xb9, 0xde]),
                    0xe8 => output.extend_from_slice(&[0xba, 0xde]),
                    0xe9 => output.extend_from_slice(&[0xbb, 0xde]),
                    0xea => output.extend_from_slice(&[0xbc, 0xde]),
                    0xeb => output.extend_from_slice(&[0xbd, 0xde]),
                    0xec => output.extend_from_slice(&[0xbe, 0xde]),
                    0xed => output.extend_from_slice(&[0xbf, 0xde]),
                    0xee => output.extend_from_slice(&[0xc0, 0xde]),
                    0xef => output.extend_from_slice(&[0xc1, 0xde]),
                    0xf0 => output.extend_from_slice(&[0xc2, 0xde]),
                    0xf1 => output.extend_from_slice(&[0xc3, 0xde]),
                    0xf2 => output.extend_from_slice(&[0xc4, 0xde]),
                    0xf3 => output.extend_from_slice(&[0xca, 0xde]),
                    0xf4 => output.extend_from_slice(&[0xca, 0xdf]),
                    0xf5 => output.extend_from_slice(&[0xcb, 0xde]),
                    0xf6 => output.extend_from_slice(&[0xcb, 0xdf]),
                    0xf7 => output.extend_from_slice(&[0xcc, 0xde]),
                    0xf8 => output.extend_from_slice(&[0xcc, 0xdf]),
                    0xf9 => output.extend_from_slice(&[0xcd, 0xde]),
                    0xfa => output.extend_from_slice(&[0xcd, 0xdf]),
                    0xfb => output.extend_from_slice(&[0xce, 0xde]),
                    0xfc => output.extend_from_slice(&[0xce, 0xdf]),
                    _ => unreachable!("the range check covers every zen2tohan entry"),
                }
                index += 2;
                continue;
            }
        }

        output.push(byte);
        if is_sjis_lead(byte) && index + 1 < bytes.len() {
            output.push(bytes[index + 1]);
            index += 2;
        } else {
            index += 1;
        }
    }
    output
}

fn delesc(bytes: &[u8]) -> Vec<u8> {
    let mut output = Vec::with_capacity(bytes.len());
    let mut index = 0;
    while index < bytes.len() {
        if bytes[index] != 0x1b {
            output.push(bytes[index]);
            index += 1;
            continue;
        }

        let command = bytes.get(index + 1).copied();
        match command {
            Some(b'[') => {
                index += 2;
                while index < bytes.len()
                    && !(bytes[index].to_ascii_uppercase() >= b'A'
                        && bytes[index].to_ascii_uppercase() <= b'Z')
                {
                    if is_sjis_lead(bytes[index]) {
                        index = index.saturating_add(2);
                    } else {
                        index += 1;
                    }
                }
                if index < bytes.len() {
                    index += 1;
                }
            }
            Some(b'=') => index = index.saturating_add(4).min(bytes.len()),
            Some(b')' | b'!') => index = index.saturating_add(3).min(bytes.len()),
            Some(_) | None => index = index.saturating_add(2).min(bytes.len()),
        }
    }
    output
}

#[cfg(test)]
mod tests {
    use super::{delesc, parse_song, zen2tohan, MainFormat, MainLayout};
    use crate::Error;
    use alloc::vec::Vec;

    fn set_u16(main: &mut [u8], mml_offset: usize, value: u16) {
        let bytes = value.to_le_bytes();
        main[1 + mml_offset..1 + mml_offset + 2].copy_from_slice(&bytes);
    }

    fn set_bytes(main: &mut [u8], mml_offset: usize, bytes: &[u8]) {
        main[1 + mml_offset..1 + mml_offset + bytes.len()].copy_from_slice(bytes);
    }

    fn metadata_song() -> Vec<u8> {
        let mut main = vec![0u8; 0x100];
        main[1] = 0x1a;
        main[2] = 0;
        set_u16(&mut main, 0x18, 0x40);
        set_u16(&mut main, 0x3c, 0x50);
        main[1 + 0x3e] = 0x40;

        let offsets = [0x70u16, 0x80, 0x90, 0xa0, 0xb0];
        for (index, offset) in offsets.into_iter().enumerate() {
            set_u16(&mut main, 0x50 + index * 2, offset);
        }

        set_bytes(&mut main, 0x70, b"bank.PPC\0");
        set_bytes(
            &mut main,
            0x80,
            &[
                0x93, 0xfa, 0x96, 0x7b, b' ', b'T', b'i', b't', b'l', b'e', 0,
            ],
        );
        set_bytes(&mut main, 0x90, b"Composer\0");
        set_bytes(&mut main, 0xa0, b"Arranger\0");
        set_bytes(&mut main, 0xb0, b"Comment\x1b[2Jvisible\0");
        main
    }

    fn companion_song() -> Vec<u8> {
        let mut main = vec![0u8; 0x100];
        main[1] = 0x1a;
        main[2] = 0;
        set_u16(&mut main, 0x18, 0x40);
        set_u16(&mut main, 0x3c, 0x50);
        main[1 + 0x3e] = 0x48;
        main[1 + 0x3f] = 0xfe;

        let offsets = [0x70u16, 0x80, 0x90, 0xa0, 0xb0];
        for (index, offset) in offsets.into_iter().enumerate() {
            set_u16(&mut main, 0x50 + index * 2, offset);
        }
        set_bytes(&mut main, 0x70, b"bank.PPZ\0");
        set_bytes(&mut main, 0x80, b"bank.PPS\0");
        set_bytes(&mut main, 0x90, b"bank.PPC\0");
        set_bytes(&mut main, 0xa0, b"Title\0");
        set_bytes(&mut main, 0xb0, b"Composer\0");
        main
    }

    #[test]
    fn rejects_truncated_and_invalid_loader_headers() {
        assert_eq!(parse_song(&[]), Err(Error::TruncatedFile));
        assert_eq!(parse_song(&[0, 0x1a]), Err(Error::TruncatedFile));
        assert_eq!(parse_song(&[0x10, 0x1a, 0]), Err(Error::InvalidHeader));
        assert_eq!(parse_song(&[0, 0x19, 0]), Err(Error::InvalidHeader));
        assert_eq!(parse_song(&[0, 0x1a, 1]), Err(Error::InvalidHeader));
    }

    #[test]
    fn rejects_main_streams_larger_than_the_cpp_work_area() {
        let mut main = vec![0u8; 64 * 1024 + 1];
        main[..3].copy_from_slice(&[0, 0x1a, 0]);
        assert_eq!(parse_song(&main), Err(Error::FileTooLarge));
    }

    #[test]
    fn parses_header_layout_and_sjis_display_memos() {
        let song = parse_song(&metadata_song()).expect("synthetic PMD should parse");
        assert_eq!(song.format, MainFormat::V1a);
        assert_eq!(song.flags, 0);
        assert_eq!(song.metadata.title.as_deref(), Some("日本 Title"));
        assert_eq!(song.metadata.composer.as_deref(), Some("Composer"));
        assert_eq!(song.metadata.arranger.as_deref(), Some("Arranger"));
        assert_eq!(song.metadata.comment.as_deref(), Some("Commentvisible"));
        assert_eq!(song.companions.pcm.as_deref(), Some("bank.PPC"));
        assert_eq!(song.companions.pps, None);
        assert_eq!(song.companions.ppz, None);
        assert_eq!(song.main.len(), 0x100);
        assert_eq!(
            song.layout,
            Some(MainLayout {
                part_offsets: [0x1a, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                rhythm_table_offset: 0,
                program_area_offset: 0x40,
            })
        );
    }

    #[test]
    fn marker_extensions_shift_companion_selectors() {
        let song = parse_song(&companion_song()).expect("synthetic PMD should parse");
        assert_eq!(song.companions.pcm.as_deref(), Some("bank.PPC"));
        assert_eq!(song.companions.pps.as_deref(), Some("bank.PPS"));
        assert_eq!(song.companions.ppz.as_deref(), Some("bank.PPZ"));
        assert_eq!(song.metadata.title.as_deref(), Some("Title"));
        assert_eq!(song.metadata.composer.as_deref(), Some("Composer"));

        assert_eq!(song.metadata.arranger, None);
        assert_eq!(song.metadata.comment, None);
    }

    #[test]
    fn ports_halfwidth_and_escape_cleanup() {
        assert_eq!(zen2tohan(&[0x85, 0x40, 0x85, 0x60, 0x85, 0x7f]), b"!A");
        assert_eq!(zen2tohan(&[0x85, 0xdd]), &[0xdf]);
        assert_eq!(
            zen2tohan(&[0x85, 0xde, 0x85, 0xdf]),
            &[0x85, 0xde, 0x85, 0xdf]
        );
        assert_eq!(delesc(b"A\x1b[2JB"), b"AB");
        assert_eq!(delesc(b"A\x1b=12B"), b"AB");
        assert_eq!(delesc(b"A\x1b)1B"), b"AB");
        assert_eq!(delesc(b"A\x1b!1B"), b"AB");
    }

    #[test]
    fn accepts_the_towns_header_variant_without_metadata() {
        let mut main = vec![0u8; 0x1a];
        main[..3].copy_from_slice(&[0xff, 0x18, 0]);
        let song = parse_song(&main).expect("Towns header should be accepted");
        assert_eq!(song.format, MainFormat::V18);
        assert_eq!(song.metadata, Default::default());
    }
}
