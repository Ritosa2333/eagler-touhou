"""Read-only TH12 1.00b fragment/idle-summon research, not a gameplay golden.

Runs original decision paths in isolated memory with explicit HUD/spawn-decision
fixtures. No retail file or browser runtime is edited. Output describes the exact
local paths exercised; it is not a complete original game capture.
"""
import hashlib,itertools,json,pathlib,struct,sys
TITLE=pathlib.Path(__file__).resolve().parents[2];WORKSPACE=TITLE.parent
sys.path.insert(0,str(WORKSPACE/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_ECX
EXE=WORKSPACE/'th12/th12.exe';pe=pefile.PE(str(EXE));base=pe.OPTIONAL_HEADER.ImageBase
mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(base,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(base,pe.get_memory_mapped_image())
STACK=0x1000000;STOP=0x2000000;MANAGER=0x3000000;ITEM=0x3100000;STATE=0x4b0c4c
mu.mem_map(STACK,0x10000);mu.mem_map(STOP,4096);mu.mem_map(MANAGER,4096);mu.mem_map(ITEM,4096)
hud_events=[];spawn_calls=[];point_displays=[];intercept_spawn=False
def return_from_fixture(cleanup=0):
    sp=mu.reg_read(UC_X86_REG_ESP);target=struct.unpack('<I',mu.mem_read(sp,4))[0]
    mu.reg_write(UC_X86_REG_EIP,target);mu.reg_write(UC_X86_REG_ESP,sp+4+cleanup)
def hook(uc,address,size,data):
    if address==0x421a60:
        hud_events.append('discard-oldest-and-rotate-HUD')
        return_from_fixture()
    elif address==0x44a8a0 and intercept_spawn:
        spawn_calls.append(mu.reg_read(UC_X86_REG_EAX));return_from_fixture()
    elif address==0x43e250:
        point_displays.append(mu.reg_read(UC_X86_REG_EAX));return_from_fixture(8)
mu.hook_add(UC_HOOK_CODE,hook)
def append(color):
    sp=STACK+0xf000;mu.mem_write(sp,struct.pack('<I',STOP));mu.reg_write(UC_X86_REG_ESP,sp);mu.reg_write(UC_X86_REG_ESI,STATE);mu.reg_write(UC_X86_REG_EDI,color)
    mu.emu_start(0x422e80,STOP,count=10000)
    if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('instruction budget exceeded')
    fields=struct.unpack('<7i',mu.mem_read(STATE,28))
    return {'slots':fields[:3],'count':fields[3],'displayState':fields[4],'gateTimer':fields[5],'combination':fields[6],'returnEax':mu.reg_read(UC_X86_REG_EAX)}
rows=[]
for colors in itertools.product(range(1,4),repeat=3):
    mu.mem_write(STATE,bytes(28));hud_events.clear();snapshots=[append(c) for c in colors]
    rows.append({'input':colors,'snapshots':snapshots,'externalCalls':hud_events.copy()})
mu.mem_write(STATE,bytes(28));full=[append(c) for c in [1,1,1,2]]
gate_rows=[];intercept_spawn=True
for display_state,gate_timer in itertools.product(range(5),[-1,0,1]):
    spawn_calls.clear();mu.mem_write(MANAGER,bytes(0x6c));mu.mem_write(0x4b0c5c,struct.pack('<3i',display_state,gate_timer,2));mu.mem_write(0x4b2ed0,struct.pack('<f',1));mu.mem_write(MANAGER+0x1c,struct.pack('<I',0x4b2ed0));mu.mem_write(MANAGER+0x20,struct.pack('<I',1))
    sp=STACK+0xf000;mu.mem_write(sp,struct.pack('<I',STOP));mu.reg_write(UC_X86_REG_ESP,sp);mu.reg_write(UC_X86_REG_EAX,MANAGER);mu.emu_start(0x44a2d0,STOP,count=10000)
    expected=[2] if display_state==3 and gate_timer>=0 else []
    if spawn_calls!=expected:raise RuntimeError('original summon gate mismatch')
    gate_rows.append({'displayState':display_state,'gateTimer':gate_timer,'spawnCallCombinations':spawn_calls.copy()})
intercept_spawn=False
mu.mem_write(MANAGER,bytes(0x6c));mu.mem_write(MANAGER+0x2c,struct.pack('<I',1));sp=STACK+0xf000;mu.mem_write(sp,struct.pack('<I',STOP));mu.reg_write(UC_X86_REG_ESP,sp);mu.reg_write(UC_X86_REG_ESI,MANAGER);mu.reg_write(UC_X86_REG_EAX,2);mu.emu_start(0x44a8a0,STOP,count=10000)
if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('original active-UFO guard did not return')
point_rows=[]
for color,current,maximum in itertools.product(range(3),[1000000,1950000],[2000000]):
    mu.mem_write(STATE,struct.pack('<7i',1,1,1,3,3,0,1));mu.mem_write(0x4b0c78,struct.pack('<i',current));mu.mem_write(0x4b0cf4,struct.pack('<i',maximum));point_displays.clear()
    sp=STACK+0xf000;mu.mem_write(sp,struct.pack('<I',STOP));mu.reg_write(UC_X86_REG_ESP,sp);mu.reg_write(UC_X86_REG_EAX,color);mu.reg_write(UC_X86_REG_ECX,ITEM);mu.emu_start(0x4270b0,STOP,count=10000)
    result=struct.unpack('<i',mu.mem_read(0x4b0c78,4))[0]
    if result!=min(current+100000,maximum) or point_displays!=[1000]:raise RuntimeError('original full-slot point value behavior mismatch')
    point_rows.append({'colorInput':color,'before':current,'cap':maximum,'after':result,'display':point_displays.copy(),'slots':struct.unpack('<3i',mu.mem_read(STATE,12))})
report={'schema':'th12-research-ufo-combinations/1','scope':'original fragment helper, idle summon gate, full-slot collection branch only','wholeGameDeterminism':False,'provider':{'functions':['0x422e80','0x44a2d0 idle branch','0x44a8a0 active guard','0x4270b0 full-slot branch'],'inputRegisters':'ESI=fragment state/manager; EDI=stored color; EAX=manager or picked color; ECX=picked item','unicorn':'2.1.4','externalFixtures':['0x421a60 HUD animation call recorded without execution','0x44a8a0 records summon decisions in idle-gate tests','0x43e250 records point-value display calls']},'exeSha256':hashlib.sha256(EXE.read_bytes()).hexdigest(),'triples':rows,'extraWhileFull':full,'idleSummonGateCases':gate_rows,'activeSpawnGuard':'returned without spawning','fullSlotCollectionCases':point_rows}
out=TITLE/'local/reverse/ufo-collection-probe.json';out.parent.mkdir(parents=True,exist_ok=True);out.write_text(json.dumps(report,indent=2)+'\n')
classes={}
for row in rows:
    final=row['snapshots'][-1];k='ready-rainbow' if final['combination']==-1 else 'ready-single-color' if final['count']==3 else 'shift-retain-last-two'
    classes[k]=classes.get(k,0)+1
print(json.dumps({'output':str(out),'triples':len(rows),'classes':classes,'extraWhileFull':full[-1]},indent=2))
