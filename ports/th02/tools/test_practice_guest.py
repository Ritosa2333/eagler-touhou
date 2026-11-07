"""Execute the real CN/JP 16-bit bridge with DOS MZ relocations in Unicorn."""
from pathlib import Path
import gzip,json,struct
from unicorn import Uc,UC_ARCH_X86,UC_MODE_16,UC_HOOK_INSN
from unicorn.x86_const import *
ROOT=Path(__file__).resolve().parents[1]
manifest=json.loads((ROOT/'web/practice-assets.json').read_text());checks=[]
for lang,item in manifest['languages'].items():
    p=item['patch'];exe=gzip.decompress((ROOT/'web'/item['url']).read_bytes());hs=struct.unpack_from('<H',exe,8)[0]*16
    u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000);load=0x1000;u.mem_write(load*16,exe[hs:])
    def w(at,x):u.mem_write(at,struct.pack('<H',x&65535))
    def byte(at,x):u.mem_write(at,bytes([x]))
    def word(at):return struct.unpack('<H',u.mem_read(at,2))[0]
    for i in range(struct.unpack_from('<H',exe,6)[0]):
        off,seg=struct.unpack_from('<HH',exe,struct.unpack_from('<H',exe,24)[0]+i*4);at=load*16+seg*16+off;w(at,word(at)+load)
    cs=load+p['codeSegment'];ds=load+p['dataSegment'];ss=0x8000;cb=cs*16;db=ds*16;e=p['practice']['entries']
    # The result loop must call the same relocated far frame-delay routine as
    # the game. A zero segment reaches DOS startup code and never returns.
    wait=bytes(u.mem_read(cb+e['waitHold'],45)); call=wait.index(bytes.fromhex('6a019ac801'))
    delay_segment=struct.unpack_from('<H',wait,call+5)[0]
    original_segment=word(load*16+0xbca9)
    assert delay_segment==original_segment and bytes(u.mem_read(delay_segment*16+0x1c8,3))==bytes.fromhex('558bec')
    checks.append(lang+': result loop calls relocated native frame delay')
    ports=[];consume_hit=False
    def marker(uc,port,size,value,user):
        ports.append((port,size,value))
        if consume_hit and port==0x7fd2:byte(db+0x2b94,0)
    u.hook_add(UC_HOOK_INSN,marker,None,1,0,UC_X86_INS_OUT)
    def run(name,end=0xfff0,sp=0xf800):
        for reg,val in [(UC_X86_REG_CS,cs),(UC_X86_REG_DS,ds),(UC_X86_REG_ES,0x6000),(UC_X86_REG_SS,ss),(UC_X86_REG_SP,sp),(UC_X86_REG_EFLAGS,0x202)]:u.reg_write(reg,val)
        w(ss*16+sp,0xfff0);u.emu_start(cb+e[name],cb+end,count=40000)
        assert u.reg_read(UC_X86_REG_IP)==end,(lang,name,hex(u.reg_read(UC_X86_REG_IP)))
        assert u.reg_read(UC_X86_REG_DS)==ds
    original=lambda at:at-0xb160
    # The real guest resource routine owns preparation stock, power and Rank.
    power_update=cb+original(0xdead);saved_power_update=bytes(u.mem_read(power_update,1));u.mem_write(power_update,b'\xc3')
    byte(cb+e['initialLives'],2);byte(cb+e['initialBombs'],3);byte(cb+e['initialPower'],64);w(cb+e['initialRank'],-4)
    for preparing in (0,1):
        byte(cb+e['targetPreparing'],preparing);byte(db+0xb2c,4);byte(db+0xb2d,5);byte(db+0xaa6,1);w(db+0x2802,8)
        run('targetResources')
        assert u.mem_read(db+0xb2c,1)[0]==(2 if preparing else 4)
        assert u.mem_read(db+0xb2d,1)[0]==(3 if preparing else 5)
        assert u.mem_read(db+0xaa6,1)[0]==(64 if preparing else 1)
        assert word(db+0x2802)==(0xfffc if preparing else 8)
    byte(cb+e['targetPreparing'],0);u.mem_write(power_update,saved_power_update)
    checks.append(lang+': target resource initialization executes in guest')
    byte(db+0x19f6,1);w(db+0x1a0c,0x1234);w(db+0x1a0e,0x5678);w(db+0x1a10,0x1234);w(db+0x1a12,0x5678)
    for stage,phase,requested,damage,expected,clock in ((0,1,2,0,700,0),(0,1,3,0,1400,0),(1,1,2,0,701,0),(4,1,3,0,1600,0),(4,2,3,0,0,100),(5,0,3,0,0,50)):
        byte(db+0x8f5,stage);byte(db+0x7886,(phase-1)&255);w(db+0x91f8,phase);byte(db+0x7b42,phase);w(db+0x2bea,damage);w(db+0x2be0,0)
        byte(cb+e['targetPreparing'],1);byte(cb+e['initialPhase'],requested);run('targetAdvance')
        assert word(db+0x2bea)==expected and word(db+0x2be0)==clock,(lang,stage,phase)
    byte(cb+e['targetPreparing'],0);checks.append(lang+': guest advances earlier native Boss attacks')
    for entry in (0,1,2):
        byte(cb+e['targetPreparing'],1);byte(cb+e['initialEntry'],entry);byte(cb+e['warpState'],0)
        w(db+0x1a0c,0x1234);w(db+0x1a0e,0x5678);w(db+0x91d0,0);w(db+0x91d2,0)
        byte(db+0x2baa,0);byte(db+0xa91,0);byte(db+0xa93,1);w(db+0x2ba6,93);w(db+0x28da,0)
        run('targetWarp');assert word(db+0x91d0)==0
        run('targetWarp')
        assert word(db+0x91d0)==(0x1234 if entry else 0)
        assert u.mem_read(db+0x2baa,1)[0]==(1 if entry==1 else 0)
        assert u.mem_read(db+0xa91,1)[0]==(1 if entry==2 else 0)
        assert word(db+0x28da)==(93 if entry==1 else 0)
    byte(cb+e['targetPreparing'],0);checks.append(lang+': guest initializes native target spawners')
    for entry,stage,phase,requested,clock,boss,expected in ((0,0,0,0,0,False,1),(1,0,0,0,17,False,0),(1,0,0,0,18,False,1),(1,1,0,0,320,False,1),(2,4,2,0,0,True,0),(2,4,3,0,0,True,1),(2,4,3,5,0,True,0),(2,4,3,3,0,True,1)):
        byte(cb+e['targetPreparing'],1);byte(cb+e['initialEntry'],entry);byte(cb+e['initialPhase'],requested)
        byte(db+0x8f5,stage);byte(db+0x19f6,int(boss));byte(db+0x2baa,1);byte(db+0x2bfa,0)
        w(db+0x1a0c,0x1234);w(db+0x1a0e,0x5678);w(db+0x1a10,0x1234);w(db+0x1a12,0x5678)
        w(db+0x91f8,phase);w(db+0x2be0,clock);run('targetCheck')
        assert u.mem_read(cb+e['targetReached'],1)[0]==expected,(lang,entry,stage,phase,clock)
    byte(cb+e['targetPreparing'],0);checks.append(lang+': guest detects native target arrival')
    arrival_bytes=(0x2b94,0x2b95,0x2b96,0x2b99,0xaa7,0x2b97)
    for at in arrival_bytes:byte(db+at,7)
    for at in (0xb28,0xb2a,0x3e3c,0x3e3e,0x3e40,0xb46,0x2538):w(db+at,123)
    byte(db+0x109d,0);byte(db+0x109e,0);byte(cb+e['arrivalPending'],1);run('targetArrival')
    assert all(u.mem_read(db+at,1)==b'\x00' for at in arrival_bytes)
    assert all(word(db+at)==0 for at in (0xb28,0xb2a,0x3e3c,0x3e3e,0x3e40,0xb46,0x2538))
    assert bytes(u.mem_read(db+0x109d,2))==b'\xff\xff' and u.mem_read(cb+e['arrivalPending'],1)==b'\x00'
    checks.append(lang+': guest clears target arrival state')
    for entry,stage,phase,hp,damage in ((1,0,0,50,251),(1,1,0,81,300),(2,0,0,261,2000),(2,0,2,100,1300),(2,1,3,200,2200),(2,3,0,500,400),(2,4,5,300,500),(2,5,9,300,1000)):
        byte(cb+e['initialEntry'],entry);byte(cb+e['initialPhase'],phase);w(cb+e['initialHp'],hp);byte(db+0x8f5,stage);w(db+0x2bea,0)
        run('targetHp');assert word(db+0x2bea)==damage,(lang,entry,stage,phase,hp)
    w(cb+e['initialHp'],0);checks.append(lang+': guest initializes native target HP thresholds')
    byte(cb+e['targetPractice'],1);byte(cb+e['targetPreparing'],0);byte(cb+e['initialStage'],4)
    byte(cb+e['initialEntry'],2);byte(cb+e['initialPhase'],3);byte(db+0x8f5,4);w(db+0x91f8,3)
    byte(db+0x19f6,1);w(db+0x1a0c,0x1234);w(db+0x1a0e,0x5678);w(db+0x1a10,0x1234);w(db+0x1a12,0x5678)
    byte(cb+e['arrivalPending'],1);run('targetArrival');byte(cb+e['hold'],0);run('targetOutcomeStep')
    assert u.mem_read(cb+e['targetOutcome'],1)==b'\x00'
    w(db+0x91f8,5);byte(db+0x2b94,1);run('targetOutcomeStep');assert u.mem_read(cb+e['targetOutcome'],1)==b'\x00'
    byte(db+0x2b94,0);run('targetOutcomeStep');assert u.mem_read(cb+e['targetOutcome'],1)==b'\x01' and u.mem_read(cb+e['hold'],1)==b'\x01'
    w(db+0x91f8,3);byte(cb+e['arrivalPending'],1);run('targetArrival');byte(cb+e['hold'],0)
    byte(db+0xaa7,1);byte(db+0xb2c,0);byte(db+0x2b99,5);run('targetOutcomeStep')
    assert u.mem_read(cb+e['targetOutcome'],1)==b'\x00'
    byte(db+0x2b99,0);run('targetOutcomeStep');assert u.mem_read(cb+e['targetOutcome'],1)==b'\x02' and u.mem_read(cb+e['hold'],1)==b'\x01'
    byte(cb+e['targetPractice'],0);byte(cb+e['hold'],0);checks.append(lang+': guest resolves target success and fatal death')
    assist_offset=exe[hs:].find(b'P98AST01')
    assert assist_offset>=0
    assist=load*16+assist_offset+8+5
    # The guest bitmask owns these branches even when the legacy host flags
    # are all clear. This executes the actual patched executable.
    byte(cb+e['lockLives'],0);byte(cb+e['unlimitedBombs'],0);byte(cb+e['enemyLock'],0)
    byte(assist,2);byte(db+0xb2c,0);run('deathGate',original(0xf107))
    byte(db+0xb2c,2);run('lifeConsume');assert u.mem_read(db+0xb2c,1)==b'\x02'
    byte(assist,4);byte(db+0xb2d,0);run('bombGate',original(0xe291))
    byte(db+0xb2d,2);run('bombConsume');assert u.mem_read(db+0xb2d,1)==b'\x02'
    byte(assist,1);byte(db+0x2b94,1);run('miss');assert u.mem_read(db+0x2b94,1)==b'\0'
    byte(assist,0)
    checks.append(lang+': guest assist flags control real F1/F2/F3 branches')
    for locked in [0,1]:
        byte(cb+e['lockLives'],locked)
        for stock in [0,1,4]:
            byte(db+0xb2c,stock);run('deathGate',original(0xf107 if locked or stock else 0xf0e1));assert u.reg_read(UC_X86_REG_SP)==0xf800
            byte(db+0xb2c,stock);run('lifeConsume');assert u.mem_read(db+0xb2c,1)[0]==(stock if locked else(stock-1)&255)
    checks.append(lang+': F2 native Game Over branch / decrement')
    for infinite in [0,1]:
        byte(cb+e['unlimitedBombs'],infinite)
        for stock in [0,1,3]:
            byte(db+0xb2d,stock);run('bombGate',original(0xe291 if infinite or stock else 0xe2d7));assert u.reg_read(UC_X86_REG_SP)==0xf800
            run('bombConsume');assert u.mem_read(db+0xb2d,1)[0]==(stock if infinite else(stock-1)&255)
    checks.append(lang+': F3 native admission / consume, including zero stock')
    for targeted in [0,1]:
        byte(cb+e['targetPractice'],targeted)
        for name,stock,body in [('livesZero',0xb2c,0xdf79),('bombsZero',0xb2d,0xdfc7)]:
            byte(db+stock,3);run(name,original(body));assert u.mem_read(db+stock,1)[0]==(0 if targeted else 3)
    checks.append(lang+': inventory clamped at native redraw, full run untouched')
    for name,body in [('dialogPre',0x12de0),('dialogPost',0x12e95),('dialogBox',0x13055)]:
        # Wrapper returns to the relocated original segment with the displaced
        # ENTER/PUSH SI and original near return address still on the stack.
        byte(cb+e['targetPractice'],0)
        dest_seg=ds-(p['dataSegment']-(0xfac0+p['gap'])//16);destination=dest_seg*16+(body-0xfac0+5)
        for reg,val in [(UC_X86_REG_CS,cs),(UC_X86_REG_DS,ds),(UC_X86_REG_ES,0x6000),(UC_X86_REG_SS,ss),(UC_X86_REG_SP,0xf800)]:u.reg_write(reg,val)
        u.emu_start(cb+e[name],destination,count=20000);assert u.reg_read(UC_X86_REG_SP)==0xf7f8
        assert u.reg_read(UC_X86_REG_CS)==dest_seg
    checks.append(lang+': full-run dialog segment/prologue/stack')
    def run_far(name):
        for reg,val in [(UC_X86_REG_CS,cs),(UC_X86_REG_DS,ds),(UC_X86_REG_SS,ss),(UC_X86_REG_SP,0xf800),(UC_X86_REG_EFLAGS,0x202)]:u.reg_write(reg,val)
        w(ss*16+0xf800,0xfff0);w(ss*16+0xf802,cs)
        u.emu_start(cb+e[name],cb+0xfff0,count=20000)
        assert u.reg_read(UC_X86_REG_CS)==cs and u.reg_read(UC_X86_REG_SP)==0xf804
    for stage,phase,requested,preparing,expected in ((4,1,3,1,0),(4,2,3,1,55),(4,3,3,1,55),(5,3,5,1,0),(5,3,5,0,55)):
        byte(db+0x8f5,stage);byte(db+0x19f6,1);w(db+0x1a0c,0x1234);w(db+0x1a0e,0x5678);w(db+0x1a10,0x1234);w(db+0x1a12,0x5678)
        w(db+0x91f8,phase);byte(db+0x7b42,phase);w(db+0x2be0,55)
        byte(cb+e['initialPhase'],requested);byte(cb+e['targetPreparing'],preparing)
        run_far('transition');assert word(db+0x2be0)==expected,(lang,stage,phase,requested,preparing)
    byte(cb+e['targetPreparing'],0);checks.append(lang+': guest owns target phase transition comparisons')
    byte(assist,32);byte(cb+e['enemyLock'],0);u.reg_write(UC_X86_REG_BP,0xf900)
    w(ss*16+0xf8f8,17);w(db+0x3e3c,100);u.reg_write(UC_X86_REG_AX,17)
    run_far('shotsReturn');assert u.reg_read(UC_X86_REG_AX)==0 and word(db+0x3e3c)==117
    byte(assist,16);byte(cb+e['lockTime'],0);w(db+0x28da,200)
    run_far('timeMid1');flags=u.reg_read(UC_X86_REG_EFLAGS)
    assert bool(flags&0x80)!=bool(flags&0x800)
    byte(assist,0);checks.append(lang+': guest U and F5 flags control damage and timeout')
    for locked in [0,1]:
        byte(cb+e['enemyLock'],locked);u.reg_write(UC_X86_REG_BP,0xf900);w(ss*16+0xf8f8,17);w(db+0x3e3c,100);u.reg_write(UC_X86_REG_AX,17)
        run_far('shotsReturn');assert u.reg_read(UC_X86_REG_AX)==(0 if locked else 17);assert word(db+0x3e3c)==117
    checks.append(lang+': shared shot/Bomb damage return keeps native CS and near epilogue')
    for name,offset,limit,byte_counter in [('timeMid1',0x28da,184,False),('timeMid2',0x2be0,1600,False),('timeMid3',0x5538,4,True),('timeMid4',0x930d,12,True),('timeMima',0x924e,12,False)]:
        for locked in [0,1]:
            byte(cb+e['lockTime'],locked)
            for count in [limit-1,limit,limit+1]:
                (byte if byte_counter else w)(db+offset,count);w(db+0x9282,limit);run_far(name);flags=u.reg_read(UC_X86_REG_EFLAGS)
                condition=(bool(flags&0x80)!=bool(flags&0x800)) if name=='timeMid1' else bool(flags&1) if name in ['timeMid3','timeMid4'] else bool(flags&0x40) or(bool(flags&0x80)!=bool(flags&0x800))
                assert condition==(bool(locked) or (count<limit if name in ['timeMid1','timeMid3','timeMid4'] else count<=limit)),(lang,name,count,locked,hex(flags))
                assert (u.mem_read(db+offset,1)[0] if byte_counter else word(db+offset))==count,'F5 must not freeze attack clocks/counters'
        checks.append(lang+': '+name+' blocks only native timeout comparison')
    consume_hit=True;byte(db+0x2b94,1);byte(cb+e['hold'],0)
    registers=[UC_X86_REG_EAX,UC_X86_REG_EBX,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_EBP]
    values=[0x12345678+i for i in range(len(registers))]
    for reg,value in zip(registers,values):u.reg_write(reg,value)
    run('miss');assert u.mem_read(db+0x2b94,1)==b'\0';assert ports[-1]==(0x7fd2,4,ds<<16|cs)
    assert [u.reg_read(reg)for reg in registers]==values;consume_hit=False
    checks.append(lang+': F1 marker consumes real hit before death, preserving registers')
    # Isolate the bridge from the already separately tested native item body.
    # Check the new near tail-call, OUT32 ABI and all preserved CPU state.
    item_at=cb+original(0xdaf0);item_head=bytes(u.mem_read(item_at,1));u.mem_write(item_at,b'\xc3')
    for reg,value in zip(registers,values):u.reg_write(reg,value)
    run('entities');assert ports[-1]==(0x7fd5,4,ds<<16|cs)
    assert [u.reg_read(reg)for reg in registers]==values
    assert u.reg_read(UC_X86_REG_SP)==0xf802 and u.reg_read(UC_X86_REG_ES)==0x6000
    for preparing in (0,1):
        byte(cb+e['targetPreparing'],preparing)
        for start,stride,count in ((0x2c18,20,150),(0x3ce6,16,20),(0x5f6c,12,12)):
            for i in range(count):byte(db+start+stride*i,1 if i%3==0 else 3)
        run('entities')
        for start,stride,count in ((0x2c18,20,150),(0x3ce6,16,20),(0x5f6c,12,12)):
            for i in range(count):
                expected=(2 if preparing else 1) if i%3==0 else 3
                assert u.mem_read(db+start+stride*i,1)[0]==expected
    byte(cb+e['targetPreparing'],0)
    checks.append(lang+': guest preparation clears only live bullet, item and laser slots')
    assert u.reg_read(UC_X86_REG_EFLAGS)==0x202
    u.mem_write(item_at,item_head)
    checks.append(lang+': pre-entity bridge preserves near return, registers, flags and segments')
    for count in [0,1,600]:
        w(db+0x2be0,count)
        for reg,value in zip(registers,values):u.reg_write(reg,value)
        run_far('transition')
        assert ports[-1]==(0x7fd3,4,ds<<16|cs)
        assert [u.reg_read(reg)for reg in registers]==values
        assert bool(u.reg_read(UC_X86_REG_EFLAGS)&0x40)==(count==0)
    checks.append(lang+': attack-transition OUT32 preserves registers and native comparison')
    for name,arg_bytes in [('dialogPre',0),('dialogPost',0),('dialogBox',2)]:
        byte(cb+e['targetPractice'],1);byte(db+0x740a,7)
        for reg,value in zip(registers,values):u.reg_write(reg,value)
        for reg,val in [(UC_X86_REG_CS,cs),(UC_X86_REG_DS,ds),(UC_X86_REG_SS,ss),(UC_X86_REG_SP,0xf800),(UC_X86_REG_EFLAGS,0x202)]:u.reg_write(reg,val)
        w(ss*16+0xf800,0xff00);w(ss*16+0xf802,0x1234)
        u.emu_start(cb+e[name],dest_seg*16+0xff00,count=20000)
        assert u.reg_read(UC_X86_REG_CS)==dest_seg and u.reg_read(UC_X86_REG_SP)==0xf802+arg_bytes
        assert [u.reg_read(reg)for reg in registers]==values
        assert u.mem_read(db+0x740a,1)[0]==(8 if name=='dialogBox' else 7)
    checks.append(lang+': target dialog skip retains native near/Pascal return and stack')
print('\n'.join('PASS '+s for s in checks));print(len(checks),'native groups passed')
import hashlib
(ROOT/'reports').mkdir(exist_ok=True)
(ROOT/'reports/practice-native.json').write_text(json.dumps({'passed':True,'groups':checks,'assetsSha256':hashlib.sha256((ROOT/'web/practice-assets.json').read_bytes()).hexdigest()},indent=2)+'\n')
