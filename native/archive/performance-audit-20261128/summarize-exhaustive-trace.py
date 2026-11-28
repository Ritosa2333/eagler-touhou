import collections,gzip,json,statistics
from pathlib import Path
root=Path(__file__).parent
data=json.loads((root/'exhaustive.json').read_bytes())
timeline=data.get('sort_timeline',{}).get('traceEvents',[])
threads={};stack=collections.defaultdict(list);spans=[];groups=collections.defaultdict(list)
for e in timeline:
    key=(e.get('pid'),e.get('tid'))
    if e.get('ph')=='M' and e.get('name')=='thread_name':threads[key]=e.get('args',{}).get('name')
    if e.get('ph')=='B':stack[key].append(e)
    elif e.get('ph')=='E' and stack[key]:
        start=stack[key].pop();duration=e['ts']-start['ts'];name=start['name']
        spans.append((duration,name,key));groups[(name,key)].append(duration)
    elif e.get('ph')=='X':
        duration=e.get('dur',0);name=e['name'];spans.append((duration,name,key));groups[(name,key)].append(duration)
def row(duration,name,key):return {'ms':round(duration/1000,3),'name':name,'thread':threads.get(key,str(key))}
result={'events':len(timeline),'earliestUs':min((e.get('ts',float('inf')) for e in timeline),default=None),'latestUs':max((e.get('ts',0) for e in timeline),default=None),'longest':[row(*s) for s in sorted(spans,reverse=True)[:35]],'groups':[
    {'name':name,'thread':threads.get(key,str(key)),'count':len(v),'totalInclusiveMs':round(sum(v)/1000,3),'maxMs':round(max(v)/1000,3),'medianMs':round(statistics.median(v)/1000,3)}
    for (name,key),v in sorted(groups.items(),key=lambda x:sum(x[1]),reverse=True)[:70]],'note':'Full Skia traced Profile | trace allocation affects timings and memory | intervals inclusive and overlap; do not sum totals.'}
(root/'sort-trace-summary.json').write_text(json.dumps(result,indent=2))
with gzip.open(root/'sort-full-trace.json.gz','wb') as out:out.write(json.dumps(data.get('sort_timeline',{}),separators=(',',':')).encode())
print(json.dumps({'events':result['events'],'spanSeconds':(result['latestUs']-result['earliestUs'])/1e6,'longest':result['longest'][:12]},indent=2))
