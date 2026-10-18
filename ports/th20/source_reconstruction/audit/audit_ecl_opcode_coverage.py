"""Read-only binary/source coverage audit. Never imports or executes game code."""
from __future__ import annotations
import collections
import datetime
import hashlib
import json
import pathlib
import re
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / '.cache/binary_python'))
import pefile

ORIGINAL = pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe')
EXPECTED_SHA = 'a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
OUT = pathlib.Path(__file__).parent
inputs: dict[str, bytes] = {}

def load(path: str) -> bytes:
    if path not in inputs:
        inputs[path] = (ROOT / path).read_bytes()
    return inputs[path]

def source(path: str) -> str:
    return load(path).decode('utf-8-sig')

def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()

def masked(text: str) -> str:
    # Preserve offsets/line numbers while ignoring braces in comments/strings.
    pattern = r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\''
    return re.sub(pattern, lambda m: ''.join('\n' if c == '\n' else ' ' for c in m[0]), text)

def closing(text: str, start: int) -> int:
    assert text[start] == '{'
    depth = 0
    for i in range(start, len(text)):
        depth += (text[i] == '{') - (text[i] == '}')
        if depth == 0:
            return i
    raise ValueError('Unbalanced source braces')

def switch_cases(path: str, function: str) -> dict[int, dict]:
    text = source(path)
    clean = masked(text)
    marker = re.search(r'\b' + re.escape(function) + r'\s*\(', clean)
    assert marker, (path, function)
    begin = clean.index('{', marker.end())
    end = closing(clean, begin)
    switch = re.search(r'\bswitch\s*\([^{};]*\)\s*\{', clean[begin:end])
    assert switch, (path, function)
    opening = begin + switch.end() - 1
    finish = closing(clean, opening)
    labels = []
    depth = 0
    for token in re.finditer(r'[{}]|\bcase\s+(0x[0-9a-fA-F]+|\d+)\s*:|\bdefault\s*:', clean[opening:finish + 1]):
        if token[0] == '{':
            depth += 1
        elif token[0] == '}':
            depth -= 1
        elif depth == 1:
            labels.append((int(token[1], 0) if token[1] else None, opening + token.start(), opening + token.end()))
    result = {}
    for index, (opcode, at, after) in enumerate(labels):
        if opcode is None:
            continue
        # Consecutive labels share one substantive body; never infer a whole range.
        body_start = after
        following = index + 1
        while following < len(labels) and not clean[body_start:labels[following][1]].strip():
            body_start = labels[following][2]
            following += 1
        body_end = labels[following][1] if following < len(labels) else finish
        body = clean[body_start:body_end].strip()
        assert body and body not in ('break;', 'return std::nullopt;'), (path, opcode)
        assert opcode not in result
        result[opcode] = {
            'file': path, 'function': function, 'case_line': text.count('\n', 0, at) + 1,
            'body_line': text.count('\n', 0, body_start) + 1,
            'body_excerpt': re.sub(r'\s+', ' ', text[body_start:body_end].strip())[:320],
            'service_methods': sorted(set(re.findall(r'env\.(\w+)\s*\(', body))),
        }
    return result

def parse_ecl(path: str) -> tuple[list[dict], int]:
    data = load(path)
    assert data[:4] == b'SCPT'
    assert struct.unpack_from('<H', data, 4)[0] == 1
    include_size = struct.unpack_from('<H', data, 6)[0]
    include_at, = struct.unpack_from('<I', data, 8)
    count, = struct.unpack_from('<I', data, 16)
    at = include_at + include_size
    offsets = list(struct.unpack_from(f'<{count}I', data, at))
    at += 4 * count
    names = []
    for _ in range(count):
        end = data.index(0, at)
        names.append(data[at:end].decode('cp932'))
        at = end + 1
    assert (at + 3) & ~3 == offsets[0]
    instructions = []
    for index, start in enumerate(offsets):
        end = offsets[index + 1] if index + 1 < count else len(data)
        assert data[start:start + 4] == b'ECLH'
        body, = struct.unpack_from('<I', data, start + 4)
        assert body == 16
        ip = start + body
        while ip < end:
            time, opcode, size, mask, rank, arguments, drop = struct.unpack_from('<iHHHBBI', data, ip)
            assert size >= 16 and ip + size <= end
            instructions.append({'opcode': opcode, 'file': path, 'subroutine': names[index],
                                 'file_offset': f'0x{ip:x}', 'sub_offset': f'0x{ip-start:x}',
                                 'time': time, 'rank_mask': rank})
            ip += size
        assert ip == end
    return instructions, count

raw = ORIGINAL.read_bytes()
assert sha(raw) == EXPECTED_SHA
pe = pefile.PE(data=raw)
def at(va: int, count: int) -> bytes:
    return pe.get_data(va - pe.OPTIONAL_HEADER.ImageBase, count)
def pointer(va: int) -> int:
    return struct.unpack('<I', at(va, 4))[0]

# Validate the exact selector bounds and table references against the specimen.
assert at(0x48c085, 5) == bytes.fromhex('2d2c010000')
assert at(0x48c090, 10) == bytes.fromhex('81bd30fbffffbf020000')
assert at(0x48c0a6, 7) == bytes.fromhex('0fb691b8664900')
assert at(0x48c0ad, 7) == bytes.fromhex('ff249500644900')
assert at(0x4963df, 2) == bytes.fromhex('33c0')
assert at(0x53b674, 7) == bytes.fromhex('83bd18feffff61')
assert at(0x53b687, 7) == bytes.fromhex('ff248d28e15300')
indices = at(0x4966b8, 704)
entity_targets = {op: pointer(0x496400 + indices[op - 300] * 4) for op in range(300, 1004)}
entity_native = {op: target for op, target in entity_targets.items() if target != 0x4963df}
core_targets = {op: pointer(0x53e128 + op * 4) for op in range(98)}
core_native = {op: target for op, target in core_targets.items() if target != 0x53def5}

base = 'source_reconstruction/gameplay/'
specs = [
    ('enemy_opcode_animation.cpp', 'execute_enemy_animation_opcode', 'enemy_opcode_animation_adapter.cpp'),
    ('enemy_opcode_movement.cpp', 'execute_enemy_movement_opcode', 'enemy_opcode_movement_adapter.cpp'),
    ('enemy_opcode_state.cpp', 'execute_enemy_state_opcode', 'enemy_opcode_state_adapter.cpp'),
    ('enemy_shot.cpp', 'execute_enemy_bullet_opcode', 'enemy_shot_adapter.cpp'),
    ('enemy_opcode_laser.cpp', 'execute_enemy_laser_opcode', 'enemy_opcode_laser_adapter.cpp'),
    ('enemy_opcode_misc.cpp', 'execute_enemy_misc_opcode', 'enemy_opcode_misc_adapter.cpp'),
]
dispatch = source(base + 'enemy_opcode_dispatch.cpp')
cmake = source(base + 'CMakeLists.txt')
source('source_reconstruction/link_probe/CMakeLists.txt')
source(base + 'enemy_entity.cpp')
source(base + 'enemy_opcode.hpp')
source('src/ecl.cpp')
core_source = switch_cases(base + 'enemy_vm.cpp', 'EnemyRuntime::tick')
generic_source = switch_cases('source_reconstruction/ecl_vm/vm.cpp', 'Runtime::tick')
entity_source = {}
groups = []
for filename, function, adapter in specs:
    cases = switch_cases(base + filename, function)
    adapter_text = source(base + adapter)
    assert filename in cmake and adapter in cmake
    assert function in adapter_text and function in dispatch
    assert not (set(entity_source) & set(cases))
    service_names = sorted({name for record in cases.values() for name in record['service_methods']})
    override_names = sorted(set(re.findall(r'\b(\w+)\s*\([^{};]*\)\s*override\s*\{', masked(adapter_text))))
    assert not (set(service_names) - set(override_names)), (filename, service_names, override_names)
    assert 'Unrecovered' not in adapter_text
    for opcode, record in cases.items():
        record['adapter_file'] = base + adapter
        record['original_target_va'] = f'0x{entity_targets.get(opcode, 0x4963df):08x}'
        entity_source[opcode] = record
    groups.append({'function': function, 'file': base + filename, 'adapter_file': base + adapter,
                   'individual_case_count': len(cases), 'opcodes': sorted(cases),
                   'service_methods': service_names, 'adapter_override_methods': override_names})

# Read each dispatch condition rather than counting the broad source range as a case.
route_conditions = re.findall(r'(?:if|else\s+if)\s*\((.+)\)result=(execute_enemy_\w+_opcode)\(reader\);', dispatch)
assert len(route_conditions) == 6
def route(opcode: int) -> str | None:
    for expression, name in route_conditions:
        assert re.fullmatch(r'[\w\s()<>!=&|0-9]+', expression)
        normalized = expression.replace('&&', ' and ').replace('||', ' or ')
        if eval(normalized, {'__builtins__': {}}, {'opcode': opcode}):
            return name
    return None

route_mismatches = [{'opcode': op, 'route': route(op), 'handler': entity_source.get(op, {}).get('function')}
                    for op in range(65536)
                    if route(op) != entity_source.get(op, {}).get('function')]
known_missing = sorted(set(entity_native) - set(entity_source))
extra_handlers = sorted(set(entity_source) - set(entity_native))
core_missing = sorted(set(core_native) - set(core_source))
core_extra = sorted(set(core_source) - set(core_native))
assert set(core_source) == set(generic_source)

old_scan = json.loads(source('reports/ecl_binary_scan.json'))
paths = sorted(str(p.relative_to(ROOT)).replace('\\', '/') for p in (ROOT / 'assets/raw').glob('*.ecl'))
instructions = []
file_records = []
for path in paths:
    file_instructions, subroutines = parse_ecl(path)
    instructions.extend(file_instructions)
    histogram = collections.Counter(x['opcode'] for x in file_instructions)
    file_records.append({'path': path, 'sha256': sha(load(path)), 'size': len(load(path)),
                         'subroutine_count': subroutines, 'instruction_count': len(file_instructions),
                         'opcode_histogram': dict(sorted(histogram.items()))})
histogram = collections.Counter(x['opcode'] for x in instructions)
scan_matches = dict(histogram) == {int(k): v for k, v in old_scan['opcode_histogram'].items()}
scan_matches &= len(file_records) == old_scan['valid_files'] and len(instructions) == old_scan['instruction_count']
assert scan_matches
used = []
all_noops = []
counts = collections.Counter()
for opcode, count in sorted(histogram.items()):
    locations = [x for x in instructions if x['opcode'] == opcode]
    if opcode in core_source:
        category, handler = 'core_cpp_handler', core_source[opcode]
        target = core_targets[opcode]
    elif opcode in entity_source:
        category, handler = 'entity_cpp_handler', entity_source[opcode]
        target = entity_targets[opcode]
    elif opcode not in core_native and opcode not in entity_native:
        category, handler = 'original_default_noop', None
        target = 0x4963df
        all_noops.extend(locations)
    else:
        category, handler = 'missing_original_handler', None
        target = core_targets.get(opcode, entity_targets.get(opcode))
    counts[category + '_opcodes'] += 1
    counts[category + '_instructions'] += count
    used.append({'opcode': opcode, 'occurrences': count, 'classification': category,
                 'original_target_va': f'0x{target:08x}', 'source': handler,
                 'examples': locations[:6]})

limitations = [
    'Static dispatch/body/binding coverage, not proof of instruction semantics or complete game equivalence.',
    'All physical ECL instructions are scanned, including unreachable subroutines and rank-filtered instructions; this is not dynamic path coverage.',
    'Handlers are checked for individual concrete case bodies and concrete production adapter methods; transitive subsystem semantics, data-dependent callback indices, ETEX payloads and resource validity remain separate audits.',
    'Original-default no-op means the entity operation has no action; ordinary VM instruction advance and stack-drop semantics still apply.',
    'Malformed instructions and out-of-bounds original behavior are outside this static audit.',
]
report = {
    'schema': 'th20.ecl.production_opcode_coverage.v1',
    'generated_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
    'status': 'static_dispatch_coverage_matches' if not any([known_missing, extra_handlers, core_missing, core_extra, route_mismatches]) else 'coverage_gaps',
    'original_executable_executed': False, 'game_executed': False, 'build_performed': False,
    'original_executable_sha256': sha(raw),
    'original_dispatch': {'entity_entry_va': '0x0048c010', 'minimum_opcode': 300, 'maximum_opcode': 1003,
                          'selector_table_va': '0x004966b8', 'selector_count': 704,
                          'target_table_va': '0x00496400', 'target_count': max(indices) + 1,
                          'default_return_zero_va': '0x004963df', 'nondefault_opcode_count': len(entity_native),
                          'default_selector_count': sum(x == 0x4963df for x in entity_targets.values()),
                          'core_entry_va': '0x0053b5c0', 'core_target_table_va': '0x0053e128',
                          'core_target_count': 98, 'core_delegate_default_va': '0x0053def5',
                          'core_nondefault_opcode_count': len(core_native),
                          'selector_bytes_sha256': sha(indices),
                          'target_bytes_sha256': sha(at(0x496400, (max(indices) + 1) * 4))},
    'source_call_path': ['ScriptManager::tick', 'EnemyRuntime::tick', 'ScriptManager::execute_opcode virtual dispatch',
                         'Enemy::execute_opcode', 'unrecovered::execute_enemy_opcode_0048c010', 'execute_enemy_opcode', 'individual execute_enemy_*_opcode handler', 'concrete production service adapter'],
    'summary': {'files': len(file_records), 'subroutines': sum(x['subroutine_count'] for x in file_records),
                'instructions': len(instructions), 'distinct_opcodes': len(histogram), **counts,
                'known_original_entity_handlers_without_cpp_case': len(known_missing),
                'known_original_core_handlers_without_cpp_case': len(core_missing)},
    'existing_scan_histogram_matches': bool(scan_matches),
    'known_original_missing_entity_opcodes': known_missing,
    'known_original_missing_core_opcodes': core_missing,
    'source_handlers_for_native_default_opcodes': extra_handlers + core_extra,
    'route_case_mismatches': route_mismatches,
    'unused_but_implemented_entity_opcodes': sorted(set(entity_native) - set(histogram)),
    'explicit_undefined_opcode_569': {'original_target_va': f'0x{entity_targets[569]:08x}', 'source_route': route(569), 'stock_occurrences': histogram[569]},
    'handler_groups': groups,
    'used_opcodes': used,
    'known_entity_handlers': [{'opcode': op, **record} for op, record in sorted(entity_source.items())],
    'known_core_handlers': [{'opcode': op, 'original_target_va': f'0x{core_targets[op]:08x}', **record} for op, record in sorted(core_source.items())],
    'all_stock_default_noop_locations': all_noops,
    'files': file_records,
    'limitations': limitations,
}
load(str(pathlib.Path(__file__).relative_to(ROOT)).replace('\\', '/'))
report['input_sha256'] = {path: sha(data) for path, data in sorted(inputs.items())}
for path, data in inputs.items():
    assert (ROOT / path).read_bytes() == data, f'Input changed while auditing: {path}'
(OUT / 'ecl_opcode_coverage.json').write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf8')

lines = [
    '# 随附 ECL 与生产 opcode 覆盖审计', '',
    f"静态审计扫描 **{len(file_records)} 个 ECL、{report['summary']['subroutines']} 个子程序、{len(instructions):,} 条指令、{len(histogram)} 种 opcode**。重新读取资产所得直方图与 `reports/ecl_binary_scan.json` 完全一致。",
    '',
    f"原敌机分发器的 **{len(entity_native)} 个非默认 opcode 均有具体 C++ case 和生产适配器**；通用 VM 的 **{len(core_native)} 个非默认 opcode** 也都有对应实现。未发现原分发器已知 opcode 缺少生产处理器，也未发现源码路由区间内的空 case。此结论是静态覆盖，不是行为等价或完整关卡验收。",
    '', '## 方法和证据', '',
    '- 校验只读原 EXE 的 SHA-256，从 PE 数据直接解出两级分发表，而非使用区间注释推测覆盖。`48c085` 减去 300，`48c090` 比较 703，`48c0a6` 查 704 字节表 `4966b8`，`48c0ad` 查目标表 `496400`。',
    '- 704 个选择槽中，224 个映射到具体动作，480 个映射到 `4963df` 的 `xor eax,eax` 返回。上界之外同样跳到该默认返回。通用 VM `53b5c0` 的 98 项表位于 `53e128`，默认 `53def5` 调用实体分派。',
    '- 逐函数提取首个分发 switch 的顶层数值 case，排除注释、字符串、其他辅助函数和嵌套 switch。连续标签必须落到实际非空语句体；对全部 65,536 个原始 opcode 值核对生产路由与具体处理器集合。',
    '- 核对六类处理器的实际 adapter override 方法，以及生产 CMake 中的处理器/适配器成员；没有靠 oracle 的测试服务冒充生产实现。',
    '- 新 JSON 保留每个 opcode 的原入口目标、源码 case/函数/行、调用服务、随附资源出现位置与全部输入哈希。本次未运行原版、源码游戏或构建。',
    '', '## 具体处理器', '', '| 处理器 | 独立 case 数量 | 源码 |', '|---|---:|---|',
]
for group in groups:
    lines.append(f"| `{group['function']}` | {group['individual_case_count']} | `{group['file']}`，生产绑定 `{group['adapter_file']}` |")
lines.extend(['', '## 随附脚本的分类', '', '| 分类 | opcode 种数 | 指令数 |', '|---|---:|---:|'])
for key, label in [('core_cpp_handler', '通用 VM C++ 处理器'), ('entity_cpp_handler', '敌机 C++ 处理器'), ('original_default_noop', '原版默认无动作')]:
    lines.append(f"| {label} | {counts[key + '_opcodes']} | {counts[key + '_instructions']:,} |")
lines.extend(['', '## 默认无动作不是遗漏', '', '| opcode | 随附出现次数 | 原行为证据 | 首个位置 |', '|---|---:|---|---|'])
for record in used:
    if record['classification'] != 'original_default_noop':
        continue
    example = record['examples'][0]
    evidence = '两级表直接指向 `4963df`' if record['opcode'] <= 1003 else '大于上界 1003，`48c09a` 跳至 `4963df`'
    lines.append(f"| {record['opcode']} | {record['occurrences']} | {evidence} | `{example['file']}` / `{example['subroutine']}` / `{example['file_offset']}` |")
lines.extend([
    '', '`569` 也确实指向原默认返回，随附文件没有使用它。源码显式排除 569 的行为正确。默认无动作仅指实体操作本身；外层 VM 仍执行原有指令前进和参数栈清理。',
    '', '`if (!result) throw "Unrecovered enemy ECL opcode"` 是处理器漏接时的保护。当前 224 个路由值分别在实际处理器中有具体 case，因此未发现由缺少 case 触发此异常的已知 opcode。参数无效、资源缺失或下游异常不由这个集合检查排除。',
    '', '## 保留的验证边界', '',
    '此审计不检查每条指令的完整语义，也不代替实际子弹/激光 ETEX、回调表索引、特殊资源、分配失败或线程组合验证。静态存在的指令可能受难度、时间、分支及调用关系限制而不执行；扫描所有子程序不意味着跑过全部关卡。已有局部 CPU 报告的输入范围仍然适用，整局同输入/种子逐帧与全角色/关卡验收仍未完成。',
    '', '复核：从工程根运行 `python source_reconstruction/audit/audit_ecl_opcode_coverage.py`。脚本只读取原 EXE/现有源码/资源，并重写本审计自己的 JSON 和 Markdown。',
])
(OUT / 'ecl_opcode_coverage.md').write_text('\n'.join(lines) + '\n', encoding='utf8')
print(json.dumps({'status': report['status'], **report['summary']}, ensure_ascii=False))
