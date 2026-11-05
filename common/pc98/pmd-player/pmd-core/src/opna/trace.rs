// SPDX-License-Identifier: AGPL-3.0-only

use alloc::vec::Vec;

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub struct RegisterWrite {
    pub time_us: u64,
    pub tick: u64,
    pub address: u16,
    pub value: u8,
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub struct TraceError {
    pub line: usize,
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub struct TraceMismatch {
    pub index: usize,
    pub expected: Option<RegisterWrite>,
    pub actual: Option<RegisterWrite>,
}

pub fn compare_register_traces(
    expected: &[RegisterWrite],
    actual: &[RegisterWrite],
) -> Result<(), TraceMismatch> {
    let count = expected.len().max(actual.len());
    for index in 0..count {
        if expected.get(index) != actual.get(index) {
            return Err(TraceMismatch {
                index,
                expected: expected.get(index).copied(),
                actual: actual.get(index).copied(),
            });
        }
    }
    Ok(())
}

pub fn parse_trace_csv(input: &[u8]) -> Result<Vec<RegisterWrite>, TraceError> {
    let text = core::str::from_utf8(input).map_err(|_| TraceError { line: 1 })?;
    let mut writes = Vec::new();
    for (line_index, line) in text.lines().enumerate() {
        let line_number = line_index + 1;
        let line = line.trim();
        if line.is_empty() || line.starts_with('#') {
            continue;
        }
        let mut fields = line.split(',').map(str::trim);
        let Some(time_us) = fields.next().and_then(parse_u64) else {
            return Err(TraceError { line: line_number });
        };
        let Some(tick) = fields.next().and_then(parse_u64) else {
            return Err(TraceError { line: line_number });
        };
        let Some(address) = fields.next().and_then(parse_u16) else {
            return Err(TraceError { line: line_number });
        };
        let Some(value) = fields.next().and_then(parse_u8) else {
            return Err(TraceError { line: line_number });
        };
        if fields.next().is_some() {
            return Err(TraceError { line: line_number });
        }
        writes.push(RegisterWrite {
            time_us,
            tick,
            address,
            value,
        });
    }
    Ok(writes)
}

fn parse_u64(value: &str) -> Option<u64> {
    if let Some(hex) = value
        .strip_prefix("0x")
        .or_else(|| value.strip_prefix("0X"))
    {
        u64::from_str_radix(hex, 16).ok()
    } else {
        value.parse().ok()
    }
}

fn parse_u16(value: &str) -> Option<u16> {
    parse_u64(value).and_then(|value| u16::try_from(value).ok())
}

fn parse_u8(value: &str) -> Option<u8> {
    parse_u64(value).and_then(|value| u8::try_from(value).ok())
}

#[cfg(test)]
mod tests {
    use super::{compare_register_traces, parse_trace_csv, RegisterWrite};

    #[test]
    fn parses_reference_header_hex_fields_and_comments() {
        let input = b"# time_us,tick,address,value\n0,0,0x026,0x00\n12,3,42,255\n";
        assert_eq!(
            parse_trace_csv(input),
            Ok(vec![
                RegisterWrite {
                    time_us: 0,
                    tick: 0,
                    address: 0x26,
                    value: 0,
                },
                RegisterWrite {
                    time_us: 12,
                    tick: 3,
                    address: 42,
                    value: 255,
                },
            ])
        );
    }

    #[test]
    fn rejects_malformed_rows_without_partial_success() {
        assert!(parse_trace_csv(b"0,0,0x20\n").is_err());
        assert!(parse_trace_csv(b"0,0,0x20,0x100\n").is_err());
        assert!(parse_trace_csv(b"0,0,0x20,0,extra\n").is_err());
    }

    #[test]
    fn compares_order_tick_time_address_and_value() {
        let expected = [RegisterWrite {
            time_us: 16_154,
            tick: 1,
            address: 0x22,
            value: 0x5a,
        }];
        assert_eq!(compare_register_traces(&expected, &expected), Ok(()));

        let mut actual = expected;
        actual[0].time_us += 1;
        let mismatch = compare_register_traces(&expected, &actual).expect_err("time differs");
        assert_eq!(mismatch.index, 0);
        assert_eq!(mismatch.expected, Some(expected[0]));
        assert_eq!(mismatch.actual, Some(actual[0]));
    }

    #[test]
    fn reports_trace_length_mismatch() {
        let expected = [RegisterWrite {
            time_us: 0,
            tick: 0,
            address: 0x20,
            value: 1,
        }];
        let mismatch = compare_register_traces(&expected, &[]).expect_err("missing row");
        assert_eq!(mismatch.index, 0);
        assert_eq!(mismatch.expected, Some(expected[0]));
        assert_eq!(mismatch.actual, None);
    }
}
