"""Read-only inspection of the retail drop selector and scatter math.

The diagnostic executes 0x412100 / 0x412140 in isolation. Item creation and
memset are intercepted, so it captures requested spawn arguments only. It is
not an ordinary test dependency or a complete game Oracle.
"""
import pathlib, sys, struct, json, hashlib
TITLE = pathlib.Path(__file__).resolve().parents[1]
WORKSPACE = TITLE.parent
sys.path.insert(0, str(WORKSPACE / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EAX, UC_X86_REG_ESI, UC_X86_REG_EIP

exe = WORKSPACE / 'th12/th12.exe'
pe = pefile.PE(str(exe))
records = []
for primary, counts in [(0, {}), (1, {}), (2, {1: 3, 2: 3}), (0, {1: 2, 2: 5})]:
    vm = Uc(UC_ARCH_X86, UC_MODE_32)
    vm.mem_map(0x400000, (pe.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095)
    vm.mem_write(0x400000, pe.get_memory_mapped_image())
    vm.mem_map(0x1000000, 0x10000)
    vm.mem_map(0x2000000, 0x10000)
    sp, stop, drops, position = 0x100e000, 0x100f000, 0x2000000, 0x2001000
    vm.mem_write(sp, struct.pack('<I', stop))
    vm.mem_write(position, struct.pack('<fff', 20, 100, 0))
    vm.mem_write(drops, struct.pack('<i', primary))
    for item_type, count in counts.items():
        vm.mem_write(drops + item_type * 4, struct.pack('<i', count))
    vm.mem_write(drops + 0x50, struct.pack('<ff', 32, 48))
    vm.mem_write(0x4ce568, struct.pack('<HHI', 1234, 0, 0))
    captured = []
    def intercept(vm, address, size, unused):
        if address not in (0x4273f0, 0x477420):
            return
        stack = vm.reg_read(UC_X86_REG_ESP)
        ret, a, b, c = struct.unpack('<IIII', vm.mem_read(stack, 16))
        if address == 0x4273f0:
            captured.append({'type': a, 'position': struct.unpack('<fff', vm.mem_read(b, 12)),
                             'angle': struct.unpack('<f', struct.pack('<I', c))[0],
                             'speed': struct.unpack('<f', vm.mem_read(stack + 16, 4))[0]})
            vm.reg_write(UC_X86_REG_ESP, stack + 20)
        else:
            vm.mem_write(a, bytes([b & 255]) * c)
            vm.reg_write(UC_X86_REG_ESP, stack + 4)
        vm.reg_write(UC_X86_REG_EIP, ret)
    vm.hook_add(UC_HOOK_CODE, intercept)
    vm.reg_write(UC_X86_REG_ESP, sp)
    vm.reg_write(UC_X86_REG_EAX, position)
    vm.reg_write(UC_X86_REG_ESI, drops)
    vm.emu_start(0x412100, stop, count=100000)
    records.append({'primary': primary, 'counts': counts, 'items': captured,
                    'seed': struct.unpack('<H', vm.mem_read(0x4ce568, 2))[0],
                    'rngCalls': struct.unpack('<I', vm.mem_read(0x4ce56c, 4))[0]})
result = {'schema': 'th12-drop-inspection/1', 'originalExeSha256': hashlib.sha256(exe.read_bytes()).hexdigest(),
          'scope': 'requested-item-spawns-and-scatter-only', 'wholeGameDeterminism': False,
          'provider': 'original instructions 0x412100 / 0x412140, Unicorn; item allocation intercepted',
          'parameters': {'position': [20, 100, 0], 'spread': [32, 48], 'seed': 1234}, 'records': records}
output = TITLE / 'artifacts/original-drop-inspection.json'
output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
print(output)
