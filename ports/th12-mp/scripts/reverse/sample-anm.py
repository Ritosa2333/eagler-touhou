"""Explicit original ANM VM arithmetic/flags sampling, without game or GPU startup."""
import pathlib,sys,struct,json,hashlib
TITLE=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(TITLE.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_FPCW
exe=TITLE.parent/'th12/th12.exe';sha=hashlib.sha256(exe.read_bytes()).hexdigest()
if sha!='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417':raise ValueError('Pinned TH12 1.00b required')
pe=pefile.PE(str(exe))
def command(op,formats,values,time=0):
 raw=struct.pack('<'+formats,*values);return struct.pack('<HHhH',op,len(raw)+8,time,0)+raw
fixtures=[
 ('transform_uv_flags',[(48,'fff',[12,24,4]),(49,'fff',[.1,.2,.3]),(50,'ff',[1.25,.75]),(53,'fff',[.01,.02,.03]),(70,'f',[-.0005]),(71,'f',[.001]),(65,'I',[65537]),(66,'i',[1]),(67,'i',[1]),(89,'i',[1])],4),
 ('primary_secondary_color',[(51,'i',[133]),(52,'iii',[120,180,240]),(77,'i',[211]),(76,'iii',[10,20,30]),(80,'i',[1]),(87,'i',[1])],2),
 ('color_interpolation',[(51,'i',[255]),(52,'iii',[255,128,16]),(58,'iii',[10,0,0]),(57,'iiiii',[10,0,32,64,128])],12),
 ('scale_interpolation',[(50,'ff',[0,0]),(60,'iiff',[10,4,1.4,1.4])],12),
]
records=[]
for name,instructions,ticks in fixtures:
 vm=Uc(UC_ARCH_X86,UC_MODE_32);vm.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);vm.mem_write(0x400000,pe.get_memory_mapped_image());vm.mem_map(0x1000000,0x10000);vm.mem_map(0x2000000,0x20000)
 obj,code,stack,stop=0x2000000,0x2010000,0x100e000,0x100f000
 def put(at,fmt,*v):vm.mem_write(at,struct.pack('<'+fmt,*v))
 def read(at,fmt):return list(struct.unpack('<'+fmt,vm.mem_read(at,struct.calcsize('<'+fmt))))
 script=b''.join(command(*c) for c in instructions)+command(63,'',[])+struct.pack('<HHhH',65535,8,0,0)
 vm.mem_write(code,script);put(0x4b2ed0,'f',1.);put(obj+0x40,'ff',1.,1.);put(obj+0x3bc,'II',0xffffffff,0xffffffff);put(obj+0x68,'iifII',-1,0,0.,0x4b2ed0,1);put(obj+0x3ec,'II',code,code);put(obj+0x47c,'I',3);put(obj+0x480,'I',1)
 vm.reg_write(UC_X86_REG_FPCW,0x37f);frames=[]
 for tick in range(ticks):
  put(stack,'II',stop,obj);vm.reg_write(UC_X86_REG_ESP,stack);vm.emu_start(0x455630,stop,count=100000)
  if vm.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('ANM VM instruction budget exceeded')
  frames.append({'tick':tick,'rotation':read(obj+0x24,'fff'),'scale':read(obj+0x40,'ff'),'uv':read(obj+0x60,'ff'),'primary':hex(read(obj+0x3bc,'I')[0]),'secondary':hex(read(obj+0x3c0,'I')[0]),'position':read(obj+0x424,'fff'),'flags':hex(read(obj+0x47c,'I')[0]),'flags2':hex(read(obj+0x480,'I')[0]),'clock':read(obj+0x6c,'i')[0]})
 records.append({'name':name,'instructions':[{'op':op,'formats':formats,'values':values} for op,formats,values in instructions],'frames':frames})
out=TITLE/'artifacts/reverse/anm-vm-samples.json';out.parent.mkdir(parents=True,exist_ok=True);out.write_text(json.dumps({'schema':'th12-original-anm-vm-samples/1','originalExeSha256':sha,'provider':'0x455630 original VM arithmetic; constructed VM and sprite-free scripts','wholeGameDeterminism':False,'wholePresentationOracle':False,'gpuExecuted':False,'records':records},indent=2)+'\n',encoding='utf-8');print(str(out))
