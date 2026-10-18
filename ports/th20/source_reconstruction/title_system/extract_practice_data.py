import hashlib,json,pathlib,struct,sys
BASE=pathlib.Path(__file__).resolve().parent;ROOT=BASE.parents[1];sys.path.insert(0,str(ROOT/'.cache/binary_python'))
import pefile
raw=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe').read_bytes()
sha=hashlib.sha256(raw).hexdigest();assert sha=='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe=pefile.PE(data=raw)
def ints(va,count):return list(struct.unpack('<'+'i'*count,pe.get_data(va-0x400000,count*4)))
difficulties=ints(0x5af058,113);groups=ints(0x5af220,7*13*5);last=ints(0x5b0a54,3)
strings={va:pe.get_data(va-0x400000,4096).split(b'\0')[0] for va in [0x57555c,0x575528,0x575534,0x5755b0,0x57557c,0x575594]}
floats={va:struct.unpack('<f',pe.get_data(va-0x400000,4))[0] for va in [0x575678,0x571058,0x57567c,0x571060,0x57565c,0x575684,0x570ae0,0x56e734,0x570acc]}
out=['#pragma once','namespace th20::source::title::practice_data {']
for va,b in strings.items():out.append(f'inline constexpr char s_{va:08x}[]="'+''.join(f'\\x{x:02x}' for x in b)+'";')
for va,f in floats.items():out.append(f'inline constexpr float f_{va:08x}={f.hex()}f;')
out.append('inline constexpr int difficulties[]={'+','.join(map(str,difficulties))+'};')
out.append('inline constexpr int groups[7][13][5]={')
for stage in range(7):out.append('{'+','.join('{'+','.join(map(str,groups[stage*65+row*5:stage*65+row*5+5]))+'}' for row in range(13))+'},')
out.extend(['};','inline constexpr int boss_counts[]={3,3,4,4,4,7,13};','inline constexpr int initial_resume[]={'+','.join(map(str,last))+'};','}'])
(BASE/'practice_data.hpp').write_text('\n'.join(out)+'\n',encoding='ascii')
(BASE/'evidence/practice_data.json').write_text(json.dumps({'original_sha256':sha,'difficulties':difficulties,'groups':groups,'resume':last,'strings':{f'{k:08x}':v.hex() for k,v in strings.items()},'floats':{f'{k:08x}':v for k,v in floats.items()}},indent=2)+'\n',encoding='ascii')
print(json.dumps({'strings':{hex(k):v.decode('cp932') for k,v in strings.items()},'resume':last,'difficulty_range':[min(difficulties),max(difficulties)]},ensure_ascii=False))
