"""Sample original ANM mode 15 gradient-circle vertices before any GPU call."""
import pathlib,sys,struct,json,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EDI,UC_X86_REG_EAX,UC_X86_REG_FPCW
exe=ROOT.parent/'th12/th12.exe';sha=hashlib.sha256(exe.read_bytes()).hexdigest()
if sha!='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417':raise ValueError('Pinned TH12 1.00b required')
pe=pefile.PE(str(exe));records=[]
for x,y,radius,angle,count in [(224,240,32,0,32),(17,-9,64,.7853981852531433,8)]:
 vm=Uc(UC_ARCH_X86,UC_MODE_32);vm.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);vm.mem_write(0x400000,pe.get_memory_mapped_image())
 vm.mem_map(0x1000000,0x10000);vm.mem_map(0x2000000,0xa00000)
 manager,vertices,stack=0x2000000,0x2900000,0x100e000
 def put(at,fmt,*v):vm.mem_write(at,struct.pack('<'+fmt,*v))
 def read(at,fmt):return list(struct.unpack('<'+fmt,vm.mem_read(at,struct.calcsize('<'+fmt))))
 put(manager+0x8856b0,'I',vertices);put(stack,'IffffII',0,x,y,radius,angle,count,0x20a0b0c0)
 vm.reg_write(UC_X86_REG_ESP,stack);vm.reg_write(UC_X86_REG_EDI,manager);vm.reg_write(UC_X86_REG_EAX,0x80708090);vm.reg_write(UC_X86_REG_FPCW,0x37f)
 vm.emu_start(0x45d430,0x45d4e1,count=10000)
 if vm.reg_read(UC_X86_REG_EIP)!=0x45d4e1:raise RuntimeError('Circle sampling exceeded instruction budget')
 records.append({'x':x,'y':y,'radius':radius,'angle':angle,'segments':count,
  'vertices':[{'position':read(vertices+i*20,'fff'),'reciprocalW':read(vertices+i*20+12,'f')[0],'argb':hex(read(vertices+i*20+16,'I')[0])} for i in range(count+2)]})
out=ROOT/'artifacts/reverse/circle-vertices-samples.json';out.parent.mkdir(parents=True,exist_ok=True)
out.write_text(json.dumps({'schema':'th12-original-circle-samples/1','originalExeSha256':sha,'provider':'0x45d430 mode15 gradient circle; stop at 0x45d4e1 before GPU calls',
 'wholePresentationOracle':False,'gpuExecuted':False,'records':records},indent=2)+'\n',encoding='utf-8');print(out)
