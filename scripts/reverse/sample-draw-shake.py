"""Observe which retail CPU draw paths apply the ANM camera screen offset."""
import json,pathlib,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'scripts/reverse'))
source=ROOT/'scripts/reverse/sample-custom-geometry.py'
scope={'__file__':str(source),'__name__':'native_draw_shake'}
exec(source.read_text(encoding='utf8').split('\nrecords=[]',1)[0],scope)
Native=scope['Native'];EAX=scope['UC_X86_REG_EAX'];ECX=scope['UC_X86_REG_ECX'];EDI=scope['UC_X86_REG_EDI']
records=[]
def capture(n,name,draw):
 n.put(0x4000000+0xb0,'2f',0,0);n.draws=[];draw();before=n.draws.copy()
 n.put(0x4000000+0xb0,'2f',3.25,-2.5);n.draws=[];draw()
 records.append({'name':name,'offset':[3.25,-2.5],'before':before,'after':n.draws.copy()})
for script in [125,126,206]:
 n=Native();vm=0x2100000;n.call(0x454d10,[vm,script],{ECX:n.resource['resource']})
 n.put(vm+0x430,'3f',224,160,0)
 for _ in range(34):n.call(0x455630,[vm])
 n.label=f'anm{script}'
 capture(n,n.label,lambda:n.call(0x45c900,[0x4000000],{EAX:vm}))
profiles=json.loads((ROOT/'artifacts/reverse/combat/native-probe.json').read_text())['laserProfiles']
for row in profiles:
 kind=row['factory']['kind'];n=Native();lm,player,sht,params=0x2000000,0x2010000,0x2020000,0x2040000
 n.put(0x4b4514,'I',player);n.put(0x4b43dc,'I',0x2030000);n.put(player+0xa28,'i',1);n.put(player+0xa2c,'I',sht);n.put(sht+4,'f',2)
 n.put(player+0x97c,'3f',5000,5000,0);n.put(player+0x9cc,'6f',4999,4999,0,5001,5001,0)
 n.call(0x427fa0,regs={EDI:lm});n.put(lm+0x464,'I',lm+0x10);n.put(lm+0x488,'I',n.resource['resource'])
 n.put(0x4b43d4,'I',0x2060000);n.put(0x2060010,'I',n.resource['resource'])
 n.mu.mem_write(params,bytes.fromhex(row['factory']['profileHex']));n.call(0x428450,[kind],{EDI:params})
 obj=n.get(lm+0x18,'I')[0]
 for _ in range(8):n.call(0x428270,regs={EDI:lm})
 n.label=f'laser{kind}';draw={0:0x429af0,1:0x42b100,2:0x42cca0}[kind]
 capture(n,n.label,lambda:n.call(draw,regs={ECX:obj}))
out=ROOT/'tests/fixtures/draw-shake-native.json'
out.write_text(json.dumps({'schema':'th12-native-draw-shake/1','originalExeSha256':scope['EXE_SHA256'],
 'wholeGameOracle':False,'gpuExecuted':False,
 'boundary':'Actual CPU vertex generation and quad batch copy; same GPU/resource host boundaries as sample-custom-geometry.py. Synthetic ANMManager+b0/b4 offsets isolate screen translation without changing game positions.',
 'records':records},indent=2)+'\n',encoding='utf8')
print(json.dumps({'cases':len(records),'output':str(out),'draws':[
 {'name':r['name'],'deltas':[[after['vertices'][0][i]-before['vertices'][0][i] for i in [0,1]]
  for before,after in zip(r['before'],r['after'])]} for r in records]}))
