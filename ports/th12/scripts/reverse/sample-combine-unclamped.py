"""Read-only original413700 evidence: sum/delta/add/floor without clamp.

Original1.00b and CRT execute under Unicorn. No host math or candidate logic is
installed in the VM. This is partial coordinate evidence, not replay golden.
"""
import hashlib,json,pathlib,random,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
sys.path.insert(0,str(pathlib.Path(__file__).resolve().parent))
import pefile
from native_anm_resource import check_original,EXE_SHA256
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
EXE=ROOT.parent/'th12/th12.exe';check_original(EXE)
PE=pefile.PE(str(EXE));ES,SP,STOP=0x2000000,0x100d000,0x100f000
def put(vm,address,fmt,*values):vm.mem_write(address,struct.pack('<'+fmt,*values))
def get(vm,address,fmt):return list(struct.unpack('<'+fmt,vm.mem_read(address,struct.calcsize('<'+fmt))))
def f32(x):return struct.unpack('<f',struct.pack('<f',x))[0]
def execute(old,absolute,relative):
    vm=Uc(UC_ARCH_X86,UC_MODE_32)
    vm.mem_map(0x400000,(PE.OPTIONAL_HEADER.SizeOfImage+4095)&~4095)
    vm.mem_write(0x400000,PE.get_memory_mapped_image())
    vm.mem_map(0x1000000,0x10000);vm.mem_map(ES,0x40000)
    put(vm,0x4b2ed0,'f',1);put(vm,0x4d52d4,'I',1)
    vm.reg_write(UC_X86_REG_MXCSR,0x1f80);vm.reg_write(UC_X86_REG_FPCW,0x037f)
    put(vm,ES+0x34,'3f',*old);put(vm,ES+0x68,'3f',*absolute);put(vm,ES+0x9c,'3f',*relative)
    # Flag10000 and combined circle/ellipse flags remain zero.
    quantizations=[]
    def hook(mu,address,size,unused):
        pointer=mu.reg_read(UC_X86_REG_ESI)
        quantizations.append({'pointer':pointer,'caller':get(mu,mu.reg_read(UC_X86_REG_ESP),'I')[0],
                              'before':get(mu,pointer,'3f')})
    vm.hook_add(UC_HOOK_CODE,hook,begin=0x465320,end=0x465320)
    put(vm,SP,'I',STOP);vm.reg_write(UC_X86_REG_ESP,SP);vm.reg_write(UC_X86_REG_ESI,ES)
    vm.emu_start(0x413700,STOP,count=100000)
    if vm.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Native combine budget')
    return {'old':old,'absolute':absolute,'relative':relative,
            'combined':get(vm,ES+0x34,'3f'),'delta':get(vm,ES+0x40,'3f'),
            'absoluteAfter':get(vm,ES+0x68,'3f'),'relativeAfter':get(vm,ES+0x9c,'3f'),
            'combinedBits':get(vm,ES+0x34,'3I'),'quantizationCalls':quantizations}
cases=[]
values=[-192.01,-128.,-4.53000020980835,-.019999980926513672,-.009999752044677734,
        -.005,-.0001,0.,.0001,.005,4.53000020980835,127.97999572753906,448.01]
for index,value in enumerate(values):
    v=f32(value)
    cases.append(execute([0.,128.,3.125],[0.,128.,1.125],[v,f32(-v),2.5]))
    cases.append(execute([f32(-v),128.,3.125],[v,128.,1.125],[f32(-.01),f32(.09),2.5]))
generator=random.Random(413700)
for index in range(128):
    old=[f32(generator.uniform(-192,192)),f32(generator.uniform(0,448)),3.125]
    absolute=[f32(generator.uniform(-192,192)),f32(generator.uniform(0,448)),1.125]
    relative=[f32(generator.uniform(-16,16)),f32(generator.uniform(-16,16)),2.5]
    cases.append(execute(old,absolute,relative))
for row in cases:
    if row['absoluteAfter']!=row['absolute'] or row['relativeAfter']!=row['relative']:
        raise RuntimeError('Unclamped combine modified its component')
    if len(row['quantizationCalls'])!=1:raise RuntimeError('Wrong native floor count')
out=ROOT/'tests/fixtures/enemy-combine-unclamped-native.json'
out.write_text(json.dumps({'schema':'th12-enemy-combine-unclamped-native/1',
    'originalExeSha256':EXE_SHA256,'provider':'Actual413700/464db0/465320/CRT493290; no execution stubs',
    'partial':True,'acceptedReplayGolden':False,'wholeGameOracle':False,
    'scope':'XY combine with clamp flag10000 clear; Z included to document no465320 Z floor',
    'cases':cases},indent=2)+'\n',encoding='utf-8')
print(str(out));print(len(cases),'native calls; each1floor call, component positions unchanged')
