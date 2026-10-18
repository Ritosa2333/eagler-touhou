"""Extract data used by 5240d0. Emits constants, never executable instructions."""
from pathlib import Path
import hashlib,json,struct,sys
base=Path(__file__).resolve().parent;root=base.parents[1]
sys.path.insert(0,str(root/'.cache/binary_python'))
import pefile
manifest=json.loads((root/'reports/source_manifest.json').read_text(encoding='utf-8'))
raw=(Path(manifest['source_directory'])/'th20.exe').read_bytes()
sha=hashlib.sha256(raw).hexdigest()
assert sha=='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe=pefile.PE(data=raw)
def read(va,n):return pe.get_data(va-0x400000,n)
def string(va):return read(va,2048).split(b'\0')[0]
def literal(b):return '"'+''.join('\\%03o'%x if x<32 or x>=127 or x in (34,92) else chr(x) for x in b)+'"'
lines=['#pragma once','namespace th20::source::title::replay_draw_data {']
tables={}
for name,va,count in [('formats',0x5b0a74,5),('characters',0x5aff80,2),('difficulties',0x5aff90,6),('stones',0x5afffc,8),('finished_stages',0x5affd0,11),('stages',0x5affa8,10)]:
    values=[string(struct.unpack('<I',read(va+4*i,4))[0]) for i in range(count)]
    lines.append('inline constexpr const char* '+name+'[]={'+','.join(map(literal,values))+'};')
    tables[name]={'va':hex(va),'values':[v.hex() for v in values]}
for va in [0x57527c,0x57528c]:lines.append(f'inline constexpr char s_{va:08x}[]={literal(string(va))};')
for va in [0x56cd94,0x56fa30,0x56f2fc,0x56e734,0x575678,0x56cd98,0x572644]:lines.append(f'inline constexpr float f_{va:08x}={struct.unpack("<f",read(va,4))[0].hex()}f;')
lines.append('}')
(base/'replay_draw_data.hpp').write_text('\n'.join(lines)+'\n',encoding='ascii')
(base/'evidence/replay_draw_data.json').write_text(json.dumps({'original_sha256':sha,'tables':tables},indent=2)+'\n',encoding='ascii')
print('Extracted all six replay display tables and two score formats.')
