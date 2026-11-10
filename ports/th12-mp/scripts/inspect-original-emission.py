"""Explicit, read-only inspection of TH12 1.00b emitter math.

Runs only the original launch selection block in an isolated diagnostic VM.
The output is evidence for focused regressions, not a complete game trace or a
replacement for the accepted Oracle goldens. Ordinary tests never run this.
"""
import pathlib, sys, struct, json, hashlib
TITLE = pathlib.Path(__file__).resolve().parents[1]
WORKSPACE = TITLE.parent
sys.path.insert(0, str(WORKSPACE / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EBP, UC_X86_REG_EBX, UC_X86_REG_ECX

exe = WORKSPACE / 'th12/th12.exe'
pe = pefile.PE(str(exe))
image = pe.get_memory_mapped_image()
records = []
for mode in range(9):
    for index, layer in [(0, 0), (1, 0), (2, 0), (3, 0), (0, 1)]:
        vm = Uc(UC_ARCH_X86, UC_MODE_32)
        vm.mem_map(0x400000, (pe.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095)
        vm.mem_write(0x400000, image)
        vm.mem_map(0x1000000, 0x10000)
        vm.mem_map(0x2000000, 0x10000)
        sp, emitter, bullet = 0x100e000, 0x2000000, 0x2001000
        vm.mem_write(emitter + 0x10, struct.pack('<ffff', .25, -.5, 4., 1.))
        vm.mem_write(emitter + 0x1f8, struct.pack('<hhh', 4, 2, mode))
        vm.mem_write(sp + 0x2c, struct.pack('<iif', index, layer, 1.5707963705062866))
        vm.mem_write(0x4ce568, struct.pack('<HHI', 1234, 0, 0))
        for register, value in [(UC_X86_REG_ESP, sp), (UC_X86_REG_EBP, emitter),
                                (UC_X86_REG_EBX, bullet), (UC_X86_REG_ECX, index)]:
            vm.reg_write(register, value)
        vm.emu_start(0x40a34d, 0x40a618, count=10000)
        records.append({'mode': mode, 'index': index, 'layer': layer,
                        'angle': struct.unpack('<f', vm.mem_read(sp + 0x28, 4))[0],
                        'speed': struct.unpack('<f', vm.mem_read(sp + 0x10, 4))[0],
                        'seed': struct.unpack('<H', vm.mem_read(0x4ce568, 2))[0],
                        'rngCalls': struct.unpack('<I', vm.mem_read(0x4ce56c, 4))[0]})
result = {'schema': 'th12-emitter-inspection/1', 'originalExeSha256': hashlib.sha256(exe.read_bytes()).hexdigest(),
          'scope': 'launch-angle-and-speed-only', 'wholeGameDeterminism': False,
          'provider': 'original instructions 0x40a34d through 0x40a617, Unicorn 2.1.4',
          'parameters': {'count': 4, 'layers': 2, 'angle': .25, 'spread': -.5,
                         'speed': 4, 'slowSpeed': 1, 'aim': 1.5707963705062866, 'seed': 1234},
          'records': records}
output = TITLE / 'artifacts/original-emission-inspection.json'
output.parent.mkdir(exist_ok=True)
output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
print(output)
