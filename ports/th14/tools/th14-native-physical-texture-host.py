"""Control D3D services while running the original TH14 texture upload routines."""
import hashlib
import struct
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP


def install(m, allocate, current, uploads, get, put, ret):
    base = 0x70000000
    create, surface, release, load, target = [base + n for n in (0x1100, 0x1200, 0x1300, 0x1400, 0x1500)]
    desc = base + 0x1000
    device, device_table = base + 0x1800, base + 0x1900
    put(device, device_table)
    put(device_table + 0x5c, target)
    put(0x4d8f68, device)
    put(0x4d905c, 21)
    m.mem_write(0x4f7a6c, struct.pack('<f', 1))
    put(0x4b12e4, create)
    put(0x4b12e0, load)

    def texture(width, height, output, fmt, kind):
        p, table = allocate(16), allocate(0x60)
        m.mem_write(table, bytes(0x60))
        put(p, table)
        put(table + 8, release)
        put(table + 0x44, desc)
        put(table + 0x48, surface)
        current['textures'][p] = (width, height)
        put(output, p)
        uploads.append(dict(chunk=current['index'], width=width, height=height,
                            format=fmt, kind=kind))

    def observer(machine, address, size, user):
        sp = machine.reg_read(UC_X86_REG_ESP)
        if address == create:
            args = [get(sp + 4 + i * 4) for i in range(8)]
            assert args[0] == device and args[3] == 1 and args[4] == 0 and args[6] == 1, args
            texture(args[1], args[2], args[7], args[5], 'embedded' if current['chunk'][32] else 'blank')
            uploads[-1]['creationArguments'] = args[1:7]
            ret(0, 8)
        elif address == target:
            args = [get(sp + 4 + i * 4) for i in range(9)]
            assert args[0] == device and args[3:7] == [1, 1, 21, 0] and args[8] == 0, args
            texture(args[1], args[2], args[7], args[5], 'renderTarget')
            uploads[-1]['creationArguments'] = args[1:7]
            ret(0, 9)
        elif address == surface:
            assert get(sp + 8) == 0
            p, table = allocate(16), allocate(0x20)
            m.mem_write(table, bytes(0x20)); put(p, table); put(table + 8, release)
            put(get(sp + 12), p)
            ret(0, 3)
        elif address == load:
            args = [get(sp + 4 + i * 4) for i in range(10)]
            assert args[1] == args[6] == args[9] == 0, args
            source = list(struct.unpack('<4i', m.mem_read(args[7], 16)))
            destination = list(struct.unpack('<4i', m.mem_read(args[2], 16)))
            assert source[:2] == destination[:2] == [0, 0]
            raw = bytes(m.mem_read(args[3], args[5] * source[3]))
            uploads[-1].update(sourceRect=source, destinationRect=destination,
                               sourceFormat=args[4], pitch=args[5], filter=args[8],
                               sourceBytes=len(raw), sourceSha256=hashlib.sha256(raw).hexdigest())
            ret(0, 10)
        elif address == release:
            ret(0, 1)

    for address in (create, surface, release, load, target):
        m.hook_add(UC_HOOK_CODE, observer, begin=address, end=address)
