"""Summarize private native Chrome observations without publishing images."""
from pathlib import Path
import hashlib,json
from PIL import Image,ImageChops
R=Path(__file__).resolve().parents[1]
O=R/'artifacts/th14/current-concurrent-mixer-nov14/launch-runtime-acceptance-nov17'
before=O/'native-running-before-valid-hide.png';after=O/'native-after-valid-hide-paused.png'
a=Image.open(before).convert('RGB');b=Image.open(after).convert('RGB');assert a.size==b.size==(1580,904)
# Declare whole number fields in the fixed native-window screenshots. Do not
# select matching pixels or normalize the images; report every changed pixel.
regions={'score':(1090,192,1307,231),'spare-hearts':(1105,251,1308,289),
         'bombs':(1105,320,1308,355),'power':(1160,448,1310,490),
         'point-value':(1165,490,1308,531),'graze':(1215,530,1307,567)}
rows=[]
for name,box in regions.items():
 x=ImageChops.difference(a.crop(box),b.crop(box))
 rows.append({'field':name,'box':box,'pixels':x.width*x.height,
              'differentPixels':sum(pixel!=(0,0,0) for pixel in x.get_flattened_data())})
text=(O/'native-probe-visibility-ax.txt').read_text(encoding='utf-8')
tail=text.rsplit('Document text:',1)[1];start=tail.index('[\n');end=tail.index('\n]',start)+2
events=json.loads(tail[start:end]);assert any(e['event']=='visibilitychange' and e['hidden'] for e in events)
assert events[-1]['event']=='visibilitychange' and not events[-1]['hidden']
q={'schema':'th14/native-hidden-observation/1','generation':'fdb227a602f0c5482ac78580da14ec77637fc2130bafab68ef8fdf8869cbe8d1',
   'timing':json.loads((O/'native-hidden-timing.json').read_bytes()),'hudImageComparisons':rows,
   'probeEventsFromEarlierControlInSameDebuggerDetachedWorkflow':events,
   'gameVisibilityReadDirectly':False,'wholeHiddenFrameCountMeasured':False,
   'heldKeyReleaseAccepted':False,'nativeReturnTitleAndQuitObserved':True,
   'invalidEarlierSampleBeforeHideAlreadyEnded':True,
   'scope':'ordinary native-window tab switch, retained HUD values, foreground pause, return title and native Quit',
   'privateImages':{p.name:{'bytes':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} for p in [before,after]},
   'imagesPublished':False,'audibleOutput':False,'completeGame':False}
(O/'native-hidden-observation.json').write_text(json.dumps(q,indent=2)+'\n')
print(json.dumps({'hiddenWallMs':q['timing']['hiddenWallMs'],'regions':rows,'wholeFrameCountMeasured':False,'completeGame':False}))
