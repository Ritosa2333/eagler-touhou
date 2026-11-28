from pathlib import Path
import ctypes,subprocess,os,time,json,hashlib
p=Path(__file__).resolve().parent;events=p/'canonical-account-events.jsonl';assert not events.exists()
k=ctypes.WinDLL('kernel32',use_last_error=True);k.WaitForSingleObject.argtypes=[ctypes.c_void_p,ctypes.c_ulong];k.GetExitCodeProcess.argtypes=[ctypes.c_void_p,ctypes.POINTER(ctypes.c_ulong)]
with (p/'canonical-account-engine.log').open('wb') as log:
 child=subprocess.Popen([str(p/'canonical-preview/account-audit.exe'),'https://touhouonline.com/login.html?lang=zh-CN'],cwd=p/'canonical-preview',env=dict(os.environ,TOUHOU_GPU_PREFERENCE='high',TOUHOU_PROBE_LOG=str(events)),stdout=log,stderr=subprocess.STDOUT,creationflags=subprocess.CREATE_NO_WINDOW)
 print(json.dumps({'pid':child.pid,'scope':'Owned nonactivating45-second production-AOT login page'}),flush=True)
 result={'pid':child.pid,'url':'https://touhouonline.com/login.html?lang=zh-CN'}
 try:
  wait=k.WaitForSingleObject(int(child._handle),70000);code=ctypes.c_ulong();k.GetExitCodeProcess(int(child._handle),ctypes.byref(code));result.update(wait=wait,exitCode=code.value,forced=False)
 finally:
  if k.WaitForSingleObject(int(child._handle),0)!=0:
   child.terminate();result.update(forced=True,finalWait=k.WaitForSingleObject(int(child._handle),10000))
 (p/'canonical-account-result.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result),flush=True)
