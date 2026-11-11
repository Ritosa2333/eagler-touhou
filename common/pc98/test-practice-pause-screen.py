"""Verify text-only pause and preservation of later transition updates."""
from pathlib import Path
import os,struct,subprocess,tempfile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_16,UC_HOOK_INTR
from unicorn.x86_const import *
here=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory() as tmp:
 src=Path(tmp)/'test.asm';out=Path(tmp)/'test.bin'
 src.write_text('bits 16\ndw practice_pause_screen_save,practice_pause_screen_restore,practice_pause_draw\n%define SOLO_MAILBOX 0\n%define PRACTICE_PAGE_FRONT 0\n%define PRACTICE_PAGE_BACK 0\n%macro PRACTICE_CAPTURE_SCROLL 0\n mov ax,[cs:test_scroll]\n%endmacro\n%macro PRACTICE_PAUSE_WAIT 0\n nop\n%endmacro\n'+f'%include "{(here/"practice-pause.inc").as_posix()}"\n'+f'%include "{(here/"practice-text.inc").as_posix()}"\ntest_scroll: dw 0\n',encoding='ascii')
 subprocess.run([os.environ.get('NASM','nasm'),'-f','bin',str(src),'-o',str(out)],check=True);code=out.read_bytes()
save,restore,draw=struct.unpack_from('<3H',code);u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000);u.mem_write(0x1000,code)
def intr(uc,n,user):
 assert n==0x21
 ah=uc.reg_read(UC_X86_REG_AH)
 if ah==0x48:uc.reg_write(UC_X86_REG_AX,0x5000);uc.reg_write(UC_X86_REG_EFLAGS,uc.reg_read(UC_X86_REG_EFLAGS)&~1)
 elif ah!=0x49:raise AssertionError(ah)
u.hook_add(UC_HOOK_INTR,intr)
def call(entry):
 for reg,val in ((UC_X86_REG_CS,0x100),(UC_X86_REG_SS,0x8000),(UC_X86_REG_SP,0xff00)):u.reg_write(reg,val)
 u.mem_write(0x8ff00,struct.pack('<H',0xfff0));u.emu_start(0x1000+entry,0x10ff0,count=2000000);assert u.reg_read(UC_X86_REG_IP)==0xfff0
chars=bytes((i*17+3)&255 for i in range(4000));attrs=bytes((i*7+1)&255 for i in range(4000));u.mem_write(0xa0000,chars);u.mem_write(0xa2000,attrs);call(save)
for row in range(25):
 actual=bytes(u.mem_read(0xa0000+row*160,160));original=chars[row*160:(row+1)*160]
 if row in (8,10,12,14):assert actual[:30]==original[:30] and actual[30:82]==b'\x20\x00'*26 and actual[82:]==original[82:]
 else:assert actual==original
planes=(0xa8000,0xb0000,0xb8000,0xe0000);original=bytes((i*37+11)&255 for i in range(32000))
for plane in planes:u.mem_write(plane,original)
call(draw)
for plane in planes:assert bytes(u.mem_read(plane,32000))==original
# A transition may update a text cell while paused. Do not restore stale data.
changed=0xa0000+8*160+30;u.mem_write(changed,b"Q\x00");u.mem_write(changed+0x2000,b"\xe1\x00")
call(restore);expected=bytearray(chars);expected[8*160+30:8*160+32]=b"Q\x00";expected_attr=bytearray(attrs);expected_attr[8*160+30:8*160+32]=b"\xe1\x00"
assert bytes(u.mem_read(0xa0000,4000))==expected and bytes(u.mem_read(0xa2000,4000))==expected_attr
print('pause: transition-updated text survives and other text restores')
