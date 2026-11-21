import argparse
import hashlib
import json
import struct
from pathlib import Path

from bullet_probe import NativeBullet, ROOT, STACK_POINTER, address, datum, emulate


MANAGER, STRIDE, CAPACITY = 0x380000, 0x11b8, 2000
POOL = MANAGER + 0x64


def report_destination(path):
    destination = Path(path).resolve()
    allowed = ROOT / 'local/research'
    if (allowed.resolve() != allowed or destination.parent != allowed or destination.suffix != '.json'
            or not destination.name.startswith('bullet-pool-') or destination.drive.lower() == 'e:'):
        raise ValueError('report must be deliverable local/research/bullet-pool-*.json')
    return destination


class NativePool(NativeBullet):
    def anm_registration(self,head):
        if type(head) is not bool:raise ValueError('explicit registration end required')
        old,new=bytearray(0x4e8),bytearray(0x4e8)
        struct.pack_into('<3I',old,4,0x390000,0,0)
        start,end=(0x466de0,0x466e57) if head else (0x466d60,0x466ddc)
        result=emulate([self.chunk(start,end)],start,
            [datum('old',0x390000,bytes(old)),datum('new',0x3a0000,bytes(new)),
             datum('primaryLinks',0x3b0000+0x8b9704,struct.pack('<2I',0x390004,0x390004)),
             datum('idCounter',0x3b0000+0x8c38fc,7,kind='I'),datum('handle',0x370000,0,kind='I'),
             datum('managerPointer',0x4d2e50,0x3b0000,kind='I',writable=False)],
            registers={'ebx':0x3a0000},arguments=struct.pack('<I',0x370000))
        result['head'],result['tail']=struct.unpack('<2I',bytes.fromhex(result['memory']['primaryLinks']['after']))
        return result

    def spawn_life_flags(self,life,flags):
        if type(life) is not int or not -2**31<=life<2**31 or type(flags) is not int or not 0<=flags<2**32:
            raise ValueError('bounded constructor life/flag inputs required')
        request=bytearray(0x54);struct.pack_into('<i',request,0x14,life)
        result=emulate([self.chunk(0x415167,0x415185)],0x415167,
            [datum('request',0x370000,bytes(request),writable=False),datum('flags',0x390000+0x50ac,flags,kind='I')],
            registers={'eax':0x370000,'ebx':0x390000},stops=[0x415180])
        result['flags']=struct.unpack('<I',bytes.fromhex(result['memory']['flags']['after']))[0]
        return result

    def sound_bindings(self):
        import re

        self.chunk(0x457740,0x457768)
        table=self.read(0x4b1268,76*20)
        bindings={}
        for index in range(76):
            identifier,wave,packed,_,_=struct.unpack_from('<5I',table,index*20)
            if identifier not in (0,2,22,40,44,73,74):continue
            if wave>=64:raise ValueError('sound wave index outside bounded original name table')
            pointer=struct.unpack('<I',self.read(0x4b1858+wave*4,4))[0]
            name=self.read(pointer,64).split(b'\0',1)[0].decode('ascii')
            if not re.fullmatch(r'se_[A-Za-z0-9_-]+\.wav',name):raise ValueError('unexpected original sound name')
            volume=struct.unpack('<h',struct.pack('<H',packed&65535))[0]
            bindings[str(identifier)]={'name':name,'volumeRaw':volume,'sourceRow':index,'waveIndex':wave}
        if len(bindings)!=7:raise ValueError('required original sound IDs are missing or duplicated')
        return bindings

    def effect_bind_domain(self):
        vm=bytearray(0x4e8);struct.pack_into('<I',vm,0x4b4,2)
        bank=bytearray(0x128);struct.pack_into('<I',bank,0x11c,0x3c0000)
        scripts=bytearray(176*4);struct.pack_into('<I',scripts,175*4,0x3d0000)
        ranges=((0x4593b0,0x459471),(0x402ea0,0x402ff9),(0x47bb30,0x47bbaa))
        result=emulate([self.chunk(start,end) for start,end in ranges],0x4593b0,
            [datum('vm',0x390000,bytes(vm)),datum('bank',0x3b0000,bytes(bank),writable=False),
             datum('scripts',0x3c0000,bytes(scripts),writable=False),datum('cpuMemset',0x4dcd68,0,writable=False),
             datum('anmManagerPointer',0x4d2e50,0,writable=False),self.constant(0x4a682c,4)],
            registers={'eax':0x390000,'edi':0x3b0000,'ebx':175})
        final=bytes.fromhex(result['memory']['vm']['after'])
        result['flags2']=struct.unpack_from('<I',final,0x4b4)[0]
        result['memory'].pop('vm');return result

    def enemy_unlink(self,selected):
        if selected not in (0,1):raise ValueError('bounded two-node destructor prefix required')
        manager=bytearray(0xcc)
        struct.pack_into('<3I',manager,0xb8,0x39145c,0x3a145c,2)
        first,second=bytearray(0x5178),bytearray(0x5178)
        struct.pack_into('<3I',first,0x145c,0x390000,0x3a145c,0)
        struct.pack_into('<3I',second,0x145c,0x3a0000,0,0x39145c)
        result=emulate([self.chunk(0x415ec0,0x415f50)],0x415ec0,
            [datum('manager',0x380000,bytes(manager)),datum('enemyA',0x390000,bytes(first)),
             datum('enemyB',0x3a0000,bytes(second)),datum('managerPointer',0x4b8948,0x380000,kind='I',writable=False)],
            registers={'eax':0x390000+selected*0x10000},stops=[0x415f2b])
        final=bytes.fromhex(result['memory']['manager']['after'])
        result['head'],result['tail'],result['count']=struct.unpack_from('<3I',final,0xb8)
        result['memory'].pop('enemyA');result['memory'].pop('enemyB')
        return result

    def effect_rng(self,flags):
        if type(flags) is not int or not 0<=flags<2**32:raise ValueError('explicit native effect flags required')
        state=bytearray(0x4e8);struct.pack_into('<I',state,0x4b4,flags)
        ranges=((0x459900,0x459ae4),(0x48fac0,0x48fadc),(0x412e90,0x412eca),(0x46a460,0x46a4f5))
        result=emulate([self.chunk(start,end) for start,end in ranges],0x459900,
            [datum('state',0x390000,bytes(state)),
             datum('displayRng',0x4d2ad8,struct.pack('<HHI',0x1234,0,0)),
             datum('gameRng',0x4d2ae0,struct.pack('<HHI',0x4321,0,0)),
             datum('threadFlags',0x4d33f8,0,writable=False),datum('SSE2',0x4dcd68,1,writable=False),
             self.constant(0x459ae4,104),self.constant(0x4a6850,4),self.constant(0x4a6660,4),
             self.constant(0x4a6788,8),self.constant(0x4a6588,8)],
            registers={'eax':0x390000},arguments=struct.pack('<f',10010))
        result['displayCalls']=struct.unpack_from('<I',bytes.fromhex(result['memory']['displayRng']['after']),4)[0]
        result['gameCalls']=struct.unpack_from('<I',bytes.fromhex(result['memory']['gameRng']['after']),4)[0]
        result['memory'].pop('state',None)
        return result

    def spawn_mode_flags(self,mode,flags):
        if type(mode) is not int or mode not in (0,1) or type(flags) is not int or not 0<=flags<2**32:
            raise ValueError('bounded spawn-mode flag inputs required')
        request=bytearray(0x54);struct.pack_into('<I',request,0x18,mode)
        result=emulate([self.chunk(0x415064,0x41507e)],0x415064,
            [datum('request',0x370000,bytes(request),writable=False),datum('flags',0x390000+0x50ac,flags,kind='I')],
            registers={'eax':0x370000,'ebx':0x390000},stops=[0x41507e])
        result['flags']=struct.unpack('<I',bytes.fromhex(result['memory']['flags']['after']))[0]
        return result

    def anm_binding(self):
        import capstone

        decoder=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
        decoder.detail=True
        slices=((0x415313,0x41532b),(0x4154dd,0x4154e4),(0x41554b,0x41555f),
                (0x418dca,0x418ddd),(0x45c4bf,0x45c4d7))
        instructions={}
        for start,end in slices:
            location,payload=self.chunk(start,end)
            decoded=list(decoder.disasm(payload,location))
            if sum(instruction.size for instruction in decoded)!=len(payload):
                raise ValueError('ANM mapping evidence must end on instruction boundaries')
            instructions.update((instruction.address,instruction) for instruction in decoded)
        reserved=instructions[0x415325].operands[0].mem.disp
        base=instructions[0x4154dd].operands[1].imm
        first=base+instructions[0x415554].operands[0].mem.disp
        lookup=instructions[0x418dd6].operands[1].mem
        return {'staticOnly':True,'reservedBulletOffset':reserved,'firstIncludeOffset':first,
                'firstIncludeBank':(first-reserved)//lookup.scale,'lookupStride':lookup.scale,
                'layerHandlerOnlyWritesOffset':instructions[0x45c4c3].operands[0].mem.disp}

    def draw_position(self, position):
        from bullet_lifecycle_probe import vector

        bullet = bytearray(0x11b8)
        bullet[0x9d8:0x9e4] = vector(position, 3)
        result = emulate([self.chunk(0x408f52, 0x408f94)], 0x408f52,
                         [datum('bullet', 0x390000, bytes(bullet)), self.constant(0x4a65f8, 8),
                          self.constant(0x4a65d8, 8), self.constant(0x4a6610, 8)],
                         registers={'esi': 0x390000}, stops=[0x408f88])
        result.update(completedRendering=False, position=None)
        if result['stop']['reason'] == 'boundary':
            final = bytes.fromhex(result['memory']['bullet']['after'])
            result['position'] = list(struct.unpack_from('<3f', final, 0x460))
        result['memory'].pop('bullet', None)
        return result

    def position(self, combined, offset, override, exclusion):
        if any(type(vector) not in (tuple, list) or len(vector) != 3 for vector in (combined, offset, override)):
            raise ValueError('explicit three-component 501 position inputs required')
        context = bytearray(0x4000)
        struct.pack_into('<3f', context, 0x34, *combined)
        struct.pack_into('<3f', context, 0x3ca0, *offset)
        struct.pack_into('<3f', context, 0x3d60, *override)
        struct.pack_into('<f', context, 0x3ef8, exclusion)
        result, _, _ = self.handler(501, struct.pack('<i', 0), context=context)
        return result

    def select(self, cursor, states):
        if (type(cursor) is not int or not 0 <= cursor < CAPACITY or not isinstance(states, dict)
                or not 1 <= len(states) <= 8 or any(type(index) is not int or not 0 <= index < CAPACITY
                    or type(state) is not int or not 0 <= state <= 5 for index, state in states.items())):
            raise ValueError('sparse bounded pool fixture required')
        manager = bytearray(0x64)
        struct.pack_into('<I', manager, 0x10, POOL + cursor * STRIDE)
        data = [datum('manager', MANAGER, bytes(manager), writable=False)]
        for index, state in states.items():
            data.append(datum(f'slot-{index}', POOL + index * STRIDE + 0xa2a,
                              struct.pack('<H', state), writable=False))
        data.append(datum('sentinel', POOL + CAPACITY * STRIDE + 0xa2a,
                          struct.pack('<H', 6), writable=False))
        result = emulate([self.chunk(0x4091f0, 0x4092e0)], 0x4091f0, data,
                         arguments=struct.pack('<5I', MANAGER, 0x370000, 0, 0, 0), stops=[0x4092dd])
        result.update(completedEmission=False, input={'cursor': cursor, 'states': states},
                      selected=(result['registers']['ebx'] - POOL) // STRIDE
                      if result['stop']['reason'] == 'boundary' else None)
        return result


def prepare_profile(executable):
    native = NativePool(executable)
    payload = native.read(0x4b2dd0, 288)
    directory = ROOT / 'local/runtime'
    if directory.resolve() != directory or directory.drive.lower() == 'e:':
        raise ValueError('runtime profile directory must stay in the deliverable')
    if any((directory / name).is_symlink() for name in ('bullet-profile-8.bin', 'bullet-profile-8.json')):
        raise ValueError('runtime profile output must not be redirected')
    directory.mkdir(parents=True, exist_ok=True)
    report = {'schema': 'th128-original-bullet-profile/1', 'type': 8, 'bytes': len(payload),
              'sourceAddress': '0x004b2dd0', 'sha256': hashlib.sha256(payload).hexdigest(),
              'exeSha256': hashlib.sha256(native.binary).hexdigest(), 'rawOriginalTable': True}
    (directory / 'bullet-profile-8.bin').write_bytes(payload)
    (directory / 'bullet-profile-8.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    from prepare_sound_bindings import prepare
    prepare(executable)
    native.image.close()
    return report


def probe(executable):
    native = NativePool(executable)
    cases = []
    for identifier, cursor, states in (('free', 0, {0: 0}), ('skip-one', 0, {0: 1, 1: 0}),
            ('skip-two', 0, {0: 2, 1: 3, 2: 0}), ('wrap', 1999, {1999: 1, 0: 0}),
            ('wrap-skip', 1999, {1999: 2, 0: 1, 1: 0})):
        result = native.select(cursor, states)
        if result['stop']['reason'] != 'boundary':
            raise ValueError(f'pool fixture did not reach its explicit boundary: {identifier}')
        result['id'] = identifier
        cases.append(result)
    draw = native.draw_position((-128, 4, .125))
    if draw['stop']['reason'] != 'boundary':
        raise ValueError('draw-position prefix did not reach its declared boundary')
    after = hashlib.sha256(native.executable.read_bytes()).hexdigest()
    binding=native.anm_binding()
    modes=[native.spawn_mode_flags(mode,0x20) for mode in (0,1)]
    effects=[native.effect_rng(flags) for flags in (0,2)]
    effect_bind=native.effect_bind_domain()
    unlink=[native.enemy_unlink(selected) for selected in (0,1)]
    report = {'schema': 'th128-original-byte-bullet-pool/1', 'nativeCpuTrace': False,
              'windowsGameProcessLaunched': False, 'code': native.code, 'caseCount': len(cases), 'cases': cases,
              'executable': {'sha256': hashlib.sha256(native.binary).hexdigest(), 'sha256After': after},
              'drawPositionPrefix': draw,
              'anmBindingStatic':binding,
              'spawnModePrefixes':modes,'rngSelectorCases':effects,'effectBindPrefix':effect_bind,
              'enemyDestructorPrefixes':unlink,
              'limitations': ['Sparse original-constructor prefixes, not completed emission.',
                              'No allocator, animation, ETEX, collisions or native manager frame is executed.',
                              'Only explicitly declared sparse states may be read; an undeclared scan stops.']}
    native.image.close()
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description='Original-byte bounded TH128 bullet pool selection')
    parser.add_argument('--exe', required=True)
    parser.add_argument('--output', default=str(ROOT / 'local/research/bullet-pool-fixtures.json'))
    parser.add_argument('--prepare-profile', action='store_true')
    arguments = parser.parse_args(argv)
    destination = report_destination(arguments.output)
    report = probe(arguments.exe)
    if arguments.prepare_profile:
        prepare_profile(arguments.exe)
    if report['executable']['sha256'] != report['executable']['sha256After']:
        raise ValueError('read-only original digest changed')
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(f'{report["caseCount"]} original-byte pool prefix cases')


if __name__ == '__main__':
    main()
