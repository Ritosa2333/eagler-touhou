"""Actual ECL273 detached Anchor trail binding, operand and RNG order."""
import json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
HELPER=ROOT/'scripts/reverse/sample-laser-phase-clear.py'
exec(compile(HELPER.read_text(encoding='utf-8').split('records=[]')[0],str(HELPER),'exec'))
records=[]
for name,angle,flags,mask in [('straight',0,0,0),('rotated',1.25,0,0),
 ('random-heading',-9999,0,4),('hidden-skip-random',-9999,0x2000000,4)]:
 saved=sys.argv;sys.argv=[str(PROVIDER),'--frames','1','--seed','4660']
 ns={'__file__':str(PROVIDER),'__name__':'native_ecl_273_effect'}
 try:exec(compile(BOOTSTRAP,str(PROVIDER),'exec'),ns)
 finally:sys.argv=saved
 v,put,get,call=(ns[n] for n in ['v','put','get','call'])
 resource=ns['install_resource'](v,(ROOT/'local/retail/stgenm04.anm').read_bytes(),ns['resource_base'],2)
 put(ns['MANAGER']+0x48,'I',resource['resource'])
 effect=bytearray(ins(273,[word(2),word(36),floating(angle)]));struct.pack_into('<H',effect,8,mask)
 raw,header=program([ins(300,[floating(25),floating(100)]),ins(402,[word(flags)]),bytes(effect),ins(83,[word(100000)])])
 base=0x4300000;v.mem_write(base,raw);ns['routines']['ClearProbe']=(base+40,base+header)
 put(ns['PROGRAM']+8,'I',len(ns['routines']))
 for i,(_, (text,routine)) in enumerate(sorted(ns['routines'].items())):put(ns['PROGRAM']+0x100+i*8,'II',text,routine)
 req=0x2070000;put(req,'fffiiiII',0,0,0,0,0,10000,0,0);call(0x412990,base+40,req)
 def capture(tick):
  nodes=[];p=get(0x5000000+0x8856b8,'I')[0]
  while p:
   vm,p=get(p,'II');nodes.append({'bank':get(vm+0x3e6,'h')[0],'script':get(vm+0x3ea,'h')[0],
    'sprite':get(vm+0x3e4,'h')[0],'layer':get(vm+0x20,'i')[0],'clock':get(vm+0x6c,'i')[0],
    'flags':get(vm+0x47c,'I')[0],'rotation':list(get(vm+0x24,'3f')),
    'scale':list(get(vm+0x40,'2f')),'engine':list(get(vm+0x430,'3f'))})
  return {'inputTick':tick,'nodes':nodes,'rng':list(get(0x4ce568,'II'))}
 frames=[capture(-1)]
 from unicorn.x86_const import UC_X86_REG_EDI
 for tick in range(12):
  v.reg_write(UC_X86_REG_EDI,0x5000000);call(0x460fe0);frames.append(capture(tick))
 records.append({'name':name,'syntheticEclHex':raw.hex(),'frames':frames})
out=ROOT/'tests/fixtures/ecl-273-effect-original.json'
out.write_text(json.dumps({'schema':'th12-original-ecl-273-effect/1','originalExeSha256':ns['SHA'],
 'wholeGameOracle':False,'gpuExecuted':False,
 'scope':'Actual412990/ECL273 dispatcher, primitive factory/ANM bind andPrimary27. Originalstgenm04 bank2, literal/reference angle, hidden flag2000000. Constructed resource/heap host, no Player/GPU callback. Original RNG/operand evaluation is not substituted.',
 'records':records},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(out),'summary':[(r['name'],r['frames'][0]) for r in records]}))
