"""Single-thread CPU host imports for the retail CRT, never game/GDI hooks.

The original46ea5c/47562a heap/thread constructors,475307 thread-data defaults,
and46cad8/46d3cc formatting still execute. No real Windows process, threads,
graphics, clock, locale selection or filesystem is accessed by this bridge.
"""
import collections,struct
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX,UC_X86_REG_EIP

class CrtHost:
 def __init__(self,v,pe,allocate,base=0x10fc000):
  self.v=v;self.allocate=allocate;self.last_error=0;self.slots={};self.blocks={};self.locks=set();self.counts=collections.Counter();self.queries=collections.Counter()
  self.heap=0x1001;self.module=0x1002;self.pointer_key=0x9e3779b9
  pops={'GetLastError':0,'SetLastError':4,'GetCurrentThreadId':0,'GetCurrentThread':0,'GetModuleHandleW':4,'GetModuleHandleA':4,'GetProcAddress':8,'EncodePointer':4,'DecodePointer':4,'TlsAlloc':0,'TlsGetValue':4,'TlsSetValue':8,'TlsFree':4,'FlsAlloc':4,'FlsGetValue':4,'FlsSetValue':8,'FlsFree':4,'HeapCreate':12,'GetProcessHeap':0,'HeapAlloc':12,'HeapFree':12,'HeapReAlloc':16,'HeapSize':12,'InitializeCriticalSection':4,'InitializeCriticalSectionAndSpinCount':8,'InitializeCriticalSectionEx':12,'EnterCriticalSection':4,'LeaveCriticalSection':4,'DeleteCriticalSection':4,'InterlockedIncrement':4,'InterlockedDecrement':4,'InterlockedExchange':8}
  self.addresses={name:base+index*16 for index,name in enumerate(pops)}
  self.by_address={address:(name,pops[name]) for name,address in self.addresses.items()}
  for address in self.by_address:v.mem_write(address,b'\xc3')
  for module in pe.DIRECTORY_ENTRY_IMPORT:
   if module.dll.upper()!=b'KERNEL32.DLL':continue
   for imported in module.imports:
    name=imported.name.decode('ascii') if imported.name else None
    if name in self.addresses:self.put(imported.address,self.addresses[name])
  v.hook_add(UC_HOOK_CODE,self.hook,begin=base,end=base+len(pops)*16-1)

 def get(self,address):return struct.unpack('<I',self.v.mem_read(address,4))[0]
 def put(self,address,value):self.v.mem_write(address,struct.pack('<I',value&0xffffffff))
 def text(self,address,wide=False):
  data=bytearray();size=2 if wide else 1
  for index in range(512):
   word=bytes(self.v.mem_read(address+index*size,size))
   if word==b'\0'*size:return data.decode('utf-16le' if wide else 'ascii')
   data.extend(word)
  raise RuntimeError('Unterminated CRT host API name')

 def hook(self,v,address,size,unused):
  if address not in self.by_address:raise RuntimeError('Entered the middle of a CRT host stub')
  name,pop=self.by_address[address];sp=v.reg_read(UC_X86_REG_ESP);args=[self.get(sp+4+index*4) for index in range(pop//4)];self.counts[name]+=1;result=0
  if name=='GetLastError':result=self.last_error
  elif name=='SetLastError':self.last_error=args[0]
  elif name=='GetCurrentThreadId':result=1
  elif name=='GetCurrentThread':result=0xfffffffe
  elif name in ('GetModuleHandleW','GetModuleHandleA'):
   module=self.text(args[0],name.endswith('W')).lower();result=self.module if module in ('kernel32','kernel32.dll') else 0
  elif name=='GetProcAddress':
   requested=self.text(args[1]);self.queries[requested]+=1;result=self.addresses.get(requested,0) if args[0]==self.module else 0
  elif name in ('EncodePointer','DecodePointer'):result=args[0]^self.pointer_key
  elif name in ('TlsAlloc','FlsAlloc'):
   result=max(self.slots,default=-1)+1;self.slots[result]=0
  elif name in ('TlsGetValue','FlsGetValue'):
   result=self.slots.get(args[0],0);self.last_error=0 if args[0] in self.slots else 87
  elif name in ('TlsSetValue','FlsSetValue'):
   if args[0] in self.slots:self.slots[args[0]]=args[1];result=1
   else:self.last_error=87
  elif name in ('TlsFree','FlsFree'):
   if args[0] in self.slots:del self.slots[args[0]];result=1
   else:self.last_error=87
  elif name in ('HeapCreate','GetProcessHeap'):result=self.heap
  elif name in ('HeapAlloc','HeapReAlloc'):
   if args[0]!=self.heap:raise RuntimeError('Native CRT used an unconstructed host heap')
   flags=args[1];amount=args[-1];result=self.allocate(amount);self.blocks[result]=amount
   if flags&8:v.mem_write(result,b'\0'*amount)
   if name=='HeapReAlloc' and args[2]:
    if args[2] not in self.blocks:raise RuntimeError('Native CRT realloc used an unknown host block')
    old_size=self.blocks.pop(args[2]);v.mem_write(result,bytes(v.mem_read(args[2],min(amount,old_size))))
  elif name=='HeapFree':
   if args[0]!=self.heap:raise RuntimeError('Native CRT free used an unconstructed host heap')
   if not args[2] or args[2] in self.blocks:self.blocks.pop(args[2],None);result=1
   else:raise RuntimeError('Native CRT free used an unknown host block')
  elif name=='HeapSize':result=self.blocks.get(args[2],0xffffffff)
  elif name.startswith('InitializeCriticalSection'):
   self.locks.add(args[0]);v.mem_write(args[0],b'\0'*24);result=1
  elif name in ('EnterCriticalSection','LeaveCriticalSection','DeleteCriticalSection'):
   if args[0] not in self.locks:raise RuntimeError('Native CRT used an unconstructed critical section')
   if name=='DeleteCriticalSection':self.locks.remove(args[0])
  elif name in ('InterlockedIncrement','InterlockedDecrement'):
   result=(self.get(args[0])+(1 if name.endswith('Increment') else -1))&0xffffffff;self.put(args[0],result)
  elif name=='InterlockedExchange':result=self.get(args[0]);self.put(args[0],args[1])
  else:raise RuntimeError('Unsupported CRT host import '+name)
  v.reg_write(UC_X86_REG_EAX,result&0xffffffff);v.reg_write(UC_X86_REG_ESP,sp+4+pop);v.reg_write(UC_X86_REG_EIP,self.get(sp))

 def metadata(self):return {'scope':'Single-thread CPU heap/TLS/import/lock boundaries only; original CRT constructors and formatter execute; no real OS, GDI or GPU.','nativeConstructors':['0x46ea5c','0x47562a','0x475a4b','0x47c20c'],'calls':dict(self.counts),'dynamicApiQueries':dict(self.queries),'threadId':1}
