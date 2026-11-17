import subprocess,struct
from pathlib import Path
from unicorn import Uc,UC_ARCH_X86,UC_MODE_16
from unicorn.x86_const import *
root=Path(__file__).resolve().parents[3]
script="""import{readFileSync}from'node:fs';import{gunzipSync}from'node:zlib';import{Fat12}from'./ports/th04/web/solo-fat12.js';import{installNetPause}from'./common/browser/pc98-net-pause.mjs';import{installChallengeHud}from'./common/browser/pc98-challenge-hud-install.mjs';import{installDimBody}from'./ports/th04/web/managed-dim-body.mjs';const overlays=JSON.parse(readFileSync('ports/th04/web/netplay/native-pause-overlay.json'));for(const lang of ['cn','jp'])for(const challenge of [false,true]){const suffix=lang==='jp'?'-jp':'';let p=JSON.parse(readFileSync(`ports/th04/web/patch${suffix}.json`)),f=new Fat12(gunzipSync(readFileSync(`ports/th04/web/th04-coop${suffix}.hdi.gz`))),b=installNetPause(f.read(f.find('GENSO/MAIN.EXE')),overlays.languages[lang]);if(challenge)b=installChallengeHud(b,'04',p).bytes;b=installDimBody(b,p,challenge);const hs=b[8]*16+b[9]*4096;process.stdout.write(Buffer.from(b.subarray(hs+p.code_segment*16,hs+p.code_segment*16+65536)));}"""
images=subprocess.check_output(['node','--input-type=module','-e',script],cwd=root)
for index in range(4):
 code=images[index*65536:(index+1)*65536];challenge=index%2;target=0xb920+160*challenge+192
 # Existing third-player sprite routine must remain complete.
 assert code[0xb920+160*challenge+173]==0xc3
 def key(ghost,distance):
  u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000);u.mem_write(0x10000,code);u.reg_write(UC_X86_REG_CS,0x1000);u.reg_write(UC_X86_REG_SS,0x5000);u.reg_write(UC_X86_REG_SP,0xff00);u.mem_write(0x5ff00,b'\x00\x90');u.reg_write(UC_X86_REG_SI,0x100);u.reg_write(UC_X86_REG_EAX,distance);u.mem_write(0x10106,bytes([ghost]));u.mem_write(0x19e86,struct.pack('<I',0xffffffff));u.emu_start(0x10000+target,0x19000,count=100);return u.reg_read(UC_X86_REG_EAX)
 assert key(1,100000)<key(0,0)
 assert key(1,10)<key(1,20) and key(0,10)<key(0,20)
 # The native aiming loop must ignore every ghost seat.
 for ghost in range(3):
  u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000);u.mem_write(0x10000,code);u.reg_write(UC_X86_REG_CS,0x1000);u.reg_write(UC_X86_REG_DS,0x3000);u.reg_write(UC_X86_REG_SS,0x5000);u.reg_write(UC_X86_REG_SP,0xff00)
  u.mem_write(0x5ff00,bytes(16));u.mem_write(0x195a2,b'\x03');u.mem_write(0x3464e,struct.pack('<h',100));u.mem_write(0x34650,struct.pack('<h',100));
  for seat,(x,y,base) in enumerate([(100,100,0),(200,100,0x9372),(300,100,0xa858)]):
   u.mem_write(0x195ab+seat*16,bytes([seat==ghost]));
   if seat:u.mem_write(0x10000+base,struct.pack('<hh',x,y))
  # Aim from the selected ghost's position; choose nearest living player.
  x=[100,200,300][ghost];u.mem_write(0x5ff04,struct.pack('<hh',x,100));u.emu_start(0x18ad3,0x18b5f,count=1000)
  selected=u.mem_read(0x1ac0d,1)[0];assert selected!=ghost,(index,ghost,selected)
print('TH04 ghost rescue priority and three-player code passed in CN/JP and challenge modes')
