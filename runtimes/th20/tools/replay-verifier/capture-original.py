"""Isolated retail TH20 oracle: observe completed frames with a Windows debugger.

The original file is hash checked and copied; all writes are in the test process
or its private working/APPDATA directory. Breakpoints observe dispatch boundaries.
Menu input is injected before the original button derivation, then stays zero.
Recorded gameplay input, gameplay objects, RNG and clocks are never overwritten.
"""
import argparse, ctypes as C, hashlib, json, os, shutil, struct, time
from ctypes import wintypes as W
from pathlib import Path

SHA = 'a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
class STARTUP(C.Structure):
    _fields_=[('cb',W.DWORD),('reserved',W.LPWSTR),('desktop',W.LPWSTR),('title',W.LPWSTR),
              *[(n,W.DWORD) for n in ('x','y','cx','cy','charsX','charsY','fill','flags')],
              ('show',W.WORD),('reserved2Size',W.WORD),('reserved2',C.c_void_p),
              ('stdin',W.HANDLE),('stdout',W.HANDLE),('stderr',W.HANDLE)]
class PROCESS(C.Structure):
    _fields_=[('process',W.HANDLE),('thread',W.HANDLE),('pid',W.DWORD),('tid',W.DWORD)]
class EVENT(C.Structure):
    _fields_=[('code',W.DWORD),('pid',W.DWORD),('tid',W.DWORD),('data',C.c_uint64*20)]
class FLOATSAVE(C.Structure):
    _fields_=[('words',W.DWORD*7),('registers',C.c_byte*80),('cr0',W.DWORD)]
class CONTEXT(C.Structure):
    _fields_=[('flags',W.DWORD),('debug',W.DWORD*6),('fp',FLOATSAVE),
              *[(n,W.DWORD) for n in ('gs','fs','es','ds','edi','esi','ebx','edx','ecx','eax','ebp','eip','cs','eflags','esp','ss')],
              ('extended',C.c_byte*512)]

def main():
    p=argparse.ArgumentParser();p.add_argument('--executable',type=Path,required=True)
    p.add_argument('--replay',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
    p.add_argument('--stage',type=int,default=6,choices=range(1,8));p.add_argument('--max-ticks',type=int,default=3000)
    p.add_argument('--timeout',type=float,default=240);p.add_argument('--diagnose-score',action='store_true');p.add_argument('--diagnose-items',action='store_true');a=p.parse_args()
    if a.max_ticks<1 or a.timeout<=0:p.error('max-ticks and timeout must be positive')
    if hashlib.sha256(a.executable.read_bytes()).hexdigest()!=SHA:raise ValueError('Oracle executable hash mismatch')
    # The isolated oracle uses a private host mutex so it can coexist with the
    # user's ordinary game. Gameplay and replay machine code remain unchanged.
    pe=a.executable.read_bytes();needle='th20 App\0'.encode('utf-16le');offset=pe.index(needle)
    nt=struct.unpack_from('<I',pe,0x3c)[0];nsec=struct.unpack_from('<H',pe,nt+6)[0]
    opt_size=struct.unpack_from('<H',pe,nt+20)[0];mutex_va=None
    for i in range(nsec):
        sec=nt+24+opt_size+i*40;vsize,rva,size,raw=struct.unpack_from('<IIII',pe,sec+8)
        if raw<=offset<raw+size:mutex_va=0x400000+rva+offset-raw
    if mutex_va is None:raise ValueError('Private mutex string not in a PE section')
    output=a.output.resolve();output.parent.mkdir(parents=True,exist_ok=True)
    work=output.parent/'original-private'/output.stem/str(os.getpid());work.mkdir(parents=True,exist_ok=True)
    shutil.copyfile(a.executable,work/'th20.exe')
    for name in ('th20.dat','thbgm.dat'):
        target=work/name
        if not target.exists():os.link(a.executable.parent/name,target)
        if not os.path.samefile(target,a.executable.parent/name):raise ValueError('Unexpected private resource')
    data=work/'appdata'/'ShanghaiAlice'/'th20';(data/'replay').mkdir(parents=True,exist_ok=True)
    shutil.copyfile(a.replay,data/'replay'/'th20_02.rpy')
    # Original configuration layout; use the default windowed 640x480 profile.
    config=bytearray(0xb0);struct.pack_into('<II',config,0,0x200002,0xb0)
    for i in range(2):
        struct.pack_into('<24H',config,8+i*48,*([0,1,2,3]+[65535]*4+[0,1,5,10]+[65535]*4+[0x5a,0x58,0x10,0x1b,0x26,0x28,0x25,0x27]))
    struct.pack_into('<HH',config,0x70,600,600)
    config[0x75:0x80]=bytes([1,1,0,3,0,0,0,0,2,100,80]);config[0x81]=2
    struct.pack_into('<IiiI',config,0x84,0,-2147483648,-2147483648,1);config[0x9a]=1
    (data/'th20.cfg').write_bytes(config)
    k=C.WinDLL('kernel32',use_last_error=True)
    signatures={
      'CreateProcessW':([W.LPCWSTR,W.LPWSTR,C.c_void_p,C.c_void_p,W.BOOL,W.DWORD,C.c_void_p,W.LPCWSTR,C.POINTER(STARTUP),C.POINTER(PROCESS)],W.BOOL),
      'WaitForDebugEvent':([C.POINTER(EVENT),W.DWORD],W.BOOL),'ContinueDebugEvent':([W.DWORD,W.DWORD,W.DWORD],W.BOOL),
      'ReadProcessMemory':([W.HANDLE,C.c_void_p,C.c_void_p,C.c_size_t,C.POINTER(C.c_size_t)],W.BOOL),
      'WriteProcessMemory':([W.HANDLE,C.c_void_p,C.c_void_p,C.c_size_t,C.POINTER(C.c_size_t)],W.BOOL),
      'FlushInstructionCache':([W.HANDLE,C.c_void_p,C.c_size_t],W.BOOL),
      'VirtualProtectEx':([W.HANDLE,C.c_void_p,C.c_size_t,W.DWORD,C.POINTER(W.DWORD)],W.BOOL),
      'OpenThread':([W.DWORD,W.BOOL,W.DWORD],W.HANDLE),'CloseHandle':([W.HANDLE],W.BOOL),
      'Wow64GetThreadContext':([W.HANDLE,C.POINTER(CONTEXT)],W.BOOL),'Wow64SetThreadContext':([W.HANDLE,C.POINTER(CONTEXT)],W.BOOL),
      'TerminateProcess':([W.HANDLE,W.UINT],W.BOOL)}
    for n,(args,ret) in signatures.items():getattr(k,n).argtypes=args;getattr(k,n).restype=ret
    si=STARTUP();si.cb=C.sizeof(si);si.flags=1;si.show=0;pi=PROCESS()
    # A debugger-started process otherwise enables the Windows debug heap.
    # Freed-block poisoning changes retail use-after-free observations and
    # creates replay divergences absent from an ordinary game launch.
    env=dict(os.environ,APPDATA=str(work/'appdata'),_NO_DEBUG_HEAP='1');environment=C.create_unicode_buffer('\0'.join(f'{n}={v}' for n,v in sorted(env.items()))+'\0\0')
    cmd=C.create_unicode_buffer('"'+str(work/'th20.exe')+'"')
    if not k.CreateProcessW(None,cmd,None,None,False,2|0x400|0x4000,environment,str(work),C.byref(si),C.byref(pi)):raise C.WinError(C.get_last_error())
    def read(at,n):
        b=C.create_string_buffer(n);done=C.c_size_t()
        if not k.ReadProcessMemory(pi.process,at,b,n,C.byref(done)) or done.value!=n:raise C.WinError(C.get_last_error(),hex(at))
        return b.raw
    def write(at,b):
        done=C.c_size_t();buffer=C.create_string_buffer(b)
        old=W.DWORD()
        if not k.VirtualProtectEx(pi.process,at,len(b),0x40,C.byref(old)):raise C.WinError(C.get_last_error(),hex(at))
        try:
            if not k.WriteProcessMemory(pi.process,at,buffer,len(b),C.byref(done)) or done.value!=len(b):raise C.WinError(C.get_last_error(),hex(at))
        finally:
            restored=W.DWORD();k.VirtualProtectEx(pi.process,at,len(b),old.value,C.byref(restored))
        k.FlushInstructionCache(pi.process,at,len(b))
    def u(at):return struct.unpack('<I',read(at,4))[0]
    delta=0;breaks={};pending={};frames=0;last=None;ticks=0;started=False;terminal=False;tail=[];first_chance=[]
    script=[0]*600
    def press(mask,wait=30):script.extend([mask]*3+[0]*wait)
    for _ in range(4):press(0x20)
    press(0x10);press(1,90);press(0x20);press(1,90)
    for _ in range(a.stage-1):press(0x20)
    press(1,30)
    previous=0;deadline=time.monotonic()+a.timeout;rows=output.with_suffix('.rows.jsonl');reason='timeout';events=0;heartbeat=time.monotonic()+10
    def capture():
        nonlocal last,ticks,started,terminal
        r=u(0x5c60fc+delta);g=u(0x5ba828+delta)
        if not r or u(r+0x10)!=1 or not g:
            if started:terminal=True
            return
        if u(r+0x25c)&2:terminal=True
        active=struct.unpack('<i',read(r+0x258,4))[0]
        if not 0<=active<8:
            if started:terminal=True
            return
        cursor=r+0xe8+active*0x2c;frame=u(cursor+0x14)
        player=u(0x5ba568+delta+4)
        if not player or frame<1:return
        if (u(0x5b6758+delta+0x58),u(0x5b6758+delta+0x5c),u(0x5b6758+delta+0x20c0))!=(320,16,0x3f800000):
            raise RuntimeError('Oracle logical viewport differs from the SDL 640x480 profile')
        identity=(active,frame)
        if identity==last:return
        if last and active==last[0] and frame!=last[1]+1:raise RuntimeError(f'Missing oracle tick {last} -> {identity}')
        last=identity;started=True
        w=[0]*36;w[0:3]=[1,active,frame]
        w[3]=u(g+0x14);w[4]=u(g+0x24);w[5]=u(g+0xe8)
        b=0x5b88b0+delta;w[6:9]=[u(b+0x29c),u(b+0x2a8),u(b+0x2ac)]
        w[9]=u(0x5ba4bc+delta);w[10]=u(0x5ba4ac+delta)
        w[11:13]=struct.unpack('<2I',read(player+0x614,8));w[13]=u(player+0x10)
        w[14:16]=struct.unpack('<2I',read(player+0x620,8));w[16]=read(player+0x204c,1)[0]
        table=0x5ba568+delta+0x88;w[17:19]=struct.unpack('<2I',read(table,8))
        w[19]=u(table+0x30);w[20]=u(table+0xb8);w[21]=u(table+0xc0);w[22]=u(table+0xd0);w[23]=u(table+0xcc)
        w[24]=u(0x5ba568+delta+0x6c);hud=u(0x5c06a4+delta);w[25]=int(bool(hud and u(hud+0x1bc)))
        w[26:30]=struct.unpack('<4I',read(player+0x20b4,16))
        w[30]=u(table+0xe4)
        w[31]=u(0x5b6758+delta+0x20c0)
        w[32:36]=struct.unpack('<4I',read(table+0x40,16))
        sample={'words':w}
        if a.diagnose_items and 3600<=frame<=4090:
            owner=u(0x5ba568+delta+12);pool=read(owner+0x18,1536*0xc4c);items=[]
            for i in range(1536):
                at=i*0xc4c
                state=struct.unpack_from('<I',pool,at+0xc24)[0]
                if state:
                    items.append([i,state,struct.unpack_from('<I',pool,at+0xc28)[0],*struct.unpack_from('<6I',pool,at+0xbe4),struct.unpack_from('<I',pool,at+0xc30)[0],struct.unpack_from('<I',pool,at+0xc34)[0],struct.unpack_from('<I',pool,at+0xc08)[0],struct.unpack_from('<I',pool,at+0xc38)[0]])
            sample['items']=items
        sink.write(json.dumps(sample,separators=(',',':'))+'\n');ticks+=1
        if ticks%300==0:print(json.dumps({'ticks':ticks,'stage':active,'frame':frame,'state':w[13],'lives':w[20],'rng':w[9]}),flush=True)
    try:
      with rows.open('w',encoding='utf-8') as sink:
        while time.monotonic()<deadline and ticks<a.max_ticks and not terminal:
            e=EVENT()
            if not k.WaitForDebugEvent(C.byref(e),1000):
                if time.monotonic()>heartbeat:
                    titles=[];user=C.WinDLL('user32');cb=C.WINFUNCTYPE(W.BOOL,W.HWND,W.LPARAM)
                    def visit(hwnd,_):
                        pid=W.DWORD();user.GetWindowThreadProcessId(hwnd,C.byref(pid))
                        if pid.value==pi.pid:
                            title=C.create_unicode_buffer(512);user.GetWindowTextW(hwnd,title,512);titles.append(title.value)
                        return True
                    user.EnumWindows(cb(visit),0)
                    print(json.dumps({'waiting':True,'frames':frames,'ticks':ticks,'windows':titles}),flush=True);heartbeat=time.monotonic()+10
                continue
            status=0x10002
            events+=1
            if events<5:print(json.dumps({'event':e.code,'pid':e.pid,'tid':e.tid,'raw':[hex(v) for v in e.data[:4]]}),flush=True)
            if e.code==3:
                delta=e.data[3]-0x400000
                write(mutex_va+delta,'rv20 App\0'.encode('utf-16le'))
                for at in ((0x42010a,0x412ccf,0x4e14b0,0x4c4960,0x4a9fb0) if a.diagnose_score else (0x42010a,0x412ccf)):
                    actual=at+delta;breaks[actual]=read(actual,1);write(actual,b'\xcc')
                if e.data[0]:k.CloseHandle(e.data[0])
            elif e.code==6:
                if e.data[0]:k.CloseHandle(e.data[0])
            elif e.code==5:
                reason=f'exit-{e.data[0]&0xffffffff}';k.ContinueDebugEvent(e.pid,e.tid,status);break
            elif e.code==1:
                raw=bytes(e.data);code=struct.unpack_from('<I',raw)[0];at=struct.unpack_from('<Q',raw,16)[0]
                thread=k.OpenThread(0x1a,False,e.tid);ctx=CONTEXT();ctx.flags=0x10007
                try:
                    if code in (0x80000003,0x4000001f) and at in breaks:
                        if not k.Wow64GetThreadContext(thread,C.byref(ctx)):raise C.WinError(C.get_last_error())
                        if at==0x42010a+delta:
                            # The stopped-loop SDL oracle has no native focus
                            # events. Match that host policy in this private
                            # process so desktop focus loss cannot auto-pause.
                            write(0x5b6758+delta+0x68,b'\x01\x00')
                            mask=script[frames] if frames<len(script) and not started else 0;frames+=1
                            for slot in range(4):write(0x5b88b0+delta+slot*0x2c0,struct.pack('<II',mask,previous))
                            previous=mask
                        elif at in (0x4e14b0+delta,0x4c4960+delta,0x4a9fb0+delta):
                            r=u(0x5c60fc+delta)
                            if r:
                                stage=u(r+0x258)
                                if stage<8:
                                    frame=u(r+0xe8+stage*0x2c+0x14)
                                    if 4078<=frame<=4130:
                                        if at==0x4a9fb0+delta:
                                            print(json.dumps({'specialDiagnostic':True,'frame':frame,'before':u(ctx.ecx+0x44),'amount':u(ctx.esp+4),'caller':hex(u(ctx.esp)-delta)}),flush=True)
                                        elif at==0x4e14b0+delta:
                                            print(json.dumps({'scoreDiagnostic':True,'stage':stage,'frame':frame,'score':u(ctx.ecx),'amount':struct.unpack('<Q',read(ctx.esp+4,8))[0],'caller':hex(u(ctx.esp)-delta)}),flush=True)
                                        else:
                                            player=u(0x5ba568+delta+4);table=0x5ba568+delta+0x88
                                            print(json.dumps({'pointDiagnostic':True,'frame':frame,'position':struct.unpack('<3f',read(ctx.ecx+0xbe4,12)),'state':u(ctx.ecx+0xc24),'line':struct.unpack('<f',read(player+0x2090,4))[0],'base':u(table+0x40),'special':u(table+0x44),'ratio':u(table+0x48)}),flush=True)
                        else:
                            capture()
                            if frames%300==0 and not started:print(json.dumps({'menuFrames':frames,'menu':u(0x5c6124+delta)}),flush=True)
                        write(at,breaks[at]);ctx.eip=at;ctx.eflags|=0x100;k.Wow64SetThreadContext(thread,C.byref(ctx));pending[e.tid]=at
                    elif code in (0x80000004,0x4000001e) and e.tid in pending:
                        at=pending.pop(e.tid);write(at,b'\xcc')
                    elif code not in (0x80000003,0x4000001f,0x40010006,0x406d1388):
                        status=0x80010001
                        record={'code':hex(code),'address':hex(at),'frame':last[1] if last else None}
                        if struct.unpack_from('<I',raw,152)[0]:first_chance.append(record)
                        else:tail.append(record)
                        print(json.dumps({'exception':hex(code),'address':hex(at)}),flush=True)
                        if len(tail)>32:tail.pop(0)
                finally:
                    if thread:k.CloseHandle(thread)
            k.ContinueDebugEvent(e.pid,e.tid,status)
        if terminal:reason='replay-finished'
        elif ticks>=a.max_ticks:reason='bounded-prefix'
    except BaseException as error:
        reason='capture-error';tail.append({'collectorError':str(error)});raise
    finally:
        k.TerminateProcess(pi.process,0);k.CloseHandle(pi.thread);k.CloseHandle(pi.process)
        output.write_text(json.dumps({'schema':'th20/replay-capture/v1','provider':'retail-windows-debug-boundary','hostFocusPolicy':'always-active-private-process','heapPolicy':'ordinary-windows-heap-no-debug-poisoning','complete':terminal,'reason':reason,'ticks':ticks,'rows':str(rows),'executableSha256':SHA,'replaySha256':hashlib.sha256(a.replay.read_bytes()).hexdigest(),'resourceSha256':hashlib.sha256((work/'th20.dat').read_bytes()).hexdigest(),'exceptions':tail,'firstChanceExceptions':first_chance},indent=2),encoding='utf-8')
    if not ticks:raise RuntimeError('No original replay ticks captured; see receipt')
if __name__=='__main__':main()
