"""TH08 Ogg Vorbis / TH10 lossless music for the native SDL filesystem."""
import sys, pathlib, json, hashlib, struct, gzip, shutil
root = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(root / 'tools/architecture/python'))
import soundfile as sf
import numpy as np
game = 'th08' if '--th08' in sys.argv else 'th10'
base = root / f'{game}_web/assets/sdl-native'
out = base / 'music'
out.mkdir(parents=True, exist_ok=True)
if game == 'th08':
    raw = (root / 'th08_web/reference/thbgm.fmt').read_bytes()
    tracks = []
    for p in range(0, len(raw)-51, 52):
        if not raw[p]: break
        name, offset, preload, intro, length = struct.unpack_from('<16sIIII', raw, p)
        tag, channels, rate, byte_rate, align, bits = struct.unpack_from('<HHIIHH', raw, p+32)
        assert (tag, channels, rate, align, bits) == (1,2,44100,4,16)
        tracks.append(dict(name=name.split(b'\0')[0].decode('ascii'), offset=offset, loop=intro, length=length))
    assert len(tracks) == 21
    (base / 'music-layout.json').write_text(json.dumps(tracks, indent=2))
    for name in ['cp932', 'blend']:
        (base / (name+'.bin')).write_bytes(gzip.decompress((root / ('th08_web/assets/cpp/fonts/'+name+'.bin.gz')).read_bytes()))
    shutil.copyfile(root / 'th10_web/assets/sdl-native/msgothic.ttc', base / 'msgothic.ttc')
    original = root / '[th08] 东方永夜抄 (日文版)/thbgm.dat'
    with original.open('rb') as source: (base / 'thbgm-header.dat').write_bytes(source.read(16))
else:
    tracks = json.loads((base / 'music-layout.json').read_text())
    original = root / '[th10] 东方风神录 (汉化版+日文版)/thbgm.dat'
reports = []
with original.open('rb') as source:
    for i, track in enumerate(tracks):
        source.seek(track['offset'])
        pcm = source.read(track['length'])
        samples = np.frombuffer(pcm, dtype='<i2').reshape(-1, 2)
        path = out / (pathlib.Path(track['name']).stem+'.ogg' if game=='th08' else f'{i:02d}.flac')
        valid = path.exists() and sf.info(str(path)).frames == len(samples)
        if not valid:
            if game == 'th08':
                # Bounded float writes avoid libsndfile's large int16 Vorbis
                # conversion buffer and leave an incomplete file unpublished.
                pending = path.with_suffix('.ogg.tmp')
                with sf.SoundFile(str(pending), 'w', samplerate=44100, channels=2,
                                  subtype='VORBIS', format='OGG', compression_level=0.4) as encoded:
                    for start in range(0, len(samples), 16384):
                        encoded.write(samples[start:start+16384].astype(np.float32)/32768.0)
                pending.replace(path)
            else:
                sf.write(str(path), samples, 44100, subtype='PCM_16', format='FLAC')
        decoded, rate = sf.read(str(path), dtype='int16', always_2d=True)
        assert rate == 44100 and decoded.shape == samples.shape, f'Track {i} sample count differs'
        if game == 'th10':
            assert decoded.tobytes() == pcm, f'Track {i} differs'
        assert 0 <= track['loop'] < len(pcm) and track['loop'] % 4 == 0
        reports.append({'track': i, 'file': path.name, 'codec': 'vorbis' if game=='th08' else 'flac',
                        'lossless': game=='th10', 'sampleRate': rate, 'channels': 2,
                        'frames': len(samples), 'loopFrame': track['loop']//4,
                        'pcmBytes': len(pcm), 'encodedBytes': path.stat().st_size,
                        'pcmSha256': hashlib.sha256(pcm).hexdigest(),
                        'encodedSha256': hashlib.sha256(path.read_bytes()).hexdigest()})
        print(f'{i+1}/{len(tracks)}: {len(pcm)} -> {path.stat().st_size} bytes, '+('sample count / loop preserved' if game=='th08' else 'PCM identical'), flush=True)
(base / 'music-verification.json').write_text(json.dumps(reports, indent=2))
