"""Actual ECL1 disposition in main and asynchronous contexts."""
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
records=[]
for kind in ['main-delete','main-return','async-delete','async-return']:
 saved=sys.argv;sys.argv=[str(PROVIDER),'--frames','1','--seed','1234']
 ns={'__file__':str(PROVIDER),'__name__':'native_ecl_context_delete'}
 try:exec(compile(BOOTSTRAP,str(PROVIDER),'exec'),ns)
 finally:sys.argv=saved
 v,put,get,call=(ns[n] for n in ['v','put','get','call'])
 bodies=[('Probe',[ins(1 if kind=='main-delete' else 10,time=1)])] if kind.startswith('main-') else [
  ('Probe',[ins(15,[string('Worker')]),ins(83,[word(100000)])]),
  ('Worker',[ins(1 if kind=='async-delete' else 10,time=1)])]
 raw,entries=program(bodies);base=0x4300000;v.mem_write(base,raw)
 for name,(text,header) in entries.items():ns['routines'][name]=(base+text,base+header)
 put(ns['PROGRAM']+8,'I',len(ns['routines']))
 for i,(_, (text,routine)) in enumerate(sorted(ns['routines'].items())):
  put(ns['PROGRAM']+0x100+i*8,'II',text,routine)
 def capture():
  rows=[];node=get(ns['MANAGER']+0x68,'I')[0]
  while node:
   enemy,next_node=get(node,'II');threads=[];thread=enemy+0x1030
   while thread:
    ctx=get(thread,'I')[0];pc=get(ctx+4,'I')[0]
    match=next(((name,pc-(base+header+16)) for name,(_,header) in entries.items()
     if base+header+16<=pc<base+len(raw)),None)
    # Use the last header preceding the current instruction; SCPT routines
    # are contiguous in the explicitly installed synthetic resource.
    candidates=[(header,name) for name,(_,header) in entries.items() if base+header+16<=pc]
    if candidates:
     header,name=max(candidates);match=(name,pc-(base+header+16))
    threads.append({'routine':match[0] if match else '', 'offset':match[1] if match else -1,
     'time':get(ctx,'f')[0]});thread=get(thread+4,'I')[0]
   rows.append({'id':get(enemy+0x27b4,'I')[0],'contexts':threads});node=next_node
  return rows
 req=0x2070000;put(req,'fffiiiII',25,100,0,0,0,10000,0,0)
 call(0x412990,base+entries['Probe'][0],req)
 samples=[{'inputTick':-1,'enemies':capture()}]
 for tick in range(5):
  ns['frame']=tick;call(0x413190,ns['MANAGER'])
  samples.append({'inputTick':tick,'enemies':capture()})
 records.append({'kind':kind,'syntheticEclHex':raw.hex(),'frames':samples})
out=ROOT/'tests/fixtures/ecl-context-delete-original.json'
out.write_text(json.dumps({'schema':'th12-original-ecl-context-delete/1',
 'originalExeSha256':ns['SHA'],'wholeGameOracle':False,
 'scope':'Actual Enemy412990/Enemy18 and ECL1/10/15 dispatch, using synthetic SCPT in research memory. No Player/ANM scheduler/GPU execution. No opcode disposition/manager deletion is intercepted.',
 'records':records},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(out),'summary':[(r['kind'],[(f['inputTick'],len(f['enemies'])) for f in r['frames']]) for r in records]}))
