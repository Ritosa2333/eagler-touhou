import hashlib,json,pathlib,struct,sys
BASE=pathlib.Path(__file__).resolve().parent;ROOT=BASE.parents[1]
sys.path.insert(0,str(ROOT/'.cache/binary_python'))
import pefile
raw=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe').read_bytes()
sha=hashlib.sha256(raw).hexdigest();assert sha=='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe=pefile.PE(data=raw)
addresses=[0x571ee0,0x571ef8,0x571f1c,0x571f28,0x571f40,0x571f50,0x571f5c]
strings={f'{va:08x}':pe.get_data(va-0x400000,1024).split(b'\0')[0] for va in addresses}
lines=['#pragma once','namespace th20::source::notice::data {']
for va,b in strings.items():lines.append(f'inline constexpr char s_{va}[]="'+''.join(f'\\x{x:02x}' for x in b)+'";')
lines.append('}')
(BASE/'data_strings.hpp').write_text('\n'.join(lines)+'\n',encoding='ascii')
tables={f'{va:08x}':[f'{n:08x}' for n in struct.unpack('<7I',pe.get_data(va-0x400000,28))] for va in [0x571ffc,0x572018,0x572034,0x572050,0x57206c]}
owner_vtable=[f'{n:08x}' for n in struct.unpack('<3I',pe.get_data(0x571ea8-0x400000,12))]
(BASE/'evidence/data.json').write_text(json.dumps({'original_sha256':sha,'strings':{k:v.hex() for k,v in strings.items()},'text_completion_vtables':tables,'owner_vtable':owner_vtable},indent=2)+'\n',encoding='ascii')
print(json.dumps({'completion_vtables':tables,'owner_vtable':owner_vtable}))
