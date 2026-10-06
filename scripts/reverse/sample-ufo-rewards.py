"""Explicit isolated execution of retail UFO fill and death-reward decisions.

Never starts the game or alters EXE/DAT/replays/goldens. Rendering, sound, ECL
enemy creation and item allocation are fixture callbacks; original arithmetic,
branches, counters, reward-item requests and multiplier stores run in Unicorn.
"""
import pathlib,sys,struct,json,hashlib
TITLE=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(TITLE.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX,UC_X86_REG_ESI,UC_X86_REG_EIP
exe=TITLE.parent/'th12/th12.exe';identity=hashlib.sha256(exe.read_bytes()).hexdigest()
if identity!='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417':raise ValueError('reward addresses require pinned TH12 1.00b')
pe=pefile.PE(str(exe))
MOCKS={0x477420:'memset fixture',0x453e20:'sound event',0x461970:'ANM interrupt',0x4273f0:'requested item allocation',0x412990:'requested ECL enemy allocation',0x40fbe0:'visual spark allocation',0x4621c0:'ANM VM allocation',0x454d10:'ANM script binding',0x461250:'ANM handle registration'}
def run(entry,ufo_type,stage=1,item_type=1,fill=0.,power=400,p_count=10,point_count=20):
 vm=Uc(UC_ARCH_X86,UC_MODE_32);vm.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);vm.mem_write(0x400000,pe.get_memory_mapped_image())
 vm.mem_map(0x1000000,0x10000);vm.mem_map(0x2000000,0x20000)
 manager,enemy,anm,itempool,stack,stop=0x2000000,0x2001000,0x2005000,0x2008000,0x100e000,0x100f000
 def put(at,fmt,*values):vm.mem_write(at,struct.pack('<'+fmt,*values))
 def read(at,fmt):return struct.unpack('<'+fmt,vm.mem_read(at,struct.calcsize('<'+fmt)))
 put(stack,'I',stop);put(0x4b4534,'I',manager);put(0x4b0cb0,'i',stage);put(0x4b0c48,'i',power);put(0x4b0cd0,'i',400);put(0x4b0cd4,'i',100)
 put(0x4b0c54,'i',2);put(0x4b0c78,'i',2000000);put(manager+0x30,'i',ufo_type);put(manager+0x34,'I',enemy);put(manager+0x48,'ii',p_count,point_count);put(manager+0x50,'f',fill)
 put(enemy+0x1074,'fff',20.,100.,0.);put(0x4ce8cc,'I',0x2011000)
 # Reward visuals read a high-offset field on this owner. Keep it in the
 # fixture mapping by biasing the pointer, rather than mapping arbitrary memory.
 owner=0x2000000-0x4debdc+0x18000;put(0x4b43c8,'I',owner);put(owner+0x4debdc,'I',0x2019000)
 items=[];created_enemies=[];events=[];alloc_index=0
 def intercept(vm,address,size,unused):
  nonlocal alloc_index
  if address not in MOCKS:return
  sp=vm.reg_read(UC_X86_REG_ESP);ret=read(sp,'I')[0];cleanup=0;result=0
  if address==0x477420:
   ptr,value,length=read(sp+4,'III');vm.mem_write(ptr,bytes([value&255])*length);result=ptr
  elif address==0x4273f0:
   kind,position,angle,speed=read(sp+4,'IIff');allocated=itempool+alloc_index*0x1000;alloc_index+=1
   items.append({'type':kind,'position':list(read(position,'fff')),'angle':angle,'speed':speed,'fixturePointer':allocated});cleanup=16;result=allocated
  elif address==0x412990:
   name,parameters=read(sp+4,'II');raw=bytes(vm.mem_read(name,64)).split(b'\0')[0].decode('ascii');created_enemies.append({'name':raw,'rawArgumentsHex':bytes(vm.mem_read(parameters,0x50)).hex()});cleanup=8;result=0x201a000
  elif address==0x4621c0:result=anm
  elif address==0x461250:
   ptr=vm.reg_read(UC_X86_REG_EAX);put(ptr,'I',123);result=ptr
  elif address==0x40fbe0:cleanup=12
  elif address==0x454d10:cleanup=8
  elif address==0x453e20:events.append({'sound':vm.reg_read(UC_X86_REG_EAX)});cleanup=4
  elif address==0x461970:cleanup=4
  vm.reg_write(UC_X86_REG_EAX,result);vm.reg_write(UC_X86_REG_ESP,sp+4+cleanup);vm.reg_write(UC_X86_REG_EIP,ret)
 vm.hook_add(UC_HOOK_CODE,intercept);vm.reg_write(UC_X86_REG_ESP,stack);vm.reg_write(UC_X86_REG_EAX,item_type);vm.reg_write(UC_X86_REG_ESI,manager)
 vm.emu_start(entry,stop,count=100000)
 if vm.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('original decision did not return within budget')
 for item in items:
  kind,np,npt,mult=read(item.pop('fixturePointer')+0x9c8,'iiif');item['bonusFields']={'ufoType':kind,'powerCount':np,'pointCount':npt,'multiplier':mult}
 return {'parameters':{'entry':hex(entry),'ufoType':ufo_type,'stage':stage,'itemType':item_type,'fill':fill,'power':power,'powerCount':p_count,'pointCount':point_count},'fillAfter':read(manager+0x50,'f')[0],'powerCountAfter':read(manager+0x48,'i')[0],'pointCountAfter':read(manager+0x4c,'i')[0],'requestedItems':items,'requestedEnemies':created_enemies,'events':events,'popupMultiplier':read(manager+0x60,'f')[0],'popupScore':read(manager+0x68,'i')[0]}
records=[]
for color in [1,2,3,-1]:
 for stage in [1,6]:
  for item in [1,2,10]:records.append(run(0x44aca0,color,stage,item,0.,p_count=0,point_count=0))
 for filled in [.99,1.]:records.append(run(0x44aca0,color,1,1,filled,p_count=0,point_count=0))
 for fill in [0.,.25,.49999997,.5,.75,.99999994,1.,1.01]:records.append(run(0x44af00,color,fill=fill,power=400))
 if color==1:records.append(run(0x44af00,color,fill=1.,power=399))
report={'schema':'th12-ufo-isolated-reward-decisions/1','originalExeSha256':identity,'wholeGameDeterminism':False,'originalGameExecuted':False,'originalInstructionsExecuted':['0x44aca0','0x44af00'],'fixtureCallbacks':{hex(k):v for k,v in MOCKS.items()},'scope':'fill-counter-and-requested-death-rewards-only','records':records}
output=TITLE/'artifacts/reverse/ufo-reward-samples.json';output.parent.mkdir(parents=True,exist_ok=True);output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(output),'records':len(records),'wholeGameDeterminism':False}))
