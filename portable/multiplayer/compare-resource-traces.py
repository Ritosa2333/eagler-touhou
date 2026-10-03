"""Compare real endpoint records at a common confirmed logical-frame frontier.

No state is repaired. Missing/overflowed evidence is inconclusive, not PASS.
"""
import argparse
import json
from pathlib import Path

INVALID = 0xffffffff


def frame_map(trace):
    meta = trace['metadata']
    ceiling = min(meta['confirmedThrough'], meta['lastFrame'])
    if meta['confirmedThrough'] == INVALID or meta['lastFrame'] == INVALID:
        ceiling = -1
    if meta['rollbackFrame'] != INVALID:
        ceiling = min(ceiling, meta['rollbackFrame'] - 1)
    result = {}
    for record in trace['records']:
        f = record['frame']
        if f <= ceiling and record['attempt']['complete'] and record['revision'] > result.get(f, {}).get('revision', 0):
            result[f] = record
    return result


def word_diff(a, b, fields):
    return [{'field': fields[i] if i < len(fields) else str(i),
             'left': a[i] if i < len(a) else None, 'right': b[i] if i < len(b) else None}
            for i in range(max(len(a), len(b)))
            if (a[i] if i < len(a) else None) != (b[i] if i < len(b) else None)]


def compare(left, right):
    report = {'schema': 'th08mp/resource-trace-comparison/1', 'status': 'inconclusive',
              'firstDivergence': None, 'firstResourceDivergence': None,
              'firstItemDivergence': None, 'firstInputDivergence': None, 'firstRngDivergence': None}
    for value in (left, right):
        if value.get('schema') != 'th08mp/resource-trace/1':
            raise ValueError('not a TH08MP resource trace')
    for key in ('runtimeGeneration', 'wasmSha256', 'valueFields', 'itemFields', 'inputFields'):
        if not left.get(key) or left[key] != right.get(key):
            report['reason'] = 'incompatible ' + key
            return report
    for key in ('sessionId', 'generation', 'playerCount', 'seed', 'difficulty', 'loadouts'):
        if left['metadata'].get(key) != right['metadata'].get(key):
            report['reason'] = 'different session setup: ' + key
            return report
    if left['metadata']['localPlayer'] == right['metadata']['localPlayer']:
        report['reason'] = 'both exports identify the same endpoint seat'
        return report
    option_keys = sorted(set(left.get('options', {})) | set(right.get('options', {})))
    report['environmentDifferences'] = [
        {'field': key, 'left': left.get('options', {}).get(key), 'right': right.get('options', {}).get(key)}
        for key in option_keys
        if left.get('options', {}).get(key) != right.get('options', {}).get(key)
    ]
    report['determinismWarnings'] = []
    if any(item['field'] == 'alwaysHitbox' for item in report['environmentDifferences']):
        report['determinismWarnings'].append(
            'alwaysHitbox differs between endpoints; older TH08MP builds wrote this local display preference into player/effect simulation state')
    a, b = frame_map(left), frame_map(right)
    common = sorted(a.keys() & b.keys())
    if not common:
        report['reason'] = 'no overlapping confirmed frames'
        return report
    report['commonWindow'] = [common[0], common[-1]]
    report['comparedFrames'] = len(common)
    report['missingFrames'] = [f for f in range(common[0], common[-1] + 1) if f not in a or f not in b]
    report['traceOverflow'] = any(value['metadata'].get('traceOverflow') for value in (left, right))
    report['restoreMismatch'] = [value['metadata'].get('restoreMismatch') for value in (left, right)]
    for records, trace in ((a, left), (b, right)):
        for f in common:
            attempt = records[f]['attempt']
            if (attempt.get('overflow') or not isinstance(attempt.get('inputs'), list)
                    or len(attempt['inputs']) != len(trace['inputFields']) * trace['metadata']['playerCount']
                    or len(attempt['end']['values']) != len(trace['valueFields'])
                    or any(len(item) != len(trace['itemFields']) for item in attempt['end']['items'])):
                report['reason'] = 'incomplete frame fields/events at ' + str(f)
                return report
    for f in common:
        ar, br = a[f], b[f]
        av, bv = ar['attempt']['end'], br['attempt']['end']
        fields = word_diff(av['values'], bv['values'], left['valueFields'])
        inputs = ar['attempt'].get('inputs') != br['attempt'].get('inputs')
        items = av['items'] != bv['items']
        resource = [d for d in fields if d['field'] in ('time', 'totalTime', 'clock', 'score')
                    or d['field'].endswith(('.power', '.lives', '.bombs'))]
        rng = [d for d in fields if d['field'] in ('rngSeed', 'rngBackup', 'rngCalls')]
        for key, found in [('firstInputDivergence', inputs), ('firstItemDivergence', items),
                           ('firstResourceDivergence', resource), ('firstRngDivergence', rng)]:
            if found and report[key] is None:
                report[key] = f
        if not (fields or inputs or items) or report['firstDivergence'] is not None:
            continue
        report['firstDivergence'] = f
        report['leftCensored'] = f == common[0]
        report['fieldDifferences'] = fields
        li, ri = {v[0]: v for v in av['items']}, {v[0]: v for v in bv['items']}
        report['itemDifferences'] = [{'slot': slot, 'fields': word_diff(li.get(slot, []), ri.get(slot, []), left['itemFields'])}
                                     for slot in sorted(li.keys() | ri.keys()) if li.get(slot) != ri.get(slot)]
        ae, be = ar['attempt'].get('events', []), br['attempt'].get('events', [])
        for i in range(max(len(ae), len(be))):
            x, y = ae[i] if i < len(ae) else None, be[i] if i < len(be) else None
            if x != y:
                report['firstDifferentEvent'] = {'ordinal': i, 'left': x, 'right': y}
                break
        report['evidenceWindow'] = [{'frame': at, 'left': a.get(at), 'right': b.get(at)}
                                    for at in range(max(common[0], f - 2), min(common[-1], f + 2) + 1)]
    if report['firstDivergence'] is not None:
        report['status'] = 'confirmed-divergence'
        report['reason'] = 'first mismatch in retained common confirmed window; causal root not inferred automatically'
    elif report['traceOverflow'] or report['missingFrames'] or any(v is not None for v in report['restoreMismatch']):
        report['reason'] = 'missing/overflowed evidence or failed local undo audit'
    else:
        report['status'] = 'equal-in-retained-window'
        report['reason'] = 'not proof of correctness outside this retained window'
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('left', type=Path)
    parser.add_argument('right', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    result = compare(json.loads(args.left.read_text(encoding='utf-8')), json.loads(args.right.read_text(encoding='utf-8')))
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps({k: result.get(k) for k in ('status', 'commonWindow', 'firstDivergence',
                     'firstItemDivergence', 'firstResourceDivergence', 'firstRngDivergence', 'reason')}, ensure_ascii=False))
    return 1 if result['status'] == 'confirmed-divergence' else 2 if result['status'] == 'inconclusive' else 0


if __name__ == '__main__':
    raise SystemExit(main())
