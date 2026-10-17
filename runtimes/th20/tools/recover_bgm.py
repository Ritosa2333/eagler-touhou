"""Recover exact PCM and loop boundaries from TH20's local thbgm.fmt/thbgm.dat."""
from pathlib import Path
import argparse
import hashlib
import json
import struct
import wave

ROOT = Path(__file__).resolve().parents[1]
DEFAULT = Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders')

def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--source', type=Path, default=DEFAULT)
    args = ap.parse_args()
    table = (ROOT / 'assets/raw/thbgm.fmt').read_bytes()
    source = args.source / 'thbgm.dat'
    out = ROOT / 'assets/bgm'
    out.mkdir(parents=True, exist_ok=True)
    rows, pos, previous_end = [], 0, 16
    with source.open('rb') as stream:
        header = stream.read(16)
        if header[:4] != b'ZWAV':
            raise ValueError('Expected ZWAV header')
        while pos + 52 <= len(table) and table[pos] != 0:
            row = table[pos:pos+52]
            name = row[:16].split(b'\0')[0].decode('ascii')
            if Path(name).name != name or '/' in name or '\\' in name:
                raise ValueError('Unsafe track name')
            offset, preload, intro, length = struct.unpack_from('<4I', row, 16)
            tag, channels, rate, bps, align, bits = struct.unpack_from('<HHIIHH', row, 32)
            if tag != 1 or bits != 16 or align != channels * 2 or bps != rate * align:
                raise ValueError(f'Unsupported PCM format: {name}')
            if offset != previous_end or not 0 <= intro < length or intro % align or length % align:
                raise ValueError(f'Invalid track boundaries: {name}')
            stream.seek(offset)
            pcm = stream.read(length)
            if len(pcm) != length:
                raise ValueError(f'Truncated track: {name}')
            target = out / name
            with wave.open(str(target), 'wb') as wav:
                wav.setnchannels(channels)
                wav.setsampwidth(bits // 8)
                wav.setframerate(rate)
                wav.writeframes(pcm)
            # Independent WAV container decode must recover exactly the input PCM.
            with wave.open(str(target), 'rb') as wav:
                decoded = wav.readframes(wav.getnframes())
            if decoded != pcm:
                raise RuntimeError(f'PCM roundtrip differs: {name}')
            rows.append({'name': name, 'archive_offset': offset, 'preload_bytes': preload,
                'pcm_bytes': length, 'intro_bytes': intro, 'sample_rate': rate,
                'channels': channels, 'bits_per_sample': bits,
                'loop_start_frame': intro // align, 'loop_end_frame_exclusive': length // align,
                'duration_seconds': length / bps,
                'pcm_sha256': hashlib.sha256(pcm).hexdigest(),
                'wav_sha256': hashlib.sha256(target.read_bytes()).hexdigest(),
                'pcm_roundtrip_equal': True, 'fmt_record_hex': row.hex()})
            pos += 52
            previous_end = offset + length
    if any(table[pos:]) or previous_end != source.stat().st_size:
        raise ValueError('Unexpected unaccounted trailing data')
    report = {'status': 'passed', 'source': str(source), 'track_count': len(rows),
        'total_pcm_bytes': sum(r['pcm_bytes'] for r in rows),
        'archive_header_hex': header.hex(), 'fmt_zero_padding_bytes': len(table)-pos,
        'loop_units': 'interleaved PCM sample frames; end exclusive',
        'note': 'WAVs contain unmodified PCM. Loop metadata is in this JSON, not embedded in WAV.',
        'format_reference': 'https://github.com/thpatch/thcrap/blob/master/thcrap_tsa/src/bgm.cpp',
        'tracks': rows}
    (ROOT / 'reports/bgm_recovery.json').write_text(json.dumps(report, indent=2), 'utf-8')
    (out / 'loops.json').write_text(json.dumps({r['name']: {
        'loop_start_frame': r['loop_start_frame'], 'loop_end_frame_exclusive': r['loop_end_frame_exclusive']}
        for r in rows}, indent=2), 'utf-8')
    print(f'Recovered {len(rows)} tracks; exact PCM roundtrip and complete archive coverage verified')

if __name__ == '__main__':
    main()
