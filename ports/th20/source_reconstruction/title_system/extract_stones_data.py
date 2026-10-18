import hashlib,json,pathlib,struct,sys
BASE=pathlib.Path(__file__).resolve().parent;ROOT=BASE.parents[1]
sys.path.insert(0,str(ROOT/'.cache/binary_python'))
import pefile
raw=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe').read_bytes()
sha=hashlib.sha256(raw).hexdigest();assert sha=='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe=pefile.PE(data=raw)
floats={va:struct.unpack('<f',pe.get_data(va-0x400000,4))[0] for va in [0x575664,0x570ad0,0x575654,0x575658,0x56f10c,0x575688,0x57568c,0x572660,0x575694,0x56e0ec,0x56cda8,0x570388]}
unknown=pe.get_data(0x5755f0-0x400000,256).split(b'\0')[0];sequence=struct.unpack('<8I',pe.get_data(0x5755cc-0x400000,32))
out=['#pragma once','namespace th20::source::title::stones_data {']
for va,f in floats.items():out.append(f'inline constexpr float f_{va:08x}={f.hex()}f;')
out.append('inline constexpr char locked_title[]="'+''.join(f'\\x{x:02x}' for x in unknown)+'";')
out.append('inline constexpr unsigned char sequence[]={'+','.join(str(x) for x in sequence)+'};');out.append('}')
(BASE/'stones_data.hpp').write_text('\n'.join(out)+'\n',encoding='ascii')
(BASE/'evidence/stones_data.json').write_text(json.dumps({'original_sha256':sha,'float_data':{f'{k:08x}':v for k,v in floats.items()},'locked_title_cp932':unknown.hex(),'key_sequence':list(sequence)},indent=2)+'\n',encoding='ascii')
print(json.dumps({'floats':floats,'sequence':list(sequence)}))
