import concurrent.futures,hashlib,json,time,urllib.request
from pathlib import Path
ops=Path(__file__).resolve().parent
plan=json.loads((ops/'input.json').read_text())
receipt=json.loads((ops/'published.json').read_text())
release=Path(receipt['candidate'])
inventory={row['path']:row for row in json.loads((release/'deployment.json').read_text())['files']}
paths=list(dict.fromkeys([row['path'] for row in plan['files']]+['app-shell-sw.js','release-manifest.json']))
def check(path):
    data=(release/path).read_bytes()
    expected=inventory.get(path,{'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest()})
    for attempt in range(3):
        try:
            request=urllib.request.Request('https://test.touhou.vip/'+path,headers={'Accept-Encoding':'identity','Cache-Control':'no-cache'})
            with urllib.request.urlopen(request,timeout=40) as response:
                body=response.read();mime=response.headers.get_content_type()
            assert len(body)==expected['bytes'] and hashlib.sha256(body).hexdigest()==expected['sha256'],path
            if path.endswith('.wasm'):assert mime=='application/wasm',(path,mime)
            return {'path':path,'bytes':len(body),'sha256':expected['sha256'],'contentType':mime}
        except Exception:
            if attempt==2:raise
            time.sleep(1)
with concurrent.futures.ThreadPoolExecutor(max_workers=5) as executor:results=list(executor.map(check,paths))
legacy=urllib.request.Request('https://test.touhou.vip/th05',method='HEAD')
with urllib.request.urlopen(legacy,timeout=30) as response:assert response.url=='https://test.touhou.vip/th05/lan.html?network=public'
report={'passed':True,'files':len(results),'bytes':sum(row['bytes'] for row in results),'releaseId':receipt['releaseId'],'legacyEntry':'https://test.touhou.vip/th05/lan.html?network=public','results':results}
(ops/'public-files.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({key:value for key,value in report.items() if key!='results'}))
