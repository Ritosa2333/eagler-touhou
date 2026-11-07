import sys,struct,json,re,os
from pathlib import Path
sys.path.insert(0,os.environ['TH19_DAT_TOOLS'])
from unpack_dat import ThDat,keyset_for
root=Path.cwd();d=ThDat(os.environ['TH19_ORIGINAL_DAT']);inv=json.loads((root/'work/th19-shot-hit-inventory.json').read_text());cpp=(root/'work/eagler-touhou/ports/th19/web/cpp/game/AnmVm.cpp').read_text(encoding='utf-8');supported={int(v) for v in re.findall(r'case\s+(\d+)\s*:',cpp)};supported.add(65535)
def scripts(b):
 base=0;out=[]
 while True:
  ns,nt=struct.unpack_from('<HH',b,base+4)
  for i in range(nt):
   sid,off=struct.unpack_from('<iI',b,base+64+ns*4+i*8);pos=base+off;ops=[]
   while pos+8<=len(b):
    op,n,t,m=struct.unpack_from('<HHhH',b,pos);args=list(struct.unpack_from('<'+'i'*((n-8)//4),b,pos+8)) if n>=8 else []
    ops.append({'op':op,'time':t,'args':args,'mask':m})
    if op==65535 or n<8:break
    pos+=n
   out.append({'id':sid,'ops':ops})
  nxt=struct.unpack_from('<I',b,base+36)[0]
  if not nxt:break
  base+=nxt
 return out
rows=[]
for ch in range(19):
 name=f'pl{ch:02}.anm';b=d.read_entry(d.find(name),keyset_for(19));ss=scripts(b);byid={s['id']:s for s in ss};tuples=sorted(set(tuple(t) for k,ls in inv[ch]['groups'].items() if int(k)<10 for t in ls));row={'character':ch,'file':name,'shots':[]}
 for init,update,hit,script,hit_script in tuples:
  seen=set();ops=set()
  def visit(s):
   if s['id'] in seen:return
   seen.add(s['id'])
   for op in s['ops']:
    ops.add(op['op'])
    if op['op'] in [500,501,502,504,505,510] and op['args'] and not(op['mask']&1):
     target=byid.get(op['args'][0])
     if target:visit(target)
  visit(ss[script])
  row['shots'].append({'init':init,'update':update,'hit':hit,'script':script,'hit_script':hit_script,'labels':[o['args'][0] for o in ss[script]['ops'] if o['op']==5],'descendants':sorted(seen),'unsupportedOpcodes':sorted(ops-supported),'ops':ss[script]['ops']})
 rows.append(row)
 print(ch,[(s['script'],s['hit'],s['labels'],s['unsupportedOpcodes']) for s in row['shots']],flush=True)
out=root/'outputs/th19-all-shots-20261106';out.mkdir(exist_ok=True);(out/'native-audit.json').write_text(json.dumps(rows,ensure_ascii=False,indent=2),encoding='utf-8')
