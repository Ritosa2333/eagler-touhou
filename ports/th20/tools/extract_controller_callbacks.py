"""Verify original draw wrappers and emit their literal registration table."""
import hashlib,json,pathlib,re,sys
ROOT=pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'.cache/binary_python'))
import pefile,capstone
source=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe')
raw=source.read_bytes();digest=hashlib.sha256(raw).hexdigest()
assert digest=='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe=pefile.PE(data=raw);dis=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
text=(ROOT/'analysis/ghidra/pseudocode/00447e00.c').read_text()
registrations=re.findall(r'FUN_00412360\((0x[0-9a-f]+|[0-9]+),FUN_([0-9a-f]+),param_1\);',text)
lines=['// Verified original literal draw-callback registrations from 0x447e00.'];entries=[]
for priority,address in registrations:
    priority=int(priority,0);va=int(address,16)
    body=[]
    for instruction in dis.disasm(pe.get_data(va-0x400000,40),va):
        body.append(instruction)
        if instruction.mnemonic=='ret':break
    reset_layer=False
    if len(body)==9:
        assert body[2].mnemonic=='mov' and body[2].op_str=='ecx, dword ptr [0x5c0028]'
        assert body[3].mnemonic=='call' and body[3].op_str=='0x44ee70'
        reset_layer=True;body=body[:2]+body[4:]
    assert [x.mnemonic for x in body]==['push','mov','push','mov','call','pop','ret'],hex(va)
    assert body[0].op_str=='ebp' and body[1].op_str=='ebp, esp' and body[3].op_str=='ecx, dword ptr [ebp + 8]'
    assert body[4].op_str=='0x449e40' and body[5].op_str=='ebp'
    layer=int(body[2].op_str,0)
    lines.append(f'    {{{priority},&draw_callback<{layer},{str(reset_layer).lower()}>}}, // 0x{va:08x}')
    entries.append({'priority':priority,'original_callback_va':f'0x{va:08x}','layer':layer,'reset_global_layer':reset_layer})
assert len(entries)==50
target=ROOT/'source_reconstruction/sprite_renderer'
(target/'controller_callbacks.inc').write_text('\n'.join(lines)+'\n',encoding='ascii')
(target/'controller_callbacks.json').write_text(json.dumps({'original_sha256':digest,'count':len(entries),'entries':entries},indent=2)+'\n')
print(f'Verified {len(entries)} direct draw-layer wrappers')
