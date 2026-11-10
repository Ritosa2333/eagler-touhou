"""Original4099e0/464a80 fractional-rate movement; ANM/audio boundary only."""
import pathlib,json,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2]
exec(compile((ROOT/'scripts/reverse/sample-etex-boundaries.py').read_text().split('native = Native()')[0],'rate_host','exec'))
native=Native();rows=[]
programs=[[],[record(1)],[record(4,4,a=.1,b=.7)],[record(8,4,a=-.01,b=.05)],
 [record(0x20000000,4,a=2.5,b=-999)],
 [record(0x10,4,2,a=.2,b=1.8)],[record(0x20,4,2,a=.2,b=1.8)],[record(0x40,4,2,a=.2,b=1.8)],
 [record(0x400,3),record(4,4,a=.1,b=.7)],[record(0x1000,3),record(8,4,a=0,b=.05)],
 [record(0x2000000,4,9,a=32,b=128)],
 [record(0x8000000,60000,a=-.8,b=.6),record(8,60000,concurrent=True,a=0,b=.03)]]
def snapshot():
 s=native.snapshot()
 return {k:s[k] for k in ['position','velocity','speed','angle','active','cursor']}|{'phase':native.get(BULLET+0x532,'H')[0],'age':native.get(BULLET+0x4e8,'i')[0]}
for rate in [.25,.5,1]:
 for phase,commands in [(1,p) for p in programs]+[(2,[]),(3,[])]:
  native.seed(commands,position=(12,120),angle=.3,speed=1.8,rate=rate)
  native.put(BULLET+0x532,'H',phase);native.put(BULLET+0x9f4,'H',3)
  native.put(BULLET+0x4e4,'iifII',-1,0,0,0x4b2ed0,1)
  native.call(0x40aa60,registers={UC_X86_REG_ECX:BULLET})
  frames=[snapshot()]
  for tick in range(40):
   native.call(0x4099e0,(BULLET,))
   # The actual Bullet21 manager performs this timer tail after4099e0.
   native.call(0x464a80,registers={UC_X86_REG_ESI:BULLET+0x4e4})
   frames.append(snapshot())
  rows.append({'rate':rate,'phase':phase,'records':commands,'frames':frames})
output={'originalExeSha256':EXE_SHA256,'samplerSha256':hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),'wholeGameOracle':False,'scope':'Controlled original bullet dispatch/motion/lifetime at rates1/.5/.25. ANM stays alive and sound is an explicit output boundary; collision disabled. Not full replay/GPU. Existing goldens and tolerances unchanged.','rows':rows}
(ROOT/'tests/fixtures/bullet-game-rate-original.json').write_text(json.dumps(output,indent=2)+'\n')
print(json.dumps({'cases':len(rows),'snapshots':sum(len(r['frames']) for r in rows)}))
