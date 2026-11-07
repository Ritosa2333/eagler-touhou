"""Execute the result menu inside a 16-bit PC-98 guest fixture."""
from pathlib import Path
import os
import struct
import subprocess
import tempfile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_16
from unicorn.x86_const import UC_X86_REG_CS, UC_X86_REG_DS, UC_X86_REG_SP, UC_X86_REG_SS

HERE=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory() as tmp:
    src,out=Path(tmp)/'result.asm',Path(tmp)/'result.bin'
    src.write_text('bits 16\ndw practice_result_tick,practice_outcome,hold,practice_result_visible,'
                   'practice_result_choice,practice_result_request,practice_result_ack,practice_result_input,practice_result_selecting,test_form_result\n'
                   'practice_outcome: db 1\nhold: db 1\npractice_assists_drawn: db 0\n'
                   'practice_assists_open: db 0\npractice_assists_draw: ret\n'
                   'practice_form_input: dw 0\npractice_form_request: db 0\ntest_form_result: db 2\npractice_form: mov al,[cs:test_form_result]\nmov [cs:practice_form_request],al\nret\n'
                   f'%include "{(HERE/"practice-result.inc").as_posix()}"\n'
                   f'%include "{(HERE/"practice-text.inc").as_posix()}"\n',encoding='ascii')
    subprocess.run([os.environ.get('NASM','nasm'),'-f','bin',str(src),'-o',str(out)],check=True)
    code=out.read_bytes()
entry,outcome,hold,visible,choice,request,ack,input_at,selecting,form_result=struct.unpack_from('<10H',code)
u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000);u.mem_write(0x1000,code)
start=0xa0000+8*160;attrs=0xa2000+8*160
original=bytes((i*7)&255 for i in range(7*160));original_attrs=bytes((i*11)&255 for i in range(7*160))
u.mem_write(start,original);u.mem_write(attrs,original_attrs)
def tick(*,arrows=0,shot=0,enter=0,esc=0):
    keys=(1 if arrows&4 else 0)|(2 if arrows&0x20 else 0)|(4 if shot or enter else 0)|(8 if esc else 0)
    u.mem_write(0x1000+input_at,struct.pack('<H',keys))
    for reg,value in ((UC_X86_REG_CS,0x100),(UC_X86_REG_DS,0),(UC_X86_REG_SS,0x8000),(UC_X86_REG_SP,0xff00)):
        u.reg_write(reg,value)
    u.mem_write(0x8ff00,struct.pack('<H',0xfff0));u.emu_start(0x1000+entry,0x10ff0,count=30000)
def read(at):return u.mem_read(0x1000+at,1)[0]
tick();assert read(visible)==1 and read(choice)==0
assert bytes(u.mem_read(0xa2000+10*160+15*2,52))==b'\xe5\0'*26
tick(arrows=0x20);assert read(choice)==1
assert bytes(u.mem_read(0xa2000+10*160+15*2,52))==b'\xe1\0'*26
assert bytes(u.mem_read(0xa2000+12*160+15*2,52))==b'\xe5\0'*26
tick(arrows=0x20);assert read(choice)==1
tick();tick(shot=2);assert read(request)==2
u.mem_write(0x1000+ack,b'\x01');tick();assert read(request)==0 and read(visible)==1 and read(hold)==1
tick();tick(arrows=0x20);assert read(choice)==2
tick();tick(enter=0x10);assert read(choice)==2 and read(visible)==1 and read(hold)==1
tick();tick(esc=1);assert read(choice)==0
tick();tick(enter=0x10);assert read(request)==1
u.mem_write(0x1000+ack,b'\x01');tick();assert read(visible)==0 and read(hold)==0
assert bytes(u.mem_read(start,7*160))==original and bytes(u.mem_read(attrs,7*160))==original_attrs
u.mem_write(0x1000+hold,b'\x01');u.mem_write(0x1000+form_result,b'\x01')
tick();tick(arrows=0x20);tick();tick(arrows=0x20);tick();tick(shot=2)
assert read(selecting)==1 and read(hold)==1 and read(visible)==0, 'selected target waits for native restart'
tick();assert read(visible)==0 and read(hold)==1
print('native result menu: navigation, replay acknowledgement and screen restore passed')
