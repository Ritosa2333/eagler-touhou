from pathlib import Path
import os,hashlib,json,ctypes,subprocess,time
p=Path('D:/Temp/native-performance-20261128/graphics-owned');baseline=Path('D:/Temp/native-performance-20261128/gpu-preference/preview');preview=p/'preview';preview.mkdir(exist_ok=True)
for f in baseline.rglob('*'):
 if f.is_file():
  target=preview/f.relative_to(baseline);target.parent.mkdir(parents=True,exist_ok=True)
  if f.name=='flutter_inappwebview_windows_plugin.dll':
   target.write_bytes((p/f.name).read_bytes())
  elif not target.exists():os.link(f,target)
sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest()
(p/'runtime-source.json').write_text(json.dumps({'aot':sha(preview/'data/app.so'),'host':sha(preview/'gpu-audit.exe'),'baselinePlugin':sha(baseline/'flutter_inappwebview_windows_plugin.dll'),'candidatePlugin':sha(preview/'flutter_inappwebview_windows_plugin.dll'),'engine':sha(preview/'flutter_windows.dll'),'scope':'Same nonactivating host, current ordinary main.dart AOT and Skia. Candidate plugin only changes two destructors. Per-manager graphics ownership, five header consumers rebuilt. No actual webview creation, not account-page acceptance.'},indent=2)+'\n')
k=ctypes.WinDLL('kernel32',use_last_error=True);k.WaitForSingleObject.argtypes=[ctypes.c_void_p,ctypes.c_ulong];k.GetExitCodeProcess.argtypes=[ctypes.c_void_p,ctypes.POINTER(ctypes.c_ulong)]
for i,mode in enumerate(['default','high','default','high','default','high'],1):
 events=p/f'events-{i}-{mode}.jsonl';result=p/f'result-{i}-{mode}.json';assert not events.exists() and not result.exists()
 env=dict(os.environ,TOUHOU_GPU_PREFERENCE=mode,TOUHOU_PROBE_LOG=str(events))
 with (p/f'engine-{i}-{mode}.log').open('wb') as log:
  child=subprocess.Popen([str(preview/'gpu-audit.exe')],cwd=preview,env=env,stdout=log,stderr=subprocess.STDOUT,creationflags=subprocess.CREATE_NO_WINDOW)
  print(json.dumps({'run':i,'pid':child.pid,'mode':mode}),flush=True)
  started=time.perf_counter();wait=258
  while time.perf_counter()-started<35:
   wait=k.WaitForSingleObject(int(child._handle),1000)
   if wait==0:break
  code=ctypes.c_ulong();k.GetExitCodeProcess(int(child._handle),ctypes.byref(code))
  outcome={'run':i,'mode':mode,'pid':child.pid,'elapsed':time.perf_counter()-started,'wait':wait,'exitCode':code.value,'forced':False}
  if wait!=0:
   child.terminate();outcome['forced']=True;outcome['finalWait']=k.WaitForSingleObject(int(child._handle),10000)
  result.write_text(json.dumps(outcome,indent=2)+'\n');print(json.dumps(outcome),flush=True)
  if wait!=0 or code.value!=0:raise SystemExit('Candidate failed; stopped')
