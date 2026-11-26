"""将浏览器 CPU 采样与同字节 WASM 的符号表合并。"""
from pathlib import Path
from collections import defaultdict
import argparse,json,re,subprocess

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--profiles',type=Path,required=True);parser.add_argument('--symbols',type=Path,required=True);parser.add_argument('--demangler',type=Path,required=True);args=parser.parse_args()
symbols={int(i):name for i,name in (line.split(':',1) for line in args.symbols.read_text(encoding='utf-8').splitlines())}
names=list(symbols.values());decoded=subprocess.check_output([str(args.demangler.resolve())],input='\n'.join(names)+'\n',text=True,encoding='utf-8').splitlines();symbols=dict(zip(symbols,decoded));reports=[]
for file in sorted(args.profiles.glob('*.cpuprofile')):
 profile=json.loads(file.read_text(encoding='utf-8'));nodes={node['id']:node for node in profile['nodes']};parents={child:node['id'] for node in nodes.values() for child in node.get('children',[])};own=defaultdict(float);inclusive=defaultdict(float)
 def label(node):
  frame=node['callFrame'];name=frame['functionName'];match=re.search(r'wasm-function\[(\d+)\]',name)
  return symbols.get(int(match[1]),name) if match else name or '(anonymous) '+frame.get('url','').split('/')[-1]
 for index,node_id in enumerate(profile.get('samples',[])):
  ms=profile['timeDeltas'][index]/1000;own[label(nodes[node_id])]+=ms
  seen=set()
  while node_id:
   name=label(nodes[node_id])
   if name not in seen:inclusive[name]+=ms;seen.add(name)
   node_id=parents.get(node_id)
 total=sum(own.values());rows=lambda values:[{'name':name,'ms':round(ms,3),'percent':round(ms/total*100,2)} for name,ms in sorted(values.items(),key=lambda x:x[1],reverse=True)[:35]]
 row={'profile':file.name,'totalMs':round(total,3),'self':rows(own),'inclusive':rows(inclusive)};reports.append(row)
 print(json.dumps({'profile':file.name,'self':row['self'][:12],'inclusive':row['inclusive'][:9]},ensure_ascii=False))
(args.profiles/'summary.json').write_text(json.dumps(reports,ensure_ascii=False,indent=2)+'\n',encoding='utf-8',newline='\n')
