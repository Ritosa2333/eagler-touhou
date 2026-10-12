"""Hardware frame breakpoint in a dedicated, debugger-owned TH12 process.

No WriteProcessMemory, game code patch, input synthesis or RNG modification.
All threads freeze at a Windows debug event. Only debug registers and the CPU
resume flag are set. The complete original main-loop transaction still runs.
"""
import ctypes as c
from ctypes import wintypes as w
import importlib.util
import json
import queue
import threading
import time
from pathlib import Path
from profile import FRAME_BOUNDARY, FRAME_SIGNATURE, capture, COMMON_REVISION

DWORD = c.c_uint32
PTR = c.c_void_p
class FloatingSave(c.Structure):
    _fields_ = [(n, DWORD) for n in ('ControlWord','StatusWord','TagWord',
        'ErrorOffset','ErrorSelector','DataOffset','DataSelector')] + [
        ('RegisterArea',c.c_ubyte*80),('Cr0NpxState',DWORD)]
class Context32(c.Structure):
    _fields_ = [('ContextFlags',DWORD)] + [(n,DWORD) for n in
        ('Dr0','Dr1','Dr2','Dr3','Dr6','Dr7')] + [('FloatSave',FloatingSave)] + [
        (n,DWORD) for n in ('SegGs','SegFs','SegEs','SegDs','Edi','Esi','Ebx',
        'Edx','Ecx','Eax','Ebp','Eip','SegCs','EFlags','Esp','SegSs')] + [
        ('ExtendedRegisters',c.c_ubyte*512)]
class ExceptionRecord(c.Structure):
    _fields_ = [('code',DWORD),('flags',DWORD),('record',PTR),('address',PTR),
                ('parameters',DWORD),('information',c.c_size_t*15)]
class ExceptionInfo(c.Structure):
    _fields_ = [('record',ExceptionRecord),('firstChance',DWORD)]
class CreateProcessInfo(c.Structure):
    _fields_ = [('file',w.HANDLE),('process',w.HANDLE),('thread',w.HANDLE),
        ('base',PTR),('debugOffset',DWORD),('debugSize',DWORD),('tls',PTR),
        ('start',PTR),('imageName',PTR),('unicode',w.WORD)]
class EventUnion(c.Union):
    _fields_ = [('exception',ExceptionInfo),('create',CreateProcessInfo),
                ('raw',c.c_ubyte*160)]
class DebugEvent(c.Structure):
    _fields_ = [('code',DWORD),('pid',DWORD),('tid',DWORD),('data',EventUnion)]
class StartupInfo(c.Structure):
    _fields_ = [('cb',DWORD),('reserved',w.LPWSTR),('desktop',w.LPWSTR),
        ('title',w.LPWSTR)] + [(n,DWORD) for n in ('x','y','width','height',
        'xChars','yChars','fill','flags')] + [('show',w.WORD),('reserved2Size',w.WORD),
        ('reserved2',PTR),('stdin',w.HANDLE),('stdout',w.HANDLE),('stderr',w.HANDLE)]
class ProcessInfo(c.Structure):
    _fields_ = [('process',w.HANDLE),('thread',w.HANDLE),('pid',DWORD),('tid',DWORD)]

class FrameDebugger:
    def __init__(self, executable, common_root, output):
        self.executable, self.output = Path(executable), Path(output)
        source = Path(common_root)/'testkit/replay-verifier/windows-process-reader.py'
        spec = importlib.util.spec_from_file_location('common_windows_reader',source)
        module = importlib.util.module_from_spec(spec); spec.loader.exec_module(module)
        self.reader = module.WindowsProcessReader(self.executable,self.executable.parent)
        self.k = c.WinDLL('kernel32',use_last_error=True)
        for name, args, result in [
            ('CreateProcessW',[w.LPCWSTR,w.LPWSTR,PTR,PTR,w.BOOL,DWORD,PTR,w.LPCWSTR,c.POINTER(StartupInfo),c.POINTER(ProcessInfo)],w.BOOL),
            ('WaitForDebugEvent',[c.POINTER(DebugEvent),DWORD],w.BOOL),
            ('ContinueDebugEvent',[DWORD,DWORD,DWORD],w.BOOL),
            ('Wow64GetThreadContext',[w.HANDLE,c.POINTER(Context32)],w.BOOL),
            ('Wow64SetThreadContext',[w.HANDLE,c.POINTER(Context32)],w.BOOL),
            ('CloseHandle',[w.HANDLE],w.BOOL),
            ('TerminateProcess',[w.HANDLE,w.UINT],w.BOOL),
            ('DebugSetProcessKillOnExit',[w.BOOL],w.BOOL)]:
            f=getattr(self.k,name); f.argtypes=args; f.restype=result
        if c.sizeof(PTR) != 8:
            raise RuntimeError('Launch with the bundled 64-bit Python.')
        assert c.sizeof(Context32)==716
        self.commands=queue.Queue(); self.lock=threading.Lock()
        self.latest={'status':'starting','commonRevision':COMMON_REVISION}
        self.mode='run'; self.target=None; self.frames=0; self.held=False
        self.ready=threading.Event(); self.closed=False
        self.worker=threading.Thread(target=self._run,name='TH12 debugger',daemon=True)
        self.worker.start()
    def check(self, result, operation):
        if not result: raise c.WinError(c.get_last_error(),operation)
    def context(self):
        ctx=Context32(); ctx.ContextFlags=0x10011
        self.check(self.k.Wow64GetThreadContext(self.pi.thread,c.byref(ctx)),'Get frame thread context')
        return ctx
    def set_context(self,ctx):
        self.check(self.k.Wow64SetThreadContext(self.pi.thread,c.byref(ctx)),'Set debug register')
    def command(self,kind,**fields):
        if self.closed: raise RuntimeError('Original inspector process has closed.')
        self.commands.put({'kind':kind,**fields})
    def state(self):
        with self.lock: return dict(self.latest)
    def _publish(self,row,status):
        with self.lock:
            self.latest={**row,'status':status,'processId':self.pi.pid,
                'presentFrame':self.frames,'boundary':hex(FRAME_BOUNDARY),
                'commonRevision':COMMON_REVISION,'coverage':'scalar fields; not complete object state'}
    def _apply(self,cmd,row):
        kind=cmd['kind']
        if kind=='close': self.closed=True; return
        if kind=='pause': self.mode='pause'
        elif kind=='resume': self.mode='run'; self.target=None
        elif kind=='step': self.mode='step'; self.target=self.frames+1
        elif kind=='until':
            stage,frame=cmd['stage'],cmd['frame']
            if row.get('replayActive') and stage==row.get('stage') and frame<=row.get('replayCursor',0):
                raise ValueError('原版只能向前运行；请重新播放该关再定位更早的帧。')
            self.mode='until'; self.target=(stage,frame)
        elif kind=='capture':
            if not self.held: raise ValueError('先停帧，再保存同帧画面。')
            from window_capture import capture_client
            capture_client(self.pi.pid,self.output/'latest.png')
            (self.output/'latest.json').write_text(json.dumps(self.state(),ensure_ascii=False,indent=2),encoding='utf8')
        else: raise ValueError('Unknown frame command')
    def _drain(self,row,block=False):
        while True:
            try: cmd=self.commands.get(timeout=.1 if block else 0)
            except queue.Empty: return
            try: self._apply(cmd,row)
            except Exception as error:
                with self.lock: self.latest['commandError']=str(error)
            if self.closed or self.mode!='pause': return
    def _run(self):
        self.pi=ProcessInfo(); si=StartupInfo(); si.cb=c.sizeof(si)
        try:
            self.check(self.k.CreateProcessW(str(self.executable),None,None,None,False,
                2,None,str(self.executable.parent),c.byref(si),c.byref(self.pi)),'Start isolated TH12')
            self.reader.handle=self.pi.process; self.reader.target_pid=self.pi.pid
            self.k.DebugSetProcessKillOnExit(True)
            self.output.mkdir(parents=True,exist_ok=True)
            with (self.output/'frames.jsonl').open('w',encoding='utf8') as trace:
                while not self.closed:
                    event=DebugEvent()
                    if not self.k.WaitForDebugEvent(c.byref(event),100):
                        if c.get_last_error()==121: continue
                        self.check(False,'WaitForDebugEvent')
                    handled=0x10002
                    if event.code in (1,5):
                        detail={'event':event.code,'tid':event.tid}
                        if event.code==1:detail.update(exception=hex(event.data.exception.record.code),address=hex(event.data.exception.record.address or 0),firstChance=event.data.exception.firstChance)
                        else:detail['exitCode']=hex(c.cast(c.byref(event.data),c.POINTER(DWORD))[0])
                        if event.code==5 or event.code==1 and event.data.exception.record.code not in (0x80000004,0x4000001e):
                            print(json.dumps(detail),flush=True)
                    if event.code==3:
                        if event.data.create.base!=0x400000: raise RuntimeError('Unexpected EXE base')
                        if self.reader.read(FRAME_BOUNDARY,4)!=FRAME_SIGNATURE: raise RuntimeError('Frame boundary signature mismatch')
                        ctx=self.context()
                        if ctx.Dr7&0xff: raise RuntimeError('Existing hardware breakpoints: refusing to replace them')
                        ctx.Dr0=FRAME_BOUNDARY; ctx.Dr6=0; ctx.Dr7=1
                        self.set_context(ctx)
                        if event.data.create.file:self.k.CloseHandle(event.data.create.file)
                    elif event.code==6:  # LOAD_DLL_DEBUG_EVENT: only the file handle is owned here.
                        handle=c.cast(c.byref(event.data),c.POINTER(w.HANDLE))[0]
                        if handle:self.k.CloseHandle(handle)
                    elif event.code==5:
                        self.closed=True
                        self._publish(self.state(),'closed')
                        with self.lock:self.latest['exitCode']=hex(c.cast(c.byref(event.data),c.POINTER(DWORD))[0])
                    elif event.code==1:
                        code=event.data.exception.record.code
                        if code in (0x80000004,0x4000001e) and event.tid==self.pi.tid:
                            ctx=self.context()
                            if ctx.Eip==FRAME_BOUNDARY and ctx.Dr6&1:
                                self.frames+=1
                                row=capture(self.reader)
                                trace.write(json.dumps({'presentFrame':self.frames,**row})+'\n')
                                if self.frames%60==0:trace.flush()
                                self._drain(row)
                                stop=(self.mode=='pause' or self.mode=='step' and self.frames>=self.target or
                                    self.mode=='until' and row.get('replayActive') and
                                    row['stage']==self.target[0] and row['replayCursor']>=self.target[1])
                                if stop:
                                    self.mode='pause'; self.held=True; trace.flush()
                                    self._publish(row,'paused'); self.ready.set()
                                    try:
                                        from window_capture import capture_client
                                        capture_client(self.pi.pid,self.output/'latest.png')
                                        (self.output/'latest.json').write_text(json.dumps(self.state(),ensure_ascii=False,indent=2),encoding='utf8')
                                    except Exception as error:
                                        with self.lock:self.latest['captureError']=str(error)
                                    while self.mode=='pause' and not self.closed: self._drain(row,block=True)
                                    self.held=False
                                self._publish(row,'running' if not self.closed else 'closed')
                                self.ready.set()
                                ctx.Dr6=0; ctx.EFlags|=0x10000  # RF: execute the faulting instruction once.
                                self.set_context(ctx)
                            else: handled=0x80010001
                        elif code not in (0x80000003,0x4000001f): handled=0x80010001
                    self.check(self.k.ContinueDebugEvent(event.pid,event.tid,handled),'Continue original frame')
        except Exception as error:
            with self.lock: self.latest={'status':'error','error':str(error)}
            self.ready.set()
        finally:
            self.closed=True
            if self.pi.process:
                self.k.TerminateProcess(self.pi.process,0)
                self.k.CloseHandle(self.pi.process)
            if self.pi.thread:self.k.CloseHandle(self.pi.thread)
