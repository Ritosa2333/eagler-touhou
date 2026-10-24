"""Read-only retail reward diagnostics; never touches runtime or goldens."""
import pathlib, sys, struct, json, hashlib
TITLE = pathlib.Path(__file__).resolve().parents[2]
WORKSPACE = TITLE.parent
sys.path.insert(0, str(WORKSPACE / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EBP, UC_X86_REG_EIP, UC_X86_REG_EAX, UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI
EXE = WORKSPACE / 'th12/th12.exe'
SHA = '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
if hashlib.sha256(EXE.read_bytes()).hexdigest() != SHA:
    raise SystemExit('Wrong retail version; refusing fixed addresses')
pe = pefile.PE(str(EXE))

def put(vm, address, fmt, *values): vm.mem_write(address, struct.pack('<' + fmt, *values))
def get(vm, address, fmt): return struct.unpack('<' + fmt, vm.mem_read(address, struct.calcsize(fmt)))
def vm_new(kind, fill, power=100, stage=1):
    vm = Uc(UC_ARCH_X86, UC_MODE_32)
    vm.mem_map(0x400000, (pe.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095)
    vm.mem_write(0x400000, pe.get_memory_mapped_image())
    vm.mem_map(0x1000000, 0x10000)
    vm.mem_map(0x2000000, 0x100000)
    manager, enemy = 0x2000000, 0x2010000
    put(vm, 0x4b4534, 'I', manager)
    put(vm, manager + 0x30, 'iI', kind, enemy)
    put(vm, manager + 0x50, 'f', fill)
    put(vm, enemy + 0x1074, 'fff', 0, 128, 0)
    put(vm, 0x4b0c48, 'i', power)
    put(vm, 0x4b0cd0, 'ii', 400, 100)
    put(vm, 0x4b0c78, 'i', 1000000)
    put(vm, 0x4b0cf4, 'i', 20000000)
    put(vm, 0x4b0cb0, 'i', stage)
    put(vm, 0x4b0c54, 'i', 3)
    return vm, manager

def hooks(vm):
    spawns = []
    def intercept(vm, address, size, unused):
        stack = vm.reg_read(UC_X86_REG_ESP)
        if address == 0x4273f0:
            ret, kind, pos, angle, speed = get(vm, stack, 'IIIff')
            entity = 0x2020000 + len(spawns) * 0x1000
            spawns.append({'type': kind, 'position': get(vm, pos, 'fff'), 'angle': angle, 'speed': speed, 'pointer': entity})
            vm.reg_write(UC_X86_REG_EAX, entity)
            vm.reg_write(UC_X86_REG_ESP, stack + 20)
            vm.reg_write(UC_X86_REG_EIP, ret)
        elif address in (0x453e20, 0x461970, 0x4385b0, 0x43e250):
            ret, = get(vm, stack, 'I')
            vm.reg_write(UC_X86_REG_ESP, stack + (12 if address == 0x43e250 else 8))
            vm.reg_write(UC_X86_REG_EIP, ret)
    vm.hook_add(UC_HOOK_CODE, intercept)
    return spawns

finished = []
for kind in (-1, 1, 2, 3):
    for fill in (0., .25, .499, .5, .999, 1.):
        for power in (100, 400):
            vm, manager = vm_new(kind, fill, power)
            spawns = hooks(vm)
            put(vm, manager + 0x48, 'ii', 3, 5)
            sp, bp, stop = 0x100d000, 0x100d080, 0x100f000
            put(vm, sp + 8, 'I', bp)
            put(vm, bp + 4, 'I', stop)
            vm.reg_write(UC_X86_REG_ESP, sp)
            vm.reg_write(UC_X86_REG_EBP, bp)
            vm.reg_write(UC_X86_REG_ESI, manager)
            vm.reg_write(UC_X86_REG_EDI, 0)
            vm.emu_start(0x44b09c, stop, count=100000)
            for spawn in spawns:
                ptr = spawn.pop('pointer')
                spawn['aggregate'] = get(vm, ptr + 0x9c8, 'iiif') if spawn['type'] in (16, 17, 18) else None
            finished.append({'kind': kind, 'fill': fill, 'power': power, 'spawns': spawns})

filled = []
for stage in (1, 4, 7):
    for kind in (-1, 1, 2, 3):
        for item_type in (1, 2, 10):
            for fill in (0., .99, 1.):
                vm, manager = vm_new(kind, fill, stage=stage)
                spawns = hooks(vm)
                sp, stop = 0x100d000, 0x100f000
                put(vm, sp, 'I', stop)
                vm.reg_write(UC_X86_REG_ESP, sp)
                vm.reg_write(UC_X86_REG_EAX, item_type)
                vm.emu_start(0x44aca0, stop, count=100000)
                for spawn in spawns: spawn.pop('pointer')
                filled.append({'stage': stage, 'kind': kind, 'type': item_type, 'fillBefore': fill,
                               'fillAfter': get(vm, manager + 0x50, 'f')[0], 'buckets': get(vm, manager + 0x48, 'ii'), 'spawns': spawns})

collected = []
for kind in (-1, 1, 2, 3):
    for multiplier in (1., 2., 3., 6., 8.):
        for power in (100, 400):
            vm, manager = vm_new(kind, 0, power)
            hooks(vm)
            item, sp = 0x2020000, 0x100d000
            put(vm, item + 0x9c8, 'iiif', kind, 3, 5, multiplier)
            vm.reg_write(UC_X86_REG_ESP, sp)
            vm.reg_write(UC_X86_REG_EDI, item)
            vm.emu_start(0x426c6f, 0x426da4, count=100000)
            collected.append({'kind': kind, 'multiplier': multiplier, 'powerBefore': power,
                              'powerAfter': get(vm, 0x4b0c48, 'i')[0], 'scoreStoredAfter': get(vm, 0x4b0c44, 'i')[0]})

death_drops=[]
for power in (100,400):
    for x in (0,150):
        vm,manager=vm_new(1,0,power)
        spawns=hooks(vm);player=0x2010000
        put(vm,player+0x97c,'fff',x,400,0)
        vm.reg_write(UC_X86_REG_ESP,0x100d000)
        vm.reg_write(UC_X86_REG_EDI,player)
        vm.reg_write(UC_X86_REG_EBX,1) # Literal set at retail 436dd4 immediately before the state-2 branch.
        vm.emu_start(0x436de6,0x436e8b,count=100000)
        for spawn in spawns:spawn.pop('pointer')
        death_drops.append({'powerBefore':power,'powerAfter':get(vm,0x4b0c48,'i')[0],'position':[x,400], 'spawns':spawns,'rngCalls':get(vm,0x4ce56c,'I')[0]})
output = TITLE / 'artifacts/reverse/ufo-reward-inspection.json'
output.write_text(json.dumps({'schema': 'th12-ufo-rewards-inspection/1', 'originalExeSha256': SHA,
                             'scope': 'native fill and reward selection/collection slices; graphics intercepted',
                             'wholeGameDeterminism': False, 'finished': finished, 'filled': filled, 'collected': collected,'deathDrops':death_drops}, indent=2) + '\n', encoding='utf-8')
print(output)
