from pathlib import Path
import os,struct,subprocess,tempfile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_16,UC_HOOK_INTR
from unicorn.x86_const import *
root=Path(__file__).resolve().parents[1]/'pc98'
for game,size,res_size in ((2,0x93c0,48),(4,0xbdc0,74)):
 with tempfile.TemporaryDirectory() as tmp:
  src,out=Path(tmp)/'checkpoint.asm',Path(tmp)/'checkpoint.bin'
  src.write_text(f'bits 16\ncpu 386\n%define PRACTICE_NATIVE_FORM_START 1\n%define PRACTICE_CHECKPOINT_BYTES {size}\n'+('%define PRACTICE_RESTART_TH02 1\n' if game==2 else '')+
   'dw practice_checkpoint_save,practice_checkpoint_restore,practice_checkpoint_valid,target_practice,practice_initial_entry,practice_initial_stage,practice_initial_gengetsu,practice_checkpoint_can_seek,practice_checkpoint_seekable,practice_checkpoint_retarget,practice_checkpoint_values,practice_form_values,practice_checkpoint_capture_baseline,practice_checkpoint_promote,practice_checkpoint_buffer,practice_checkpoint_baseline_buffer,practice_checkpoint_baseline_valid,practice_checkpoint_baseline_values\n'
   'target_practice: db 1\npractice_initial_entry: db 2\npractice_initial_stage: db '+('1' if game==2 else '6')+'\npractice_initial_gengetsu: db 1\npractice_form_values: db '+('1' if game==2 else '6')+',1,2,2,2,3,2,80,1,6\n'
   +f'%include "{(root/"practice-checkpoint.inc").as_posix()}"\n')
  subprocess.run([os.environ.get('NASM','nasm'),'-f','bin',str(src),'-o',str(out)],check=True)
  image=out.read_bytes();save,restore,valid,target,entry,stage,gen,can_seek,seekable,retarget,saved,form,capture,promote,buffer,basebuffer,basevalid,basevalues=struct.unpack_from('<18H',image)
  u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000);code,data,resident=0x10000,0x30000,0x40100;u.mem_write(code,image)
  original=bytearray((i*29+5)&255 for i in range(size));ptr=0x27fc if game==2 else 0xba86;struct.pack_into('<HH',original,ptr,0x100,0x4000)
  before_res=bytes((i*13+17)&255 for i in range(res_size));u.mem_write(data,bytes(original));u.mem_write(resident,before_res)
  next_segment=[0x5000];allocation_fails=[False]
  def interrupt(uc,n,user):
   assert n==0x21 and uc.reg_read(UC_X86_REG_AH)==0x48
   if allocation_fails[0]:uc.reg_write(UC_X86_REG_EFLAGS,uc.reg_read(UC_X86_REG_EFLAGS)|1);return
   uc.reg_write(UC_X86_REG_AX,next_segment[0]);next_segment[0]+=uc.reg_read(UC_X86_REG_BX)+1;uc.reg_write(UC_X86_REG_EFLAGS,uc.reg_read(UC_X86_REG_EFLAGS)&~1)
  u.hook_add(UC_HOOK_INTR,interrupt)
  def run(at):
   for r,v in ((UC_X86_REG_CS,0x1000),(UC_X86_REG_DS,0x3000),(UC_X86_REG_SS,0x8000),(UC_X86_REG_SP,0xff00)):u.reg_write(r,v)
   u.mem_write(0x8ff00,struct.pack('<H',0xfff0));u.emu_start(code+at,code+0xfff0,count=300000)
   assert u.reg_read(UC_X86_REG_IP)==0xfff0
  u.mem_write(code+seekable,b'\1')
  for kind in (0,1,2):
   u.mem_write(code+valid,b'\0');u.mem_write(code+entry,bytes([kind]));u.mem_write(data,bytes(original));u.mem_write(resident,before_res)
   run(save);assert u.mem_read(code+valid,1)==b'\1'
   u.mem_write(data,bytes(size));u.mem_write(resident,bytes(res_size));run(save);run(restore)
   assert bytes(u.mem_read(data,size))==bytes(original) and bytes(u.mem_read(resident,res_size))==before_res
  u.mem_write(code+entry,b'\2')
  for index,value,expected in ((4,3,1),(4,1,0),(0,0,0),(1,0,0),(2,1,0),(3,1,0),(5,4,0),(6,4,0),(7,1,0),(8,0,0),(9,0,0)):
   u.mem_write(code+form,bytes(u.mem_read(code+saved,10)));u.mem_write(code+form+index,bytes([value]));run(can_seek)
   assert u.mem_read(code+retarget,1)==bytes([expected]),(game,index,value)
  u.mem_write(code+form,bytes(u.mem_read(code+saved,10)));u.mem_write(code+form+4,b'\3')
  u.mem_write(code+seekable,b'\0');run(can_seek);assert u.mem_read(code+retarget,1)==b'\0'
  u.mem_write(code+seekable,b'\1')
  if game==2:
   u.mem_write(code+stage,b'\4');u.mem_write(code+saved+4,b'\1');u.mem_write(code+form+4,b'\x09');run(can_seek);assert u.mem_read(code+retarget,1)==b'\0'
  else:
   u.mem_write(code+gen,b'\0');run(can_seek);assert u.mem_read(code+retarget,1)==b'\0'
   u.mem_write(code+stage,b'\0');u.mem_write(code+saved,b'\0');u.mem_write(code+form,b'\0')
   run(can_seek);assert u.mem_read(code+retarget,1)==b'\1'
   u.mem_write(code+stage,b'\3');u.mem_write(code+saved,b'\3');u.mem_write(code+form,b'\3')
   run(can_seek);assert u.mem_read(code+retarget,1)==b'\1'
  # The encounter baseline and selected attack must occupy distinct DOS
  # buffers; replacing the latter must not alter the former.
  u.mem_write(code+stage,bytes([1 if game==2 else 6]));u.mem_write(code+gen,b'\1');u.mem_write(code+entry,b'\2');u.mem_write(code+seekable,b'\1')
  u.mem_write(code+form,bytes([1 if game==2 else 6,1,2,2,3,3,2,80,1,6]));u.mem_write(code+valid,b'\0');u.mem_write(code+basevalid,b'\0');u.mem_write(data,bytes(original));u.mem_write(resident,before_res)
  u.reg_write(UC_X86_REG_BX,1 if game==2 else 2);run(capture)
  assert u.mem_read(code+basevalid,1)==b'\1' and u.mem_read(code+valid,1)==b'\0'
  assert u.mem_read(code+basevalues+4,1)==bytes([1 if game==2 else 2])
  altered=bytearray(original);altered[0]=77;u.mem_write(data,bytes(altered));run(save)
  primary=bytes(u.mem_read(code+buffer,2));baseline=bytes(u.mem_read(code+basebuffer,2));assert primary!=baseline
  u.mem_write(code+form,bytes(u.mem_read(code+basevalues,10)));run(can_seek);assert u.mem_read(code+retarget,1)==b'\1'
  u.mem_write(data,bytes(size));run(restore);assert bytes(u.mem_read(data,size))==bytes(altered)
  u.mem_write(code+buffer,baseline);u.mem_write(data,bytes(size));run(restore);assert bytes(u.mem_read(data,size))==bytes(original)
  u.mem_write(code+buffer,primary)
  u.mem_write(code+basevalid,b'\0');u.mem_write(code+valid,b'\1');run(promote)
  assert u.mem_read(code+basevalid,1)==b'\1' and u.mem_read(code+valid,1)==b'\0' and bytes(u.mem_read(code+basebuffer,2))==primary
  allocation_fails[0]=True;u.mem_write(data,bytes(altered));run(save)
  assert u.mem_read(code+basevalid,1)==b'\0' and u.mem_read(code+valid,1)==b'\1'
  assert bytes(u.mem_read(code+buffer,2))==primary
  u.mem_write(data,bytes(size));run(restore);assert bytes(u.mem_read(data,size))==bytes(altered)
  u.mem_write(code+basevalid,b'\0');u.mem_write(code+valid,b'\0');u.mem_write(code+buffer,b'\0\0');run(capture)
  assert u.mem_read(code+basevalid,1)==b'\0' and u.mem_read(code+valid,1)==b'\0'
print('PASS TH02/TH04 game-owned checkpoint saves and restores data and resident')
