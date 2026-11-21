/*
 * RGL Web runtime -- 后期效果（post effect）执行器
 *
 * 游戏的渲染链大量依赖后期：世界先画进 RenderTarget，再经一个后期着色器
 * 贴回屏幕；灰度/色相/亮度也是后期。少这一层，世界层就永远只剩黑屏。
 *
 * 着色器来源是 assets/shader/*.hlsl（D3D11 HLSL）。这里**手翻成 GLSL ES 3.0**，
 * 不写 HLSL 转译器：全项目只有 8 个像素着色器，逐个翻更可靠、更好读，
 * 而且它们的语义（cbuffer 布局、engine_data 里的 screen_texture）必须逐个核对。
 *
 * 引擎注入的统一约定（见各 hlsl 头部注释）：
 *   screen_texture        t4  **PostEffect 第二个参数指定的那张渲染目标**
 *   screen_texture_size   b1  纹理尺寸
 *   viewport              b1  视口
 *   user_data_0..7        b0  用户传的 float4 序列
 *
 * 最容易搞错的一点就是 screen_texture 的来源：它不是「当前帧缓冲」，
 * 而是 PostEffect(shader, name, ...) 里的 name。WorldTex / mask_fader /
 * screen_save 这一整条合成链全靠它把渲染目标贴回屏幕；绑成当前帧缓冲
 * 的话（常常是刚 pop 回来的空屏幕）整条链只会输出黑，且没有任何报错。
 * 只有 name 为空时才退化成「抓当前帧缓冲」。
 *
 * 关键实现细节：D3D 里 screen_texture 允许同时是写入目标（驱动内部拷贝）。
 * WebGL 不允许读写同一张纹理，所以退化路径必须先把当前 framebuffer
 * blit 到一张 scratch 纹理，着色器读 scratch。
 */

const VERT = `#version 300 es
layout(location = 0) in vec2 a_pos;     // 裁剪空间 [-1,1]
out vec2 v_uv;                          // GL 纹理坐标：v=0 在屏幕底
void main() {
  v_uv = a_pos * 0.5 + 0.5;
  gl_Position = vec4(a_pos, 0.0, 1.0);
}`;

/* 所有效果共用的 uniform 前缀，其余各自追加。
   注意 uv 约定：v_uv.y 是 GL 的 t（0 = 抓取结果的底行）。
   HLSL 里的 uv 是「0 在顶部」，所以各效果内部统一用
   `vec2 uv = vec2(v_uv.x, 1.0 - v_uv.y);` 再采样时翻回去。 */
const HEADER = `#version 300 es
precision mediump float;
in vec2 v_uv;
out vec4 o_col;
uniform sampler2D u_screen;
uniform vec2 u_screenSize;
uniform vec4 u_viewport;
`;

/* 把「顶部为 0」的 uv 采样回 GL 坐标 */
const SAMPLE = `
vec4 sampleScreen(vec2 uv) { return texture(u_screen, vec2(uv.x, 1.0 - uv.y)); }
`;

const EFFECTS = {
  /* 直接拷贝。未知着色器的兜底——宁可原样显示，也不要黑屏。 */
  passthrough: {
    frag: `${HEADER}${SAMPLE}
void main() { o_col = texture(u_screen, v_uv); }`,
  },

  /* screen_save.hlsl：screen_texture * alpha */
  'screen_save': {
    frag: `${HEADER}${SAMPLE}
uniform vec4 user_data_0;
void main() {
  o_col = sampleScreen(vec2(v_uv.x, 1.0 - v_uv.y)) * user_data_0.x;
}`,
  },

  /* alpha.hlsl：当前版本就是原样采样（原作者的居中裁剪分支被注释掉了） */
  'alpha': {
    frag: `${HEADER}${SAMPLE}
uniform vec4 user_data_0;
void main() {
  o_col = sampleScreen(vec2(v_uv.x, 1.0 - v_uv.y));
}`,
  },

  /* gray.hlsl：按 alpha 与灰度值混合；flag>0.99 时保留原 alpha */
  'gray': {
    frag: `${HEADER}${SAMPLE}
uniform vec4 user_data_0;
void main() {
  vec2 uv = vec2(v_uv.x, 1.0 - v_uv.y);
  vec2 xy = uv * u_screenSize;
  if (xy.x < u_viewport.x || xy.x > u_viewport.z ||
      xy.y < u_viewport.y || xy.y > u_viewport.w) discard;
  vec4 c = sampleScreen(uv);
  float h = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
  float a = user_data_0.x;
  float flag = user_data_0.y;
  float outA = flag > 0.99 ? c.a : 1.0;
  o_col = vec4(mix(c.rgb, vec3(h), a), outA);
}`,
  },

  /* brightness_set.hlsl：四通道统一乘 delta（原脚本连 alpha 一起乘，照抄） */
  'brightness_set': {
    frag: `${HEADER}${SAMPLE}
uniform vec4 user_data_0;
void main() {
  vec4 c = sampleScreen(vec2(v_uv.x, 1.0 - v_uv.y));
  o_col = c * user_data_0.x;
}`,
  },

  /* hue_set.hlsl：YIQ 旋转色相。参数：Hue、flag、open */
  'hue_set': {
    frag: `${HEADER}${SAMPLE}
uniform vec4 user_data_0;
void main() {
  vec4 c = sampleScreen(vec2(v_uv.x, 1.0 - v_uv.y));
  float hue = user_data_0.x;
  float open = user_data_0.z;
  float angle = hue * 3.14159265;
  float s = sin(angle), co = cos(angle);
  // RGB → YIQ → 绕 I/Q 平面旋转 → 回 RGB
  float y = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
  float i = 0.596 * c.r - 0.274 * c.g - 0.322 * c.b;
  float q = 0.211 * c.r - 0.523 * c.g + 0.312 * c.b;
  float i2 = i * co - q * s;
  float q2 = i * s + q * co;
  vec3 rgb = vec3(
    y + 0.956 * i2 + 0.621 * q2,
    y - 0.272 * i2 - 0.647 * q2,
    y - 1.106 * i2 + 1.703 * q2);
  o_col = vec4(mix(c.rgb, clamp(rgb, 0.0, 1.0), open), c.a);
}`,
  },

  /* fade.hlsl：按高度图把世界「擦出来」，mask_fader 的开门/关门动画就靠它。
     具名 uniform：height、width、edge_color；纹理：u_texture、u_texture_h */
  'fade': {
    frag: `${HEADER}${SAMPLE}
uniform sampler2D u_texture;
uniform sampler2D u_texture_h;
uniform float u_height;
uniform float u_width;
uniform vec4 u_edge_color;
void main() {
  vec2 uv = vec2(v_uv.x, 1.0 - v_uv.y);
  vec4 ori = texture(u_texture, vec2(uv.x, 1.0 - uv.y));
  float hmap = texture(u_texture_h, vec2(uv.x, 1.0 - uv.y)).r;
  float a = clamp(u_height - hmap, 0.0, u_width) / max(u_width, 1e-5);
  vec4 ec = u_edge_color;
  if (a <= 0.0) {
    ori.a = 0.0;
    ec.a = 0.0;
  } else {
    ec.a = ori.a * (0.5 + 0.5 * a);
  }
  o_col = mix(ec, ori, a);
}`,
    needs: ['u_texture', 'u_texture_h'],
  },

  /* ---- BRO 特有 shader（RGL 没有，1c 手翻）---- */

  /* screen_save.hlsl（BRO 版：强制不透明，忽略 user alpha） */
  'screen_save_bro': {
    frag: `${HEADER}${SAMPLE}
void main() {
  vec4 c = sampleScreen(vec2(v_uv.x, 1.0 - v_uv.y));
  c.a = 1.0;
  o_col = c;
}`,
  },

  /* color_set.hlsl：open<0.99 时按 flag 保留/清 alpha；
     open>=0.99 时算灰度 l=0.299r+0.587g+0.114b，s=1-l，HSL2RGB(Hue,s,l) 重着色 */
  'color_set': {
    frag: `${HEADER}${SAMPLE}
uniform vec4 user_data_0;
float hue2rgb(float v1, float v2, float vH) {
  if (vH < 0.0) vH += 1.0;
  if (vH > 1.0) vH -= 1.0;
  if (6.0*vH < 1.0) return v1 + (v2-v1)*6.0*vH;
  if (2.0*vH < 1.0) return v2;
  if (3.0*vH < 2.0) return v1 + (v2-v1)*(2.0/3.0 - vH)*6.0;
  return v1;
}
vec3 hsl2rgb(float H, float S, float L) {
  if (S == 0.0) return vec3(L);
  float var2 = (L < 0.5) ? L*(1.0+S) : (L+S)-(S*L);
  float var1 = 2.0*L - var2;
  return vec3(hue2rgb(var1,var2,H+1.0/3.0),
              hue2rgb(var1,var2,H),
              hue2rgb(var1,var2,H-1.0/3.0));
}
void main() {
  vec4 c = sampleScreen(vec2(v_uv.x, 1.0 - v_uv.y));
  float Hue = user_data_0.x, flag = user_data_0.y, open = user_data_0.z;
  float outA = (flag > 0.99) ? c.a : 1.0;
  if (open >= 0.99) {
    float l = 0.299*c.r + 0.587*c.g + 0.114*c.b;
    float s = 1.0 - l;
    c = vec4(hsl2rgb(Hue, s, l), outA);
  } else {
    c.a = outA;
  }
  o_col = c;
}`,
  },

  /* gaussian_blur.hlsl（BRO 版：box 判定内原样采样，否则 1/4 亮度 + 不透明） */
  'gaussian_blur': {
    frag: `${HEADER}${SAMPLE}
uniform vec4 user_data_0;
uniform vec4 user_data_1;
uniform vec4 user_data_2;
bool ifinbox(vec2 p1, vec2 p2, vec2 p4, vec2 p0) {
  vec2 V1=p2-p1, V2=p4-p1, V3=p0-p1;
  float D1=dot(V1,V3), D2=dot(V2,V3), D3=dot(V1,V1), D4=dot(V2,V2);
  return (D1>0.0 && D1<D3 && D2>0.0 && D2<D4);
}
void main() {
  vec2 uv = vec2(v_uv.x, 1.0 - v_uv.y);
  vec2 xy = uv * u_screenSize;
  vec4 c = sampleScreen(uv);
  vec2 A=vec2(user_data_1.x,user_data_2.x);
  vec2 B=vec2(user_data_1.y,user_data_2.y);
  vec2 D=vec2(user_data_1.w,user_data_2.w);
  if (!ifinbox(A,B,D,xy)) { c = vec4(c.rgb*0.25, 1.0); }
  o_col = c;
}`,
  },

  /* world_rotate / texture_BoxBlur49 用到极少，直通（画面不错位，少一点效果） */
  'world_rotate': { frag: `${HEADER}${SAMPLE}\nvoid main() { o_col = texture(u_screen, v_uv); }` },
  'texture_BoxBlur49': { frag: `${HEADER}${SAMPLE}\nvoid main() { o_col = texture(u_screen, v_uv); }` },
  'thl_distortion': { frag: `${HEADER}${SAMPLE}\nvoid main() { o_col = texture(u_screen, v_uv); }` },
};

const basename = (p) => {
  const s = String(p).replace(/\\/g, '/');
  const f = s.slice(s.lastIndexOf('/') + 1);
  return f.replace(/\.(hlsl|psi)$/i, '');
};

export function createPostFX(gl) {
  const programs = new Map();      // key → { prog, locs }
  const scratch = { fbo: null, tex: null, w: 0, h: 0 };
  let quadVao = null;
  /* byKey 是**诊断用**的每效果调用计数：评估「全屏 pass 数」时唯一可信的口径。
     drawn 是累计值，per-frame 需要配合 resetStats() 才能读。 */
  const stats = { drawn: 0, namedSrc: 0, captureSrc: 0, unknown: [], capped: 0,
                  byKey: Object.create(null) };

  function ensureQuad() {
    if (quadVao) return quadVao;
    /* ARRAY_BUFFER 是全局绑定点，bindVertexArray(null) 不会把它还原。
       这里若不留神，sprite 管线的 bufferSubData 会写进这张 32 字节的
       全屏四边形 VBO —— 越界报 INVALID_VALUE 且画面全黑。 */
    const prevBuf = gl.getParameter(gl.ARRAY_BUFFER_BINDING);
    quadVao = gl.createVertexArray();
    gl.bindVertexArray(quadVao);
    const buf = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, buf);
    gl.bufferData(gl.ARRAY_BUFFER,
      new Float32Array([-1, -1, 1, -1, -1, 1, 1, 1]), gl.STATIC_DRAW);
    gl.enableVertexAttribArray(0);
    gl.vertexAttribPointer(0, 2, gl.FLOAT, false, 0, 0);
    gl.bindVertexArray(null);
    gl.bindBuffer(gl.ARRAY_BUFFER, prevBuf);
    return quadVao;
  }

  function compile(key, frag) {
    let rec = programs.get(key);
    if (rec) return rec;
    const vs = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vs, VERT);
    gl.compileShader(vs);
    if (!gl.getShaderParameter(vs, gl.COMPILE_STATUS)) {
      throw new Error('postfx vert: ' + gl.getShaderInfoLog(vs));
    }
    const fs = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fs, frag);
    gl.compileShader(fs);
    if (!gl.getShaderParameter(fs, gl.COMPILE_STATUS)) {
      throw new Error(`postfx frag(${key}): ` + gl.getShaderInfoLog(fs));
    }
    const prog = gl.createProgram();
    gl.attachShader(prog, vs);
    gl.attachShader(prog, fs);
    gl.linkProgram(prog);
    if (!gl.getProgramParameter(prog, gl.LINK_STATUS)) {
      throw new Error(`postfx link(${key}): ` + gl.getProgramInfoLog(prog));
    }
    rec = { prog, locs: new Map() };
    programs.set(key, rec);
    return rec;
  }

  function loc(rec, name) {
    if (rec.locs.has(name)) return rec.locs.get(name);
    const l = gl.getUniformLocation(rec.prog, name);
    rec.locs.set(name, l);
    return l;
  }

  /** 把当前 framebuffer 抓成一张可采样纹理（截到 viewport 区域） */
  function capture(viewW, viewH) {
    /* 首次创建 scratch 会动单元 0 的纹理绑定和 FRAMEBUFFER。以前把
       FRAMEBUFFER 留在 scratch.fbo 上，下面第 250 行读到的「当前绑定」
       就是自己刚设的那张，恢复等于没恢复 —— 该 POSTEFFECT 之后本帧
       剩余的 sprite 全画进离屏纹理，表现为整帧闪烁/后半帧消失。 */
    const prevTex = gl.getParameter(gl.TEXTURE_BINDING_2D);
    const prevFbo = gl.getParameter(gl.FRAMEBUFFER_BINDING);
    if (!scratch.fbo || scratch.w !== viewW || scratch.h !== viewH) {
      if (scratch.tex) gl.deleteTexture(scratch.tex);
      if (scratch.fbo) gl.deleteFramebuffer(scratch.fbo);
      scratch.tex = gl.createTexture();
      gl.bindTexture(gl.TEXTURE_2D, scratch.tex);
      gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, viewW, viewH, 0,
        gl.RGBA, gl.UNSIGNED_BYTE, null);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
      scratch.fbo = gl.createFramebuffer();
      gl.bindFramebuffer(gl.FRAMEBUFFER, scratch.fbo);
      gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0,
        gl.TEXTURE_2D, scratch.tex, 0);
      gl.bindFramebuffer(gl.FRAMEBUFFER, prevFbo);
      gl.bindTexture(gl.TEXTURE_2D, prevTex);
      scratch.w = viewW;
      scratch.h = viewH;
    }
    /* 从当前读缓冲 blit 到 scratch。当前绑定可能是默认帧缓冲，
       也可能是别的 FBO，blit 对两者都成立。 */
    const cur = gl.getParameter(gl.FRAMEBUFFER_BINDING);
    gl.bindFramebuffer(gl.READ_FRAMEBUFFER, cur);
    gl.bindFramebuffer(gl.DRAW_FRAMEBUFFER, scratch.fbo);
    gl.blitFramebuffer(0, 0, viewW, viewH, 0, 0, viewW, viewH,
      gl.COLOR_BUFFER_BIT, gl.NEAREST);
    gl.bindFramebuffer(gl.FRAMEBUFFER, cur);
    return scratch.tex;
  }

  /**
   * 执行一个后期效果。
   * spec = { fx, tex, named:{k:v}, args:[[..]], textures:{k:texname} }
   * ctx  = { viewW, viewH, viewport:[l,r,b,t], resolve(texname)->WebGLTexture,
   *          blendApply(fn) }
   */
  function draw(spec, ctx) {
    const key = basename(spec.fx) || 'passthrough';
    const def = EFFECTS[key] || EFFECTS.passthrough;
    if (!EFFECTS[key] && stats.unknown.length < 16) stats.unknown.push(spec.fx);

    const rec = compile(key, def.frag);

    gl.useProgram(rec.prog);
    ensureQuad();

    /* 画面源：只有着色器真的用到 screen_texture 才去取——fade / gray 这类
       用自带的 u_texture，白抓一次整屏 blit 纯属浪费（1440p 下每帧几个 MB）。 */
    const screenLoc = loc(rec, 'u_screen');
    let srcW = ctx.viewW, srcH = ctx.viewH;
    if (screenLoc !== null) {
      let src;
      const namedTex = spec.tex ? ctx.resolve(spec.tex) : null;
      if (namedTex) {
        src = namedTex;
        const info = ctx.texSize ? ctx.texSize(spec.tex) : null;
        if (info) { srcW = info.w; srcH = info.h; }
        stats.namedSrc++;
      } else {
        src = capture(ctx.viewW, ctx.viewH);
        stats.captureSrc++;
      }
      gl.activeTexture(gl.TEXTURE0);
      gl.bindTexture(gl.TEXTURE_2D, src);
      gl.uniform1i(screenLoc, 0);
    }
    gl.uniform2f(loc(rec, 'u_screenSize'), srcW, srcH);
    gl.uniform4f(loc(rec, 'u_viewport'),
      ctx.viewport[0], ctx.viewport[1], ctx.viewport[2], ctx.viewport[3]);

    // 用户位置参数 → user_data_0..7
    for (let i = 0; i < spec.args.length && i < 8; i++) {
      const a = spec.args[i];
      gl.uniform4f(loc(rec, 'user_data_' + i),
        a[0] || 0, a[1] || 0, a[2] || 0, a[3] || 0);
    }

    // 具名 uniform：标量与 vec4 都在这里
    let unit = 1;
    for (const [name, val] of Object.entries(spec.named)) {
      const l = loc(rec, 'u_' + name);
      if (l === null) continue;                 // 着色器没用这个 uniform
      if (Array.isArray(val)) {
        gl.uniform4f(l, val[0] || 0, val[1] || 0, val[2] || 0, val[3] || 0);
      } else {
        gl.uniform1f(l, val);
      }
    }

    /* 具名纹理：setTexture 的键就是**完整 uniform 名**。
       shader.lua 传的是 "u_texture" / "u_texture_h"；C 侧把它们原样写进
       POSTEFFECT 字符串的第 4 段。这里不能再补一次 "u_" 前缀——否则
       getUniformLocation("u_u_texture_h") 恒为 null，整条 continue，
       fade 的两个 sampler 会一直留在默认的纹理单元 0：既读不到 mask_fader
       渲染目标，也读不到 fade_effect 高度图，合成结果恒为全黑。
       兼容两种写法（有的着色器可能用不带前缀的键）。 */
    for (const [name, texName] of Object.entries(spec.textures)) {
      const l = loc(rec, name) ?? loc(rec, 'u_' + name);
      if (l === null) continue;
      const t = ctx.resolve(texName);
      gl.activeTexture(gl.TEXTURE0 + unit);
      gl.bindTexture(gl.TEXTURE_2D, t);
      gl.uniform1i(l, unit);
      unit++;
    }

    ctx.blendApply();
    gl.bindVertexArray(quadVao);
    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
    gl.bindVertexArray(null);
    gl.activeTexture(gl.TEXTURE0);
    stats.drawn++;
    stats.byKey[key] = (stats.byKey[key] || 0) + 1;
  }

  return {
    draw,
    /* 诊断：清空计数，下一次读取就是「这一段」的 pass 分布 */
    resetStats() {
      stats.drawn = 0; stats.namedSrc = 0; stats.captureSrc = 0;
      stats.capped = 0; stats.byKey = Object.create(null);
      stats.unknown.length = 0;
    },
    get stats() {
      return {
        drawn: stats.drawn, namedSrc: stats.namedSrc, captureSrc: stats.captureSrc,
        unknown: stats.unknown.slice(), programs: programs.size,
        byKey: { ...stats.byKey },
      };
    },
  };
}
