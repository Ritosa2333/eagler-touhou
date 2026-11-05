"""Run actual priority12 STD callback to distinguish hidden geometry from pause."""
import pathlib,sys,struct,json,math
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe));std=(ROOT/'local/retail/stage01.std').read_bytes();rows=[]
for flags,age in [(0,0),(1,0),(4,29),(4,30),(4,59),(4,60),(8,0),(9,0)]:
    vm=Uc(UC_ARCH_X86,UC_MODE_32);vm.mem_map(0,4096);vm.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);vm.mem_write(0x400000,pe.get_memory_mapped_image())
    vm.mem_map(0x1000000,0x10000);vm.mem_map(0x2000000,0x40000)
    stage,code,header,stack,stop=0x2000000,0x2010000,0x2030000,0x100e000,0x100f000
    put=lambda at,fmt,*v:vm.mem_write(at,struct.pack('<'+fmt,*v))
    read=lambda at,fmt:list(struct.unpack('<'+fmt,vm.mem_read(at,struct.calcsize('<'+fmt))))
    def hook(uc,address,size,data):
        if address==0x46c6bc:
            # D3DXVec3Normalize is a graphics-library dependency. The callback
            # guard and actual4049a0 remain original executable instructions.
            sp=vm.reg_read(UC_X86_REG_ESP);dest,source=read(sp+4,'2I');v=read(source,'fff');length=math.sqrt(sum(x*x for x in v))
            put(dest,'fff',*(x/length if length else 0 for x in v));vm.reg_write(UC_X86_REG_EAX,dest);vm.reg_write(UC_X86_REG_ESP,sp+12);vm.reg_write(UC_X86_REG_EIP,read(sp,'I')[0])
    vm.hook_add(UC_HOOK_CODE,hook);vm.reg_write(UC_X86_REG_FPCW,0x37f)
    start=struct.unpack_from('<I',std,8)[0];vm.mem_write(code,std[start:]);put(stage+0x10,'I',header);put(stage+0x1c,'I',code);put(stage+0x4c,'I',code)
    put(stage+0x38,'iifII',-1,0,0,0x4b2ed0,1);put(stage+0x35bc,'I',flags);put(stage+0x35c4,'i',age);put(stage+0x3618,'fff',0,0,1);put(stage+0x3624,'fff',0,1,0)
    put(0x4b2ed0,'f',1);put(0x4b43c0,'I',stage);samples=[]
    for tick in range(2):
        put(stack,'I',stop);vm.reg_write(UC_X86_REG_ESP,stack);vm.reg_write(UC_X86_REG_ECX,stage);vm.emu_start(0x403ec0,stop,count=200000)
        if vm.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Native STD callback budget')
        samples.append({'tick':tick,'scriptClock':read(stage+0x3c,'i')[0],'callbackCount':read(stage+0x35d8,'i')[0],
            'position':read(stage+0x360c,'fff'),'flags':read(stage+0x35bc,'I')[0]})
    rows.append({'initialFlags':flags,'transitionAge':age,'samples':samples})
out=ROOT/'tests/fixtures/std-visibility-original.json';out.write_text(json.dumps({'schema':'th12-original-std-tick-gate-samples/1',
    'originalExeSha256':EXE_SHA256,'provider':'Original403ec0->403020->4049a0; original stage01.std commands; objectCount0 omits ANM object ticks; D3DXnormalize fixture',
    'wholeGameDeterminism':False,'wholePresentationOracle':False,'gpuExecuted':False,'records':rows},indent=2)+'\n',encoding='utf-8');print(out)
