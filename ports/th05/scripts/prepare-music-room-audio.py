"""Prepare the 84 original MUSIC.DAT arrangements missing from PR #143's TH05 set."""
from pathlib import Path
import argparse,concurrent.futures,hashlib,importlib.util,json,re,subprocess
p=argparse.ArgumentParser();p.add_argument('--archive',type=Path,required=True);p.add_argument('--dll',type=Path,required=True);p.add_argument('--renderer',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--ffmpeg',default='ffmpeg');a=p.parse_args()
root=Path(__file__).resolve().parent.parent
spec=importlib.util.spec_from_file_location('artwork',root/'scripts/prepare-artwork.py');module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
rows=re.search(r'musicFiles\[5\]\[28\]=\{(.*?)\n\};',(root/'Game/MusicRoom/MusicTracks.h').read_text(encoding='utf-8'),re.S)[1]
names=re.findall(r'"([A-Z0-9_]+)"',rows)[:84]
assert len(names)==84
a.output.mkdir(parents=True,exist_ok=True);work=a.output/'source';work.mkdir(exist_ok=True)
for name in names:
    try:payload=module.archive_entry(a.archive,name+'.M2')
    except ValueError:payload=module.archive_entry(a.archive,name+'.M')
    (work/(name+'.M2')).write_bytes(payload)
def render(name):
    source=work/(name+'.M2');raw=work/(name+'.pcm');output=a.output/('room-'+name.lower()+'.ogg');receipt=work/(name+'.json')
    if not receipt.exists() or not output.exists():
        result=subprocess.run([str(a.renderer.resolve()),str(a.dll.resolve()),str(source.resolve()),str(raw.resolve())],capture_output=True,text=True,check=True,timeout=120)
        timing=json.loads(result.stdout)
        subprocess.run([a.ffmpeg,'-hide_banner','-loglevel','error','-y','-f','s16le','-ar','44100','-ac','2','-i',str(raw),'-c:a','libopus','-b:a','96k',str(output)],check=True)
        receipt.write_text(json.dumps(timing),encoding='utf-8')
    timing=json.loads(receipt.read_text());audio=output.read_bytes()
    return name.lower(),{**timing,'url':output.name,'bytes':len(audio),'audioSha256':hashlib.sha256(audio).hexdigest(),'sourceSha256':hashlib.sha256(source.read_bytes()).hexdigest()}
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:tracks=dict(executor.map(render,names))
manifest={'version':1,'source':'TH05 original MUSIC.DAT / TH01-04 Arrange and Special MIX','archiveSha256':hashlib.sha256(a.archive.read_bytes()).hexdigest(),'rendererDllSha256':hashlib.sha256(a.dll.read_bytes()).hexdigest(),'tracks':tracks}
(a.output/'manifest.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print('Prepared original Music Room tracks:',len(tracks))
