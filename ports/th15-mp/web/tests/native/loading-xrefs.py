"""Read-only native timing evidence; never patches the user's executable."""
import sys
import struct
import pefile
import capstone

pe = pefile.PE(sys.argv[1])
engine = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
needle = struct.pack('<I', int(sys.argv[2], 16))
for section in pe.sections:
    if not section.Characteristics & 0x20000000:
        continue
    data = section.get_data()
    at = data.find(needle)
    while at >= 0:
        start = max(0, at - 40)
        address = pe.OPTIONAL_HEADER.ImageBase + section.VirtualAddress + start
        print('\nXREF', hex(pe.OPTIONAL_HEADER.ImageBase + section.VirtualAddress + at))
        for instruction in engine.disasm(data[start:at+60], address):
            print(hex(instruction.address), instruction.mnemonic, instruction.op_str)
        at = data.find(needle, at + 4)
