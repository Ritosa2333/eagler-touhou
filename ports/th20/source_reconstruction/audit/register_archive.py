"""Refresh the seven manually reviewed archive mappings using source-bound evidence."""
from __future__ import annotations
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
ADDRESSES = {0x4100e0, 0x456270, 0x5391f0, 0x539ed0, 0x53a210, 0x53a350, 0x53a3c0}


def main():
    report_path = ROOT/'source_reconstruction/archive/archive_validation.json'
    report = json.loads(report_path.read_text(encoding='utf-8'))
    if report['status'] != 'passed' or report['records'] != 285 or not all(x['identical'] for x in report['entries']):
        raise ValueError('Archive evidence is not a complete passing 285-record comparison')
    bindings = {}
    for suffix in ('cpp', 'hpp'):
        path = f'source_reconstruction/archive/archive.{suffix}'
        actual = hashlib.sha256((ROOT/path).read_bytes()).hexdigest()
        if actual != report[f'archive_{suffix}_sha256']:
            raise ValueError(f'Archive evidence is stale for {path}')
        bindings[path] = actual
    registry_path = HERE/'additional_recoveries.json'
    registry = json.loads(registry_path.read_text(encoding='utf-8'))
    entries = [x for x in registry['recoveries'] if int(x['entry_va'], 16) in ADDRESSES]
    if len(entries) != len(ADDRESSES):
        raise ValueError('Expected seven existing manually reviewed archive entries')
    for entry in entries:
        entry.update(module_id='archive', build_integration_status='compiled_but_unlinked',
            complete_original_function_equivalence_claimed=False, source_hash_bindings=bindings,
            evidence_hash_bindings={str(report_path.relative_to(ROOT)).replace('\\','/'): hashlib.sha256(report_path.read_bytes()).hexdigest()},
            hash_binding_origin='Matching source SHA256 fields embedded in the passing resource validation report')
    registry_path.write_text(json.dumps(registry, ensure_ascii=False, indent=2)+'\n', encoding='utf-8')
    print(json.dumps(dict(algorithm_mappings=len(entries), build_integration_status='compiled_but_unlinked')))


if __name__ == '__main__':
    main()
