from pathlib import Path
import os,struct,subprocess,tempfile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_16,UC_HOOK_INTR
from unicorn.x86_const import *
root=Path(__file__).resolve().parents[1]/'pc98'
for game,size,res_size in ((2,0x93c0,48),(4,0xbdc0,74)):
 with tempfile.TemporaryDirectory() as tmp:
  src,out=Path(tmp)/'checkpoint.asm',Path(tmp)/'checkpoint.bin'
  src.write_text(f'bits 16\ncpu 386\n%define PRACTICE_CHECKPOINT_BYTES {size}\n'+('%define PRACTICE_RESTART_TH02 1\n' if game==2 else '')+
   'dw practice_checkpoint_save,practice_checkpoint_restore,practice_checkpoint_valid,target_practice,practice_initial_entry,practice_initial_stage,practice_initial_gengetsu\n'
   'target_practice: db 1\npractice_initial_entry: db 2\npractice_initial_stage: db '+('1' if game==2 else '6')+'\npractice_initial_gengetsu: db 1\n'
   +f'%include "{(root/"practice-checkpoint.inc").as_posix()}"\n')
  subprocess.run([os.environ.get('NASM','nasm'),'-f','bin',str(src),'-o',str(out)],check=True)
  image=out.read_bytes();save,restore,valid,target,entry,stage,gen=struct.unpack_from('<7H',image)
  u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000);code,data,resident=0x10000,0x30000,0x40100;u.mem_write(code,image)
  original=bytearray((i*29+5)&255 for i in range(size));ptr=0x27fc if game==2 else 0xba86;struct.pack_into('<HH',original,ptr,0x100,0x4000)
  before_res=bytes((i*13+17)&255 for i in range(res_size));u.mem_write(data,bytes(original));u.mem_write(resident,before_res)
  def interrupt(uc,n,user):
   assert n==0x21 and uc.reg_read(UC_X86_REG_AH)==0x48
   uc.reg_write(UC_X86_REG_AX,0x5000);uc.reg_write(UC_X86_REG_EFLAGS,uc.reg_read(UC_X86_REG_EFLAGS)&~1)
  u.hook_add(UC_HOOK_INTR,interrupt)
  def run(at):
   for r,v in ((UC_X86_REG_CS,0x1000),(UC_X86_REG_DS,0x3000),(UC_X86_REG_SS,0x8000),(UC_X86_REG_SP,0xff00)):u.reg_write(r,v)
   u.mem_write(0x8ff00,struct.pack('<H',0xfff0));u.emu_start(code+at,code+0xfff0,count=300000)
   assert u.reg_read(UC_X86_REG_IP)==0xfff0
  run(save);assert u.mem_read(code+valid,1)==b'\1'
  u.mem_write(data,bytes(size));u.mem_write(resident,bytes(res_size));run(save);run(restore)
  assert bytes(u.mem_read(data,size))==bytes(original) and bytes(u.mem_read(resident,res_size))==before_res
print('PASS TH02/TH04 game-owned checkpoint saves and restores data and resident')
