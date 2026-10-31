"""Actual439a40 shot clock/emission gate, with emission as output boundary."""
import pathlib,json,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2]
exec(compile((ROOT/'scripts/reverse/sample-etex-boundaries.py').read_text().split('native = Native()')[0],'shot_clock_host','exec'))
class ShotNative(Native):
 def __init__(self):super().__init__();self.emissions=[]
 def hook(self,mu,address,size,unused):
  if address==0x4399d0:
   self.emissions.append(mu.reg_read(UC_X86_REG_EDI));self.ret();return
  super().hook(mu,address,size,unused)
native=ShotNative();rows=[]
for rate in [1,.5,.25]:
 for release in [False,True]:
  native.mu.mem_write(BULLET,bytes(0xc600));native.put(0x4b2ed0,'f',rate)
  native.put(BULLET+0xa28,'i',1);native.put(BULLET+0xc420,'iifII',-2,-1,-1,0x4b2ed0,1)
  frames=[]
  for tick in range(80):
   held=not release or tick<20;native.put(0x4d49d0,'I',int(held));before=len(native.emissions)
   native.call(0x439a40,(BULLET,))
   frames.append({'held':held,'clock':native.get(BULLET+0xc424,'i')[0],'emissions':native.emissions[before:]})
  rows.append({'rate':rate,'release':release,'frames':frames})
out={'originalExeSha256':EXE_SHA256,'samplerSha256':hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),'wholeGameOracle':False,'scope':'Actual439a40/4067e0/464a20/464a80. Controlled normal Player state and held input.4399d0 emission recorded as output boundary, no bullets/RNG substituted for a score oracle. Not replay/GPU.','rows':rows}
(ROOT/'tests/fixtures/shot-game-rate-original.json').write_text(json.dumps(out,indent=2)+'\n')
print(json.dumps({'cases':len(rows),'snapshots':sum(len(r['frames']) for r in rows)}))
