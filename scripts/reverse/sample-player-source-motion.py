"""Original4390f0 circle birth and436f90..437098 Source16 motion/timer.

Synthetic circles, actual CPU functions and raw floating-point stores. No
Player shots, query damage, GPU, replay/golden writes or core logic stubs.
"""
import json, pathlib, struct, sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'));sys.path.insert(0,str(ROOT/'scripts/reverse'))
from native_anm_resource import check_original, EXE_SHA256
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import *
EXE=ROOT.parent/'th12/th12.exe';check_original(EXE);PE=pefile.PE(str(EXE))
PLAYER,POSITION,SP,STOP=0x2000000,0x2010000,0x100d000,0x100f000
rows=[]
for x,y in [(147.92999267578125,417.3299865722656),(142.42999267578125,411.260009765625),(-142.42999267578125,411.260009765625),(0,120),(12.019999504089355,14.010000228881836),(188,432)]:
 mu=Uc(UC_ARCH_X86,UC_MODE_32)
 for address,size in [(0x400000,(PE.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),(PLAYER,0x20000)]:mu.mem_map(address,size)
 mu.mem_write(0x400000,PE.get_memory_mapped_image());mu.reg_write(UC_X86_REG_FPCW,0x37f);mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
 put=lambda p,fmt,*v:mu.mem_write(p,struct.pack('<'+fmt,*v))
 get=lambda p,fmt:list(struct.unpack('<'+fmt,mu.mem_read(p,struct.calcsize('<'+fmt))))
 put(0x4b2ed0,'f',1);put(POSITION,'3f',x,y,.1)
 put(SP,'IIffii',STOP,POSITION,12,1.399999976158142,15,4);mu.reg_write(UC_X86_REG_ESP,SP);mu.reg_write(UC_X86_REG_EAX,PLAYER)
 mu.emu_start(0x4390f0,STOP,count=100000)
 if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Circle ctor budget')
 source=PLAYER+0x8988
 def state():return {'position':get(source+0x18,'2f'),'radius':get(source,'f')[0],'timer':get(source+0x4c,'2i'),'flags':get(source+0x70,'I')[0]}
 before=state();frames=[]
 for tick in range(5):
  put(SP,'I',STOP);mu.reg_write(UC_X86_REG_ESP,SP);mu.reg_write(UC_X86_REG_EDI,PLAYER)
  mu.emu_start(0x436f90,0x437098,count=100000)
  if mu.reg_read(UC_X86_REG_EIP)!=0x437098:raise RuntimeError('Source16 loop budget')
  frames.append(state())
 rows.append({'initial':{'position':[x,y],'radius':12,'growth':1.399999976158142,'ticks':15,'damage':4},'birth':before,'frames':frames})
output=ROOT/'tests/fixtures/player-source-motion-original.json'
output.write_text(json.dumps({'schema':'th12-original-player-source-motion/1','originalExeSha256':EXE_SHA256,'wholeGameOracle':False,'gpuExecuted':False,'boundary':'Actual4390f0 ctor +436f90..437098 Source16 loop, including464db0/465320/493290; source velocities explicitly ctor-zero, rate1, six synthetic positions, no instruction hooks.','rows':rows},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'cases':len(rows),'firstCenters':[r['frames'][0]['position'] for r in rows],'output':str(output)}))
