"""Encode original TH15 PCM with exact source length and loop positions."""
from pathlib import Path
import hashlib, json, struct, sys
workspace = Path(__file__).resolve().parents[2]
project = workspace / 'th15_web'
sys.path.insert(0, str(workspace / 'tools/architecture/python'))
import numpy as np
import soundfile as sf
out = project / 'assets/music'
out.mkdir(parents=True, exist_ok=True)
layout = (project / 'reference/assets/thbgm.fmt').read_bytes()
original = workspace / '[th15] 东方绀珠传 (汉化版+日文版)/thbgm.dat'
rows = []
with original.open('rb') as source:
    if source.read(4) != b'ZWAV':
        raise ValueError('Unexpected original music container')
    for at in range(0, len(layout) - 51, 52):
        if not layout[at]:
            break
        name, offset, length, intro, end = struct.unpack_from('<16sIIII', layout, at)
        name = name.split(b'\0')[0].decode('ascii')
        fmt = struct.unpack_from('<HHIIHH', layout, at + 32)
        if fmt != (1, 2, 44100, 176400, 4, 16) or not 0 <= intro < end <= length or intro % 4 or end % 4:
            raise ValueError('Invalid PCM layout ' + name)
        source.seek(offset)
        pcm = source.read(end)
        if len(pcm) != end:
            raise ValueError('Truncated original PCM ' + name)
        samples = np.frombuffer(pcm, dtype='<i2').reshape(-1, 2)
        path = out / (Path(name).stem + '.ogg')
        pending = path.with_suffix('.ogg.tmp')
        with sf.SoundFile(str(pending), 'w', samplerate=44100, channels=2, subtype='VORBIS', format='OGG', compression_level=.4) as target:
            for start in range(0, len(samples), 16384):
                target.write(samples[start:start+16384].astype(np.float32) / 32768.)
        pending.replace(path)
        decoded, rate = sf.read(str(path), dtype='int16', always_2d=True)
        if decoded.shape != samples.shape or rate != 44100:
            raise ValueError('Vorbis PCM sample count mismatch ' + name)
        rows.append(dict(filename=name, offset=offset, length=end, intro=intro, rate=44100, channels=2, align=4, frames=len(samples), loopFrame=intro//4, bytes=path.stat().st_size, sha256=hashlib.sha256(path.read_bytes()).hexdigest(), originalPcmSha256=hashlib.sha256(pcm).hexdigest(), decodedBytes=decoded.size*2))
        print(f'{len(rows)}/18 {name}: {len(samples)} PCM frames verified', flush=True)
if len(rows) != 18:
    raise ValueError('Missing original music tracks')
report = dict(kind='th15-original-pcm-vorbis-conversion', layoutSha256=hashlib.sha256(layout).hexdigest(), sourceBytes=original.stat().st_size, codec='Vorbis compression_level 0.4; lossy audio, exact sample count and loop positions', tracks=rows)
(out / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
