"""Record the original PMD Music Room key/measure display for Ogg playback."""
from pathlib import Path
import gzip
import argparse,concurrent.futures,hashlib,importlib.util,json,re,struct,subprocess
p=argparse.ArgumentParser();p.add_argument('--archive',type=Path,required=True);p.add_argument('--dll',type=Path,required=True);p.add_argument('--renderer',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
root=Path(__file__).resolve().parents[1];spec=importlib.util.spec_from_file_location('artwork',root/'scripts/prepare-artwork.py');module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
rows=re.search(r'musicFiles\[5\]\[28\]=\{(.*?)\n\};',(root/'Game/MusicRoom/MusicTracks.h').read_text(encoding='utf-8'),re.S)[1];names=re.findall(r'"([A-Z0-9_]+)"',rows)
a.output.mkdir(parents=True,exist_ok=True)
def render(name):
    source=a.output/(name+'.M2');pcm=a.output/(name+'.pcm');notes=a.output/(name+'.notes')
    try:payload=module.archive_entry(a.archive,name+'.M2')
    except ValueError:payload=module.archive_entry(a.archive,name+'.M')
    source.write_bytes(payload)
    if not notes.exists():subprocess.run([str(a.renderer.resolve()),str(a.dll.resolve()),str(source.resolve()),str(pcm.resolve()),str(notes.resolve())],capture_output=True,check=True,timeout=120)
    return name.lower(),notes.read_bytes()
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:tracks=list(pool.map(render,names))
offset=0;index={};payload=bytearray()
for name,notes in tracks:
    assert len(notes)%14==0
    index[name]=[offset,len(notes)//14];offset+=len(notes);payload+=notes
header=json.dumps(index,separators=(',',':')).encode();output=a.output/'music-notes.bin.gz';output.write_bytes(gzip.compress(struct.pack('<I',len(header))+header+payload,compresslevel=9,mtime=0))
report={'schema':'eagler-touhou/th05-music-notes/1','source':'original MUSIC.DAT / PMDWin getpartwork and getopenwork','sourceSha256':hashlib.sha256(a.archive.read_bytes()).hexdigest(),'tracks':len(tracks),'bytes':output.stat().st_size,'sha256':hashlib.sha256(output.read_bytes()).hexdigest()}
(root/'resources/audio/music-notes.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8');print(json.dumps(report))
