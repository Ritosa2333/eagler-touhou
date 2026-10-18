"""Join every observed emulator disagreement with the independent hardware run."""
from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[1]


def key(row):
    return tuple(row[field].lower() if row[field] is not None else None
        for field in ('function', 'current_bits', 'rate_bits', 'delta_bits'))


def main():
    cpu = json.loads((ROOT / 'reports/native_cpu_validation.json').read_text('utf-8'))
    edge = json.loads((ROOT / 'reports/native_edges_validation.json').read_text('utf-8'))
    probes = {key(row): row for row in cpu['hardware_probes']}
    matches, missing = [], []
    for failure in edge['failures_all']:
        hardware = probes.get(key(failure))
        if hardware is None:
            missing.append(failure)
            continue
        matches.append({field: failure[field]
            for field in ('function', 'current_bits', 'rate_bits', 'delta_bits')} | {
            'unicorn': failure['original_object'], 'cpp_x64': failure['cpp_object'],
            'original_hardware': hardware['original_object'], 'cpp_x86': hardware['cpp_object'],
            'hardware_matches_cpp_x64': hardware['original_object'] == failure['cpp_object'],
            'hardware_matches_cpp_x86': hardware['original_object'] == hardware['cpp_object'],
            'hardware_matches_unicorn': hardware['original_object'] == failure['original_object']})
    attributed = (not missing and len(matches) == edge['failure_count'] and len(matches) > 0
        and all(row['hardware_matches_cpp_x64'] and row['hardware_matches_cpp_x86']
            and not row['hardware_matches_unicorn'] for row in matches))
    report = {
        'status': 'attributed_to_emulator_nan_propagation' if attributed else 'unresolved',
        'scope': 'Attribution applies to the exact observed disagreement inputs; no claim about every IEEE754 input or full gameplay',
        'cpu_status': cpu['status'], 'cpu_total': cpu['total'],
        'observed_emulator_disagreements': edge['failure_count'],
        'matched_hardware_probes': len(matches), 'missing_hardware_cases': missing,
        'all_hardware_matches_cpp_x64': all(row['hardware_matches_cpp_x64'] for row in matches),
        'all_hardware_matches_cpp_x86': all(row['hardware_matches_cpp_x86'] for row in matches),
        'all_hardware_differs_from_unicorn': all(not row['hardware_matches_unicorn'] for row in matches),
        'matches': matches
    }
    (ROOT / 'reports/native_nan_oracle_comparison.json').write_text(json.dumps(report, indent=2), encoding='utf-8')
    print(json.dumps({k: report[k] for k in ('status', 'observed_emulator_disagreements', 'matched_hardware_probes')}))
    return 0 if attributed else 1


if __name__ == '__main__':
    raise SystemExit(main())
