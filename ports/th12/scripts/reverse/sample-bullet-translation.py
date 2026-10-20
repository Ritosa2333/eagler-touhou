"""Actual8000000 dispatch,40c3b0 translation and full4099e0 motion."""
import pathlib,json,hashlib,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
exec(compile((ROOT/'scripts/reverse/sample-etex-boundaries.py').read_text().split('native = Native()')[0],'translation_host','exec'))
class TranslationNative(Native):
 def snapshot(self):
  return {k:v for k,v in super().snapshot().items() if k in ['position','velocity','speed','angle','active','cursor']}|{
   'translation':self.get(BULLET+0x90c,'i')[0:1]+self.get(BULLET+0x91c,'5f')+self.get(BULLET+0x930,'i')[0:1]}
native=TranslationNative();records=[]
for angle,speed in [(1.570796,.5),(-.8,.6),(2.4,.55),(0,0)]:
 for duration in [1,60000]:
  commands=[record(0x8000000,duration,a=angle,b=speed)]
  native.seed(commands,position=(12,120),angle=.3,speed=1.8)
  native.call(0x40aa60,registers={UC_X86_REG_ECX:BULLET})
  frames=[native.snapshot()]
  for tick in range(12):native.call(0x4099e0,(BULLET,));frames.append(native.snapshot())
  records.append({'angle':angle,'speed':speed,'duration':duration,'polar':False,'frames':frames})
# The retail pair: translation plus concurrent polar rotation, clockwise and
# anticlockwise. Full4099e0 owns the order and ordinary integration.
for angle in [1.570796,-.8,2.4]:
 for turn in [-.03141593/4.8,.03141593/4.8]:
  commands=[record(0x8000000,60000,a=angle,b=.6),record(8,60000,concurrent=True,a=0,b=turn)]
  native.seed(commands,position=(12,120),angle=.3,speed=1.8)
  native.call(0x40aa60,registers={UC_X86_REG_ECX:BULLET});frames=[native.snapshot()]
  for tick in range(12):native.call(0x4099e0,(BULLET,));frames.append(native.snapshot())
  records.append({'angle':angle,'speed':.6,'duration':60000,'polar':True,'turn':turn,'frames':frames})
callbacks=[]
for rate in [.5,1.,1.5]:
 native.seed([record(0x8000000,60000,a=-.8,b=.6)],position=(12,120),angle=.3,speed=1.8,rate=rate)
 native.call(0x40aa60,registers={UC_X86_REG_ECX:BULLET})
 for elapsed in [0,59999,60000]:
  native.put(BULLET+0x90c,'i',elapsed);native.put(BULLET+0x528,'I',0x800000a)
  before=native.snapshot();returned=native.call(0x40c3b0,registers={UC_X86_REG_EAX:BULLET})
  callbacks.append({'rate':rate,'elapsed':elapsed,'before':before,'after':native.snapshot(),'returned':returned})
out=ROOT/'tests/fixtures/bullet-translation-original.json'
out.write_text(json.dumps({'originalExeSha256':EXE_SHA256,'samplerSha256':hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),
 'wholeGameOracle':False,'scope':'Actual40aa60 and4099e0 with controlled alive bullet/records, 455630 ANM tail and audio as explicit boundaries. Polar plus translation order runs original CPU. Separate direct40c3b0 cases cover rate and exceptional elapsed input without claiming full-dispatch termination. No replay/golden edits.',
 'records':records,'callbacks':callbacks},indent=2)+'\n')
print(json.dumps({'cases':len(records),'frames':sum(len(r['frames']) for r in records),'callbacks':len(callbacks)}))
