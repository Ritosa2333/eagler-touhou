"""Original40a150 draw-time heading, stopping at45c900 before GPU submission."""
import pathlib,sys,struct,json
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe))
mu=Uc(UC_ARCH_X86,UC_MODE_32)
for address,size in [(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),(0x2000000,0x10000)]:mu.mem_map(address,size)
mu.mem_write(0x400000,pe.get_memory_mapped_image())
MANAGER,BULLET,STACK,STOP=0x2000000,0x2001000,0x100e000,0x100f000
def put(at,fmt,*values):mu.mem_write(at,struct.pack('<'+fmt,*values))
def read(at,fmt):return list(struct.unpack('<'+fmt,mu.mem_read(at,struct.calcsize('<'+fmt))))
def hook(uc,at,size,user):
    if at==0x45c900:
        sp=mu.reg_read(UC_X86_REG_ESP);target=read(sp,'I')[0]
        mu.reg_write(UC_X86_REG_ESP,sp+8);mu.reg_write(UC_X86_REG_EIP,target)
mu.hook_add(UC_HOOK_CODE,hook,begin=0x45c900,end=0x45c900)
mu.reg_write(UC_X86_REG_FPCW,0x37f);mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
rows=[]
for heading in [-3.1415927410125732,-2.7,-1.5707963705062866,-.25,0,.25,1.5707963705062866,2.7,3.1415927410125732]:
    for enabled in [False,True]:
        mu.mem_write(BULLET,bytes(0xa00));put(MANAGER+0x14,'I',BULLET)
        put(BULLET+0x4bc,'fff',13.25,128.5,0);put(BULLET+0x4d8,'f',heading)
        put(BULLET+0x34,'f',.375);put(BULLET+0x484,'I',3|(0x20000000 if enabled else 0))
        put(STACK,'I',STOP);mu.reg_write(UC_X86_REG_ESP,STACK)
        mu.reg_write(UC_X86_REG_EAX,MANAGER);mu.reg_write(UC_X86_REG_ECX,0)
        before=read(0x4ce568,'II');mu.emu_start(0x40a150,STOP,count=1000)
        if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Native heading budget')
        rows.append({'angle':read(BULLET+0x4d8,'f')[0],'rotate':enabled,'initialRotation':.375,
                     'rotation':read(BULLET+0x34,'f')[0],'flags':read(BULLET+0x484,'I')[0],
                     'rngUnchanged':before==read(0x4ce568,'II')})
out=ROOT/'tests/fixtures/bullet-heading-native.json'
out.write_text(json.dumps({'schema':'th12-native-bullet-heading/1','originalExeSha256':EXE_SHA256,
    'gpuExecuted':False,'wholeGameOracle':False,'source':'Original40a150/464640;45c900 submission intercepted',
    'rows':rows},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'cases':len(rows),'output':str(out)}))
