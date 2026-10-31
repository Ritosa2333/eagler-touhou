"""Explicit read-only original subsystem diagnostic; never run by npm tests.

Execute original TH12 option creation. ANM allocation/interrupt calls are isolated
presentation fixtures, returning zero IDs; position arithmetic executes in retail
instructions. This does not capture a complete game tick or accept a golden.
"""
import hashlib,json,pathlib,struct,sys
title=pathlib.Path(__file__).resolve().parents[1];workspace=title.parent
sys.path.insert(0,str(workspace/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX
exe=workspace/'th12/th12.exe';pe=pefile.PE(str(exe));base=pe.OPTIONAL_HEADER.ImageBase
mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(base,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(base,pe.get_memory_mapped_image())
STACK=0x1000000;PLAYER=0x2000000;SHT=0x3000000;STOP=0x4000000
for addr,size in [(STACK,0x10000),(PLAYER,0x100000),(SHT,0x10000),(STOP,4096)]:mu.mem_map(addr,size)
def read32(addr):return struct.unpack('<I',mu.mem_read(addr,4))[0]
def write32(addr,x):mu.mem_write(addr,struct.pack('<I',x&0xffffffff))
def ret(value,cleanup):
    sp=mu.reg_read(UC_X86_REG_ESP);target=read32(sp);mu.reg_write(UC_X86_REG_EAX,value);mu.reg_write(UC_X86_REG_ESP,sp+4+cleanup);mu.reg_write(UC_X86_REG_EIP,target)
created=[];shots=[];current_tick=0
def presentation(uc,address,size,data):
    sp=mu.reg_read(UC_X86_REG_ESP)
    if address==0x4615a0:
        out=read32(sp+8);script=read32(sp+12);created.append(script);write32(out,0);ret(out,16)
    elif address==0x41c6a0:
        out=read32(sp+4);write32(out,0);ret(out,8)
    elif address==0x461970:ret(0,4)
    elif address==0x439630:
        spec=read32(sp+4);shots.append({'tick':current_tick,'animation':struct.unpack('<h',mu.mem_read(spec+30,2))[0]});ret(0,8)
mu.hook_add(UC_HOOK_CODE,presentation)
rows=[]
for loadout in range(6):
    b=(title/'local/retail'/f'pl0{loadout//2}{"b" if loadout%2 else "a"}.sht').read_bytes()
    for count in range(1,5):
        for focus in [0,1]:
            created.clear();mu.mem_write(PLAYER,bytes(0xc59c));mu.mem_write(SHT,b)
            for addr,x in [(0x4b4514,PLAYER),(0x4b0c90,loadout//2),(0x4b0c94,loadout%2),(0x4b0c48,count*100),(0x4b0cd4,100),(0x4b0cd0,400),(PLAYER+0xa2c,SHT),(PLAYER+0x988,0),(PLAYER+0x98c,51200),(PLAYER+0xc598,focus)]:write32(addr,x)
            sp=STACK+0xf000;mu.mem_write(sp,struct.pack('<II',STOP,PLAYER));mu.reg_write(UC_X86_REG_ESP,sp)
            mu.emu_start(0x4385b0,STOP,count=1000000)
            if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('instruction budget exceeded')
            options=[struct.unpack('<2i',mu.mem_read(PLAYER+0x82bc+i*0xe4,8)) for i in range(count)]
            offsets=[struct.unpack_from('<2f',b,40+(count*(count-1)//2+i+focus*10)*8) for i in range(count)]
            expected=[(int(x*128),51200+int(y*128)) for x,y in offsets]
            if options!=expected:raise RuntimeError(f'original formation mismatch {loadout,count,focus,options,expected}')
            scripts=[18,19,11,12,15,16]
            if created!=[scripts[loadout]]*count:raise RuntimeError('original option script mismatch')
            rows.append({'loadout':loadout,'power':count*100,'focused':bool(focus),'position128':options,'scripts':created.copy()})
schedule_rows=[]
for release in [False,True]:
    shots.clear();mu.mem_write(PLAYER,bytes(0xc59c));b=(title/'local/retail/pl00b.sht').read_bytes();mu.mem_write(SHT,b)
    for i in range(10):write32(SHT+0x268+i*8,SHT+struct.unpack_from('<I',b,0x268+i*8)[0])
    for addr,x in [(0x4b4514,PLAYER),(0x4b0c48,100),(0x4b0cd4,100),(PLAYER+0xa2c,SHT),(PLAYER+0xa28,1),(PLAYER+0xc420,-1),(PLAYER+0xc424,-1),(PLAYER+0xc42c,0x4b2ed0),(PLAYER+0xc430,1)]:write32(addr,x)
    mu.mem_write(0x4b2ed0,struct.pack('<f',1))
    for current_tick in range(24):
        write32(0x4d49d0,1 if not release or current_tick==0 else 0)
        sp=STACK+0xf000;mu.mem_write(sp,struct.pack('<II',STOP,PLAYER));mu.reg_write(UC_X86_REG_ESP,sp);mu.emu_start(0x439a40,STOP,count=1000000)
        if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('schedule instruction budget exceeded')
    orb=[s['tick'] for s in shots if s['animation']==6];main=[s['tick'] for s in shots if s['animation']==0]
    expected_orb=[0,8] if release else [0,8,15,23]
    expected_main=[0,0,3,3,6,6,9,9,12,12] if release else [0,0,3,3,6,6,9,9,12,12,15,15,18,18,21,21]
    if orb!=expected_orb or main!=expected_main:raise RuntimeError(f'original firing schedule mismatch {release,orb,main}')
    schedule_rows.append({'releaseAfterFirstTick':release,'ticks':24,'orbSpawnTicks':orb,'mainSpawnTicks':main})
report={'scope':'TH12 original option creation and firing schedule only','wholeGameDeterminism':False,'exeSha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'provider':'original 0x4385b0 and 0x439a40 under Unicorn 2.1.4','presentationFixtures':['0x4615a0 ANM allocate zero ID','0x41c6a0 auxiliary allocate zero ID','0x461970 animation interrupt no-op'],'shotFixtures':['0x439630 records firing decisions without creating projectiles'],'cases':len(rows),'status':'pass','rows':rows,'firingSchedules':schedule_rows}
out=title/'artifacts/original-player-formations.json';out.write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n')
print(f'Original TH12 option creation: {len(rows)} cases passed; {out}')
