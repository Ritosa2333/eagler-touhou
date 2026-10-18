import hashlib,json,pathlib,struct,sys
root=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(root/'.cache/binary_python'))
import pefile,capstone
raw=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe').read_bytes()
sha=hashlib.sha256(raw).hexdigest();assert sha=='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe=pefile.PE(data=raw)
def data(va,n):return pe.get_data(va-pe.OPTIONAL_HEADER.ImageBase,n)
floats={f'{va:08x}':struct.unpack('<f',data(va,4))[0] for va in (0x56cd98,0x56d7c0,0x56fe7c,0x56fe5c,0x56fe84,0x56fe78,0x56fe80,0x56fe88)}
doubles={f'{va:08x}':struct.unpack('<d',data(va,8))[0] for va in (0x56fe60,0x56fe68,0x56fe70)}
report={'original_sha256':sha,'floats':floats,'doubles':doubles,'name_56fe1c':data(0x56fe1c,64).split(b'\0')[0].hex()}
out=pathlib.Path(__file__).parent
(out/'evidence/constants.json').write_text(json.dumps(report,indent=2),encoding='utf8')
lines=['#pragma once','namespace th20::source::card::data {']
for va,v in floats.items():lines.append(f'inline constexpr float f_{va}={v!r}f;')
for va,v in doubles.items():lines.append(f'inline constexpr double d_{va}={v!r};')
lines.append('}')
(out/'data_constants.hpp').write_text('\n'.join(lines)+'\n',encoding='ascii')
print(json.dumps(report))
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
for i in md.disasm(data(0x54ffa0,10),0x54ffa0):print(hex(i.address),i.mnemonic,i.op_str)
for base in (0x59a740,0x56fe44):
    values=struct.unpack('<7I',data(base,28));print(hex(base),[hex(v) for v in values])
    for va in values:
        if 0x56a000<=va<0x5b2000:print(hex(va),data(va,32).hex())
