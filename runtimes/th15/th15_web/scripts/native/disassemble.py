"""Developer-only raw x86 disassembly for native-oracle analysis."""
import argparse
import pathlib
import struct
import sys
root = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(root / "tools" / "capstone"))
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
parser = argparse.ArgumentParser()
parser.add_argument("executable", type=pathlib.Path)
parser.add_argument("address", type=lambda value: int(value, 0))
parser.add_argument("--length", type=lambda value: int(value, 0), default=512)
args = parser.parse_args()
data = args.executable.read_bytes()
pe = struct.unpack_from("<I", data, 0x3c)[0]
count, optional_size = struct.unpack_from("<H", data, pe + 6)[0], struct.unpack_from("<H", data, pe + 20)[0]
base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
rva = args.address - base
for index in range(count):
    section = pe + 24 + optional_size + index * 40
    virtual_size, virtual_start, raw_size, raw_start = struct.unpack_from("<IIII", data, section + 8)
    if virtual_start <= rva < virtual_start + max(virtual_size, raw_size):
        offset = raw_start + rva - virtual_start
        break
else:
    raise SystemExit("Address is outside PE sections")
for instruction in Cs(CS_ARCH_X86, CS_MODE_32).disasm(data[offset:offset + args.length], args.address):
    print(f"{instruction.address:08x} {instruction.mnemonic.upper()} {instruction.op_str}")
