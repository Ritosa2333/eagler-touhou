import collections, json, struct
from pathlib import Path
from PIL import Image

root=Path('C:/Users/Ritosa/AppData/Local/Packages/OpenAI.Codex_2p2nqsd0c76g0/LocalCache/Local/eagler-account-api-routing/native')
bundle=Path(__file__).parent/'preview'
files=[p for p in bundle.rglob('*') if p.is_file()]
groups=collections.defaultdict(lambda:{'files':0,'bytes':0})
for p in files:
    group='assets' if 'flutter_assets' in p.parts else p.suffix.lower() or 'other'
    groups[group]['files']+=1;groups[group]['bytes']+=p.stat().st_size
images=[]
for p in (root/'assets').rglob('*'):
    if p.suffix.lower() not in {'.png','.jpg','.jpeg','.webp'}:continue
    with Image.open(p) as im:
        images.append({'file':p.relative_to(root).as_posix(),'width':im.width,'height':im.height,'bytes':p.stat().st_size,'rgbaDecodedBytes':im.width*im.height*4})
covers=[x for x in images if '/book-covers/' in x['file']]
report={'bundleBytes':sum(p.stat().st_size for p in files),'groups':dict(groups),'largestFiles':[{'file':p.relative_to(bundle).as_posix(),'bytes':p.stat().st_size} for p in sorted(files,key=lambda p:p.stat().st_size,reverse=True)[:18]],'images':images,'coverCount':len(covers),'allCoversRgbaBytes':sum(x['rgbaDecodedBytes'] for x in covers),'coverMaxWidth':max(x['width'] for x in covers),'coverMaxHeight':max(x['height'] for x in covers),'note':'Disk file size and theoretical uncompressed RGBA size. Neither is resident process memory.'}
(Path(__file__).parent/'footprint.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps({k:v for k,v in report.items() if k not in ['images','largestFiles']},ensure_ascii=False))
