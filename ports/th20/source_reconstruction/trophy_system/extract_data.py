import hashlib,json,pathlib,sys,struct
BASE=pathlib.Path(__file__).resolve().parent;ROOT=BASE.parents[1]
sys.path.insert(0,str(ROOT/'.cache/binary_python'))
import pefile,capstone
raw=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe').read_bytes()
sha=hashlib.sha256(raw).hexdigest();assert sha=='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe=pefile.PE(data=raw)
def string(va):return pe.get_data(va-0x400000,1024).split(b'\0')[0]
addresses=[0x571f5c,0x575700,0x575764,0x570afc,0x5757cc,struct.unpack('<I',pe.get_data(0x5b0aa8-0x400000,4))[0]]
strings={f'{v:08x}':string(v) for v in addresses}
out=['#pragma once','namespace th20::source::trophy::data {']
for v,b in strings.items():out.append(f'inline constexpr char s_{v}[]="'+''.join(f'\\x{x:02x}' for x in b)+'";')
out += [f'inline constexpr const char* announcement=s_{addresses[-1]:08x};','}']
(BASE/'data_strings.hpp').write_text('\n'.join(out)+'\n',encoding='ascii')
(BASE/'evidence/strings.json').write_text(json.dumps({'original_sha256':sha,'strings':{k:v.hex() for k,v in strings.items()}},indent=2)+'\n',encoding='ascii')
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
for vt in [0x5757d8,0x5757f4]:
    values=struct.unpack('<7I',pe.get_data(vt-0x400000,28));print('closure',hex(vt),[hex(v) for v in values])
    for va in values[2:3]:print(''.join(f'{i.address:08x} {i.mnemonic} {i.op_str}\n' for i in md.disasm(pe.get_data(va-0x400000,100),va)))
print({hex(v):struct.unpack('<f',pe.get_data(v-0x400000,4))[0] for v in [0x56c8d0,0x56c8cc,0x56c8e0]})
for va in [0x571148,0x571214,0x5712e8,0x571374]:
    print('laser slot44',hex(va),hex(struct.unpack('<I',pe.get_data(va+0x44-0x400000,4))[0]))
