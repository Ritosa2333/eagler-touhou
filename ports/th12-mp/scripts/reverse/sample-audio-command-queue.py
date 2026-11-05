"""Original BGM queue append, not sound playback or a threaded audio Oracle."""
import json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe));samples=[]
for occupied,command,index,name in [(0,1,7,'bgm/th12_07.wav'),(0,2,0,''),(0,4,12,'probe'),(30,1,1,'bgm/th12_01.wav'),(31,1,2,'bgm/th12_02.wav')]:
 mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(0x400000,pe.get_memory_mapped_image())
 STACK,OBJ,TEXT,STOP=0x1000000,0x2000000,0x2010000,0x100f000
 mu.mem_map(STACK,0x10000);mu.mem_map(OBJ,0x20000)
 put=lambda at,fmt,*v:mu.mem_write(at,struct.pack('<'+fmt,*v))
 read=lambda at,fmt:list(struct.unpack('<'+fmt,mu.mem_read(at,struct.calcsize('<'+fmt))))
 for n in range(occupied):put(OBJ+0x1fec+n*0x10c,'3I',0x7fffffff,n,123)
 put(0x4cee78,'I',0);mu.mem_write(TEXT,name.encode('ascii')+b'\0');sp=STACK+0xe000;put(sp,'3I',STOP,command,index)
 mu.reg_write(UC_X86_REG_ESP,sp);mu.reg_write(UC_X86_REG_EAX,OBJ);mu.reg_write(UC_X86_REG_EDI,TEXT)
 mu.emu_start(0x454960,STOP,count=10000)
 if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Original BGM queue budget')
 slots=[]
 for n in range(31):
  at=OBJ+0x1fec+n*0x10c;values=read(at,'3I')
  if values[0]:slots.append({'slot':n,'command':values[0],'index':values[1],'field8':values[2],'nameHex':bytes(mu.mem_read(at+12,256)).split(b'\0',1)[0].hex()})
 samples.append({'occupied':occupied,'input':{'command':command,'index':index,'name':name},'slotsAfter':slots})
out=ROOT/'artifacts/reverse/audio-command-queue-original.json';out.parent.mkdir(parents=True,exist_ok=True)
out.write_text(json.dumps({'schema':'th12-original-audio-command-queue/1','originalExeSha256':EXE_SHA256,
 'provider':'Actual454960 with threading bit4cee78:8000 disabled and synthetic mapped object; no callback fixtures.',
 'threadingExecuted':False,'audioPlaybackExecuted':False,'wholeGameOracle':False,'samples':samples},indent=2)+'\n',encoding='utf8')
print(json.dumps({'path':str(out),'cases':len(samples)}))
