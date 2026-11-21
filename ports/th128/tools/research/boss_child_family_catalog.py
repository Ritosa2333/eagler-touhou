import hashlib
import json
import math
import re
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CATALOG = ROOT / 'local' / 'catalog.json'
BULLET_META = ROOT / 'local' / 'catalog.json'
OUTPUT = ROOT / 'local' / 'research' / 'boss-child-family-catalog.json'
SPAWN_OPS = {88, 90, 91, 92, 95, 96, 97}
WHITELIST = {
    182: [(176, 181)],
    185: [(184, 184)],
    189: [(176, 181), (186, 188)],
    209: [(206, 208)],
    133: [(132, 132), (121, 121)],
    160: [(159, 159)],
    161: [(159, 159)],
    123: [(121, 122)],
    127: [(126, 126), (121, 121)],
    197: [(194, 196)],
}
WHITELIST_SOURCE = 'src/game/EnemyVisuals.cpp:76-86'
IN_PROGRESS_PARENT = 197
FOCUSED_197_TEST = 'tests/boss-child-197.test.mjs'
FOCUSED_197_NATIVE = 'tests/native/boss-child-197.cpp'
REQUIRED_RANGE_OPS = {9, 23}
CLONE_PARENT = 197
CLONE_PARENT_OPS = {1, 4, 51, 64, 88, 111}
CLONE_CHILD_OPS = [
    {1, 3, 9, 23, 48, 50, 52, 58, 60, 65, 66, 68, 111},
    {1, 3, 9, 23, 49, 50, 51, 52, 58, 60, 66, 67, 68, 111},
    {1, 3, 9, 23, 49, 50, 51, 52, 58, 60, 66, 67, 68, 111},
]


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def words(command):
    raw = bytes.fromhex(command.get('raw', ''))
    return [int.from_bytes(raw[index:index + 4], 'little') for index in range(0, len(raw), 4)]


def signed(value):
    return value - (1 << 32) if value & 0x80000000 else value


def as_float(value):
    return struct_float(value)


def struct_float(value):
    import struct
    return struct.unpack('<f', value.to_bytes(4, 'little'))[0]


def spawn_record(parent, command):
    payload = words(command)
    opcode = command['opcode']
    target = payload[0] if payload else None
    mask = command.get('mask', 0)
    has_offset = opcode in (96, 97)
    return {
        'parentScript': parent,
        'parentBank': 0,
        'childScript': target,
        'childBank': 0,
        'opcode': opcode,
        'registrationFlags': 4 if opcode == 90 else 2 if opcode == 91 else 6 if opcode == 92 else 0,
        'detached': opcode in (95, 97),
        'hasOffset': has_offset,
        'offsetXWord': payload[1] if has_offset and len(payload) > 1 else None,
        'offsetYWord': payload[2] if has_offset and len(payload) > 2 else None,
        'offsetXMasked': bool(mask & 2) if has_offset else False,
        'offsetYMasked': bool(mask & 4) if has_offset else False,
        'offsetX': as_float(payload[1]) if has_offset and len(payload) > 1 and not (mask & 2) else None,
        'offsetY': as_float(payload[2]) if has_offset and len(payload) > 2 and not (mask & 4) else None,
        'dynamicMaskedTarget': bool(mask & 1),
        'commandOffset': command.get('offset'),
        'commandTime': command.get('time'),
        'raw': command.get('raw', ''),
    }


def supported_ops():
    text = (ROOT / 'src' / 'animation' / 'AnmLogic.cpp').read_text(encoding='utf-8')
    start = text.index('bool VM::command(')
    end = text.index('const State& VM::state()', start)
    body = text[start:end]
    if 'if (op >= 6 && op <= 27)' not in body or 'if (op >= 28 && op <= 39)' not in body:
        raise ValueError('AnmLogic command range dispatch changed; review parser')
    supported = {int(value) for value in re.findall(r'\bcase\s+(-?\d+)\s*:', body)}
    supported.update(range(6, 28))
    supported.update(range(28, 40))
    supported.update({-1})
    if not REQUIRED_RANGE_OPS <= supported:
        raise ValueError('known op9/op23 range handlers are missing')
    return supported


def script_metadata(script, supported):
    ops = Counter(command['opcode'] for command in script['commands'])
    layers = set()
    modes = set()
    geometry = []
    sprites = []
    holds = loops = timers = 0
    for command in script['commands']:
        op = command['opcode']
        payload = words(command)
        if op == 68 and payload:
            layers.add(payload[0] & 255)
        elif op == 67 and payload:
            modes.add(payload[0] & 31)
        elif op in (84, 101) and payload:
            modes.add(9 if op == 84 else 13)
            geometry.append({'opcode': op, 'count': payload[0]})
        elif op in (104, 105) and len(payload) > 1:
            modes.add(16 if op == 104 else 17)
            geometry.append({'opcode': op, 'radius': as_float(payload[0]), 'count': payload[1]})
        elif op == 108 and len(payload) > 1:
            modes.add(18)
            geometry.append({'opcode': op, 'radius': as_float(payload[0]), 'width': as_float(payload[1])})
        elif op == 114 and payload:
            modes.add(14)
            geometry.append({'opcode': op, 'count': payload[0]})
        elif op == 3 and payload:
            sprites.extend(signed(value) for value in payload)
        elif op == 63:
            holds += 1
        elif op == 4:
            loops += 1
        elif op == 75:
            timers += 1
    unsupported = sorted(set(ops) - supported)
    return {
        'script': script['sourceId'],
        'index': script.get('index'),
        'entry': script.get('entry'),
        'offset': script.get('offset'),
        'ops': sorted(ops),
        'opCounts': dict(sorted(ops.items())),
        'unsupportedOps': unsupported,
        'layers': sorted(layers),
        'modes': sorted(modes),
        'geometry': geometry,
        'sprites': sorted(set(sprites)),
        'negativeSprites': sorted(value for value in set(sprites) if value < 0),
        'requiresAscii313': any(value < 0 for value in sprites),
        'controlFlow': {'op4Loops': loops, 'op63Holds': holds,
                        'op75Timers': timers, 'lastOpcode': script['commands'][-1]['opcode'] if script['commands'] else None},
    }


def classify(parent, child, whitelist):
    ranges = WHITELIST.get(parent, [])
    return any(low <= child <= high for low, high in ranges)


def main():
    catalog = json.loads(CATALOG.read_text(encoding='utf-8'))
    manifest_rel = catalog['variants']['ja']['manifest']
    variant = (ROOT / 'local' / manifest_rel).parent
    bullet_meta_path = variant / 'animations' / 'bullet.json'
    bullet_anm_path = variant / 'raw' / 'bullet.anm'
    meta = json.loads(bullet_meta_path.read_text(encoding='utf-8'))
    supported = supported_ops()

    scripts = {script['sourceId']: script_metadata(script, supported) for script in meta['scripts']}
    spawn_edges = []
    for script in meta['scripts']:
        for command in script['commands']:
            if command['opcode'] in SPAWN_OPS:
                spawn_edges.append(spawn_record(script['sourceId'], command))

    incoming = defaultdict(list)
    outgoing = defaultdict(list)
    for edge in spawn_edges:
        if edge['childScript'] is not None:
            incoming[edge['childScript']].append(edge['parentScript'])
            outgoing[edge['parentScript']].append(edge['childScript'])

    def reaches(start, target, seen=None):
        if start == target:
            return True
        if seen is None:
            seen = set()
        if start in seen:
            return False
        seen.add(start)
        return any(reaches(child, target, seen) for child in outgoing.get(start, []))

    for edge in spawn_edges:
        child = edge['childScript']
        edge['parentNested'] = bool(incoming.get(edge['parentScript']))
        edge['childHasChildren'] = bool(outgoing.get(child))
        edge['cycle'] = child is not None and reaches(child, edge['parentScript'])
        edge['topologyWhitelisted'] = classify(edge['parentScript'], child, WHITELIST)
        edge['inProgress197'] = edge['parentScript'] == IN_PROGRESS_PARENT
        edge['focusedTested197'] = edge['parentScript'] == IN_PROGRESS_PARENT
        edge['reviewStatus'] = ('focused-tested 1/0' if edge['focusedTested197']
                                else 'whitelisted-topology' if edge['topologyWhitelisted'] else 'unlisted')
        edge['parentMeta'] = scripts[edge['parentScript']]
        edge['childMeta'] = scripts.get(child) if child is not None else None
        edge['parentUnsupportedOps'] = edge['parentMeta']['unsupportedOps']
        edge['childUnsupportedOps'] = edge['childMeta']['unsupportedOps'] if edge['childMeta'] else []

    families = []
    for parent in sorted({edge['parentScript'] for edge in spawn_edges}):
        edges = [edge for edge in spawn_edges if edge['parentScript'] == parent]
        children = sorted({edge['childScript'] for edge in edges if edge['childScript'] is not None})
        one_layer = all(not outgoing.get(child) for child in children)
        all_attached_op88 = all(
            edge['opcode'] == 88 and edge['registrationFlags'] == 0 and not edge['detached']
            and not edge['hasOffset'] and not edge['dynamicMaskedTarget'] for edge in edges)
        parent_meta = scripts[parent]
        op_blocked = bool(parent_meta['unsupportedOps']) or any(
            scripts[child]['unsupportedOps'] for child in children if child in scripts)
        clone_children = [scripts[child] for child in children if child in scripts]
        clone_shape = (
            parent != CLONE_PARENT and len(edges) == 3 and len(children) == 3
            and all_attached_op88 and one_layer
            and sorted(edge['commandOffset'] for edge in edges) == [24, 36, 48]
            and sorted(edge['commandTime'] for edge in edges) == [0, 3, 4]
            and parent_meta['ops'] == sorted(CLONE_PARENT_OPS)
            and [child['ops'] for child in clone_children] == [sorted(value) for value in CLONE_CHILD_OPS]
        )
        families.append({
            'parentScript': parent,
            'parentIsGraphRoot': not incoming.get(parent),
            'parentNested': bool(incoming.get(parent)),
            'childScripts': children,
            'edgeCount': len(edges),
            'oneLayer': one_layer,
            'allAttachedOp88NoFlagsNoOffset': all_attached_op88,
            'topologyWhitelisted': parent in WHITELIST,
            'inProgress197': parent == IN_PROGRESS_PARENT,
            'focusedTested197': parent == IN_PROGRESS_PARENT,
            'reviewStatus': ('focused-tested 1/0' if parent == IN_PROGRESS_PARENT
                             else 'whitelisted-topology' if parent in WHITELIST else 'unlisted'),
            'opBlocked': op_blocked,
            'parentOps': parent_meta['ops'],
            'parentUnsupportedOps': parent_meta['unsupportedOps'],
            'childUnsupportedOps': sorted({op for child in children if child in scripts
                                           for op in scripts[child]['unsupportedOps']}),
            'clone197Shape': clone_shape,
            'parentLayers': parent_meta['layers'],
            'childLayers': sorted({layer for child in children if child in scripts for layer in scripts[child]['layers']}),
            'parentModes': parent_meta['modes'],
            'childModes': sorted({mode for child in children if child in scripts for mode in scripts[child]['modes']}),
            'parentSprites': parent_meta['sprites'],
            'childSprites': sorted({sprite for child in children if child in scripts for sprite in scripts[child]['sprites']}),
            'ascii313': parent_meta['requiresAscii313'] or any(
                scripts[child]['requiresAscii313'] for child in children if child in scripts),
        })

    candidate_families = [family for family in families
                          if family['clone197Shape'] and not family['topologyWhitelisted']
                          and not family['inProgress197']]
    clean_batch = [family for family in families if family['parentIsGraphRoot']
                   and family['allAttachedOp88NoFlagsNoOffset'] and family['oneLayer']
                   and not family['opBlocked'] and not family['ascii313']]
    unsupported_counts = Counter()
    for script in scripts.values():
        for op in script['unsupportedOps']:
            unsupported_counts[op] += 1
    edge_counts = Counter(edge['opcode'] for edge in spawn_edges)
    combined = hashlib.sha256(bullet_anm_path.read_bytes() + bullet_meta_path.read_bytes()).hexdigest()

    result = {
        'schema': 'th128-boss-child-family-catalog/1',
        'generatedBy': 'tools/research/boss_child_family_catalog.py',
        'activeGeneration': {
            'catalog': str(CATALOG.relative_to(ROOT)),
            'variant': 'ja',
            'manifest': manifest_rel,
        },
        'data': {
            'bulletAnm': {'path': str(bullet_anm_path.relative_to(ROOT)),
                          'bytes': bullet_anm_path.stat().st_size, 'sha256': sha(bullet_anm_path)},
            'bulletJson': {'path': str(bullet_meta_path.relative_to(ROOT)),
                           'bytes': bullet_meta_path.stat().st_size, 'sha256': sha(bullet_meta_path)},
            'combinedSha256': combined,
            'bank': 0,
            'scripts': len(meta['scripts']),
            'edges': len(spawn_edges),
            'opcodeCounts': dict(sorted(edge_counts.items())),
            'graphRoots': sum(1 for script in scripts if not incoming.get(script)),
            'graphRootsWithChildren': sum(1 for family in families if family['parentIsGraphRoot']),
            'nestedEdgeCount': sum(1 for edge in spawn_edges if edge['childHasChildren']),
            'cycleCount': sum(1 for edge in spawn_edges if edge['cycle']),
            'dynamicMaskedTargetCount': sum(1 for edge in spawn_edges if edge['dynamicMaskedTarget']),
        },
        'source': {
            'enemyVisuals': {'path': 'src/game/EnemyVisuals.cpp', 'sha256': sha(ROOT / 'src/game/EnemyVisuals.cpp'),
                             'whitelistSource': WHITELIST_SOURCE},
            'anmLogic': {'path': 'src/animation/AnmLogic.cpp', 'sha256': sha(ROOT / 'src/animation/AnmLogic.cpp'),
                         'supportedOpCount': len(supported), 'supportedOps': sorted(supported),
                         'rangeDispatch': [[6, 27], [28, 39]],
                         'sourceAssertions': ['if (op >= 6 && op <= 27)', 'if (op >= 28 && op <= 39)'],
                         'requiredRangeOps': sorted(REQUIRED_RANGE_OPS)},
        },
        'focused197': {
            'status': 'focused-tested 1/0',
            'native': FOCUSED_197_NATIVE,
            'runner': FOCUSED_197_TEST,
            'nativeSha256': sha(ROOT / FOCUSED_197_NATIVE),
            'runnerSha256': sha(ROOT / FOCUSED_197_TEST),
        },
        'whitelist': {str(parent): [list(item) for item in ranges] for parent, ranges in WHITELIST.items()},
        'inProgressParent': IN_PROGRESS_PARENT,
        'scripts': {str(key): value for key, value in sorted(scripts.items())},
        'edges': spawn_edges,
        'families': families,
        'candidateFamilies': candidate_families,
        'cleanBatchFamilies': clean_batch,
        'unsupportedOpsByScriptCount': dict(sorted(unsupported_counts.items())),
        'notes': [
            'Only bank 0 bullet.anm/bullet.json; no ECL stage ownership is inferred.',
            'Layers/modes/geometry are command-declared, not atlas-rect screen layout guesses.',
            'Op63 hold/op4 loop/op75 timer are recorded; final op1 time is not treated as natural lifetime.',
            'Clone197Shape is topology-only; child op support and a new targeted family matrix are still required.',
        ],
    }
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT.write_text(json.dumps(result, indent=2, ensure_ascii=False) + '\n', encoding='utf-8', newline='\n')
    print(json.dumps({
        'output': str(OUTPUT.relative_to(ROOT)),
        'bytes': OUTPUT.stat().st_size,
        'data': result['data'],
        'candidates': [(family['parentScript'], family['childScripts'],
                        family['childUnsupportedOps']) for family in candidate_families],
        'cleanBatch': [(family['parentScript'], family['childScripts']) for family in clean_batch],
        'unsupportedOpsByScriptCount': result['unsupportedOpsByScriptCount'],
    }, ensure_ascii=False, indent=2))


if __name__ == '__main__':
    main()
