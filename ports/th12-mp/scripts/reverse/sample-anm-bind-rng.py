"""Execute actual ANM binding and verify authoritative/display RNG domain reset."""
import pathlib,sys,struct,json,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_ECX,UC_X86_REG_FPCW
exe=ROOT.parent/'th12/th12.exe';sha=hashlib.sha256(exe.read_bytes()).hexdigest()
if sha!='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417':raise ValueError('Pinned TH12 1.00b required')
pe=pefile.PE(str(exe));records=[]
for initial_domain,explicit_domain in [(0,None),(1,None),(0,1),(1,1),(1,0)]:
 vm=Uc(UC_ARCH_X86,UC_MODE_32);vm.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);vm.mem_write(0x400000,pe.get_memory_mapped_image())
 vm.mem_map(0x1000000,0x10000);vm.mem_map(0x2000000,0x20000)
 obj,resource,table,code,manager,stack,stop=0x2000000,0x2001000,0x2002000,0x2010000,0x2003000,0x100e000,0x100f000
 def put(at,fmt,*v):vm.mem_write(at,struct.pack('<'+fmt,*v))
 def read(at,fmt):return list(struct.unpack('<'+fmt,vm.mem_read(at,struct.calcsize('<'+fmt))))
 def command(op,fmt,values,mask=0):raw=struct.pack('<'+fmt,*values);return struct.pack('<HHhH',op,len(raw)+8,0,mask)+raw
 script=(command(87,'i',[explicit_domain]) if explicit_domain is not None else b'')+command(49,'fff',[0,0,10010],4)+command(63,'',[])
 vm.mem_write(code,script);put(obj+0x480,'I',initial_domain);put(resource+0x11c,'I',table);put(table,'I',code)
 put(0x4ce8cc,'I',manager);put(0x4b2ed0,'f',1.);put(0x4ce568,'II',0x1234,0);put(0x4ce560,'II',0xbeef,0)
 put(stack,'III',stop,obj,0);vm.reg_write(UC_X86_REG_ESP,stack);vm.reg_write(UC_X86_REG_ECX,resource);vm.reg_write(UC_X86_REG_FPCW,0x37f)
 vm.emu_start(0x454d10,stop,count=100000)
 if vm.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Binding exceeded native instruction budget')
 records.append({'initialDomain':initial_domain,'explicitOpcode87':explicit_domain,'boundFlags2':read(obj+0x480,'I')[0],
  'rotationZ':read(obj+0x2c,'f')[0],'gameSeedCounter':read(0x4ce568,'II'),'displaySeedCounter':read(0x4ce560,'II')})
out=ROOT/'artifacts/reverse/anm-binding-rng-samples.json';out.parent.mkdir(parents=True,exist_ok=True)
out.write_text(json.dumps({'schema':'th12-original-anm-binding-rng-samples/1','originalExeSha256':sha,
 'provider':'Original 0x454d10 binding through 0x402520 reset and 0x455630 interpreter, fixture masked RNG instruction',
 'wholeGameDeterminism':False,'wholePresentationOracle':False,'gpuExecuted':False,'records':records},indent=2)+'\n',encoding='utf-8');print(out)
