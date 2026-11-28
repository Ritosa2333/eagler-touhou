import argparse
from collections import Counter
import hashlib
import json
import math
from pathlib import Path
import struct

from bullet_probe import NativeBullet, ROOT, EXE_SHA256, address, datum
from freeze_probe import emulate, STACK_POINTER


PLAYER, SHOT, RECORD = 0x300000, 0x380000, 0x390000
POSITION, EDGES, OUT_FLAG = 0x3a0000, 0x3b0000, 0x3c0000
ENEMY, CONTROL, MANAGER, SPELL = 0x350000, 0x360000, 0x370000, 0x3d0000
SOURCE_FLAGS = PLAYER + 0x8edc + 0x74
MAX_INSTRUCTIONS, MAX_CASES = 40000, 192
SHT_SHA256 = 'aed90a45b565510e97cc7d9ab4846d8c93914e57faafe9339e9ab03d47b1511b'
SPANS = ((0x43fbf0, 0x43ffaf), (0x43eb20, 0x43ed53), (0x4165a0, 0x416654),
         (0x4147d0, 0x41480b), (0x414e50, 0x414e69), (0x4166e0, 0x416735))


def report_destination(path):
    destination = Path(path).resolve()
    allowed = ROOT / 'local/research'
    if (allowed.resolve() != allowed or destination.parent != allowed or destination.suffix != '.json'
            or not destination.name.startswith('shot-enemy-collision') or destination.drive.lower() == 'e:'):
        raise ValueError('report must be owned local/research/shot-enemy-collision*.json')
    return destination


def bit_payload(value):
    if type(value) is not int or not 0 <= value < 2**32:
        raise ValueError('exact uint32 float bits required')
    return struct.pack('<I', value)


def bits(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]


def words(payload):
    return list(struct.unpack('<' + 'I' * (len(payload) // 4), payload))


def signed(value):
    return struct.unpack('<i', bit_payload(value & 0xffffffff))[0]


def actual_record():
    local = (ROOT / 'local').resolve()
    catalog = json.loads((local / 'catalog.json').read_bytes())
    manifest = (local / catalog['variants']['ja']['manifest']).resolve()
    source = (manifest.parent / 'raw/pl00a.sht').resolve()
    if not manifest.is_relative_to(local) or not source.is_relative_to(local):
        raise ValueError('actual Japanese shot resource must stay in local generation')
    payload = source.read_bytes()
    if hashlib.sha256(payload).hexdigest() != SHT_SHA256:
        raise ValueError('unexpected actual shot resource')
    record_offset = struct.unpack_from('<I', payload, 0x268)[0]
    return {'sha256': SHT_SHA256, 'group': 0, 'recordOffset': record_offset,
            'record': payload[record_offset:record_offset + 52].hex()}


def execute(chunks, entry, data, **options):
    if (len(chunks) > 16 or sum(len(code) for _, code in chunks) > 8192 or len(data) > 32
            or sum(len(region['bytes']) for region in data) > 131072):
        raise ValueError('collision explicit code/data budget exceeded')
    expanded = []
    sparse_names = []
    for region in data:
        if 'stride' not in region:
            expanded.append(region)
            continue
        if (region['name'] != 'damageSourceFlags' or region['address'] != SOURCE_FLAGS
                or region['stride'] != 0x78 or len(region['bytes']) != 8000 or region['writable']):
            raise ValueError('only the exact explicit 2000-word readonly flag snapshot may be sparse')
        for index in range(2000):
            name = f'damageSourceFlags[{index}]'
            sparse_names.append(name)
            expanded.append(datum(name, SOURCE_FLAGS + index * 0x78,
                                  region['bytes'][4 * index:4 * index + 4], writable=False))
    if len(expanded) > 2032:
        raise ValueError('sparse flag region budget exceeded')
    result = emulate(chunks, entry, expanded, budget=MAX_INSTRUCTIONS, **options)
    if result['stop']['reason'] not in ('returned', 'boundary', 'unsupported_call'):
        raise ValueError(f'original-byte collision boundary failed: {result["stop"]}')
    if result['instructionsExecuted'] >= MAX_INSTRUCTIONS:
        raise ValueError('collision instruction budget exhausted')
    for name in sparse_names:
        sample = result['memory'].pop(name)
        if sample['before'] != sample['after']:
            raise ValueError('readonly sparse flag snapshot changed')
    result['declaredData'] = [
        {'name': region['name'], 'address': address(region['address']), 'bytes': len(region['bytes']),
         'stride': region.get('stride', 1), 'writable': region['writable'],
         'beforeSha256': hashlib.sha256(region['bytes']).hexdigest()} for region in data]
    result['explicitStateBytes'] = sum(len(region['bytes']) for region in data)
    return result


def compact(result):
    trace = result.pop('instructionTrace')
    result['traceSha256'] = hashlib.sha256('\n'.join(trace).encode()).hexdigest()
    result['instructionVisits'] = dict(Counter(trace))
    result.pop('memory')
    return result


def slot_image(position_bits, size_bits, *, active=1, damage=13, handle=0x12345678):
    if (len(position_bits) != 3 or len(size_bits) != 2
            or any(type(value) is not int or not 0 <= value < 2**32 for value in (*position_bits, *size_bits))
            or type(active) is not int or not 0 <= active < 2**32
            or type(damage) is not int or not -2**31 <= damage < 2**31):
        raise ValueError('exact bounded slot fields required')
    payload = bytearray(0x78)
    struct.pack_into('<3I', payload, 0x14, *position_bits)
    struct.pack_into('<3f', payload, 0x20, 0, -24, 0)
    struct.pack_into('<I', payload, 0x48, active)
    struct.pack_into('<I', payload, 0x4c, handle)
    struct.pack_into('<I', payload, 0x58, 0x31415926)
    struct.pack_into('<i', payload, 0x60, damage)
    struct.pack_into('<2I', payload, 0x68, *size_bits)
    struct.pack_into('<I', payload, 0x74, RECORD)
    return bytes(payload)


class NativeShotEnemyCollision(NativeBullet):
    def __init__(self, executable):
        super().__init__(executable)
        self.chunks = [self.chunk(start, end) for start, end in SPANS]
        self.half = self.constant(0x4a65a8, 8)
        self.resource = actual_record()
        self.raw_record = bytes.fromhex(self.resource['record'])
        self.edge_cache = {}

    def record(self, kind=0, callback=0):
        if (type(kind) is not int or not 0 <= kind <= 255
                or type(callback) is not int or not 0 <= callback < 2**32):
            raise ValueError('exact runtime kind and callback identity required')
        record = bytearray(self.raw_record)
        record[0x1d] = kind
        struct.pack_into('<I', record, 0x30, callback)
        return bytes(record)

    def enemy_edges(self, position_bits, size_bits):
        key = tuple(position_bits) + tuple(size_bits)
        if key not in self.edge_cache:
            result = self.query(position_bits=position_bits, size_bits=size_bits, prefix=True)
            head = bytes.fromhex(result['stackHead'])
            self.edge_cache[key] = words(head[0x2c:0x34] + head[0x20:0x28])
        return self.edge_cache[key]

    def contact(self, *, position_bits=None, size_bits=None, shot_position_bits=None,
                shot_size_bits=None, active=1, damage=13, kind=0, callback=0):
        position_bits = position_bits or [bits(0), bits(100), bits(0)]
        size_bits = size_bits or [bits(32), bits(32)]
        shot_position_bits = shot_position_bits or [bits(0), bits(100), bits(0)]
        shot_size_bits = shot_size_bits or [bits(18), bits(48)]
        edges = self.enemy_edges(position_bits, size_bits)
        image = slot_image(shot_position_bits, shot_size_bits, active=active, damage=damage)
        data = [datum('slot', SHOT, image), datum('record', RECORD, self.record(kind, callback), writable=False),
                datum('position', POSITION, b''.join(map(bit_payload, position_bits)), writable=False),
                datum('edges', EDGES, b''.join(map(bit_payload, edges)), writable=False), self.half,
                datum('playerPointer', 0x4b8a80, PLAYER, kind='I', writable=False)]
        result = execute(self.chunks, 0x43eb20, data, registers={'edi': SHOT, 'eax': EDGES, 'edx': EDGES + 8},
                         arguments=struct.pack('<I', POSITION))
        after = result['memory']['slot']['after']
        stop = result['stop']['address']
        prefix_damage = (struct.unpack_from('<i', bytes.fromhex(result['stackHead']), 40)[0]
                         if stop == '0x0043ec8a' else 0)
        numeric = [struct.unpack('<f', bit_payload(value))[0]
                   for value in (*position_bits[:2], *size_bits, *shot_position_bits[:2], *shot_size_bits, *edges)]
        result.update(positionBits=position_bits, sizeBits=size_bits, shotPositionBits=shot_position_bits,
                      shotSizeBits=shot_size_bits, enemyEdgesBits=edges, active=active, damage=damage,
                      kind=kind, callback=callback, slotBefore=image.hex(), slotAfter=after,
                      damagePrefix=prefix_damage, supportedGeometry=all(map(math.isfinite, numeric)),
                      callbackSource='explicit runtime binding, not SHT file IDs')
        return compact(result)

    def query(self, *, position_bits=None, size_bits=None, prefix=False, same_time=False,
              out_flag=0xa5a55a5a, null_out=False, slot=None, shot_changes=None, active_source=None,
              inactive_source_flags=0):
        position_bits = position_bits or [bits(0), bits(100), bits(0)]
        size_bits = size_bits or [bits(32), bits(32)]
        data = [datum('playerTimes', PLAYER + 0xf80, struct.pack('<2I', 7, 7 if same_time else 8), writable=False),
                datum('playerPointer', 0x4b8a80, PLAYER, kind='I', writable=False),
                datum('position', POSITION, b''.join(map(bit_payload, position_bits)), writable=False),
                datum('size', EDGES, b''.join(map(bit_payload, size_bits)), writable=False),
                datum('outFlag', OUT_FLAG, out_flag, kind='I'), self.half]
        if not prefix and not same_time:
            pool = bytearray(256 * 0x78)
            if slot is not None:
                if type(slot) is not int or not 0 <= slot < 256:
                    raise ValueError('exact pool slot required')
                changes = dict(shot_changes or {})
                changes.setdefault('position_bits', [bits(1000), bits(100), bits(0)])
                changes.setdefault('size_bits', [bits(18), bits(48)])
                pool[slot * 0x78:(slot + 1) * 0x78] = slot_image(**changes)
            source_flags = [inactive_source_flags] * 2000
            if active_source is not None:
                source_flags[active_source] = 1
            data.extend([datum('shots', PLAYER + 0xfa8, bytes(pool)),
                         datum('record', RECORD, self.record(), writable=False),
                         dict(datum('damageSourceFlags', SOURCE_FLAGS,
                                    struct.pack('<2000I', *source_flags), writable=False), stride=0x78)])
        result = execute(self.chunks, 0x43fbf0, data,
                         arguments=struct.pack('<3I', POSITION, EDGES, 0 if null_out else OUT_FLAG),
                         stops=[0x43fc71] if prefix else [0x43fd53])
        result.update(positionBits=position_bits, sizeBits=size_bits, sameTime=same_time,
                      nullOut=null_out, outFlagBefore=out_flag,
                      outFlagAfter=words(bytes.fromhex(result['memory']['outFlag']['after']))[0],
                      damage=signed(result['registers']['eax']), slot=slot, activeSource=active_source,
                      inactiveSourceFlags=inactive_source_flags,
                      slotBefore=(None if slot is None else bytes(pool[slot * 0x78:(slot + 1) * 0x78]).hex()),
                      slotAfter=(None if slot is None else result['memory']['shots']['after'][slot * 0xf0:(slot + 1) * 0xf0]))
        return result if prefix else compact(result)

    def accounting(self, *, damage=13, player_state=1, out_flag=0, bypass=0, enemy_flags=0,
                   protection=0, life_words=None, stats=(0, 0), spell_flags=0):
        life_words = list(life_words if life_words is not None else (100, 0, 0, 0, 0, 0))
        if (type(damage) is not int or not -2**31 <= damage < 2**31 or len(life_words) != 6
                or len(stats) != 2):
            raise ValueError('exact damage/life/stats snapshot required')
        life_words = [value & 0xffffffff for value in life_words]
        stats = [value & 0xffffffff for value in stats]
        data = [datum('playerPointer', 0x4b8a80, PLAYER, kind='I', writable=False),
                datum('playerState', PLAYER + 0xf78, player_state),
                datum('controlPointer', 0x4b8950, CONTROL, kind='I', writable=False),
                datum('controlBypass', CONTROL + 0x5a1c, bypass),
                datum('managerPointer', 0x4b8948, MANAGER, kind='I', writable=False),
                datum('stats', MANAGER + 0x24, struct.pack('<2I', *stats)),
                datum('life', ENEMY + 0x3e3c, struct.pack('<6I', *life_words)),
                datum('enemyFlags', ENEMY + 0x3ee4, enemy_flags, kind='I', writable=False),
                datum('protection', ENEMY + 0x3ea4, protection, writable=False),
                datum('spellPointer', 0x4b8934, SPELL, kind='I', writable=False),
                datum('spellFlags', SPELL + 0x7c, spell_flags, kind='I', writable=False)]
        result = execute(self.chunks, 0x4165a0, data, registers={'eax': damage, 'ebx': ENEMY},
                         stack={0x18: bit_payload(out_flag)}, stops=[0x416654, 0x41673f])
        scaled = signed(result['registers']['edi'])
        result.update(damage=damage, playerState=player_state, outFlag=out_flag, bypass=bypass,
                      enemyFlags=enemy_flags, protection=protection, lifeBefore=life_words, statsBefore=stats,
                      spellFlags=spell_flags, lifeAfter=words(bytes.fromhex(result['memory']['life']['after'])),
                      statsAfter=words(bytes.fromhex(result['memory']['stats']['after'])),
                      hpDamage=scaled,
                      playerScaledDamage=(math.trunc(damage / 5) if player_state in (0, 2) else damage))
        return compact(result)

    def hit_tail(self, *, speed=24, position=(0, 100, .1), rotation=-1.5707963705062866, handle=0x1234):
        image = bytearray(slot_image([bits(value) for value in position], [bits(18), bits(48)]))
        struct.pack_into('<f', image, 0x2c, speed)
        struct.pack_into('<I', image, 0x48, 1)
        struct.pack_into('<I', image, 0x4c, handle)
        struct.pack_into('<I', image, 0x58, 1)
        animation = 0x710000
        result = execute([self.chunk(0x43ed12, 0x43ed53), self.chunk(0x426f10, 0x426f33)], 0x43ed12,
                         [datum('slot', SHOT, bytes(image)), datum('animation', animation, bytes(0x4c0)),
                          self.constant(0x4a6aa0, 4), self.constant(0x4a67a8, 8), self.constant(0x4a65f8, 8),
                          self.constant(0x4a65d8, 8), self.constant(0x4a6610, 8)],
                         registers={'eax': animation, 'edi': SHOT, 'ebp': STACK_POINTER + 0x100},
                         stack={0xf8: struct.pack('<f', rotation), 0xf4: struct.pack('<I', SHOT + 0x14),
                                0xfc: struct.pack('<i', 13)}, stops=[0x43ed48])
        after = bytes.fromhex(result['memory']['animation']['after'])
        result.update(slotBefore=image.hex(), slotAfter=result['memory']['slot']['after'],
                      rotationBits=bits(rotation), replacementHandle=handle,
                      animationPositionBits=words(after[0x464:0x470]),
                      animationRotationBits=struct.unpack_from('<I', after, 0x2c)[0],
                      animationFlags=struct.unpack_from('<I', after, 0x4b0)[0])
        return compact(result)

    def impact(self, value, flag=0):
        result = execute([self.chunk(0x427260, 0x4272f8)], 0x427260,
                         [datum('motivation', 0x4b4d64, value), datum('flag', 0x4b4d68, flag)],
                         arguments=struct.pack('<i', 2))
        result.update(value=value, flag=flag,
                      valueAfter=struct.unpack('<i', bytes.fromhex(result['memory']['motivation']['after']))[0],
                      flagAfter=struct.unpack('<i', bytes.fromhex(result['memory']['flag']['after']))[0])
        return compact(result)

    def enemy_query_gate(self, flags):
        result = execute([self.chunk(0x416500, 0x416530)], 0x416500,
                         [datum('flags', ENEMY + 0x3ee4, flags, kind='I'),
                          datum('callback', ENEMY + 0x3fa8, 0, kind='I', writable=False)],
                         registers={'ebx': ENEMY}, stops=[0x41658a, 0x41673f])
        result.update(flags=flags, flagsAfter=struct.unpack('<I', bytes.fromhex(result['memory']['flags']['after']))[0],
                      queryRequired=result['stop']['address'] == '0x0041658a')
        return compact(result)

    def ordinary_birth_life(self, life):
        data = [datum('request', RECORD + 0x14, life), datum('life', ENEMY + 0x5004, bytes(8)),
                datum('protection', ENEMY + 0x506c, bytes(4)), self.constant(0x4a682c, 4)]
        result = execute([self.chunk(0x415049, 0x41505b), self.chunk(0x4150d8, 0x4150ee)], 0x415049,
                         data, registers={'eax': RECORD, 'ebx': ENEMY}, stops=[0x41505b])
        result.update(life=life, lifeWords=words(bytes.fromhex(result['memory']['life']['after'])))
        return compact(result)

    def ordinary_post_damage(self, life):
        block = bytearray(0x150)
        struct.pack_into('<i', block, 0, life)
        struct.pack_into('<I', block, 8, 0x12345678)
        for index in range(8):
            struct.pack_into('<4i', block, 0xc0 + index * 16, -1, -1, 0, 0)
        result = execute([self.chunk(0x41dea0, 0x41df8b)], 0x41dea0,
                         [datum('enemyLifeAndThresholds', ENEMY + 0x5004, bytes(block))], registers={'ecx': ENEMY})
        result.update(life=life, lifeAfter=words(bytes.fromhex(result['memory']['enemyLifeAndThresholds']['after'])[:24]))
        return compact(result)

    def death_prefix(self, *, life=0, enemy_flags=0):
        data = [datum('life', ENEMY + 0x3e3c, life, writable=False),
                datum('flags', ENEMY + 0x3ee4, enemy_flags, kind='I', writable=False),
                datum('taskIdentity', ENEMY + 0x3f7c, 0x700000, kind='I', writable=False)]
        result = execute(self.chunks, 0x4166e0, data, registers={'ebx': ENEMY},
                         stack={0x18: bit_payload(0)}, stops=[0x416735])
        result.update(life=life, enemyFlags=enemy_flags)
        return compact(result)


def probe(executable):
    native = NativeShotEnemyCollision(executable)
    try:
        contacts = [native.contact(), native.contact(callback=0x720000),
                    native.contact(damage=0), native.contact(damage=-1), native.contact(damage=2**31 - 1)]
        for horizontal in (24.999998092651367, 25, 25.000001907348633,
                           -24.999998092651367, -25, -25.000001907348633):
            contacts.append(native.contact(shot_position_bits=[bits(horizontal), bits(100), bits(0)]))
        for vertical in (139.99998474121094, 140, 140.00001525878906,
                         60.000003814697266, 60, 59.999996185302734):
            contacts.append(native.contact(shot_position_bits=[bits(0), bits(vertical), bits(0)]))
        for maximum in (-2**-149, -0.0, 0.0, 2**-149):
            contacts.append(native.contact(position_bits=[bits(0), bits(maximum), 0],
                                           size_bits=[bits(32), bits(0)],
                                           shot_position_bits=[bits(0), 0, 0]))
        patterns = (0, 0x80000000, 1, 0x80000001, 0x007fffff, 0x807fffff,
                    0x00800000, 0x80800000, 0x3f7fffff, 0x3f800000, 0x3f800001,
                    0x7f7fffff, 0xff7fffff, 0x7f800000, 0xff800000, 0x7fc00001, 0x7f800001)
        for pattern in patterns:
            for field in ('position_bits', 'size_bits', 'shot_position_bits', 'shot_size_bits'):
                default = {'position_bits': [0, bits(100), 0], 'size_bits': [bits(32), bits(32)],
                           'shot_position_bits': [0, bits(100), 0], 'shot_size_bits': [bits(18), bits(48)]}[field]
                default[0] = pattern
                contacts.append(native.contact(**{field: default}))
        contacts += [native.contact(kind=kind) for kind in (1, 2, 3, 4, 255)]
        misses = [native.query(same_time=True), native.query(same_time=True, null_out=True),
                  native.query(), native.query(null_out=True), native.query(slot=0), native.query(slot=255),
                  native.query(inactive_source_flags=2)]
        misses += [native.query(slot=5, shot_changes={'active': active}) for active in (0, 2, 0xffffffff)]
        source_prefixes = [native.query(active_source=0), native.query(active_source=1999)]
        query_prefixes = [native.query(slot=slot, shot_changes={'position_bits': [0, bits(100), 0]})
                          for slot in (0, 255)]
        accounting = [native.accounting(damage=value) for value in
                      (0, 1, 4, 5, 13, 160, 2000, -1, -5, -2147483648, 2147483647)]
        accounting += [native.accounting(damage=value, player_state=state) for state in (0, 2)
                       for value in (4, 5, 13, -13, -2147483648, 2147483647)]
        accounting += [native.accounting(out_flag=flag, life_words=(life, 0, 0, 0, 0, 0),
                                        stats=(0xfffffff8, 0x7fffffff))
                       for flag in (0, 2, 0x80000000) for life in (100, 13, 1, -2147483648)]
        accounting += [native.accounting(enemy_flags=0x10), native.accounting(protection=1),
                       native.accounting(protection=-1), native.accounting(bypass=1),
                       native.accounting(bypass=-1), native.accounting(spell_flags=0x41),
                       native.accounting(damage=-2147483648, spell_flags=0x41)]
        accounting += [native.accounting(damage=damage, life_words=(100, 17, 29, accumulated, base, 1))
                       for damage, accumulated, base in ((13, 700, 100), (-13, 700, 100),
                                                        (2147483647, 0x80000000, 0x7fffffff),
                                                        (-2147483648, 0x7fffffff, 0x80000000))]
        deaths = [native.death_prefix(life=life, enemy_flags=flags)
                  for life, flags in ((0, 0), (-1, 0), (0, 0x80), (1, 0))]
        hit_tails = [native.hit_tail(speed=speed, position=position) for speed, position in
                     ((24, (0, 100, .1)), (18, (-128.25, 401.5, 0)), (0, (192, 448, -0.0)),
                      (-8, (-192, 0, 3)), (3, (.125, -.125, .1)))]
        impact = [native.impact(value, flag) for value in (0, 9997, 9998, 9999, 10000, 99999, 100000)
                  for flag in (0, 1)]
        gates = [native.enemy_query_gate(flags) for flags in (0, 1, 0x20, 0x21, 0x400000, 0x400020)]
        birth_life = [native.ordinary_birth_life(life) for life in (20, 1000, -1)]
        post_damage = [native.ordinary_post_damage(life) for life in (20, 7, 0, -6)]
        count = sum(map(len, (contacts, misses, source_prefixes, query_prefixes, accounting, deaths, hit_tails, impact, gates, birth_life, post_damage)))
        if count > MAX_CASES:
            raise ValueError(f'collision fixture case budget exceeded: {count}')
        native.chunk(0x41658a, 0x4165a0)
        native.chunk(0x43ec87, 0x43ed53)
        digest = hashlib.sha256(native.executable.read_bytes()).hexdigest()
        return {'schema': 'th128-original-byte-shot-enemy-collision-fixtures/1',
                'nativeCpuTrace': False, 'windowsGameProcessLaunched': False,
                'executable': {'sha256': EXE_SHA256, 'sha256After': digest, 'unchanged': digest == EXE_SHA256},
                'resource': native.resource,
                'abi': {'query': address(0x43fbf0), 'caller': address(0x41659b),
                        'stackArguments': ['combinedPositionXYZ*', 'enemyState+d0/fullSizeXY*', 'uint32_t*outFlag/null'],
                        'outFlagBytes': 4, 'returnRegister': 'eax/signed-int32', 'calleePopBytes': 12,
                        'equalTimePreservesOutFlag': True},
                'layout': {'poolOffset': 0xfa8, 'slotBytes': 0x78, 'slots': 256,
                           'damageSourceOffset': 0x8edc, 'damageSourceFlagsOffset': 0x74, 'damageSources': 2000},
                'contactCases': contacts, 'completeMissReturns': misses, 'sourcePrefixes': source_prefixes,
                'queryPrefixes': query_prefixes, 'accountingCases': accounting, 'deathPrefixes': deaths,
                'hitTailCases': hit_tails, 'impactCases': impact, 'enemyGateCases': gates,
                'birthLifeCases': birth_life, 'ordinaryPostDamageCases': post_damage, 'caseCount': count,
                'code': native.code, 'constants': native.constants,
                'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                'engineSha256': {name: hashlib.sha256((Path(__file__).parent / name).read_bytes()).hexdigest()
                                 for name in ('freeze_probe.py', 'bullet_probe.py')},
                'limits': {'instructions': MAX_INSTRUCTIONS, 'cases': MAX_CASES, 'codeBytes': 8192,
                           'dataBytes': 131072, 'codeSpans': 16, 'logicalDataSpans': 32,
                           'sparseFlags': 2000, 'sparseFlagBytes': 8000},
                'limitations': ['Original byte emulation, never a Windows process or native CPU trace.',
                                'Sparse pool declares every readonly flag word explicitly; no implicit zero reads.',
                                'No allocator, Windows API, animation or callback return stub is installed.',
                                'Non-finite geometry and nonzero kinds are observed but outside the production subset.',
                                'Contact damage is a local prefix, not completed query damage.',
                                'HP cases enter after an explicitly supplied completed-query boundary.',
                                'Death stops at unknown 0x417370; no actor release or item call is invented.']}
    finally:
        native.image.close()


def main(argv=None):
    parser = argparse.ArgumentParser(description='Readonly original-byte shot/enemy collision and HP boundaries')
    parser.add_argument('--exe', required=True)
    parser.add_argument('--output', default=str(ROOT / 'local/research/shot-enemy-collision-fixtures.json'))
    arguments = parser.parse_args(argv)
    destination = report_destination(arguments.output)
    report = probe(arguments.exe)
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(json.dumps(report, indent=2, allow_nan=False) + '\n', encoding='utf-8')
    print(f'{report["caseCount"]} original-byte cases -> {destination}')


if __name__ == '__main__':
    main()
