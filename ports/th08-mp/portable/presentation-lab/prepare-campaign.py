"""Fetch only named public player replays and attest their original metadata.

The original bytes are never edited. Decoder mirrors the local ReplayFile/Lzss
read format; the shipping C++ parser must independently accept every fixture.
"""
import hashlib
import json
import struct
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'artifacts/presentation-lab/input/campaign'
FIXTURES = [
    ('lunatic-border', 7955, 'LucasL', 'Reimu & Yukari', 3),
    ('lunatic-magic', 8175, 'R24MARI', 'Marisa & Alice', 3),
    ('lunatic-ghost', 8399, 'IHNN LNB', 'Youmu & Yuyuko', 3),
    ('extra-border', 7153, 'ReiEX', 'Reimu & Yukari', 4),
]


def decode(data):
    if len(data) < 104 or len(data) > 1024*1024 or data[:4] != b'T8RP':
        raise ValueError('Unexpected replay format/size')
    plain = bytearray(data)
    size, checksum = struct.unpack_from('<II', data, 12)
    key = data[21]
    if not 104 <= size <= len(data):
        raise ValueError('Invalid replay length')
    for i in range(24, size):
        plain[i] = (plain[i]-key) & 255
        key = (key+7) & 255
    if (0x3f000318+sum(plain[21:size])) & 0xffffffff != checksum:
        raise ValueError('Replay checksum')
    compressed, expected = struct.unpack_from('<II', plain, 24)
    if compressed > size-104 or not 204 <= expected <= 8*1024*1024:
        raise ValueError('Invalid replay compression size')
    bits = ''.join(f'{b:08b}' for b in plain[104:104+compressed])+'0'*32
    pos, head = 0, 1
    dictionary = bytearray(8192)
    payload = bytearray()

    def get(n):
        nonlocal pos
        if pos+n > len(bits):
            raise ValueError('Truncated replay bitstream')
        value = int(bits[pos:pos+n], 2)
        pos += n
        return value

    def put(value):
        nonlocal head
        if len(payload) >= expected:
            raise ValueError('Replay expansion overflow')
        payload.append(value)
        dictionary[head] = value
        head = (head+1) & 8191

    while True:
        if get(1):
            put(get(8))
        else:
            offset = get(13)
            if not offset:
                break
            count = get(4)+3
            for i in range(count):
                put(dictionary[(offset+i)&8191])
    if len(payload) != expected:
        raise ValueError('Replay payload length')
    decoded = plain[:104]+payload
    stages = struct.unpack_from('<9I', decoded, 32)
    timings = struct.unpack_from('<9I', decoded, 68)
    stage_info = []
    for i, offset in enumerate(stages):
        if not offset:
            continue
        end = min([v for v in (*stages, *timings, len(decoded)) if v > offset])
        stage_info.append({'stageIndex': i, 'offset': offset, 'endScore': struct.unpack_from('<I', decoded, offset)[0],
                           'inputWordsUpperBound': (end-offset-36)//2, 'timingOffset': timings[i]})
    return {'difficulty': decoded[107], 'shot': decoded[106], 'player': bytes(decoded[114:122]).split(b'\0')[0].decode('cp932','replace'),
            'date': bytes(decoded[108:114]).decode('ascii','replace'), 'practice': decoded[123],
            'spell': struct.unpack_from('<h',decoded,124)[0], 'clearState': decoded[284],
            'version': bytes(decoded[300:306]).split(b'\0')[0].decode('ascii','replace'),
            'slowMode': decoded[217], 'stageInfo': stage_info,
            'extensionBytes': len(data)-size, 'hasPRAC': b'PRAC' in data[size:], 'payloadBytes': expected}


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    manifest = []
    for name, entry, player, shot, difficulty in FIXTURES:
        url = f'https://replay.lunarcast.net/replay/th8_ud{entry}.rpy'
        path = OUT / f'{name}.rpy'
        if path.exists():
            data = path.read_bytes()
        else:
            with urllib.request.urlopen(url, timeout=30) as response:
                data = response.read(1024*1024+1)
        metadata = decode(data)
        if metadata['difficulty'] != difficulty or metadata['practice'] or metadata['slowMode'] or metadata['hasPRAC']:
            raise ValueError(f'{name}: not an ordinary full-run fixture: {metadata}')
        path.write_bytes(data)
        item = {'id': name, 'url': url, 'listing': 'https://replay.lunarcast.net/index.php?RpGame=IN&RpDifficulty='+('Extra' if difficulty == 4 else 'Lunatic')+'&RpProgress=Clear',
                'listedPlayer': player, 'listedTeam': shot, 'listingFlags': 'No TAS / Increased FPS / DeSync marking on selected entries',
                'bytes': len(data), 'sha256': hashlib.sha256(data).hexdigest(), 'metadata': metadata}
        manifest.append(item)
        print(json.dumps(item, ensure_ascii=False), flush=True)
    (OUT / 'manifest.json').write_text(json.dumps({'schema': 'th08/campaign-fixtures/1', 'fixtures': manifest}, ensure_ascii=False, indent=2)+'\n', encoding='utf-8')


if __name__ == '__main__':
    main()
