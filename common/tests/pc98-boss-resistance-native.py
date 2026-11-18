import subprocess,struct
from pathlib import Path
from unicorn import Uc,UC_ARCH_X86,UC_MODE_16
from unicorn.x86_const import *
root=Path(__file__).resolve().parents[2]
script="""import{readFileSync}from'node:fs';import{gunzipSync}from'node:zlib';import{Fat12}from'./ports/th02/web/solo-fat12.js';import{installBossResistance}from'./common/browser/pc98-boss-resistance.mjs';for(const g of ['02','04'])for(const l of ['jp','cn']){const p=JSON.parse(readFileSync(g==='02'?'ports/th02/web/local-patch.json':`ports/th04/web/patch${l==='jp'?'-jp':''}.json`));const file=g==='02'?`ports/th02/web/th02-local-${l}.hdi.gz`:`ports/th04/web/th04-coop${l==='jp'?'-jp':''}.hdi.gz`;const f=new Fat12(gunzipSync(readFileSync(file)));const b=f.read(f.find(g==='02'?'HUMAROKU/MAIN.EXE':'GENSO/MAIN.EXE'));installBossResistance(b,g,p);const h=b[8]*16+b[9]*4096;const start=h+(g==='02'?p.codeSegment:p.code_segment)*16;process.stdout.write(Buffer.from(b.subarray(start,start+65536)));}"""
images=subprocess.check_output(['node','--input-type=module','-e',script],cwd=root)
for i,game in enumerate(['02','02','04','04']):
 code=images[i*65536:(i+1)*65536];u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000);u.mem_write(0x10000,code);u.reg_write(UC_X86_REG_CS,0x1000);u.reg_write(UC_X86_REG_DS,0x3000);u.reg_write(UC_X86_REG_SS,0x5000)
 start=0x81ff if game=='02' else 0xad7c; end=0x821b if game=='02' else 0xad9a; numerator=start+(6 if game=='02' else 1); remainder=0x7d04 if game=='02' else 0xad21
 def hit(value,mult):
  u.mem_write(0x10000+numerator,bytes([mult]));u.reg_write(UC_X86_REG_AX,value);u.reg_write(UC_X86_REG_BX,0);u.reg_write(UC_X86_REG_SI,remainder);u.reg_write(UC_X86_REG_SP,0xff00);u.mem_write(0x5ff00,b'\x00\x90');u.emu_start(0x10000+start,0x10000+end,count=100);return u.reg_read(UC_X86_REG_AX)
 for mult in (5,3,2):
  u.mem_write(0x10000+remainder,b'\0\0');total=sum(hit(1,mult) for _ in range(100));assert total==20*mult,(game,mult,total)
 u.mem_write(0x10000+remainder,b'\0\0');seq=[3,2,5,3,2]*25;total=sum(hit(1,m) for m in seq);assert total==sum(seq)//5,(game,total)
 # A restored checkpoint must reproduce both damage and the retained fraction.
 u.mem_write(0x10000+remainder,b'\0\0');hit(1,3)
 snapshot=bytes(u.mem_read(0x10000+remainder,2));expected=[hit(1,m) for m in [2,5,3,2,3]];ending=bytes(u.mem_read(0x10000+remainder,2))
 u.mem_write(0x10000+remainder,snapshot);assert [hit(1,m) for m in [2,5,3,2,3]]==expected;assert bytes(u.mem_read(0x10000+remainder,2))==ending
 print('TH'+game,'JP' if i%2==0 else 'CN','native damage and changing living count passed')

