"""Actual430a70 view/projection inputs versus its final ANM screen-offset writes.

Graphics-library calls record arguments and return prepared identity matrices;
they do not supply an original projection result. Original430a70 arithmetic,
dispatch and final Camera+e8/ec -> ANMManager+b0/b4 copies execute untouched.
"""
import json, math, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
sys.path.insert(0, str(ROOT / 'scripts/reverse'))
import pefile
from native_anm_resource import check_original, EXE_SHA256
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import *
EXE = ROOT.parent / 'th12/th12.exe'
check_original(EXE)
PE = pefile.PE(str(EXE))
CAMERA, MANAGER, DEVICE, VTABLE = 0x2000000, 0x2010000, 0x2020000, 0x2021000
SP, STOP, SET_TRANSFORM = 0x100e000, 0x100f000, 0x100d000
records = []
for shift in [[0,0],[3.25,-2.5]]:
    mu = Uc(UC_ARCH_X86, UC_MODE_32)
    for address, size in [(0x400000, (PE.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),
                          (0x1000000,0x10000),(0x2000000,0x100000)]: mu.mem_map(address,size)
    mu.mem_write(0x400000, PE.get_memory_mapped_image())
    mu.reg_write(UC_X86_REG_FPCW,0x37f)
    def put(at, fmt, *values): mu.mem_write(at,struct.pack('<'+fmt,*values))
    def get(at, fmt): return list(struct.unpack('<'+fmt,mu.mem_read(at,struct.calcsize('<'+fmt))))
    def ret(pop=0, result=0):
        sp=mu.reg_read(UC_X86_REG_ESP);target=get(sp,'I')[0]
        mu.reg_write(UC_X86_REG_ESP,sp+4+pop);mu.reg_write(UC_X86_REG_EAX,result);mu.reg_write(UC_X86_REG_EIP,target)
    put(0x4ce8cc,'I',MANAGER);put(0x4ce8f0,'I',DEVICE);put(DEVICE,'I',VTABLE);put(VTABLE+0xb0,'I',SET_TRANSFORM)
    put(CAMERA,'9f',0,3000,-700,0,300,330,.05,1,0)
    put(CAMERA+0x3c,'3f',5,0,-5);put(CAMERA+0x48,'f',.5235987901687622)
    put(CAMERA+0xcc,'4I',13,0,422,480);put(CAMERA+0xe8,'2f',*shift)
    row={'screenShake':shift,'viewInputs':None,'projectionInputs':None,'setTransformKinds':[]}
    def hook(uc,at,size,user):
        sp=mu.reg_read(UC_X86_REG_ESP)
        if at==0x45a3c0:ret()
        elif at==0x46c6da:
            destination,eye,target,up=get(sp+4,'4I')
            row['viewInputs']={'eye':get(eye,'3f'),'target':get(target,'3f'),'up':get(up,'3f')}
            put(destination,'16f',*(1. if i%5==0 else 0. for i in range(16)));ret(16,destination)
        elif at==0x46c6e0:
            destination=get(sp+4,'I')[0];row['projectionInputs']=get(sp+8,'4f')
            put(destination,'16f',*(1. if i%5==0 else 0. for i in range(16)));ret(20,destination)
        elif at==0x46c6bc:
            destination,source=get(sp+4,'2I');vector=get(source,'3f');length=math.sqrt(sum(n*n for n in vector))
            put(destination,'3f',*(n/length if length else 0 for n in vector));ret(8,destination)
        elif at==SET_TRANSFORM:
            row['setTransformKinds'].append(get(sp+8,'I')[0]);ret(12)
    for at in [0x45a3c0,0x46c6da,0x46c6e0,0x46c6bc,SET_TRANSFORM]:mu.hook_add(UC_HOOK_CODE,hook,begin=at,end=at)
    put(SP,'I',STOP);mu.reg_write(UC_X86_REG_ESP,SP);mu.reg_write(UC_X86_REG_EDI,CAMERA)
    mu.emu_start(0x430a70,STOP,count=100000)
    if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Original430a70 camera budget')
    row['anmScreenOffset']=get(MANAGER+0xb0,'2f');records.append(row)
output=ROOT/'tests/fixtures/std-screen-shake-native.json'
output.write_text(json.dumps({'schema':'th12-native-std-screen-shake/1','originalExeSha256':EXE_SHA256,
 'wholeGameOracle':False,'gpuExecuted':False,'matrixHelperExecuted':False,
 'scope':'Complete original430a70 with synthetic Camera input and ANMManager/Device. Actual LookAt and Perspective arguments and final screen-offset writes; D3DX helpers record inputs and supply identity matrices, SetTransform records kind only. No native matrix/projection/pixel claim. Native45bce0 mode8 uses World/View/Projection transforms and never reads ANMManager+b0/b4; those offsets belong to459e50 flat XYZRHW quads.',
 'records':records},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(output),'records':records}))
