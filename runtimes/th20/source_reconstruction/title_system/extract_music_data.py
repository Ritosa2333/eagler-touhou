import hashlib,json,pathlib,struct,sys
BASE=pathlib.Path(__file__).resolve().parent;ROOT=BASE.parents[1]
sys.path.insert(0,str(ROOT/'.cache/binary_python'))
import pefile
raw=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe').read_bytes()
sha=hashlib.sha256(raw).hexdigest();assert sha=='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe=pefile.PE(data=raw)
warning=struct.unpack('<8I',pe.get_data(0x5b0a88-0x400000,32))
strings={va:pe.get_data(va-0x400000,4096).split(b'\0')[0] for va in [0x575508,0x5753e0,*warning]}
floats={va:struct.unpack('<f',pe.get_data(va-0x400000,4))[0] for va in [0x56fa28,0x56fe7c,0x56ec8c,0x56f090,0x56f10c,0x572644]}
out=['#pragma once','namespace th20::source::title::music_data {']
for va,b in strings.items():out.append(f'inline constexpr char s_{va:08x}[]="'+''.join(f'\\x{x:02x}' for x in b)+'";')
for va,f in floats.items():out.append(f'inline constexpr float f_{va:08x}={f.hex()}f;')
out.append('inline constexpr const char* warnings[]={'+','.join(f's_{va:08x}' for va in warning)+'};');out.append('}')
(BASE/'music_data.hpp').write_text('\n'.join(out)+'\n',encoding='ascii')
(BASE/'evidence/music_data.json').write_text(json.dumps({'original_sha256':sha,'strings':{f'{k:08x}':v.hex() for k,v in strings.items()},'float_data':{f'{k:08x}':v for k,v in floats.items()},'warning_table':[f'{x:08x}' for x in warning]},indent=2)+'\n',encoding='ascii')
print(json.dumps({'floats':floats,'warning_table':[hex(x) for x in warning]}))
