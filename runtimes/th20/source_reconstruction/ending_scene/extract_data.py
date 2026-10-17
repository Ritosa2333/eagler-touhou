"""Read-only EndingInf tables and literal evidence from the verified specimen."""
import hashlib,json,pathlib,sys,struct
BASE=pathlib.Path(__file__).resolve().parent;ROOT=BASE.parents[1]
sys.path.insert(0,str(ROOT/'.cache/binary_python'))
import pefile,capstone
raw=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe').read_bytes()
sha=hashlib.sha256(raw).hexdigest();assert sha=='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe=pefile.PE(data=raw)
def u32(va):return struct.unpack('<I',pe.get_data(va-0x400000,4))[0]
def string(va):return pe.get_data(va-0x400000,1024).split(b'\0')[0]
def literal(b):return '"'+''.join(f'\\x{x:02x}' for x in b)+'"'
tables={'endings':(0x5afca0,23),'credits':(0x5afce8,5)}
out=['#pragma once','namespace th20::source::ending::data {'];evidence={'original_sha256':sha}
for name,(va,count) in tables.items():
    values=[string(u32(va+i*4)) for i in range(count)];out.append(f'inline constexpr const char* {name}[]={{'+','.join(map(literal,values))+'};');evidence[name]=[v.hex() for v in values]
for va in [0x56f610,0x56fe1c]:out.append(f'inline constexpr char s_{va:08x}[]={literal(string(va))};')
for va in [0x56cda8,0x570388,0x56c8d8]:
    value=struct.unpack('<f',pe.get_data(va-0x400000,4))[0];out.append(f'inline constexpr float f_{va:08x}={repr(value)}f;');evidence[f'{va:08x}']=value
out.append('}');(BASE/'data_constants.hpp').write_text('\n'.join(out)+'\n',encoding='ascii')
(BASE/'evidence/data.json').write_text(json.dumps(evidence,indent=2)+'\n',encoding='ascii')
print('closure invocations',[(hex(v),hex(u32(v+8))) for v in [0x57031c,0x570338,0x570354,0x570370]])
print('opcode targets',[(i,hex(u32(0x4a03dc+4*i))) for i in range(18)])
print('float constants',[(k,v) for k,v in evidence.items() if isinstance(v,float)])
print('file table',[(i,string(u32(0x5afca0+4*i)).decode('ascii','replace')) for i in range(23)])
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
for va,size in [(0x49e240,0x70),(0x49e2b0,0x70)]:
    print(hex(va),[(i.mnemonic,i.op_str) for i in md.disasm(pe.get_data(va-0x400000,size),va) if i.mnemonic=='call'])
