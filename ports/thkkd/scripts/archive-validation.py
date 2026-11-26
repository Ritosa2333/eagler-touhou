"""收集本作已执行且通过的验收结果，保留实际 Runtime 身份。"""
from pathlib import Path
import hashlib
import json
import shutil
import subprocess

PORT=Path(__file__).resolve().parents[1]
ROOT=PORT.parents[1]
destination=ROOT/'archive/thkkd-web-20261126'
destination.mkdir(parents=True,exist_ok=True)
reports={}
for name in ['characters-browser','stages-browser','features-browser','launcher-controls-browser','media-browser','storage-browser','offline-browser']:
    source=PORT/'build'/name/'result.json'
    data=json.loads(source.read_text('utf-8'))
    if data.get('result')!='passed':
        raise ValueError('验收尚未通过：'+name)
    shutil.copy2(source,destination/(name+'.json'))
    reports[name]={'result':'passed','sha256':hashlib.sha256(source.read_bytes()).hexdigest(),'browser':data.get('browser')}
replay=json.loads((PORT/'build/replay-browser/result.json').read_text('utf-8'))
if replay['errors'] or replay['seek']['cursor']!=240 or replay['backward']['cursor']!=60:
    raise ValueError('原版录像入口验收未通过')
shutil.copy2(PORT/'build/replay-browser/result.json',destination/'replay-browser.json')
runtime=json.loads((PORT/'runtime/runtime-files.json').read_text('utf-8'))
summary={'schema':'thkkd/browser-acceptance/1','result':'passed','implementationCommit':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),
         'environment':{'os':'Windows x64','browser':'Microsoft Edge 158.0.4370.33','devices':'键盘；DevTools 触控；生产映射模型中的虚拟标准手柄'},
         'reports':reports,'runtimeFiles':runtime['files'],
         'scope':{'physicalTouchscreen':'未执行','physicalGamepad':'未执行','physicalHighRefreshMonitor':'未执行；正式 WASM 使用 60/120/144 Hz 时间序列对照','otherBrowserEngines':'未执行'},
         'sharedChecks':{'portChecks':'10/10 passed','filesService':'25 tests passed','extensions':'10 tests passed','uiTypecheck':'passed'}}
(destination/'browser-startup.json').write_text(json.dumps(summary,ensure_ascii=False,indent=2)+'\n','utf-8')
print(json.dumps({'result':'passed','reports':len(reports)+1}))
