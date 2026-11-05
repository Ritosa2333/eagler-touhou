"""Original2000000 extension initialization and40c230 movement callbacks.

This directly exercises the subsystem helper, followed by original4099e0's
ordinary rate-scaled position integration formula. No ANM/collision stub or
gameplay decision runs, and no full-world determinism claim is made.
"""
import json,math,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe))
v=Uc(UC_ARCH_X86,UC_MODE_32)
for a,n in [(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),(0x2000000,0x30000),(0x3000000,4096)]:v.mem_map(a,n)
v.mem_write(0x400000,pe.get_memory_mapped_image());v.reg_write(UC_X86_REG_FPCW,0x37f)
BULLET,STOP,SP=0x2000000,0x3000000,0x100f000
def put(a,f,*x):v.mem_write(a,struct.pack('<'+f,*x))
def get(a,f):return list(struct.unpack('<'+f,v.mem_read(a,struct.calcsize('<'+f))))
def f32(x):return struct.unpack('<f',struct.pack('<f',x))[0]
def call(a):
 put(SP,'I',STOP);v.reg_write(UC_X86_REG_ESP,SP);v.reg_write(UC_X86_REG_ECX,BULLET);v.reg_write(UC_X86_REG_EAX,BULLET)
 v.emu_start(a,STOP,count=500000)
 if v.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('native instruction budget')
 return v.reg_read(UC_X86_REG_EAX)
def snap():return dict(position=get(BULLET+0x4bc,'2f'),velocity=get(BULLET+0x4c8,'2f'),speed=get(BULLET+0x4d4,'f')[0],angle=get(BULLET+0x4d8,'f')[0],active=get(BULLET+0x528,'I')[0],elapsed=get(BULLET+0x8d4,'iif')[:3],interpolation=get(BULLET+0x9d8,'iif')[:3],target=get(BULLET+0x8f0,'2f'))
cases=[]
for mode,relative,duration,rate,target in [(9,False,d,r,(-7.282229423522949,133.5649871826172)) for d in [0,1,2,3,180] for r in [1.,.5,1.5]]+[(m,True,23,1.,(76.,-48.)) for m in [0,1,2,3,4,5,6,9,10,11,12,13,14,15,16]]:
 initial=dict(position=[12.,120.],angle=.3,speed=2.);record=dict(a=target[0],b=target[1],c=duration,d=mode|(0x100 if relative else 0),kind=0x2000000,concurrent=True)
 v.mem_write(BULLET,bytes(0x9f8));put(BULLET+0x4bc,'3f',*initial['position'],0);put(BULLET+0x4d4,'2f',initial['speed'],initial['angle']);put(BULLET+0x4c8,'3f',math.cos(initial['angle'])*initial['speed'],math.sin(initial['angle'])*initial['speed'],0);put(BULLET+0x550,'ffiiII',record['a'],record['b'],record['c'],record['d'],record['kind'],1);put(0x4b2ed0,'f',rate)
 call(0x40aa60);frames=[dict(frame=-1,**snap())]
 for frame in range(math.ceil(max(duration,0)/rate)+2):
  complete=call(0x40c230);before=snap();pos=get(BULLET+0x4bc,'2f');vel=get(BULLET+0x4c8,'2f');put(BULLET+0x4bc,'2f',*[f32(p+f32(q*rate)) for p,q in zip(pos,vel)]);frames.append(dict(frame=frame,returnValue=complete,beforeIntegration=before,**snap()))
  if complete:break
 cases.append(dict(record=record,initial=initial,gameRate=rate,frames=frames))
 assert frames[-1]['returnValue']==1
 assert all(math.isfinite(x) for f in frames for x in f['position']+f['velocity'])
out=ROOT/'tests/native/bullet-target-motion-original.json'
out.write_text(json.dumps(dict(schema='th12-bullet-target-motion-native/1',originalExeSha256=EXE_SHA256,partial=True,wholeGameOracle=False,scope='40aa60 start2000000 + direct original40c230 + documented409de2 rate integration;15 scalar easing modes and0/1/2/3/180 duration with rate0.5/1/1.5',fixtures=['zeroed Bullet state and interpolation globals, no Player/ANM/collision boundary called','ordinary position integration applied after helper; no next-command/cancel dispatch is claimed'],records=cases),indent=2)+'\n')
print(json.dumps(dict(output=str(out),cases=len(cases),callbacks=sum(len(c['frames'])-1 for c in cases))))
