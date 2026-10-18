import hashlib,json,pathlib,struct,sys
BASE=pathlib.Path(__file__).resolve().parent;ROOT=BASE.parents[1]
sys.path.insert(0,str(ROOT/'.cache/binary_python'))
import pefile
raw=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe').read_bytes()
sha=hashlib.sha256(raw).hexdigest();assert sha=='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe=pefile.PE(data=raw)
def words(va,n):return struct.unpack('<'+'I'*n,pe.get_data(va-0x400000,n*4))
characters=words(0x5aff88,2);stages=words(0x5affa8,9);digits=tuple(range(0x574f60,0x574f88,4));sequence=words(0x575298,12)
strings={va:pe.get_data(va-0x400000,4096).split(b'\0')[0] for va in [*characters,*stages,*digits,0x5752c8,0x575300,0x575334,0x575338,0x575348,0x575350,0x575388,0x575390]}
floats={va:struct.unpack('<f',pe.get_data(va-0x400000,4))[0] for va in [0x56f10c,0x56fa30,0x56fe7c,0x56fb7c,0x57566c,0x572644,0x56cda0,0x56ec9c,0x575680,0x575690]}
out=['#pragma once','namespace th20::source::title::player_data_constants {']
for va,b in strings.items():out.append(f'inline constexpr char s_{va:08x}[]="'+''.join(f'\\x{x:02x}' for x in b)+'";')
for va,f in floats.items():out.append(f'inline constexpr float f_{va:08x}={f.hex()}f;')
for key,table in [('characters',characters),('stages',stages),('digits',digits)]:out.append(f'inline constexpr const char* {key}[]={{'+','.join(f's_{va:08x}' for va in table)+'};')
out.append('inline constexpr unsigned unlock_keys[]={'+','.join(map(str,sequence))+'};');out.append('}')
(BASE/'player_data_constants.hpp').write_text('\n'.join(out)+'\n',encoding='ascii')
(BASE/'evidence/player_data_constants.json').write_text(json.dumps({'original_sha256':sha,'strings':{f'{k:08x}':v.hex() for k,v in strings.items()},'floats':{f'{k:08x}':v for k,v in floats.items()},'sequence':sequence},indent=2)+'\n',encoding='ascii')
print(json.dumps({'strings':{hex(k):v.decode('cp932') for k,v in strings.items()},'floats':floats,'sequence':sequence},ensure_ascii=False))
