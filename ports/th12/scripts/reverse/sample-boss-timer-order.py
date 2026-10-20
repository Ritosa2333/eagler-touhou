"""Actual Enemy18 countdown writes and clock tail, using controlled SCPT."""
import json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
PROVIDER=ROOT/'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP=PROVIDER.read_text(encoding='utf-8').split('if args.routine not in routines:')[0]
def word(n):return struct.pack('<i',n)
def ins(op,args=(),time=0):
 raw=b''.join(args);return struct.pack('<iHHHBBI',time,op,len(raw)+16,0,63,len(args),0)+raw
def string(s):
 raw=s.encode()+b'\0';return word(len(raw))+raw+bytes((-len(raw))%4)
def program(bodies):
 names=b''.join(name.encode()+b'\0' for name,_ in bodies);start=(36+4*len(bodies)+len(names)+3)&~3
 data=bytearray(start);data[:4]=b'SCPT';struct.pack_into('<H',data,4,1);struct.pack_into('<H',data,16,len(bodies))
 data[36+4*len(bodies):36+4*len(bodies)+len(names)]=names;entries={};name_at=36+4*len(bodies)
 for i,(name,code) in enumerate(bodies):
  header=len(data);struct.pack_into('<I',data,36+i*4,header);entries[name]=(name_at,header)
  name_at+=len(name)+1;data+=b'ECLH'+bytes(12)+b''.join(code)
 return bytes(data),entries
rows=[]
for kind,limit in [('short-timeout',2),('normal-timeout',60),('remove-timer',60)]:
 saved=sys.argv;sys.argv=[str(PROVIDER),'--frames','1','--seed','1234']
 ns={'__file__':str(PROVIDER),'__name__':'native_boss_timer_order'}
 try:exec(compile(BOOTSTRAP,str(PROVIDER),'exec'),ns)
 finally:sys.argv=saved
 v,put,get,call=(ns[n] for n in ['v','put','get','call'])
 from unicorn.x86_const import UC_X86_REG_EDI
 setup=[ins(414,[word(0),word(0),word(limit),string('End')])]
 if kind=='remove-timer':setup+=[ins(414,[word(0),word(-1),word(0),string('End')],time=1)]
 setup+=[ins(83,[word(100000)])]
 raw,entries=program([('main',setup),('End',[ins(83,[word(100000)])])]);base=0x4300000;v.mem_write(base,raw)
 for name,(text,header) in entries.items():ns['routines'][name]=(base+text,base+header)
 put(ns['PROGRAM']+8,'I',len(ns['routines']))
 for i,(_, (text,routine)) in enumerate(sorted(ns['routines'].items())):put(ns['PROGRAM']+0x100+i*8,'II',text,routine)
 put(ns['STAGE']+0x6d38,'2i',-1,0)
 req=0x2070000;put(req,'fffiiiII',25,100,0,0,0,10000,0,0)
 enemy=call(0x412990,base+entries['main'][0],req)
 def snapshot(tick):return {'inputTick':tick,'phaseAge':get(enemy+0x12b0,'i')[0],'flags':get(enemy+0x26f8,'I')[0],'timer':get(ns['STAGE']+0x6d38,'2i')}
 frames=[snapshot(-1)]
 for tick in range((4 if limit==2 else 63)):
  ns['frame']=tick;call(0x413190,ns['MANAGER']);frames.append(snapshot(tick))
 rows.append({'kind':kind,'syntheticEclHex':raw.hex(),'frames':frames})
out=ROOT/'tests/fixtures/boss-timer-order-original.json'
out.write_text(json.dumps({'schema':'th12-original-boss-timer-order/1','originalExeSha256':ns['SHA'],'wholeGameOracle':False,'gpuExecuted':False,
 'scope':'Original412990 birth,413190 Enemy18 update,41a6f0 interrupt/countdown and414a65 phase-clock tail. Controlled SCPT and synthetic valid HUD fields; no Player, whole application scheduling, HUD25 or GPU execution.',
 'records':rows},indent=2)+'\n',encoding='utf8')
print(json.dumps({'output':str(out),'initial':[(r['kind'],r['frames'][:4]) for r in rows],'tail':[(r['kind'],r['frames'][-4:]) for r in rows]}))
