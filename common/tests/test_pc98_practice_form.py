from pathlib import Path
import os,struct,subprocess,tempfile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_16,UC_HOOK_INTR,UC_HOOK_INSN
from unicorn.x86_const import *
root=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as tmp:
    source=Path(tmp)/'form.asm'; binary=Path(tmp)/'form.bin'
    source.write_text("bits 16\n%define FORM_STAGE_MAX 5\n%define FORM_CHARACTER_MAX 2\n%define FORM_PHASE_MAX 9\n%define FORM_STOCK_MAX 5\n%define FORM_POWER_MAX 80\n%define FORM_BRANCH_MAX 0\n%define FORM_RANK_DEFAULT 6\n%define FORM_RANK_MAX 22\n%define FORM_RANK_BASE -6\n%define FORM_HP_MAX 8500\n%define SOLO_MAILBOX 0\n%define PRACTICE_PAGE_BACK 0\n%macro PRACTICE_PAUSE_WAIT 0\n nop\n%endmacro\ndw practice_form_step,practice_form_draw,practice_form_choice,practice_form_values,practice_form_max,practice_form_graphics_save,practice_form_graphics_restore,practice_form_scroll,practice_form_options\n%include 'practice-form.inc'\n%include 'practice-text.inc'\n")
    subprocess.run([os.environ.get('NASM','nasm'),'-f','bin','-I',str(root/'pc98')+'/',str(source),'-o',str(binary)],check=True)
    image=binary.read_bytes();step,draw,choice,values,maxima,save_graphics,restore_graphics,scroll,options=struct.unpack_from('<9H',image)
    machine=Uc(UC_ARCH_X86,UC_MODE_16);machine.mem_map(0,0x100000);base=0x10000;machine.mem_write(base,image)
    def call(where,keys=0):
        for reg,val in ((UC_X86_REG_CS,0x1000),(UC_X86_REG_DS,0x1000),(UC_X86_REG_ES,0xa000),(UC_X86_REG_SS,0x8000),(UC_X86_REG_SP,0xff00),(UC_X86_REG_AX,keys)):
            machine.reg_write(reg,val)
        machine.mem_write(0x8ff00,struct.pack('<H',0xfff0));machine.emu_start(base+where,base+0xfff0,count=10000)
        assert machine.reg_read(UC_X86_REG_IP)==0xfff0
        return machine.reg_read(UC_X86_REG_AX)
    def key(k):
        result=call(step,k);call(step,0);return result
    assert key(16)==0, 'confirm on an editable field must not start practice'
    assert key(8)==1 and machine.mem_read(base+values,1)==b'\1'
    assert key(4)==1 and machine.mem_read(base+values,1)==b'\0'
    assert call(step,8)==1
    for _ in range(23): assert call(step,8)==0
    assert call(step,8)==0
    for _ in range(3): assert call(step,8)==0
    assert call(step,8)==1, 'held direction repeats without new key edges'
    assert machine.mem_read(base+values,1)==b'\2'
    call(step,0);machine.mem_write(base+values,b'\0')
    assert key(1)==1 and machine.mem_read(base+choice,1)==b'\x0c'
    assert key(16)==1 and machine.mem_read(base+choice,1)==b'\0'
    assert key(8)==1
    assert key(1)==1 and machine.mem_read(base+choice,1)==b'\x0a'
    assert key(1)==1 and machine.mem_read(base+choice,1)==b'\x09'
    assert key(8)==1 and machine.mem_read(base+options+10,1)==b'\x01'
    assert key(32)==1 and machine.mem_read(base+choice,1)==b'\x0c'
    assert key(2)==1 and machine.mem_read(base+choice,1)==b'\0'
    # Difficulty and stage changes must discard phase/HP and non-Extra branch.
    machine.mem_write(base+values,bytes([6,0,4,2,8,3,2,80,1,6]))
    machine.mem_write(base+choice,b'\x02')
    assert key(4)==1
    assert machine.mem_read(base+values,5)==bytes([6,0,3,2,0])
    machine.mem_write(base+choice,b'\0')
    assert key(4)==1
    assert machine.mem_read(base+values+8,1)==b'\0'
    machine.mem_write(base+values+7,b'\x80');call(draw)
    at=0xa0000+5*160+20*2
    assert bytes(machine.mem_read(at,10))[::2]==b'STAGE'
    at+=7*160+22*2
    assert bytes(machine.mem_read(at,6))[::2]==b'128'
    assert key(32)==3
    # Simulate two PC-98 VRAM pages and DOS allocation to verify exact restore.
    planes=(0xa8000,0xb0000,0xb8000,0xe0000)
    pages=[[bytes(((i*17+plane*19+page*41)&255) for i in range(32768)) for plane in range(4)] for page in range(2)]
    current=[0];allocated=[]
    for address,data in zip(planes,pages[0]):machine.mem_write(address,data)
    def out_hook(uc,port,size,value,user):
        if port!=0xa6:return
        pages[current[0]]=[bytes(uc.mem_read(a,32768)) for a in planes]
        current[0]=value&1
        for a,data in zip(planes,pages[current[0]]):uc.mem_write(a,data)
    def interrupt(uc,number,user):
        assert number==0x21
        ah=uc.reg_read(UC_X86_REG_AH)
        if ah==0x48:
            assert uc.reg_read(UC_X86_REG_BX)==2600
            uc.reg_write(UC_X86_REG_AX,0x2000);uc.reg_write(UC_X86_REG_EFLAGS,uc.reg_read(UC_X86_REG_EFLAGS)&~1);allocated.append(True)
        elif ah==0x49:allocated.append(False)
        else:raise AssertionError(ah)
    machine.hook_add(UC_HOOK_INSN,out_hook,None,1,0,UC_X86_INS_OUT)
    machine.hook_add(UC_HOOK_INTR,interrupt)
    original=[[bytes(x) for x in page] for page in pages]
    # Graphics routines copy 41,600 bytes, requiring a larger instruction budget.
    def graphics(where):
        for reg,val in ((UC_X86_REG_CS,0x1000),(UC_X86_REG_DS,0x1000),(UC_X86_REG_SS,0x8000),(UC_X86_REG_SP,0xff00)):
            machine.reg_write(reg,val)
        machine.mem_write(0x8ff00,struct.pack('<H',0xfff0));machine.emu_start(base+where,base+0xfff0,count=500000)
        assert machine.reg_read(UC_X86_REG_IP)==0xfff0
    for offset in (0, 137, 399):
        machine.mem_write(base+scroll,struct.pack('<H',offset))
        graphics(save_graphics)
        pages[current[0]]=[bytes(machine.mem_read(a,32768)) for a in planes]
        affected={((80+offset+y)%400)*80+x for y in range(208) for x in range(20,45)}
        for page in range(2):
            for plane in range(4):
                assert all(pages[page][plane][i]==(0 if i in affected else original[page][plane][i]) for i in range(32000))
        graphics(restore_graphics)
        pages[current[0]]=[bytes(machine.mem_read(a,32768)) for a in planes]
        assert pages==original
    assert allocated==[True,False]*3
print('PASS native practice form navigation, editing and text-plane rendering')
