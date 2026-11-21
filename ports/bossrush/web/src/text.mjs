/*
 * RGL Web runtime -- 文本层（字形与排版）
 *
 * 两种字体，语义完全不同，都严格照引擎（v0.21.103 血统）的算法来：
 *
 *   1. `.fnt`（HGE 纹理字体）：字模是一张 PNG 里的矩形切片。
 *      Char="<字>",x,y,w,h,left_offset,right_offset
 *        · 字形在笔触坐标系里的位置 = (left_offset, 0)，且笔触 y 是**行顶**
 *        · advance = w + left_offset + right_offset
 *        · ascender = 所有字形 h 的最大值，lineHeight = ascender + 1
 *      （见 ResourceFontImpl.cpp 的 [HGEFONT] 分支 + TextRenderer::drawGlyph）
 *
 *   2. `.ttf` / `.otf`：字形由 FreeType 光栅化。
 *        · position = (bitmap_left, bitmap_top)，笔触 y 是**基线**
 *        · 字形左上角 = (pen.x + bl, pen.y + bt - h)
 *        · ascender / lineHeight 取 face 的 ascender / height
 *        · 注意 RenderTTF 把 scale 又乘了 0.5（AppFrameFontRenderer.cpp：
 *          `Vector2F(scale, scale) * 0.5f`，注释自己都在问号），必须照抄，
 *          否则所有 TTF 文字大一倍。
 *
 * 排版算法（AppFrame::RenderText）逐行照搬：
 *   行数 → 起始笔触 y（Top/Middle/Bottom 三档，再减 ascender）
 *   → 每行按 Left/Center/Right 定 x → 画完 y -= lineHeight
 *
 * 光栅化策略：不按「基准字号光栅化再缩放」（那是引擎因为 D3D 纹理固定才这么做），
 * 而是按**最终屏幕像素尺寸**光栅化到字形图集，缩放由 uv 表达。文字在任何缩放下
 * 都清晰，且不影响排版数值（度量都换算到屏幕像素后再参与计算）。
 */

const PAD = 1;                 // 图集内每个字形四周留 1px，避免线性采样串色
const ATLAS_W = 1024;
const ATLAS_H = 1024;
const ATLAS_MAX_PAGES = 8;

/* fmt 位，来自 THlib/lib/Ltext.lua 的 ENUM_TTF_FMT */
const FMT = {
  H_CENTER: 0x01, H_RIGHT: 0x02,
  V_CENTER: 0x04, V_BOTTOM: 0x08,
  WORDBREAK: 0x10,
};

const normPath = (p) => String(p).replace(/\\/g, '/').replace(/^\/+/, '');
const joinUrl = (base, path) =>
  base + normPath(path).split('/').map(encodeURIComponent).join('/');

export function createTextLayer(opts) {
  const { gl, assetBase = './game/' } = opts;
  /* 导入包优先；不传时退回纯 HTTP（旧的 joinUrl 行为）。 */
  const assets = opts.assets || {
    fetchAsset: (p) => fetch(joinUrl(assetBase, p)),
    url: (p) => Promise.resolve(joinUrl(assetBase, p)),
  };

  /* TTF 超采样：游戏坐标系恒为 960x540，字形按逻辑字号光栅化后会被
     canvas 放大 screen.scale 倍显示（1080p 就是 2 倍），字就糊了。
     这里按输出高度 / 540 提高图集里的光栅化密度，再把世界尺寸除回去，
     逻辑排版完全不变，只是纹理更细。上限 2 是显存与图集页数的折中。 */
  let ssFactor = 1;
  const setSuperSampling = (v) => {
    const n = +v;
    /* 1d-50（task#5）：上限 2 → 3。上限原本是显存折中，但 2 在 2.5x/3x 屏上
       意味着字形比屏幕实际像素少 20%~33%，浏览器再把图集放大 = 一片糊。
       实测（图鉴页，`?ss=2` vs `?ss=3`，见 probe_1d50_sspages.js）：
       图集页 26 → 30（+15%，约 +16MB），字形 204 → 228，绘制量不变。
       真要省显存可以用 `?ss=1` 手动压回去。 */
    ssFactor = (n > 0 && n <= 3) ? n : 1;
    return ssFactor;
  };

  /** 名字 → { kind, path, size, url, face, family, glyphs, lineHeight, ascender, tex } */
  const fonts = new Map();
  /** 图集键（family@px#page）→ atlas */
  const atlases = new Map();
  /* 每新建一张图集页/纹理就 +1。renderer 拿它判断「curTex 缓存是否还可能
     可信」——建纹理一定会动全局 TEXTURE_BINDING_2D（见 createTexture 注释）。 */
  let atlasEpoch = 0;

  let diag = { fonts: 0, faces: 0, fnt: 0, failed: [], glyphs: 0, layoutCalls: 0 };

  /* ---- 注册（资源表来一行就登记一行，真正的加载在 prepare） ---- */
  function registerFont(name, rec) {
    if (!name || !rec || !rec.path) return;
    if (fonts.has(name)) return;
    fonts.set(name, {
      kind: rec.kind,               // 7 = .fnt, 8 = .ttf/.otf
      path: normPath(rec.path),
      size: rec.size || 0,          // 仅 TTF：LoadTTF 的第 4 个参数
      url: joinUrl(assetBase, rec.path),   // 仅诊断用，实际取地址走 assets
      ready: false,
      error: null,
      family: null,
      fntGlyphs: null,
      tex: null,                    // .fnt 的位图纹理
      lineHeight: 0,
      ascender: 0,
    });
    diag.fonts = fonts.size;
  }

  /* ---- 加载 ------------------------------------------------------------ */
  async function prepare() {
    const jobs = [];
    for (const [name, f] of fonts) jobs.push(loadFont(name, f));
    await Promise.all(jobs);
    return diag;
  }

  /* 资源表分帧增长，后来才登记的字体要补加载；已成功的不要重复建 FontFace。 */
  async function preparePending() {
    const jobs = [];
    for (const [name, f] of fonts) {
      if (!f.ready && !f.error) jobs.push(loadFont(name, f));
    }
    await Promise.all(jobs);
    return diag;
  }

  async function loadFont(name, f) {
    try {
      if (f.kind === 7) await loadFnt(name, f);
      else await loadFace(name, f);
      f.ready = true;
    } catch (e) {
      f.error = e && e.message ? e.message : String(e);
      diag.failed.push({ name, path: f.path, error: f.error });
    }
  }

  /* HGE 纹理字体：解析 .fnt 文本 + 读回它指向的位图 */
  async function loadFnt(name, f) {
    const res = await assets.fetchAsset(f.path);
    if (!res.ok) throw new Error('HTTP ' + res.status);
    const text = await res.text();

    const glyphs = new Map();
    let bitmap = '';
    let maxH = 0;
    for (const raw of text.split('\n')) {
      const line = raw.replace(/\r$/, '');
      if (line.startsWith('Bitmap=')) { bitmap = line.slice(7).trim(); continue; }
      if (!line.startsWith('Char=')) continue;
      // Char="X",x,y,w,h,left_offset,right_offset
      const m = /^Char="([\s\S]*?)",\s*(-?[\d.]+),\s*(-?[\d.]+),\s*(-?[\d.]+),\s*(-?[\d.]+),\s*(-?[\d.]+),\s*(-?[\d.]+)\s*$/.exec(line);
      if (!m) continue;
      const ch = m[1];
      if (!ch) continue;
      // 只取第一个码点（字体里不会出现多码点字形）
      const code = ch.codePointAt(0);
      const x = +m[2], y = +m[3], w = +m[4], h = +m[5];
      const lo = +m[6], ro = +m[7];
      glyphs.set(code, {
        x, y, w, h,
        /* 笔触 y 是**基线**，不是行顶。引擎 ResourceFontImpl.cpp:245 里
           position = Vector2F(left_offset, char_data.h) —— 字形顶边在基线上方
           一个字形高度处。早先写 0 会让所有位图字整体下沉一个字高，
           正好把 setname 键盘的每行往下一格，点 A 出 N。 */
        dx: lo, dy: h,
        adv: w + lo + ro,
      });
      if (h > maxH) maxH = h;
    }
    if (!glyphs.size) throw new Error('没有解析到任何字形');

    f.fntGlyphs = glyphs;
    f.ascender = maxH;
    f.lineHeight = maxH + 1;          // 引擎：m_line_height + 1.0f

    // 位图路径相对 .fnt 所在目录
    const dir = f.path.includes('/') ? f.path.slice(0, f.path.lastIndexOf('/') + 1) : '';
    const bmpUrl = await assets.url(dir + bitmap.replace(/\\/g, '/'));
    const img = await loadImage(bmpUrl);
    f.tex = createTexture(img.width, img.height, img, 'LINEAR');
    f.bitmapSize = { w: img.width, h: img.height };
    diag.fnt++;
  }

  /* TTF/OTF：浏览器原生字体，度量用 canvas measureText 取 */
  async function loadFace(name, f) {
    const family = 'rgl_' + name.replace(/[^\w]/g, '_');
    const faceUrl = await assets.url(f.path);
    const face = new FontFace(family, `url("${faceUrl}")`);
    await face.load();
    document.fonts.add(face);
    f.family = family;
    f.face = face;
    // 基准字号取 LoadTTF 的 size；canvas 里 px 就是 em 尺寸，与 FreeType
    // FT_Set_Pixel_Sizes 的语义一致
    f.size = f.size > 0 ? f.size : 16;
    diag.faces++;
  }

  function loadImage(url) {
    return new Promise((resolve, reject) => {
      const img = new Image();
      img.onload = () => resolve(img);
      img.onerror = () => reject(new Error('图片加载失败 ' + url));
      img.src = url;
    });
  }

  /* ---- 图集 ------------------------------------------------------------ */
  function createTexture(w, h, source, filter) {
    /* ⚠ TEXTURE_BINDING_2D 是全局绑定点，必须存/恢复。
       图集页是**在渲染循环内部**按需新建的（glyphOf → atlasFor → makeAtlas，
       由 renderer 的 TEXT 分支调 layoutSpace 触发），而 renderer 用 curTex
       缓存决定「同一张图不必重绑」。这里一旦把绑定改走又不还原：
         · 上一页图集刚被 bindRaw 绑上、缓存里 curTex = 旧页；
         · 这一条文本要新页 → 全局绑定变成新页，curTex 仍是旧页；
         · syncAtlases 取到的 prev 已经是新页，restore 也救不回来；
         · 下一条落到旧页的文本命令 bindRaw 判定「同一张图」直接 return，
           于是拿新页的像素去画旧页的 uv —— 字形缺笔画、整行跳变。
       表现出来就是「某些页面的大字号标题概率性闪烁」，且只在首次出现
       新字号/新字符、正好要开新图集页的那一帧发生（所以是概率性的）。
       renderer.mjs 的 makeWhite / uploadTexture 都做过同一件事，这里漏了。 */
    const prev = gl.getParameter(gl.TEXTURE_BINDING_2D);
    const t = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, t);
    gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, false);
    if (source) gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, source);
    else gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, w, h, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl[filter]);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl[filter]);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.bindTexture(gl.TEXTURE_2D, prev);
    atlasEpoch++;                 // 让 renderer 知道「图集变过了」
    return t;
  }

  function makeAtlas() {
    const cvs = document.createElement('canvas');
    cvs.width = ATLAS_W;
    cvs.height = ATLAS_H;
    const ctx = cvs.getContext('2d');
    ctx.textBaseline = 'alphabetic';
    ctx.textAlign = 'left';
    const tex = createTexture(ATLAS_W, ATLAS_H, null, 'LINEAR');
    return {
      cvs, ctx, tex,
      x: PAD, y: PAD, rowH: 0,
      glyphs: new Map(),
      dirtyTop: Infinity, dirtyBottom: -1,
    };
  }

  /* 取一个能画 font/px 的图集「页」，装不下就新开一页 */
  function atlasFor(family, px, page) {
    const key = `${family}@${px}#${page}`;
    let a = atlases.get(key);
    if (!a) { a = makeAtlas(); atlases.set(key, a); a.px = px; a.family = family; }
    return a;
  }

  /* 光栅化一个字形进图集，返回描述。
     L = **连续**逻辑字号（= font.size × scale），不再取整。 */
  function glyphOf(f, ch, L) {
    const code = ch.codePointAt(0);

    if (f.kind === 7) {
      // 位图字体：不做光栅化，直接在原图上取矩形
      const g = f.fntGlyphs.get(code);
      if (!g) return null;
      const bw = f.bitmapSize.w, bh = f.bitmapSize.h;
      return {
        tex: f.tex,
        dx: g.dx, dy: g.dy, w: g.w, h: g.h, adv: g.adv,
        u0: g.x / bw, v0: g.y / bh, u1: (g.x + g.w) / bw, v1: (g.y + g.h) / bh,
        scaled: true,     // 位图字形的度量是 1x，绘制时要乘 scale
      };
    }

    // TTF：按「设备像素尺寸」光栅化。
    // 1d-31 task#3：字号必须**连续**。以前是
    //     px = round(f.size * scale)        ← 先取整到逻辑像素
    //     realPx = round(px * ssFactor)
    // 结果字号动画（选关界面关卡名放大/缩小）按 1 逻辑像素一格地跳，
    // 看起来「一顿一顿」。现在 L 是连续值，光栅尺寸只量化到 0.5 设备像素
    // （限制图集条目数），度量存**设备像素原值** + dev，排版时再乘 L/dev：
    // 文本宽度随 L 线性变化 ⇒ 绝对平滑；位图只被拉伸 ±0.5/(2R)，看不出来。
    const R = Math.max(1, Math.round(L * ssFactor * 2) / 2);
    let page = 0;
    for (;;) {
      const a = atlasFor(f.family, R, page);
      const hit = a.glyphs.get(code);
      if (hit) return hit;
      const m = measure(f, R, ch);
      const gw = Math.ceil(m.w) + PAD * 2;
      const gh = Math.ceil(m.h) + PAD * 2;
      if (a.x + gw > ATLAS_W) { a.x = PAD; a.y += a.rowH; a.rowH = 0; }
      if (a.y + gh > ATLAS_H) { page++; continue; }

      const gx = a.x + PAD, gy = a.y + PAD;
      a.ctx.font = `${R}px "${f.family}"`;
      a.ctx.fillStyle = '#fff';
      // 字形画在 (gx - bl, gy + bt)：canvas 的 baseline 原点在 (gx, gy+bt) 处
      a.ctx.fillText(ch, gx - m.bl, gy + m.bt);

      const rec = {
        tex: a.tex,
        // dev = 光栅化用的设备像素字号；下面的度量都是**设备像素原值**，
        // 排版时统一乘 (L / dev) 换回逻辑单位（L 是连续逻辑字号）。
        dev: R,
        // 笔触 y 是基线；世界 y 向上，所以字形顶边在基线上方 bt 处。
        dx: m.bl, dy: m.bt,
        w: m.w, h: m.h,
        adv: m.adv,
        u0: gx / ATLAS_W, v0: gy / ATLAS_H,
        u1: (gx + m.w) / ATLAS_W, v1: (gy + m.h) / ATLAS_H,
        scaled: false,    // 已经按设备像素光栅化，按 L/dev 缩放
        atlas: a,
      };
      a.glyphs.set(code, rec);
      a.x += gw;
      if (gh > a.rowH) a.rowH = gh;
      a.dirtyTop = Math.min(a.dirtyTop, a.y);
      a.dirtyBottom = Math.max(a.dirtyBottom, a.y + a.rowH);
      diag.glyphs = atlases.size ? diag.glyphs + 1 : 1;
      return rec;
    }
  }

  /* 单个字形的度量（屏幕像素，未取整） */
  const measureCache = new Map();
  function measure(f, px, ch) {
    const key = f.family + '|' + px + '|' + ch;
    let m = measureCache.get(key);
    if (m) return m;
    const cvs = document.createElement('canvas');
    const ctx = cvs.getContext('2d');
    ctx.font = `${px}px "${f.family}"`;
    const t = ctx.measureText(ch);
    m = {
      w: Math.max(0, t.width),
      bl: t.actualBoundingBoxLeft ? -t.actualBoundingBoxLeft : 0,
      bt: t.actualBoundingBoxAscent || 0,
      h: (t.actualBoundingBoxAscent || 0) + (t.actualBoundingBoxDescent || 0),
      adv: t.width,
      asc: t.fontBoundingBoxAscent || px,
      desc: t.fontBoundingBoxDescent || 0,
    };
    // actualBoundingBoxLeft 向右为正；canvas 里 bitmap_left 要用负值
    m.bl = -(t.actualBoundingBoxLeft || 0);
    measureCache.set(key, m);
    return m;
  }

  /* 字体在某缩放下的行度量（已换算到屏幕像素）。
     1d-31 task#3：字号的取整只能发生在**光栅化**那一步（glyphOf 里的 0.5
     设备像素量化），行度量必须跟着连续字号 L 走，否则行距也会一格格跳。 */
  function metricsOf(f, scale) {
    if (f.kind === 7) {
      return {
        px: 0,
        ascender: f.ascender * scale,
        lineHeight: f.lineHeight * scale,
        scale,
      };
    }
    const L = Math.max(0.5, f.size * scale);
    const R = Math.max(1, Math.round(L * ssFactor * 2) / 2);
    const k = L / R;
    const base = measure(f, R, 'M');
    return {
      px: L,                          // 连续逻辑字号，交给 glyphOf
      ascender: base.asc * k,
      lineHeight: (base.asc + base.desc) * k,
      scale,
    };
  }

  /* 行宽（逻辑单位）。TTF 字形的度量是设备像素原值，按 L/dev 换算。 */
  function lineAdvance(f, text, scale, L) {
    let w = 0;
    for (const ch of text) {
      const g = glyphOf(f, ch, L);
      if (!g) continue;
      w += g.scaled ? g.adv * scale : g.adv * (L / g.dev);
    }
    return w;
  }

  /* ---- 排版：完全照 AppFrame::RenderText ------------------------------
     box = { left, right, bottom, top }（注意 bottom 是较大的 y）
     返回字形四边形列表，坐标已是屏幕像素。
     引擎里 rect = RectF(left, top, right, bottom)，所以 |a.x-b.x| = |left-right|。 */
  function layout(fontName, text, box, scale, fmt) {
    const f = fonts.get(fontName);
    if (!f || !f.ready || !text) return null;

    const M = metricsOf(f, scale);
    const px = M.px;
    const width = Math.abs(box.left - box.right);

    // 1) 切行：显式换行 + 可选按宽度折行（TEXT_FLAG_WORDBREAK）
    const wordBreak = (fmt & FMT.WORDBREAK) !== 0;
    const lines = [];
    let cur = '';
    let curW = 0;
    for (const ch of text) {
      if (ch === '\n') { lines.push({ text: cur, w: curW }); cur = ''; curW = 0; continue; }
      const g = glyphOf(f, ch, px);
      const adv = g ? (g.scaled ? g.adv * scale : g.adv * (px / g.dev)) : 0;
      if (wordBreak && width > 0 && curW + adv > width && cur.length > 0) {
        lines.push({ text: cur, w: curW });
        cur = ''; curW = 0;
      }
      cur += ch;
      curW += adv;
    }
    lines.push({ text: cur, w: curW });

    // 2) 起始笔触 y
    const totalH = M.lineHeight * lines.length;
    const hAlign = (fmt & FMT.H_CENTER) ? 1 : (fmt & FMT.H_RIGHT) ? 2 : 0;
    const vAlign = (fmt & FMT.V_CENTER) ? 1 : (fmt & FMT.V_BOTTOM) ? 2 : 0;

    let penY;
    if (vAlign === 2) penY = box.bottom + totalH;
    else if (vAlign === 1) penY = box.top - Math.abs(box.top - box.bottom) / 2 + totalH / 2;
    else penY = box.top;
    penY -= M.ascender;

    // 3) 逐行出字形
    const quads = [];
    for (let i = 0; i < lines.length; i++) {
      const ln = lines[i];
      let penX = box.left;
      if (hAlign === 2) penX += width - ln.w;
      else if (hAlign === 1) penX += width / 2 - ln.w / 2;

      for (const ch of ln.text) {
        const g = glyphOf(f, ch, px);
        if (!g) continue;
        /* 位图字形按 scale 缩放；TTF 字形按「连续逻辑字号 / 光栅化设备字号」
           缩放 —— 这是 1d-31 task#3 的关键：字号动画不再按整数像素跳。 */
        const s = g.scaled ? scale : (px / g.dev);
        const x = penX + g.dx * s;
        const y = penY + g.dy * s;
        quads.push({
          tex: g.tex,
          x, y, w: g.w * s, h: g.h * s,
          u0: g.u0, v0: g.v0, u1: g.u1, v1: g.v1,
        });
        penX += g.adv * s;
      }
      penY -= M.lineHeight;
    }
    diag.layoutCalls++;
    return quads;
  }

  /* ---- 沿方向向量铺字（ROP_TTF3 / FontRenderer.RenderTextInSpace） ----
     与 layout() 的差别：不做对齐、不折行，只按 (hs, vs) 缩放后从笔触原点铺开，
     由调用方用 rvec/dvec 决定每个字形落在世界的哪里。RenderTTF3 的旋转就是
     靠这两个向量——所以这里必须保留每个字形相对基线的真实偏移，
     否则旋转的锚点会偏。

     返回 { x, yTop, w, h, u0, v0, u1, v1, tex }：
       x    = 左边相对笔触的偏移
       yTop = 字形顶边相对笔触基线的偏移，**y 向上为正**（引擎 GlyphInfo.position.y 语义）
     底边 = yTop - h。 */
  function layoutSpace(fontName, text, hs, vs) {
    const f = fonts.get(fontName);
    if (!f || !f.ready || !text) return null;

    hs = hs || 1;
    vs = vs || 1;
    /* fnt 字形是 1x 度量，横向乘 hs、纵向乘 vs；
       TTF 按「连续逻辑字号 L = size*vs」光栅化，字形度量是设备像素原值，
       统一乘 L/dev 换回逻辑单位，再补横向拉伸 hs/vs。 */
    const kx = f.kind === 7 ? hs : hs / vs;
    const ky = f.kind === 7 ? vs : 1;
    const L = f.kind === 7 ? 0 : Math.max(0.5, f.size * vs);
    let lineH;
    let ks = 1;                       // TTF：L / 光栅化设备字号
    if (f.kind === 7) {
      lineH = f.lineHeight * vs;
    } else {
      const R = Math.max(1, Math.round(L * ssFactor * 2) / 2);
      ks = L / R;
      const bm = measure(f, R, 'M');
      lineH = (bm.asc + bm.desc) * ks;
    }

    const quads = [];
    let penX = 0, line = 0;
    for (const ch of text) {
      if (ch === '\n') { penX = 0; line++; continue; }
      const g = glyphOf(f, ch, L);
      if (!g) continue;
      const gs = g.scaled ? 1 : ks;   // 位图字形本来就是 1x 度量
      quads.push({
        tex: g.tex,
        x: penX + g.dx * gs * kx,
        yTop: g.dy * gs * ky - line * lineH,
        w: g.w * gs * kx, h: g.h * gs * ky,
        u0: g.u0, v0: g.v0, u1: g.u1, v1: g.v1,
      });
      penX += g.adv * gs * kx;
    }
    diag.layoutCalls++;
    return quads;
  }

  /* MeasureTextBoundary 的真实值：[l, r, b, t]，y 向上（t 顶边、b 底边）。
     和引擎 TextRenderer::getTextBoundary 同一套包围盒，游戏拿它做居中/右对齐。 */
  /* 字体的行度量（**字体单位**，未缩放）。
     引擎语义（上游 AppFrameFontRenderer.cpp）：
         FontRenderer_GetFontLineHeight() → getGlyphManager()->getLineHeight()
         FontRenderer_GetFontAscender()   → getGlyphManager()->getAscender()
     两者都是**字体单位**，用的时候自己乘 scale（引擎的 RenderText 就是
     `getLineHeight() * scale.y`）。Cyanlib 的 Core.Render.Text 同理。
     .fnt 位图字体直接用解析出来的 ascender / lineHeight；TTF 量一次
     'M' 的 ascent+descent（和排版时用的是同一套度量）。 */
  function fontMetrics(fontName) {
    const f = fonts.get(fontName);
    if (!f || !f.ready) return null;
    if (f.kind === 7) {
      return { lineHeight: f.lineHeight, ascender: f.ascender };
    }
    const base = measure(f, Math.max(1, f.size), 'M');
    if (!base) return null;
    return { lineHeight: base.asc + base.desc, ascender: base.asc };
  }

  function measureBounds(fontName, text, hs, vs) {
    const q = layoutSpace(fontName, text, hs, vs);
    if (!q || !q.length) return null;
    let l = Infinity, r = -Infinity, t = -Infinity, b = Infinity;
    for (const g of q) {
      if (g.x < l) l = g.x;
      if (g.x + g.w > r) r = g.x + g.w;
      if (g.yTop > t) t = g.yTop;
      if (g.yTop - g.h < b) b = g.yTop - g.h;
    }
    return [l, r, b, t];
  }

  /* CalcuTextSize 的复刻：AppFrameFontRenderer.cpp:163 ——
     返回 { w = 最长行宽, h = 行数 × 行高 }。
     RenderText 的 halign/valign 就是靠它先把 x/y 挪成矩形左上角，
     再交给 layout 二次对齐。之前 ROP_TEXT 完全不看对齐参数，
     结果「centerpoint」的文字整体右移半个字宽、下移一行。 */
  function measureSize(fontName, text, scale) {
    const f = fonts.get(fontName);
    if (!f || !f.ready || !text) return null;
    const px = f.kind === 7 ? 0 : Math.max(0.5, f.size * (scale || 1));
    const M = metricsOf(f, scale || 1);
    let lines = 1, cur = 0, maxW = 0;
    for (const ch of text) {
      if (ch === '\n') {
        if (cur > maxW) maxW = cur;
        cur = 0; lines++;
        continue;
      }
      const g = glyphOf(f, ch, px);
      if (g) cur += g.scaled ? g.adv * scale : g.adv * (px / g.dev);
    }
    if (cur > maxW) maxW = cur;
    return { w: maxW, h: lines * M.lineHeight, lines };
  }

  /* 上传脏区域：新字形只动一小条，不必每帧重传整张图集。
     WebGL2 的 texSubImage2D 不支持「从 canvas 取子矩形」那个重载（那只有
     ArrayBufferView 能用），所以先把脏条带拷到一张等宽的小 canvas 再上传。 */
  let scratch = null;
  function syncAtlases() {
    /* ⚠ TEXTURE_BINDING_2D 是全局绑定点。这里每上传一页都会 bindTexture，
       函数返回时 GL 停在「最后一页」上；而 renderer.mjs 用 curTex 缓存判断
       bindRaw(q.tex) 要不要重绑，缓存并不知道 GL 侧被换过。结果是：多页
       图集时某几条文本命令复用错误页的纹理，字形采样串页 → 文字缺笔画、
       逐帧位置跳动（切换关卡时新字符涌入、图集正好分页，最为明显）。
       必须保存/恢复绑定（1c-1 的 uploadTexture 踩过同一个坑）。 */
    let prev = null, touched = false;
    for (const a of atlases.values()) {
      if (a.dirtyBottom < 0) continue;
      if (!touched) {
        prev = gl.getParameter(gl.TEXTURE_BINDING_2D);
        touched = true;
      }
      const y0 = Math.max(0, a.dirtyTop - 1);
      const y1 = Math.min(ATLAS_H, a.dirtyBottom + 1);
      const bh = Math.max(1, y1 - y0);
      if (!scratch) scratch = document.createElement('canvas');
      if (scratch.width !== ATLAS_W || scratch.height !== bh) {
        scratch.width = ATLAS_W;
        scratch.height = bh;
      }
      const sctx = scratch.getContext('2d');
      sctx.clearRect(0, 0, ATLAS_W, bh);
      sctx.drawImage(a.cvs, 0, y0, ATLAS_W, bh, 0, 0, ATLAS_W, bh);

      gl.bindTexture(gl.TEXTURE_2D, a.tex);
      gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, false);
      gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, false);
      gl.texSubImage2D(gl.TEXTURE_2D, 0, 0, y0, gl.RGBA, gl.UNSIGNED_BYTE, scratch);
      a.dirtyTop = Infinity;
      a.dirtyBottom = -1;
    }
    if (touched) gl.bindTexture(gl.TEXTURE_2D, prev);
  }

  return {
    registerFont,
    prepare,
    preparePending,
    setSuperSampling,
    get superSampling() { return ssFactor; },
    get atlasPages() { return atlases.size; },
    get atlasEpoch() { return atlasEpoch; },
    layout,
    layoutSpace,
    fontMetrics,
    measureBounds,
    measureSize,
    syncAtlases,
    has: (name) => {
      const f = fonts.get(name);
      return !!(f && f.ready);
    },
    get diag() { return diag; },
    fontNames: () => [...fonts.keys()],
    /* 诊断：把字形图集导成 data URL（字形串字的取证用）。
       返回每页的 key / 尺寸 / 已排到哪个位置 / PNG。 */
    atlasDump: (limit = 4) => [...atlases.entries()].slice(0, limit).map(([k, a]) => ({
      key: k, w: ATLAS_W, h: ATLAS_H, x: a.x, y: a.y, rowH: a.rowH,
      glyphs: a.glyphs.size, url: a.cvs.toDataURL('image/png'),
    })),
  };
}
