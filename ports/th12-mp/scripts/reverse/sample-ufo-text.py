"""Capture original UFO text callbacks without executing GPU or game ticks.

Only font submission and handle/Enemy lookups are fixtures. Original
44a4f0/44a5d0 execute all display gates, arithmetic, and FontManager writes.
"""
import json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256

exe=ROOT.parent/'th12/th12.exe';check_original(exe)
pe=pefile.PE(str(exe));mu=Uc(UC_ARCH_X86,UC_MODE_32)
mu.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095)
mu.mem_write(0x400000,pe.get_memory_mapped_image())
STACK,STOP,FONT,UFO,ENEMY,LIST,VM=0x1000000,0x100f000,0x2000000,0x2200000,0x2210000,0x2250000,0x2260000
mu.mem_map(STACK,0x10000);mu.mem_map(FONT,0x300000)
def put(address,fmt,*values):mu.mem_write(address,struct.pack('<'+fmt,*values))
def read(address,fmt):return list(struct.unpack('<'+fmt,mu.mem_read(address,struct.calcsize('<'+fmt))))
def cstring(address):return bytes(mu.mem_read(address,128)).split(b'\0',1)[0].decode('ascii')
def ret(cleanup=0):
    sp=mu.reg_read(UC_X86_REG_ESP)
    mu.reg_write(UC_X86_REG_EIP,read(sp,'I')[0]);mu.reg_write(UC_X86_REG_ESP,sp+4+cleanup)
calls=[]
def hook(uc,address,size,data):
    sp=mu.reg_read(UC_X86_REG_ESP)
    if address in (0x4020a0,0x4021a0):
        position=read(mu.reg_read(UC_X86_REG_EDI if address==0x4020a0 else UC_X86_REG_EDX),'3f')
        call={'font':read(FONT+0x18f98,'i')[0],'position':position,
              'colorARGB':read(FONT+0x18f80,'I')[0],'scale':read(FONT+0x18f84,'2f'),
              'drawFlag':read(FONT+0x18f9c,'i')[0],'alignment':read(FONT+0x18fa4,'2i')}
        if address==0x4021a0:
            value=mu.reg_read(UC_X86_REG_ECX)
            call.update(groupedNumber=value,text=f'{value:,}')
        else:
            fmt=cstring(read(sp+4,'I')[0])
            args=read(sp+8,'d') if fmt=='*%1.1f' else read(sp+8,'2i' if fmt=='%2d:%.2d' else 'i')
            call.update(format=fmt,arguments=args,text=fmt%tuple(args))
        calls.append(call);ret()
    elif address==0x412530:mu.reg_write(UC_X86_REG_EAX,ENEMY);ret()
    elif address==0x461920:mu.reg_write(UC_X86_REG_EAX,VM);ret(4)
mu.hook_add(UC_HOOK_CODE,hook)
mu.reg_write(UC_X86_REG_FPCW,0x37f);mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
put(0x4b43b8,'I',FONT);put(0x4b43dc,'I',LIST);put(0x4ce8cc,'I',0x2270000)
put(0x4ce568,'2I',0x1234,0);put(0x4ce560,'2I',0xbeef,0)
def call(address):
    sp=STACK+0xe000;put(sp,'I',STOP)
    mu.reg_write(UC_X86_REG_ESP,sp);mu.reg_write(UC_X86_REG_EBX,UFO)
    mu.emu_start(address,STOP,count=30000)
    if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Original callback exceeded budget')

base={'enemyId':42,'age':60,'duration':720,'bucketA':12,'bucketB':34,'fill':.75,
      'enemy':{'id':42,'x':40,'y':128,'z':0},'displayFrames':0,'multiplier':6,
      'score':1234560,'rewardX':264,'rewardY':144,'rewardZ':0}
cases=[]
def add(name,**updates):cases.append((name,{**base,**updates}))
for age in [0,19,20,59,60,70,71,719,720,721]:add('age-'+str(age),age=age)
for remaining in [1,2,3,5,7,11,13,17,19,29,59,60,61,65,67,119,120,121,659,660,661,719,720]:
    add('remaining-'+str(remaining),duration=base['age']+remaining)
for display in [1,4,5,9,10,119,120]:add('display-'+str(display),displayFrames=display)
add('reward-without-UFO',enemyId=0,enemy=None,displayFrames=120)
add('absent-enemy',enemy=None)
add('mismatched-enemy',enemy={'id':99,'x':40,'y':128,'z':0})
add('empty-score',displayFrames=120,score=0)
add('fractional-fill',fill=.29,bucketA=1,bucketB=999)
add('fractional-position',enemy={'id':42,'x':-100.125,'y':205.375,'z':0},rewardX=123.25,rewardY=456.125,displayFrames=120)
records=[]
for name,state in cases:
    calls.clear();mu.mem_write(UFO,bytes(0x100));mu.mem_write(FONT,bytes(0x19000));mu.mem_write(VM,bytes(0x500))
    put(FONT+0x18f80,'I',0xffffffff);put(FONT+0x18f84,'2f',1,1);put(FONT+0x18fa4,'2i',1,1)
    put(UFO+0x14,'i',state['age']);put(UFO+0x28,'i',state['duration']);put(UFO+0x34,'2I',ENEMY,state['enemyId'])
    put(UFO+0x44,'I',VM);put(UFO+0x48,'2if',state['bucketA'],state['bucketB'],state['fill'])
    put(UFO+0x54,'3f',state['rewardX'],state['rewardY'],state['rewardZ'])
    put(UFO+0x60,'f',state['multiplier']);put(UFO+0x64,'2i',state['displayFrames'],state['score'])
    enemy=state['enemy'];put(LIST+0x68,'I',LIST+0x100 if enemy else 0)
    put(LIST+0x100,'2I',ENEMY,0);put(ENEMY+0x27b4,'I',enemy['id'] if enemy else 0)
    if enemy:put(ENEMY+0x1074,'3f',enemy['x'],enemy['y'],enemy['z'])
    put(ENEMY+0x2648,'2i',1234,1900)
    before=read(0x4ce568,'2I')+read(0x4ce560,'2I')
    call(0x44a4f0);call(0x44a5d0)
    after=read(0x4ce568,'2I')+read(0x4ce560,'2I');assert after==before
    state['fill']=read(UFO+0x50,'f')[0]
    records.append({'name':name,'input':state,'calls':calls.copy(),'healthScale':read(VM+0x40,'f')[0],
                    'healthEnginePosition':read(VM+0x430,'3f'),'rngUnchanged':True})
out=ROOT/'tests/fixtures/ufo-text-native.json'
out.write_text(json.dumps({'schema':'th12-native-ufo-text/1','originalExeSha256':EXE_SHA256,
    'provider':'Actual44a4f0/44a5d0; no candidate runtime, no game/ANM ticks.',
    'fixtureCallbacks':['4020a0/4021a0 capture native format/arguments and FontManager state; text formatting is Python, no glyphs/GPU executed.',
                        '412530 returns the matching mapped live Enemy;461920 returns a mapped handle VM.'],
    'wholeGameOracle':False,'wholePresentationOracle':False,'gpuExecuted':False,'records':records},indent=2)+'\n',encoding='utf8')
print(json.dumps({'path':str(out),'cases':len(records),'fontCalls':sum(len(r['calls']) for r in records)}))
