"""Record StageClearInf's data constants from the hash-checked original PE."""
import hashlib, json, pathlib, struct, sys
root = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(root / '.cache/binary_python'))
import pefile
raw = pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe').read_bytes()
sha = hashlib.sha256(raw).hexdigest()
assert sha == 'a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe = pefile.PE(data=raw)
def data(va, size): return pe.get_data(va - pe.OPTIONAL_HEADER.ImageBase, size)
floats = {f'{va:08x}': struct.unpack('<f', data(va, 4))[0] for va in (0x56ed14,0x56ec98,0x573fdc,0x573fe0,0x573fe4,0x573fe8,0x56f10c,0x56c8d0)}
formats = {f'{va:08x}': data(va, 256).split(b'\0')[0] for va in (0x573f50,0x573f64,0x573f7c,0x573f94,0x573fac,0x573fc4)}
report = {'original_sha256': sha, 'floats': floats, 'formats_hex': {va: value.hex() for va, value in formats.items()}}
out = pathlib.Path(__file__).parent
(out / 'evidence/constants.json').write_text(json.dumps(report, indent=2), encoding='utf8')
lines = ['#pragma once', 'namespace th20::source::stage_clear::data {']
for va, value in floats.items(): lines.append(f'inline constexpr float f_{va} = {value!r}f;')
for va, value in formats.items(): lines.append(f'inline constexpr char text_{va}[] = "' + ''.join(f'\\x{byte:02x}' for byte in value) + '";')
lines.append('}')
(out/'data_constants.hpp').write_text('\n'.join(lines)+'\n', encoding='ascii')
print(json.dumps(report))
print('empty closure tables:', {hex(va): [hex(p) for p in struct.unpack('<7I',data(va,28))] for va in (0x570994,0x570a3c)})
