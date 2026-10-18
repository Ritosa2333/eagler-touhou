"""Register reviewed entry mappings as compilation/symbol-audit evidence only.

The 22-entry static call graph is evidence, not 22 recovered implementations.
These records do not inherit the independent scheduler's CPU validation.
"""
from __future__ import annotations
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
MODULE = 'source_reconstruction/program_entry'
MAPPINGS = {
    0x41e7d0: ('recovered_win_main', 'program_entry.cpp', 'Control flow with unresolved engine calls and globals.'),
    0x419c20: ('run_unlimited_frame', 'frame_schedulers.cpp', 'Unthrottled frame control flow; real clock, renderer and callbacks remain unresolved.'),
    0x419de0: ('run_timed_frame', 'frame_schedulers.cpp', 'Software-paced frame control flow; timing/floating environment has no original CPU comparison.'),
    0x41a030: ('run_present_paced_frame', 'frame_schedulers.cpp', 'Present-paced control flow; original device and timing behavior are not verified.'),
    0x41cc70: ('display_mode', 'frame_schedulers.cpp', 'Field operation; no original CPU comparison.'),
    0x41d080: ('needs_device_reset', 'frame_schedulers.cpp', 'Field operation; no original CPU comparison.'),
    0x41dcc0: ('set_draw_counter', 'frame_schedulers.cpp', 'Field operation; no original CPU comparison.'),
    0x41de00: ('set_device_reset', 'frame_schedulers.cpp', 'Field operation; no original CPU comparison.'),
    0x41de30: ('set_reset_delay', 'frame_schedulers.cpp', 'Field operation; no original CPU comparison.'),
    0x412730: ('device', 'frame_schedulers.cpp', 'Field operation; no original CPU comparison.'),
    0x415800: ('is_windowed', 'frame_schedulers.cpp', 'Field operation; no original CPU comparison.'),
    0x41a240: ('release_device', 'frame_schedulers.cpp', 'COM call and field operation; no live COM equivalence test.'),
    0x41a200: ('release_direct3d', 'frame_schedulers.cpp', 'COM call and field operation; no live COM equivalence test.'),
    0x41b480: ('SetForegroundWindow statement in recovered_win_main', 'program_entry.cpp', 'Original wrapper represented by an inline source statement; not a separate ABI replacement.'),
    0x41b490: ('restore_system_settings', 'program_entry.cpp', 'Original Windows API call sequence; system settings behavior has not been tested.'),
}


def main():
    report_path = ROOT/MODULE/'module_status.json'
    report = json.loads(report_path.read_text(encoding='utf-8'))
    if report['status'] != 'compiled_static_module_with_unrecovered_dependencies' or report['independent_executable_linkable']:
        raise ValueError('Review changed program_entry evidence scope before registration')
    bindings = {}
    for filename, expected in report['source_hashes'].items():
        path = f'{MODULE}/{filename}'
        actual = hashlib.sha256((ROOT/path).read_bytes()).hexdigest()
        if actual != expected:
            raise ValueError(f'Program-entry source evidence is stale for {path}')
        bindings[path] = actual
    # The library hash proves the symbol-audited artifact is present. It does not
    # make the unresolved dependencies executable or create an ABI equivalence.
    library = ROOT/MODULE/'build/Release/th20_program_entry_source.lib'
    if hashlib.sha256(library.read_bytes()).hexdigest() != report['library_sha256']:
        raise ValueError('Program-entry library differs from the symbol audit')
    registry_path = HERE/'additional_recoveries.json'
    registry = json.loads(registry_path.read_text(encoding='utf-8'))
    registry['recoveries'] = [x for x in registry['recoveries'] if x.get('module_id') != 'program_entry']
    for va, (name, source, limitation) in sorted(MAPPINGS.items()):
        status = ('compiled_control_flow_with_unrecovered_dependencies' if va in {0x41e7d0,0x419c20,0x419de0,0x41a030}
                  else 'compiled_inline_or_helper_coverage_unvalidated' if va in {0x41b480,0x41b490}
                  else 'compiled_field_or_com_operation_unvalidated')
        registry['recoveries'].append(dict(entry_va=f'0x{va:08x}', name=name, module_id='program_entry',
            source=f'{MODULE}/{source}', status=status,
            validation_kind='compile_and_symbol_audit_only', build_integration_status='compiled_but_unlinked',
            complete_original_function_equivalence_claimed=False, source_hash_bindings=bindings,
            hash_binding_origin='Source snapshot and library hash recorded together by the compiled module symbol audit; no CPU equivalence validation',
            evidence_hash_bindings={f'{MODULE}/module_status.json': hashlib.sha256(report_path.read_bytes()).hexdigest()},
            evidence=[f'{MODULE}/module_status.json', f'{MODULE}/README.md', f'{MODULE}/entry_call_graph.json'],
            unresolved_function_count=report['unrecovered_function_count'],
            unresolved_global_count=report['unresolved_global_count'],
            limitations=limitation + ' Module has 50 unresolved engine functions and 13 undefined global/environment symbols. Existing core_scheduler tests do not validate this entry module; no independently linked game or original-CPU/gameplay/COM equivalence is claimed.'))
    registry_path.write_text(json.dumps(registry, ensure_ascii=False, indent=2)+'\n', encoding='utf-8')
    print(json.dumps(dict(compile_only_mappings=len(MAPPINGS), build_integration_status='compiled_but_unlinked',
                         unrecovered_functions=report['unrecovered_function_count'], unresolved_globals=report['unresolved_global_count'])))


if __name__ == '__main__':
    main()
