"""Execute original464f80 in the precision observed in the live D3D9 game."""
import pathlib,sys,struct,json,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe))
u=Uc(UC_ARCH_X86,UC_MODE_32)
u.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);u.mem_write(0x400000,pe.get_memory_mapped_image())
u.mem_map(0x1000000,0x10000);u.mem_map(0x2000000,0x1000)
# Diagnostic return thunk stores the original function's ST0 result unchanged.
u.mem_write(0x100f000,bytes.fromhex('d91d00000002'))
fractions=sorted({struct.pack('<f',i/n) for n in (26,40,80,120) for i in range(n+1)})
rows=[]
for mode in range(17):
 for raw in fractions:
  fraction=struct.unpack('<f',raw)[0]
  u.reg_write(UC_X86_REG_ESP,0x100e000);u.mem_write(0x100e000,struct.pack('<I',0x100f000)+raw)
  u.reg_write(UC_X86_REG_FPCW,0x7f);u.reg_write(UC_X86_REG_EAX,mode-1 if mode else 6)
  u.emu_start(0x464f80,0x100f006,count=10000)
  rows.append({'mode':mode,'inputBits':struct.unpack('<I',raw)[0],'outputBits':struct.unpack('<I',u.mem_read(0x2000000,4))[0]})
result={'originalExeSha256':EXE_SHA256,'entry':'0x464f80','x87ControlWord':'0x007f',
 'precisionEvidence':'Read-only Wow64GetThreadContext at live original frame18005; precision discrimination microcase (100000000+1)-100000000 gives0 for007f,1 for037f.',
 'scope':'Original scalar easing functions, gameplay precision, all17 modes; not whole game acceptance.','rows':rows}
path=ROOT/'tests/fixtures/gameplay-easing-original.json';path.write_text(json.dumps(result,separators=(',',':'))+'\n');print(len(rows),'original scalar samples')
