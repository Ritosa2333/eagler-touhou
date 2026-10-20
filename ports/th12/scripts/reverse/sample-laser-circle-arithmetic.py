"""Actual Moving circle predicate basic block under gameplay FPU precision."""
import pathlib,sys,struct,hashlib,json
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import *
exe=ROOT.parent/'th12/th12.exe';sha=hashlib.sha256(exe.read_bytes()).hexdigest()
assert sha=='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
pe=pefile.PE(str(exe));rows=[]
for x,y,radius in [(43.16087341308594,89.25546264648438,58),(43.1608772277832,89.25546264648438,58),(43.16086959838867,89.25546264648438,58),(3,132,5),(0,128,0)]:
 mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(0x400000,pe.get_memory_mapped_image());mu.mem_map(0x1000000,0x10000)
 sp,bp,origin,scratch=0x1001000,0x1003000,0x1005000,0x1009000
 put=lambda p,fmt,*v:mu.mem_write(p,struct.pack('<'+fmt,*v))
 put(sp+0x14,'ff',x,y);put(origin,'ff',0,128);put(bp+8,'If',origin,radius*radius)
 mu.mem_write(scratch,bytes.fromhex('dbe3')+bytes.fromhex('d9ee')*4)
 mu.emu_start(scratch,scratch+10,count=8);mu.reg_write(UC_X86_REG_FPCW,0x7f)
 mu.reg_write(UC_X86_REG_ESP,sp);mu.reg_write(UC_X86_REG_EBP,bp)
 mu.emu_start(0x42a328,0x42a360,count=80)
 assert mu.reg_read(UC_X86_REG_EIP)==0x42a360
 distance=struct.unpack('<f',mu.mem_read(sp+0x20,4))[0]
 rows.append({'point':[x,y],'origin':[0,128],'radius':radius,'nativeDistanceSquared':distance,'nativeCancelled':bool(mu.reg_read(UC_X86_REG_EFLAGS)&4)})
out=ROOT/'tests/fixtures/laser-circle-arithmetic-original.json';out.write_text(json.dumps({'exeSha256':sha,'scope':'Original42a328..42a360 circle predicate, controlled stack/FPU context and inputs; no instruction or result replacement','x87ControlWord':'0x007f','wholeGameOracle':False,'rows':rows},indent=2)+'\n',encoding='utf-8')
print(json.dumps(rows))
