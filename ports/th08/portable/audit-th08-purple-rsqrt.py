"""Read-only native-hook evidence; never copies or modifies retail assets."""
import argparse
import hashlib
import json
import re
import struct
from pathlib import Path
import capstone
import pefile

parser = argparse.ArgumentParser()
parser.add_argument("executable", type=Path)
parser.add_argument("purple_source", type=Path)
args = parser.parse_args()
source = args.purple_source.read_text(encoding="utf-8-sig")
hooks = re.findall(r"HOOK_RSQRTSS\((0x[0-9A-Fa-f]+),\s*(\d+),\s*(\d+)\)", source)
if len(hooks) != 12:
    raise SystemExit("Unexpected purple TH08 RSQRT hook membership")
pe = pefile.PE(str(args.executable))
decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
base = pe.OPTIONAL_HEADER.ImageBase
# Native SIMD dispatch installers 004867B0/004868D0. These are function
# table slots, not guessed semantic substitutions for individual instructions.
owners = {
    0x48d452: (0x1c, 0x48d3d0), 0x48d527: (0x54, 0x48d4a0),
    0x48da95: (0x20, 0x48da50), 0x48e324: (0x34, 0x48e290),
    0x48e424: (0x6c, 0x48e380), 0x48e56c: (0x70, 0x48e510),
    0x48e6c8: (0x50, 0x48e680), 0x48f032: (0x1c, 0x48efb0),
    0x48f107: (0x54, 0x48f080), 0x48f3b4: (0x34, 0x48f320),
    0x48f4b4: (0x6c, 0x48f410), 0x48f5fc: (0x70, 0x48f5a0),
}
text = next(s for s in pe.sections if s.Name.rstrip(b'\0') == b'.text')
code = text.get_data()
code_base = base + text.VirtualAddress
raw = bytes(pe.__data__)
dispatch = pe.get_data(0x4867b0-base, 0x4869ef-0x4867b0)
for slot, start in owners.values():
    # Both CPU installers must actually write the recorded function address.
    if struct.pack('<I', start) not in dispatch:
        raise SystemExit(f'Missing native dispatch owner {start:#x}')
evidence = []
for address, destination, operand in hooks:
    address = int(address, 16)
    instruction = next(decoder.disasm(pe.get_data(address-base, 4), address), None)
    expected = f"xmm{destination}, xmm{operand}"
    if not instruction or instruction.mnemonic != "rsqrtss" or instruction.op_str != expected or instruction.size != 4:
        raise SystemExit(f"Retail/purple mismatch at {address:#x}")
    # Begin at the exact hook instruction, not an arbitrary pre-hook byte
    # that could split an instruction and manufacture a false disassembly.
    context = []
    for item in decoder.disasm(pe.get_data(address-base, 128), address):
        context.append({"address":hex(item.address), "instruction":item.mnemonic+" "+item.op_str})
        if item.mnemonic == "ret":
            break
    slot, start = owners[address]
    indirect = []
    callers = []
    for i in range(len(code)-6):
        if code[i:i+2] in (b'\xff\x25', b'\xff\x15') and struct.unpack_from('<I',code,i+2)[0] == 0x4c8710+slot:
            wrapper = code_base+i
            indirect.append(hex(wrapper))
            callers += [hex(code_base+j) for j in range(len(code)-5)
                        if code_base+j < 0x476000 and code[j] == 0xe8
                        and code_base+j+5+struct.unpack_from('<i',code,j+1)[0] == wrapper]
            if raw.count(struct.pack('<I',wrapper)):
                raise SystemExit(f'Indirect address reference requires review: {wrapper:#x}')
    if (slot == 0x1c) != bool(callers):
        raise SystemExit(f'Unexpected game RSQRT owner usage at slot {slot:#x}')
    evidence.append({"hook":hex(address), "bytes":instruction.bytes.hex(), "instructions":context,
                     "dispatch_slot":hex(slot), "native_function":hex(start),
                     "indirect_entrypoints":indirect, "game_callers":callers,
                     "portable_owner":"GraphicsMath::normalize SIMD" if callers else "unused retail D3DX dispatch slot"})
print(json.dumps({"executable_sha256":hashlib.sha256(args.executable.read_bytes()).hexdigest(),
                  "purple_source_sha256":hashlib.sha256(source.replace("\r\n","\n").encode()).hexdigest(),
                  "hooks":evidence, "owner_mapping_verified":True,
                  "coverage":"Two used Vec3 SIMD implementations map to the shared portable normalize owner; ten hooks belong to uncalled retail D3DX slots. No unused APIs are fabricated."}, indent=2))
