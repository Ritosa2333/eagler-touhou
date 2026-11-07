"""Exercise pause screen preservation with the actual 16-bit routines."""
from pathlib import Path
import os,struct,subprocess,tempfile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_16,UC_HOOK_INTR
from unicorn.x86_const import *
here=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory() as tmp:
 src=Path(tmp)/'test.asm';out=Path(tmp)/'test.bin'
 src.write_text("bits 16\ndw practice_pause_screen_save,practice_pause_screen_restore,practice_pause_pack_pixel,practice_pause_unpack_pixel,practice_pause_mask,practice_pause_gfx_borders,practice_pause_copy,practice_pause_scroll,practice_pause_gfx,practice_pause_gfx_save,practice_pause_gfx_restore,practice_pause_gfx_buttons,test_scroll\n%define SOLO_MAILBOX 0\n%define PRACTICE_PAGE_FRONT 0\n%define PRACTICE_PAGE_BACK 0\n%macro PRACTICE_CAPTURE_SCROLL 0\n mov ax,[cs:test_scroll]\n%endmacro\n%macro PRACTICE_PAUSE_WAIT 0\n nop\n%endmacro\n"+f'%include "{(here/"practice-pause.inc").as_posix()}"\n'+f'%include "{(here/"practice-text.inc").as_posix()}"\ntest_scroll: dw 0\n',encoding='ascii')
 subprocess.run([os.environ.get('NASM','nasm'),'-f','bin',str(src),'-o',str(out)],check=True);code=out.read_bytes()
save,restore,pack,unpack,mask,borders,copy,scroll,gfx,gfxsave,gfxrestore,buttons,testscroll=struct.unpack_from('<13H',code);u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000);u.mem_write(0x1000,code)
chars=bytes((i*17+3)&255 for i in range(4000));attrs=bytes((i*7+1)&255 for i in range(4000));u.mem_write(0xa0000,chars);u.mem_write(0xa2000,attrs)
def intr(uc,n,user):
 assert n==0x21
 ah=uc.reg_read(UC_X86_REG_AH)
 if ah==0x48:uc.reg_write(UC_X86_REG_AX,0x5000);uc.reg_write(UC_X86_REG_EFLAGS,uc.reg_read(UC_X86_REG_EFLAGS)&~1)
 elif ah!=0x49:raise AssertionError(ah)
u.hook_add(UC_HOOK_INTR,intr)
def call(entry,limit=300000):
 for reg,val in ((UC_X86_REG_CS,0x100),(UC_X86_REG_SS,0x8000),(UC_X86_REG_SP,0xff00)):u.reg_write(reg,val)
 u.mem_write(0x8ff00,struct.pack('<H',0xfff0));u.emu_start(0x1000+entry,0x10ff0,count=limit);assert u.reg_read(UC_X86_REG_IP)==0xfff0
call(save)
for row in range(25):
 assert bytes(u.mem_read(0xa0000+row*160,112))==b'\x20\x00'*56
 assert bytes(u.mem_read(0xa2000+row*160,112))==b'\x00\x00'*56
call(restore)
assert bytes(u.mem_read(0xa0000,4000))==chars and bytes(u.mem_read(0xa2000,4000))==attrs
for keep in (0x55,0xaa):
 u.mem_write(0x1000+mask,bytes([keep]))
 for value in range(256):
  u.mem_write(0x60000,bytes([value]));u.reg_write(UC_X86_REG_ES,0x6000);u.reg_write(UC_X86_REG_DI,0)
  call(pack);packed=u.reg_read(UC_X86_REG_AL);preserved=u.mem_read(0x60000,1)[0]
  u.reg_write(UC_X86_REG_AL,packed);call(unpack)
  assert preserved|u.reg_read(UC_X86_REG_AL)==value,(keep,value)
for shift in (0,103,399):
 planes=(0xa8000,0xb0000,0xb8000,0xe0000)
 original=bytes((i*37+11)&255 for i in range(32000))
 for plane in planes:u.mem_write(plane,original)
 u.reg_write(UC_X86_REG_GS,0x5000);u.mem_write(0x1000+gfx,struct.pack('<H',0x6000));u.mem_write(0x1000+scroll,struct.pack('<H',shift));u.mem_write(0x1000+copy,b'\x00')
 call(borders)
 for plane in planes:
  for y in range(400):
   row=bytes(u.mem_read(plane+((y+shift)%400)*80,80))
   assert row[:2]==bytes(2) and row[54:58]==bytes(4)
   if y<16 or y>=384:assert row[:58]==bytes(58)
 u.mem_write(0x1000+copy,b'\x01');call(borders)
 for plane in planes:assert bytes(u.mem_read(plane,32000))==original
for shift in (0,103,399):
 for plane in planes:u.mem_write(plane,original)
 u.mem_write(0x1000+testscroll,struct.pack('<H',shift));u.reg_write(UC_X86_REG_DS,0)
 call(gfxsave,3000000);call(buttons,3000000);call(gfxrestore,3000000)
 for plane in planes:assert bytes(u.mem_read(plane,32000))==original,('full pause restore',shift,hex(plane))
print('pause screen: text, full graphics and scrolling margins restored')
