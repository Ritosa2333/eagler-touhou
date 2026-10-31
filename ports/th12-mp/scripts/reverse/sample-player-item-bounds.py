"""Original Player init/update rectangle arithmetic, independent of candidates."""
import pathlib,sys,json,struct
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);p=pefile.PE(str(exe));v=Uc(UC_ARCH_X86,UC_MODE_32)
for at,n in [(0x400000,(p.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),(0x1000000,0x10000),(0x2000000,0x20000)]:v.mem_map(at,n)
v.mem_write(0x400000,p.get_memory_mapped_image());PLAYER,SHT,STACK=0x2000000,0x2010000,0x100e000
def put(at,f,*x):v.mem_write(at,struct.pack('<'+f,*x))
def get(at,f):return list(struct.unpack('<'+f,v.mem_read(at,struct.calcsize('<'+f))))
rows=[]
for character in range(3):
 v.mem_write(PLAYER,bytes(0xd000));put(PLAYER+0xa2c,'I',SHT);put(SHT+4,'f',[2,3.5,3][character]);put(0x4b0c90,'i',character)
 v.reg_write(UC_X86_REG_ESP,STACK);v.reg_write(UC_X86_REG_EDI,PLAYER);v.reg_write(UC_X86_REG_FPCW,0x37f)
 v.emu_start(0x435d53,0x435dd8,count=2000)
 initialized=get(PLAYER+0x9e4,'9f')
 put(PLAYER+0x97c,'fff',100,200,0)
 v.emu_start(0x4371ef,0x4372cd,count=2000)
 rect=get(PLAYER+0xc444,'6f')
 rows.append({'character':character,'initializedHitHalfExtents':initialized[:2],'initializedPickupDimensions':initialized[3:5],'currentPickupRectangle':rect,'currentPickupHalfExtents':[100-rect[0],200-rect[1]],'nearHalfExtents':initialized[6:8]})
out=ROOT/'tests/fixtures/player-item-bounds-native.json';out.write_text(json.dumps({'schema':'th12-original-player-item-bounds/1','originalExeSha256':EXE_SHA256,'wholeGameOracle':False,'scope':'original435d53..435dd8 init and4371ef..4372cd current-frame rectangle arithmetic; no candidate inputs','rows':rows},indent=2)+'\n')
print(json.dumps(rows,indent=2))
