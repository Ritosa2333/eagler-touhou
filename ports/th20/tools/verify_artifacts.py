"""Independently hash exported/rebuilt artifacts; does not run recovery tools."""
from pathlib import Path
from collections import Counter
import hashlib
import json
import wave

ROOT = Path(__file__).resolve().parents[1]
def digest(path):
    with path.open('rb') as f:
        return hashlib.file_digest(f, 'sha256').hexdigest()

def main():
    assets = json.loads((ROOT / 'reports/assets_recovery.json').read_text('utf-8'))
    failures, counts = [], Counter()
    for item in assets['roundtrips']:
        original = ROOT / 'assets/raw' / item['name']
        rebuilt = ROOT / 'scripts/recovered/roundtrip' / item['kind'] / item['name']
        expected = item['original_sha256']
        if not rebuilt.is_file() or digest(original) != expected or digest(rebuilt) != expected:
            failures.append({'kind': item['kind'], 'name': item['name']})
        counts[item['kind']] += 1
    if dict(counts) != {'ecl': 21, 'msg': 87, 'anm': 73, 'std': 10}:
        failures.append({'missing_or_unexpected_archive_counts': dict(counts)})
    bgm = json.loads((ROOT / 'reports/bgm_recovery.json').read_text('utf-8'))
    for track in bgm['tracks']:
        wav_file = ROOT / 'assets/bgm' / track['name']
        with wave.open(str(wav_file), 'rb') as stream:
            pcm = stream.readframes(stream.getnframes())
        if hashlib.sha256(pcm).hexdigest() != track['pcm_sha256']:
            failures.append({'kind': 'bgm', 'name': track['name']})
    result = {'status': 'passed' if not failures else 'failed',
        'verification': 'independent rehash of actual raw and rebuilt files plus WAV-decoded PCM',
        'script_counts': dict(counts), 'script_total': sum(counts.values()),
        'bgm_total': len(bgm['tracks']), 'failures': failures,
        'gameplay_validated': False}
    (ROOT / 'reports/artifact_verification.json').write_text(json.dumps(result, indent=2), 'utf-8')
    print(json.dumps(result))
    return int(bool(failures))

if __name__ == '__main__':
    raise SystemExit(main())
