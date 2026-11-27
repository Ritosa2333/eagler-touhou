#!/usr/bin/env python3
"""对拍原版与移植版的绘制调用流。

原版侧：game-oracle.py --draw 录下的 D3D 调用流（d3d-recorder.py，JSON Lines，每帧一行 {"frame", "prefix"?, "calls"}）。
移植版侧：harness `game-draw` 录下的 RecordingDevice 调用流（每帧一行 {"frame", "ops"}）。

比较口径是"每次 Clear / 绘制时生效的状态"，不比较冗余的 SetRenderState 调用次数与顺序：
原版流先按 D3D 语义折叠成 (目标, 视口, 纹理, 状态, 矩阵) 的生效快照，再与移植版逐条比对——
目标 / 视口 / 清屏参数 / 图元类型与数量 / 顶点格式 / 顶点字（float 允许 --eps 误差）/ 渲染状态子集 /
世界格式的 world·view·projection（与纹理矩阵）。

用法：
    draw-compare.py <oracle.jsonl> <port.jsonl> [--frames a-b] [--eps 1e-4] [--max-report 20] [--ignore-state k1,k2]
    draw-compare.py <oracle.jsonl> - --structure-golden <out.json> [--frames a-b] [--only-textures 7]
        只用原版流：把每帧绘制操作的"结构"（目标 / 视口 / 图元 / 格式 / 纹理 / 状态，不含顶点与矩阵）摘要成 sha1，
        写成 tests/golden 用的 JSON；tests/draw.test.mjs 用同一口径（structure_lines）对移植版复算。
        --only-textures 把摘要限定在用这些 AnmFile 槽贴图的绘制操作（Clear 一并丢弃），并把槽号列表写进 golden 的
        texture_slots，供在其它子系统尚未对齐的场景里单独守住一个管理器（如 7 = bullet.anm：弹幕与激光）
    draw-compare.py <raw.jsonl> - --digest <out.jsonl>
        整段摘要：原始流（原版或移植侧格式）→ 每帧一行 {"frame","ops","s","v","a","b"}（口径见 frame_digests）。
        原版长程用 game-oracle.py --draw --draw-digest 直接在回放里出摘要，不必落原始流
    draw-compare.py <oracle.digest.jsonl> <port.digest.jsonl> --compare-digests [--segment 600] [--golden-segments out.json]
        逐帧比摘要：结构相同且顶点在量化网格 a 或 b 之一同格即通过；通过的帧按网格折成段，
        --golden-segments 写出 tests/golden/draw-long.json 用的段表（tests/draw-long.test.mjs 对移植侧复算）
退出码：0 全部一致；1 有差异；2 输入问题。
"""
import argparse
import hashlib
import json
import math
import struct
import sys

# ---- D3D 常量 → 移植版 PipelineState 枚举 ----
RS = {'ZENABLE': 7, 'ZWRITEENABLE': 14, 'ALPHATESTENABLE': 15, 'SRCBLEND': 19, 'DESTBLEND': 20, 'CULLMODE': 22,
      'ZFUNC': 23, 'ALPHAREF': 24, 'ALPHAFUNC': 25, 'ALPHABLENDENABLE': 27, 'FOGENABLE': 28, 'FOGCOLOR': 34,
      'FOGSTART': 36, 'FOGEND': 37, 'TEXTUREFACTOR': 60, 'BLENDOP': 171}
TSS = {'COLOROP': 1, 'COLORARG1': 2, 'COLORARG2': 3, 'ALPHAOP': 4, 'ALPHAARG1': 5, 'ALPHAARG2': 6, 'TEXTURETRANSFORMFLAGS': 24}
SAMP = {'ADDRESSU': 1, 'ADDRESSV': 2, 'MAGFILTER': 5, 'MINFILTER': 6}
D3DTA_TO_SOURCE = {0: 0, 2: 1, 3: 2, 4: 3}          # DIFFUSE / TEXTURE / TFACTOR / SPECULAR
D3DTEXF_TO_FILTER = {1: 0, 2: 1}                      # POINT / LINEAR
D3DCULL_TO_CULL = {1: 0, 2: 1, 3: 2}
FVF_TO_LAYOUT = {0x102: 'WorldUv', 0x142: 'WorldColorUv', 0x144: 'ScreenColorUv', 0x44: 'ScreenColor', 0x104: 'ScreenUv'}
TOPOLOGY = {1: 'points', 2: 'lines', 3: 'linestrip', 4: 'triangles', 5: 'strip', 6: 'fan'}
WORLD_LAYOUTS = ('WorldUv', 'WorldColorUv')
# 顶点字中哪些是 float（其余按 u32 精确比较）：按格式给出每顶点的 (字数, float 下标)
LAYOUT_WORDS = {'WorldUv': (5, (0, 1, 2, 3, 4)), 'WorldColorUv': (6, (0, 1, 2, 4, 5)), 'ScreenColorUv': (7, (0, 1, 2, 3, 5, 6)),
                'ScreenColor': (5, (0, 1, 2, 3)), 'ScreenUv': (6, (0, 1, 2, 3, 4, 5))}

# 0x45ba80 设备默认态（D3D 口径），ZWRITEENABLE 未设 → D3D 默认 TRUE
DEFAULT_RS = {7: 1, 22: 1, 27: 1, 19: 5, 20: 6, 23: 8, 171: 1, 15: 1, 24: 1, 25: 7, 28: 1, 34: 0xffa0a0a0,
              36: 0x447a0000, 37: 0x459c4000, 14: 1, 60: 0xffffffff}
DEFAULT_TSS = {1: 4, 2: 2, 3: 3, 4: 4, 5: 2, 6: 3, 24: 2}
DEFAULT_SAMP = {5: 2, 6: 2, 1: 1, 2: 1}


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & 0xffffffff))[0]


def identity_bits():
    return [0x3f800000 if i % 5 == 0 else 0 for i in range(16)]


class OracleState:
    """按 D3D 语义折叠原版调用流，产出与移植版同形的 op 列表。"""

    def __init__(self):
        self.rs = dict(DEFAULT_RS)
        self.tss = dict(DEFAULT_TSS)
        self.samp = dict(DEFAULT_SAMP)
        self.tex = None
        self.fvf = 0
        self.xf = {256: identity_bits(), 2: identity_bits(), 3: identity_bits(), 16: identity_bits()}
        self.vp = [0, 0, 640, 480]
        self.rt = 'backbuffer'

    def texture_ref(self, ident):
        if ident is None or ident == 'backbuffer':
            return [-1, -1]
        if isinstance(ident, list) and len(ident) == 2 and isinstance(ident[0], int):
            return ident
        return ['?', ident]

    def state(self):
        rs, tss, samp = self.rs, self.tss, self.samp
        return {
            'depthTest': 1 if rs[7] else 0, 'depthWrite': 1 if rs[14] else 0, 'depthCompare': rs[23] - 1,
            'blend': 1 if rs[27] else 0, 'srcBlend': rs[19] - 1, 'dstBlend': rs[20] - 1, 'blendOp': rs[171] - 1,
            'alphaTest': 1 if rs[15] else 0, 'alphaRef': rs[24], 'alphaCompare': rs[25] - 1,
            'fog': 1 if rs[28] else 0, 'fogColor': rs[34], 'fogNear': rs[36], 'fogFar': rs[37], 'textureFactor': rs[60],
            'colorOp': tss[1] - 1, 'colorArg1': D3DTA_TO_SOURCE.get(tss[2] & 0xf, tss[2]), 'colorArg2': D3DTA_TO_SOURCE.get(tss[3] & 0xf, tss[3]),
            'alphaOp': tss[4] - 1, 'alphaArg1': D3DTA_TO_SOURCE.get(tss[5] & 0xf, tss[5]), 'alphaArg2': D3DTA_TO_SOURCE.get(tss[6] & 0xf, tss[6]),
            'textureTransform': 1 if tss[24] else 0,
            'minFilter': D3DTEXF_TO_FILTER.get(samp[6], samp[6]), 'magFilter': D3DTEXF_TO_FILTER.get(samp[5], samp[5]),
            'addressU': samp[1] - 1, 'addressV': samp[2] - 1, 'cull': D3DCULL_TO_CULL.get(rs[22], rs[22]),
        }

    def apply(self, call):
        kind = call[0]
        if kind == 'rs':
            self.rs[call[1]] = call[2]
        elif kind == 'tss':
            if call[1] == 0:
                self.tss[call[2]] = call[3]
        elif kind == 'ss':
            if call[1] == 0:
                self.samp[call[2]] = call[3]
        elif kind == 'tex':
            if call[1] == 0:
                self.tex = call[2]
        elif kind == 'fvf':
            self.fvf = call[1]
        elif kind == 'xf':
            self.xf[call[1]] = call[2]
        elif kind == 'vp':
            self.vp = call[1][:4]
        elif kind == 'rt':
            self.rt = call[1]
            self.vp = [0, 0, 640, 480]      # SetRenderTarget 把视口重置为整个目标
        return None

    def fold(self, calls):
        ops = []
        chain = None    # 最近一次 ["chain", 回调地址]（game-oracle 在 0x4015a0 调回调前记录）
        for call in calls:
            kind = call[0]
            if kind == 'chain':
                chain = call[1]
            elif kind == 'clear':
                ops.append({'op': 'clear', 'flags': call[1], 'color': call[2], 'target': self.texture_ref(self.rt), 'rect': call[3],
                            'chain': chain})
            elif kind == 'draw':
                _, prim_type, prim_count, fvf, stride, words = call
                layout = FVF_TO_LAYOUT.get(fvf, 'fvf:%x' % fvf)
                op = {'op': 'draw', 'topology': TOPOLOGY.get(prim_type, str(prim_type)), 'layout': layout,
                      'primitives': prim_count, 'target': self.texture_ref(self.rt), 'texture': self.texture_ref(self.tex),
                      'viewport': list(self.vp), 'state': self.state(), 'words': words, 'chain': chain}
                if layout in WORLD_LAYOUTS:
                    op['world'] = self.xf[256]
                    op['view'] = self.xf[2]
                    op['projection'] = self.xf[3]
                    if op['state']['textureTransform']:
                        op['textureMatrix'] = self.xf[16]
                ops.append(op)
            else:
                self.apply(call)
        return ops


def load_jsonl(path):
    frames = {}
    with open(path) as f:
        for line in f:
            line = line.strip()
            if line:
                rec = json.loads(line)
                frames[rec['frame']] = rec
    return frames


def floats_close(a, b, eps):
    fa, fb = f32(a), f32(b)
    if fa != fa or fb != fb:
        return a == b
    return abs(fa - fb) <= eps * max(1.0, abs(fa), abs(fb))


def compare_words(layout, a, b, eps):
    if len(a) != len(b):
        return 'word count %d vs %d' % (len(a), len(b))
    per, float_idx = LAYOUT_WORDS.get(layout, (1, ()))
    for i, (x, y) in enumerate(zip(a, b)):
        if x == y:
            continue
        if (i % per) in float_idx and floats_close(x, y, eps):
            continue
        if (i % per) in float_idx:
            return 'vertex %d word %d: %.9g vs %.9g' % (i // per, i % per, f32(x), f32(y))
        return 'vertex %d word %d: %08x vs %08x' % (i // per, i % per, x, y)
    return None


def compare_matrix(name, a, b, eps):
    for i, (x, y) in enumerate(zip(a, b)):
        if x != y and not floats_close(x, y, eps):
            return '%s[%d]: %.9g vs %.9g' % (name, i, f32(x), f32(y))
    return None


def compare_op(o, p, eps, ignore_state):
    if o['op'] != p['op']:
        return 'kind %s vs %s' % (o['op'], p['op'])
    if o['target'] != p['target']:
        return 'target %s vs %s' % (o['target'], p['target'])
    if o['op'] == 'clear':
        for k in ('flags', 'color', 'rect'):
            if o[k] != p[k]:
                return 'clear %s %s vs %s' % (k, o[k], p[k])
        return None
    for k in ('topology', 'layout', 'primitives', 'texture', 'viewport'):
        if o[k] != p[k]:
            return '%s %s vs %s' % (k, o[k], p[k])
    world = o['layout'] in WORLD_LAYOUTS
    for k, v in o['state'].items():
        if k in ignore_state:
            continue
        # XYZRHW 顶点不经 TnL，D3D 不会对其套纹理矩阵：原版从不关 TEXTURETRANSFORMFLAGS，移植版对屏幕批显式关掉
        if k == 'textureTransform' and not world:
            continue
        pv = p['state'].get(k)
        if v != pv:
            if k in ('fogNear', 'fogFar') and floats_close(v, pv, eps):
                continue
            return 'state.%s %s vs %s' % (k, hex(v) if isinstance(v, int) and v > 255 else v, hex(pv) if isinstance(pv, int) and pv > 255 else pv)
    if o['layout'] in WORLD_LAYOUTS:
        for name in ('world', 'view', 'projection', 'textureMatrix'):
            if name in o or name in p:
                if (name in o) != (name in p):
                    return '%s present only in %s' % (name, 'oracle' if name in o else 'port')
                err = compare_matrix(name, o[name], p[name], eps)
                if err:
                    return err
    return compare_words(o['layout'], o['words'], p['words'], eps)


# 结构摘要用的状态键（顺序即口径；fogNear / fogFar 为 float 位型，差 1 ulp 也会翻盘，故不入摘要）
STRUCTURE_STATE_KEYS = ('depthTest', 'depthWrite', 'depthCompare', 'blend', 'srcBlend', 'dstBlend', 'blendOp',
                        'alphaTest', 'alphaRef', 'alphaCompare', 'fog', 'fogColor', 'textureFactor',
                        'colorOp', 'colorArg1', 'colorArg2', 'alphaOp', 'alphaArg1', 'alphaArg2', 'textureTransform',
                        'minFilter', 'magFilter', 'addressU', 'addressV', 'cull')


def structure_lines(ops):
    """每条 Clear / 绘制一行，与 tests/draw.test.mjs 的 structureLines 逐字一致。"""
    lines = []
    for op in ops:
        target = '%d,%d' % tuple(op['target'])
        if op['op'] == 'clear':
            rect = '-' if op['rect'] is None else ','.join(str(v) for v in op['rect'])
            lines.append('C|%s|%d|%d|%s' % (target, op['flags'], op['color'], rect))
            continue
        world = op['layout'] in WORLD_LAYOUTS
        state = []
        for key in STRUCTURE_STATE_KEYS:
            if key == 'textureTransform' and not world:
                continue   # 屏幕格式不经 TnL，纹理矩阵开关无意义（compare_op 同样跳过）
            state.append(str(op['state'][key]))
        lines.append('D|%s|%s|%s|%d|%d,%d|%s|%s' % (target, op['topology'], op['layout'], op['primitives'],
                                                   op['texture'][0], op['texture'][1],
                                                   ','.join(str(v) for v in op['viewport']), ','.join(state)))
    return lines


def structure_digest(ops, texture_slots=None):
    if texture_slots is not None:
        ops = [op for op in ops if op['op'] == 'draw' and op['texture'][0] in texture_slots]
    return hashlib.sha1('\n'.join(structure_lines(ops)).encode()).hexdigest()


# ---- 整段摘要：每帧把操作流压成几条 sha1，两侧在各自进程里算，只交换摘要 ----
# 原始绘制流每帧可达数百 KB，整段落盘不可行；顶点又不能直接比位（个别字会差 1 ulp）。
# 所以每帧出四条：s = 结构（structure_lines），v = 位级顶点与矩阵，a / b = 量化网格 a / b。
# 量化：浮点字按二进制档位取格宽 cell = QUANT_EPS × 2^max(0, E - 127)（E 为指数位），格号 floor((|v| + half·cell) / cell)；
# 网格 a 的 half = 0，网格 b 的 half = 0.5。两值相差不超过一格时至少在一张网格里同格。两侧（本文件 / tools/draw-structure.mjs）
# 都只用位运算与二进制精确的乘除，不调 libm，记号逐字一致。
QUANT_EPS = 1e-4


def quant_token(value, bits, half):
    magnitude = abs(value)
    exponent = (bits >> 23) & 0xff
    if exponent == 0xff:
        return 'n%08x' % bits if bits & 0x7fffff else ('-' if value < 0.0 else '') + 'inf'
    cell = QUANT_EPS if exponent < 128 else math.ldexp(QUANT_EPS, exponent - 127)
    return ('-' if value < 0.0 else '') + str(exponent) + ':' + str(int(math.floor((magnitude + cell * half) / cell)))


def _float_mask(layout):
    per, float_idx = LAYOUT_WORDS.get(layout, (1, ()))
    return [i % per in float_idx for i in range(per)]


def _word_tokens(words, mask, half):
    if not any(mask):
        return ','.join(map(str, words))
    picks = [i for i in range(len(words)) if mask[i % len(mask)]]
    floats = struct.unpack('<%df' % len(picks), struct.pack('<%dI' % len(picks), *[words[i] for i in picks]))
    tokens = [str(w) for w in words]
    for i, value in zip(picks, floats):
        tokens[i] = quant_token(value, words[i], half)
    return ','.join(tokens)


def _op_tails(op):
    """绘制操作在结构行之后追加的部分（位级 / 网格 a / 网格 b 三份）：顶点字，世界格式再加 world / view / projection（与纹理矩阵）。"""
    mask = _float_mask(op['layout'])
    matrices = [name for name in ('world', 'view', 'projection', 'textureMatrix') if name in op]
    exact = '|' + '|'.join([','.join(map(str, op['words']))] + [name[0] + ','.join(map(str, op[name])) for name in matrices])

    def tail(half):
        parts = [_word_tokens(op['words'], mask, half)]
        for name in matrices:
            parts.append(name[0] + _word_tokens(op[name], [True], half))
        return '|' + '|'.join(parts)
    return [exact, tail(0.0), tail(0.5)]


def frame_digests(ops):
    """{"frame" 之外的字段：ops = 操作数，s / v / a / b = 四条 sha1（见上）。ops 可含移植侧的 target / viewport 记录。"""
    ops = [op for op in ops if op['op'] in ('clear', 'draw')]
    lines = structure_lines(ops)
    digests = {'ops': len(ops), 's': hashlib.sha1('\n'.join(lines).encode()).hexdigest()}
    tails = [('|', '|', '|') if op['op'] == 'clear' else _op_tails(op) for op in ops]
    for column, grid in enumerate('vab'):
        digests[grid] = hashlib.sha1('\n'.join(line + tail[column] for line, tail in zip(lines, tails)).encode()).hexdigest()
    return digests


def digest_record(rec, st):
    """一行原始流 → 一行摘要。原版流（有 calls）先经 st 折叠；移植侧直接取 ops。"""
    if 'calls' not in rec:
        return dict(frame=rec['frame'], **frame_digests(rec['ops']))
    for call in rec.get('prefix', []):
        st.apply(call)
    return dict(frame=rec['frame'], **frame_digests(st.fold(rec['calls'])))


def load_digests(path):
    rows = {}
    with open(path) as f:
        for line in f:
            if line.strip():
                rec = json.loads(line)
                rows[rec['frame']] = rec
    return rows


def segment_digest(rows, frames, grid):
    return hashlib.sha1('\n'.join('%d|%s|%s' % (frame, rows[frame]['s'], rows[frame][grid]) for frame in frames).encode()).hexdigest()


def compare_digests(oracle, port, segment, max_report):
    """逐帧：s 相同且（a 同格或 b 同格）即通过；grid 记通过所用的网格（两张都同记 a）。
    连续同网格的帧折成一段，每段至多 segment 帧，段摘要进 golden（tests/draw-long.test.mjs 对移植侧复算）。"""
    frames = sorted(set(oracle) & set(port))
    missing = sorted(set(oracle) ^ set(port))
    segments, run = [], []
    bad = {'structure': [], 'vertices': []}
    identical = quantized = 0
    for frame in frames:
        o, p = oracle[frame], port[frame]
        if o['s'] != p['s']:
            bad['structure'].append(frame)
            grid = None
        elif o['v'] == p['v']:
            grid, identical = 'a', identical + 1
        elif o['a'] == p['a']:
            grid, quantized = 'a', quantized + 1
        elif o['b'] == p['b']:
            grid, quantized = 'b', quantized + 1
        else:
            bad['vertices'].append(frame)
            grid = None
        if grid is None:
            if run:
                segments.append(run)
                run = []
            continue
        if run and (run[-1][0] + 1 != frame or run[-1][1] != grid or len(run) >= segment):
            segments.append(run)
            run = []
        run.append((frame, grid))
    if run:
        segments.append(run)
    report = ['frames=%d bit-identical=%d quantized=%d structure-differs=%d vertices-differ=%d only-one-side=%d'
              % (len(frames), identical, quantized, len(bad['structure']), len(bad['vertices']), len(missing))]
    for kind, label in (('structure', 'structure differs'), ('vertices', 'vertices differ beyond one grid cell')):
        if bad[kind]:
            report.append('%s on frames %s%s' % (label, bad[kind][:max_report], ' …' if len(bad[kind]) > max_report else ''))
    if missing:
        report.append('frames recorded on one side only: %s%s' % (missing[:max_report], ' …' if len(missing) > max_report else ''))
    golden = [[run[0][0], run[-1][0], run[0][1], segment_digest(oracle, [f for f, _ in run], run[0][1])] for run in segments]
    return report, golden, bool(frames) and not bad['structure'] and not bad['vertices'] and not missing


def write_digests(path, out_path):
    """原始流（原版或移植侧格式，逐行）→ 每帧一行摘要。原版流的折叠状态跨帧连续。"""
    st, count = OracleState(), 0
    with open(path) as src, open(out_path, 'w') as dst:
        for line in src:
            if not line.strip():
                continue
            dst.write(json.dumps(digest_record(json.loads(line), st), separators=(',', ':')) + '\n')
            count += 1
    print('digests: %d frames → %s' % (count, out_path))
    return 0


def compare_digest_files(oracle_path, port_path, segment, max_report, golden_path):
    report, golden, ok = compare_digests(load_digests(oracle_path), load_digests(port_path), segment, max_report)
    print('\n'.join(report))
    if golden_path:
        with open(golden_path, 'w') as f:
            json.dump({'segment': segment, 'segments': golden}, f, separators=(',', ':'))
            f.write('\n')
        print('segments: %d → %s' % (len(golden), golden_path))
    return 0 if ok else 1


def describe(op):
    if op['op'] == 'clear':
        return 'clear flags=%d color=%08x rect=%s target=%s' % (op['flags'], op['color'], op['rect'], op['target'])
    return 'draw %s %s n=%d tex=%s vp=%s target=%s' % (op['topology'], op['layout'], op['primitives'], op['texture'], op['viewport'], op['target'])


def describe_vertices(op, limit=4):
    """--dump 用：前 limit 个顶点的 (x, y[, 颜色])，屏幕格式直接读坐标，世界格式只给模型坐标。"""
    if op['op'] != 'draw':
        return ''
    per, _ = LAYOUT_WORDS.get(op['layout'], (1, ()))
    words = op['words']
    color_index = {'ScreenColorUv': 4, 'ScreenColor': 4, 'WorldColorUv': 3}.get(op['layout'])
    parts = []
    for i in range(0, min(len(words), per * limit), per):
        v = words[i:i + per]
        text = '(%.6g, %.6g' % (f32(v[0]), f32(v[1]))
        if color_index is not None:
            text += ' %08x' % v[color_index]
        parts.append(text + ')')
    return ' '.join(parts)


def dump_frames(frames_map, frames, fold):
    """按帧逐条打印绘制操作（oracle 侧先折叠状态调用）。"""
    st = OracleState()
    for frame in sorted(frames_map):
        if frame > frames[-1]:
            break
        rec = frames_map[frame]
        if fold:
            for call in rec.get('prefix', []):
                st.apply(call)
            ops = st.fold(rec['calls'])
        else:
            ops = [op for op in rec['ops'] if op['op'] in ('clear', 'draw')]
        if frame < frames[0]:
            continue
        print('frame %d: %d ops' % (frame, len(ops)))
        for i, op in enumerate(ops):
            chain = op.get('chain')
            tag = ('[%#x] ' % chain) if isinstance(chain, int) else ''
            print('  %3d %s%s %s' % (i, tag, describe(op), describe_vertices(op)))


def write_structure_golden(oracle_path, out_path, frame_range, texture_slots=None):
    oracle = load_jsonl(oracle_path)
    lo, hi = (int(x) for x in frame_range.split('-')) if frame_range else (min(oracle), max(oracle))
    st = OracleState()
    digests = {}
    for frame in sorted(oracle):
        rec = oracle[frame]
        for call in rec.get('prefix', []):
            st.apply(call)
        ops = st.fold(rec['calls'])
        if lo <= frame <= hi:
            digests[str(frame)] = structure_digest(ops, texture_slots)
    existing = {}
    try:
        with open(out_path) as f:
            existing = json.load(f)
    except (OSError, ValueError):
        pass
    existing.setdefault('frames', {}).update(digests)
    if texture_slots is not None:
        existing['texture_slots'] = sorted(texture_slots)
    with open(out_path, 'w') as f:
        json.dump(existing, f, indent=1, sort_keys=True)
        f.write('\n')
    print('structure golden: %d frames → %s' % (len(digests), out_path))
    return 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('oracle')
    ap.add_argument('port')
    ap.add_argument('--frames', default='', help='a-b 只比较此帧范围')
    ap.add_argument('--eps', type=float, default=1e-4, help='float 相对误差容限')
    ap.add_argument('--max-report', type=int, default=20)
    ap.add_argument('--ignore-state', default='', help='逗号分隔的忽略状态键')
    ap.add_argument('--dump', choices=('oracle', 'port'), help='不比较，只按帧列出一侧的绘制操作')
    ap.add_argument('--skip-textures', default='',
                    help='逗号分隔的 AnmFile 槽号（如 2 = ascii.anm）：两侧都丢弃用这些槽贴图的绘制操作，'
                         '用于暂未移植的子系统。按链地址过滤不可行：链回调里的出批会把上一层残批也记在它名下')
    ap.add_argument('--structure-golden', default='', help='只读原版流，写出每帧结构摘要 JSON（见模块说明）')
    ap.add_argument('--only-textures', default='',
                    help='--structure-golden 用：逗号分隔的 AnmFile 槽号，摘要只含用这些槽贴图的绘制操作')
    ap.add_argument('--digest', default='', help='原始流（第一个参数，原版或移植侧格式）→ 每帧一行摘要，写到该路径')
    ap.add_argument('--compare-digests', action='store_true', help='两个输入都是 --draw-digest / --digest 的摘要流')
    ap.add_argument('--segment', type=int, default=600, help='--compare-digests：每段至多多少帧（默认 600）')
    ap.add_argument('--golden-segments', default='', help='--compare-digests：把通过帧的段表写到该路径')
    a = ap.parse_args()
    if a.digest:
        return write_digests(a.oracle, a.digest)
    if a.compare_digests:
        return compare_digest_files(a.oracle, a.port, a.segment, a.max_report, a.golden_segments)
    ignore_state = set(filter(None, a.ignore_state.split(',')))
    skip_textures = {int(x, 0) for x in filter(None, a.skip_textures.split(','))}

    def keep(op):
        return op['op'] != 'draw' or op['texture'][0] not in skip_textures
    if a.structure_golden:
        only = {int(x, 0) for x in filter(None, a.only_textures.split(','))} or None
        return write_structure_golden(a.oracle, a.structure_golden, a.frames, only)
    oracle, port = load_jsonl(a.oracle), load_jsonl(a.port)
    frames = sorted(set(oracle) & set(port)) if a.dump is None else sorted(oracle if a.dump == 'oracle' else port)
    if a.frames:
        lo, hi = (int(x) for x in a.frames.split('-'))
        frames = [f for f in frames if lo <= f <= hi]
    if not frames:
        print('no common frames', file=sys.stderr)
        return 2
    if a.dump is not None:
        dump_frames(oracle if a.dump == 'oracle' else port, frames, fold=a.dump == 'oracle')
        return 0
    st = OracleState()
    mismatches = 0
    reported = 0
    for frame in sorted(oracle):
        if frame > frames[-1]:
            break
        rec = oracle[frame]
        for call in rec.get('prefix', []):
            st.apply(call)
        ops_o = [op for op in st.fold(rec['calls']) if keep(op)]
        if frame not in port or frame < frames[0]:
            continue
        ops_p = port[frame]['ops']
        ops_p = [op for op in ops_p if op['op'] in ('clear', 'draw') and keep(op)]
        n = min(len(ops_o), len(ops_p))
        frame_bad = 0
        for i in range(n):
            err = compare_op(ops_o[i], ops_p[i], a.eps, ignore_state)
            if err:
                frame_bad += 1
                if reported < a.max_report:
                    print('frame %d op %d: %s\n  oracle: %s\n  port:   %s' % (frame, i, err, describe(ops_o[i]), describe(ops_p[i])))
                    reported += 1
        if len(ops_o) != len(ops_p):
            frame_bad += 1
            if reported < a.max_report:
                extra = ops_o[n:] if len(ops_o) > n else ops_p[n:]
                who = 'oracle' if len(ops_o) > n else 'port'
                print('frame %d: op count %d vs %d; first extra in %s: %s' % (frame, len(ops_o), len(ops_p), who, describe(extra[0])))
                reported += 1
        mismatches += frame_bad
    print('frames=%d mismatches=%d' % (len(frames), mismatches))
    return 1 if mismatches else 0


if __name__ == '__main__':
    sys.exit(main())
