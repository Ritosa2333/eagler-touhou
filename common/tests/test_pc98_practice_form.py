from pathlib import Path
import os,struct,subprocess,tempfile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_16
from unicorn.x86_const import *
root=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as tmp:
    source=Path(tmp)/'form.asm'; binary=Path(tmp)/'form.bin'
    source.write_text("bits 16\ndw practice_form_step,practice_form_draw,practice_form_choice,practice_form_values,practice_form_max\n%include 'practice-form.inc'\n%include 'practice-text.inc'\n")
    subprocess.run([os.environ.get('NASM','nasm'),'-f','bin','-I',str(root/'pc98')+'/',str(source),'-o',str(binary)],check=True)
    image=binary.read_bytes();step,draw,choice,values,maxima=struct.unpack_from('<5H',image)
    machine=Uc(UC_ARCH_X86,UC_MODE_16);machine.mem_map(0,0x100000);base=0x10000;machine.mem_write(base,image)
    def call(where,keys=0):
        for reg,val in ((UC_X86_REG_CS,0x1000),(UC_X86_REG_DS,0x1000),(UC_X86_REG_ES,0xa000),(UC_X86_REG_SS,0x8000),(UC_X86_REG_SP,0xff00),(UC_X86_REG_AX,keys)):
            machine.reg_write(reg,val)
        machine.mem_write(0x8ff00,struct.pack('<H',0xfff0));machine.emu_start(base+where,base+0xfff0,count=10000)
        assert machine.reg_read(UC_X86_REG_IP)==0xfff0
        return machine.reg_read(UC_X86_REG_AX)
    def key(k):
        result=call(step,k);call(step,0);return result
    assert key(8)==1 and machine.mem_read(base+values,1)==b'\1'
    assert key(4)==1 and machine.mem_read(base+values,1)==b'\0'
    assert key(1)==1 and machine.mem_read(base+choice,1)==b'\x09'
    assert key(16)==3
    assert key(2)==1 and machine.mem_read(base+choice,1)==b'\0'
    machine.mem_write(base+values+7,b'\x80');call(draw)
    at=0xa0000+5*160+20*2
    assert bytes(machine.mem_read(at,10))[::2]==b'STAGE'
    at+=7*160+22*2
    assert bytes(machine.mem_read(at,6))[::2]==b'128'
    assert key(32)==3
print('PASS native practice form navigation, editing and text-plane rendering')
