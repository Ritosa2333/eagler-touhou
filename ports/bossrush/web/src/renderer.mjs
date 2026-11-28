/*
 * RGL Web runtime -- WebGL2 渲染后端
 *
 * 定位：**薄执行器**。所有引擎语义（层级、切图、锚点、viewmode 变换、混合
 * 模式）都已经在 C 侧的绘制命令流里算好（rgl_render.c），这里只做三件事：
 *   1. 把 ROP_* 命令翻译成顶点
 *   2. 按名字懒加载纹理（像素走 HTTP，不进 .data）
 *   3. 切混合状态、drawArrays
 *
 * 为什么不在这里复刻引擎逻辑：确定性与后续联机帧同步都要求「同输入同输出」。
 * 逻辑只要有一处躺在 JS 里，将来做状态回滚就得在 JS 里再维护一份等价状态，极易分叉。
 * （本作没有录像/回放功能，也不产生 .rep 文件；这里的「确定性」与回放文件无关。）
 *
 * 坐标约定（与 v0.21.103 一致）：
 *   · 世界/屏幕坐标 y 向下，原点左上
 *   · SetOrtho(l, r, b, t)：x∈[l,r] → NDC[-1,1]，y∈[b,t] → NDC[-1,1]
 *     注意 t 是顶（小 y）、b 是底（大 y），所以 NDC 的 y 要取反
 *   · SetViewport(l, r, b, t)：同样 l/r 是 x，b/t 是 y；WebGL 视口 y 从底算
 */

import { createTextLayer } from './text.mjs';
import { createPostFX } from './postfx.mjs';

const OP = {  VIEWMODE: 1, VIEWPORT: 2, SCISSOR: 3, ORTHO: 4, PERSPECTIVE: 5, FOG: 6,
  IMAGESCALE: 7, SAMPLER: 8,
  DRAW: 9, DRAW_RECT: 10, DRAW_4V: 11, DRAW_TEX: 12, DRAW_ANIM: 13,
  CLEAR: 14, PUSH_TARGET: 15, POP_TARGET: 16,
  TEXT: 17, TTF: 18, POSTEFFECT: 19, TTF3: 20,
  SECTOR: 21,
  DRAW_4V_BATCH: 22,
  DRAW_BATCH: 23,
  ZENABLE: 24,
  ZCLEAR: 25,
};

const HEAD_SIZE = 32;          // 与 RopHead 一致，启动时用 rgl_rop_layout 校验

/* 顶点色混合模式，编号与 IRenderer::VertexColorBlendState 一致
   （engine/graphics/core/Graphics/Renderer.hpp:37）。 */
const VC_ZERO = 0, VC_ONE = 1, VC_ADD = 2, VC_MUL = 3;

/* blend 表：**两段式**，与 LegacyBlendStateHelper.hpp 的
   translateLegacyBlendState() 逐条对应：

     data.vertex_color_blend_state = <前半>   // 顶点色怎么并进像素色
     data.blend_state              = <后半>   // 与底图怎么合

   数组是 [顶点色模式, 颜色方程, 颜色src, 颜色dst, alpha_src, alpha_dst]。
   alpha 通道的因子来自 Renderer_Pipeline_D3D11.cpp 的 applyBlendState，
   它与颜色通道并不总是同一组（例如 Mul 的 alpha 是 one / 1-srcAlpha，
   而颜色是 dest_color / zero），所以必须分开设置。

   另一半关键事实：引擎的像素着色器**末尾统一预乘**（pixel_shader.hlsl:
   `output.col.rgb *= output.col.a;`），因此 BlendState::Alpha 用的是
   one / one-srcAlpha 而不是 srcAlpha / one-srcAlpha。我们的 FRAG 也照做，
   否则加法类混合（one/one）会被二次衰减。 */
const B_ADD = 'add', B_SUB = 'sub', B_REVSUB = 'revSub', B_MIN = 'min', B_MAX = 'max';
const BLEND = {
  0:  [VC_MUL, B_ADD,    'one', 'oneMinusSrcAlpha', 'one',  'oneMinusSrcAlpha'], // ""(==mul+alpha)
  1:  [VC_MUL, B_ADD,    'one', 'oneMinusSrcAlpha', 'one',  'oneMinusSrcAlpha'], // mul+alpha
  2:  [VC_MUL, B_ADD,    'one', 'one',              'one',  'oneMinusSrcAlpha'], // mul+add
  3:  [VC_MUL, B_REVSUB, 'one', 'one',              'one',  'oneMinusSrcAlpha'], // mul+rev
  4:  [VC_MUL, B_SUB,    'one', 'one',              'one',  'oneMinusSrcAlpha'], // mul+sub
  5:  [VC_ADD, B_ADD,    'one', 'oneMinusSrcAlpha', 'one',  'oneMinusSrcAlpha'], // add+alpha
  6:  [VC_ADD, B_ADD,    'one', 'one',              'one',  'oneMinusSrcAlpha'], // add+add
  7:  [VC_ADD, B_REVSUB, 'one', 'one',              'one',  'oneMinusSrcAlpha'], // add+rev
  8:  [VC_ADD, B_SUB,    'one', 'one',              'one',  'oneMinusSrcAlpha'], // add+sub
  9:  [VC_MUL, B_ADD,    'oneMinusDstColor', 'oneMinusSrcColor', 'zero', 'one'], // alpha+bal
  10: [VC_MUL, B_MIN,    'one', 'one',              'one',  'one'],              // mul+min
  11: [VC_MUL, B_MAX,    'one', 'one',              'one',  'one'],              // mul+max
  12: [VC_MUL, B_ADD,    'dstColor', 'zero',        'one',  'oneMinusSrcAlpha'], // mul+mul
  13: [VC_MUL, B_ADD,    'one', 'oneMinusSrcColor', 'one',  'oneMinusSrcAlpha'], // mul+screen
  14: [VC_ADD, B_MIN,    'one', 'one',              'one',  'one'],              // add+min
  15: [VC_ADD, B_MAX,    'one', 'one',              'one',  'one'],              // add+max
  16: [VC_ADD, B_ADD,    'dstColor', 'zero',        'one',  'oneMinusSrcAlpha'], // add+mul
  17: [VC_ADD, B_ADD,    'one', 'oneMinusSrcColor', 'one',  'oneMinusSrcAlpha'], // add+screen
  18: [VC_MUL, B_ADD,    'one', 'zero',             'one',  'zero'],             // one
};

/* a_iw = 1 / 相机空间 z（2D 恒为 1）。
   3D 背景是横跨 z=-6..10 的大四边形，透视除法后 GPU 只会按屏幕空间线性
   插值 varying；如果直接把 u/v 交给它，等于「仿射纹理映射」，远处不收缩、
   近处被拉成长条 —— 就是清酒看到的背景拉伸异常。
   D3D 侧由光栅化器按 1/w 做透视校正，这里在 CPU 侧投影完再自己补上：
   顶点输出 u*iw / v*iw / col*iw / iw，片元里统一除以 iw。 */
const VERT = `#version 300 es
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec4 a_col;
layout(location = 3) in float a_iw;
layout(location = 4) in vec3 a_cpos;
uniform vec2 u_scale;
uniform vec2 u_offset;
/* 深度：a_cpos.z 是相机空间 z（2D 顶点为 0）。映射到 [-1,1] 交给 GPU 比较，
   这样背景里「柱子 vs 地面」的遮挡与原版 z 测试一致（bugs #17/#21）。 */
uniform float u_zNear;
uniform float u_zFar;
out vec2 v_uv;
out vec4 v_col;
out float v_iw;
out vec3 v_cpos;
void main() {
  float zNdc = 0.5;
  if (a_cpos.z != 0.0) {
    zNdc = clamp((a_cpos.z - u_zNear) / max(u_zFar - u_zNear, 1e-6), 0.0, 1.0)
           * 2.0 - 1.0;
  }
  gl_Position = vec4(a_pos * u_scale + u_offset, zNdc, 1.0);
  v_uv = a_uv * a_iw;
  v_col = a_col * a_iw;
  v_cpos = a_cpos * a_iw;
  v_iw = a_iw;
}`;

/* 与 engine/graphics/d3d11/shader/renderer/pixel_shader.hlsl 等价：
   1) VERTEX_COLOR_BLEND_ONE 时完全不采样纹理，像素色 = 顶点色
   2) MUL: color * vc        ADD: min(color.rgb + vc.rgb, 1), color.a * vc.a
      ONE: vc               ZERO: color（原样）
   3) 末尾统一 `rgb *= a` 预乘，配合 one/one-srcAlpha 得到标准 alpha 混合 */
const FRAG = `#version 300 es
precision highp float;       // 背景 uv 会到 0..8，mediump 会让 REPEAT 平铺出现条纹
in vec2 v_uv;
in vec4 v_col;
in float v_iw;
in vec3 v_cpos;
uniform sampler2D u_tex;
uniform int u_vc;            // VertexColorBlendState：0=Zero 1=One 2=Add 3=Mul
uniform int u_fogOn;
uniform float u_fogNear;
uniform float u_fogFar;
uniform vec4 u_fogColor;
out vec4 o_col;
void main() {
  float iw = max(v_iw, 1e-6);
  vec2 uv = v_uv / iw;       // 透视校正后的纹理坐标
  vec4 vc = v_col / iw;      // 顶点色同理（3D 四边形一般四角同色，但语义要对齐）
  vec4 c;
  if (u_vc == 1) {
    c = vec4(1.0);           // 引擎这一支把纹理当成纯白，等效于不采样
  } else {
    c = texture(u_tex, uv);
  }
  vec4 o;
  if (u_vc == 3)      o = c * vc;
  else if (u_vc == 2) o = vec4(min(c.rgb + vc.rgb, vec3(1.0)), c.a * vc.a);
  else if (u_vc == 1) o = vc;
  else                o = c;
  if (u_fogOn != 0) {
    /* pixel_shader.hlsl:63 的 applyFogColor，位置是「顶点色之后、预乘之前」：
         d = distance(camera_pos.xyz, pixel_position)      // 世界坐标距离
         k = (d - fog_range.x) / fog_range.w               // w = far - near
         lerp(color, float4(fog_color.rgb, fog_color.a * color.a), clamp(k,0,1))
       关键：距离必须逐片元算。此前是在 4 个顶点上各算一次 k 再插值，背景大
       四边形跨 16 个世界单位，中间地带的雾浓度会明显偏，看起来就是「和原生
       有差别」。v_cpos 是相机空间坐标（刚体变换，欧氏距离与世界坐标等价），
       除以 iw 得到透视校正后的插值位置，再取 length。 */
    float d = length(v_cpos / iw);
    float k = clamp((d - u_fogNear) / max(u_fogFar - u_fogNear, 1e-6), 0.0, 1.0);
    o = vec4(mix(o.rgb, u_fogColor.rgb, k), mix(o.a, u_fogColor.a * o.a, k));
  }
  o.rgb *= o.a;              // pixel_shader.hlsl 的 output.col.rgb *= output.col.a
  o_col = o;
}`;

const FLOATS_PER_VERT = 12;    // x, y, u, v, r, g, b, a, iw, cx, cy, cz
                               /* cx/cy/cz 是相机空间坐标，只给片元级雾算距离用；
                                  2D 顶点填 0 且 u_fogOn=0，行为与之前完全一致。 */
const VERTS_PER_QUAD = 6;
const MAX_QUADS = 8192;

export function createRenderer(Module, canvas, opts = {}) {
  const assetBase = opts.assetBase || '../game/';
  const ssOverride = +opts.ssOverride || 0;   // 0 = 按 canvas 高度自动
  /* 资源源：导入包优先，未命中回退 HTTP。不传时退化为纯 HTTP（旧行为）。 */
  const assets = opts.assets || {
    url: (p) => Promise.resolve(assetBase + String(p).replace(/\\/g, '/')
      .replace(/^\/+/, '').split('/').map(encodeURIComponent).join('/')),
    httpUrl: (p) => assetBase + String(p).replace(/\\/g, '/').replace(/^\/+/, '')
      .split('/').map(encodeURIComponent).join('/'),
    describePath: (p) => 'http:' + p,
    hasPack: () => false,
  };
  /* preserveDrawingBuffer 默认关：对 60fps 的 STG 来说它是纯开销。
     但无头验证必须开——暂停后只画一帧，关掉的话合成器拿到的是已清空的缓冲，
     截图永远全黑（这个坑踩过一次，记在这里）。 */
  const gl = canvas.getContext('webgl2', {
    alpha: false, antialias: false, depth: true, stencil: false,
    premultipliedAlpha: false, preserveDrawingBuffer: !!opts.preserveDraw,
  });
  if (!gl) throw new Error('WebGL2 不可用');

  /* ---- 布局校验 -------------------------------------------------------- */
  const layout = JSON.parse(Module.ccall('rgl_rop_layout', 'string', [], []));
  if (layout.head !== HEAD_SIZE) {
    throw new Error(`RopHead 布局不一致：C=${layout.head} JS=${HEAD_SIZE}`);
  }

  /* ---- 着色器 ---------------------------------------------------------- */
  const compile = (type, src) => {
    const s = gl.createShader(type);
    gl.shaderSource(s, src);
    gl.compileShader(s);
    if (!gl.getShaderParameter(s, gl.COMPILE_STATUS)) {
      throw new Error('shader: ' + gl.getShaderInfoLog(s));
    }
    return s;
  };
  const prog = gl.createProgram();
  gl.attachShader(prog, compile(gl.VERTEX_SHADER, VERT));
  gl.attachShader(prog, compile(gl.FRAGMENT_SHADER, FRAG));
  gl.linkProgram(prog);
  if (!gl.getProgramParameter(prog, gl.LINK_STATUS)) {
    throw new Error('link: ' + gl.getProgramInfoLog(prog));
  }
  gl.useProgram(prog);
  const uScale = gl.getUniformLocation(prog, 'u_scale');
  const uOffset = gl.getUniformLocation(prog, 'u_offset');
  const uTex = gl.getUniformLocation(prog, 'u_tex');
  const uVc = gl.getUniformLocation(prog, 'u_vc');
  const uFogOn = gl.getUniformLocation(prog, 'u_fogOn');
  const uFogNear = gl.getUniformLocation(prog, 'u_fogNear');
  const uFogFar = gl.getUniformLocation(prog, 'u_fogFar');
  const uFogColor = gl.getUniformLocation(prog, 'u_fogColor');
  const uZNear = gl.getUniformLocation(prog, 'u_zNear');
  const uZFar = gl.getUniformLocation(prog, 'u_zFar');
  gl.uniform1i(uTex, 0);

  /* ---- 顶点缓冲 -------------------------------------------------------- */
  const vbo = gl.createBuffer();
  const vdata = new Float32Array(MAX_QUADS * VERTS_PER_QUAD * FLOATS_PER_VERT);
  const vao = gl.createVertexArray();
  gl.bindVertexArray(vao);
  gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
  gl.bufferData(gl.ARRAY_BUFFER, vdata.byteLength, gl.DYNAMIC_DRAW);
  const stride = FLOATS_PER_VERT * 4;
  gl.enableVertexAttribArray(0);
  gl.vertexAttribPointer(0, 2, gl.FLOAT, false, stride, 0);
  gl.enableVertexAttribArray(1);
  gl.vertexAttribPointer(1, 2, gl.FLOAT, false, stride, 8);
  gl.enableVertexAttribArray(2);
  gl.vertexAttribPointer(2, 4, gl.FLOAT, false, stride, 16);
  gl.enableVertexAttribArray(3);
  gl.vertexAttribPointer(3, 1, gl.FLOAT, false, stride, 32);
  gl.enableVertexAttribArray(4);
  gl.vertexAttribPointer(4, 3, gl.FLOAT, false, stride, 36);
  gl.bindVertexArray(null);

  gl.disable(gl.DEPTH_TEST);
  gl.enable(gl.BLEND);
  /* 默认态对应 mul+alpha，且因为像素着色器输出已预乘，源因子是 one */
  gl.blendFuncSeparate(gl.ONE, gl.ONE_MINUS_SRC_ALPHA, gl.ONE, gl.ONE_MINUS_SRC_ALPHA);
  gl.uniform1i(uVc, VC_MUL);

  /* ---- 资源表 ---------------------------------------------------------- */
  /** sprite 名 → { tex, u0,v0,u1,v1, ax,ay, w,h, cols,rows, tw,th } */
  const sprites = new Map();
  /** tex 名 → { path, w, h, kind } */
  const texInfo = new Map();
  /** 字体名 → { path, kind }（.fnt 位图字体 / .ttf），留给文本层用 */
  const fonts = new Map();

  const K_TEX = 1, K_IMG = 2, K_ANI = 3, K_FNT = 7, K_TTF = 8;

  /* 文本层：字体登记在资源表解析时做，真正的加载在 prepare() */
  const text = createTextLayer({ gl, assetBase, assets });
  /* 后期效果执行器：世界层靠它从 RenderTarget 贴回屏幕 */
  const postfx = createPostFX(gl);

  function loadResourceTable() {
    const csv = Module.ccall('rgl_res_csv', 'string', [], []);
    const rows = csv.split('\n');
    if (rows.length < 2) return { rows: 0, sprites: 0 };
    const head = rows[0].split(',');
    const col = (n) => head.indexOf(n);
    const cName = col('name'), cKind = col('kind'), cTex = col('tex'), cPath = col('path');
    const cX = col('x'), cY = col('y'), cW = col('w'), cH = col('h');
    const cTW = col('tw'), cTH = col('th'), cCols = col('cols'), cRows = col('rows');
    const cCX = col('cx'), cCY = col('cy');
    /* hs/vs = SetImageScale(name, s) 的 per-resource unitsPerPixel。
       引擎在 SpriteRenderer::setTransform 里把整个切图矩形乘上它，
       不是乘 Lua Render 的 hscale。漏掉这一项会把弹幕放大 2~2.67 倍
       （游戏里 766 个 sprite 设过非 1 的值）。 */
    const cHS = col('hs'), cVS = col('vs');
    const cSampler = col('sampler');
    /* hc = 旋转中心是否被显式设过（见 rgl_resource.c 的 has_center 注释）。
       缺列（老 dump）时按 0 处理，退化成「一律居中」。 */
    const cHC = col('hc');
    /* 1d-7：动画帧间隔（timer / interval 才换一帧）。老 dump 没这列 ⇒ 退化 1。 */
    const cInterval = col('interval');

    /* 每个 Res 只在自己那行带齐信息：纹理行有 path 没有 tex，sprite 行有 tex
       没有 path。所以分两遍——先建「名字 → 文件路径」索引，再挂 sprite。 */
    const parsed = [];
    for (let i = 1; i < rows.length; i++) {
      const line = rows[i];
      if (!line || line[0] === '#') continue;
      const f = line.split(',');
      if (f.length < head.length) continue;
      parsed.push(f);
    }

    for (const f of parsed) {
      const kind = +f[cKind];
      if (kind !== K_TEX && kind !== K_FNT && kind !== K_TTF) continue;
      const name = f[cName];
      if (!name) continue;
      const rec = { path: f[cPath] || '', w: +f[cTW] || 0, h: +f[cTH] || 0,
                    kind, sampler: f[cSampler] || '' };
      texInfo.set(name, rec);
      if (kind !== K_TEX) {
        fonts.set(name, rec);
        // TTF 的 LoadTTF 把字号存在 w 列（r->w），.fnt 的度量在文件里
        text.registerFont(name, {
          kind, path: rec.path,
          size: kind === K_TTF ? (+f[cW] || 0) : 0,
        });
      }
    }

    for (const f of parsed) {
      const kind = +f[cKind];
      if (kind !== K_IMG && kind !== K_ANI) continue;
      const tex = f[cTex];
      if (!tex) continue;

      let tw = +f[cTW] || 0, th = +f[cTH] || 0;
      /* sprite 行理论上由 C 侧补好纹理尺寸了（res_tex_dims）。万一没有
         （老 dump、纹理行缺 tw），退回纹理自己那行的尺寸——UV 归一化用的
         是纹理尺寸，用错就整张图变成边缘像素拉伸。 */
      if (!tw || !th) {
        const ti = texInfo.get(tex);
        if (ti && ti.w && ti.h) { tw = ti.w; th = ti.h; }
      }

      const w = +f[cW], h = +f[cH];
      /* 锚点：引擎默认是切图中心，但 SetImageCenter 可以显式设成任意值，
         **(0,0) 是完全合法的「左上角锚点」**（top_bar_back 就是这种）。
         以前只看 cx==cy==0 就当作「没设过」，把左上角锚点当成中心，
         结果顶栏只画出左上四分之一块。C 侧现在多给一列 hc 表「设过没有」。 */
      let ax = +f[cCX], ay = +f[cCY];
      if (!(+f[cHC] || 0)) { ax = w / 2; ay = h / 2; }

      sprites.set(f[cName], {
        tex,
        u0: (+f[cX]) / (tw || 1),
        v0: (+f[cY]) / (th || 1),
        u1: ((+f[cX]) + w) / (tw || 1),
        v1: ((+f[cY]) + h) / (th || 1),
        ax, ay, w, h,
        /* ⚠️ 不能用 `+f[cHS] || 1`：`SetImageScale(name, 0)` 是补丁层隐藏
           资源的手段，而 0 是 falsy，会被悄悄变回 1（「外框还在」的根因）。 */
        hs: (f[cHS] !== '' && f[cHS] != null) ? +f[cHS] : 1,
        vs: (f[cVS] !== '' && f[cVS] != null) ? +f[cVS] : 1,
        cols: +f[cCols] || 1,
        rows: +f[cRows] || 1,
        interval: +f[cInterval] > 0 ? +f[cInterval] : 1,
        tw, th,
        sub: null,          // 动画用：precomputed 帧矩形
      });
    }

    // 动画预先算好每一帧的 uv，避免每帧做除法。
    // 1d-7：LoadAnimation 的 w/h 是**单帧**尺寸（引擎
    // ResourceAnimationImpl.cpp:35-40 是 x + w*i / y + h*j），不是整张网格
    // 的总宽高。以前这里又除以 cols/rows，level_obj_ningyou_soul 的 8 帧
    // 全挤进 obj5.png 前 32px（每帧仅 4px），再被 quadSprite 拉成 32px ⇒
    // 蓬莱人形玩偶渲染成竖条纹/半边人。步进直接用 w/h。
    for (const [, sp] of sprites) {
      if (sp.cols > 1 || sp.rows > 1) {
        const fw = sp.w, fh = sp.h;
        const frames = [];
        const tw = sp.tw || 1, th = sp.th || 1;
        for (let r = 0; r < sp.rows; r++) {
          for (let c = 0; c < sp.cols; c++) {
            const x = sp.u0 * tw + c * fw, y = sp.v0 * th + r * fh;
            frames.push({
              u0: x / tw, v0: y / th, u1: (x + fw) / tw, v1: (y + fh) / th,
            });
          }
        }
        sp.sub = frames;
      }
    }
    return { rows: parsed.length, sprites: sprites.size, texes: texInfo.size };
  }

  /* 引擎内置纹理：没有文件，用 1×1 纯白合成。所有 RenderRect / Render4V
     都靠它上色（顶点色 × 白 = 顶点色）。 */
  const BUILTIN_TEX = new Set(['$white', '$white1', '$white8', '$white_rect']);

  /* ---- 纹理 ------------------------------------------------------------ */
  const textures = new Map();          // tex 名 → WebGLTexture
  const pending = new Set();
  const failed = new Set();
  /* 失败**不立刻判死**（Misty 1d-22）。过去这里一张图 HTTP 抖一下就永久进
     failed，玩家那一整局都看不到它 ——「很多资源加载不出来」有一半是这个：
     Cloudflare 边上偶发超时/连接重置，图上不来就再也没人重试。
     现在每张图最多试 MAX_TEX_ATTEMPTS 次（指数退避），全用尽才进 failed，
     另外 exposed retryFailedTextures() 让玩家/诊断可以整体重来一次。 */
  const attempts = new Map();
  const MAX_TEX_ATTEMPTS = 3;
  const failLog = [];              // 最终判死的名字 + 试了几次（诊断用）
  /* 资源表是分帧增长的（loading 场景每帧加载一批），启动时读到的只是
     其中一部分。查不到的名字先放这里，等下一轮刷新资源表再重试；
     直接进 failed 会让「后来才注册」的贴图永远画不出来。 */
  const deferred = new Set();
  /* 1d-4：这里按**墙钟**而不是「渲染帧数」计时。渲染帧解耦后会跟显示器
     刷新率走（144/240Hz），用帧数计时会让资源表重读频率翻 2~4 倍。
     300 帧 @60Hz = 5 s；缺图时 30 帧 = 0.5 s。 */
  let lastReloadMs = -1e9;
  let lastResVersion = -1;       // C 侧 rgl_resource_version() 的上次值
  /* 关卡切换时脚本会现场 LoadTexture2/LoadImageFromFile 注册一批新资源，
     原来的「每 30 帧重读一次」会让刚进场的背景贴图空半秒；而且暂停/快进
     的调试路径永远跑不满 30 帧。发现查不到路径时置脏，下一帧立刻重读。 */
  let tableDirty = false;
  /* 这一帧请求过但拿不到像素的名字，诊断用 */
  const missingSet = new Set();
  let texLoaded = 0, texFailed = 0;

  /* ---- 加载队列：并发上限 + 巨图降采样 --------------------------------
     为什么要限流：资源表里有 673 张图，切屏时脚本会一口气注册上百张。
     以前是「来一张就 new Image()」，等于把并发数交给浏览器（6~8 个 TCP
     连接）和 decoder（无上限）。后果是：
       · 换关/换界面瞬间几十张巨图同时解码，主线程被 decode 挤住掉帧；
       · mission_back 是 5120x2880（59MB 显存），三五个一起上传直接把
         移动端/低端机的显存打满，表现为「图加载不出来」而不是报错。
     限成 6 个槽位后，队列顺序稳定、帧时间平稳，而且失败重试不会雪崩。

     巨图降采样：单边超过 MAX_TEX_DIM 就在上传前用 createImageBitmap 缩放。
     这张游戏里 5120x2880 的 mission_back 缩到 2048 后显存从 59MB 降到
     9.4MB，而它在屏幕上最大也就铺满 1280 宽，肉眼无差别。
     **uv 不受影响**：uv 是按资源表里的逻辑尺寸归一化的，缩放只改采样精度。 */
  const LOAD_MAX = 6;
  const MAX_TEX_DIM = 2048;
  let loading = 0;
  const loadQueue = [];
  const downscaled = [];        // 诊断：被降采样的纹理（名字 + 原始尺寸）

  function pumpLoads() {
    while (loading < LOAD_MAX && loadQueue.length) {
      const job = loadQueue.shift();
      loading++;
      Promise.resolve()
        .then(job)
        .catch(() => {})
        .then(() => { loading--; pumpLoads(); });
    }
  }

  /* 图片过大就等比缩到 MAX_TEX_DIM 以内。createImageBitmap 的 resize 走
     浏览器内部缩放（通常 GPU 加速），比先画 canvas 再取回快得多；
     浏览器不支持 resize 参数时退回原图，只损失内存不损失正确性。 */
  async function fitSize(img) {
    const w = img.naturalWidth || img.width || 0;
    const h = img.naturalHeight || img.height || 0;
    const m = Math.max(w, h);
    if (!w || !h || m <= MAX_TEX_DIM || typeof createImageBitmap !== 'function') return img;
    const k = MAX_TEX_DIM / m;
    const rw = Math.max(1, Math.round(w * k));
    const rh = Math.max(1, Math.round(h * k));
    try {
      /* resizeQuality 用 'medium'（Misty 1d-23）。'high' 对 5120x2880 这种
         巨图要做多级高质量重采样，实测能占住主线程几百毫秒；而这批图的
         最终显示宽度只有 1280 左右，medium 与 high 肉眼无差。 */
      return await createImageBitmap(img, {
        resizeWidth: rw, resizeHeight: rh, resizeQuality: 'medium',
      });
    } catch (e) {
      return img;
    }
  }

  /* 同步 URL 只用于诊断输出；真正取地址走 assets.url()（导入包时是异步的）。 */
  const texUrl = (p) => assets.httpUrl(p);

  /* 上传一张纹理。**必须原样恢复当前的 TEXTURE_2D 绑定**：
     图片是异步 load 的，回调可能正好落在某个批次还没 flush 的时候，
     直接 bind 会把这条批次里已攒的顶点换成新纹理（表现为「一部分贴图
     突然变成另一张图」，极其难查）。 */
  function uploadTexture(name, img) {
    const prev = gl.getParameter(gl.TEXTURE_BINDING_2D);
    const t = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, t);
    gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, false);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, img);
    // 弹幕贴图基本都是像素对齐的，默认线性 + clamp；点采样由 ROP_SAMPLER 覆盖
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    /* 3D 背景的地面/水面用 0..2048 像素的 uv 平铺（LoadTexture2 设
       "linear+wrap"）；CLAMP 会把整片拉成纹理边缘的一条颜色。 */
    const smp = texInfo.get(name);
    const wrap = (smp && smp.sampler.includes('wrap')) ? gl.REPEAT : gl.CLAMP_TO_EDGE;
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, wrap);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, wrap);
    gl.bindTexture(gl.TEXTURE_2D, prev);
    textures.set(name, t);
    texLoaded++;
    /* 降采样产物是 ImageBitmap，上传完必须 close()，否则那块内存要等 GC
       才释放（巨图一张几十 MB，不主动释放等于限流白做）。 */
    if (img && typeof img.close === 'function' && !(img instanceof Image)) img.close();
  }

  /** 1×1 纯白纹理：引擎的 $white 系列是内置资源，没有对应文件 */
  function makeWhite(name, rgba = null) {
    const prev = gl.getParameter(gl.TEXTURE_BINDING_2D);
    const t = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, t);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE,
                  new Uint8Array(rgba || [255, 255, 255, 255]));
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.bindTexture(gl.TEXTURE_2D, prev);
    textures.set(name, t);
    texLoaded++;
    return t;
  }

  async function requestTexture(name) {
    if (textures.has(name) || pending.has(name) || failed.has(name)) return;
    if (BUILTIN_TEX.has(name)) { makeWhite(name); return; }
    if (targets.has(name)) return;   // 渲染目标，不是文件纹理
    const info = texInfo.get(name);
    if (!info || !info.path) {
      deferred.add(name);
      missingSet.add(name);
      tableDirty = true;
      return;
    }
    /* 字体（.fnt/.ttf/.otf）不是 Image 能 load 的东西，交给 text.mjs。
       不过滤掉的话，每次预加载都会把它们记成「纹理加载失败」，把真正的
       404 淹掉（踩过：failedTextures 里 8 条全是字体，看着像全崩了）。 */
    if (info.kind === K_FNT || info.kind === K_TTF) return;
    pending.add(name);
    /* 只排队，不立刻发请求：真正的 decode 由 pumpLoads() 按并发上限跑。
       pending 在排队阶段就要置上，因为调用方（requestAll / bindTex）
       靠它判断「已经在处理中」，否则同名的会重复入队。 */
    loadQueue.push(async () => {
      if (textures.has(name) || failed.has(name)) { pending.delete(name); return; }
      try {
        const url = await assets.url(info.path);
        const img = new Image();
        const loaded = new Promise((resolve, reject) => {
          img.onload = () => resolve(true);
          img.onerror = () => reject(new Error('img'));
        });
        img.src = url;
        await loaded;
        /* 必须在 texImage2D 之前**显式异步解码**（Misty 1d-23）。
           `img.onload` 只代表字节到齐，真正的解码会被推迟到第一次使用 ——
           而 `texImage2D(..., img)` 是同步的，一张 5120x2880 的图能卡主线程
           几十毫秒。首屏后连续预热五百多张，就等于每张都卡一下，玩家的
           体感是「移动不跟手 + 画面一顿一顿」。`decode()` 把解码挪到
           异步任务里，主线程只在最后上传时付一次拷贝的代价。
           解码失败不在这里判死：交给 uploadTexture 抛错，走统一的退避重试。 */
        if (typeof img.decode === 'function') {
          try { await img.decode(); } catch { /* 留给上传阶段报错 */ }
        }
        const sized = await fitSize(img);
        if (sized !== img) {
          downscaled.push(`${name}:${img.naturalWidth}x${img.naturalHeight}->` +
            `${sized.width}x${sized.height}`);
        }
        uploadTexture(name, sized);
        attempts.delete(name);
        pending.delete(name);
      } catch (e) {
        pending.delete(name);
        /* 失败不立刻判死（Misty 1d-22）：Cloudflare 边上偶发超时/连接重置
           很常见，过去一次失败就永久进 failed，那一整局这张图都不会再出现
           —— 玩家看到的就是「很多图片加载不出来」。退避后重排队，
           三次都失败才认输（并记进 failLog 供诊断）。 */
        const n = (attempts.get(name) || 0) + 1;
        attempts.set(name, n);
        if (n < MAX_TEX_ATTEMPTS) {
          setTimeout(() => requestTexture(name), 300 * n * n);
        } else {
          failed.add(name);
          texFailed++;
          if (failLog.length < 24) failLog.push(name + ' ×' + n);
        }
      }
    });
    pumpLoads();
  }

  /* 首帧之前预热。有导入包时全部素材都在本地 Blob，可以全量预热；
     没包时走 HTTP，393MB 全量拉取不现实 —— 只预热启动界面素材，
     其余由 bindTex() 在实际绘制到时才懒加载（会晚一两帧补上）。 */
  async function requestAll({ filter = null, budgetMs = 0 } = {}) {
    const jobs = [];
    for (const [name, info] of texInfo) {
      if (filter && !filter(info.path || '', name)) continue;
      requestTexture(name);
      if (pending.has(name)) {
        jobs.push(new Promise((resolve) => {
          // requestTexture 已经在跑；这里用轮询等它离开 pending
          const tick = () => {
            if (pending.has(name)) { setTimeout(tick, 8); return; }
            resolve();
          };
          setTimeout(tick, 0);
        }));
      }
    }
    // 字体与纹理并行：.otf/.ttf 各有 10~16MB，串起来首屏要多等一拍
    const fontJob = text.prepare().catch((e) => ({ error: String(e) }));
    const texJob = Promise.all(jobs);
    // #35 加载优化：给了预算就只等这么久。没等到的那部分仍在 pending 队列里继续
    // 下载/解码，bindTex() 真正绘制到时补上（晚一两帧）——线上 536 张贴图全等齐
    // 要几十秒，正是启动器超时的主因。字体始终等齐：首屏几乎全是文字。
    if (budgetMs > 0) {
      await Promise.race([texJob, sleepMs(budgetMs)]);
      return await fontJob;
    }
    const [, fontDiag] = await Promise.all([texJob, fontJob]);
    return fontDiag;
  }

  /* ---- 后台预热（Misty 1d-22） -----------------------------------------
     线上站点是「用到才取」：进选关界面才开始下 mission_back，打开图鉴才
     一张张抓 245 个道具图标 —— 玩家看到的就是「图片一个个蹦出来」，
     慢一点还会以为图挂了（没加载完的 sprite 直接被跳过不画）。
     这里在首屏稳定之后，用**很低的水位**把整套贴图默默取完：
       · 队列里最多留 watermark 个任务，前台请求永远排得进（FIFO 队列，
         预热把水位占满就会挡住建制中的新图）；
       · 顺序由调用方给的 hot 前缀表决定（选关/图鉴/道具在前，背景在后）；
       · 页面切到后台就停手，回到前台继续；
       · 省流量模式 / 2G-3G 由调用方跳过（这里不管策略，只管排队）。
     调用方不 await：任何失败都只是少预热几张，不影响游戏。 */
  /* 1d-23：水位从 3 提到 8。资源已经在首启阶段全量进了 HTTP 缓存，
     每次「取图」只剩本地读盘 + 解码 + 上传，前台的请求也不会慢；水位太低
     反而让 534 张图排上十几秒，玩家进关卡时还在「一张张往外蹦」。 */
  /* 1d-27：8 → 3。后台预热只是「提前把贴图收进来」，不该让 8 张图同时
     解码 + 上传挤占主线程；关卡里每多一次几十毫秒的阻塞就是一次「顿」。
     配合下面循环末尾的每帧一张，把负载摊到帧间。 */
  const PREFETCH_WATERMARK = 3;
  let prefetchLast = null;         // 上一次预热的成绩单（诊断用）
  const sleepMs = (ms) => new Promise((r) => setTimeout(r, ms));
  const pageHidden = () => (typeof document !== 'undefined' && document.hidden === true);

  function prefetchTargets(prefixes) {
    const list = [];
    const fallback = prefixes.length;
    for (const [name, info] of texInfo) {
      if (!info || !info.path) continue;
      if (info.kind === K_FNT || info.kind === K_TTF) continue;   // 字体走 text 层
      if (textures.has(name) || pending.has(name) || failed.has(name)) continue;
      const p = info.path.replace(/\\/g, '/').toLowerCase();
      let rank = fallback;
      for (let i = 0; i < prefixes.length; i++) {
        if (p.indexOf(prefixes[i]) >= 0) { rank = i; break; }
      }
      if (rank === fallback) continue;
      list.push({ name: name, rank: rank, p: p });
    }
    list.sort((a, b) => (a.rank - b.rank) || (a.p < b.p ? -1 : (a.p > b.p ? 1 : 0)));
    return list;
  }

  async function prefetch(prefixes, opts = {}) {
    const signal = opts.signal || null;
    const mark = opts.watermark == null ? PREFETCH_WATERMARK : opts.watermark;
    const onProgress = opts.onProgress || null;
    /* 1d-53 task#5.2：原来是一次性快照 —— 只能覆盖「调用那一刻已经在
       texInfo 里」的贴图。stage init() / wave_init() 之后 LoadTexture 才
       注册的（sceneN_pic、season_icon、boss aura、关卡背景…）永远不在
       那张快照里，只能等玩家真的看到那一帧，由 bindTex 临时发起取回 ⇒
       表现就是「素材从无到有蹦出来」。持续模式每轮重扫 texInfo，新注册
       项也进后台低水位队列；水位仍是 3，前台 bindTex 永远排得进。 */
    const continuous = !!opts.continuous;
    const rescanMs = opts.rescanMs == null ? 600 : opts.rescanMs;
    const t0 = performance.now();
    let done = 0, skipped = 0, rounds = 0, total = 0;
    for (;;) {
      const list = prefetchTargets(prefixes || []);
      total = Math.max(total, list.length);
      /* 一开跑就留诊断快照：第一轮可能要十几秒，别让 prefetchStats()
         在这段时间里返回 null。 */
      if (continuous) {
        prefetchLast = { total: total, done: done, skipped: skipped, rounds: rounds,
                         live: true, ms: Math.round(performance.now() - t0) };
      }
      for (let i = 0; i < list.length; i++) {
        if (signal && signal.aborted) break;
        if (pageHidden()) { await sleepMs(500); i--; continue; }
        /* 水位：queue + 在飞的任务数要留出空位，否则前台新请求会排在几百个
           预热任务后面（首屏之后还会注册新贴图，这个必须让路）。 */
        let guard = 0;
        while (loadQueue.length + loading > mark) {
          if (signal && signal.aborted) break;
          await sleepMs(16);
          if (++guard > 6000) break;        // 兜底：2 分钟还没降下来就继续排队
        }
        const it = list[i];
        if (textures.has(it.name) || pending.has(it.name) || failed.has(it.name)) {
          skipped++;
          continue;
        }
        requestTexture(it.name);
        done++;
        if (onProgress) {
          try { onProgress({ done: done, total: list.length, skipped: skipped,
                             name: it.name, round: rounds }); }
          catch (e) { /* 进度回调不许影响预热 */ }
        }
        /* 1d-27：一帧最多发一张。原来只有「水位满」时才 sleep，于是每次
           队列一空就连续发 8 张 —— createImageBitmap + texImage2D 全挤在
           同一两个 rAF 里，玩家看到的就是走位突然顿一下、偶发长阻塞。 */
        await sleepMs(16);
      }
      if (!continuous) break;
      rounds++;
      /* 持续模式不会走到函数末尾，诊断必须在这里实时刷新：
         verify_site_assets / verify_1d27_pacing 都靠 prefetchStats() 判断
         后台预热确实跑过，返回 null 会被判成回归。 */
      prefetchLast = { total: total, done: done, skipped: skipped, rounds: rounds,
                       live: true, ms: Math.round(performance.now() - t0) };
      await sleepMs(rescanMs);
      if (signal && signal.aborted) break;
    }
    prefetchLast = { total: total, done: done, skipped: skipped, rounds: rounds,
                     ms: Math.round(performance.now() - t0) };
    return { total: total, done: done, skipped: skipped, rounds: rounds };
  }
  /** 把判死的贴图整体重来一次（玩家可在调试面板 / 控制台调用）。 */
  function retryFailedTextures() {
    const list = [...failed];
    for (const n of list) {
      failed.delete(n);
      attempts.delete(n);
      texFailed--;
    }
    for (const n of list) requestTexture(n);
    return list.length;
  }

  /* ---- 渲染目标 -------------------------------------------------------- */
  const targets = new Map();     // name → { fbo, tex, w, h }
  /* RenderTexture 的 uv 按原版 D3D9 语义是 **WRAP 寻址**（uv 可越界平铺）。
     上传纹理默认 CLAMP，只有 sampler 名带 'wrap' 的（3D 背景地面/水面）
     才是 REPEAT —— 但 mod/GAME/th09.lua 的毒雾（class.Poison）直接
     RenderTexture("Poison", ...)，uv = imgx±100、imgx ∈ [-256,256]，
     大半落在 [0,1] 之外：CLAMP 会把边缘一行像素拉成整条色带，随机
     imgx 让它「有概率」出现（bugs.txt #6 毒雾异常拉伸）。
     这里在 DRAW_TEX 时检测 uv 越界，按需给该纹理补 REPEAT（一次性，
     WebGL2 对 NPOT 同样支持 repeat）。渲染目标保持 CLAMP 不动。 */
  const wrapApplied = new Set();
  function ensureWrap(name) {
    if (wrapApplied.has(name)) return;
    const t = textures.get(name);
    if (!t) return;
    wrapApplied.add(name);
    const prevTex = gl.getParameter(gl.TEXTURE_BINDING_2D);
    gl.bindTexture(gl.TEXTURE_2D, t);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
    gl.bindTexture(gl.TEXTURE_2D, prevTex);
    if (curTex === name) curTex = null;   // 绑定被动过，缓存作废下帧重绑
  }
  const targetStack = [];

  function getTarget(name) {
    let rt = targets.get(name);
    if (rt) return rt;
    const w = canvas.width, h = canvas.height;
    /* TEXTURE_BINDING_2D / FRAMEBUFFER_BINDING 都是**全局绑定点**。
       这里曾在渲染循环里新建 RT 却不还原：单元 0 停在空白 rt.tex、
       curTex 还记着上一张 sprite，bindTex 命中缓存跳过重绑 → 后续贴图
       整片消失；FRAMEBUFFER 被硬置 null 还会让嵌套 RT 的 POP 恢复错位。
       uploadTexture / makeWhite / text.createTexture 都已按这个纪律修过，
       这是第六个漏点。 */
    const prevTex = gl.getParameter(gl.TEXTURE_BINDING_2D);
    const prevFbo = gl.getParameter(gl.FRAMEBUFFER_BINDING);
    const tex = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, tex);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, w, h, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    const fbo = gl.createFramebuffer();
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, tex, 0);
    /* 渲染目标也挂一个深度附件：背景把 3D 画进 RT 时（WorldTex1 之类），
       没有深度就没法做 z 测试，遮挡关系会退化。 */
    const depthRb = gl.createRenderbuffer();
    gl.bindRenderbuffer(gl.RENDERBUFFER, depthRb);
    gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, w, h);
    gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT,
      gl.RENDERBUFFER, depthRb);
    gl.bindRenderbuffer(gl.RENDERBUFFER, null);
    /* 新 RT 的初始内容不该被赌成「全 0」：着色器在建立的同一帧就可能开始
       采样，脏值会被当成不透明颜色画到屏幕上 —— 表现为「特效刚出现的
       一瞬间战场矩形闪一块黑」（bugs #16 遗留）。建好即清成透明黑。 */
    gl.clearColor(0, 0, 0, 0);
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.bindFramebuffer(gl.FRAMEBUFFER, prevFbo);
    gl.bindTexture(gl.TEXTURE_2D, prevTex);
    curTex = null;                 // 单元 0 被动过，缓存一律作废
    rt = { fbo, tex, w, h, depthRb };
    targets.set(name, rt);
    textures.set(name, tex);      // 渲染目标可以当纹理采样
    /* 关键：渲染目标也要登记进资源表。background.Capture 用
       RenderTexture("WorldTex1", "", ...) 把整帧贴回屏幕，其 uv 是屏幕
       像素坐标（WorldToScreen），需要除以纹理尺寸才能归一化到 0..1；
       查不到 texInfo 时 DRAW_TEX 会拿 tw=th=1 兜底，uv 变成 0..1024，
       CLAMP 之后整幅画面退化成目标纹理的一个边缘像素（通常是清屏黑），
       表现就是「3D 背景画了却完全看不见」。 */
    texInfo.set(name, { path: '', w, h, kind: K_TEX, sampler: '' });
    /* 这个名字可能先被当成「缺文件的纹理」记过一次失败（fade_effect 之类
       由 CreateRenderTarget 生成，没有资源表路径）。目标一建好就撤销误报。 */
    if (failed.delete(name)) texFailed--;
    return rt;
  }

  /* ---- 状态 ------------------------------------------------------------ */
  let ortho = { l: 0, r: 960, b: 540, t: 0 };
  let curBlend = -1;
  let vcApplied = false;      // u_vc 是否已在当前 program 上设好
  let curTex = null;             // 当前绑定的纹理：sprite 名（字符串）或 WebGLTexture
  /* 深度测试开关：由 game 的 SetZBufferEnable 控制（只有 3D 背景会打开），
     每帧复位，避免一关的背景把后续 2D 绘制挡掉。 */
  let depthOn = false;
  let texEpochSeen = 0;          // 上次见到的 text.atlasEpoch，用来发现「建过新纹理」
  let vcount = 0;
  /* 引擎 RenderTTF 的 scale 还要再乘 globalImageScaleFactor（见 LW_Render.cpp），
     游戏开了 GLOBAL_SCALE_COLLI_SHAPE，这个值可能被脚本改，所以从 C 侧取。 */
  let imageScale = 1;
  /* 当前正交投影的 y 方向：t > b 即 y 向上（游戏自己的 SetOrtho 就是这样） */
  let yUp = true;

  /* ---- 3D 视角 ---------------------------------------------------------
     关卡背景（assets/background 下各关的 bg.lua）走 SetViewMode("3d")：
     SetPerspective 给出相机，随后 RenderTexture / Render4V 给的是
     (x,y,z) 三维顶点。此前 ROP_PERSPECTIVE 被当成 no-op、quad4 又把 z
     丢掉，三维顶点被当成屏幕像素坐标画 —— 背景缩成几像素的碎片，看起来
     就是「playfield 一片纯黑、没有关卡背景」。

     这里不复刻 D3D 的常量缓冲，而是在 CPU 侧做 LookAtLH × PerspectiveFovLH
     （与 Renderer_D3D11.cpp:972 完全一致），把结果换算回当前正交投影的
     输入空间：顶点着色器仍然是 vec2 + u_scale/u_offset，不用改 shader，
     也不用动 VBO 布局。代价是深度值被压平（gl_Position.z=0），遮挡关系
     靠命令顺序（游戏本来就是按远→近发命令）。 */
  let curScale = { sx: 1, sy: 1, ox: 0, oy: 0 };
  let cam = null;
  let proj3d = false;
  /* 线性雾（SetFog(near, far, color)）。引擎用 FOG_ENABLE 的 shader 变体，
     像素着色器里按「相机到片元的世界距离」把颜色 lerp 到雾色。
     SetRenderRect 会调 SetFog() 清掉雾，所以这里只在 3D 段生效。 */
  let fog = null;
  /* 雾 uniform 只在参数变化后重设一次；flush 是唯一保证 sprite program
     处于激活状态的时机（postfx 有自己的 program，在那边写 location 会报错）。 */
  let fogDirty = true;
  /* 诊断开关：关掉就退回「屏幕空间线性插值 uv」的旧行为，用于 A/B 取证 */
  let perspectiveCorrect = true;

  function applyOrtho() {
    const { l, r, b, t } = ortho;
    const sx = 2 / (r - l), sy = 2 / (t - b);
    const ox = -2 * l / (r - l) - 1, oy = -2 * b / (t - b) - 1;
    curScale = { sx, sy, ox, oy };
    gl.uniform2f(uScale, sx, sy);
    gl.uniform2f(uOffset, ox, oy);
  }

  const v3sub = (a, b) => [a[0] - b[0], a[1] - b[1], a[2] - b[2]];
  const v3cross = (a, b) => [a[1] * b[2] - a[2] * b[1],
                             a[2] * b[0] - a[0] * b[2],
                             a[0] * b[1] - a[1] * b[0]];
  const v3norm = (a) => {
    const n = Math.hypot(a[0], a[1], a[2]) || 1;
    return [a[0] / n, a[1] / n, a[2] / n];
  };

  function setCamera(f) {
    const eye = [f(0), f(1), f(2)], at = [f(3), f(4), f(5)];
    const up = [f(6), f(7), f(8)];
    const fovy = f(9) || 1, aspect = f(10) || 1, zn = f(11) || 0.1;
    const zf = f(12) || 100;
    /* DirectX::XMMatrixLookAtLH 的 zaxis = normalize(At - Eye)（不是 Eye-At）。
       写反的话所有背景顶点都落在相机背后，被近平面判成 null 整块丢弃。 */
    const z = v3norm(v3sub(at, eye));
    let x = v3cross(up, z);
    if (Math.hypot(x[0], x[1], x[2]) < 1e-6) x = [1, 0, 0];
    x = v3norm(x);
    const y = v3cross(z, x);
    const t = Math.tan(fovy * 0.5) || 1;
    cam = { ex: eye[0], ey: eye[1], ez: eye[2], x, y, z,
            xs: 1 / (aspect * t), ys: 1 / t, zn, zf };
    gl.uniform1f(uZNear, zn);
    gl.uniform1f(uZFar, zf);
    proj3d = true;
  }

  /* 世界坐标 → 相机空间 [vx, vy, vz]（vz 就是透视除法的 w） */
  function camSpace(px, py, pz) {
    const dx = px - cam.ex, dy = py - cam.ey, dz = pz - cam.ez;
    return [cam.x[0] * dx + cam.x[1] * dy + cam.x[2] * dz,
            cam.y[0] * dx + cam.y[1] * dy + cam.y[2] * dz,
            cam.z[0] * dx + cam.z[1] * dy + cam.z[2] * dz];
  }
  /* NDC → applyOrtho 的输入坐标，顶点着色器不用改 */
  const ndcToInput = (nx, ny) =>
    [(nx - curScale.ox) / curScale.sx, (ny - curScale.oy) / curScale.sy];

  /* 近平面裁剪（Sutherland-Hodgman，平面 vz >= zn）。
     背景地面是 z=-6..10 的大四边形，相机在 z=-1，必然横跨近平面；
     不裁剪就得整块丢弃，结果就是「关卡背景永远看不见」。
     每个顶点携带 uv 与颜色，交点处线性插值，保证平铺纹理连续。 */
  function clipNear(vs, zn) {
    const out = [];
    for (let i = 0; i < vs.length; i++) {
      const a = vs[i], b = vs[(i + 1) % vs.length];
      const ain = a.c[2] >= zn, bin = b.c[2] >= zn;
      if (ain) out.push(a);
      if (ain !== bin) {
        const t = (zn - a.c[2]) / (b.c[2] - a.c[2]);
        out.push({
          c: [a.c[0] + (b.c[0] - a.c[0]) * t,
              a.c[1] + (b.c[1] - a.c[1]) * t, zn],
          u: a.u + (b.u - a.u) * t,
          v: a.v + (b.v - a.v) * t,
          col: a.col.map((x, k) => x + (b.col[k] - x) * t),
        });
      }
    }
    return out;
  }

  const BLEND_FACTOR = {
    one: gl.ONE, zero: gl.ZERO,
    srcAlpha: gl.SRC_ALPHA, oneMinusSrcAlpha: gl.ONE_MINUS_SRC_ALPHA,
    srcColor: gl.SRC_COLOR, oneMinusSrcColor: gl.ONE_MINUS_SRC_COLOR,
    dstColor: gl.DST_COLOR, oneMinusDstColor: gl.ONE_MINUS_DST_COLOR,
  };
  const BLEND_EQ = {
    add: gl.FUNC_ADD, sub: gl.FUNC_SUBTRACT, revSub: gl.FUNC_REVERSE_SUBTRACT,
    min: gl.MIN, max: gl.MAX,
  };

  /* setVc=false 供后期效果复用：它有自己的 program，此时写 u_vc（属于 sprite
     program 的 uniform location）会触发 GL_INVALID_OPERATION，也设不进去。
     混合因子是全局状态，两边共用没问题；顶点色模式等回到 sprite program 再补。 */
  function applyBlend(id, setVc = true) {
    if (id === curBlend && setVc === vcApplied) return;
    flush();                       // 先按旧状态把攒的顶点画掉，否则会串状态
    curBlend = id;
    vcApplied = setVc;
    const m = BLEND[id] || BLEND[1];
    const F = BLEND_FACTOR, E = BLEND_EQ;
    gl.blendFuncSeparate(F[m[2]], F[m[3]], F[m[4]], F[m[5]]);
    // min/max 方程下因子被忽略，但 alpha 方程必须跟颜色一致，否则 alpha 走 add
    const aeq = (m[1] === B_MIN || m[1] === B_MAX) ? E[m[1]] : E.add;
    gl.blendEquationSeparate(E[m[1]], aeq);
    if (setVc) gl.uniform1i(uVc, m[0]);
  }

  /* ---- 道具高光：运行时生成的径向渐变（1d-14 task#5）-------------------
     game/assets/UI/menus/pure_circle.png 是一张**纯白实心圆盘**，关卡内 HUD
     拿它当道具高光（'mul+add' + 等级色），于是每个道具下面都垫着一个硬边
     小圆片 —— 清酒说"高光只是叠了个圈在道具下面，非常丑陋"。
     这里改成一张 128×128 的白→全透明渐变，中心有一小段接近满亮的核心，
     往外按 (1-t)^1.7 衰减。仍是单 quad、零额外 draw call。
     只在关卡内替换（C 侧 g_glow，Lua 的 addition_hud 切换）—— 图鉴/菜单
     里的 pure_circle 保持原样。 */
  const GLOW_NAME = 'menu_pure_circle';
  let glowTex = null;

  function makeGlow() {
    const N = 128, half = (N - 1) / 2;
    const px = new Uint8Array(N * N * 4);
    for (let y = 0; y < N; y++) {
      for (let x = 0; x < N; x++) {
        const dx = (x - half) / half, dy = (y - half) / half;
        const t = Math.sqrt(dx * dx + dy * dy);
        let a = 0;
        if (t < 1) {
          const k = 1 - t;
          /* 1d-14 task#5：UI.lua 给 menu_pure_circle 的 alpha 固定是 `_a*50`
             （≈0.2），所以贴图自己要够亮才看得见。曲线取 k^1.2（而不是
             更陡的 1.7）：中段仍有 0.4~0.5 的亮度，外沿平滑归零 —— 这才
             读得出「从中心往外发散」，而不是一圈硬边圆盘。 */
          a = Math.min(1, Math.pow(k, 1.2) * 1.25 + (k > 0.6 ? (k - 0.6) * 0.55 : 0));
        }
        const i = (y * N + x) * 4;
        px[i] = 255; px[i + 1] = 255; px[i + 2] = 255;
        px[i + 3] = Math.round(a * 255);
      }
    }
    const prev = gl.getParameter(gl.TEXTURE_BINDING_2D);
    const t = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, t);
    gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, false);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, N, N, 0, gl.RGBA, gl.UNSIGNED_BYTE, px);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.bindTexture(gl.TEXTURE_2D, prev);
    return t;
  }

  function bindTex(name, forceGlow) {
    if (forceGlow) {
      /* 1d-15 task#3：只有 C 侧把绘制名改成 `__rgl_glow` 的调用（= 道具 HUD
         那一格高光）才用渐变贴图。以前是全局开关 + 资源名匹配，结果符卡槽
         的黄色背景也被换成了光晕。 */
      if (!glowTex) glowTex = makeGlow();
      if (curTex !== glowTex) { flush(); gl.bindTexture(gl.TEXTURE_2D, glowTex); curTex = glowTex; }
      return true;
    }
    if (curTex === name) return true;
    let t = textures.get(name);
    if (!t) {
      requestTexture(name);
      t = textures.get(name);   // 内置纹理同步就绪，这一帧就能画
      if (!t) return false;
    }
    flush();                    // 纹理切换 = 新批次
    gl.bindTexture(gl.TEXTURE_2D, t);
    curTex = name;
    return true;
  }

  /* 1d-15：开关不再在这里轮询 —— 改由 C 侧 l_render 直接把绘制名换成
     `__rgl_glow`，renderer 只在 OP.DRAW 里认这个名字。省掉每帧一次 ccall。 */

  /* 后期效果的纹理参数可能是：纹理名（fade_effect）、渲染目标名（mask_fader）、
     或者 sprite 名。逐级回落，都没有就给一张 1×1 白图占位，避免绑定 null。 */
  let blankTex = null, hmapFallback = null;
  function resolveTexForEffect(name) {
    if (name && textures.has(name)) return textures.get(name);
    if (name) {
      const sp = sprites.get(name);
      if (sp && textures.has(sp.tex)) return textures.get(sp.tex);
      requestTexture(name);
      if (textures.has(name)) return textures.get(name);
    }
    /* fade.hlsl 把 u_texture_h 的 r 通道当高度图（hmap 越大越晚显出）：
       1×1 白图 = hmap 恒 1 → a = clamp(height - 1, 0, width) 恒 0 →
       整个画面被擦成全黑。mask_fader 的开门/关门过渡就是这么「消失」的，
       只剩被清屏前画进去的零散粒子，看着像加载瞬间闪出奇怪素材。
       真实 fade_effect.png 还没就绪时改用 hmap=0 的黑图：a 随 height
       均匀变化，至少是一次正常的淡入/淡出；纹理加载完自动切回云雾高度图。 */
    if (name === 'fade_effect') {
      if (!hmapFallback) hmapFallback = makeWhite('#postfx-hmap', [0, 0, 0, 255]);
      return hmapFallback;
    }
    if (!blankTex) blankTex = makeWhite('#postfx-blank');
    return blankTex;
  }

  /* 直接绑一个 WebGLTexture（字形图集走这条路，不进 sprite 的纹理表） */
  function bindRaw(t) {
    if (curTex === t) return;
    flush();
    gl.bindTexture(gl.TEXTURE_2D, t);
    curTex = t;
  }

  function flush() {
    /* vcount==0 时直接走人：切个状态、换个纹理都会叫一次 flush，
       要是在这里计数，统计里会出现一堆假的"没画出来"。 */
    if (!vcount) return;
    gl.bindVertexArray(vao);
    /* 必须显式重绑 vbo：postfx 的 ensureQuad() 会 bindBuffer 到它自己的
       32 字节全屏四边形 VBO，而 ARRAY_BUFFER 是全局绑定点，不会随
       bindVertexArray(null) 恢复。不重绑的话 bufferSubData 会写进那个小
       buffer：既越界报 GL_INVALID_VALUE，又让 sprite VBO 永远是全 0，
       所有顶点退化成 (0,0,0) —— 表现就是「quads 数正常但画面全黑」。 */
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
    if (fogDirty) {
      gl.uniform1i(uFogOn, fog ? 1 : 0);
      if (fog) {
        gl.uniform1f(uFogNear, fog.near);
        gl.uniform1f(uFogFar, fog.far);
        gl.uniform4f(uFogColor, fog.col[0], fog.col[1], fog.col[2], fog.col[3]);
      }
      fogDirty = false;
    }
    gl.bufferSubData(gl.ARRAY_BUFFER, 0, vdata, 0, vcount * FLOATS_PER_VERT);
    gl.drawArrays(gl.TRIANGLES, 0, vcount);
    stat.glDraws++;
    gl.bindVertexArray(null);
    vcount = 0;
  }

  /* ---- 顶点装配 -------------------------------------------------------- */
  /* iw = 1/相机空间 z，2D 顶点恒为 1（着色器乘 1、除 1，行为不变）。
     只有 quad4 的 3D 分支会传非 1 值。 */
  function pushVert(x, y, u, v, r, g, b, a, iw = 1, cx = 0, cy = 0, cz = 0) {
    if (vcount >= MAX_QUADS * VERTS_PER_QUAD) flush();
    const o = vcount * FLOATS_PER_VERT;
    vdata[o] = x; vdata[o + 1] = y; vdata[o + 2] = u; vdata[o + 3] = v;
    vdata[o + 4] = r; vdata[o + 5] = g; vdata[o + 6] = b; vdata[o + 7] = a;
    vdata[o + 8] = iw;
    vdata[o + 9] = cx; vdata[o + 10] = cy; vdata[o + 11] = cz;
    vcount++;
  }

  /* 颜色来自数组时的快速版。原先一律写 `pushVert(x, y, u, v, ...cols[i])`，
     展开运算符每次调用都会新建一个参数数组 —— 一个 quad 6 次，chaos 粒子
     一帧就是上万个一次性数组。这里直接按下标取，零分配。 */
  function pushVertC(x, y, u, v, c) {
    if (vcount >= MAX_QUADS * VERTS_PER_QUAD) flush();
    const o = vcount * FLOATS_PER_VERT;
    vdata[o] = x; vdata[o + 1] = y; vdata[o + 2] = u; vdata[o + 3] = v;
    vdata[o + 4] = c[0]; vdata[o + 5] = c[1]; vdata[o + 6] = c[2]; vdata[o + 7] = c[3];
    vdata[o + 8] = 1;
    vdata[o + 9] = 0; vdata[o + 10] = 0; vdata[o + 11] = 0;
    vcount++;
  }

  const rgbOf = (argb) => [
    ((argb >>> 16) & 255) / 255,
    ((argb >>> 8) & 255) / 255,
    (argb & 255) / 255,
    ((argb >>> 24) & 255) / 255,
  ];

  /* 四顶点色：RopHead @16..31 是 左上/右上/右下/左下 四个 ARGB。
     复用这 4 个数组，避免每帧上千条命令各建 4 个小对象。单色时四个值相同，
     与旧的 rgbOf(color) 完全等价。 */
  const vcCols = [[1, 1, 1, 1], [1, 1, 1, 1], [1, 1, 1, 1], [1, 1, 1, 1]];
  const setRgba = (out, argb) => {
    out[0] = ((argb >>> 16) & 255) / 255;
    out[1] = ((argb >>> 8) & 255) / 255;
    out[2] = (argb & 255) / 255;
    out[3] = ((argb >>> 24) & 255) / 255;
  };
  /* ROP_DRAW_4V_BATCH：批里每条 quad 是四角同色，复用同一组数组，
     18090 条不产生任何临时对象。 */
  const batchCol = [1, 1, 1, 1];
  const batchCols = [batchCol, batchCol, batchCol, batchCol];

  /* 取证用：把每条文本命令画成自己的色相，就能在截图上一眼看出
     「这块像素到底是第几条命令画的」。默认关闭，不影响正常运行。 */
  let debugTint = false;
  /* 测谎仪拖影（bright + mul+add 的连续批）单批最多画的 quad 数。
     拖影表长度 ∝ 半径 r，也 ∝ 瞬时线速度（急转身、高低切时会一帧插进几千点）。

     **必须均匀抽稀，不能截前缀。** self.smear 的追加顺序是「每帧 × 每个子机
     追加一段」，所以表的头部不是「只有最老的」—— 它是「所有子机里最老的那一帧」，
     而尾部是「最后几个子机的最新点」。1d-2 之前按前缀裁到 512 条，等于把
     前 17 个子机的拖影整段丢掉，只有最后 3~4 个还在画；一旦某帧插入量暴涨，
     保留窗口就只覆盖到一个子机 ⇒ 清酒看到的「拖影直接消失」。

     现在改成 i += step 的**均匀重采样**：全部子机、全部时间都在，只是点变稀，
     并按 step 把 alpha 乘回去（加法混合下 N 个点画一次 ≡ 一个点 alpha×N，
     在 255 处饱和）。C 侧 __rgl_draw_smear 已经按预算抽过一轮（见
     rgl_render.c 的 budget），所以正常情况这里不会二次抽稀 —— 阈值给到
     1600 就是为了留够余量。0 = 不抽稀，供 A/B 取证。 */
  let smearKeep = 1600;
  /* 把命令下标编进颜色，截图上取一个亮像素就能反解出「这是第几条命令画的」：
     r = (i % 16) * 17, g = (floor(i / 16) % 16) * 17, b = 128, a = 1。
     例：(170,136,128) → i = 10 + 16*8 = 138。 */
  function hueOf(i) {
    return [((i % 16) * 17) / 255,
            (Math.floor(i / 16) % 16) * 17 / 255,
            128 / 255, 1];
  }

  const newStat = () => ({ ops: 0, draws: 0, quads: 0, missingTex: 0,
                           missingSprite: 0, textSkipped: 0, unknownOp: 0, glyphs: 0,
                           inTarget: 0, noVerts: 0, scissored: 0, skipped: 0,
                           /* glDraws = 真实 gl.drawArrays 次数。它和 draws（ROP 命令数）
                              的比值就是批处理效率：1.6 个 quad 一次 draw 说明纹理
                              切得太碎，是「draw 阶段慢」的第一嫌疑。 */
                           glDraws: 0,
                           /* 拖影裁剪掉的 quad 数，用于量化「半径变大不再变卡」 */
                           smearTrimmed: 0,
                           /* 动画贴图的绘制条数（bugs #13：DefaultRenderFunc 以前
                              只画第 1 帧，这个计数为 0 就说明帧传递又断了） */
                           anims: 0,
                           postDrawn: 0, postSkipped: 0, postError: '',
                           /* 渲染目标/后期特效的视口取证（#16/#16.1/#30/#31.1 都栽在
                              这两处视口上）：最近一帧里所有 PushRenderTarget /
                              PostEffect 的视口与 RT 尺寸，读 __broRenderer.stats
                              即可看到（每条 {name, vp, rt}，最多 8 条）。 */
                           rtPushes: [], postFx: null,
                           /* textSkipped 的明细（字体名 + 文本前 40 字符），最多 8 条 */
                           skippedText: [] });
  const noteSkippedText = (fontName, body) => {
    if (stat.skippedText.length < 8) {
      stat.skippedText.push(fontName + '|' + (body || '').slice(0, 40));
    }
  };
  let stat = newStat();

  /* 一个 sprite 的四角（已含旋转缩放平移）+ uv，组装成两个三角形。
     「图的顶边」落在哪个 y 上由当前正交投影决定：
       · 世界是 y 向上（游戏 SetOrtho(l,r,b,t)，b<t，见 Lscreen.lua 的
         b=-240 / t=240），所以图顶边在**较大**的 y 上，v0 给那个顶点；
       · 默认正交（本文件 drawFrame 开头那个 y 向下的兜底）则相反。
     这个方向搞错的表现就是整幅画面上下镜像——uv 与顶点配错，几何位置却是对的。 */
  function quadSprite(sp, x, y, rot, hs, vs, cols, u0, v0, u1, v1, z3d) {
    const rad = rot * Math.PI / 180;
    const c = Math.cos(rad), s = Math.sin(rad);
    /* unitsPerPixel 参与的是「切图像素 → 世界单位」这一步，所以整块矩形
       （含锚点）都要乘，旋转/平移/调用方的 hscale 再乘在外面。 */
    /* ⚠️ 必须 `== null` 而不是 `||`。`SetImageScale(name, 0)` 是补丁层用来
       **隐藏**某张资源的标准手段（quad 面积 0，光栅化直接丢弃），而 `0 || 1`
       会把它悄悄变回 1 —— 表现就是「明明设了 0，外框还在」。hs 的初始值由
       rgl_resource.c:204 置 1.0，所以只有显式 0 会走到这里。 */
    const sh = hs * (sp.hs == null ? 1 : sp.hs), sv = vs * (sp.vs == null ? 1 : sp.vs);
    const lx0 = -sp.ax * sh;
    const lx1 = (sp.w - sp.ax) * sh;
    /* 纵向：引擎的锚点 (ax, ay) 是「贴图坐标里落在 (x, y) 的那一点」。
       y 向上的世界里，贴图顶端（v=0）在锚点上方 ay 处、底端在下方 h-ay 处：
         lyTop = +ay,  lyBot = -(h - ay)
       y 向下的世界则整体取反。以前两个分支写反了——只有 ay == h/2 时
       两种写法结果相同，所以居中的贴图一直是对的，而 SetImageCenter(0,0)
       这类非居中锚点会被画到锚点上方去（顶栏只露出左上四分之一就是这个）。 */
    const uy = yUp ? 1 : -1;
    const lyTop = (uy > 0 ? sp.ay : -sp.ay) * sv;
    const lyBot = (uy > 0 ? -(sp.h - sp.ay) : (sp.h - sp.ay)) * sv;

    const tx0 = x + lx0 * c - lyTop * s, ty0 = y + lx0 * s + lyTop * c;   // 左上（v0）
    const tx1 = x + lx1 * c - lyTop * s, ty1 = y + lx1 * s + lyTop * c;   // 右上（v0）
    const tx2 = x + lx1 * c - lyBot * s, ty2 = y + lx1 * s + lyBot * c;   // 右下（v1）
    const tx3 = x + lx0 * c - lyBot * s, ty3 = y + lx0 * s + lyBot * c;   // 左下（v1）

    /* 3D viewmode 下的 Render(img,x,y,rot,hs,vs,z)：第 7 参是深度 z
       （LuaSTG 标准语义，THlib 背景混用 Render 与 Render4V 画同一场景）。
       以前这里永远走 2D pushVertC、把 z 丢了 ⇒ 花映冢 scene1 的向日葵
       （Render("hyz_bg2", x,y,rot,scale,scale,f.z)）被当 2D 画到错误位置，
       整片太阳花田只剩泥土地面。3D 时用 quad4 逐点做透视投影。 */
    if (z3d !== undefined && proj3d) {
      quad4([[tx0, ty0, z3d], [tx1, ty1, z3d], [tx2, ty2, z3d], [tx3, ty3, z3d]],
            [u0, v0, u1, v0, u1, v1, u0, v1], cols);
      return;
    }

    pushVertC(tx0, ty0, u0, v0, cols[0]);
    pushVertC(tx1, ty1, u1, v0, cols[1]);
    pushVertC(tx2, ty2, u1, v1, cols[2]);
    pushVertC(tx0, ty0, u0, v0, cols[0]);
    pushVertC(tx2, ty2, u1, v1, cols[2]);
    pushVertC(tx3, ty3, u0, v1, cols[3]);
    stat.quads++;
  }

  /* 矩形（RenderRect）：参数顺序是 (img, x1, x2, y1, y2)。

     ⚠ 引擎 ResourceSpriteImpl::RenderRect(l,r,b,t) 直接把参数塞进
     RectF(l,t,r,b)，SpriteRenderer::setTransform 由此构造
       v0=(x1,y2) v1=(x2,y2) v2=(x2,y1) v3=(x1,y1)
     **完全不做 min/max 归一化**。misc.RenderBrightOutline 正是靠这一点，
     用 y/x 反序的 RenderRect 让渐变「镜像」到另一条边：
       RenderRect(x1,x2,y2,y2-ws)  顶边（y 反序）
       RenderRect(x2,x2-ws,y1,y2)  右边（x 反序）
     我们这里为了顶点绕序统一仍按 min/max 排列几何，但**四顶点色必须跟着
     参数反序一起翻**，否则顶边/右边的渐变方向会反过来（亮色跑到内侧），
     四条边在四个角叠加出一块明显更亮的方块——就是「转角亮边重叠」。 */
  function quadRect(sp, x1, x2, y1, y2, cols) {
    let c = cols;
    const flipX = x1 > x2, flipY = y1 > y2;
    if (flipX || flipY) {
      c = cols.slice();
      if (flipX) {                    // 引擎 v0 在右、v1 在左
        [c[0], c[1]] = [c[1], c[0]];
        [c[2], c[3]] = [c[3], c[2]];
      }
      if (flipY) {                    // 引擎 v0/v1 在下、v2/v3 在上
        [c[0], c[3]] = [c[3], c[0]];
        [c[1], c[2]] = [c[2], c[1]];
      }
    }
    const xl = Math.min(x1, x2), xr = Math.max(x1, x2);
    let yTop = Math.max(y1, y2), yBot = Math.min(y1, y2);
    if (!yUp) { const t = yTop; yTop = yBot; yBot = t; }
    const uy = yUp ? 1 : -1;
    // 换回世界坐标：yUp 时较大 y 是图顶
    const vTopY = uy > 0 ? yTop : yBot;
    const vBotY = uy > 0 ? yBot : yTop;
    pushVertC(xl, vTopY, sp.u0, sp.v0, c[0]);
    pushVertC(xr, vTopY, sp.u1, sp.v0, c[1]);
    pushVertC(xr, vBotY, sp.u1, sp.v1, c[2]);
    pushVertC(xl, vTopY, sp.u0, sp.v0, c[0]);
    pushVertC(xr, vBotY, sp.u1, sp.v1, c[2]);
    pushVertC(xl, vBotY, sp.u0, sp.v1, c[3]);
    stat.quads++;
  }

  /* 文本字形：layout() 给出的 y 是「视觉顶边、向下为正的屏幕量」，
     这里按当前正交方向把它换成世界坐标，并保证 v0 落在图顶边。 */
  function quadText(q, col) {
    const xl = q.x, xr = q.x + q.w;
    const yA = q.y;          // 视觉顶边
    const yB = q.y - q.h;    // 视觉底边（y 向上 = 更小）
    const yTop = yUp ? yA : yB;
    const yBot = yUp ? yB : yA;
    pushVertC(xl, yTop, q.u0, q.v0, col);
    pushVertC(xr, yTop, q.u1, q.v0, col);
    pushVertC(xr, yBot, q.u1, q.v1, col);
    pushVertC(xl, yTop, q.u0, q.v0, col);
    pushVertC(xr, yBot, q.u1, q.v1, col);
    pushVertC(xl, yBot, q.u0, q.v1, col);
    stat.quads++;
  }

  /* 任意 4 顶点 + 每顶点 uv/颜色。pts 是 [[x,y,z] × 4]；
     2D 命令（RenderRect 之类）也会走到这里，此时 z 恒 0 且 proj3d=false，
     直接取 x/y。3D 模式下逐点做透视投影，任一点落到近平面外就整块丢弃。 */
  function quad4(pts, uvs, col) {
    if (proj3d) {
      const vs = [];
      for (let i = 0; i < 4; i++) {
        vs.push({ c: camSpace(pts[i][0], pts[i][1], pts[i][2]),
                  u: uvs[i * 2], v: uvs[i * 2 + 1], col: col[i] });
      }
      const poly = clipNear(vs, cam.zn);
      if (poly.length < 3) return;
      const pv = poly.map((w) => {
        /* w.c[2] 是相机空间深度（即透视除法的 w）。把它取倒数交给顶点属性，
           着色器按 1/w 插值 uv/颜色，等价于 D3D 光栅化器的透视校正。
           近平面交点 c[2]=zn，取倒数同样是精确值。
           雾不在这里算：只把相机空间坐标带给片元着色器，距离在片元级求
           （见 FRAG 里 u_fogOn 那段），否则大四边形中段雾浓度会偏。 */
        const iw = perspectiveCorrect ? 1 / Math.max(w.c[2], 1e-6) : 1;
        const q = ndcToInput(w.c[0] * cam.xs / w.c[2], w.c[1] * cam.ys / w.c[2]);
        return [q[0], q[1], w.u, w.v, ...w.col, iw, w.c[0], w.c[1], w.c[2]];
      });
      for (let k = 1; k + 1 < pv.length; k++) {
        pushVert(...pv[0]); pushVert(...pv[k]); pushVert(...pv[k + 1]);
      }
      stat.quads++;
      return;
    }
    const a = pts[0], b = pts[1], c = pts[2], d = pts[3];
    pushVertC(a[0], a[1], uvs[0], uvs[1], col[0]);
    pushVertC(b[0], b[1], uvs[2], uvs[3], col[1]);
    pushVertC(c[0], c[1], uvs[4], uvs[5], col[2]);
    pushVertC(a[0], a[1], uvs[0], uvs[1], col[0]);
    pushVertC(c[0], c[1], uvs[4], uvs[5], col[2]);
    pushVertC(d[0], d[1], uvs[6], uvs[7], col[3]);
    stat.quads++;
  }

  /* 2D 快速路径：任意四顶点 + 同一 sprite 的 uv/颜色。
     避开 quad4 的 pts4v / uvs / p 三个临时数组（Render4V 每帧上万次，
     见 misc.SectorRender —— chaos 边缘粒子与测谎仪的圆环都走这里）。 */
  function quad4xy(x0, y0, x1, y1, x2, y2, x3, y3, sp, col) {
    const u0 = sp.u0, v0 = sp.v0, u1 = sp.u1, v1 = sp.v1;
    pushVertC(x0, y0, u0, v0, col[0]);
    pushVertC(x1, y1, u1, v0, col[1]);
    pushVertC(x2, y2, u1, v1, col[2]);
    pushVertC(x0, y0, u0, v0, col[0]);
    pushVertC(x2, y2, u1, v1, col[2]);
    pushVertC(x3, y3, u0, v1, col[3]);
    stat.quads++;
  }

  /* 输出分辨率变了就调 TTF 字形图集的光栅化密度。
     ?ss=1 可强制关掉超采样（对比用），?ss=N 手动指定 1..2。 */
  let lastSS = -1;
  function syncSuperSampling() {
    /* 1d-50（task#5）：上限 2 → 3，与 text.mjs 的 setSuperSampling 一致。
       2.5x/3x 屏（画布高 1350~1620）以前被钳到 2，字形比屏幕像素少 20%+，
       浏览器放大后就是「网页版字体总体更模糊」。 */
    const auto = Math.max(1, Math.min(3, canvas.height / 540));
    const want = ssOverride > 0 ? Math.max(1, Math.min(3, ssOverride)) : auto;
    if (Math.abs(want - lastSS) < 0.01) return;
    lastSS = want;
    text.setSuperSampling(want);
    stat.superSampling = +want.toFixed(2);
  }

  /* 改分辨率时把已有渲染目标**就地重建**成新尺寸。
     为什么必须做：getTarget() 只在第一次 CreateRenderTarget/PushRenderTarget
     时按当时的 canvas 取尺寸，之后永不改。而游戏的渲染目标是**加载期建一次、
     整局复用**的（background.lua:109 的 WorldTex1/2、ext_other.lua:392 的
     mask_fader、player_system.lua:343 的自机层……）。设置里一改分辨率，
     画布从 1280×720 变成别的尺寸，这些 RT 还是旧的：世界层被画进小一号的
     纹理再按屏幕 UV 贴回来 → 背景错位/拉伸、遮罩盖不满、自机层偏移，
     也就是「很多地方渲染错误」。
     就地重建（而不是删掉）是因为 CreateRenderTarget 一生只调一次，
     删了名字就再也不会被建回来，RenderTexture(name) 会永久落空。 */
  function resizeTargets(w, h) {
    if (!targets.size) return;
    /* 帧首调用，理论上栈是空的；万一不是也不能带着旧 fbo 继续往下画 */
    targetStack.length = 0;
    const prevTex = gl.getParameter(gl.TEXTURE_BINDING_2D);
    const prevFbo = gl.getParameter(gl.FRAMEBUFFER_BINDING);
    for (const [name, rt] of targets) {
      if (rt.w === w && rt.h === h) continue;
      gl.deleteFramebuffer(rt.fbo);
      gl.deleteTexture(rt.tex);
      const tex = gl.createTexture();
      gl.bindTexture(gl.TEXTURE_2D, tex);
      gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, w, h, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
      const fbo = gl.createFramebuffer();
      gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
      gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, tex, 0);
      rt.tex = tex; rt.fbo = fbo; rt.w = w; rt.h = h;
      textures.set(name, tex);          // 采样这张 RT 的地方要走新对象
      const info = texInfo.get(name);   // WorldToScreen 的 uv 归一化靠它
      if (info) { info.w = w; info.h = h; }
      else texInfo.set(name, { path: '', w, h, kind: K_TEX, sampler: '' });
    }
    gl.bindFramebuffer(gl.FRAMEBUFFER, prevFbo);
    gl.bindTexture(gl.TEXTURE_2D, prevTex);
    curTex = null;                      // 单元 0 被动过，缓存作废
  }

  /* 游戏 SetResolution 之后，后备缓冲必须跟着变：
     游戏用自己认为的窗口尺寸算 viewport，画布不一致的话整幅画面会缩在
     左上角一块（先被这个坑过：1024×576 的内容画进 1280×720 的画布）。 */
  function applyPendingResolution() {
    const s = Module.ccall('rgl_take_resolution', 'string', [], []);
    if (!s) return;
    let [w, h] = s.split(',').map(Number);
    if (!(w > 0 && h > 0)) return;
    /* 上下限只防呆：设置菜单最低 360p、最高 2160p（见 patches.lua 的
       __rgl_hook_settingmenu），这里留一倍余量兜 ?res= 手动指定。 */
    w = Math.max(320, Math.min(5120, Math.round(w)));
    h = Math.max(180, Math.min(5120, Math.round(h)));
    if (canvas.width === w && canvas.height === h) return;
    canvas.width = w;
    canvas.height = h;
    gl.viewport(0, 0, w, h);
    resizeTargets(w, h);
    stat.resized = `${w}x${h}`;
  }

  /* 诊断用：只消费 [skipAt, cutAt) 这段命令（-1 = 不设限）。
     定位「哪一条命令之后画面就没了」比逐条读代码快得多，也能在真实场景里
     做二分、单独重放某一层（指绘制命令重放，不是游戏录像回放）。
     无头验证脚本通过 renderer.setRange(a,b) 使用。 */
  let cutAt = -1;
  let skipAt = 0;
  /* 诊断用：true 时跳过每帧开头的清屏，让连续几次 drawFrame 叠加。
     只在「想累加多段命令看全局效果」时需要，正常路径必须保持 false。 */
  let keepBuffer = false;
  /* 按 op 号整类跳过（只给诊断用，别在正式路径上开）。
     最常用的组合是 setSkipOps([17,18]) —— 把文本整类关掉，
     一眼就能区分「白色大块是文字字形没出对」还是「贴图/混合不对」。 */
  let skipOps = new Set();

  /* ---- 高刷新率重复帧（1d-5）------------------------------------------
   * 逻辑帧固定 60Hz。1d-4 曾让「没有新逻辑帧」的 rAF 也跑一遍完整
   * drawFrame() —— 于是 7~12k 条 ROP 命令、全部 GL draw call、postfx 整屏
   * blit 在 144/240Hz 屏上被重复付 2~4 倍，主线程占满、DOM 键盘事件排队，
   * 表现就是「移动不跟手」（1c-18 修过的同一机制）。
   *
   * 重复帧根本不需要重算：把上一逻辑帧画完的默认 framebuffer 整屏拷进一张
   * 缓存纹理，n=0 的 rAF 只做一次 blitFramebuffer 回贴，成本 O(1)、与命令流
   * 规模无关。postfx.mjs 的 capture() 已验证「默认 FB → FBO」的 blit 可用。 */
  const repeat = { fbo: null, tex: null, w: 0, h: 0 };

  function ensureRepeatBuffer() {
    const w = canvas.width, h = canvas.height;
    if (repeat.fbo && repeat.w === w && repeat.h === h) return true;
    if (!w || !h) return false;
    if (repeat.tex) gl.deleteTexture(repeat.tex);
    if (repeat.fbo) gl.deleteFramebuffer(repeat.fbo);
    repeat.tex = null; repeat.fbo = null;
    const tex = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, tex);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, w, h, 0,
      gl.RGBA, gl.UNSIGNED_BYTE, null);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    const fbo = gl.createFramebuffer();
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0,
      gl.TEXTURE_2D, tex, 0);
    const ok = gl.checkFramebufferStatus(gl.FRAMEBUFFER) === gl.FRAMEBUFFER_COMPLETE;
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    gl.bindTexture(gl.TEXTURE_2D, null);
    if (!ok) {
      gl.deleteTexture(tex);
      gl.deleteFramebuffer(fbo);
      return false;
    }
    repeat.tex = tex; repeat.fbo = fbo; repeat.w = w; repeat.h = h;
    return true;
  }

  /** 正常逻辑帧画完后调用：把最终画面缓存一份，供后续 n=0 的 rAF 重复呈现。 */
  function captureFrame() {
    if (!ensureRepeatBuffer()) return false;
    /* 这一帧若还绑在 render target 上，默认 FB 里不是最终画面 —— 放弃缓存
       比贴出错内容安全（此时 repeatFrame 会重贴上一帧，画面只是静止一拍）。 */
    if (gl.getParameter(gl.FRAMEBUFFER_BINDING) !== null) return false;
    const w = repeat.w, h = repeat.h;
    gl.bindFramebuffer(gl.READ_FRAMEBUFFER, null);
    gl.bindFramebuffer(gl.DRAW_FRAMEBUFFER, repeat.fbo);
    gl.blitFramebuffer(0, 0, w, h, 0, 0, w, h,
      gl.COLOR_BUFFER_BIT, gl.NEAREST);
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    return true;
  }

  /** n=0 的 rAF 调用：一次整屏 blit，不遍历 ROP、不跑 RenderFunc。 */
  function repeatFrame() {
    if (!repeat.fbo) return false;
    if (repeat.w !== canvas.width || repeat.h !== canvas.height) return false;
    const w = repeat.w, h = repeat.h;
    gl.bindFramebuffer(gl.READ_FRAMEBUFFER, repeat.fbo);
    gl.bindFramebuffer(gl.DRAW_FRAMEBUFFER, null);
    gl.blitFramebuffer(0, 0, w, h, 0, 0, w, h,
      gl.COLOR_BUFFER_BIT, gl.NEAREST);
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    return true;
  }

  /** 阶段 3C：只呈现上一帧缓存（ROP 空帧 / lock60 跳帧时用），零重绘开销。 */
  function presentCached() {
    if (repeatFrame()) return;
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    gl.viewport(0, 0, canvas.width, canvas.height);
    gl.clearColor(0, 0, 0, 1);
    gl.clear(gl.COLOR_BUFFER_BIT);
  }

  function drawFrame() {
    const nAll = Module.ccall('rgl_rop_count', 'number', [], []);
    if (!nAll) {
      /* 阶段 3C：ROP 为空的帧绝不能什么都不画。preserveDrawingBuffer=false
         时浏览器合成后清空后备缓冲，这一帧就是整屏全黑（用户报的「正常游玩
         偶发黑一瞬」）。用上一帧缓存重贴兜底；没有缓存（首帧）则画纯色底。
         这层防御与 ROP 为何为空无关，lock60 / 非 lock60 都适用。 */
      presentCached();
      return stat;
    }
    const start = skipAt > 0 ? Math.min(skipAt, nAll) : 0;
    const end = cutAt >= 0 ? Math.min(cutAt, nAll) : nAll;
    const n = Math.max(0, end - start);

    // 内存可能已经增长（memory.grow），每帧重新取视图，绝不跨帧缓存
    const H16 = Module.HEAPU16, U32 = Module.HEAPU32, F32 = Module.HEAPF32, U8 = Module.HEAPU8;
    const headBase = Module.ccall('rgl_rop_heads', 'number', [], []);
    const fltBase = Module.ccall('rgl_rop_floats', 'number', [], []);
    const strBase = Module.ccall('rgl_rop_strs', 'number', [], []);
    const f32off = fltBase >> 2;

    /* 资源表改成「版本号驱动」的按需重建（1c-37 特效性能重写）。
       旧做法：每 30 帧、缺 sprite 时每 3 帧都全量 rgl_res_csv（1629 行）
       → split 出上千个临时数组 → 重建 sprites/texInfo/fonts 三张 Map。
       两个毛病：① 资源表一帧没变也要重建，制造周期性 GC 抖动；
       ② 新资源真正注册后反而要等最多 30 帧才被看见，正是「特效贴图
       概率性不显示 / 出现时先卡一下」的窗口。
       现在 C 侧 rgl_resource_version() 只在 Load* / SetImageCenter /
       SetImageScale / SetTextureSamplerState / RemoveResource 真的改变
       CSV 字段时 +1，每帧一次极轻的 ccall 比对：变了下一帧立即重读；
       没变时 300 帧（5 秒）兜底一次，持续缺图时 30 帧兜底一次。 */
    const nowMs = performance.now();
    const resVer = Module.ccall('rgl_res_version', 'number', [], []);
    if (resVer !== lastResVersion ||
        nowMs - lastReloadMs >= (tableDirty ? 500 : 5000)) {
      const before = fonts.size;
      loadResourceTable();
      deferred.clear();
      tableDirty = false;
      lastResVersion = resVer;
      lastReloadMs = nowMs;
      if (fonts.size !== before && text.preparePending) {
        text.preparePending().catch(() => { });
      }
    }

    /* 帧内字符串解码缓存：一帧里上千条 sprite 命令往往共用同一个资源名，
       UTF8ToString 重复解码几千次纯属浪费。命令缓冲每帧重建、soff 是帧内
       相对偏移，所以缓存只在本次 drawFrame 内有效，出函数即丢弃。 */
    const strCache = new Map();

    /* 命令流读取器：**必须建在循环外**。
       以前 `f` / `str` 是写在 `for (i...)` 里面当 `const f = (k) => …`，
       于是每条命令都新建两个闭包。chaos 边缘粒子、测谎仪 drag 这类特效
       一帧就是 7~12k 条 ROP 命令，等于每帧十几万个闭包，GC 与分配直接吃掉
       渲染线程（用户侧表现就是「特效一出来就掉帧」）。改成单例闭包 + 循环
       内只更新游标，语义完全不变（命令之间没有嵌套调用）。 */
    let curF = 0, curSlen = 0, curSoff = 0;
    /* 同一张图连画几千次是常态（SectorRender 全靠 "white"、测谎仪的 drag
       全靠 "bright"）。先比上一次的键，命中就不进 Map —— 这一条把 chaos
       粒子帧的 7000 次 Map.get 基本清零。 */
    let lastSoff = -1, lastSlen = 0, lastStr = '';
    const f = (k) => F32[curF + k];
    const str = () => {
      if (!curSlen) return '';
      if (curSoff === lastSoff && curSlen === lastSlen) return lastStr;
      /* slen 是 uint16，soff 是帧内偏移：(soff<<16)|slen 唯一 */
      const key = curSoff * 65536 + curSlen;
      let v = strCache.get(key);
      if (v === undefined) {
        v = Module.UTF8ToString(strBase + curSoff, curSlen);
        strCache.set(key, v);
      }
      lastSoff = curSoff; lastSlen = curSlen; lastStr = v;
      return v;
    };

    stat = newStat();
    missingSet.clear();           // 只报「这一帧」缺什么
    stat.ops = n;
    stat.opsTotal = nAll;
    applyPendingResolution();
    syncSuperSampling();
    imageScale = Module.ccall('rgl_image_scale', 'number', [], []) || 1;
    vcount = 0;
    curBlend = -1;
    vcApplied = false;
    curTex = null;

    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    gl.viewport(0, 0, canvas.width, canvas.height);
    gl.disable(gl.SCISSOR_TEST);
    /* 每帧显式清屏，对齐原生 SwapChain.cpp:520 的 ClearRenderTargetView。
       之前完全依赖「preserveDrawingBuffer=false 时浏览器合成后自动清」这条
       隐式行为，结果是：① 取证脚本为了能截图必须开 preserve，而一开就不再
       自动清，多帧叠加——UI 界面背景透明，上一屏的字体图集残留会糊满整屏，
       看着像渲染 bug；② 真实运行也把一个正确性前提交给浏览器实现。
       游戏自己的 Clear(color) 仍走 OP.CLEAR，会覆盖这次清屏。 */
    /* 深度每帧无条件清（D3D9 的 ClearZBuffer 只是显式再清一次）。
       注意深度测试默认关闭：只有 3D 背景会 SetZBufferEnable(1)。 */
    if (depthOn) { gl.disable(gl.DEPTH_TEST); depthOn = false; }
    if (!keepBuffer) {
      gl.clearColor(0, 0, 0, 0);
      gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    }
    ortho = { l: 0, r: canvas.width, b: canvas.height, t: 0 };
    yUp = false;                  // 兜底正交是 y 向下（0 在顶）
    applyOrtho();
    /* 命令流每帧重建，相机/雾状态不跨帧保留 */
    proj3d = false;
    cam = null;
    fog = null;
    fogDirty = true;

    for (let i = start; i < start + n; i++) {
      const o = headBase + i * HEAD_SIZE;
      const op = H16[o >> 1];
      const nf = H16[(o + 2) >> 1];
      const foff = U32[(o + 4) >> 2];
      const soff = U32[(o + 8) >> 2];
      const slen = H16[(o + 12) >> 1];
      const blend = U8[o + 14];
      const aux = U8[o + 15];
      const color = U32[(o + 16) >> 2];
      const c1 = U32[(o + 20) >> 2], c2 = U32[(o + 24) >> 2], c3 = U32[(o + 28) >> 2];
      setRgba(vcCols[0], color);
      if (c1 === color && c2 === color && c3 === color) {
        /* 绝大多数命令四角同色：复制比再做三次移位除法便宜得多 */
        const s0 = vcCols[0], t1 = vcCols[1], t2 = vcCols[2], t3 = vcCols[3];
        t1[0] = s0[0]; t1[1] = s0[1]; t1[2] = s0[2]; t1[3] = s0[3];
        t2[0] = s0[0]; t2[1] = s0[1]; t2[2] = s0[2]; t2[3] = s0[3];
        t3[0] = s0[0]; t3[1] = s0[1]; t3[2] = s0[2]; t3[3] = s0[3];
      } else {
        setRgba(vcCols[1], c1);   // 右上
        setRgba(vcCols[2], c2);   // 右下
        setRgba(vcCols[3], c3);   // 左下
      }

      curF = f32off + foff;
      curSoff = soff;
      curSlen = slen;

      if (skipOps.size && skipOps.has(op)) { stat.skipped++; continue; }

      switch (op) {
        case OP.ORTHO: {
          flush();
          ortho = { l: f(0), r: f(1), b: f(2), t: f(3) };
          yUp = ortho.t > ortho.b;
          applyOrtho();
          proj3d = false;          // SetRenderRect / SetViewMode("world"|"ui")
          break;
        }
        /* 视口 / 裁剪矩形：参数是 (l, r, b, t)，**y 向上**（与 SetOrtho 同一套），
           数值已经是设备像素（Lscreen.lua 里乘过 scale）。
           以前写成 `canvas.height - t`，只有在「全屏」或「上下对称」时才等于 b——
           全屏 b=0、世界视口 40..680 在 720 画布里正好对称，所以一直没露馅。
           成就列表的 SetRenderRect 是 60..566.7（贴面板上半），非对称，于是整段
           列表内容被抬高 93px，压到顶部分类标签那一行上，看起来像「文字重影」。
           applyOrtho 把 y=b 映到 NDC -1（GL 底部），所以 GL 的 y 原点就该是 b。 */
        case OP.VIEWPORT: {
          flush();
          const l = f(0), r = f(1), b = f(2), t = f(3);
          gl.viewport(Math.round(l), Math.round(b),
            Math.round(r - l), Math.round(t - b));
          break;
        }
        case OP.SCISSOR: {
          flush();
          const l = f(0), r = f(1), b = f(2), t = f(3);
          if (r - l <= 0 || t - b <= 0) {
            gl.disable(gl.SCISSOR_TEST);
          } else {
            gl.enable(gl.SCISSOR_TEST);
            gl.scissor(Math.round(l), Math.round(b),
              Math.round(r - l), Math.round(t - b));
          }
          break;
        }
        case OP.CLEAR: {
          flush();
          const a = ((color >>> 24) & 255) / 255;
          gl.clearColor(((color >>> 16) & 255) / 255, ((color >>> 8) & 255) / 255,
            (color & 255) / 255, a);
          gl.clear(gl.COLOR_BUFFER_BIT);
          break;
        }
        case OP.ZENABLE: {
          flush();
          const on = f(0) !== 0;
          if (on !== depthOn) {
            depthOn = on;
            if (on) { gl.enable(gl.DEPTH_TEST); gl.depthFunc(gl.LEQUAL); }
            else    { gl.disable(gl.DEPTH_TEST); }
          }
          break;
        }
        case OP.ZCLEAR: {
          flush();
          const sc = gl.isEnabled(gl.SCISSOR_TEST);
          if (sc) gl.disable(gl.SCISSOR_TEST);
          gl.clear(gl.DEPTH_BUFFER_BIT);
          if (sc) gl.enable(gl.SCISSOR_TEST);
          break;
        }
        case OP.PUSH_TARGET: {
          /* aux===1 是 CreateRenderTarget：C 侧只要登记 RT，但它同样会
             新建纹理并弄脏单元 0。以前这里既不 flush 也不作废 curTex，
             正好构成「有概率」的贴图消失。 */
          if (aux === 1) { flush(); getTarget(str()); curTex = null; break; }
          flush();
          const name = str();
          /* 必须在 getTarget() 之前取当前 FBO：getTarget 内部虽然会还原，
             但早先版本是硬置 null，入栈值因此永远是 null，嵌套 RT 的
             POP_TARGET 会恢复到错误的帧缓冲。 */
          const prevFbo = gl.getParameter(gl.FRAMEBUFFER_BINDING);
          getTarget(name);
          const rtPush = targets.get(name);
          /* 视口/裁剪盒必须入栈并在 POP 时还原（下面那把 switch 会改这两样东西）。
             但切 FBO 本身**不动视口**：#16 曾经在这里把视口撑成 RT 全尺寸，理由
             是「RT 只有按全尺寸画，后期效果才铺得满」。那个前提本身是错的 ——
             RT 与画布同尺寸，视口保持原样时，RT 里存的就是当前屏幕内容的 1:1
             像素拷贝；后期效果再把整张 RT 按全画布 1:1 贴回去，两头正好抵消。
             反过来（入栈撑满全 RT + POP 后在战场矩形里贴）虽然也能让「世界层」
             自洽，却只对「世界正交」这一种情形成立；一旦后期效果按全画布贴回
             （#16.1 为 legacy「想起所长之技」改的，那张卡必须 1:1），撑满视口的
             RT 就会把战场矩形的内容放大成 1280/384 宽、720/448 高 ——bugs #31.1
             秘术「岐道里的四季」着色器一出现的「比例偏大且拉伸」就是这个。
             现在统一成 1:1：入栈不碰视口，POP 还原视口，POSTEFFECT 满画布贴回。
             __effect1 是全仓唯一的 `__` postfx RT（只被岐道用），因此没有需要
             「撑满 RT 才好用」的遗留调用者。 */
          const pushed = {
            fbo: prevFbo,
            ortho: { ...ortho },
            vp: Array.from(gl.getParameter(gl.VIEWPORT)),
            sc: gl.isEnabled(gl.SCISSOR_TEST),
            box: Array.from(gl.getParameter(gl.SCISSOR_BOX)),
          };
          targetStack.push(pushed);
          if (stat.rtPushes.length < 8) {
            stat.rtPushes.push({ name, vp: pushed.vp.slice(), rt: [rtPush.w, rtPush.h] });
          }
          /* 只切 FBO、不动视口（见 pushed 上方的说明）：
             · `__effect1`（岐道 color_set）：按战场矩形视口画进去，RT 里就是
               屏幕那片区域的 1:1 拷贝，POSTEFFECT 满画布贴回 → 位置/比例不变；
               以前撑满全 RT，贴回全画布就变成 1.33×1.07 的放大拉伸（#31.1）。
             · misc.RotateWorld / WorldTex1 / Capturer1 这类抓屏 RT 用
               WorldToScreen() 的战场矩形像素坐标当 uv 采样（misc.lua:472-475），
               撑满视口会让内容重投影到全画布而 uv 仍按战场矩形取 → 错位/缩小
               一圈（bugs #30：天邪鬼「逆转 天地有用的遮蔽布」比例错误）。
             视觉上「1:1 拷贝 + 1:1 贴回」不依赖当前是 world 还是 ui 正交，
             是唯一对所有调用者都成立的语义。 */
          gl.bindFramebuffer(gl.FRAMEBUFFER, rtPush.fbo);
          gl.disable(gl.SCISSOR_TEST);
          curTex = null;
          break;
        }
        case OP.POP_TARGET: {
          flush();
          const prev = targetStack.pop();
          if (prev) {
            gl.bindFramebuffer(gl.FRAMEBUFFER, prev.fbo);
            /* 恢复 push 之前的视口与裁剪盒（见 PUSH_TARGET 处的说明） */
            if (prev.vp) gl.viewport(prev.vp[0], prev.vp[1], prev.vp[2], prev.vp[3]);
            if (prev.sc) {
              if (prev.box) gl.scissor(prev.box[0], prev.box[1], prev.box[2], prev.box[3]);
              gl.enable(gl.SCISSOR_TEST);
            } else {
              gl.disable(gl.SCISSOR_TEST);
            }
          } else {
            gl.bindFramebuffer(gl.FRAMEBUFFER, null);
          }
          curTex = null;
          break;
        }
        case OP.PERSPECTIVE: {
          /* SetViewMode 在 THlib 里是 Lua 函数，不会发 ROP_VIEWMODE；
             真正标志「进入 3D」的就是这条 SetPerspective。 */
          setCamera(f);
          break;
        }
        case OP.VIEWMODE: {
          /* 只有 C 侧被直接调用时才会出现，按值切；没有相机就退回 2D */
          proj3d = f(0) === 2 && cam !== null;
          break;
        }
        case OP.FOG: {
          /* SetFog(near, far, color)；SetFog() 无参表示清除（far<=near）。
             与 Renderer_D3D11.cpp:1027 的 fog_range.w = zfar-znear 一致。
             先 flush：已攒的顶点必须用旧雾画掉，否则会被新参数污染。 */
          flush();
          const near = f(0), far = f(1);
          fog = (far > near && near >= 0) ? { near, far, col: rgbOf(color >>> 0) } : null;
          fogDirty = true;
          break;
        }
        case OP.IMAGESCALE:
        case OP.SAMPLER:
          /* IMAGESCALE 的全局缩放已由 C 侧乘进每条绘制命令的 hscale/vscale；
             SAMPLER 只影响采样过滤，先不改动混合结果。 */
          break;

        case OP.DRAW: {
          let name = str();
          let glowed = false;
          if (name === '__rgl_glow') { name = GLOW_NAME; glowed = true; }
          const sp = sprites.get(name);
          if (!sp) { stat.missingSprite++; missingSet.add(name); tableDirty = true; break; }
          if (!bindTex(sp.tex, glowed)) { stat.missingTex++; missingSet.add(sp.tex); break; }
          applyBlend(blend);
          {
            const dx = f(0), dy = f(1), rot = f(2), hs = f(3), vs = f(4);
            /* 第 6 个 float 是 Render 的第 7 参：2D 下是 layer(默认 0.5)，
               3D viewmode 下是深度 z。只有显式给了非默认值才当 3D 深度用。 */
            const z3d = f(5);
            const useZ = proj3d && z3d !== 0.5 && Number.isFinite(z3d);
            if (glowed) {
              /* 1d-15：UI.lua 给 menu_pure_circle 的 alpha 固定是 `_a*50`
                 （≈0.2）。0.2 的加法混合在深色背景上只剩几十级亮度，
                 实测画出来看不见 —— 而且「径向渐变」本身越往外越透明，
                 峰值只剩中心那一小块。这里叠三层、每层放大 30%，
                 加法混合下亮度与半径同时堆出「中心往外发散」的观感，
                 仍是同一个 quad 批次，不增加 draw call。 */
              for (let k = 0; k < 3; k++) {
                const s = 1 + k * 0.30;
                quadSprite(sp, dx, dy, rot, hs * s, vs * s, vcCols,
                  sp.u0, sp.v0, sp.u1, sp.v1, useZ ? z3d : undefined);
              }
            } else {
              quadSprite(sp, dx, dy, rot, hs, vs, vcCols,
                sp.u0, sp.v0, sp.u1, sp.v1, useZ ? z3d : undefined);
            }
          }
          stat.draws++;
          if (targetStack.length) stat.inTarget++;
          break;
        }
        /* 批量 Render / 对象直绘（测谎仪 smear、连续同纹理弹幕）。
           f[0]=n，每条 10 个 float：6 个 (x,y,rot,hs,vs,layer)
           + 4 个 (a,r,g,b)。逐条带色，不重排、不跨纹理。 */
        case OP.DRAW_BATCH: {
          let name = str();
          /* 1d-19 task#1：C 侧 rop_coalesce_draw 会把连续的 `__rgl_glow`
             并成一条 ROP_DRAW_BATCH。这里若不像 OP.DRAW 那样认哨兵，
             sprites.get('__rgl_glow') 必然 miss ⇒ 整批道具高光凭空消失
             （diag_1d19_draw.js 实测 missSpr=1 / draws=0）。 */
          let glowed = false;
          if (name === '__rgl_glow') { name = GLOW_NAME; glowed = true; }
          const sp = sprites.get(name);
          if (!sp) { stat.missingSprite++; missingSet.add(name); tableDirty = true; break; }
          if (glowed) {
            if (!bindTex(name, true)) { stat.missingTex++; break; }
          } else if (!bindTex(sp.tex)) {
            stat.missingTex++; missingSet.add(sp.tex); break;
          }
          applyBlend(blend);
          const cnt = f(0) | 0;
          const u0 = sp.u0, v0 = sp.v0, u1 = sp.u1, v1 = sp.v1;
          /* 只对测谎仪那类「bright + 加法」拖影批做长度封顶。表是
             「每帧 × 每个子机各追加一段」的，所以只能**均匀抽稀**（i += step），
             绝不能截前缀 —— 截前缀只会留下最后几个子机，其余整段消失。
             抽稀后 alpha 乘回 step，加法混合下总亮度不变（255 处饱和）。
             其它纹理 / mul+alpha（子弹、UI）一律原样画，叠加顺序不变。 */
          let step = 1;
          if (smearKeep > 0 && blend === 2 && name === 'bright' && cnt > smearKeep) {
            step = Math.ceil(cnt / smearKeep);
            stat.smearTrimmed += cnt - Math.ceil(cnt / step);
          }
          for (let i = 0; i < cnt; i += step) {
            const b = 1 + i * 10;
            batchCol[0] = f(b + 7) / 255;
            batchCol[1] = f(b + 8) / 255;
            batchCol[2] = f(b + 9) / 255;
            const ba = (f(b + 6) / 255) * step;
            batchCol[3] = ba > 1 ? 1 : ba;
            /* 3D viewmode 下 Render(img,...,z) 的第 6 个 float 是深度 z：
               花映冢 scene1 的向日葵被 rop_coalesce_draw 并进这条批，
               以前当 layer 丢掉 ⇒ 整片太阳花田只剩泥土地面。 */
            const bz = f(b + 5);
            const z3d = proj3d && bz !== 0.5 && Number.isFinite(bz) ? bz : undefined;
            if (glowed) {
              /* 与 OP.DRAW 的 glow 分支一致：叠三层、每层 +30%，仍是同一批。 */
              for (let k = 0; k < 3; k++) {
                const s = 1 + k * 0.30;
                quadSprite(sp, f(b), f(b + 1), f(b + 2), f(b + 3) * s, f(b + 4) * s,
                  batchCols, u0, v0, u1, v1, z3d);
              }
            } else {
              quadSprite(sp, f(b), f(b + 1), f(b + 2), f(b + 3), f(b + 4),
                batchCols, u0, v0, u1, v1, z3d);
            }
          }
          stat.draws += Math.ceil(cnt / step);
          if (targetStack.length) stat.inTarget++;
          break;
        }
        case OP.DRAW_ANIM: {
          const name = str();
          const sp = sprites.get(name);
          if (!sp) { stat.missingSprite++; missingSet.add(name); tableDirty = true; break; }
          if (!bindTex(sp.tex)) { stat.missingTex++; missingSet.add(sp.tex); break; }
          applyBlend(blend);
          /* 1d-7：原生是 (timer / interval) % count；self.ani 每帧 +1，
             以前直接 timer % count 会让 interval=8 的人偶本体每帧换帧
             （快 8 倍、持续闪烁）。 */
          const idx = Math.floor((f(0) | 0) / (sp.interval || 1));
          let fr = sp.sub ? sp.sub[((idx % sp.sub.length) + sp.sub.length) % sp.sub.length] : null;
          if (!fr) fr = { u0: sp.u0, v0: sp.v0, u1: sp.u1, v1: sp.v1 };
          quadSprite(sp, f(1), f(2), f(3), f(4), f(5), vcCols,
            fr.u0, fr.v0, fr.u1, fr.v1);
          stat.draws++;
          stat.anims++;
          break;
        }
        case OP.DRAW_RECT: {
          const name = str();
          const sp = sprites.get(name);
          if (!sp) { stat.missingSprite++; missingSet.add(name); tableDirty = true; break; }
          if (!bindTex(sp.tex)) { stat.missingTex++; missingSet.add(sp.tex); break; }
          applyBlend(blend);
          quadRect(sp, f(0), f(1), f(2), f(3), vcCols);
          stat.draws++;
          break;
        }
        case OP.DRAW_4V: {
          const name = str();
          const sp = sprites.get(name);
          if (!sp) { stat.missingSprite++; missingSet.add(name); tableDirty = true; break; }
          if (!bindTex(sp.tex)) { stat.missingTex++; missingSet.add(sp.tex); break; }
          applyBlend(blend);
          // Render4V 每顶点 3 个数 (x,y,z)；z 之前被丢掉，3D 背景因此画不出来
          if (proj3d) {
            const pts4v = [
              [f(0), f(1), f(2)], [f(3), f(4), f(5)],
              [f(6), f(7), f(8)], [f(9), f(10), f(11)],
            ];
            quad4(pts4v,
              [sp.u0, sp.v0, sp.u1, sp.v0, sp.u1, sp.v1, sp.u0, sp.v1],
              vcCols);
          } else {
            quad4xy(f(0), f(1), f(3), f(4), f(6), f(7), f(9), f(10), sp, vcCols);
          }
          stat.draws++;
          break;
        }
        /* 批量 Render4V（四级彩虹环 / 任何连续同纹理单色四顶点绘制）。
           C 侧把 n 条命令压成一条，f[0]=n，之后每条 quad 占 16 个 float：
           12 个 (x,y,z)×4 + 4 个 (a,r,g,b)。逐条带色、顺序不变，因此
           additive 叠加结果与逐条命令完全一致，但可以省掉 n 次 head 解析、
           n 次 bindTex/applyBlend 判定和 n 份纹理名字符串。 */
        case OP.DRAW_4V_BATCH: {
          const name = str();
          const sp = sprites.get(name);
          if (!sp) { stat.missingSprite++; missingSet.add(name); tableDirty = true; break; }
          if (!bindTex(sp.tex)) { stat.missingTex++; missingSet.add(sp.tex); break; }
          applyBlend(blend);
          const cnt = f(0) | 0;
          const uvsB = proj3d
            ? [sp.u0, sp.v0, sp.u1, sp.v0, sp.u1, sp.v1, sp.u0, sp.v1] : null;
          const ptsB = proj3d
            ? [[0, 0, 0.5], [0, 0, 0.5], [0, 0, 0.5], [0, 0, 0.5]] : null;
          for (let i = 0; i < cnt; i++) {
            const b = 1 + i * 16;
            batchCol[0] = f(b + 13) / 255;
            batchCol[1] = f(b + 14) / 255;
            batchCol[2] = f(b + 15) / 255;
            batchCol[3] = f(b + 12) / 255;
            if (ptsB) {
              ptsB[0][0] = f(b);      ptsB[0][1] = f(b + 1);  ptsB[0][2] = f(b + 2);
              ptsB[1][0] = f(b + 3);  ptsB[1][1] = f(b + 4);  ptsB[1][2] = f(b + 5);
              ptsB[2][0] = f(b + 6);  ptsB[2][1] = f(b + 7);  ptsB[2][2] = f(b + 8);
              ptsB[3][0] = f(b + 9);  ptsB[3][1] = f(b + 10); ptsB[3][2] = f(b + 11);
              quad4(ptsB, uvsB, batchCols);
            } else {
              quad4xy(f(b), f(b + 1), f(b + 3), f(b + 4),
                      f(b + 6), f(b + 7), f(b + 9), f(b + 10), sp, batchCols);
            }
          }
          stat.draws += cnt;
          break;
        }
        /* 扇区：一条命令展开成 n 个 quad，等价于 THlib/misc.lua:270 的
           misc.SectorRender 逐条 Render4V('white', …)。顶点已由 C 侧按同一
           公式用 double 算好再压成 float32（见 rgl_render.c:l_sector），
           这里只做装配，不重算三角函数 —— 否则粒子边缘会差出几个像素。
           f[0]=n，f[1..8n]=每片段 (r2@A, r1@A, r1@angle, r2@angle) 的 x,y，
           与 quad4xy 的 uv (u0v0, u1v0, u1v1, u0v1) 一一对应，不能改。 */
        case OP.SECTOR: {
          const name = str();
          const sp = sprites.get(name);
          if (!sp) { stat.missingSprite++; missingSet.add(name); tableDirty = true; break; }
          if (!bindTex(sp.tex)) { stat.missingTex++; missingSet.add(sp.tex); break; }
          applyBlend(blend);
          const cnt = f(0) | 0;
          const u0 = sp.u0, v0 = sp.v0, u1 = sp.u1, v1 = sp.v1;
          /* 3D 分支才需要 pts/uvs；循环外各建一次，避免每个片段再分配 */
          const pts3 = proj3d ? [[0, 0, 0.5], [0, 0, 0.5], [0, 0, 0.5], [0, 0, 0.5]] : null;
          const uvs3 = proj3d ? [u0, v0, u1, v0, u1, v1, u0, v1] : null;
          for (let i = 0; i < cnt; i++) {
            const b = 1 + i * 8;
            const x0 = f(b), y0 = f(b + 1), x1 = f(b + 2), y1 = f(b + 3);
            const x2 = f(b + 4), y2 = f(b + 5), x3 = f(b + 6), y3 = f(b + 7);
            if (pts3) {
              pts3[0][0] = x0; pts3[0][1] = y0;
              pts3[1][0] = x1; pts3[1][1] = y1;
              pts3[2][0] = x2; pts3[2][1] = y2;
              pts3[3][0] = x3; pts3[3][1] = y3;
              quad4(pts3, uvs3, vcCols);
            } else {
              quad4xy(x0, y0, x1, y1, x2, y2, x3, y3, sp, vcCols);
            }
          }
          stat.draws++;
          break;
        }
        case OP.DRAW_TEX: {
          const name = str();
          const info = texInfo.get(name);
          if (!bindTex(name)) { stat.missingTex++; missingSet.add(name); break; }
          applyBlend(blend);
          const tw = (info && info.w) || 1, th = (info && info.h) || 1;
          /* RenderTexture 的 u/v 是 **D3D 风格的纹理像素坐标：v=0 在贴图顶**
             （background.Capture 直接塞 WorldToScreen() 的屏幕像素值）。
             普通贴图靠「上传时不设 UNPACK_FLIP_Y」刚好把这一约定抵消：
             GL 的 v=0 正好落在图片首行，所以 v/th 直接可用。
             但**渲染目标是 GL 原生帧缓冲，v=0 在屏幕底**，两者相反，
             必须翻成 (th - v)/th。曾漏掉这一步：WorldTex1 整幅上下镜像，
             使 LAYER.BG(-700) 的 boss 扫气特效（夹在 push -701 / pop -699
             之间被画进离屏目标）从 boss 头顶镜像到画面下方，看起来就是
             「白色特效在离 boss 很远的地方生成」。postfx.mjs 的
             sampleScreen() 也是这么翻的，保持一致。 */
          const isTarget = targets.has(name);
          // 每顶点 7 个数：x, y, z, u, v, color_hi, color_lo
          const S = 7;
          const uAt = (k) => f(S * k + 3) / tw;
          const vAt = (k) => isTarget ? (th - f(S * k + 4)) / th
                                      : f(S * k + 4) / th;
          const pts = [
            [f(0), f(1), f(2)], [f(S), f(S + 1), f(S + 2)],
            [f(S * 2), f(S * 2 + 1), f(S * 2 + 2)],
            [f(S * 3), f(S * 3 + 1), f(S * 3 + 2)],
          ];
          const uvs = [uAt(0), vAt(0), uAt(1), vAt(1),
                       uAt(2), vAt(2), uAt(3), vAt(3)];
          if (!isTarget) {
            for (let k = 0; k < 8; k += 2) {
              if ((uvs[k] < 0 || uvs[k] > 1 || uvs[k + 1] < 0 || uvs[k + 1] > 1)) {
                ensureWrap(name);
                break;
              }
            }
          }
          const argbAt = (k) => ((f(S * k + 5) | 0) * 65536 + (f(S * k + 6) | 0)) >>> 0;
          const cols = [rgbOf(argbAt(0)), rgbOf(argbAt(1)), rgbOf(argbAt(2)), rgbOf(argbAt(3))];
          quad4(pts, uvs, cols);
          stat.draws++;
          break;
        }
        /* 文本：C 侧把 "字体名\x1F正文" 塞进字符串池。
           TEXT  = lstg.RenderText(font, text, x, y, size, fmt)
                   → 位图字体语义，pen 是「行顶」，rect 退化成一个点
           TTF   = lstg.RenderTTF(face, text, l, r, b, t, fmt, color, scale)
                   → 真字体语义，pen 是基线，rect 是排版框
           两条都交给 text.mjs 排好版再出四边形，这里只负责压顶点。 */
        case OP.TEXT:
        case OP.TTF: {
          const raw = str();
          const sep = raw.indexOf('\x1F');
          if (sep < 0) { stat.textSkipped++; break; }
          const fontName = raw.slice(0, sep);
          const body = raw.slice(sep + 1);

          if (!text.has(fontName)) { stat.textSkipped++; noteSkippedText(fontName, body); break; }

          let quads;
          if (op === OP.TTF) {
            /* RenderTTF 内部把 scale 再乘 0.5，照抄引擎。
               **不能写 `f(5) || 1`**：C 侧 l_render_ttf 的
               `rop_f(luaL_optnumber(L, 9, 1))` 恒定写第 6 个浮点，
               所以 f(5) 永远有效，0 就是「字号 0 = 不可见」这个合法取值。
               游戏拿字号当淡入量：simple1.lua Summary 的
               `ui:RenderText("pretty", title, st.x, st.y, st.alpha * 0.5, ...)`
               在 st.alpha = 0 时传 size = 0；`|| 1` 把 0 顶成 1，于是
               结算界面刚进场的那 30 帧里，左上角会以全尺寸糊出
               「Normal / 1.雾之湖」——bug4。 */
            const s = 0.5 * f(5) * imageScale;
            quads = text.layout(fontName, body,
              { left: f(0), right: f(1), bottom: f(2), top: f(3) }, s, f(4) | 0);
          } else {
            /* RenderText：x,y 是笔触点；第 6 参是 ENUM_TTF_FMT 位掩码
               （center 0x01 / right 0x02 / vcenter 0x04 / bottom 0x08 /
               wordbreak 0x10，centerpoint = 0x105）。
               引擎（AppFrameFontRenderer.cpp:194）先用 CalcuTextSize 把 x/y
               挪成矩形左上角，再交给 rect 版做二次对齐——两步都要照抄，
               否则居中/右对齐的文字会整体右移半个字宽、下移一行。 */
            /* 同上：l_render_text 恒定写 4 个浮点（x,y,size,fmt），
               f(2) 就是 size，0 表示字号 0（不可见），不能兜成 1。 */
            const s = f(2);
            const fmt = f(3) | 0;
            const m = text.measureSize(fontName, body, s);
            const w = m ? m.w : 0, hh = m ? m.h : 0;
            const left = f(0) - ((fmt & 0x01) ? w / 2 : (fmt & 0x02) ? w : 0);
            const top = f(1) + ((fmt & 0x04) ? hh / 2 : (fmt & 0x08) ? hh : 0);
            quads = text.layout(fontName, body,
              { left, right: left + w, bottom: top - hh, top }, s, fmt & 0x10);
          }
          if (!quads || !quads.length) { stat.textSkipped++; noteSkippedText(fontName, body); break; }

          text.syncAtlases();
          /* 这一条文本可能刚刚新建了图集页（大字号标题首帧）：建纹理必然动
             全局 TEXTURE_BINDING_2D，curTex 缓存从这一刻起不可信，作废重绑。
             见 text.mjs createTexture 的长注释。 */
          if (text.atlasEpoch !== texEpochSeen) { texEpochSeen = text.atlasEpoch; curTex = null; }
          applyBlend(blend || 1);
          const col = debugTint ? hueOf(i) : rgbOf(color >>> 0);
          let curGlyphTex = null;
          for (const q of quads) {
            if (q.tex !== curGlyphTex) { bindRaw(q.tex); curGlyphTex = q.tex; }
            quadText(q, col);
          }
          stat.draws++;
          stat.glyphs += quads.length;
          break;
        }
        /* 3D 空间文本：顶点 = pos + u*rvec + v*dvec。
           RenderTTF3 / FontRenderer.RenderTextInSpace 走这条；旋转就是靠
           rvec/dvec 两个方向向量，所以这里不能再走 quadText 的 yUp 修正——
           方向已经完全由向量表达。v 从「基线向下」量，字形顶边在 yTop 处
           （y 向上为正），所以顶边的 v = -yTop。 */
        case OP.TTF3: {
          const raw = str();
          const sep = raw.indexOf('\x1F');
          if (sep < 0) { stat.textSkipped++; break; }
          const fontName = raw.slice(0, sep);
          const body = raw.slice(sep + 1);
          if (!text.has(fontName)) { stat.textSkipped++; noteSkippedText(fontName, body); break; }

          const px = f(0), py = f(1);
          const rx = f(3), ry = f(4);
          const dxv = f(6), dyv = f(7);
          /* 1d-49 task#9：**不能写 `|| 1`**。C 侧 fr_emit 恒定写
             g_fr_hs/g_fr_vs（FontRenderer.SetScale 设什么就是什么），
             0 是合法取值：图书的 Core.Render.Text 用「字号 0」表示不可见，
             而 `simple1.lua` 的 Summary 小标题正是 `size = subtitle.alpha * 0.5`
             —— alpha 从 0 起的那 30 帧里 size 就是 0。`|| 1` 把 0 顶成 1，
             于是结算界面开头左上角以**全尺寸**糊出「Normal / 1.雾之湖」
             （1c-26 bug4 在新文本管线上的回归；旧 RenderTTF 路径已修过）。
             只跳「正好 0」的，负缩放是镜像，照旧画。 */
          const hs = f(9), vs = f(10);
          if (hs === 0 || vs === 0) { stat.textSkipped++; break; }

          const quads = text.layoutSpace(fontName, body, hs, vs);
          if (!quads || !quads.length) { stat.textSkipped++; noteSkippedText(fontName, body); break; }

          text.syncAtlases();
          /* 这一条文本可能刚刚新建了图集页（大字号标题首帧）：建纹理必然动
             全局 TEXTURE_BINDING_2D，curTex 缓存从这一刻起不可信，作废重绑。
             见 text.mjs createTexture 的长注释。 */
          if (text.atlasEpoch !== texEpochSeen) { texEpochSeen = text.atlasEpoch; curTex = null; }
          applyBlend(blend || 1);
          const col = debugTint ? hueOf(i) : rgbOf(color >>> 0);
          let curGlyphTex = null;
          for (const q of quads) {
            if (q.tex !== curGlyphTex) { bindRaw(q.tex); curGlyphTex = q.tex; }
            const vT = -q.yTop, vB = -q.yTop + q.h;     // 顶边 / 底边到基线的向下距离
            const ax = px + q.x * rx, ay = py + q.x * ry;
            const bx = px + (q.x + q.w) * rx, by = py + (q.x + q.w) * ry;
            const uxT = vT * dxv, uyT = vT * dyv;
            const uxB = vB * dxv, uyB = vB * dyv;
            pushVertC(ax + uxT, ay + uyT, q.u0, q.v0, col);
            pushVertC(bx + uxT, by + uyT, q.u1, q.v0, col);
            pushVertC(bx + uxB, by + uyB, q.u1, q.v1, col);
            pushVertC(ax + uxT, ay + uyT, q.u0, q.v0, col);
            pushVertC(bx + uxB, by + uyB, q.u1, q.v1, col);
            pushVertC(ax + uxB, ay + uyB, q.u0, q.v1, col);
            stat.quads++;
          }
          stat.draws++;
          stat.glyphs += quads.length;
          break;
        }
        /* 后期效果。字符串是 5 段用 0x1F 分隔的编码（见 rgl_render.c 的
           l_post_effect 注释）：fx | 目标纹理 | 具名 uniform | 位置参数 | 具名纹理 */
        case OP.POSTEFFECT: {
          const raw = str();
          const sec = raw.split('\x1F');
          const fx = sec[0] || '';
          if (!fx) { stat.postSkipped++; break; }

          const named = {};
          for (const kv of (sec[2] || '').split(';')) {
            if (!kv) continue;
            const eq = kv.indexOf('=');
            if (eq < 0) continue;
            const k = kv.slice(0, eq), v = kv.slice(eq + 1);
            named[k] = v.includes(',') ? v.split(',').map(Number) : Number(v);
          }
          const args = [];
          for (const grp of (sec[3] || '').split(';')) {
            if (grp) args.push(grp.split(',').map(Number));
          }
          const bindings = {};
          for (const kv of (sec[4] || '').split(';')) {
            if (!kv) continue;
            const eq = kv.indexOf('=');
            if (eq < 0) continue;
            bindings[kv.slice(0, eq)] = kv.slice(eq + 1);
          }

          flush();          // 后期效果要读当前画面，先把批次画完
          const vp = Array.from(gl.getParameter(gl.VIEWPORT));
          stat.postFx = { fx, tex: sec[1] || '', vp: vp.slice(), canvas: [canvas.width, canvas.height] };
          try {
            /* 后期特效画的是一张覆盖整屏的 quad，postfx 自身不调用 gl.viewport，
               覆盖区域完全由此刻的 viewport 决定。这里一律铺满画布，与
               PUSH_TARGET「RT 里存的是屏幕的 1:1 像素拷贝」配套：
                 · legacy「想起所长之技」(SaveScreen："objectTH11_0-1") 的 RT 是
                   在战场矩形视口下画的，按战场矩形贴回会把整幅画面压进那个方框
                   → 小圈外被挤压缩放（#16.1）；
                 · 岐道 color_set("__effect1") 的 RT 同样是 1:1 拷贝，按战场矩形
                   贴回反而会缩小（#16），满画布贴回才对。
               ctx.viewport 仍传绘制前的战场矩形，令 gray 等按 u_viewport 裁剪的
               效果保持在原战场区域（不改变它们的行为）。 */
            gl.viewport(0, 0, canvas.width, canvas.height);
            postfx.draw({ fx, tex: sec[1] || '', named, args, textures: bindings }, {
              viewW: canvas.width,
              viewH: canvas.height,
              viewport: [vp[0], vp[1], vp[0] + vp[2], vp[1] + vp[3]],
              resolve: resolveTexForEffect,
              /* 着色器要 screen_texture_size；渲染目标按画布尺寸建，纹理则用资源表里的 */
              texSize: (n) => {
                const rt = targets.get(n);
                if (rt) return { w: rt.w, h: rt.h };
                const ti = texInfo.get(n);
                if (ti && ti.w && ti.h) return { w: ti.w, h: ti.h };
                const sp = sprites.get(n);
                if (sp) return { w: sp.tw || sp.w, h: sp.th || sp.h };
                return null;
              },
              blendApply: () => applyBlend(blend || 1, false),
            });
            stat.postDrawn++;
          } catch (e) {
            if (!stat.postError) stat.postError = String(e.message || e);
            stat.postSkipped++;
          } finally {
            gl.viewport(vp[0], vp[1], vp[2], vp[3]);   // #16.1 恢复战场矩形 viewport
          }
          // 后期换过 program 与纹理绑定，恢复 sprite 管线的状态
          gl.useProgram(prog);
          gl.uniform1i(uTex, 0);
          curTex = null;
          curBlend = -1;
          vcApplied = false;    // program 刚切回来，uniform 需重设
          break;
        }
        default:
          stat.unknownOp++;
          break;
      }
    }
    flush();
    gl.bindTexture(gl.TEXTURE_2D, null);
    /* 阶段 3C：正常绘制后自动缓存整屏，供 ROP=0 的空帧重贴兜底（防偶发黑屏）。
       仅在完整绘制（未做 skipAt/cutAt 诊断裁剪）时缓存，避免存进残缺画面。 */
    if (skipAt <= 0 && cutAt < 0) captureFrame();
    return stat;
  }

  return {
    gl, canvas,
    loadResourceTable,
    requestAll,
    drawFrame,
    captureFrame,
    repeatFrame,
    presentCached,
    get stats() {
      return {
        ...stat,
        texLoaded, texFailed, texPending: pending.size,
        texQueue: loadQueue.length, texActive: loading,
        texDownscaled: downscaled.length,
        sprites: sprites.size, texes: texInfo.size,
        targets: targets.size,
      };
    },
    /* 诊断：被降采样的纹理（名字 + 原始尺寸 → 实际尺寸） */
    downscaledTextures: () => downscaled.slice(0, 64),
    /* 诊断：加载失败的纹理名 + 它们解析出的 URL，用于定位 404 / 路径错位 */
    failedTextures: () => [...failed].slice(0, 64).map((n) => {
      const i = texInfo.get(n);
      return { name: n, path: i ? i.path : '',
        url: i && i.path ? assets.describePath(i.path) : '' };
    }),
    texInfoOf: (n) => texInfo.get(n) || null,
    /* 诊断：贴图名 → 资源路径（预热/404 排查用；无 path 的是渲染目标或内置） */
    texPaths: (n) => [...texInfo.entries()].slice(0, n || 20)
      .map(([k, v]) => k + '=' + (v.path || '')),
    /* 后台预热（Misty 1d-22）与失败重试的对外入口 */
    prefetch,
    prefetchStats: () => prefetchLast,
    retryFailedTextures,
    failedTextureLog: () => failLog.slice(),
    spriteOf: (n) => sprites.get(n) || null,
    hasTexture: (n) => textures.has(n),
    /* 调试：把某个渲染目标的像素读出来（返回 dataURL），用于定位
       「特效卡闪黑块」这类只在真实浏览器出现的问题。 */
    readTargetURL: (n) => {
      const rt = targets.get(n); if (!rt) return null;
      try {
        flush();
        const px = new Uint8Array(rt.w * rt.h * 4);
        const prev = gl.getParameter(gl.FRAMEBUFFER_BINDING);
        gl.bindFramebuffer(gl.FRAMEBUFFER, rt.fbo);
        gl.readPixels(0, 0, rt.w, rt.h, gl.RGBA, gl.UNSIGNED_BYTE, px);
        gl.bindFramebuffer(gl.FRAMEBUFFER, prev);
        const cv = document.createElement('canvas');
        cv.width = rt.w; cv.height = rt.h;
        const ctx = cv.getContext('2d');
        const img = ctx.createImageData(rt.w, rt.h);
        for (let y = 0; y < rt.h; y++) {
          const src = (rt.h - 1 - y) * rt.w * 4, dst = y * rt.w * 4;
          img.data.set(px.subarray(src, src + rt.w * 4), dst);
        }
        ctx.putImageData(img, 0, 0);
        return cv.toDataURL('image/png');
      } catch (e) { return null; }
    },
    /* 供 C 侧 CreateRenderTarget 立即建 target 用（见 rgl_render.c 的同名注释）：
       该函数常在逻辑帧里被调用，排队的 ROP 会被渲染周期清掉。 */
    createTargetNow: (n) => { try { flush(); getTarget(n); curTex = null; } catch (e) { /* 忽略：排队那条还会再试一次 */ } },
    /* 诊断：读一块矩形区域的平均色。target 传 null 读默认帧缓冲，
       传目标名读那张渲染目标。**必须先切对帧缓冲再 readPixels**——
       readPixels 读的是当前绑定的 FB，不是画布（这个坑踩过两次）。 */
    /* 诊断：直接读某张纹理的像素（把纹理挂到临时 FBO 上 readPixels，
       不需要画，也就不受混合/投影/着色器影响）。返回尺寸 + 平均色 +
       gw×gh 的缩略网格，用来回答「这张贴图到底是不是我以为是的那张图」。
       游戏里「贴图渲染成一片白/一片糊」时，先看这里：白=绑定到了占位图。 */
    readAvg: (target, x, y, w, h) => {
      const prev = gl.getParameter(gl.FRAMEBUFFER_BINDING);
      const rt = target ? targets.get(target) : null;
      gl.bindFramebuffer(gl.FRAMEBUFFER, rt ? rt.fbo : null);
      const W = rt ? rt.w : gl.drawingBufferWidth;
      const H = rt ? rt.h : gl.drawingBufferHeight;
      const ww = Math.max(1, Math.min(w | 0, W - (x | 0)));
      const hh = Math.max(1, Math.min(h | 0, H - (y | 0)));
      const buf = new Uint8Array(ww * hh * 4);
      gl.readPixels(x | 0, y | 0, ww, hh, gl.RGBA, gl.UNSIGNED_BYTE, buf);
      gl.bindFramebuffer(gl.FRAMEBUFFER, prev);
      let r = 0, g = 0, b = 0, a = 0, nz = 0;
      for (let i = 0; i < buf.length; i += 4) {
        r += buf[i]; g += buf[i + 1]; b += buf[i + 2]; a += buf[i + 3];
        if (buf[i] > 8 || buf[i + 1] > 8 || buf[i + 2] > 8) nz++;
      }
      const n = buf.length / 4;
      return {
        avg: [Math.round(r / n), Math.round(g / n), Math.round(b / n), Math.round(a / n)],
        paintedPct: Math.round((1000 * nz) / n) / 10, size: [W, H],
      };
    },
    targetsInfo: () => [...targets.entries()].map(([k, v]) => `${k}:${v.w}x${v.h}`),
    /* 诊断：这一帧里被请求过但没拿到像素的名字（缺 sprite / 缺纹理都算） */
    missingNames: () => [...missingSet].slice(0, 64),
    text,                              // 文本层（字形图集、字体诊断）
    get postStats() { return postfx.stats; },
    /* 诊断：清空后期效果计数 —— 之后读 postStats 就是这一段（通常一帧）的 pass 分布 */
    resetPostStats: () => postfx.resetStats(),
    /* 诊断：只跑前 n 条命令 / 只跑 [a,b) 段；n < 0 恢复整帧 */
    setCut: (v) => { cutAt = (v == null || v < 0) ? -1 : v | 0; },
    setRange: (a, b) => {
      skipAt = (a == null || a < 0) ? 0 : a | 0;
      cutAt = (b == null || b < 0) ? -1 : b | 0;
    },
    get cut() { return cutAt; },
    get skip() { return skipAt; },
    /* 诊断：true 时连续 drawFrame 会叠加（不清屏），用于累加多段命令。 */
    setKeepBuffer: (v) => { keepBuffer = !!v; return keepBuffer; },
    /* 诊断：按 op 号整类跳过。RGL.OP 在 shell 里有导出。 */
    /* 诊断：关掉 1/w 透视校正，复现旧的仿射纹理映射（拉伸异常） */
    setPerspectiveCorrect: (v) => { perspectiveCorrect = !!v; return perspectiveCorrect; },
    setSkipOps: (list) => { skipOps = new Set((list || []).map((v) => v | 0)); return [...skipOps]; },
    /* 取证用：把文本命令按序号上色（每条命令一个色相），默认关 */
    setDebugTint: (v) => { debugTint = !!v; return debugTint; },
    /* 拖影长度上限：0 = 关闭裁剪（A/B 用）；默认 512 */
    setSmearKeep: (v) => { smearKeep = Math.max(0, v | 0); return smearKeep; },
    get smearKeep() { return smearKeep; },
    get skipOpsList() { return [...skipOps]; },
    get imageScale() { return imageScale; },
  };
}
