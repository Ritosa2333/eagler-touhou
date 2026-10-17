"""Sparse read-only observation of an already running retail process.

No debugger, suspension, memory writes, input injection or process termination.
Samples are non-atomic diagnostics, never a completed-frame parity/PASS trace.
"""
import argparse, ctypes as C, json, struct, time
from pathlib import Path

def main():
    p=argparse.ArgumentParser()
    p.add_argument('--pid',type=int,required=True)
    p.add_argument('--image-base',type=lambda s:int(s,0),required=True)
    p.add_argument('--seconds',type=float,default=180)
    p.add_argument('--output',type=Path,required=True)
    a=p.parse_args()
    if a.seconds<=0:p.error('seconds must be positive')
    k=C.WinDLL('kernel32',use_last_error=True)
    k.OpenProcess.argtypes=[C.c_uint32,C.c_int,C.c_uint32];k.OpenProcess.restype=C.c_void_p
    k.ReadProcessMemory.argtypes=[C.c_void_p,C.c_void_p,C.c_void_p,C.c_size_t,C.c_void_p]
    k.CloseHandle.argtypes=[C.c_void_p]
    h=k.OpenProcess(0x10,False,a.pid) # PROCESS_VM_READ only.
    if not h:raise C.WinError(C.get_last_error())
    def read(at,n):
        b=C.create_string_buffer(n);done=C.c_size_t()
        if not k.ReadProcessMemory(h,at,b,n,C.byref(done)) or done.value!=n:raise C.WinError(C.get_last_error())
        return b.raw
    def u(at):return struct.unpack('<I',read(at,4))[0]
    def word(data,at):return struct.unpack_from('<I',data,at)[0]
    d=a.image_base-0x400000;end=time.monotonic()+a.seconds;last=None;count=0;heartbeat=0
    a.output.parent.mkdir(parents=True,exist_ok=True)
    try:
        with a.output.open('w',encoding='utf-8') as sink:
            while time.monotonic()<end:
                try:
                    r=u(0x5c60fc+d);g=u(0x5ba828+d)
                    if not r or not g or u(r+0x10)!=1:time.sleep(.1);continue
                    stage=u(r+0x258)
                    if stage>=8:time.sleep(.1);continue
                    cursor=r+0xe8+stage*0x2c;frame=u(cursor+0x14)
                    player=u(0x5ba568+d+4)
                    if not player:time.sleep(.1);continue
                    entity=read(player,0x2100);table=read(0x5ba568+d+0x88,0x228)
                    rng=read(0x5ba4a8+d,0x28);flags=u(g+0xe8)
                    if frame!=u(cursor+0x14) or stage!=u(r+0x258):continue
                    identity=(stage,frame)
                    if identity!=last:
                        last=identity;count+=1
                        row={'purpose':'sparse-nonatomic-observation','stage':stage,'frame':frame,
                             'rng':word(rng,0x14),'rngState':word(rng,4),
                             'playerXBits':word(entity,0x614),'playerYBits':word(entity,0x618),
                             'playerState':word(entity,0x10),'scoreLow':word(table,0),
                             'lives':word(table,0xb8),'specialItems':word(table,0x44),
                             'overlayMeter':word(table,0x4c),'gameFlags':flags,
                             'filename':read(r+0x260,256).split(b'\0')[0].decode('ascii','replace')}
                        sink.write(json.dumps(row,separators=(',',':'))+'\n');sink.flush()
                        if time.monotonic()>=heartbeat:
                            print(json.dumps(row),flush=True);heartbeat=time.monotonic()+15
                except OSError as error:
                    # Object pointers may change during a live scene transition.
                    if C.get_last_error() not in (299,487):raise
                time.sleep(.05)
    finally:
        k.CloseHandle(h)
        print(json.dumps({'samples':count,'purpose':'diagnostic-only','processLeftRunning':True}),flush=True)

if __name__=='__main__':main()
