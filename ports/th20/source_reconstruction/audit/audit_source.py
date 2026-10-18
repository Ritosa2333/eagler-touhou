"""Reproducible inventory of decompiler evidence versus registered C++ recovery.

The audit reads the original PE and existing evidence without modifying them.
Lexical markers are triage indicators, not semantic proof. Ghidra's inventory is
not assumed to enumerate every real function, and library names can be wrong.
"""
from __future__ import annotations
import argparse
import bisect
from collections import Counter, defaultdict
import csv
import hashlib
import json
from pathlib import Path
import re
import shutil
import struct
import subprocess

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
GHIDRA = ROOT / 'analysis' / 'ghidra'


def hash_file(path):
    with Path(path).open('rb') as f:
        return hashlib.file_digest(f, 'sha256').hexdigest()


def rows(path):
    with Path(path).open(encoding='utf-8-sig', newline='') as f:
        return list(csv.DictReader(f))


def dump(path, value):
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2), encoding='utf-8')


def merged_size(spans):
    total = 0
    end = -1
    for first, last in sorted(spans):
        if last > end:
            total += last - max(first, end)
            end = last
    return total


class Image:
    def __init__(self, path):
        self.data = Path(path).read_bytes()
        pe = struct.unpack_from('<I', self.data, 0x3c)[0]
        count, opts = struct.unpack_from('<H', self.data, pe + 6)[0], struct.unpack_from('<H', self.data, pe + 20)[0]
        self.base = struct.unpack_from('<I', self.data, pe + 24 + 28)[0]
        self.sections = []
        for i in range(count):
            off = pe + 24 + opts + 40 * i
            virtual_size, rva, size, raw = struct.unpack_from('<IIII', self.data, off + 8)
            self.sections.append((self.base + rva, size, raw))

    def at(self, va, size=6):
        for base, raw_size, raw in self.sections:
            if base <= va and va + size <= base + raw_size:
                return self.data[raw + va - base:raw + va - base + size]
        return b''


def compile_probe(compiler):
    probe = HERE / 'compile_probe'
    probe.mkdir(exist_ok=True)
    source = GHIDRA / 'th20_pseudocode.c'
    copied = probe / 'th20_pseudocode_unmodified.cpp'
    shutil.copyfile(source, copied)
    cmd = [str(compiler), '/nologo', '/TP', '/std:c++17', '/Zs', '/utf-8', '/diagnostics:column', str(copied)]
    completed = subprocess.run(cmd, cwd=probe, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    raw_log = probe / 'compiler_output.raw'
    raw_log.write_bytes(completed.stdout)
    try:
        output = completed.stdout.decode('utf-8')
        encoding = 'utf-8'
    except UnicodeDecodeError:
        output = completed.stdout.decode('gb18030', errors='replace')
        encoding = 'gb18030'
    (probe / 'compiler_output.txt').write_text(output, encoding='utf-8')
    errors = Counter(re.findall(r'\b(?:fatal error|error|错误|致命错误)\s+(C\d+)\b', output))
    if not errors:
        errors = Counter(re.findall(r'\b(C\d{4})\b', output))
    report = dict(command=cmd, compiler=str(compiler), exit_code=completed.returncode,
        original_source_sha256=hash_file(source), copied_source_sha256=hash_file(copied),
        copied_bytes_unchanged=hash_file(source) == hash_file(copied),
        no_shims_or_stub_definitions_added=True, syntax_only=True,
        generated_binary=False, diagnostic_codes=dict(errors),
        decoded_encoding=encoding, diagnostics_file='source_reconstruction/audit/compile_probe/compiler_output.txt',
        limitation='Compiler error limit can stop parsing early; diagnostics are representative, not a complete count of blockers.')
    dump(probe / 'result.json', report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--probe-compile', action='store_true')
    parser.add_argument('--compiler', type=Path,
        default=Path('D:/VS2019/IDE/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x86/cl.exe'))
    args = parser.parse_args()
    HERE.mkdir(parents=True, exist_ok=True)
    source_manifest = json.loads((ROOT / 'reports' / 'source_manifest.json').read_text(encoding='utf-8'))
    exe = Path(source_manifest['source_directory']) / 'th20.exe'
    expected_sha = next(x['sha256'] for x in source_manifest['files'] if x['name'] == 'th20.exe')
    if hash_file(exe) != expected_sha:
        raise ValueError('Source EXE differs from the analyzed revision')
    image = Image(exe)
    functions = rows(GHIDRA / 'functions.csv')
    source_links = defaultdict(list)
    for link in rows(GHIDRA / 'source_location_functions.csv'):
        if link['function_entry']:
            source_links[int(link['function_entry'], 16)].append(link)
    imports = {int(x['iat_va'], 16): x for x in rows(ROOT / 'analysis' / 'binary' / 'imports.csv')}
    import_names = {x['name'] for x in imports.values()}
    spans = []
    spans_by_func = defaultdict(list)
    for entry in rows(GHIDRA / 'function_ranges.csv'):
        start, end, owner = int(entry['range_start'], 16), int(entry['range_end_inclusive'], 16)+1, int(entry['function_entry'], 16)
        spans.append((start, end, owner))
        spans_by_func[owner].append((start, end))
    spans.sort()
    starts = [s[0] for s in spans]
    machine_calls = defaultdict(list)
    unmatched_call_sites = []
    instruction_re = re.compile(r'^([0-9a-fA-F]{8})\s+((?:[0-9a-fA-F]{2}\s+)+)([a-z][a-z0-9]*)\s*(.*?)\s*$')
    for line in (ROOT / 'analysis' / 'binary' / 'disassembly.asm').open(encoding='utf-8'):
        match = instruction_re.match(line)
        if not match or match[3] != 'call':
            continue
        va = int(match[1], 16)
        operand = match[4].split(';', 1)[0].strip()
        target = re.fullmatch(r'0x([0-9a-fA-F]+)', operand)
        memory = re.search(r'\[0x([0-9a-fA-F]+)\]', operand)
        iat = imports.get(int(memory[1], 16)) if memory else None
        kind = 'direct_call' if target else 'resolved_iat_import_call' if iat else 'unresolved_indirect_call'
        call = dict(va=f'0x{va:08x}', kind=kind, operand=operand)
        if iat:
            call['import'] = iat['dll'] + '!' + iat['name']
        index = bisect.bisect_right(starts, va) - 1
        if index >= 0 and spans[index][0] <= va < spans[index][1]:
            machine_calls[spans[index][2]].append(call)
        else:
            unmatched_call_sites.append(call)
    # Only explicit, evidence-backed address mappings count as recovered.
    native_links = rows(GHIDRA / 'native_validation_links.csv')
    native_cpu_report = json.loads((ROOT/'reports/native_cpu_validation.json').read_text(encoding='utf-8'))
    native_core_sha256 = hash_file(ROOT/'native_recovered/native_core.hpp')
    native_evidence_current = native_cpu_report.get('status') == 'passed' and native_cpu_report.get('native_core_sha256') == native_core_sha256 and native_cpu_report.get('source_sha256') == expected_sha
    recovered = {int(x['machine_code_va'], 16): dict(
        name=x['reconstructed_cpp_name'], source='native_recovered/native_core.hpp',
        module_id='native_core', build_integration_status='linked_to_retained_original_engine_only',
        complete_original_function_equivalence_claimed=False,
        status='validated_compilable_cpp_component' if native_evidence_current else 'cpp_present_validation_stale',
        evidence=['analysis/ghidra/native_validation_links.csv', 'reports/native_validation.json',
                  'reports/native_cpu_validation.json', 'incremental/native_bridge/bridge_abi_validation.json'],
        limitations='Selected component behavior/ABI only; not a complete game module.') for x in native_links}
    registry_path = HERE / 'additional_recoveries.json'
    registry_issues = []
    if registry_path.exists():
        for entry in json.loads(registry_path.read_text(encoding='utf-8'))['recoveries']:
            entry = dict(entry)
            for required in ('entry_va', 'name', 'source', 'status', 'evidence', 'limitations', 'module_id', 'build_integration_status'):
                if required not in entry:
                    raise ValueError(f'Recovery registry entry is missing {required}: {entry}')
            for cited in [entry['source'], *entry['evidence']]:
                if not (ROOT/cited).is_file():
                    raise ValueError(f'Recovery registry cites a missing file: {cited}')
            if int(entry['entry_va'],16) in recovered:
                raise ValueError(f'Duplicate recovery address: {entry["entry_va"]}')
            entry['declared_recovery_status'] = entry['status']
            bindings = entry.get('source_hash_bindings', {})
            evidence_bindings = entry.get('evidence_hash_bindings', {})
            mismatches = []
            for cited, expected in {**bindings, **evidence_bindings}.items():
                actual = hash_file(ROOT/cited) if (ROOT/cited).is_file() else None
                if actual != expected:
                    mismatches.append(dict(path=cited, expected_sha256=expected, actual_sha256=actual))
            entry['source_binding_current'] = not mismatches if bindings else None
            entry['hash_binding_mismatches'] = mismatches
            if mismatches:
                entry['status'] = 'cpp_present_validation_stale'
                registry_issues.append(dict(entry_va=entry['entry_va'], issue='hash_binding_changed', files=mismatches))
            elif not bindings:
                registry_issues.append(dict(entry_va=entry['entry_va'], issue='validation_has_no_source_hash_binding'))
            # A mapping is evidence about an algorithm or selected behavior, not
            # an automatic assertion that an entire original function is equal.
            if entry.get('complete_original_function_equivalence_claimed'):
                raise ValueError('This registry does not accept unscoped complete-function equivalence claims')
            recovered[int(entry['entry_va'], 16)] = entry
    marker_patterns = {
        'undefined_width_type': r'\bundefined(?:[1-8])?\b',
        'extended_float_type': r'\bfloat(?:10|16)\b',
        'width_or_vector_type': r'\b(?:u?int[1-7]|u?longlong|uint|ushort|byte|unkbyte\d+|int\d+x\d+)\b',
        'ghidra_code_type': r'\bcode\b',
        'generated_global': r'\b_?(?:DAT|PTR|BYTE|WORD|DWORD|QWORD|FLOAT|DOUBLE)_[0-9a-fA-F]{8}\b',
        'unresolved_register_value': r'\b(?:unaff|extraout|in)_[A-Za-z][A-Za-z0-9_]*\b',
        'concat_subpiece_helper': r'\b(?:CONCAT\d+|SUB\d+|ZEXT\d+|SEXT\d+)\s*\(',
        'partial_variable_access': r'\b\w+\._\d+_\d+_\b',
        'synthetic_stack_symbol': r'\bstack0x[0-9a-fA-F]+\b',
        'global_label_address': r'\bLAB_[0-9a-fA-F]{8}\b',
        'encoded_template_type': r'\b(?:class|struct|enum)_[A-Za-z_][A-Za-z0-9_:]*',
        'fid_conflict_symbol': r'FID_conflict:',
        'bad_or_unimplemented_marker': r'\b(?:BADSPACEBASE|halt_baddata|halt_unimplemented|unimplemented)\b',
    }
    warning_counter = Counter()
    marker_functions = Counter()
    marker_occurrences = Counter()
    marker_identifiers = defaultdict(Counter)
    roles = Counter()
    statuses = Counter()
    inventory = []
    named_runtime = re.compile(r'(?:std::|struct_std|class_std|Concurrency|__scrt|__acrt|__vcrt|__crt|^_{1,3}(?:Cxx|EH|SEH|security|except|all|aul|ftol|CI)|operator_new|operator_delete|bad_alloc|bad_array_new_length|basic_string|unique_ptr|shared_ptr|_Compressed_pair|_String_val|CDocObjectServer|CWnd|CObject|AFX|__libm|__vdecl|_Mtx|_Cnd|_Thrd|^mem(?:cpy|set|move|cmp)$|^str(?:len|cpy|ncpy|cmp|ncmp|chr)$)')
    entry_re = re.compile(r'/\* Entry: (0x[0-9a-fA-F]+);')
    actual_combined_entries = [int(x,16) for x in entry_re.findall((GHIDRA/'th20_pseudocode.c').read_text(encoding='utf-8'))]
    listed_paths = set()
    for function in functions:
        va = int(function['entry_va'], 16)
        source = GHIDRA / function['pseudocode_file']
        listed_paths.add(source.resolve())
        text = source.read_text(encoding='utf-8') if source.exists() else ''
        warnings = [re.sub(r'\s+', ' ', x).strip() for x in re.findall(r'/\*\s*WARNING:\s*(.*?)\*/', text, flags=re.S)]
        warning_counter.update(warnings)
        stripped = re.sub(r'/\*.*?\*/|//[^\n]*|L?"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'', ' ', text, flags=re.S)
        markers = {}
        for name, pattern in marker_patterns.items():
            hits = re.findall(pattern, stripped)
            if hits:
                markers[name] = dict(occurrences=len(hits), identifiers=sorted(set(hits)))
                marker_functions[name] += 1
                marker_occurrences[name] += len(hits)
                marker_identifiers[name].update(hits)
        head = image.at(va)
        imported = imports.get(struct.unpack_from('<I', head, 2)[0]) if head.startswith(b'\xff\x25') else None
        linked_paths = sorted({x['source_path'] for x in source_links[va]})
        evidence = []
        if imported:
            role, confidence = 'verified_import_jump_thunk', 'machine_bytes_and_import_directory'
            evidence.append('FF 25 absolute jump targets ' + imported['dll'] + '!' + imported['name'])
        elif va in recovered and recovered[va]['status'] == 'validated_compilable_cpp_component':
            role, confidence = 'recovered_engine_component', 'manual_machine_code_validation'
        elif va in recovered and recovered[va]['status'] == 'compiled_algorithm_recovery_resource_validated':
            role, confidence = 'recovered_algorithm_with_new_cpp_api', 'independent_resource_byte_comparison_not_original_abi'
        elif va in recovered and recovered[va]['status'] == 'compiled_scheduler_component_cpu_validated':
            role, confidence = 'recovered_scheduler_library_component', 'selected_cpu_behavior_comparison_with_normalized_pointers'
        elif va in recovered and recovered[va]['status'] == 'compiled_scheduler_partial_or_composed':
            role, confidence = 'partial_recovered_scheduler_composition', 'composed_cpp_with_unrecovered_original_dependencies'
        elif va in recovered and recovered[va]['status'] == 'compiled_control_flow_with_unrecovered_dependencies':
            role, confidence = 'partial_recovered_control_flow', 'static_reconstruction_compiled_library_not_runtime_equivalence'
        elif va in recovered and recovered[va]['status'] == 'compiled_field_or_com_operation_unvalidated':
            role, confidence = 'compiled_field_or_com_operation', 'compilation_and_static_review_only'
        elif va in recovered and recovered[va]['status'] == 'compiled_inline_or_helper_coverage_unvalidated':
            role, confidence = 'compiled_inline_or_helper_coverage', 'source_statement_or_helper_not_separate_original_abi_export'
        elif linked_paths:
            role, confidence = 'game_or_engine_source_diagnostic', 'retained_source_string_reference'
            evidence.extend(linked_paths)
        elif named_runtime.search(function['name'] + ' ' + function['signature']):
            role, confidence = 'possible_crt_stl_or_library_function', 'ghidra_signature_or_library_name_only'
        elif function['thunk'] == 'true':
            role, confidence = 'internal_or_unresolved_thunk', 'ghidra_thunk_flag'
        else:
            role, confidence = 'unclassified_application_or_library', 'unknown'
        status = recovered[va]['status'] if va in recovered else 'pseudocode_only_not_recovered_cpp'
        roles[role] += 1
        statuses[status] += 1
        calls = machine_calls[va]
        inventory.append(dict(entry_va=function['entry_va'], name=function['name'],
            ghidra_signature=function['signature'], ghidra_body_bytes=int(function['body_bytes']),
            ghidra_range_count=int(function['range_count']), ghidra_thunk=function['thunk']=='true',
            role_candidate=role, role_confidence=confidence, role_evidence=evidence,
            recovery_status=status, recovered_cpp=recovered.get(va),
            build_integration_status=recovered[va]['build_integration_status'] if va in recovered else 'not_compiled_from_recovered_cpp',
            retained_source_paths=linked_paths,
            pseudocode_file=function['pseudocode_file'], pseudocode_exists=source.exists(),
            pseudocode_sha256=hash_file(source) if source.exists() else None,
            pseudocode_lines=len(text.splitlines()), warnings=warnings,
            markers=markers, machine_call_counts=dict(Counter(x['kind'] for x in calls)),
            unresolved_indirect_call_sites=[x for x in calls if x['kind']=='unresolved_indirect_call'],
            called_fun_tokens=sorted(set(re.findall(r'\bFUN_([0-9a-fA-F]{8})\s*\(', stripped)) - {f'{va:08x}'})))
    # Separate file/source inventory avoids mistaking tool code and bridges for game recovery.
    module_inventory = []
    for directory, role in [('native_recovered','manually_recovered_game_components'),
            ('src','new_analysis_parsers_not_recovered_runtime'),
            ('incremental/native_bridge','abi_adapters_for_retained_original_engine'),
            ('source_reconstruction','ongoing_source_reconstruction_requires_explicit_evidence')]:
        base = ROOT / directory
        files = [p for p in base.rglob('*') if p.suffix.lower() in {'.cpp','.hpp','.h','.c'}
                 and not any(x in p.parts for x in ('build','audit','CMakeFiles'))] if base.exists() else []
        module_inventory.append(dict(directory=directory, role=role,
            files=[dict(path=str(p.relative_to(ROOT)).replace('\\','/'), sha256=hash_file(p),
                        lines=len(p.read_text(encoding='utf-8',errors='replace').splitlines())) for p in sorted(files)]))
    listed_vas = {int(x['entry_va'],16) for x in functions}
    combined_vas = set(actual_combined_entries)
    orphan_files = sorted(str(p.relative_to(GHIDRA)) for p in (GHIDRA/'pseudocode').glob('*.c') if p.resolve() not in listed_paths)
    recovery_modules = []
    for module in sorted({x.get('module_id', 'unspecified') for x in recovered.values()}):
        entries = [x for x in recovered.values() if x.get('module_id', 'unspecified') == module]
        recovery_modules.append(dict(module_id=module, registered_original_address_mappings=len(entries),
            recovery_status_counts=dict(Counter(x['status'] for x in entries)),
            build_integration_status_counts=dict(Counter(x['build_integration_status'] for x in entries)),
            complete_original_function_equivalence_claimed=False,
            source_binding_current=all(x.get('source_binding_current') is True for x in entries) if module != 'native_core' else native_evidence_current))
    summary = dict(source_executable_sha256=expected_sha,
        scope='All 6928 entries discovered by the existing Ghidra analysis; missed or misidentified functions may exist.',
        inventory_entries=len(inventory), combined_file_entries=len(actual_combined_entries),
        unique_combined_entries=len(combined_vas), missing_from_combined=[hex(v) for v in sorted(listed_vas-combined_vas)],
        extra_combined_entries=[hex(v) for v in sorted(combined_vas-listed_vas)],
        missing_pseudocode_files=[x['pseudocode_file'] for x in inventory if not x['pseudocode_exists']],
        orphan_pseudocode_files=orphan_files,
        ghidra_defined_machine_bytes_union=merged_size([(a,b) for a,b,_ in spans]),
        original_text_virtual_size=json.loads((ROOT/'analysis/binary/pe_summary.json').read_text())['sections'][0]['virtual_size'],
        roles=dict(roles), recovery_status_counts=dict(statuses),
        build_integration_status_counts=dict(Counter(x['build_integration_status'] for x in inventory)),
        recovered_module_registry=recovery_modules, registry_issues=registry_issues,
        complete_independent_game_executable_available=False,
        ghidra_fid_conflict_names=sum('FID_conflict:' in x['name'] for x in inventory),
        native_core_evidence=dict(source_sha256=native_core_sha256,
             cpu_validation_matches_current_source=native_evidence_current),
        additional_recovery_registry='source_reconstruction/audit/additional_recoveries.json' if registry_path.exists() else None,
        registered_entries_not_in_ghidra=[hex(x) for x in recovered if x not in listed_vas],
        validated_component_entries=sorted(f'0x{x:08x}' for x in recovered if recovered[x]['status']=='validated_compilable_cpp_component'),
        independent_algorithm_recovery_entries=sorted(f'0x{x:08x}' for x in recovered if recovered[x]['status']=='compiled_algorithm_recovery_resource_validated'),
        scheduler_component_cpu_covered_entries=sorted(f'0x{x:08x}' for x in recovered if recovered[x]['status']=='compiled_scheduler_component_cpu_validated'),
        partial_or_composed_scheduler_entries=sorted(f'0x{x:08x}' for x in recovered if recovered[x]['status']=='compiled_scheduler_partial_or_composed'),
        control_flow_compilation_only_entries=sorted(f'0x{x:08x}' for x in recovered if recovered[x]['status']=='compiled_control_flow_with_unrecovered_dependencies'),
        field_or_com_compilation_only_entries=sorted(f'0x{x:08x}' for x in recovered if recovered[x]['status']=='compiled_field_or_com_operation_unvalidated'),
        inline_or_helper_compilation_only_entries=sorted(f'0x{x:08x}' for x in recovered if recovered[x]['status']=='compiled_inline_or_helper_coverage_unvalidated'),
        recovered_component_machine_bytes_union=merged_size([span for va in recovered if recovered[va]['status']=='validated_compilable_cpp_component' for span in spans_by_func[va]]),
        warning_files=sum(bool(x['warnings']) for x in inventory), warning_occurrences=sum(warning_counter.values()),
        common_warnings=[dict(warning=x,count=n) for x,n in warning_counter.most_common(30)],
        lexical_markers={name:dict(functions=marker_functions[name], occurrences=marker_occurrences[name],
                                  distinct_identifiers=len(marker_identifiers[name]),
                                  most_common_identifiers=marker_identifiers[name].most_common(20)) for name in marker_patterns},
        functions_with_unresolved_indirect_calls=sum(any(c['kind']=='unresolved_indirect_call' for c in machine_calls[int(x['entry_va'],16)]) for x in inventory),
        unresolved_indirect_machine_call_sites=sum(x['machine_call_counts'].get('unresolved_indirect_call',0) for x in inventory),
        call_sites_not_owned_by_discovered_function=len(unmatched_call_sites),
        module_inventory=module_inventory,
        limitations=['Function and byte counts are inventory measures, not a percentage of game completion.',
                     'compiled_but_unlinked means a standalone library was compiled but is not linked into a complete independently rebuilt game.',
                     'Address mappings, algorithm tests, scoped original-CPU comparisons and original function ABI equivalence are separate claims; none establishes full-game equivalence.',
                     'Retained source paths are diagnostic strings, not recovered source files.',
                     'Library candidate classification can be wrong; Ghidra FID names are not authoritative.',
                     'Call-site evidence uses existing linear disassembly inside discovered function ranges; original analysis can misidentify data as code.',
                     'Lexical unknown-type markers can overcount aliases, and do not enumerate every semantic problem.',
                     'Analysis parsers, original machine-code bridges and script DSL exports are not complete C++ engine recovery.'])
    probe_path = HERE/'compile_probe/result.json'
    if args.probe_compile:
        summary['raw_pseudocode_compile_probe'] = compile_probe(args.compiler)
    elif probe_path.exists():
        summary['raw_pseudocode_compile_probe'] = json.loads(probe_path.read_text(encoding='utf-8'))
    if summary.get('raw_pseudocode_compile_probe'):
        summary['raw_pseudocode_compile_probe']['matches_current_pseudocode'] = summary['raw_pseudocode_compile_probe']['original_source_sha256'] == hash_file(GHIDRA/'th20_pseudocode.c')
    dump(HERE/'coverage.json',summary)
    dump(HERE/'functions.json',inventory)
    dump(HERE/'unowned_call_sites.json',unmatched_call_sites)
    with (HERE/'functions.csv').open('w',encoding='utf-8-sig',newline='') as f:
        writer=csv.writer(f)
        writer.writerow(['entry_va','name','role_candidate','role_confidence','recovery_status','build_integration_status','module_id','ghidra_body_bytes','source_paths','warnings','undefined_type_occurrences','generated_globals','register_values','unresolved_indirect_calls','pseudocode_file'])
        for x in inventory:
            writer.writerow([x['entry_va'],x['name'],x['role_candidate'],x['role_confidence'],x['recovery_status'],x['build_integration_status'],(x['recovered_cpp'] or {}).get('module_id',''),x['ghidra_body_bytes'],
                '|'.join(x['retained_source_paths']),len(x['warnings']),x['markers'].get('undefined_width_type',{}).get('occurrences',0),
                len(x['markers'].get('generated_global',{}).get('identifiers',[])),len(x['markers'].get('unresolved_register_value',{}).get('identifiers',[])),
                x['machine_call_counts'].get('unresolved_indirect_call',0),x['pseudocode_file']])
    known=statuses.get('validated_compilable_cpp_component',0)
    doc=['# C++ 源码恢复覆盖审计','',
         f'本清单包含 Ghidra 当前识别的 **{len(inventory):,} 个入口**，不是已恢复的完整源码，也不保证已发现全部真实函数。',
         f'原有 RNG / Timer 严格组件基线仍为 **{known} 个入口**。独立归档算法登记 **{statuses.get("compiled_algorithm_recovery_resource_validated",0)} 个原地址映射**，调度器有 **{statuses.get("compiled_scheduler_component_cpu_validated",0)} 个限定 CPU 行为覆盖映射**及 **{statuses.get("compiled_scheduler_partial_or_composed",0)} 个组合或部分映射**。这些类别不能加算成完整原函数恢复数量，也不能换算为游戏完成百分比。',
         '目前没有从独立源码链接出的完整游戏。`compiled_but_unlinked` 明确表示独立库已编译，尚未接成完整游戏。','',
         f'入口模块另登记 **{statuses.get("compiled_control_flow_with_unrecovered_dependencies",0)} 个控制流映射**、**{statuses.get("compiled_field_or_com_operation_unvalidated",0)} 个字段/COM 操作映射**及 **{statuses.get("compiled_inline_or_helper_coverage_unvalidated",0)} 个内联语句或 helper 覆盖映射**；只有编译/符号审计证据，不含原 CPU 或玩法等价验证。后两项 helper 覆盖不表示独立 ABI 导出。','',
         '## 编译与证据状态','', '| 模块 | 原地址映射 | 状态 |','| --- | ---: | --- |']
    doc.extend(f'| {x["module_id"]} | {x["registered_original_address_mappings"]} | {", ".join(f"{k}: {v}" for k,v in x["recovery_status_counts"].items())} |' for x in recovery_modules)
    if registry_issues:
        doc.extend(['',f'登记校验发现 **{len(registry_issues)} 项** hash 过期或缺少源码绑定的问题，见 `coverage.json` 的 `registry_issues`。过期记录自动降为 `cpp_present_validation_stale`。'])
    doc.extend(['',
         '## 分类（候选分类，不等于所有权结论）','', '| 分类 | 数量 |','| --- | ---: |']
    )
    doc.extend(f'| {name} | {count:,} |' for name,count in roles.items())
    doc.extend(['','## 直接编译阻碍','',f'- {summary["warning_files"]:,} 个函数文件含反编译警告。',
        f'- {summary["functions_with_unresolved_indirect_calls"]:,} 个函数含未解析动态目标的机器码间接调用，共 {summary["unresolved_indirect_machine_call_sites"]:,} 个调用位置。',
        '- `undefinedN`、未声明全局地址、缺失对象布局、寄存器传参、分段变量访问、模板名称和转换辅助宏需要逐项恢复。补 typedef 或空函数只能掩盖部分语法错误，不能恢复语义。',''])
    if summary.get('raw_pseudocode_compile_probe'):
        p=summary['raw_pseudocode_compile_probe']
        doc.append(f'原样复制全量伪代码后使用 MSVC `/TP /std:c++17 /Zs` 实测，退出码 **{p["exit_code"]}**。没有加入 shim、stub 或替代实现。编译器在错误上限处终止，详见 `compile_probe/compiler_output.txt`；这些诊断不是全部恢复工作量。')
    doc.extend(['','## 与真正可编译模块的区别','',
        '- `native_recovered/native_core.hpp`：七个 RNG / Timer 组件，有明确原地址及验证证据。',
        '- `src/`：新写的 PE / ECL 分析解析器；并非原游戏运行时恢复。',
        '- `incremental/native_bridge/`：调用约定适配代码，依赖保留的原引擎，不满足纯源码要求。',
        '- `source_reconstruction/archive/`：独立 C++ 的解密、文件名参数、持久字典 LZSS 和归档读写算法；285 条资源记录的 152,040,763 字节与独立提取结果相同，不宣称原对象、虚表、分配器或调用约定已重建。',
        '- `source_reconstruction/core_scheduler/`：独立 C++ 的链表、迭代器、回调和帧调度组件。报告为 13,767 次原 CPU 对照及 1 次源码所有权路径检查；不覆盖原分配器、渲染 flush、多线程调度或回调异常展开。',
        '- `source_reconstruction/program_entry/`：WinMain、三个帧节拍控制流、字段和 COM 操作已形成静态库；50 个函数及 13 个全局/环境仍未定义，不能独立链接。22 个函数、194 条静态调用边是分析证据，不能算作 22 个已恢复函数。',
        '- `source_reconstruction/`：本轮其他恢复模块逐步加入；仅源码文件存在不计入已验证覆盖，需明确地址与验证报告登记。',
        '- `analysis/ghidra/`：反编译证据。`scripts/recovered/`：专用脚本语言。均不能当作已经可以编译的完整 C++。','',
        '## 复跑','', '```powershell', 'python source_reconstruction/audit/audit_source.py --probe-compile', '```','',
        '编译探针只生成临时源副本和诊断，不生成可运行游戏。新增恢复映射通过 `additional_recoveries.json` 明确登记，必须包含原地址、模块、证据状态、编译集成状态、源码及验证依据。`source_hash_bindings` 每次复跑与文件逐项核验；hash 不符会降低有效状态，保留原声明供追溯。源码快照绑定与验证报告内的源码绑定在 `hash_binding_origin` 区分。'])
    (HERE/'README.md').write_text('\n'.join(doc)+'\n',encoding='utf-8')
    print(json.dumps({k:summary[k] for k in ['inventory_entries','roles','recovery_status_counts','warning_files','functions_with_unresolved_indirect_calls','unresolved_indirect_machine_call_sites']},indent=2))


if __name__ == '__main__':
    main()
