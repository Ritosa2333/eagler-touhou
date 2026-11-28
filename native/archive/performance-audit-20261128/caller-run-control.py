from pathlib import Path
import ctypes,json,subprocess,time
p=Path('D:/Temp/native-performance-20261128/client-info')
assert not (p/'caller-control-events.jsonl').exists()
with (p/'caller-control-stdout.log').open('wb') as log:
    process=subprocess.Popen([str(p/'preview/caller-control-host.exe')],cwd=p/'preview',stdout=log,stderr=subprocess.STDOUT,creationflags=subprocess.CREATE_NO_WINDOW)
    kernel=ctypes.WinDLL('kernel32',use_last_error=True)
    kernel.WaitForSingleObject.argtypes=[ctypes.c_void_p,ctypes.c_ulong]
    kernel.WaitForSingleObject.restype=ctypes.c_ulong
    kernel.GetExitCodeProcess.argtypes=[ctypes.c_void_p,ctypes.POINTER(ctypes.c_ulong)]
    start=time.perf_counter()
    print(json.dumps({'pid':process.pid,'path':str(p/'preview/caller-control-host.exe')}),flush=True)
    observations=[]
    while time.perf_counter()-start<78:
        state=kernel.WaitForSingleObject(int(process._handle),1000)
        exit_code=ctypes.c_ulong()
        ok=kernel.GetExitCodeProcess(int(process._handle),ctypes.byref(exit_code))
        observations.append({'elapsed':time.perf_counter()-start,'wait':state,'getExitCodeOk':bool(ok),'exitCode':exit_code.value})
        if state==0: break
    final_wait=kernel.WaitForSingleObject(int(process._handle),0)
    result={'pid':process.pid,'observations':observations,'signaledWithin78s':final_wait==0,'testOnlyTerminated':False}
    if final_wait==258:
        process.terminate()
        process.wait(timeout=15)
        result['testOnlyTerminated']=True
    result['finalWait']=kernel.WaitForSingleObject(int(process._handle),0)
    (p/'caller-control-result.json').write_text(json.dumps(result,indent=2))
    print(json.dumps({k:v for k,v in result.items() if k!='observations'}),flush=True)
