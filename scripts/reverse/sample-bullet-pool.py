"""Original physical BulletManager search and after-birth cursor writes."""
import pathlib,sys,struct,json
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);p=pefile.PE(str(exe));v=Uc(UC_ARCH_X86,UC_MODE_32)
for at,n in [(0x400000,(p.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),(0x2000000,0x600000)]:v.mem_map(at,n)
v.mem_write(0x400000,p.get_memory_mapped_image());M,STACK,STOP=0x2000000,0x100e000,0x100f000
def put(at,f,*x):v.mem_write(at,struct.pack('<'+f,*x))
def get(at,f):return struct.unpack('<'+f,v.mem_read(at,struct.calcsize('<'+f)))
def slot(n):return M+0x64+n*0x9f8
def stop_hook(uc,at,size,user):
 if at in [0x40aa0a,0x40aa1b,STOP]:v.emu_stop()
v.hook_add(UC_HOOK_CODE,stop_hook)
rows=[]
for cursor in [0,1,5,99,999,1995,1998,1999]:
 for free in [None,[],[0],[1],[5],[1999],[0,7,500,1997]]:
  for i in range(2000):put(slot(i)+0x532,'H',0 if free is None or i in free else 1)
  put(slot(2000)+0x532,'H',5);put(M+0x10,'I',slot(cursor));put(STACK,'6I',STOP,M,0,0,0,0);v.reg_write(UC_X86_REG_ESP,STACK)
  v.emu_start(0x40a250,0x40a34d,count=100000)
  found=v.reg_read(UC_X86_REG_EIP)==0x40a34d;chosen=(v.reg_read(UC_X86_REG_EBX)-slot(0))//0x9f8 if found else -1
  if found:
   put(STACK+0x24,'I',M);v.reg_write(UC_X86_REG_ESP,STACK);v.reg_write(UC_X86_REG_EBX,slot(chosen));v.emu_start(0x40a9f0,STOP,count=100)
  after=(get(M+0x10,'I')[0]-slot(0))//0x9f8
  rows.append({'cursor':cursor,'free':free,'selected':chosen,'cursorAfterCommit':after})
out=ROOT/'tests/fixtures/bullet-pool-native.json';out.write_text(json.dumps({'schema':'th12-native-bullet-pool/1','originalExeSha256':EXE_SHA256,'wholeGameOracle':False,'scope':'original40a250 search until40a34d; original40a9f0 cursor suffix after birth; no emitter/RNG/ANM substitute','rows':rows},indent=2)+'\n');print(f'{len(rows)} original physical pool cases recorded.')
