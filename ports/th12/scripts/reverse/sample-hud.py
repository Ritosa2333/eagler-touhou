"""Capture original TH12 HUD text calls; no GPU, formatter or full game execution."""
import pathlib,sys,struct,json,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_ESI,UC_X86_REG_ECX,UC_X86_REG_EBX,UC_X86_REG_FPCW
exe=ROOT.parent/'th12/th12.exe';sha=hashlib.sha256(exe.read_bytes()).hexdigest()
if sha!='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417':raise ValueError('Pinned TH12 1.00b required')
pe=pefile.PE(str(exe));vm=Uc(UC_ARCH_X86,UC_MODE_32)
vm.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);vm.mem_write(0x400000,pe.get_memory_mapped_image())
vm.mem_map(0x1000000,0x10000);vm.mem_map(0x2000000,0x30000)
ascii,hud,stack=0x2000000,0x2020000,0x100e000
def put(at,fmt,*v):vm.mem_write(at,struct.pack('<'+fmt,*v))
def read(at,fmt):return list(struct.unpack('<'+fmt,vm.mem_read(at,struct.calcsize('<'+fmt))))
put(0x4b43b8,'I',ascii);put(0x4b0c40,'i',12345678);put(0x4b0cc8,'i',0)
put(hud+0x6cdc,'i',9876543);put(0x4b0cc4,'i',0);put(0x4b0c48,'i',143);put(0x4b0cd4,'i',100);put(0x4b0cd0,'i',400)
put(0x4b0c78,'i',12345000);put(0x4b0cdc,'i',246);put(ascii+0x18f84,'ff',1.,1.)
vm.reg_write(UC_X86_REG_ESP,stack);vm.reg_write(UC_X86_REG_ESI,hud);vm.reg_write(UC_X86_REG_FPCW,0x37f)
calls=[]
def capture(uc,address,size,user):
 if address not in (0x401610,0x4015c0):return
 sp=uc.reg_read(UC_X86_REG_ESP);pos=uc.reg_read(UC_X86_REG_EBX)
 result={'address':hex(address),'position':read(pos,'fff'),'scale':read(ascii+0x18f84,'ff'),'font':read(ascii+0x18f98,'i')[0],
  'alignment':read(ascii+0x18fa4,'ii')}
 if address==0x401610:result['groupedNumber']=uc.reg_read(UC_X86_REG_ECX)
 else:
  fmt=read(sp+4,'I')[0];result['format']=bytes(uc.mem_read(fmt,32)).split(b'\0')[0].decode('ascii');result['argument']=read(sp+8,'i')[0]
 calls.append(result);uc.reg_write(UC_X86_REG_EIP,read(sp,'I')[0]);uc.reg_write(UC_X86_REG_ESP,sp+4)
vm.hook_add(UC_HOOK_CODE,capture);vm.emu_start(0x41f473,0x41f760,count=10000)
if vm.reg_read(UC_X86_REG_EIP)!=0x41f760:raise RuntimeError('HUD sampling did not reach selected end')
out=ROOT/'artifacts/reverse/hud-text-samples.json';out.parent.mkdir(parents=True,exist_ok=True)
out.write_text(json.dumps({'schema':'th12-original-hud-text-samples/1','originalExeSha256':sha,'provider':'Original HUD draw slice 0x41f473..0x41f760, text entry points captured and returned without formatting or GPU',
 'wholePresentationOracle':False,'gpuExecuted':False,'calls':calls},indent=2)+'\n',encoding='utf-8');print(out)
