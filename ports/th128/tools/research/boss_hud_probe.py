import argparse
import hashlib
import json
import math
from pathlib import Path
import struct
from bullet_probe import NativeBullet, ROOT, datum, emulate, STACK_POINTER


def probe(executable):
    native = NativeBullet(executable)
    try:
        hud, enemy, manager, vertices = 0x380000, 0x300000, 0x3a0000, 0x3b0000
        ignored_index=native.read(0x41c7c0+440-256,1)[0]
        ignored_target=struct.unpack('<I',native.read(0x41c554+ignored_index*4,4))[0]
        if ignored_target!=0x41c52c:raise ValueError('unexpected original440 dispatch')
        fills, timers, rings, fire_gates, markers, clears = [], [], [], [], [], []
        for slot in range(3):
            image=bytearray(0x5a40)
            for offset in (0x5908,0x5918,0x5920,0x5928,0x5930):struct.pack_into('<f',image,offset+slot*0x54,.5)
            for offset in (0x591c,0x5924,0x592c,0x5934):struct.pack_into('<I',image,offset+slot*0x54,0xffff8080)
            result=emulate([native.chunk(0x421381,0x4213bd)],0x421381,[datum('hud',hud,bytes(image))],
                registers={'ecx':slot,'ebp':STACK_POINTER+0x200},stack={0x208:struct.pack('<I',hud)},stops=(0x4213b7,),budget=64)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['hud']['after']);result.update(slot=slot,ratioBits=[struct.unpack_from('<I',after,offset+slot*0x54)[0] for offset in (0x5908,0x5918,0x5920,0x5928,0x5930)],colors=[struct.unpack_from('<I',after,offset+slot*0x54)[0] for offset in (0x591c,0x5924,0x592c,0x5934)]);clears.append(result)
        for angle in (-3.1415927410125732, 0., 1.5707963705062866, -2.1):
            angle=struct.unpack('<f',struct.pack('<f',angle))[0]
            result=emulate([native.chunk(0x4218f1,0x421963)],0x4218f1,[datum('marker',hud,bytes(0x4e8)),native.constant(0x4a6af0,8)],
                registers={'esi':hud},stack={0x40:struct.pack('<2f',math.cos(angle),math.sin(angle)),0x48:struct.pack('<3f',320,116,0)},stops=(0x421963,),budget=128)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['marker']['after']);result.update(angle=angle,positionBits=list(struct.unpack_from('<3I',after,0x464)));markers.append(result)
        for message in (False, True):
            image = bytearray(0x5a40)
            struct.pack_into('<I', image, 0x5a1c, 1 if message else 0)
            source = bytearray(0xc4)
            struct.pack_into('<I', source, 0xc0, 1)
            result = emulate([native.chunk(0x43bf75, 0x43bff9),native.chunk(0x43c013,0x43c0b3)], 0x43bf75,
                [datum('hud', hud, bytes(image), writable=False), datum('manager', manager, bytes(source), writable=False),
                 datum('hudGlobal', 0x4b8950, hud, kind='I', writable=False), datum('managerGlobal', 0x4b8948, manager, kind='I', writable=False)],
                registers={'ebx': 0}, stops=(0x43bf8d, 0x43c013), budget=32)
            if result['stop']['reason'] != 'boundary':
                raise ValueError(result['stop'])
            result.update(message=message, fireSuppressed=result['stop']['address'] == '0x0043c013')
            fire_gates.append(result)
        reset = emulate([native.chunk(0x43c013, 0x43c0b3)], 0x43c013,
            [datum('shotHeldTimers', enemy + 0x438fc, struct.pack('<iifIIiifII', 5, 6, 6., 0x4b6f58, 1, 44, 45, 45., 0x4b6f58, 1)),
             datum('shotSideFields', enemy + 0x8ed4, bytes(8)), native.constant(0x4a6688, 4)],
            registers={'esi': enemy, 'ebx': 0}, stops=(0x43c0b3,), budget=128)
        if reset['stop']['reason'] != 'boundary':
            raise ValueError(reset['stop'])
        for life, maximum, previous in ((9200, 9200, 0.), (6300, 10900, .9), (10900, 10900, .99), (0, 9200, .1), (9200, 9200, .25)):
            image = bytearray(0x5a40)
            struct.pack_into('<f', image, 0x5908, previous)
            source = bytearray(0x5178)
            struct.pack_into('<ii', source, 0x5004, life, maximum)
            result = emulate([native.chunk(0x421497, 0x421518)], 0x421497,
                [datum('hud', hud, bytes(image)), datum('enemy', enemy, bytes(source), writable=False), native.constant(0x4a6af8, 8)],
                registers={'edi': hud, 'esi': enemy, 'ecx': 0, 'ebp': STACK_POINTER + 0x200},
                stack={0x208: struct.pack('<I', hud)}, stops=(0x421518,), budget=128)
            if result['stop']['address'] != '0x00421518':
                raise ValueError(result['stop'])
            after = bytes.fromhex(result['memory']['hud']['after'])
            result.update(input=[life, maximum, previous], fillBits=struct.unpack_from('<I', after, 0x5908)[0], targetBits=struct.unpack_from('<I', after, 0x590c)[0])
            fills.append(result)
        for seconds, previous, present, message, hidden in ((30, -1, True, False, False), (9, 10, True, False, False), (4, 5, True, False, False), (0, 1, True, False, False), (10, 11, True, False, False), (30, 30, True, False, False), (30, -1, False, False, False), (30, -1, True, True, False), (30, -1, True, False, True), (-1, 30, True, False, False)):
            image = bytearray(0x5a40)
            struct.pack_into('<i', image, 0x5a24, seconds)
            struct.pack_into('<i', image, 0x5a2c, previous)
            struct.pack_into('<I', image, 0x5a1c, 1 if message else 0)
            owner = bytearray(0x80)
            struct.pack_into('<I', owner, 0x5c, enemy if present else 0)
            struct.pack_into('<I', owner, 0x7c, 1 if hidden else 0)
            result = emulate([native.chunk(0x421279, 0x421347)], 0x421279,
                [datum('hud', hud, bytes(image)), datum('manager', manager, bytes(owner), writable=False), datum('managerGlobal', 0x4b8948, manager, kind='I', writable=False)],
                registers={'edi': hud, 'esi': hud + 0x4e90, 'ebx': 0}, stops=(0x421347, 0x458410), budget=128)
            sound_boundary = result['stop'].get('target') == '0x00458410'
            if result['stop']['reason'] != 'boundary' and not sound_boundary:
                raise ValueError(result['stop'])
            after = bytes.fromhex(result['memory']['hud']['after'])
            result.update(input=[seconds, previous, present, message, hidden], irq=struct.unpack_from('<H', after, 0x4e90 + 0x3f8)[0], sound=result['registers']['edx'] if sound_boundary else -1)
            timers.append(result)
        for mode, span, angle, width, radius, count in ((14, -6.2831854820251465, -1.5707963705062866, 4., 56., 38), (14, -3.1415927410125732, -1.5707963705062866, 4., 56., 38), (14, 0., -1.5707963705062866, 4., 56., 38), (9, 0., 0., 1., 58., 38), (9, 0., 0., 1., 54., 38)):
            image = bytearray(0x4e8)
            struct.pack_into('<f', image, 0x24, span)
            struct.pack_into('<f', image, 0x2c, angle)
            struct.pack_into('<2f', image, 0x40, width, radius)
            struct.pack_into('<f', image, 0x84, 0.)
            struct.pack_into('<f', image, 0x8c, .25)
            struct.pack_into('<I', image, 0x3f0, 0xffffffff)
            struct.pack_into('<2i', image, 0x430, count, 1)
            struct.pack_into('<3f', image, 0x464, 320., 116., 0.)
            struct.pack_into('<2I', image, 0x4ac, vertices, 6 | (mode << 24))
            start = 0x45cdc0 if mode == 14 else 0x45cb25
            result = emulate([native.chunk(start, 0x45d049), native.chunk(0x45e010, 0x45e030), native.chunk(0x46a660, 0x46a790)], start,
                [datum('vm', hud, bytes(image)), datum('vertices', vertices, bytes(count * 56)), native.constant(0x4a6578, 72)],
                registers={'edi': hud, 'esi': mode, 'ecx': count, 'edx': 6 | (mode << 24)}, stack={0x1c: struct.pack('<I', hud)}, stops=(0x45d049,), budget=12000)
            if result['stop']['address'] != '0x0045d049':
                raise ValueError(result['stop'])
            after = bytes.fromhex(result['memory']['vertices']['after'])
            result.update(input=[mode, span, angle, width, radius, count], vertices=[list(struct.unpack_from('<7I', after, index * 28)) for index in range(count * 2)])
            rings.append(result)
        return {'schema': 'th128-original-boss-hud/1', 'fillCases': fills, 'timerCases': timers, 'ringCases': rings, 'markerCases':markers,'clearCases':clears,'fireGateCases': fire_gates, 'dialogueFireReset': reset,
            'ignoredOpcode440':{'index':ignored_index,'target':ignored_target},'code': native.code, 'constants': native.constants, 'sourceExeSha256': hashlib.sha256(native.executable.read_bytes()).hexdigest(),
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(), 'apiReturnsAssigned': False,
            'scope': 'Native HP fill and countdown IRQ/sound prefixes, original mode9/14 CPU ring vertices; stops before sound and graphics APIs. No game process launched.'}
    finally:
        native.image.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    report = probe(parser.parse_args().exe)
    (ROOT / 'local/research/boss-hud-fixtures.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print('Original HP fill, timer warning and mode9/14 ring evidence recorded')
