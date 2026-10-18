"""Translate the original Pause drawing data into C++ constants; no code copied."""
from pathlib import Path
import hashlib, json, struct, sys
root=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(root/'.cache/binary_python'))
import pefile
manifest=json.loads((root/'reports/source_manifest.json').read_text(encoding='utf-8'))
raw=(Path(manifest['source_directory'])/'th20.exe').read_bytes()
assert hashlib.sha256(raw).hexdigest()=='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe=pefile.PE(data=raw)
def read(va,n):return pe.get_data(va-0x400000,n)
def string(va):return read(va,2048).split(b'\0',1)[0]
def literal(data):return '"'+''.join(chr(b) if 32<=b<127 and b not in (34,92) else '\\%03o'%b for b in data)+'"'
lines=['#pragma once','namespace th20::source::pause::draw_data {']
floats=[0x56fb7c,0x56fa28,0x56f7a4,0x572658,0x56f10c,0x56cd90,0x572644,0x56f2fc,0x56ed14,0x56e734,0x572650,0x572648,0x56fe7c,0x57265c,0x56d7c0,0x572660]
for va in floats:
    f=struct.unpack('<f',read(va,4))[0]
    lines.append(f'inline constexpr float f_{va:08x}={repr(f)}f;')
for va in [0x56e0e4,0x572524,0x572528,0x57254c,0x572570,0x572598,0x5725b4,0x5725d8,0x5725f8,0x572604]:
    lines.append(f'inline constexpr char s_{va:08x}[]={literal(string(va))};')
va=0x5aff7c
lines.append('inline constexpr char name_characters[]='+literal(string(struct.unpack('<I',read(va,4))[0]))+';')
for name,va,count in [('characters',0x5aff80,2),('score_stages',0x5affa8,10),('replay_stages',0x5affd0,11),('difficulties',0x5b001c,5)]:
    values=[literal(string(struct.unpack('<I',read(va+i*4,4))[0])) for i in range(count)]
    lines.append(f'inline constexpr const char* {name}[]={{'+','.join(values)+'};')
lines.append('}')
(Path(__file__).parent/'draw_constants.hpp').write_text('\n'.join(lines)+'\n',encoding='ascii')
print('\n'.join(lines))
