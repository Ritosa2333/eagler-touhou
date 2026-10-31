"""Explicit isolated retail item-tick traces, with rendering calls intercepted.

This is a subsystem diagnostic, not full-game Oracle or ordinary test input.
"""
import pathlib,sys,struct,json,hashlib
TITLE=pathlib.Path(__file__).resolve().parents[2];WORKSPACE=TITLE.parent
sys.path.insert(0,str(WORKSPACE/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX
EXE=WORKSPACE/'th12/th12.exe';SHA='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
if hashlib.sha256(EXE.read_bytes()).hexdigest()!=SHA:raise SystemExit('Wrong retail version')
pe=pefile.PE(str(EXE))
def put(v,a,f,*x):v.mem_write(a,struct.pack('<'+f,*x))
def get(v,a,f):return struct.unpack('<'+f,v.mem_read(a,struct.calcsize(f)))
cases=[
 {'name':'token_move','type':10,'state':6,'position':[175,200],'velocity':[2,0],'age':100,'timer':0,'frames':[[0,400,0,0]]},
 {'name':'changing_near60','type':13,'state':6,'position':[60,0],'velocity':[0,0],'age':100,'timer':30,'frames':[[0,0,0,0]]},
 {'name':'changing_far','type':13,'state':6,'position':[60.001,0],'velocity':[0,0],'age':100,'timer':30,'frames':[[0,0,0,0]]},
 {'name':'changing_switch','type':13,'state':6,'position':[100,200],'velocity':[0,0],'age':100,'timer':150,'frames':[[-180,400,0,0]]},
 {'name':'token_expire','type':10,'state':6,'position':[207,200],'velocity':[2,0],'age':1200,'timer':0,'frames':[[0,400,0,0]]},
 {'name':'ordinary_sticky','type':1,'state':1,'position':[-180,100],'velocity':[0,-2.2],'age':0,'timer':0,'frames':[[0,100,0,0],[150,400,0,0]]},
 {'name':'ordinary_death_window','type':1,'state':3,'position':[-180,100],'velocity':[0,0],'age':0,'timer':0,'attractionSpeed':5,'frames':[[150,400,0,4]]},
 {'name':'ordinary_explosion_no_pickup','type':1,'state':1,'position':[0,400],'velocity':[0,0],'age':0,'timer':0,'frames':[[0,400,0,2]]},
 {'name':'small_point_delay','type':9,'state':5,'position':[0,400],'velocity':[0,-2.2],'age':0,'timer':0,'frames':[[0,400,0,0],[0,400,0,0]]},
 {'name':'aggregate_delay','type':18,'state':8,'position':[0,128],'velocity':[0,0],'age':59,'timer':0,'frames':[[150,400,0,0],[150,400,0,0],[150,400,0,0]]},
 {'name':'ufo_absorb_enter','type':1,'state':1,'position':[100,200],'velocity':[0,-2.2],'age':0,'timer':0,'ufo':True,'frames':[[0,400,0,0]]},
 {'name':'ufo_absorb_collect','type':2,'state':7,'position':[0,128],'velocity':[0,0],'age':0,'timer':0,'ufo':True,'frames':[[0,400,0,0]]},
]
for ordinary_type in range(1,10):
 for power in (100,400):cases.append({'name':f'ordinary_reward_{ordinary_type}_{power}','type':ordinary_type,'state':1,'position':[0,400],'velocity':[0,-2.2],'age':0,'timer':0,'power':power,'frames':[[0,400,0,0]]})
cases.extend([
 {'name':'life_piece_completes','type':4,'state':1,'position':[0,400],'velocity':[0,0],'age':0,'timer':0,'lifeFragments':4,'frames':[[0,400,0,0]]},
 {'name':'bomb_piece_completes','type':5,'state':1,'position':[0,400],'velocity':[0,0],'age':0,'timer':0,'bombFragments':4,'frames':[[0,400,0,0]]},
])
records=[]
for case in cases:
 v=Uc(UC_ARCH_X86,UC_MODE_32);v.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);v.mem_write(0x400000,pe.get_memory_mapped_image());v.mem_map(0x1000000,0x10000);v.mem_map(0x2000000,0x100000);v.mem_map(0x2100000,0x670000);v.mem_map(0x2800000,0x500000)
 manager,player,ufo,enemy=0x2100000,0x2000000,0x2010000,0x2060000
 slot=0x14 if case['type']<9 else 0x17afd4 if case['type']==9 else 0x171254;item=manager+slot
 for a,x in [(0x4b44f0,manager),(0x4b4514,player),(0x4b4534,ufo),(0x4b43e4,0x2030000),(0x4b43dc,0x2040000),(0x4b43c8,0x2800000)]:put(v,a,'I',x)
 put(v,0x2800000+0x4debdc,'I',0x2020000);put(v,0x4b2ed0,'f',1);put(v,0x4b0c48,'i',case.get('power',100));put(v,0x4b0cd0,'ii',400,100);put(v,0x4b0c78,'i',1000000);put(v,0x4b0cf4,'i',20000000);put(v,0x4b0c98,'iiii',2,case.get('lifeFragments',0),2,case.get('bombFragments',0));put(v,0x4b0c58,'i',0)
 put(v,item+0x96c,'fff',*case['position'],0);put(v,item+0x978,'fff',*case['velocity'],0);put(v,item+0x9b0,'ii',case['state'],case['type']);put(v,item+0x9bc,'f',case.get('attractionSpeed',0));put(v,item+0x9c8,'iiif',1,0,0,1)
 for off,timer in [(0x988,case['age']),(0x99c,case['timer'])]:put(v,item+off,'iifII',timer-1,timer,float(timer),0x4b2ed0,1)
 put(v,player+0xa2c,'I',0x2070000);put(v,0x2070000+8,'f',5)
 if case.get('ufo'):
  put(v,ufo+0x14,'i',100);put(v,ufo+0x2c,'iii',1,1,enemy);put(v,ufo+0x38,'i',77);put(v,enemy+0x27b4,'i',77);put(v,enemy+0x1074,'fff',0,128,0);put(v,0x2040000+0x68,'I',0x2050000);put(v,0x2050000,'II',enemy,0)
 calls=[]
 def intercept(v,a,size,unused):
  pops={0x43e250:8,0x453e20:4,0x453d90:0,0x4385b0:4,0x454d10:8,0x4061e0:8,0x455630:4,0x461970:4,0x461a70:4,0x402520:0,0x41ce60:12,0x41cf40:12,0x420f90:4}
  sp=v.reg_read(UC_X86_REG_ESP)
  if a in [0x4270b0,0x44aca0]:calls.append({'entry':hex(a),'value':v.reg_read(UC_X86_REG_EAX)});pops[a]=0
  if a not in pops:return
  ret=get(v,sp,'I')[0];v.reg_write(UC_X86_REG_ESP,sp+4+pops[a]);v.reg_write(UC_X86_REG_EIP,ret)
 v.hook_add(UC_HOOK_CODE,intercept);frames=[]
 for x,y,focus,state in case['frames']:
  put(v,player+0x97c,'fff',x,y,0);put(v,player+0xa28,'i',state);put(v,0x4d49d0,'I',8 if focus else 0)
  for off,half in [(0xc444,30),(0xc45c,50),(0xc474,30)]:put(v,player+off,'ffffff',x-half,y-half,0,x+half,y+half,0)
  sp,stop=0x100d000,0x100f000;put(v,sp,'II',stop,manager);v.reg_write(UC_X86_REG_ESP,sp);v.emu_start(0x425c20,stop,count=1000000)
  frames.append({'state':get(v,item+0x9b0,'i')[0],'type':get(v,item+0x9b4,'i')[0],'position':get(v,item+0x96c,'ff'),'velocity':get(v,item+0x978,'ff'),'age':get(v,item+0x98c,'i')[0],'timer':get(v,item+0x9a0,'i')[0],'attractionSpeed':get(v,item+0x9bc,'f')[0],'aggregateSpeed':get(v,item+0x984,'f')[0],'power':get(v,0x4b0c48,'i')[0],'scoreStored':get(v,0x4b0c44,'i')[0],'pivStored':get(v,0x4b0c78,'i')[0],'rank':get(v,0x4b0ccc,'i')[0],'lifeAndBombCounters':get(v,0x4b0c98,'iiii')})
 records.append({'input':case,'frames':frames,'interceptedPickupCalls':calls})
output=TITLE/'artifacts/reverse/item-frame-inspection.json';output.write_text(json.dumps({'schema':'th12-item-frame-inspection/1','originalExeSha256':SHA,'scope':'original ItemManager full tick with fake graphics resources; graphics and UFO pickup side effects intercepted','wholeGameDeterminism':False,'records':records},indent=2)+'\n',encoding='utf-8');print(output)
