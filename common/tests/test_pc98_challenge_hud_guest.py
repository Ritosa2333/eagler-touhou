from pathlib import Path
import struct,subprocess,tempfile,re
from unicorn import Uc,UC_ARCH_X86,UC_MODE_16,UC_HOOK_CODE
from unicorn.x86_const import *
root=Path(__file__).resolve().parents[2]
for game,origin,char,flush,count,row,step,col in [('02',0x8aa8,0x6c02,0x6c6c,0x8528,10,3,59),('04',0xb920,0x90c5,0x9125,0x95a2,7,2,69)]:
 with tempfile.TemporaryDirectory() as temp:
  output=Path(temp)/'hud.bin'
  args={'CH_ORG':origin,'CH_CHAR':char,'CH_FLUSH':flush,'CH_GAME':int(game),'CH_COUNT':count,'CH_ROW':row,'CH_STEP':step,'CH_COL':col,'CH_WIDTH':11}
  subprocess.run(['nasm','-f','bin',*[f'-D{k}={v}' for k,v in args.items()],str(root/'common/pc98/challenge-hud.asm'),'-o',str(output)],check=True)
  code=output.read_bytes()
  published=re.search(r"const code"+game+r"='([0-9a-f]+)'",(root/'common/browser/pc98-challenge-hud-install.mjs').read_text(encoding='utf-8')).group(1)
  assert code.hex()==published,(game,'native HUD binary is stale')
 for values in ([0,-23,2147483647],[-2147483648,12,0]):
  u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x40000);base=0x10000;stack=0x20000
  u.mem_write(base+origin,code);u.mem_write(base+char,b'\x47\x47\xc3');u.mem_write(base+flush,b'\xc3');u.mem_write(base+count,b'\x03\x00');
  for i,n in enumerate(values):u.mem_write(base+origin+i*4,struct.pack('<i',n))
  u.mem_write(stack+0xfefe,struct.pack('<H',0x4000));
  for reg,n in [(UC_X86_REG_CS,0x1000),(UC_X86_REG_DS,0x1000),(UC_X86_REG_SS,0x2000),(UC_X86_REG_SP,0xfefe),(UC_X86_REG_DI,100)]:u.reg_write(reg,n)
  cells={}
  def hook(uc,address,size,data):
   if address==base+char:cells[uc.reg_read(UC_X86_REG_DI)]=chr(uc.reg_read(UC_X86_REG_AX)&255)
   if address==base+0x4000:uc.emu_stop()
  u.hook_add(UC_HOOK_CODE,hook);u.emu_start(base+origin+12,base+0x4000,count=4000)
  for i,n in enumerate(values):
   at=(row+step*i)*160+col*2; actual=''.join(cells.get(at+j*2,'?') for j in range(11)).rstrip()
   assert actual==str(n),(game,i,n,actual)
  assert u.reg_read(UC_X86_REG_DI)==100
print('PASS TH02/TH04 native signed HUD digits and seat selection')
