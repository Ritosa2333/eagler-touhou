"""Execute the shipped CN/JP practice overlays with real MZ relocations."""
from pathlib import Path
import gzip,json,struct
from unicorn import Uc,UC_ARCH_X86,UC_MODE_16,UC_HOOK_CODE,UC_HOOK_INSN,UC_HOOK_INTR
from unicorn.x86_const import *
ROOT=Path(__file__).resolve().parents[1]
BASE=0x1000
def guest(spec):
    exe=gzip.decompress((ROOT/'web'/spec['url']).read_bytes())
    h=struct.unpack_from('<14H',exe);image=bytearray(exe[h[4]*16:]);rel=set()
    for i in range(h[3]):
        off,seg=struct.unpack_from('<HH',exe,h[12]+i*4);at=seg*16+off;rel.add(at)
        struct.pack_into('<H',image,at,(struct.unpack_from('<H',image,at)[0]+BASE)&65535)
    u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x200000);u.mem_write(BASE*16,bytes(image))
    return u,rel
def main():
    checked=0
    for lang,spec in json.loads((ROOT/'web/practice-assets.json').read_text())['languages'].items():
        p=spec['patch'];e=p['practice']['entries'];cs=BASE+p['code_segment'];ds=BASE+p['data_segment'];cb=cs*16;db=ds*16
        def init(u):
            regs={UC_X86_REG_CS:cs,UC_X86_REG_DS:ds,UC_X86_REG_ES:0x9000,UC_X86_REG_SS:0x8000,
                  UC_X86_REG_SP:0xff00,UC_X86_REG_EAX:0x12345678,UC_X86_REG_EBX:0x23456789,
                  UC_X86_REG_ECX:0x3456789a,UC_X86_REG_EDX:0x456789ab,UC_X86_REG_ESI:0x56789abc,
                  UC_X86_REG_EDI:0x6789abcd,UC_X86_REG_EBP:0x789abcde,UC_X86_REG_EFLAGS:0x246}
            for r,v in regs.items():u.reg_write(r,v)
            u.mem_write(0x8ff00,struct.pack('<H',0xfff0));return regs
        for preparing in (0,1):
            u,_=guest(spec);init(u)
            u.mem_write(db+0xba86,struct.pack('<HH',0,0x9000))
            u.mem_write(0x9000b,b'\x07');u.mem_write(0x9000d,b'\x06')
            u.mem_write(db+0x4664,b'\x01');u.mem_write(db+0x4676,b'\x05');u.mem_write(db+0xbccc,struct.pack('<H',123))
            u.mem_write(db+0x5395,b'\x0a');u.mem_write(cb+e['resourceDirty'],b'\x00')
            u.mem_write(cb+e['targetPreparing'],bytes([preparing]));u.mem_write(cb+e['initialLives'],b'\x02')
            u.mem_write(cb+e['initialBombs'],b'\x03');u.mem_write(cb+e['initialPower'],b'\x40');u.mem_write(cb+e['initialRank'],b'\x12')
            u.emu_start(cb+e['targetResources'],cb+0xfff0,count=1000)
            assert u.mem_read(0x9000b,1)==bytes([3 if preparing else 7])
            assert u.mem_read(0x9000d,1)==bytes([3 if preparing else 6])
            assert u.mem_read(db+0x4664,1)==bytes([64 if preparing else 1])
            assert u.mem_read(db+0x4676,1)==bytes([0 if preparing else 5])
            assert struct.unpack('<H',u.mem_read(db+0xbccc,2))[0]==(0 if preparing else 123)
            assert u.mem_read(db+0x5395,1)==bytes([18 if preparing else 10])
            assert u.mem_read(cb+e['resourceDirty'],1)==bytes([3 if preparing else 0]);checked+=1
        for entry in (0,1,2):
            u,_=guest(spec);init(u);u.mem_write(cb+e['targetPreparing'],b'\x01');u.mem_write(cb+e['initialEntry'],bytes([entry]))
            u.mem_write(db+0xbcd0,struct.pack('<HH',0x1234,0x5678));u.mem_write(db+0x53c0,struct.pack('<H',93))
            u.mem_write(db+0x4646,b'\x00\x00\x00\x00');u.mem_write(db+0x46b6,b'\x00\x00\x00\x00')
            u.mem_write(db+0x4277,b'\x09');u.mem_write(db+0x53c5,b'\x04');u.mem_write(db+0x53c2,struct.pack('<H',100))
            u.emu_start(cb+e['targetWarp'],cb+0xfff0,count=1000)
            assert bytes(u.mem_read(db+0x4646,4))==(struct.pack('<HH',0x1234,0x5678) if entry else bytes(4))
            if entry==1:assert struct.unpack('<H',u.mem_read(db+0x538a,2))[0]==93
            if entry==2:
                assert bytes(u.mem_read(db+0x46b6,4))==struct.pack('<HH',0x1234,0x5678)
                assert u.mem_read(db+0x4277,1)==b'\x00' and u.mem_read(db+0x53c5,1)==b'\xff'
                assert u.mem_read(db+0x53c2,2)==b'\x00\x00'
            checked+=1
        target_cases=[(0,0,0,0,0,0,1),(1,0,2,0,0,0,0),(1,0,3,0,0,0,1),
                      (2,2,2,0,0,0,0),(2,2,3,0,0,0,1),(2,0,3,4,0,0,0),
                      (2,0,3,3,0,0,1),(2,6,2,0,1,0x6013,0),(2,6,2,0,1,0xc7da,1)]
        for entry,stage,phase,wanted,gen,boss,expected in target_cases:
            u,_=guest(spec);init(u);u.mem_write(cb+e['targetPreparing'],b'\x01');u.mem_write(cb+e['initialEntry'],bytes([entry]))
            u.mem_write(cb+e['initialPhase'],bytes([wanted]));u.mem_write(cb+e['initialGengetsu'],bytes([gen]))
            u.mem_write(db+0x5394,bytes([stage]));u.mem_write(db+0x53d9,bytes([phase]));u.mem_write(db+0x53c5,bytes([phase]))
            u.mem_write(db+0xbcd0,struct.pack('<H',boss or (0x6013 if entry==2 else 0x11c0)))
            u.mem_write(db+0x46b6,struct.pack('<H',0x6013 if entry==1 else 0x11c0))
            u.emu_start(cb+e['targetCheck'],cb+0xfff0,count=1000)
            assert u.mem_read(cb+e['targetReached'],1)==bytes([expected]),(lang,entry,stage,phase,wanted,gen,boss)
            assert u.mem_read(cb+e['targetPreparing'],1)==bytes([1-expected])
            assert u.mem_read(cb+e['arrivalPending'],1)==bytes([expected])
            assert u.mem_read(cb+e['entryPause'],1)==bytes([expected])
            assert u.mem_read(cb+e['cleanupPending'],1)==bytes([int(bool(expected and entry))])
            checked+=1
        u,_=guest(spec);init(u);u.mem_write(cb+e['arrivalPending'],b'\x01')
        for at in (0x1a66,0x4662,0x4663,0x4669,0x466a,0xbcca,0xbcba):u.mem_write(db+at,b'\x05')
        for at in (0x4349,0x435a):u.mem_write(db+at,bytes([9])*8)
        for at in (0x4656,0x4658,0x3974):u.mem_write(db+at,struct.pack('<H',123))
        u.emu_start(cb+e['targetArrival'],cb+0xfff0,count=1000)
        for at in (0x1a66,0x4662,0x4663,0x4669,0x466a,0xbcca,0xbcba):assert u.mem_read(db+at,1)==b'\x00'
        for at in (0x4349,0x435a):assert bytes(u.mem_read(db+at,8))==bytes(8)
        for at in (0x4656,0x4658,0x3974):assert u.mem_read(db+at,2)==b'\x00\x00'
        assert u.mem_read(cb+e['arrivalPending'],1)==b'\x00';checked+=1
        for entry,hp,maximum,minimum,expected in ((0,100,800,300,800),(1,0,500,0,500),(1,600,500,0,500),(1,50,500,0,50),(2,100,1200,300,301),(2,2000,1200,300,1200),(2,500,1200,300,500)):
            u,_=guest(spec);init(u);u.mem_write(cb+e['initialEntry'],bytes([entry]));u.mem_write(cb+e['initialHp'],struct.pack('<H',hp))
            at=0x53c2 if entry==1 else 0x53d6
            u.mem_write(db+at,struct.pack('<H',maximum));u.mem_write(db+0x53e0,struct.pack('<H',minimum))
            u.emu_start(cb+e['targetHp'],cb+0xfff0,count=1000)
            assert struct.unpack('<H',u.mem_read(db+at,2))[0]==expected,(lang,entry,hp,maximum,minimum)
            checked+=1
        u,_=guest(spec);init(u);u.mem_write(db+0xba86,struct.pack('<HH',0,0x9000));u.mem_write(0x9000b,b'\x01')
        u.mem_write(cb+e['targetPractice'],b'\x01');u.mem_write(cb+e['initialEntry'],b'\x02');u.mem_write(cb+e['initialStage'],b'\x00');u.mem_write(cb+e['initialPhase'],b'\x03')
        u.mem_write(db+0x5394,b'\x00');u.mem_write(db+0x53d9,b'\x03');u.mem_write(db+0xbcd0,struct.pack('<H',0x6013))
        def step(name):
            u.reg_write(UC_X86_REG_SP,0xff00);u.mem_write(0x8ff00,struct.pack('<H',0xfff0));u.emu_start(cb+e[name],cb+0xfff0,count=2000)
        u.mem_write(cb+e['arrivalPending'],b'\x01');step('targetArrival');step('targetOutcomeStep')
        assert u.mem_read(cb+e['targetOutcome'],1)==b'\x00'
        u.mem_write(db+0x53d9,b'\x04');u.mem_write(db+0x4669,b'\x01');step('targetOutcomeStep');assert u.mem_read(cb+e['targetOutcome'],1)==b'\x00'
        u.mem_write(db+0x4669,b'\x00');step('targetOutcomeStep');assert u.mem_read(cb+e['targetOutcome'],1)==b'\x01' and u.mem_read(cb+e['hold'],1)==b'\x01'
        u.mem_write(db+0x53d9,b'\x03');u.mem_write(cb+e['arrivalPending'],b'\x01');step('targetArrival');u.mem_write(cb+e['hold'],b'\x00')
        u.mem_write(db+0x466a,b'\x05');step('targetOutcomeStep');assert u.mem_read(cb+e['targetOutcome'],1)==b'\x00'
        u.mem_write(db+0x466a,b'\x00');step('targetOutcomeStep');assert u.mem_read(cb+e['targetOutcome'],1)==b'\x02' and u.mem_read(cb+e['hold'],1)==b'\x01';checked+=1
        u.mem_write(cb+e['initialStage'],b'\x06');u.mem_write(cb+e['initialPhase'],b'\x00');u.mem_write(cb+e['initialGengetsu'],b'\x00')
        u.mem_write(db+0x5394,b'\x06');u.mem_write(db+0x53d9,b'\x02');u.mem_write(cb+e['arrivalPending'],b'\x01');step('targetArrival');u.mem_write(cb+e['hold'],b'\x00');step('targetOutcomeStep')
        u.mem_write(db+0x53d9,b'\xfe');step('targetOutcomeStep');assert u.mem_read(cb+e['targetOutcome'],1)==b'\x00'
        u.mem_write(db+0xbcd0,struct.pack('<H',0xc7da));u.mem_write(db+0x53d9,b'\x02');step('targetOutcomeStep')
        u.mem_write(db+0x53d9,b'\xfe');step('targetOutcomeStep');assert u.mem_read(cb+e['targetOutcome'],1)==b'\x01';checked+=1
        for stage,phase,wanted,reimu,gen,ptr,expected in ((0,2,3,0,0,0x6013,299),(0,3,3,0,0,0x6013,1000),(3,4,6,1,0,0x6013,299),(3,4,6,0,0,0x6013,1000),(6,2,0,0,1,0x6013,299),(6,2,0,0,1,0xc7da,1000),(6,2,3,0,1,0xc7da,299)):
            u,_=guest(spec);init(u);u.mem_write(cb+e['targetPreparing'],b'\x01');u.mem_write(cb+e['initialEntry'],b'\x02')
            u.mem_write(cb+e['initialPhase'],bytes([wanted]));u.mem_write(cb+e['initialReimu'],bytes([reimu]));u.mem_write(cb+e['initialGengetsu'],bytes([gen]))
            u.mem_write(db+0x5394,bytes([stage]));u.mem_write(db+0x53d9,bytes([phase]));u.mem_write(db+0xbcd0,struct.pack('<H',ptr))
            u.mem_write(db+0x53d6,struct.pack('<H',1000));u.mem_write(db+0x53e0,struct.pack('<H',300))
            u.emu_start(cb+e['targetAdvance'],cb+0xfff0,count=1000)
            assert struct.unpack('<H',u.mem_read(db+0x53d6,2))[0]==expected,(lang,stage,phase,wanted,reimu,gen,ptr)
            checked+=1
        # Earlier survival attack HP is retained by the guest and restored
        # before applying an optional custom HP at the selected arrival.
        u,_=guest(spec);init(u);u.mem_write(cb+e['targetPreparing'],b'\x01');u.mem_write(cb+e['initialEntry'],b'\x02');u.mem_write(cb+e['initialPhase'],b'\x10')
        u.mem_write(db+0x5394,b'\x05');u.mem_write(db+0x53d9,b'\x0e');u.mem_write(db+0xbcd0,struct.pack('<H',0x6013))
        u.mem_write(db+0x53d6,struct.pack('<H',1200));u.mem_write(db+0x53e0,b'\0\0')
        u.emu_start(cb+e['targetAdvance'],cb+0xfff0,count=2000);assert u.mem_read(db+0x53d6,2)==b'\0\0'
        u.mem_write(cb+e['arrivalPending'],b'\1');u.mem_write(cb+e['initialHp'],b'\0\0');u.emu_start(cb+e['targetArrival'],cb+0xfff0,count=2000)
        assert struct.unpack('<H',u.mem_read(db+0x53d6,2))[0]==1200;checked+=1
        for stage,phase,ptr,frame,expected in ((5,0,0x6013,1,128),(5,1,0x6013,1,63),(6,3,0x6013,1,63),(6,5,0x6013,1,127),(6,6,0xc7da,2,127),(5,0,0x6013,0,0)):
            u,_=guest(spec);init(u);u.mem_write(cb+e['targetPreparing'],b'\x01');u.mem_write(cb+e['initialEntry'],b'\x02');u.mem_write(cb+e['initialPhase'],b'\x08')
            u.mem_write(db+0x5394,bytes([stage]));u.mem_write(db+0x53d9,bytes([phase]));u.mem_write(db+0xbcd0,struct.pack('<H',ptr));u.mem_write(db+0x53da,struct.pack('<H',frame))
            u.emu_start(cb+e['targetAdvance'],cb+0xfff0,count=2000)
            assert struct.unpack('<H',u.mem_read(db+0x53da,2))[0]==expected,(lang,stage,phase,ptr,frame)
            checked+=1
        u,_=guest(spec);init(u);u.mem_write(cb+e['targetPreparing'],b'\x01');u.mem_write(cb+e['initialEntry'],b'\x02');u.mem_write(cb+e['initialPhase'],b'\x08')
        u.mem_write(db+0x5394,b'\x06');u.mem_write(db+0x53d9,b'\x03');u.mem_write(db+0x53dd,b'\xff');u.mem_write(db+0xbcd0,struct.pack('<H',0xc7da))
        u.mem_write(db+0x53da,struct.pack('<H',20));u.mem_write(db+0x53ca,struct.pack('<h',3000));u.mem_write(db+0x53d2,struct.pack('<h',-12))
        u.emu_start(cb+e['targetAdvance'],cb+0xfff0,count=2000)
        assert struct.unpack('<H',u.mem_read(db+0x53da,2))[0]==64
        assert struct.unpack('<h',u.mem_read(db+0x53ca,2))[0]==3000-(64-20)*12;checked+=1
        for name,native,ports in [('load',0xad03-p['code_segment']*16,[0x7fe0]),('frame',0x50,[0x7fe1]),
                                 ('player',p['entries'][1],[0x7fe2,0x7fe3]),('world',p['entries'][7],[0x7fe4])]:
            u,_=guest(spec);regs=init(u);u.mem_write(cb+native,b'\xc3');u.mem_write(db+0x428a,struct.pack('<H',native));seen=[]
            u.hook_add(UC_HOOK_INSN,lambda uc,port,size,value,data:seen.append((port,size,value)) if size==4 else None,None,1,0,UC_X86_INS_OUT)
            u.hook_add(UC_HOOK_INTR,lambda uc,number,data: None if number==0x18 else (_ for _ in ()).throw(AssertionError(number)))
            u.emu_start(cb+e[name],cb+0xfff0,count=1000)
            assert seen==[(port,4,ds<<16|cs) for port in ports],(lang,name,seen)
            for r,v in regs.items():assert u.reg_read(r)==v+(2 if r==UC_X86_REG_SP else 0),(lang,name,r)
            checked+=1
        for answer in (0,1):
            u,_=guest(spec);regs=init(u);order=[];wipe=0x10d4b-p['code_segment']*16;pause=p['entries'][4];native=0x50
            for ip in (wipe,native):u.mem_write(cb+ip,b'\xc3')
            u.mem_write(cb+pause,b'\xb8'+struct.pack('<H',answer)+b'\xc3');u.mem_write(cb+e['entryPause'],b'\x01');u.mem_write(db+0x5392,b'\x00')
            u.mem_write(db+0x428a,struct.pack('<H',native))
            def ordering(uc,at,size,data):
                if at in (cb+wipe,cb+pause,cb+native):order.append({cb+wipe:'wipe',cb+pause:'pause',cb+native:'std'}[at])
            u.hook_add(UC_HOOK_CODE,ordering);u.hook_add(UC_HOOK_INSN,lambda *args:None,None,1,0,UC_X86_INS_OUT)
            u.emu_start(cb+e['frame'],cb+0xfff0,count=1000)
            assert order==['wipe','pause','std'];assert u.mem_read(cb+e['entryPause'],1)==b'\x00'
            assert u.mem_read(db+0x5392,1)==bytes([answer])
            for r,v in regs.items():assert u.reg_read(r)==v+(2 if r==UC_X86_REG_SP else 0)
            checked+=1
        # The arrival callback must run after the far native Boss update and
        # before a redraw of this page. Full play never emits this callback.
        for target,pause in ((0,0),(0,1),(1,0),(1,1)):
            u,_=guest(spec);regs=init(u);order=[];native=0x60;redraw=0x70
            u.mem_write(cb+native,b'\xcb');u.mem_write(cb+redraw,b'\xc3')
            u.mem_write(db+0xbcd0,struct.pack('<HH',native,cs))
            u.mem_write(db+0x4268,struct.pack('<H',redraw))
            u.mem_write(cb+e['targetPractice'],bytes([target]))
            def boss_order(uc,at,size,data):
                if at==cb+native:order.append('boss')
                if at==cb+redraw:order.append('redraw')
            def arrival(uc,port,size,value,data):
                assert (port,size,value)==(0x7fe5,4,ds<<16|cs)
                order.append('arrival');uc.mem_write(cb+e['entryPause'],bytes([pause]))
            u.hook_add(UC_HOOK_CODE,boss_order)
            u.hook_add(UC_HOOK_INSN,arrival,None,1,0,UC_X86_INS_OUT)
            u.emu_start(cb+e['boss'],cb+0xfff0,count=1000)
            assert order==['boss']+(['arrival']+(['redraw'] if pause else []) if target else []),(lang,target,pause,order)
            for reg,value in regs.items():assert u.reg_read(reg)==value+(2 if reg==UC_X86_REG_SP else 0),(lang,'boss',reg)
            checked+=1
        for keep in (0,1):
            u,_=guest(spec);regs=init(u);native=0x60;redraw=0x70
            u.mem_write(cb+native,b'\xcb');u.mem_write(cb+redraw,b'\xc3')
            u.mem_write(db+0xbcd0,struct.pack('<HH',native,cs));u.mem_write(db+0x4268,struct.pack('<H',redraw))
            u.mem_write(cb+e['targetPractice'],b'\x01');u.mem_write(cb+e['cleanupKeepBits'],bytes([keep]))
            pools=[(0x5a22,26,440),(0x42f0,24,2),(0x9292,42,16),(0xaf34,20,32),(0x9594,10,16),(0xb204,26,32)]
            for offset,stride,count in pools:
                for i in range(count):u.mem_write(db+offset+stride*i,b'\x01\x5a')
            u.mem_write(db+0xbcb9,b'\x0c\x2d')
            def arrival_cleanup(uc,port,size,value,data):
                uc.mem_write(cb+e['entryPause'],b'\x01');uc.mem_write(cb+e['cleanupPending'],b'\x01')
            u.hook_add(UC_HOOK_INSN,arrival_cleanup,None,1,0,UC_X86_INS_OUT)
            u.emu_start(cb+e['boss'],cb+0xfff0,count=20000)
            for pool,(offset,stride,count) in enumerate(pools):
                for i in range(count):
                    alive=keep and pool==5 and i<4
                    assert bytes(u.mem_read(db+offset+stride*i,2))==bytes([int(alive),0x5a])
                assert struct.unpack('<H',u.mem_read(cb+e['cleanupCounts']+pool*2,2))[0]==count-(4 if keep and pool==5 else 0)
            assert bytes(u.mem_read(db+0xbcb9,2))==b'\x00\x00'
            assert all(x==1 for x in u.mem_read(db+0x4700,0x640))
            assert u.mem_read(cb+e['cleanupPending'],1)==b'\x00'
            for reg,value in regs.items():assert u.reg_read(reg)==value+(2 if reg==UC_X86_REG_SP else 0)
            for frame in range(3):
                u.mem_write(db+0x4700,bytes(0x640));u.reg_write(UC_X86_REG_SP,0xff00)
                u.mem_write(0x8ff00,struct.pack('<H',0xfff0));u.emu_start(cb+e['redrawFrame'],cb+0xfff0,count=10000)
                assert all(x==(1 if frame<2 else 0) for x in u.mem_read(db+0x4700,0x640))
            checked+=1
        # Execute the real power updater, not a JS mirror of its byte value.
        # A fake loaded character table lets us verify the selected function
        # and all thresholds without depending on DOS-loaded shot resources.
        levels=[6,12,16,24,32,48,72,96,128]
        for power in (1,5,6,12,16,31,32,48,64,72,96,127,128):
            for hook,port in [('frame',0x7fe1),('player',0x7fe2),('player',0x7fe3),('world',0x7fe4),('boss',0x7fe5)]:
                u,_=guest(spec);regs=init(u);seen=[];native=0x50
                for ip in (native,p['entries'][1],p['entries'][7]):u.mem_write(cb+ip,b'\xc3')
                for at in (0xf0a5,0xf07a):u.mem_write(BASE*16+at,b'\xcb')
                u.mem_write(db+0x428a,struct.pack('<H',native))
                u.mem_write(cb+0x60,b'\xcb');u.mem_write(db+0xbcd0,struct.pack('<HH',0x60,cs))
                u.mem_write(cb+e['targetPractice'],b'\x01')
                u.mem_write(db+0x449c,struct.pack('<H',0x1000))
                u.mem_write(db+0x1000,struct.pack('<10H',*[0x2000+2*i for i in range(10)]))
                def change(uc,current,size,value,data):
                    if current==port:
                        uc.mem_write(db+0x4664,bytes([power]))
                        uc.mem_write(cb+e['resourceDirty'],b'\x03')
                def draw(uc,at,size,data):seen.append(at-BASE*16)
                u.hook_add(UC_HOOK_INSN,change,None,1,0,UC_X86_INS_OUT)
                for at in (0xf0a5,0xf07a):u.hook_add(UC_HOOK_CODE,draw,begin=BASE*16+at,end=BASE*16+at)
                u.emu_start(cb+e[hook],cb+0xfff0,count=2000)
                level=sum(power>=n for n in levels)
                assert u.mem_read(db+0x4665,1)==bytes([level]),(lang,power,hook,port)
                assert struct.unpack('<H',u.mem_read(db+0x449a,2))[0]==0x2000+2*level
                assert seen==[0xf0a5,0xf07a],seen
                assert u.mem_read(cb+e['resourceDirty'],1)==b'\x00'
                for r,v in regs.items():assert u.reg_read(r)==v+(2 if r==UC_X86_REG_SP else 0),(hook,port,r)
                checked+=1
        for target in (0,1):
            for name,field,value,cont in [('livesZero',11,1,0xeeeb),('bombsZero',13,0,0xefa4)]:
                u,_=guest(spec);init(u);u.mem_write(db+0xba86,struct.pack('<HH',0,0x9000));u.mem_write(0x90000+field,b'\x07');u.mem_write(cb+e['targetPractice'],bytes([target]))
                u.emu_start(cb+e[name],BASE*16+cont,count=100)
                assert u.mem_read(0x90000+field,1)==bytes([value if target else 7]);assert u.reg_read(UC_X86_REG_SP)==0xfefe
                assert u.reg_read(UC_X86_REG_EFLAGS)==0x246;checked+=1
        image=gzip.decompress((ROOT/'web'/spec['url']).read_bytes())
        header=struct.unpack_from('<H',image,8)[0]*16
        marker=image[header:].find(b'P98AST01')
        assert marker>=0
        assist=BASE*16+marker+8+5
        u,_=guest(spec);init(u);u.reg_write(UC_X86_REG_BX,0)
        u.mem_write(assist,b'\x02');u.mem_write(0x9000b,b'\x00')
        u.emu_start(cb+e['respawnLives'],BASE*16+0x10a9a,count=100)
        u.mem_write(assist,b'\x04');u.mem_write(0x9000d,b'\x00')
        u.emu_start(cb+e['bombGate'],BASE*16+0xffc9,count=100)
        u.mem_write(0x9000d,b'\x02');u.mem_write(0x8ff00,struct.pack('<H',0xfff0))
        u.emu_start(cb+e['bombConsume'],cb+0xfff0,count=100)
        assert u.mem_read(0x9000d,1)==b'\x02';checked+=3
        u,_=guest(spec);init(u);u.reg_write(UC_X86_REG_BX,0);u.mem_write(0x9000b,b'\x01')
        u.mem_write(cb+e['targetPractice'],b'\x01');u.emu_start(cb+e['respawnLives'],BASE*16+0x10a9a,count=100)
        assert u.mem_read(cb+e['targetOutcome'],1)==b'\x02' and u.mem_read(cb+e['hold'],1)==b'\x01';checked+=1
        for lock in (0,1):
            for stock in (0,1,2):
                u,_=guest(spec);init(u);u.reg_write(UC_X86_REG_BX,0);u.mem_write(0x9000b,bytes([stock]));u.mem_write(cb+e['lockLives'],bytes([lock]))
                dest=0x10a9a if lock else 0x10ab7 if stock<=1 else 0x10a96
                u.emu_start(cb+e['respawnLives'],BASE*16+dest,count=100)
                assert u.reg_read(UC_X86_REG_IP)==dest-p['code_segment']*16;assert u.mem_read(0x9000b,1)==bytes([stock]);checked+=1
                u.mem_write(0x9000d,bytes([stock]));u.mem_write(cb+e['unlimitedBombs'],bytes([lock]));dest=0xffc9 if lock or stock else 0x10028
                u.emu_start(cb+e['bombGate'],BASE*16+dest,count=100);assert u.reg_read(UC_X86_REG_IP)==dest-p['code_segment']*16;checked+=1
                u.mem_write(0x8ff00,struct.pack('<H',0xfff0));u.emu_start(cb+e['bombConsume'],cb+0xfff0,count=100)
                assert u.mem_read(0x9000d,1)==bytes([stock if lock else (stock-1)&255]);checked+=1
            u,_=guest(spec);init(u);u.reg_write(UC_X86_REG_SP,0xfef0);u.reg_write(UC_X86_REG_BP,0xff00);u.reg_write(UC_X86_REG_DI,50)
            u.mem_write(cb+e['enemyLock'],bytes([lock]));u.mem_write(0x8fef0,struct.pack('<HH',0x1234,0x5678));u.mem_write(0x8ff00,struct.pack('<HHH',0x2345,0xfff0,cs))
            u.emu_start(cb+e['shotsReturn'],cb+0xfff0,count=100)
            assert u.reg_read(UC_X86_REG_AX)==(0 if lock else 50);assert u.reg_read(UC_X86_REG_DI)==0x1234;assert u.reg_read(UC_X86_REG_SI)==0x5678
            assert u.reg_read(UC_X86_REG_BP)==0x2345 and u.reg_read(UC_X86_REG_SP)==0xff06 and u.reg_read(UC_X86_REG_EFLAGS)==0x246;checked+=1
        # Native dialog operations still consume their own parameters. Dialogue
        # text/waits are skipped, resource and BGM opcodes are not reimplemented.
        u,_=guest(spec);init(u);u.mem_write(cb+e['targetPractice'],b'\x01')
        script=b'0\x81\x5c\x81\x5c\\=1\\$1\x81\x5c\\#\\c\\l,boss.bft\x00\\m,boss.m\x00\\#\\c\\l,next.bft\x00\\m,next.m\x00\\$\\c'
        u.mem_write(0x90000,script);u.mem_write(db+0x428c,struct.pack('<HH',0,0x9000));ops=[]
        audit=json.loads((ROOT/f'build/practice/{lang}.json').read_text());op=BASE*16+audit['dialogOp'];wipe=BASE*16+0x10d4b
        u.mem_write(op,b'\xc2\x02\x00');u.mem_write(wipe,b'\xc3')
        def operation(uc,at,size,data):
            sp=uc.reg_read(UC_X86_REG_SP);cmd=struct.unpack('<H',uc.mem_read(0x80000+sp+2,2))[0];ops.append(chr(cmd))
            cursor=struct.unpack('<H',uc.mem_read(db+0x428c,2))[0]
            if cmd in (ord('l'),ord('m')):
                while script[cursor]:cursor+=1
                cursor+=1
            elif cmd==ord('='):cursor+=1
            uc.mem_write(db+0x428c,struct.pack('<H',cursor))
        u.hook_add(UC_HOOK_CODE,operation,begin=op,end=op);u.emu_start(cb+e['dialogRun'],cb+0xfff0,count=2000)
        assert ops==['=','c','l','m'],ops;assert u.reg_read(UC_X86_REG_SP)==0xff02
        assert struct.unpack('<H',u.mem_read(db+0x428c,2))[0]==script.index(b'\\#\\c')+2+len(b'\\c\\l,boss.bft\x00\\m,boss.m\x00\\#');checked+=1
        # An outer STOP preserves the cursor for the next native dialog;
        # a text-box STOP only leaves that box. SJIS trail bytes are not ops.
        u.reg_write(UC_X86_REG_SP,0xff00);u.mem_write(0x8ff00,struct.pack('<H',0xfff0))
        u.emu_start(cb+e['dialogRun'],cb+0xfff0,count=2000)
        assert ops==['=','c','l','m','c','l','m'],ops;assert u.reg_read(UC_X86_REG_SP)==0xff02;checked+=1
        u,_=guest(spec);init(u);u.emu_start(cb+e['dialogRun'],BASE*16+audit['dialog']+4,count=100)
        assert u.reg_read(UC_X86_REG_SP)==0xfef8 and u.reg_read(UC_X86_REG_BP)==0xfefe;checked+=1
        u,rel=guest(spec)
        for row in audit['edits']:
            at=row['offset'];data=bytes(u.mem_read(BASE*16+at,len(bytes.fromhex(row['after']))))
            if data[0]==0xea:assert at+3 in rel and struct.unpack_from('<HH',data,1)==(e['shotsReturn'],cs)
    print(f'PASS {checked} shipped CN/JP native hook checks: markers, register/flag/stack preservation, native pause, stocks, F2/F3, damage and dialog')
if __name__=='__main__':main()
