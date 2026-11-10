from unicorn import Uc,UC_ARCH_X86,UC_MODE_16
from unicorn.x86_const import *
import struct
import json, subprocess
from pathlib import Path
root=Path(__file__).resolve().parents[3]
script="""import{readFileSync}from'node:fs';import{gunzipSync}from'node:zlib';import{Fat12}from'./ports/th02/web/solo-fat12.js';import{installNetPause}from'./common/browser/pc98-net-pause.mjs';import{installChallengeHud}from'./common/browser/pc98-challenge-hud-install.mjs';import{installRescueSound}from'./ports/th02/web/managed-rescue-sound.mjs';const p=JSON.parse(readFileSync('ports/th02/web/local-patch.json')),o=JSON.parse(readFileSync('ports/th02/web/netplay/native-pause-overlay.json'));for(const lang of ['cn','jp'])for(const challenge of [false,true]){let f=new Fat12(gunzipSync(readFileSync(`ports/th02/web/th02-local-${lang}.hdi.gz`))),b=f.read(f.find('HUMAROKU/MAIN.EXE'));b=installNetPause(b,o.languages[lang]);if(challenge)b=installChallengeHud(b,'02',p).bytes;b=installRescueSound(b,p,o.languages[lang]);process.stdout.write(Buffer.from(b.subarray(p.headerSize+p.codeSegment*16,p.headerSize+p.codeSegment*16+65536)));}"""
images=subprocess.check_output(['node','--input-type=module','-e',script],cwd=root)
for code in [images[i:i+65536] for i in range(0,len(images),65536)]:
 cs=0x1000;ds=0x3000;base=cs*16;data=ds*16
 for enabled,out,want in [(0,0,(10,10)),(0,1,(210,50)),(1,0,(210,50)),(1,1,(210,50))]:
  u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000);u.mem_write(base,code[:0x10000]);u.reg_write(UC_X86_REG_CS,cs);u.reg_write(UC_X86_REG_DS,ds);u.reg_write(UC_X86_REG_SS,0x5000);u.reg_write(UC_X86_REG_SP,0xff00);u.mem_write(0x5ff00,b'\x00\x90');u.reg_write(UC_X86_REG_AX,200);u.reg_write(UC_X86_REG_DX,50)
  def w8(a,x):u.mem_write(base+a,bytes([x]))
  def w16(a,x):u.mem_write(base+a,struct.pack('<h',x))
  u.mem_write(data+0x2b86,struct.pack('<hh',10,10));w8(0x8634,enabled);w8(0x852a,out);w8(0x74e6,3)
  for i,(x,y,live) in enumerate([(10,10,1-out),(210,50,1),(300,50,1)]):w16(0x74e7+i*8,x);w16(0x74e9+i*8,y);w8(0x74eb+i*8,live)
  u.emu_start(base+0x75b8,base+0x9000,count=1000);got=(u.reg_read(UC_X86_REG_AX),u.reg_read(UC_X86_REG_DX));assert got==want,(enabled,out,got,want)
# Exercise the real live-player refresh before the selector, for each seat.
for code in [images[i:i+65536] for i in range(0,len(images),65536)]:
 for ghost in range(3):
  u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000);u.mem_write(0x10000,code);u.reg_write(UC_X86_REG_CS,0x1000);u.reg_write(UC_X86_REG_DS,0x3000);u.reg_write(UC_X86_REG_SS,0x5000);u.reg_write(UC_X86_REG_SP,0xff00)
  u.mem_write(0x5ff00,b'\x00\x90');u.mem_write(0x18528,b'\x03');u.mem_write(0x1852a+ghost,b'\x01');u.mem_write(0x34686,struct.pack('<hh',100,100));u.mem_write(0x1856e,struct.pack('<hh',200,100));u.mem_write(0x185ac,struct.pack('<hh',300,100));
  u.emu_start(0x17528,0x19000,count=300);assert u.mem_read(0x174eb+ghost*8,1)==b'\x00'
  u.reg_write(UC_X86_REG_SP,0xff00);u.mem_write(0x5ff00,b'\x00\x90');u.reg_write(UC_X86_REG_AX,[100,200,300][ghost]);u.reg_write(UC_X86_REG_DX,100);u.mem_write(0x18634,b'\x01');u.emu_start(0x175b8,0x19000,count=800)
  selected=u.mem_read(0x17517,1)[0];assert selected!=ghost,(ghost,selected)
print('P1-only and nearest targeting skip P1 ghost: passed')

# The distance key must rank a ghost ahead of a closer living recipient,
# and preserve nearest-first ordering within either recipient class.
for code in [images[i:i+65536] for i in range(0,len(images),65536)]:
 def key02(ghost,distance):
  u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000);u.mem_write(0x10000,code);u.reg_write(UC_X86_REG_CS,0x1000);u.reg_write(UC_X86_REG_SS,0x5000);u.reg_write(UC_X86_REG_SP,0xff00);u.mem_write(0x5ff00,b'\x00\x90');u.reg_write(UC_X86_REG_BX,1);u.reg_write(UC_X86_REG_EAX,distance);u.mem_write(0x10000+0x852b,bytes([ghost]));u.mem_write(0x10000+0x5eb5,struct.pack('<I',0xffffffff));u.emu_start(0x10000+0x8f58,0x19000,count=100);return u.reg_read(UC_X86_REG_EAX)
 assert key02(1,200)<key02(0,0)
 assert key02(1,10)<key02(1,20) and key02(0,10)<key02(0,20)
print('TH02 ghost rescue priority and living transfer ordering passed')
