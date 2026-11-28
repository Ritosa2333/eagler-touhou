import json,re,time,urllib.request,urllib.parse
from pathlib import Path
root=Path(__file__).parent
base=re.findall(r'Connecting to Flutter application at (http://\S+/)',(root/'exhaustive.log').read_text())[-1]
opener=urllib.request.build_opener(urllib.request.ProxyHandler({}))
def rpc(method,**params):
    return json.load(opener.open(base+method+'?'+urllib.parse.urlencode(params),timeout=3))['result']
rows=[]
for _ in range(40):
    try:
        phase=json.loads((root/'exhaustive-live.json').read_text()).get('phase')
        vm=rpc('getVM')
        memory=[{'name':isolate['name'],**rpc('getMemoryUsage',isolateId=isolate['id'])} for isolate in vm['isolates']]
        rows.append({'at':time.time(),'phase':phase,'isolates':memory})
    except Exception as exc:
        rows.append({'at':time.time(),'error':str(type(exc).__name__)})
    (root/'heap-samples.json').write_text(json.dumps({'mode':'Profile VM service | no forced GC | whole-isolate aggregate','samples':rows},indent=2))
    time.sleep(2)
