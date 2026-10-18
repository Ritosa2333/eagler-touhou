"""Static stock-ECL/production-source audit; never builds or executes game code."""
from __future__ import annotations
import collections
import datetime
import hashlib
import json
import pathlib
import re
import struct

ROOT = pathlib.Path(__file__).resolve().parents[2]
OUT = pathlib.Path(__file__).parent
inputs = {}


def load(path):
    if path not in inputs:
        inputs[path] = (ROOT / path).read_bytes()
    return inputs[path]


def text(path):
    return load(path).decode('utf-8-sig')


def signed(value):
    return value if value < 0x80000000 else value - 0x100000000


def parse(path):
    data = load(path)
    assert data[:4] == b'SCPT'
    include_size = struct.unpack_from('<H', data, 6)[0]
    include_at = struct.unpack_from('<I', data, 8)[0]
    count = struct.unpack_from('<I', data, 16)[0]
    at = include_at + include_size
    offsets = struct.unpack_from(f'<{count}I', data, at)
    at += count * 4
    names = []
    for _ in range(count):
        end = data.index(0, at)
        names.append(data[at:end].decode('cp932'))
        at = end + 1
    assert (at + 3) & ~3 == offsets[0]
    result = []
    for index, start in enumerate(offsets):
        end = offsets[index + 1] if index + 1 < count else len(data)
        assert data[start:start + 4] == b'ECLH'
        assert struct.unpack_from('<I', data, start + 4)[0] == 16
        ip = start + 16
        instructions = []
        while ip < end:
            time, opcode, size, mask, rank, argc, drop = struct.unpack_from('<iHHHBBI', data, ip)
            assert size >= 16 and ip + size <= end and (size - 16) % 4 == 0
            args = list(struct.unpack_from(f'<{(size - 16) // 4}I', data, ip + 16))
            instructions.append(dict(file=path, subroutine=names[index], file_offset=f'0x{ip:x}',
                                     instruction_offset=ip - start - 16, opcode=opcode, size=size,
                                     time=time, rank_mask=rank, parameter_mask=mask, arguments_u32=args))
            ip += size
        assert ip == end
        result.append(instructions)
    return result


def site(ins):
    keys = ('file', 'subroutine', 'file_offset', 'instruction_offset', 'opcode', 'time', 'rank_mask',
            'parameter_mask', 'arguments_u32')
    result = {k: ins[k] for k in keys}
    dsl = 'scripts/recovered/ecl/' + pathlib.Path(ins['file']).name + '.txt'
    lines = text(dsl).splitlines()
    begin = next(i for i, line in enumerate(lines) if re.match(r'void ' + re.escape(ins['subroutine']) + r'\(', line))
    end = next((i for i in range(begin + 1, len(lines)) if lines[i].startswith('void ')), len(lines))
    matches = [(i + 1, lines[i].strip()) for i in range(begin, end)
               if re.search(r'\bins_' + str(ins['opcode']) + r'\(', lines[i])]
    siblings = [i for i in by_sub[ins['file'], ins['subroutine']] if i['opcode'] == ins['opcode']]
    assert len(matches) == len(siblings), (ins, len(matches), len(siblings))
    line, content = matches[siblings.index(ins)]
    result.update(dsl_file=dsl, dsl_line=line, dsl_text=content)
    return result


def cfg(instructions):
    by_ip = {i['instruction_offset']: n for n, i in enumerate(instructions)}
    successors = {}
    for n, ins in enumerate(instructions):
        op = ins['opcode']
        dest = [] if op in (1, 10) or n + 1 == len(instructions) else [n + 1]
        if op in (12, 13, 14):
            target = by_ip[ins['instruction_offset'] + signed(ins['arguments_u32'][0])]
            dest = [target] if op == 12 else sorted(set(dest + [target]))
        successors[n] = dest
    reachable, pending = set(), [0]
    while pending:
        n = pending.pop()
        if n not in reachable:
            reachable.add(n)
            pending.extend(successors[n])
    predecessors = {n: set() for n in reachable}
    for n in reachable:
        for target in successors[n]:
            predecessors[target].add(n)
    dom = {n: ({0} if n == 0 else set(reachable)) for n in reachable}
    changed = True
    while changed:
        changed = False
        for n in sorted(reachable - {0}):
            value = {n} | set.intersection(*(dom[p] for p in predecessors[n]))
            if value != dom[n]:
                dom[n], changed = value, True
    return successors, reachable, dom


scan = json.loads(load('reports/ecl_binary_scan.json'))
files = sorted(ROOT.joinpath('assets/raw').glob('*.ecl'))
assert {f.name for f in files} == {f['path'] for f in scan['files']}
subroutines = [sub for file in files for sub in parse(file.relative_to(ROOT).as_posix())]
by_sub = {(sub[0]['file'], sub[0]['subroutine']): sub for sub in subroutines}
instructions = [i for sub in subroutines for i in sub]
histogram = collections.Counter(i['opcode'] for i in instructions)
assert dict(histogram) == {int(k): v for k, v in scan['opcode_histogram'].items()}
assert len(instructions) == scan['instruction_count'] and len(subroutines) == scan['subroutine_count']
setters = [i for i in instructions if i['opcode'] in (609, 610, 611, 612)]
code_arg = lambda i: 2 if i['opcode'] >= 611 else 3
dynamic_codes = [i for i in setters if i['parameter_mask'] & (1 << code_arg(i))]
assert not dynamic_codes
writers = [i for i in setters if i['arguments_u32'][code_arg(i)] == 13]
creators = [i for i in instructions if i['opcode'] == 711]
assert len(writers) == 4 and len(creators) == 2

writer_records = []
for writer in writers:
    sub = by_sub[writer['file'], writer['subroutine']]
    index, cursor, reset = None, None, None
    for ins in sub[:sub.index(writer) + 1]:
        if ins['opcode'] == 600 and ins['arguments_u32'][0] == 0:
            cursor, reset = 0, ins
        elif ins['opcode'] in (609, 610, 611, 612):
            assert ins['arguments_u32'][0] == 0 and not ins['parameter_mask'] & 1
            index = cursor if ins['opcode'] >= 611 else ins['arguments_u32'][1]
            assert index is not None
            cursor = index + 1
    assert reset is not None and not writer['parameter_mask']
    r = site(writer)
    r.update(queue=0, command_index=index, command_index_method='literal' if writer['opcode'] < 611 else 'straight-line append cursor from reset',
             reset=site(reset), ordinary_bullet_fires=[site(i) for i in sub if i['opcode'] == 601],
             type2_creations_in_same_subroutine=[site(i) for i in sub if i['opcode'] == 711],
             classification='ordinary-bullet queue setup; no Type2 creator in this subroutine')
    if writer['opcode'] >= 611:
        # Cursor proof only uses the straight-line prefix from reset through this writer.
        assert not any(i['opcode'] in range(10, 25) or i['opcode'] in (614, 633)
                       for i in sub[sub.index(reset):sub.index(writer)])
    writer_records.append(r)

creator_records = []
for creator in creators:
    sub = by_sub[creator['file'], creator['subroutine']]
    successors, reachable, dom = cfg(sub)
    assert creator['parameter_mask'] == 0 and creator['arguments_u32'] == [0]
    assert {i['rank_mask'] for i in sub} == {255} and {i['time'] for i in sub} == {0}
    assert all(i['arguments_u32'][1] == 0 for i in sub if i['opcode'] in (12, 13, 14))
    # Exhaustive whitelist for these two bodies: ordinary VM arithmetic/control and inspected queue fields.
    assert all(i['opcode'] in {10, 12, 14, 40, 42, 43, 44, 45, 50, 51, 53, 55, 57, 78, 82,
                              600, 602, 604, 605, 608, 609, 620, 700, 701, 711} for i in sub)
    assert not any(i['opcode'] in {11, 15, 16, 17, 18, 19, 20, 21, 23, 24, 614, 633} for i in sub)
    reset_indices = [n for n, i in enumerate(sub) if i['opcode'] == 600]
    assert len(reset_indices) == 1
    reset_n = reset_indices[0]
    assert sub[reset_n]['arguments_u32'] == [0] and sub[reset_n]['parameter_mask'] == 0
    spawn_n = sub.index(creator)
    assert reset_n in dom[spawn_n]
    commands = [0, 0]
    writes = []
    for n, ins in enumerate(sub):
        if ins['opcode'] != 609:
            continue
        a = ins['arguments_u32']
        assert not ins['parameter_mask'] & 0b1011 and a[0] == 0
        assert reset_n in dom[n] and n in dom[spawn_n]
        while len(commands) <= a[1]:
            commands.append(0)
        commands[a[1]] = a[3]
        writes.append(site(ins))
    assert 13 not in commands and set(commands) == {3, 7}
    callers = []
    for ins in instructions:
        if ins['opcode'] not in (11, 15, 16):
            continue
        raw = load(ins['file'])
        at = int(ins['file_offset'], 16)
        length = ins['arguments_u32'][0]
        callee = raw[at + 20:at + 20 + length].split(b'\0', 1)[0].decode('cp932')
        if callee == creator['subroutine']:
            callers.append({k: ins[k] for k in ('file', 'subroutine', 'file_offset', 'opcode')})
    r = site(creator)
    r.update(queue=0, commands_at_every_creation=commands, command_writes=writes, reset=site(sub[reset_n]),
             reset_dominates_creation=True, every_command_write_dominates_creation=True,
             instruction_count=len(sub), reachable_instruction_count=len(reachable), all_times=0, all_rank_masks=255,
             calls_or_yields_or_queue_copies_in_subroutine=[], direct_literal_callers=callers,
             cfg_edges=[{'from': sub[n]['file_offset'], 'to': [sub[v]['file_offset'] for v in dest]} for n, dest in successors.items()],
             conclusion='ETEX13 excluded at every reachable creation in this body, independently of incoming queue contents')
    creator_records.append(r)

evidence_specs = [
    ('gameplay/enemy_shot.cpp', 'case 600:', 'ECL600 replaces metadata with fresh defaults and resets the append cursor; prior/aliased commands are discarded.'),
    ('bullet_system/metadata.cpp', 'ShotMetadata::ShotMetadata()', 'Fresh metadata has two value-initialized zero command records.'),
    ('bullet_system/command.hpp', 'std::uint32_t words[11]{};', 'Command words are initially zero.'),
    ('gameplay/enemy_shot.cpp', 'case 609:case 610:', 'Only the mapped ETEX argument is written to words[8]; append and explicit index setters differ.'),
    ('gameplay/enemy_shot.cpp', 'case 602:', 'The third argument of ECL602 sets color; a literal 13 here is not ETEX13.'),
    ('gameplay/enemy_shot.cpp', 'case 614:', 'Queue copy exists globally but is absent between reset and both Type2 creation sites.'),
    ('gameplay/enemy_opcode_laser.cpp', 'case 711:', 'ECL711 copies queued commands by value into Type2Parameters and creates kind 2.'),
    ('gameplay/enemy_opcode_laser_adapter.cpp', 'void create(', 'Production adapter calls spawn_laser.'),
    ('laser_system/spawn.cpp', 'case 2:', 'Kind 2 dispatch calls spawn_type2.'),
    ('laser_system/type2.cpp', 'parameters=input;', 'Type2 owns a copy; subsequent queue writes cannot inject ETEX13 into an existing curve.'),
    ('laser_system/type2.cpp', 'command_index=99;', 'Curve splitting clones path and disables command replay in the child.'),
    ('laser_system/type2_cancellation.cpp', 'Type2Parameters p=parameters;', 'The only additional Type2 spawn path copies existing Type2 parameters during splitting.'),
    ('laser_system/type2_commands.cpp', 'case 13:reemit(op);', 'The exceptional reemit path requires command.words[8] == 13.'),
    ('laser_system/type2_commands.cpp', 'case 6:', 'Command mutation decrements words[4], not words[8]; existing 3/7 codes do not synthesize 13.'),
    ('bullet_system/laser_spawn.cpp', 'if(op.words[4]==0)', 'Bullet ETEX27 supports Type0 and Type1 only; it cannot turn a bullet ETEX13 list into Type2.'),
    ('gameplay/enemy_vm.cpp', 'target.instruction_offset=0;', 'Synchronous/async subroutine calls enter at offset zero, before the dominating reset.'),
    ('gameplay/enemy_vm.cpp', 'if(!(static_cast<float>(ins.time())<=time))', 'Interpreter yields at a future instruction time; both inspected bodies have time 0 and no waits.'),
    ('gameplay/enemy_vm.cpp', 'std::int32_t ScriptManager::tick(', 'Asynchronous runtimes run sequentially; there is no instruction-level preemption in a body.'),
]
evidence = []
for path, marker, meaning in evidence_specs:
    path = 'source_reconstruction/' + path
    lines = text(path).splitlines()
    line = next(n for n, value in enumerate(lines, 1) if marker in value)
    evidence.append(dict(file=path, line=line, excerpt=lines[line - 1].strip(), meaning=meaning))
text('source_reconstruction/laser_system/type2_reemit.cpp')
text('source_reconstruction/laser_system/TYPE2_ETEX13.md')
text('source_reconstruction/laser_system/type2.hpp')
text('source_reconstruction/audit/audit_type2_etex13_reachability.py')

limits = [
    'This is a static result for the hashed 21 supplied ECL resources and inspected production implementation, not an executed whole-game replay or proof that all recovered native semantics are equivalent.',
    'Cross-OS-thread writes, reentrant external callbacks and whole-program global aliases were not independently audited against the native program. Their absence is an assumption, not a proved global non-reachability result; unknown behavior outside the inspected sequential ECL execution is retained as unknown.',
    'The conclusion assumes normal subroutine entry at offset zero, valid VM rank/state, sequential runtime scheduling and memory-safe state. It does not model corrupt pointers, arbitrary instruction-pointer injection, external modified scripts or another program version.',
    'The CFG overapproximates both conditional branches and does not prove which difficulties, phases or loops execute. A writer/fire site is a syntactic candidate, not proof that a bullet reaches its ETEX13 at runtime.',
    'Dynamic angles, counts and other values remain unresolved. All 683 command-type arguments are literal; both Type2 queue operands, their reset queues and their command indices/types are literal. Incoming queues and cross-subroutine state may be arbitrary because the dominating reset discards them.',
    'The original Type2 ETEX13 out-of-bounds-copy limitation remains real for synthetic or modified inputs. This audit does not change that code, execute the invalid copy or count it as an oracle pass.',
]
report = dict(schema='th20.type2.etex13.static_reachability.v1', generated_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
              method='Raw SCPT/ECLH decode + literal parameter-mask audit + intra-subroutine CFG dominators + production source review; no build or game execution.',
              coverage=dict(files=len(files), subroutines=len(subroutines), instructions=len(instructions),
                            exact_histogram_matches_existing_scan=True, etex_setters=len(setters),
                            etex_setter_opcodes=dict(sorted(collections.Counter(i['opcode'] for i in setters).items())),
                            dynamic_command_type_arguments=len(dynamic_codes), literal_etex13_sites=len(writers), type2_creator_sites=len(creators)),
              result='Both supplied ECL Type2 creation bodies locally exclude ETEX13 through dominating reset and literal command writes; whole-native-program/global reachability is not proved.',
              etex13_writers=writer_records, type2_creators=creator_records, source_evidence=evidence, limitations=limits,
              input_sha256={p: hashlib.sha256(v).hexdigest() for p, v in sorted(inputs.items())})
(OUT / 'type2_etex13_reachability.json').write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')

md = ['# Type2 ETEX13：随附脚本静态可达性审计', '',
      '结论：随附 21 个 ECL 中找到 4 处 ETEX13 设置、2 处 Type2 创建。两个创建子程序的局部控制流均先执行必经的 queue 0 重置，再只写入 ETEX7/3，因此局部流入 ETEX13 已排除。进入重置前的队列内容可以任意；本报告不宣称整个原生程序全局不可达。', '',
      '这是有条件的静态排除，不能当作全游戏逐帧一致性验收，也不能消除 `laser_system/TYPE2_ETEX13.md` 所记录的原始越界行为差异。未运行游戏、原版函数或构建程序；生产文件未修改。', '',
      '## 范围与可重跑方法', '',
      '在工程根目录执行 `python source_reconstruction/audit/audit_type2_etex13_reachability.py`。仅写本审计 JSON/Markdown。JSON 记录全部输入 SHA-256、二进制偏移、原始参数、DSL 行号、CFG 边和源码证据。', '',
      f'- 重新解析 {len(files)} 文件、{len(subroutines)} 子程序、{len(instructions)} 条指令；opcode 直方图与既有 `reports/ecl_binary_scan.json` 完全相同。',
      '- 609/610/611/612 共 683 处 ETEX 设置（分别 128/32/515/8）。按 `enemy_shot.cpp` 的参数布局读取命令类型，并检查 parameter mask：命令类型参数动态引用数为 **0**。',
      '- 609/610 的 ETEX 编号在参数 3，611/612 的编号在参数 2（均从 0 开始计数）。仅搜索文本中的数字 13 会误报颜色等参数。', '',
      '## 全部 ETEX13 设置位置', '',
      '| ECL / 子程序 | 二进制偏移 | DSL 行 | 指令 / queue / ETEX槽 | 同子程序普通弹发射 |',
      '|---|---|---:|---|---|']
for r in writer_records:
    fires = ', '.join(f"{i['file_offset']} (行 {i['dsl_line']})" for i in r['ordinary_bullet_fires'])
    md.append(f"| {pathlib.Path(r['file']).name} / `{r['subroutine']}` | `{r['file_offset']}` | {r['dsl_line']} | {r['opcode']} / 0 / {r['command_index']} | ECL601 {fires} |")
md += ['', '上述 4 处实际进入普通弹的配置队列；这里只证明存在 ECL601 消费位置，未声称每次实玩都会走到该 ETEX。它们各自子程序没有 ECL711。第 1 处的槽 2 来自重置后连续 append；其余槽号直接为常量。完整命令如下：', '', '```text']
md += [f"{pathlib.Path(r['file']).name}:{r['dsl_line']} {r['dsl_text']}" for r in writer_records]
md += ['```', '', '## 两个 Type2 创建点的排除证据', '',
       '对每个子程序从入口建立 CFG：ECL12 是无条件跳转，13/14 同时保留两条分支，1/10 退出。固定点计算支配集合；重置与每一处 ETEX 写入都必须支配 ECL711。条件分支过近似，因此不靠具体随机数、难度或循环次数证明。', '']
for r in creator_records:
    md += [f"### {pathlib.Path(r['file']).name} / `{r['subroutine']}`", '',
           f"- ECL711 位于 `{r['file_offset']}` / DSL 行 {r['dsl_line']}，queue 为常量 0；必经重置位于 `{r['reset']['file_offset']}` / 行 {r['reset']['dsl_line']}。",
           f"- 每次创建时的完整命令类型向量为 `{r['commands_at_every_creation']}`；所有 ETEX 写入均支配创建点，参数中的 queue、槽号、类型均非动态引用。",
           f"- 子程序共 {r['instruction_count']} 条指令，时间戳全部为 0，rank mask 全部为 255，跳转的目标时间全部为 0。无同步/异步调用、等待、queue copy 或 cursor 回退指令。故重置和创建之间不会把执行权让给其他 ECL runtime。",
           '- 创建后 Type2 保存命令向量的值拷贝，之后在别的子程序里改队列也不会修改该曲线的命令。', '']
md += ['`st07mbs.ecl` 确实同时包含 ETEX13 和 Type2：`MBossCard1_at` 的 ETEX13 可先写 queue 0，但 `MBossCard2_at` 的必经 ECL600 会丢弃它。不能因为它们位于同一个文件就判定曲线 reemit 可达。后者 `ins_602(0, 0, 13)` 的 13 是颜色，不是 ETEX 编号。', '',
       '## 跨子程序、异步与其他生成路径', '',
       '- 已检查的生产源码中，同步/异步调用从子程序 offset 0 开始；两个创建子程序内部没有调用或等待，ScriptManager 逐个执行 ECL runtime。因此在此顺序执行模型下，重置到创建之间无其他 ECL runtime 写入。原生程序跨 OS 线程写入、外部回调重入及全局别名没有单独完成全面审计，保留为未知；不能把源码顺序执行观察升级为全原生程序不可达证明。',
       '- ECL614 确有复制队列的能力，但两个创建子程序均没有该指令。无需把全局所有队列别名求解完毕：进入 Type2 设置区间时队列内容可以视为任意值，必经重置仍会覆盖。',
       '- 生产创建链是 ECL711 → `execute_enemy_laser_opcode` → adapter `create(kind=2)` → `spawn_laser` → `spawn_type2`。已检查的其他 Type2 生成路径只有现有曲线的 split；它复制原有参数，并在 path 分支将 command_index 置 99，不会添加 ETEX13。',
       '- Type2 的 ETEX3/7 只建立轨迹段/写字段，不改命令类型。解释器里 ETEX6 只修改 words[4]，ETEX16 只改索引，也不会凭空合成 words[8]=13。普通弹 ETEX27 只支持 Type0/Type1，不能将这些普通弹 ETEX13 配置转成 Type2。', '',
       '## 生产源码证据', '', '| 文件:行 | 所支持的事实 |', '|---|---|']
md += [f"| `{e['file']}:{e['line']}` | {e['meaning']} |" for e in evidence]
md += ['', '## 限制', ''] + ['- ' + value for value in limits]
md += ['', '没有改动或弱化原 `TYPE2_ETEX13.md` 的行为边界，也没有把原始 0x420 越界复制算入成功 CPU 对照；本报告仅补齐“随附脚本是否有正常静态流入点”这一问题。', '']
(OUT / 'type2_etex13_reachability.md').write_text('\n'.join(md), encoding='utf-8')
print(json.dumps(report['coverage'], ensure_ascii=False))
print(report['result'])
