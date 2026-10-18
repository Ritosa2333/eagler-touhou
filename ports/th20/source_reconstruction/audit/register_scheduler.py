"""Register explicitly reviewed scheduler mappings with source-bound CPU evidence.

This is a reviewed address list, not an automatic inference from VA comments.
None of these records asserts complete game integration or original CRT ABI.
"""
from __future__ import annotations
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent

VALIDATED = {
    0x411970: 'initialize_link', 0x411a70: 'initialize_node',
    0x418a10: 'initialize_sentinel_link', 0x418a50: 'initialize_list',
    0x418a80: 'initialize_state', 0x4119b0: 'iterator_begin',
    0x411b00: 'iterator_destroy', 0x411c30: 'iterator_advance',
    0x411d50: 'unlink_raw', 0x411ce0: 'unlink_observer_repair',
    0x411d20: 'unlink_owner_tail_repair', 0x411e30: 'list_lookup_helper',
    0x411e70: 'list_find', 0x411ee0: 'insert_after',
    0x411f30: 'insert_before', 0x411ea0: 'append',
    0x411f80: 'ordered_update_insert', 0x412100: 'ordered_draw_insert',
    0x412400: 'remove_unlocked', 0x412810: 'dispatch_update',
    0x412aa0: 'dispatch_draw', 0x412d30: 'set_owned',
    0x412d80: 'enable', 0x4127f0: 'disable', 0x412d50: 'set_callback',
    0x412d10: 'set_userdata', 0x412dc0: 'set_before_insert',
    0x412da0: 'set_shutdown_callback', 0x411b80: 'clear_callbacks',
}
PARTIAL = {
    0x4124b0: ('remove_locked', 'Real C++ mutex replaces original CRT lock layout; full lock integration is unverified.'),
    0x4127b0: ('create_owned_node', 'Composes recovered initialization using standard allocation; original allocator is not recovered.'),
    0x411910: ('allocate_node', 'Standard C++ allocation replaces original allocation metadata and error behavior.'),
    0x4122c0: ('register_update_enabled', 'Composed wrapper; original global manager becomes explicit State argument.'),
    0x412310: ('register_update_disabled', 'Composed wrapper; original global manager becomes explicit State argument.'),
    0x412360: ('register_draw_enabled', 'Composed wrapper; original global manager becomes explicit State argument.'),
    0x4123b0: ('register_draw_disabled', 'Composed wrapper; original global manager becomes explicit State argument.'),
    0x4125b0: ('shutdown_chains_scheduling_portion', 'Leading renderer flush at 0x004d9e30 remains an explicit external dependency.'),
}


def main():
    report_path = ROOT/'source_reconstruction/core_scheduler/cpu_validation.json'
    report = json.loads(report_path.read_text(encoding='utf-8'))
    if report['status'] != 'passed' or report['failed'] != 0:
        raise ValueError('Scheduler CPU evidence is not passing')
    bindings = {}
    for suffix in ('cpp', 'hpp'):
        path = f'source_reconstruction/core_scheduler/scheduler.{suffix}'
        actual = hashlib.sha256((ROOT/path).read_bytes()).hexdigest()
        if actual != report[f'scheduler_{suffix}_sha256']:
            raise ValueError(f'Scheduler evidence is stale for {path}')
        bindings[path] = actual
    evidence = ['source_reconstruction/core_scheduler/cpu_validation.json',
                'source_reconstruction/core_scheduler/README.md']
    registry_path = HERE/'additional_recoveries.json'
    registry = json.loads(registry_path.read_text(encoding='utf-8'))
    registry['recoveries'] = [x for x in registry['recoveries'] if x.get('module_id') != 'core_scheduler']
    for va in sorted(VALIDATED.keys() | PARTIAL.keys()):
        partial = va in PARTIAL
        entry = dict(entry_va=f'0x{va:08x}', module_id='core_scheduler',
            name=PARTIAL[va][0] if partial else VALIDATED[va],
            source='source_reconstruction/core_scheduler/scheduler.cpp',
            status='compiled_scheduler_partial_or_composed' if partial else 'compiled_scheduler_component_cpu_validated',
            build_integration_status='compiled_but_unlinked',
            complete_original_function_equivalence_claimed=False,
            source_hash_bindings=bindings, evidence=evidence,
            hash_binding_origin='Matching source SHA256 fields embedded in the passing CPU validation report',
            evidence_hash_bindings={str(report_path.relative_to(ROOT)).replace('\\','/'): hashlib.sha256(report_path.read_bytes()).hexdigest()},
            validation_scope='Module-level 13767 original CPU comparisons plus 1 source-owned allocation/removal execution; selected functions covered directly or through composed operations. See README address table.',
            limitations=(PARTIAL[va][1] + ' ' if partial else '') +
                'No complete-engine linkage. Original allocator, renderer flush, multithread scheduling and callback exception unwinding are outside validation; corresponding object pointers are normalized in comparisons.')
        registry['recoveries'].append(entry)
    registry_path.write_text(json.dumps(registry, ensure_ascii=False, indent=2)+'\n', encoding='utf-8')
    print(json.dumps(dict(cpu_covered_mappings=len(VALIDATED), partial_or_composed_mappings=len(PARTIAL),
                         build_integration_status='compiled_but_unlinked'), indent=2))


if __name__ == '__main__':
    main()
