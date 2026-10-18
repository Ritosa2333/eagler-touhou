import hashlib,json,pathlib,struct,sys
BASE=pathlib.Path(__file__).resolve().parent;ROOT=BASE.parents[1]
sys.path.insert(0,str(ROOT/'.cache/binary_python'))
import pefile
raw=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe').read_bytes()
sha=hashlib.sha256(raw).hexdigest();assert sha=='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe=pefile.PE(data=raw)
labels=struct.unpack('<6I',pe.get_data(0x5affac-0x400000,24))
strings={va:pe.get_data(va-0x400000,4096).split(b'\0')[0] for va in [0x56e0e4,0x575124,0x575130,*labels]}
floats={va:struct.unpack('<f',pe.get_data(va-0x400000,4))[0] for va in [0x56fe5c,0x56fa30,0x575684,0x575668,0x570ad4,0x575654,0x56cda8,0x570388]}
last=struct.unpack('<i',pe.get_data(0x5b0a50-0x400000,4))[0]
out=['#pragma once','namespace th20::source::title::stage_select_data {']
for va,b in strings.items():out.append(f'inline constexpr char s_{va:08x}[]="'+''.join(f'\\x{x:02x}' for x in b)+'";')
for va,f in floats.items():out.append(f'inline constexpr float f_{va:08x}={f.hex()}f;')
out.append('inline constexpr const char* labels[]={'+','.join(f's_{va:08x}' for va in labels)+'};');out.extend([f'inline constexpr int initial_last_stage={last};','}'])
(BASE/'stage_select_data.hpp').write_text('\n'.join(out)+'\n',encoding='ascii')
(BASE/'evidence/stage_select_data.json').write_text(json.dumps({'original_sha256':sha,'strings':{f'{k:08x}':v.hex() for k,v in strings.items()},'floats':{f'{k:08x}':v for k,v in floats.items()},'last_stage':last},indent=2)+'\n',encoding='ascii')
print(json.dumps({'strings':{hex(k):v.decode('cp932') for k,v in strings.items()},'floats':floats,'last_stage':last},ensure_ascii=False))
