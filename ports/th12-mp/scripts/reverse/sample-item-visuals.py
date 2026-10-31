"""Sample original ItemManager body/offscreen-arrow selection without GPU startup."""
import pathlib,sys,struct,json,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_FPCW
exe=ROOT.parent/'th12/th12.exe';sha=hashlib.sha256(exe.read_bytes()).hexdigest()
if sha!='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417':raise ValueError('Pinned TH12 1.00b required')
pe=pefile.PE(str(exe));records=[]
for y in [-64,-48,-40,-32,-24,-16,-8,-7,0,8,100]:
 vm=Uc(UC_ARCH_X86,UC_MODE_32);vm.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);vm.mem_write(0x400000,pe.get_memory_mapped_image())
 vm.mem_map(0x1000000,0x10000);vm.mem_map(0x2000000,0x700000)
 manager,item,stack,stop=0x2000000,0x2000014,0x100e000,0x100f000
 def put(at,fmt,*v):vm.mem_write(at,struct.pack('<'+fmt,*v))
 def read(at,fmt):return list(struct.unpack('<'+fmt,vm.mem_read(at,struct.calcsize('<'+fmt))))
 put(item+0x9b0,'i',1);put(item+0x47c,'I',3);put(item+0x96c,'fff',0.,float(y),0.)
 put(item+0x3bc,'I',0xffffffff);put(item+0x4b4+0x3bc,'I',0xffffffff);put(stack,'I',stop)
 draw=[]
 def capture(uc,address,size,user):
  if address!=0x45c900:return
  sp=uc.reg_read(UC_X86_REG_ESP);obj=uc.reg_read(UC_X86_REG_EAX)
  draw.append({'slot':'body' if obj==item else 'secondary','screenPosition':read(obj+0x424,'fff'),'alphaByte':read(obj+0x3bf,'B')[0]})
  uc.reg_write(UC_X86_REG_EIP,read(sp,'I')[0]);uc.reg_write(UC_X86_REG_ESP,sp+8)
 vm.hook_add(UC_HOOK_CODE,capture);vm.reg_write(UC_X86_REG_ESP,stack);vm.reg_write(UC_X86_REG_EAX,manager);vm.reg_write(UC_X86_REG_FPCW,0x37f)
 vm.emu_start(0x427230,stop,count=100000)
 if vm.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Item draw sampling exceeded instruction budget')
 records.append({'worldY':y,'draw':draw,'secondaryFlag':read(item+0x9b8,'I')[0]})
out=ROOT/'artifacts/reverse/item-visual-samples.json';out.parent.mkdir(parents=True,exist_ok=True)
out.write_text(json.dumps({'schema':'th12-original-item-visual-samples/1','originalExeSha256':sha,
 'provider':'0x427230 original ItemManager draw; constructed one active item; intercepted 0x45c900 GPU draw call',
 'wholePresentationOracle':False,'gpuExecuted':False,'records':records},indent=2)+'\n',encoding='utf-8');print(out)
