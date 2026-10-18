"""Prepare original TH09 music as Vorbis with original PCM lengths and loops."""
from pathlib import Path
import hashlib, json, struct, sys, shutil
root=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(root/'tools/architecture/python'))
import numpy as np
import soundfile as sf
out=root/'th09_web/assets/sdl-native'
(out/'music').mkdir(parents=True,exist_ok=True)
for name in ['cp932.bin','blend.bin','msgothic.ttc']:
    shutil.copyfile(root/'th08_web/assets/sdl-native'/name,out/name)
formats=(root/'th09_web/reference/assets/thbgm.fmt').read_bytes()
report=[]
with (root/'[th09] 东方花映塚 (日文版)/thbgm.dat').open('rb') as source:
    assert source.read(4)==b'ZWAV'
    for at in range(0,len(formats)-51,52):
        if not formats[at]:break
        name,offset,preload,intro,length=struct.unpack_from('<16sIIII',formats,at)
        name=name.split(b'\0')[0].decode('ascii')
        assert struct.unpack_from('<HHIIHH',formats,at+32)==(1,2,44100,176400,4,16)
        source.seek(offset);pcm=source.read(length)
        assert len(pcm)==length and length%4==0 and 0<=intro<length and intro%4==0
        samples=np.frombuffer(pcm,dtype='<i2').reshape(-1,2)
        path=out/'music'/(Path(name).stem+'.ogg')
        if not path.exists() or sf.info(str(path)).frames!=len(samples):
            pending=path.with_suffix('.ogg.tmp')
            with sf.SoundFile(str(pending),'w',samplerate=44100,channels=2,subtype='VORBIS',format='OGG',compression_level=.4) as target:
                for start in range(0,len(samples),16384):target.write(samples[start:start+16384].astype(np.float32)/32768.)
            pending.replace(path)
        decoded,rate=sf.read(str(path),dtype='int16',always_2d=True)
        assert decoded.shape==samples.shape and rate==44100
        report.append(dict(name=name,file=path.name,frames=len(samples),loopFrame=intro//4,encodedBytes=path.stat().st_size,pcmSha256=hashlib.sha256(pcm).hexdigest(),encodedSha256=hashlib.sha256(path.read_bytes()).hexdigest(),codec='vorbis',lossless=False))
        print(f'{len(report)}/19 {name}: {len(samples)} PCM frames, loop at {intro//4}',flush=True)
assert len(report)==19
(out/'music-verification.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
