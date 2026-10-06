"""Explicit read-only TH12 UFO diagnostics, not a decompiler or full Oracle.

Exports native call sites/constants and executes only the tiny inventory and
colour-cycle selectors in an isolated VM. Rendering calls are intercepted.
Never writes the executable, runtime, replays, or accepted golden files.
"""
import hashlib, itertools, json, pathlib, struct, sys

TITLE = pathlib.Path(__file__).resolve().parents[2]
WORKSPACE = TITLE.parent
sys.path.insert(0, str(WORKSPACE / 'tools/python'))
import capstone, pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_EAX, UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI

EXE = WORKSPACE / 'th12/th12.exe'
EXPECTED_EXE_SHA256 = '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
actual_sha256 = hashlib.sha256(EXE.read_bytes()).hexdigest()
if actual_sha256 != EXPECTED_EXE_SHA256:
    raise SystemExit('TH12 1.00b identity mismatch; refusing version-specific addresses: ' + actual_sha256)
pe = pefile.PE(str(EXE))
base = pe.OPTIONAL_HEADER.ImageBase
image = pe.get_memory_mapped_image()
out = TITLE / 'artifacts/reverse/ufo-tokens'
out.mkdir(parents=True, exist_ok=True)
cs = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
cs.skipdata = True

def vm_new():
    vm = Uc(UC_ARCH_X86, UC_MODE_32)
    vm.mem_map(base, (pe.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095)
    vm.mem_write(base, image)
    vm.mem_map(0x1000000, 0x10000)
    vm.mem_map(0x2000000, 0x10000)
    vm.mem_write(0x4b2ed0, struct.pack('<f', 1.0))
    return vm

inventory = []
for colors in itertools.product((1, 2, 3), repeat=3):
    vm = vm_new()
    stock, sp, stop = 0x4b0c4c, 0x100e000, 0x100f000
    vm.mem_write(stock, bytes(28))
    gui_calls = []
    def skip_gui(vm, address, size, unused):
        if address == 0x421a60:
            gui_calls.append('0x421a60')
            stack = vm.reg_read(UC_X86_REG_ESP)
            ret = struct.unpack('<I', vm.mem_read(stack, 4))[0]
            vm.reg_write(UC_X86_REG_ESP, stack + 4)
            vm.reg_write(UC_X86_REG_EIP, ret)
    vm.hook_add(UC_HOOK_CODE, skip_gui)
    steps = []
    for color in colors:
        vm.mem_write(sp, struct.pack('<I', stop))
        vm.reg_write(UC_X86_REG_ESP, sp)
        vm.reg_write(UC_X86_REG_ESI, stock)
        vm.reg_write(UC_X86_REG_EDI, color)
        vm.emu_start(0x422e80, stop, count=10000)
        words = struct.unpack('<7i', vm.mem_read(stock, 28))
        steps.append({'slots': words[:3], 'count': words[3], 'uiState': words[4], 'other': words[5], 'summonKind': words[6], 'return': vm.reg_read(UC_X86_REG_EAX)})
    inventory.append({'inputColors': colors, 'steps': steps, 'interceptedGuiCalls': len(gui_calls)})

cycles = []
for item_type in (13, 14, 15):
    for age in (149, 150):
        vm = vm_new()
        vm.mem_map(0x2100000, 0x500000)
        vm.mem_write(0x4b43c8, struct.pack('<I', 0x2100000))
        vm.mem_write(0x2100000 + 0x4debdc, struct.pack('<I', 0x2003000))
        item, sp = 0x2000000, 0x100e000
        vm.mem_write(item + 0x9b4, struct.pack('<i', item_type))
        vm.mem_write(item + 0x99c, struct.pack('<iifII', age - 1, age, float(age), 0x4b2ed0, 1))
        binds = []
        def intercept_cycle(vm, address, size, unused):
            if address == 0x426368:
                vm.emu_stop()
            elif address == 0x4061e0:
                stack = vm.reg_read(UC_X86_REG_ESP)
                ret, bank, script = struct.unpack('<III', vm.mem_read(stack, 12))
                binds.append({'bank': bank, 'script': script})
                vm.reg_write(UC_X86_REG_ESP, stack + 12)
                vm.reg_write(UC_X86_REG_EIP, ret)
        vm.hook_add(UC_HOOK_CODE, intercept_cycle)
        vm.reg_write(UC_X86_REG_ESP, sp)
        vm.reg_write(UC_X86_REG_EDI, item)
        vm.reg_write(UC_X86_REG_EAX, age)
        vm.emu_start(0x426483, 0x4264d0, count=10000)
        cycles.append({'typeBefore': item_type, 'timerBefore': age,
                       'typeAfter': struct.unpack('<i', vm.mem_read(item + 0x9b4, 4))[0],
                       'timerAfter': struct.unpack('<i', vm.mem_read(item + 0x9a0, 4))[0],
                       'interceptedAnimationBinds': binds})

proximity = []
for distance, color_timer in [(59.999, 30), (60, 30), (60.001, 30), (0, 91), (100, 90)]:
    vm = vm_new()
    item, player, sp = 0x2000000, 0x2005000, 0x100e000
    vm.mem_write(0x4b4514, struct.pack('<I', player))
    vm.mem_write(item + 0x96c, struct.pack('<fff', distance, 0, 0))
    vm.mem_write(item + 0x9b4, struct.pack('<i', 13))
    vm.mem_write(item + 0x988, struct.pack('<iifII', 99, 100, 100., 0x4b2ed0, 1))
    vm.mem_write(item + 0x99c, struct.pack('<iifII', color_timer - 1, color_timer, float(color_timer), 0x4b2ed0, 1))
    vm.reg_write(UC_X86_REG_ESP, sp)
    vm.reg_write(UC_X86_REG_EDI, item)
    vm.reg_write(UC_X86_REG_EBX, 0)
    vm.emu_start(0x426276, 0x426375, count=10000)
    proximity.append({'distance': distance, 'colorTimerBefore': color_timer,
                      'colorTimerAfter': struct.unpack('<i', vm.mem_read(item + 0x9a0, 4))[0],
                      'lifeTimerAfterBeforeCommonIncrement': struct.unpack('<i', vm.mem_read(item + 0x98c, 4))[0],
                      'nearFlag': struct.unpack('<i', vm.mem_read(item + 0x9c4, 4))[0],
                      'animationInterrupt': struct.unpack('<h', vm.mem_read(item + 0x3c4, 2))[0]})

births = []
for item_type in range(10, 19):
    for side_counter in (0, 1):
        vm = vm_new()
        manager, position, sp = 0x2100000, 0x2001000, 0x100e000
        vm.mem_map(manager, 0x670000)
        vm.mem_write(0x4b44f0, struct.pack('<I', manager))
        vm.mem_write(manager + 0x666fe0, struct.pack('<i', side_counter))
        vm.mem_write(position, struct.pack('<fff', -200, -4, 0))
        vm.mem_write(sp, struct.pack('<IIIff', 0x100f000, item_type, position, .125, 7.0))
        vm.reg_write(UC_X86_REG_ESP, sp)
        def intercept_birth(vm, address, size, unused):
            if address in (0x4275a7, 0x42770c):
                vm.emu_stop()
            elif address == 0x453e20:
                stack = vm.reg_read(UC_X86_REG_ESP)
                ret = struct.unpack('<I', vm.mem_read(stack, 4))[0]
                vm.reg_write(UC_X86_REG_ESP, stack + 8)
                vm.reg_write(UC_X86_REG_EIP, ret)
        vm.hook_add(UC_HOOK_CODE, intercept_birth)
        vm.emu_start(0x4273f0, 0x100f000, count=10000)
        item = vm.reg_read(UC_X86_REG_ESI)
        births.append({'type': item_type, 'sideCounterBefore': side_counter,
                       'sideCounterAfter': struct.unpack('<i', vm.mem_read(manager + 0x666fe0, 4))[0],
                       'itemOffsetInManager': hex(item - manager),
                       'position': struct.unpack('<3f', vm.mem_read(item + 0x96c, 12)),
                       'velocity': struct.unpack('<3f', vm.mem_read(item + 0x978, 12)),
                       'state': struct.unpack('<i', vm.mem_read(item + 0x9b0, 4))[0],
                       'storedType': struct.unpack('<i', vm.mem_read(item + 0x9b4, 4))[0]})

constants = {}
for address, fmt, label in [(0x4a4410, 'd', 'nearDistanceSquared'), (0x4a440c, 'f', 'bounceLeft'), (0x4a4198, 'f', 'bounceRight'), (0x4a3f4c, 'f', 'bounceTop'), (0x4a4408, 'f', 'bounceBottom'), (0x4a4400, 'd', 'expiryLeft'), (0x4a43a0, 'd', 'expiryRight'), (0x4a43f8, 'f', 'expiryTop'), (0x4a3e90, 'd', 'expiryBottom'), (0x4a4274, 'f', 'initialAngleEven'), (0x4a4278, 'f', 'initialAngleOdd'), (0x4a4270, 'f', 'initialSpeed')]:
    constants[label] = {'address': hex(address), 'value': struct.unpack('<' + fmt, pe.get_data(address - base, struct.calcsize(fmt)))[0]}

targets = {0x4273f0, 0x4270b0, 0x422e80, 0x44a8a0, 0x44af00}
section = pe.sections[0]
instructions = list(cs.disasm(section.get_data(), base + section.VirtualAddress))
xrefs = {hex(target): [] for target in targets}
for index, ins in enumerate(instructions):
    if ins.mnemonic == 'call' and ins.op_str.startswith('0x'):
        target = int(ins.op_str, 16)
        if target in targets:
            xrefs[hex(target)].append({'address': hex(ins.address), 'context': [f'{i.address:08x} {i.mnemonic} {i.op_str}' for i in instructions[max(0, index - 8):index + 5]]})

regions = {'spawn': (0x4273f0, 0x427af6), 'tokenCollect': (0x4270b0, 0x4271b6), 'inventory': (0x422e80, 0x422f19), 'tokenMovement': (0x426258, 0x426870), 'rewardMovement': (0x426189, 0x426236), 'pickupTypes': (0x426c3f, 0x426da4), 'rewardSpawnPrefix': (0x44af00, 0x44b13a)}
for label, (start, end) in regions.items():
    lines = [f'{i.address:08x} {i.mnemonic:9} {i.op_str}' for i in cs.disasm(pe.get_data(start - base, end - start), start)]
    (out / (label + '.asm.txt')).write_text('\n'.join(lines) + '\n', encoding='utf-8')

if hashlib.sha256(EXE.read_bytes()).hexdigest() != actual_sha256:
    raise SystemExit('Retail executable identity changed during the diagnostic.')
result = {'schema': 'th12-ufo-token-inspection/1', 'originalExeSha256': actual_sha256,
          'method': 'Capstone disassembly and isolated original-instruction diagnostics; not decompiled C',
          'wholeGameDeterminism': False, 'ordinaryTestDependency': False,
          'interceptedCalls': {'0x421a60': 'inventory UI callback', '0x4061e0': 'animation bind', '0x453e20': 'spawn sound'},
          'inventory': inventory, 'colorCycles': cycles, 'proximityTimerBlock': proximity,
          'birthsBeforeAnimationSetup': births, 'constants': constants, 'callSites': xrefs}
(out / 'inspection.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
print(out)
