"""Regenerate read-only replay string evidence and C++ constants."""
import hashlib,json,pathlib,re,struct,sys
BASE=pathlib.Path(__file__).resolve().parent
ROOT=BASE.parents[1]
sys.path.insert(0,str(ROOT/'.cache/binary_python'))
import pefile
source=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe')
raw=source.read_bytes();sha=hashlib.sha256(raw).hexdigest()
assert sha=='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe=pefile.PE(data=raw)
def string(va):return pe.get_data(va-0x400000,2048).split(b'\0')[0]
def array(va,n):return [string(p) for p in struct.unpack('<'+'I'*n,pe.get_data(va-0x400000,4*n))]
def literal(b):return '"'+''.join(f'\\x{x:02x}' for x in b)+'"'
addresses=[int(s,16) for s in re.findall(r'char s_([0-9a-f]{8})', (BASE/'data_strings.hpp').read_text(encoding='utf8'))]
strings={f'{v:08x}':string(v) for v in addresses}
characters=array(0x5aff80,18);ranks=array(0x5aff90,5)
out=['#pragma once','namespace th20::source::replay::data {']
out += [f'inline constexpr char s_{v}[]={literal(b)};' for v,b in strings.items()]
out += [f'inline constexpr const char* {name}[]={{'+','.join(map(literal,values))+'};' for name,values in [('characters',characters),('ranks',ranks)]]
out += ['}']
(BASE/'data_strings.hpp').write_text('\n'.join(out)+'\n',encoding='ascii')
(BASE/'evidence/strings.json').write_text(json.dumps({'original_sha256':sha,'strings':{k:v.hex() for k,v in strings.items()},'characters_005aff80':[v.hex() for v in characters],'ranks_005aff90':[v.hex() for v in ranks]},indent=2)+'\n',encoding='ascii')
print(json.dumps({'strings':len(strings),'characters':len(characters),'ranks':len(ranks),'original_sha256':sha}))
