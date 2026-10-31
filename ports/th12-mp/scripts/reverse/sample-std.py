"""Sample original TH12 STD camera arithmetic, without starting the game or GPU."""
import pathlib, sys, struct, json, hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_ECX, UC_X86_REG_FPCW
exe=ROOT.parent/'th12/th12.exe'
sha=hashlib.sha256(exe.read_bytes()).hexdigest()
if sha!='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417':raise ValueError('Pinned TH12 1.00b required')
pe=pefile.PE(str(exe)); std=(ROOT/'local/retail/stage01.std').read_bytes()
vm=Uc(UC_ARCH_X86,UC_MODE_32)
vm.mem_map(0,4096);vm.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095)
vm.mem_write(0x400000,pe.get_memory_mapped_image())
vm.mem_map(0x1000000,0x10000);vm.mem_map(0x2000000,0x40000)
stage,code,config,stack,stop=0x2000000,0x2010000,0x2030000,0x100e000,0x100f000
def put(at,fmt,*v):vm.mem_write(at,struct.pack('<'+fmt,*v))
def read(at,fmt):return list(struct.unpack('<'+fmt,vm.mem_read(at,struct.calcsize('<'+fmt))))
def call(address,args=()):
 put(stack,'I'*(1+len(args)),stop,*args);vm.reg_write(UC_X86_REG_ESP,stack)
 vm.emu_start(address,stop,count=200000)
 if vm.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Native instruction budget exceeded')
vm.reg_write(UC_X86_REG_FPCW,0x37f)
camera_defaults=[]
for flags in [0,0x4000]:
 put(config+0x590,'I',flags);vm.reg_write(UC_X86_REG_ECX,config);call(0x430be0)
 camera_defaults.append({'configFlags':hex(flags),'position':read(0x4ced1c,'fff'),'direction':read(0x4ced28,'fff'),
  'up':read(0x4ced34,'fff'),'fov':read(0x4ced64,'f')[0],'viewport':read(0x4cede8,'IIII'),
  'nearFar':read(0x4a3e14,'ff')})
put(config+0x590,'I',0);vm.reg_write(UC_X86_REG_ECX,config);call(0x430be0)
vm.mem_write(stage+0x360c,bytes(vm.mem_read(0x4ced1c,0x118)))
start=struct.unpack_from('<I',std,8)[0];vm.mem_write(code,std[start:])
put(0x4b2ed0,'f',1.);put(0x4b43c0,'I',stage)
put(stage+0x1c,'I',code);put(stage+0x4c,'I',code)
put(stage+0x38,'iifII',-1,0,0.,0x4b2ed0,1)
frames=[];selected={0,1,2,9,99,299,599,689,690,691,1201,1202,1203,1500}
for frame in range(1501):
 call(0x4049a0,(stage,))
 if frame in selected:frames.append({'frame':frame,'scriptClock':read(stage+0x3c,'i')[0],
  'position':read(stage+0x360c,'fff'),'direction':read(stage+0x3618,'fff'),'up':read(stage+0x3624,'fff'),
  'fov':read(stage+0x3654,'f')[0],'fogArgb':hex(read(stage+0x3720,'I')[0]),'fogRange':read(stage+0x3708,'ff')})
out=ROOT/'artifacts/reverse/std-camera-samples.json';out.parent.mkdir(parents=True,exist_ok=True)
out.write_text(json.dumps({'schema':'th12-original-std-samples/1','originalExeSha256':sha,
 'provider':'Original 0x430be0 camera setup and 0x4049a0 STD interpreter; constructed Stage and original stage01.std',
 'wholePresentationOracle':False,'gpuExecuted':False,'cameraDefaults':camera_defaults,'frames':frames},indent=2)+'\n',encoding='utf-8')
print(out)
