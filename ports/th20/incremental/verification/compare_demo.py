"""Compare named game fields at shared original replay frame numbers."""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('baseline', type=Path)
    parser.add_argument('rebuilt', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    original = json.loads(args.baseline.read_text(encoding='utf-8'))
    rebuilt = json.loads(args.rebuilt.read_text(encoding='utf-8'))
    def comparison_fields(sample):
        fields = sample['fields'].copy()
        # Initial recorder captured a tuple from 0x1b8b4c, 0x1b8b54,
        # and 0x1b8b58. Only the first is a verified keyboard input bitmask.
        # Preserve raw files while restricting comparison to that known field.
        if 'replay_input' in fields:
            fields['replay_input_bits'] = fields.pop('replay_input')[0]
        return fields
    a = {(x['replay'], x['stage'], x['frame']): comparison_fields(x) for x in original['samples']}
    b = {(x['replay'], x['stage'], x['frame']): comparison_fields(x) for x in rebuilt['samples']}
    common = sorted(a.keys() & b.keys())
    mismatches = []
    fields_checked = 0
    by_replay = {}
    for replay, stage, frame in common:
        key = (replay, stage, frame)
        summary = by_replay.setdefault(replay, dict(stage=stage, shared_frames=0,
             identical_frames=0, first_frame=frame, last_frame=frame, field_comparisons=0))
        summary['shared_frames'] += 1
        summary['last_frame'] = frame
        diffs = {}
        for field in sorted(a[key].keys() | b[key].keys()):
            fields_checked += 1
            summary['field_comparisons'] += 1
            if a[key].get(field) != b[key].get(field):
                diffs[field] = dict(original=a[key].get(field), rebuilt=b[key].get(field))
        if diffs:
            mismatches.append(dict(replay=replay, stage=stage, frame=frame, differences=diffs))
        else:
            summary['identical_frames'] += 1
    report = dict(status='passed_listed_fields' if common and not mismatches else 'differences_or_insufficient_overlap',
                  baseline_file=str(args.baseline), rebuilt_file=str(args.rebuilt),
                  baseline_sha256=hashlib.sha256(args.baseline.read_bytes()).hexdigest(),
                  rebuilt_sha256=hashlib.sha256(args.rebuilt.read_bytes()).hexdigest(),
                  baseline_executable=original['executable']['path'],
                  rebuilt_executable=rebuilt['executable']['path'],
                  baseline_executable_sha256=hashlib.sha256(Path(original['executable']['path']).read_bytes()).hexdigest(),
                  rebuilt_executable_sha256=hashlib.sha256(Path(rebuilt['executable']['path']).read_bytes()).hexdigest(),
                  executable_hash_provenance='Read from the recorded process module paths at comparison time; compare with launch and build manifests.',
                  shared_frames=len(common), identical_frames=len(common)-len(mismatches),
                  field_comparisons=fields_checked,
                  compared_fields=sorted(next(iter(a.values())).keys()) if a else [],
                  by_replay=by_replay, mismatches=mismatches,
                  limitations=original['limitations'] + ['This is not evidence of full-game 1:1 equivalence.'])
    args.output.write_text(json.dumps(report, indent=2), encoding='utf-8')
    print(json.dumps({k: v for k, v in report.items() if k != 'mismatches'}, indent=2))
    if mismatches:
        print(json.dumps(dict(first_mismatches=mismatches[:5]), indent=2))
    if not common or mismatches:
        raise SystemExit(1)


if __name__ == '__main__':
    main()
