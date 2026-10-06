"""Original stage restore 43c730; synthetic header records, no disk I/O."""
import itertools,json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_MEM_INVALID
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256,install_resource
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe));samples=[]
for stage,power,lives,life_frag,bombs,bomb_frag,colors in [
 (1,143,6,4,2,2,[1,2,3]),(2,0,2,0,2,0,[0,0,0]),(3,500,4,2,0,1,[1,1,1]),
 (4,99,0,3,4,2,[1,2,1]),(5,400,3,2,2,2,[3,2,1]),(6,0,-1,0,0,0,[3,3,3]),(7,400,8,1,8,1,[2,2,2])]:
 mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(0x400000,pe.get_memory_mapped_image())
 STACK,MANAGER,HEADER,STOP=0x1000000,0x2000000,0x2010000,0x100f000
 mu.mem_map(STACK,0x10000);mu.mem_map(MANAGER,0x600000)
 put=lambda at,fmt,*v:mu.mem_write(at,struct.pack('<'+fmt,*v))
 read=lambda at,fmt:list(struct.unpack('<'+fmt,mu.mem_read(at,struct.calcsize('<'+fmt))))
 def invalid(uc,access,address,size,value,data):
  print(f'stage{stage}: invalid {address:x} from {mu.reg_read(UC_X86_REG_EIP):x}',flush=True);return False
 mu.hook_add(UC_HOOK_MEM_INVALID,invalid)
 HUD,RESOURCE,ANM=0x2020000,0x2100000,0x2500000
 install_resource(mu,(ROOT/'local/retail/front.anm').read_bytes(),RESOURCE)
 put(0x4b2ed0,'f',1);put(0x4b43e4,'I',HUD);put(0x4ce8cc,'I',ANM)
 for n in [1,2,3]:
  put(STACK+0xe000,'3I',STOP,HUD+0x54b8+n*0x4b4,80);mu.reg_write(UC_X86_REG_ESP,STACK+0xe000);mu.reg_write(UC_X86_REG_ECX,RESOURCE)
  mu.emu_start(0x454d10,STOP,count=100000)
 put(0x4b4518,'I',MANAGER);put(MANAGER+0x10,'i',1);put(0x4b0cb0,'i',stage);put(MANAGER+stage*36+0xb8,'I',HEADER)
 put(0x4b0cd4,'i',100);put(0x4b0cd0,'i',400);put(HEADER+2,'H',0x1234)
 put(HEADER+0xc,'i',1234567);put(HEADER+0x10,'h',power);put(HEADER+0x14,'i',12345000)
 put(HEADER+0x18,'4h',lives,life_frag,bombs,bomb_frag);put(HEADER+0x20,'3i',*colors)
 put(HEADER+0x2c,'i',42);put(HEADER+0x38,'i',314);put(HEADER+0x3c,'i',2000);put(0x4ce568,'II',0xabcd,123)
 put(STACK+0xe000,'I',STOP);mu.reg_write(UC_X86_REG_ESP,STACK+0xe000);mu.reg_write(UC_X86_REG_FPCW,0x37f)
 mu.emu_start(0x43c730,STOP,count=20000)
 if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Original replay restore budget')
 samples.append({'stage':stage,'headerInputs':{'power':power,'lives':lives,'lifeFragments':life_frag,'bombs':bombs,'bombFragments':bomb_frag,'colors':colors},
  'rngSeedCounter':read(0x4ce568,'II'),'scoreStored':read(0x4b0c44,'i')[0],'power':read(0x4b0c48,'i')[0],
  'pivStored':read(0x4b0c78,'i')[0],'lives':read(0x4b0c98,'i')[0],'lifeFragments':read(0x4b0c9c,'i')[0],
  'bombs':read(0x4b0ca0,'i')[0],'bombFragments':read(0x4b0ca4,'i')[0],
  'inventorySevenWords':read(0x4b0c4c,'7i'),'rank':read(0x4b0ccc,'i')[0],'graze':read(0x4b0cc4,'i')[0],'field_cd8':read(0x4b0cd8,'i')[0]})
out=ROOT/'artifacts/reverse/replay-stage-restore-original.json';out.parent.mkdir(parents=True,exist_ok=True)
out.write_text(json.dumps({'schema':'th12-original-replay-stage-restore/1','originalExeSha256':EXE_SHA256,
 'provider':'Actual43c730 and422e80 with synthetic header pointer for each stage; no function callback fixtures. Inventory mismatch executes421a60/455630 on three synthetic HUD VMs bound to actual front80 bytecode; complete HUD constructor is excluded.',
 'wholeGameOracle':False,'originalFileLoaded':False,'diskIOExecuted':False,'samples':samples},indent=2)+'\n',encoding='utf8')
print(json.dumps({'path':str(out),'cases':len(samples)}))
