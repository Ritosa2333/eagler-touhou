"""Read-only literal evidence used by recovered Enemy state/drop opcodes."""
import hashlib,json,pathlib,sys,struct
BASE=pathlib.Path(__file__).resolve().parent;ROOT=BASE.parents[1]
sys.path.insert(0,str(ROOT/'.cache/binary_python'))
import pefile
raw=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe').read_bytes()
sha=hashlib.sha256(raw).hexdigest();assert sha=='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe=pefile.PE(data=raw);out=['#pragma once','namespace th20::source::gameplay::opcode_data {'];evidence={'original_sha256':sha,'strings':{},'float_bits':{}}
for va in [0x56feb4,0x56fec0,0x56fecc,0x56fedc]:
    b=pe.get_data(va-0x400000,256).split(b'\0')[0];out.append(f'inline constexpr char s_{va:08x}[]="'+''.join(f'\\x{x:02x}' for x in b)+'";');evidence['strings'][f'{va:08x}']=b.hex()
for va in [0x56e0ec,0x56e0f0,0x56e0f8,0x56fd34,0x56c8d0,0x56c8e0]:
    b=pe.get_data(va-0x400000,4);v=struct.unpack('<f',b)[0];out.append(f'inline constexpr float f_{va:08x}={v!r}f;');evidence['float_bits'][f'{va:08x}']=b.hex()
out.append('}');(BASE/'enemy_opcode_data.hpp').write_text('\n'.join(out)+'\n',encoding='ascii')
(BASE/'evidence_state_opcodes/data.json').write_text(json.dumps(evidence,indent=2)+'\n',encoding='ascii')
print(json.dumps(evidence))
