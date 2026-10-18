"""Read-only extraction of exact instructions/constants used by this module."""
import hashlib
import json
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / '.cache/binary_python'))
import capstone
import pefile

EXE = pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe')
raw = EXE.read_bytes()
assert hashlib.sha256(raw).hexdigest() == 'a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe = pefile.PE(data=raw)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
out = pathlib.Path(__file__).parent / 'evidence'
out.mkdir(exist_ok=True)
for address, size in [(0x4b9c10, 0x1ee), (0x41fb10, 0x13d), (0x4b9b80, 0x88),
                      (0x4dc1c0, 0x344), (0x41aa70, 0xc0), (0x410e70, 0x1f0),
                      (0x41b4f0, 0x386), (0x41cb10, 0x160), (0x542e00, 0x60),
                      (0x54a4e0, 0x5a), (0x559d30, 0x242), (0x559a70, 0x22)]:
    instructions = md.disasm(pe.get_data(address - 0x400000, size), address)
    (out / f'{address:08x}.asm').write_text(''.join(f'{i.address:08x} {i.bytes.hex():24} {i.mnemonic:10} {i.op_str}\n' for i in instructions))
addresses = [0x56ca9c,0x56cab8,0x571c40,0x571c74,0x571cb0,0x571ccc,0x571cf4,
             0x571d14,0x571d3c,0x571d74,0x571d94,0x571dac,0x571dd0,0x571df0]
strings = {}
for address in addresses:
    value = pe.get_data(address - 0x400000, 1024).split(b'\0', 1)[0]
    strings[f'{address:08x}'] = {'hex': value.hex(), 'cp932': value.decode('cp932', errors='replace')}
(out / 'strings.json').write_text(json.dumps(strings, ensure_ascii=False, indent=2), encoding='utf8')
for address in [0x56cd88, 0x56cdd0, 0x56cdd8]:
    import struct
    print(hex(address), struct.unpack('<d',pe.get_data(address-0x400000,8))[0])
print(json.dumps(strings,ensure_ascii=False,indent=2))
