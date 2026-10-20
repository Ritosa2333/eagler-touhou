"""Execute original4018f0 numeric Font5 layout and45a570 geometry, GPU-free."""
import pathlib,sys,json,struct
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,install_resource,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe))
v=Uc(UC_ARCH_X86,UC_MODE_32)
for at,n in [(0,0x10000),(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),(0x2000000,0x300000),(0x3000000,0x400000)]:v.mem_map(at,n)
v.mem_write(0x400000,pe.get_memory_mapped_image())
FONT,RECORD,STACK,STOP=0x2000000,0x2200000,0x100e000,0x100f000
resource=install_resource(v,(ROOT/'local/retail/ascii.anm').read_bytes(),0x3000000,11)
def put(at,f,*x):v.mem_write(at,struct.pack('<'+f,*x))
def get(at,f):return list(struct.unpack('<'+f,v.mem_read(at,struct.calcsize('<'+f))))
def ret(pop=0):
 sp=v.reg_read(UC_X86_REG_ESP);v.reg_write(UC_X86_REG_EIP,get(sp,'I')[0]);v.reg_write(UC_X86_REG_ESP,sp+4+pop)
glyphs=[]
def hook(uc,at,size,user):
 if at==0x459e50:
  vm=v.reg_read(UC_X86_REG_EAX);sprite=(get(vm+0x3f4,'I')[0]-resource['sprites'])//72
  glyphs.append({'sprite':sprite,'position':get(vm+0x424,'fff'),'scale':get(vm+0x40,'ff'),'size':get(vm+0x58,'ff'),'color':get(vm+0x3bc,'I')[0],
   'vertices':[get(p,'ffffIff') for p in [0x4d47e8,0x4d4804,0x4d4820,0x4d483c]]})
  ret(4)
v.hook_add(UC_HOOK_CODE,hook)
rows=[]
for text in ['50','100','150','300','0','1234567890']:
 for ax,ay,sx,sy in [(0,0,2,2),(1,1,1,1),(2,2,1,1),(0,0,.75,1.25),(0,2,2,1),(2,0,1,2)]:
  v.mem_write(FONT,bytes(0x19000));v.mem_write(RECORD,bytes(0x138));put(FONT+0x18fb4,'I',resource['resource']);put(FONT+0x490,'I',3)
  v.mem_write(RECORD,text.encode()+b'\0');put(RECORD+0x100,'fffIff',224.25,116.75,0,0xd0ffffff,sx,sy);put(RECORD+0x120,'i',5);put(RECORD+0x130,'ii',ax,ay)
  v.reg_write(UC_X86_REG_FPCW,0x37f);v.reg_write(UC_X86_REG_MXCSR,0x1f80);put(STACK,'III',STOP,FONT,RECORD);v.reg_write(UC_X86_REG_ESP,STACK);glyphs.clear()
  v.emu_start(0x4018f0,STOP,count=300000)
  if v.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Native Font budget exhausted')
  rows.append({'text':text,'position':[224.25,116.75,0],'scale':[sx,sy],'alignment':[ax,ay],'color':0xd0ffffff,'glyphs':glyphs.copy()})
out=ROOT/'tests/fixtures/font5-native.json';out.write_text(json.dumps({'schema':'th12-native-font5/1','originalExeSha256':EXE_SHA256,'wholeGameOracle':False,'source':'Original4018f0/402390/45a570; onlyGPU459e50 intercepted; originalASCII sprite table','rows':rows},indent=2)+'\n')
print(json.dumps({'scenarios':len(rows),'glyphs':sum(len(x['glyphs']) for x in rows),'first':rows[0]},indent=2))
