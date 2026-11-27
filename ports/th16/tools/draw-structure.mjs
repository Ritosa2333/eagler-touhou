// 绘制调用流的"结构"行与整段摘要，口径与 tools/oracle/draw-compare.py 的 structure_lines / frame_digests 逐字一致。
// tests/draw.test.mjs 用 structureLines 对移植侧复算每帧结构摘要；tests/draw-long.test.mjs 用 digestStream 把整段跑折成段摘要。
import {createHash} from 'node:crypto';
import {createInterface} from 'node:readline';

export const WORLD_LAYOUTS = new Set(['WorldUv', 'WorldColorUv']);
export const STRUCTURE_STATE_KEYS = ['depthTest', 'depthWrite', 'depthCompare', 'blend', 'srcBlend', 'dstBlend', 'blendOp',
  'alphaTest', 'alphaRef', 'alphaCompare', 'fog', 'fogColor', 'textureFactor',
  'colorOp', 'colorArg1', 'colorArg2', 'alphaOp', 'alphaArg1', 'alphaArg2', 'textureTransform',
  'minFilter', 'magFilter', 'addressU', 'addressV', 'cull'];

export function structureLines(ops) {
  const lines = [];
  for (const op of ops) {
    if (op.op !== 'clear' && op.op !== 'draw') continue;
    const target = `${op.target[0]},${op.target[1]}`;
    if (op.op === 'clear') {
      const rect = op.rect === null ? '-' : op.rect.join(',');
      lines.push(`C|${target}|${op.flags}|${op.color}|${rect}`);
      continue;
    }
    const world = WORLD_LAYOUTS.has(op.layout);
    const state = [];
    for (const key of STRUCTURE_STATE_KEYS) {
      if (key === 'textureTransform' && !world) continue;
      state.push(String(op.state[key]));
    }
    lines.push(`D|${target}|${op.topology}|${op.layout}|${op.primitives}|${op.texture[0]},${op.texture[1]}|${op.viewport.join(',')}|${state.join(',')}`);
  }
  return lines;
}

// golden.texture_slots 存在时只摘要用这些 AnmFile 槽贴图的绘制操作（draw-compare.py --only-textures 同口径）
export function structureDigest(ops, textureSlots) {
  if (textureSlots) ops = ops.filter(op => op.op === 'draw' && textureSlots.has(op.texture[0]));
  return createHash('sha1').update(structureLines(ops).join('\n')).digest('hex');
}

// ---- 整段摘要（draw-compare.py frame_digests）----
// 每帧四条 sha1：s = 结构行，v = 位级顶点与矩阵，a / b = 量化网格。浮点字的量化格宽 = QUANT_EPS × 2^max(0, 指数位 - 127)，
// 网格 a 的格号 floor(|v| / 格宽)，网格 b 偏移半格。只用位运算与二进制精确的乘除，不调 libm，记号与 Python 侧逐字一致。
const QUANT_EPS = 1e-4;
// 各顶点格式一个顶点的字数与其中哪些是浮点（draw-compare.py LAYOUT_WORDS）；表长即周期
const LAYOUT_FLOAT_MASK = {
  WorldUv: [true, true, true, true, true],
  WorldColorUv: [true, true, true, false, true, true],
  ScreenColorUv: [true, true, true, true, false, true, true],
  ScreenColor: [true, true, true, true, false],
  ScreenUv: [true, true, true, true, true, true],
};
const bitsView = new DataView(new ArrayBuffer(4));

export function quantToken(value, bits, half) {
  const magnitude = Math.abs(value);
  const exponent = (bits >>> 23) & 0xff;
  if (exponent === 0xff) {
    return (bits & 0x7fffff) !== 0 ? `n${(bits >>> 0).toString(16).padStart(8, '0')}` : `${value < 0 ? '-' : ''}inf`;
  }
  const cell = exponent < 128 ? QUANT_EPS : QUANT_EPS * 2 ** (exponent - 127);
  return `${value < 0 ? '-' : ''}${exponent}:${Math.floor((magnitude + cell * half) / cell)}`;
}

function wordTokens(words, mask, half) {
  if (!mask.some(Boolean)) return words.join(',');
  return words.map((word, index) => {
    if (!mask[index % mask.length]) return String(word);
    bitsView.setUint32(0, word >>> 0, true);
    return quantToken(bitsView.getFloat32(0, true), word, half);
  }).join(',');
}

// 绘制操作在结构行之后追加的部分（位级 / 网格 a / 网格 b 三份）：顶点字，世界格式再加 world / view / projection（与纹理矩阵）
function opTails(op) {
  const mask = LAYOUT_FLOAT_MASK[op.layout] ?? [false];
  const matrices = ['world', 'view', 'projection', 'textureMatrix'].filter(name => name in op);
  const exact = '|' + [op.words.join(','), ...matrices.map(name => name[0] + op[name].join(','))].join('|');
  const tail = half => '|' + [wordTokens(op.words, mask, half),
    ...matrices.map(name => name[0] + wordTokens(op[name], [true], half))].join('|');
  return [exact, tail(0), tail(0.5)];
}

export function frameDigests(allOps) {
  const ops = allOps.filter(op => op.op === 'clear' || op.op === 'draw');
  const lines = structureLines(ops);
  const tails = ops.map(op => op.op === 'clear' ? ['|', '|', '|'] : opTails(op));
  const sha1 = text => createHash('sha1').update(text).digest('hex');
  return {ops: ops.length, s: sha1(lines.join('\n')),
    v: sha1(lines.map((line, i) => line + tails[i][0]).join('\n')),
    a: sha1(lines.map((line, i) => line + tails[i][1]).join('\n')),
    b: sha1(lines.map((line, i) => line + tails[i][2]).join('\n'))};
}

// harness game-draw 的 JSON 行流 → 逐帧摘要
export async function* digestStream(input) {
  const lines = createInterface({input, crlfDelay: Infinity});
  for await (const line of lines) {
    if (!line) continue;
    const record = JSON.parse(line);
    yield {frame: record.frame, ...frameDigests(record.ops)};
  }
}

// 段摘要：段内各帧 "frame|s|网格" 依次拼接（draw-compare.py segment_digest 同口径）
export function segmentDigest(rows, frames, grid) {
  return createHash('sha1').update(frames.map(frame => `${frame}|${rows.get(frame).s}|${rows.get(frame)[grid]}`).join('\n')).digest('hex');
}
