"""Capture actual430a70 CPU eye/target/up construction before D3DX/GPU."""
import json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe));records=[]
cases=[('none',[0,0,-400],[20,300,200],[0,1,0],[0,0,0]),
 ('shake',[0,3000,-700],[0,300,330],[.05,1,0],[5,0,-5]),
 ('translation',[10,20,30],[1,2,3],[0,1,0],[-5,2,10]),
 ('fractional',[.1,4000.3,-700.1],[20.1,300.2,330.3],[.05,1,.02],[3.5,-.2,.1])]
for name,pos,direction,up,offset in cases:
 mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(0x400000,pe.get_memory_mapped_image());mu.mem_map(0x1000000,0x10000);mu.mem_map(0x2000000,0x10000)
 put=lambda p,f,*x:mu.mem_write(p,struct.pack('<'+f,*x))
 get=lambda p,f:list(struct.unpack('<'+f,mu.mem_read(p,struct.calcsize('<'+f))))
 camera,stack,stop=0x2000000,0x100e000,0x100f000
 put(camera,'9f',*pos,*direction,*up);put(camera+0x3c,'3f',*offset);put(0x4ce8cc,'I',0)
 out={'name':name,'position':get(camera,'3f'),'direction':get(camera+0xc,'3f'),'up':get(camera+0x18,'3f'),'offset':get(camera+0x3c,'3f')}
 def hook(uc,address,size,data):
  if address==0x46c6da:
   sp=mu.reg_read(UC_X86_REG_ESP);_,eye,target,up=get(sp+4,'4I')
   out.update(eye=get(eye,'3f'),target=get(target,'3f'),viewUp=get(up,'3f'));mu.emu_stop()
 mu.hook_add(UC_HOOK_CODE,hook);mu.reg_write(UC_X86_REG_FPCW,0x37f);mu.reg_write(UC_X86_REG_ESP,stack);mu.reg_write(UC_X86_REG_EDI,camera);put(stack,'I',stop)
 mu.emu_start(0x430a70,stop,count=100000)
 if mu.reg_read(UC_X86_REG_EIP)!=0x46c6da:raise RuntimeError('Original camera call not reached')
 records.append(out)
out=ROOT/'tests/fixtures/std-view-inputs-original.json'
out.write_text(json.dumps({'schema':'th12-original-std-view-inputs/1','originalExeSha256':EXE_SHA256,
 'provider':'Original430a70 prefix constructs actual LookAt eye/target/up; execution stops at D3DX call46c6da.',
 'wholeGameOracle':False,'gpuExecuted':False,'matrixHelperExecuted':False,'fixtures':['mapped Camera with synthetic float inputs; no ANMManager so no GPU flush'],'records':records},indent=2)+'\n',encoding='utf8');print(out)
