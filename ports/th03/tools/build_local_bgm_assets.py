"""Extract TH03 PMD music from the original PF archive and encode Opus."""
from pathlib import Path
import gzip, hashlib, json, shutil, struct, subprocess
from prepare_assets import files
from inspect_th03 import executable

ROOT = Path(__file__).resolve().parents[1]
DISK = ROOT / 'web/disks/3-jp.hdi.gz'
RENDER = ROOT.parent / 'th04-coop-lab/build/render-pmd.exe'
PRESET = 'opus64-single-loop-v2'

def archive_bytes():
    disk = gzip.decompress(DISK.read_bytes())
    entry = next(e for e in files(disk) if e[0] == 'YUMEZIKU/夢時空1.DAT')
    return b''.join(disk[at:at + 8192] for at in entry[1])[:entry[2]]

def assets(data, suffix='.m'):
    size = struct.unpack_from('<H', data)[0]
    key = data[6]
    table = bytearray(data[16:16 + size])
    for i in range(size):
        table[i] ^= key
        key = (key - table[i]) & 255
    result = {}
    for i in range(0, size, 32):
        kind = struct.unpack_from('<H', table, i)[0]
        if not kind:
            break
        name = bytes(table[i + 3:i + 16]).split(b'\0')[0].decode('ascii').lower()
        packed, original, offset = struct.unpack_from('<HHI', table, i + 16)
        if kind not in (0x9595, 0xf388) or offset < size + 16 or offset + packed > len(data):
            raise ValueError(f'Invalid PF entry: {name}')
        source = bytes(x ^ table[i + 2] for x in data[offset:offset + packed])
        if kind == 0x9595:
            decoded, j, previous = bytearray(), 0, None
            while j < len(source):
                value = source[j]; j += 1; decoded.append(value)
                if value == previous and j < len(source):
                    count = source[j]; j += 1; decoded.extend(bytes([value]) * count)
                previous = value
            source = bytes(decoded)
        if name.endswith(suffix):
            if len(source) < original:
                raise ValueError(f'Truncated song: {name}')
            result[name] = source[:original]
    return result

def build():
    if not RENDER.exists():
        raise RuntimeError(f'missing PMD renderer: {RENDER}')
    ffmpeg = shutil.which('ffmpeg')
    if not ffmpeg:
        raise RuntimeError('ffmpeg not found')
    rawdir, out = ROOT / 'build/bgm-source', ROOT / 'web/bgm'
    rawdir.mkdir(parents=True, exist_ok=True); out.mkdir(exist_ok=True)
    tracks, headers = {}, {}
    songs = assets(archive_bytes())
    if songs != assets(executable('cn', 'YUMEZIKU/夢時空1.DAT')):
        raise RuntimeError('JP/CN music differs; generate language-specific manifests first')
    for name, data in sorted(songs.items()):
        track = name[:-2] + '-m26'
        src = rawdir / name; src.write_bytes(data)
        pcm = rawdir / (name + '.pcm')
        result = subprocess.run([str(RENDER), str(src.relative_to(ROOT)), str(pcm.relative_to(ROOT))], cwd=ROOT,
                                capture_output=True, text=True)
        if result.returncode:
            raise RuntimeError(f'PMD render failed for {name}: {result.stderr}')
        length, loop = map(int, result.stdout.strip().split())
        audio = out / f'{track}-{PRESET}.opus'
        subprocess.run([ffmpeg, '-hide_banner', '-loglevel', 'error', '-y', '-f', 's16le', '-ar', '44100', '-ac', '2',
                        '-i', str(pcm), '-c:a', 'libopus', '-b:a', '64k', '-vbr', 'constrained',
                        '-compression_level', '10', '-application', 'audio', '-ar', '48000', str(audio)], check=True)
        pcm.unlink(missing_ok=True)
        digest = hashlib.sha256(data).hexdigest()
        item = {'url': audio.name, 'loopStart': max(0, length-loop)/1000 if loop else 0,
                'loopEnd': length/1000 if loop else 0, 'sha256': digest,
                'preset': PRESET, 'bytes': audio.stat().st_size}
        tracks[track] = item
        signature = 0x811c9dc5
        for b in data[:25]: signature = ((signature ^ b) * 0x01000193) & 0xffffffff
        if str(signature) in headers:
            raise RuntimeError(f'PMD header collision: {name}')
        headers[str(signature)] = track
        print(f'converted {name}: {length/1000:.2f}s')
    if len(tracks) != 21:
        raise RuntimeError(f'Expected all 21 TH03 songs, got {len(tracks)}')
    temporary = out / 'manifest.next.json'
    temporary.write_text(json.dumps({'version': 2, 'codec': 'opus', 'bitrate': 64000,
        'sampleRate': 48000, 'headerBytes': 25, 'headers': headers, 'tracks': tracks}, indent=2) + '\n', encoding='utf-8')
    temporary.replace(out / 'manifest.json')
    print(f'generated {len(tracks)} TH03 tracks')

if __name__ == '__main__': build()
