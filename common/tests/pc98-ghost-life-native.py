import re,subprocess,struct
from pathlib import Path
from unicorn import Uc,UC_ARCH_X86,UC_MODE_16
from unicorn.x86_const import *
root=Path(__file__).resolve().parents[2]
def images(game):
 source=(root/f'ports/th{game}/tests'/('ghost-target.py' if game=='02' else 'ghost-rescue.py')).read_text(encoding='utf8')
 script=re.search(r'script="""(.*?)"""',source,re.S).group(1)
 return subprocess.check_output(['node','--input-type=module','-e',script],cwd=root)
def machine(code):
 u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000);u.mem_write(0x10000,code);u.reg_write(UC_X86_REG_CS,0x1000);u.reg_write(UC_X86_REG_DS,0x3000);u.reg_write(UC_X86_REG_SS,0x5000);u.reg_write(UC_X86_REG_SP,0xff00);return u
for game in ['02','04']:
 data=images(game)
 for variant in range(4):
  code=data[variant*65536:(variant+1)*65536]
  for count in (2,3):
   for kind in ('item','extend'):
    for ghost in range(count):
     # A living picker is required for a pickup; a ghost cannot collect one.
     if kind=='item' and ghost==0:continue
     u=machine(code)
     if game=='02':
      banks=[2]*count;banks[ghost]=0;active=[0];u.mem_write(0x30b2c,bytes([banks[0]]));u.mem_write(0x18528,struct.pack('<H',count));u.mem_write(0x1852a,bytes(1 if i==ghost else 0 for i in range(3)));u.mem_write(0x17d14,b'\x04');u.mem_write(0x17d15,b'\x00');u.mem_write(0x14ff0,b'\xc3')
      def swap(uc,address,size,_):
       if address==0x14ff0:
        banks[active[0]]=uc.mem_read(0x30b2c,1)[0];active[0]=uc.reg_read(UC_X86_REG_AX);uc.mem_write(0x30b2c,bytes([banks[active[0]]]))
      u.hook_add(4,swap)
      if kind=='item':start,end=0x7f1b,0x7f92
      else:u.mem_write(0x18099,b'\x90'*10);start,end=0x8047,0x80aa
      u.emu_start(0x10000+start,0x10000+end,count=3000);banks[active[0]]=u.mem_read(0x30b2c,1)[0]
      assert banks[ghost]==1,(game,variant,count,kind,ghost,banks)
      assert u.mem_read(0x1852a+ghost,1)==b'\x01'
      u.reg_write(UC_X86_REG_SP,0xff00);u.emu_start(0x10000+start,0x10000+end,count=3000);banks[active[0]]=u.mem_read(0x30b2c,1)[0]
      assert banks[ghost]==2 and u.mem_read(0x1852a+ghost,1)==b'\x01'
     else:
      u.mem_write(0x18b79,b'\xc3');u.mem_write(0x18bb5,b'\xc3');u.mem_write(0x195a2,bytes([count]));u.mem_write(0x195e6,b'\x00')
      for seat in range(count):u.mem_write(0x195a5+seat*16+4,bytes([0 if seat==ghost else 3,0,1 if seat==ghost else 0]))
      if kind=='item':u.reg_write(UC_X86_REG_BX,4);u.reg_write(UC_X86_REG_AH,100);start,end=0x8d0d,0x8d52
      else:u.mem_write(0x5ff00,b'\x00\x90');start,end=0x8e92,0x9000
      u.emu_start(0x10000+start,0x10000+end,count=3000)
      for seat in range(count):
       value=u.mem_read(0x195a5+seat*16+4,3);expected=2 if seat==ghost else (3 if kind=='item' and seat==0 else 4)
       assert value[0]==expected,(game,variant,count,kind,ghost,seat,list(value),expected)
       assert value[2]==(1 if seat==ghost else 0),(game,variant,count,kind,ghost,seat,list(value))
      u.reg_write(UC_X86_REG_SP,0xff00);u.mem_write(0x5ff00,b'\x00\x90');u.reg_write(UC_X86_REG_BX,4);u.reg_write(UC_X86_REG_AH,100);u.emu_start(0x10000+start,0x10000+end,count=3000)
      value=u.mem_read(0x195a5+ghost*16+4,3);assert value[0]==3 and value[2]==1,(game,variant,count,kind,ghost,list(value))
      # Native rescue clears ghost state but must retain the saved reserves.
      u.mem_write(0x187af,b'\xc3');u.mem_write(0x1ac4f,b'\xc3');u.reg_write(UC_X86_REG_SI,0x95a5+ghost*16);u.reg_write(UC_X86_REG_DX,ghost);u.reg_write(UC_X86_REG_SP,0xff00);u.mem_write(0x5ff00,b'\x00\x90');u.emu_start(0x18d55,0x19000,count=3000)
      value=u.mem_read(0x195a5+ghost*16+4,3);assert value[0]==3 and value[2]==0,(game,variant,count,kind,ghost,list(value))
  print('TH'+game,'CN' if variant<2 else 'JP','challenge' if variant%2 else 'normal','extend and 1UP preserve ghosts')
