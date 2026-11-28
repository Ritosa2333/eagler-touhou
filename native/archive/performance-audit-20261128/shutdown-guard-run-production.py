from pathlib import Path
import ctypes,json,os,subprocess,sys,time
p=Path(__file__).resolve().parent
round_number=int(sys.argv[1]);assert 1<=round_number<=12
events=p/f'events-{round_number}.jsonl'
assert not events.exists()
env=dict(os.environ,TOUHOU_PROBE_LOG=str(events))
with (p/f'stdout-{round_number}.log').open('wb') as log:
    process=subprocess.Popen([str(p/'preview/guard-host.exe'),'https://touhouonline.com/login.html?lang=zh-CN'],cwd=p/'production-validation',env=env,stdout=log,stderr=subprocess.STDOUT,creationflags=subprocess.CREATE_NO_WINDOW)
    kernel=ctypes.WinDLL('kernel32',use_last_error=True)
    kernel.WaitForSingleObject.argtypes=[ctypes.c_void_p,ctypes.c_ulong];kernel.WaitForSingleObject.restype=ctypes.c_ulong
    kernel.GetExitCodeProcess.argtypes=[ctypes.c_void_p,ctypes.POINTER(ctypes.c_ulong)]
    start=time.perf_counter();samples=[]
    print(json.dumps({'round':round_number,'pid':process.pid}),flush=True)
    while time.perf_counter()-start<45:
        state=kernel.WaitForSingleObject(int(process._handle),1000)
        code=ctypes.c_ulong();ok=kernel.GetExitCodeProcess(int(process._handle),ctypes.byref(code))
        samples.append({'elapsedSeconds':time.perf_counter()-start,'kernelWait':state,'exitCodeRead':bool(ok),'exitCode':code.value})
        if state==0:break
    result={'round':round_number,'pid':process.pid,'observations':samples,'kernelSignaled':state==0,'exitCode':code.value,'forcedTermination':False}
    if state==258:
        process.terminate();process.wait(timeout=15);result['forcedTermination']=True
    result['finalWait']=kernel.WaitForSingleObject(int(process._handle),0)
    (p/f'result-{round_number}.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k!='observations'}),flush=True)
