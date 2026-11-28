from pathlib import Path
import ctypes,json,subprocess,sys,time
p=Path(__file__).resolve().parent
width=int(sys.argv[1]);assert width in (320,390,1280)
assert not (p/f'events-{width}.jsonl').exists()
with (p/f'stdout-{width}.log').open('wb') as log:
    process=subprocess.Popen([str(p/f'preview/account-{width}.exe'),'https://touhouonline.com/login.html?lang=zh-CN'],cwd=p/'preview',stdout=log,stderr=subprocess.STDOUT,creationflags=subprocess.CREATE_NO_WINDOW)
    kernel=ctypes.WinDLL('kernel32',use_last_error=True)
    kernel.WaitForSingleObject.argtypes=[ctypes.c_void_p,ctypes.c_ulong]
    kernel.WaitForSingleObject.restype=ctypes.c_ulong
    start=time.perf_counter()
    print(json.dumps({'pid':process.pid,'path':str(p/f'preview/account-{width}.exe')}),flush=True)
    samples=[]
    while time.perf_counter()-start<140:
        state=kernel.WaitForSingleObject(int(process._handle),1000)
        samples.append({'elapsedSeconds':time.perf_counter()-start,'kernelWait':state})
        if state==0:break
    result={'pid':process.pid,'observations':samples,'signaledWithin140s':state==0,'forcedTermination':False}
    if state==258:
        process.terminate();process.wait(timeout=15);result['forcedTermination']=True
    result['finalWait']=kernel.WaitForSingleObject(int(process._handle),0)
    (p/f'result-{width}.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k!='observations'}),flush=True)
