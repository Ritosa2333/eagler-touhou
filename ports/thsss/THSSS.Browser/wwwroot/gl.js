const textures = new Map();
const framebuffers = new Map();
let gl;
let nextId = 1;
let whiteId = 0;
let spriteProg;
let fixedProg;
let uScreen;
let uFlip;
let uTex;
let fuWorld;
let fuView;
let fuProj;
let fuMaterial;
let fuLighting;
let fuFog;
let fuFogStart;
let fuFogEnd;
let fuFogColor;
let fuTex;
let spriteBuf;
let fixedBuf;
const effectRecs = new Map();
let viewX = 0, viewY = 0, viewW = 640, viewH = 480, fbW = 640, fbH = 480;
const imageJobs = new Map();
let imageJobId = 1;

function compile(type, source) {
  const shader = gl.createShader(type);
  gl.shaderSource(shader, source);
  gl.compileShader(shader);
  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    throw new Error(gl.getShaderInfoLog(shader));
  }
  return shader;
}

function program(vs, fs) {
  const prog = gl.createProgram();
  gl.attachShader(prog, compile(gl.VERTEX_SHADER, vs));
  gl.attachShader(prog, compile(gl.FRAGMENT_SHADER, fs));
  gl.linkProgram(prog);
  if (!gl.getProgramParameter(prog, gl.LINK_STATUS)) {
    throw new Error(gl.getProgramInfoLog(prog));
  }
  return prog;
}

function blendFactor(value) {
  switch (value) {
    case 1: return gl.ZERO;
    case 2: return gl.ONE;
    case 3: return gl.SRC_COLOR;
    case 4: return gl.ONE_MINUS_SRC_COLOR;
    case 5: return gl.SRC_ALPHA;
    case 6: return gl.ONE_MINUS_SRC_ALPHA;
    case 7: return gl.DST_ALPHA;
    case 8: return gl.ONE_MINUS_DST_ALPHA;
    case 9: return gl.DST_COLOR;
    case 10: return gl.ONE_MINUS_DST_COLOR;
    case 11: return gl.SRC_ALPHA_SATURATE;
    default: return gl.SRC_ALPHA;
  }
}

function makeTexture(width, height, pixels, renderTarget) {
  const tex = gl.createTexture();
  gl.bindTexture(gl.TEXTURE_2D, tex);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
  gl.pixelStorei(gl.UNPACK_ALIGNMENT, 1);
  const data = pixels && pixels.length ? pixels : null;
  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, data);
  const id = nextId++;
  const rec = { tex, width, height, renderTarget };
  textures.set(id, rec);
  if (renderTarget) {
    const fb = gl.createFramebuffer();
    gl.bindFramebuffer(gl.FRAMEBUFFER, fb);
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, tex, 0);
    const depth = gl.createRenderbuffer();
    gl.bindRenderbuffer(gl.RENDERBUFFER, depth);
    gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, width, height);
    gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.RENDERBUFFER, depth);
    framebuffers.set(id, fb);
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
  }
  return id;
}

export function createGl(canvas, shaderSources) {
  gl = canvas.getContext("webgl2", { alpha: false, depth: true, antialias: false, stencil: false, preserveDrawingBuffer: true });
  if (!gl) {
    throw new Error("WebGL2 is not available.");
  }
  const spriteVs = `#version 300 es
layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aUv;
layout(location=2) in vec4 aColor;
uniform vec2 uScreen;
out vec2 vUv;
out vec4 vColor;
void main() {
  vec2 c = vec2(aPos.x / uScreen.x * 2.0 - 1.0, 1.0 - aPos.y / uScreen.y * 2.0);
  gl_Position = vec4(c, 0.0, 1.0);
  vUv = aUv;
  vColor = aColor;
}`;
  const spriteFs = `#version 300 es
precision mediump float;
uniform sampler2D uTex;
uniform float uFlipV;
in vec2 vUv;
in vec4 vColor;
out vec4 o;
void main() {
  vec2 uv = vec2(vUv.x, mix(vUv.y, 1.0 - vUv.y, uFlipV));
  vec4 tex = texture(uTex, uv);
  if (uFlipV > 0.5) tex.a = 1.0;
  o = tex * vColor;
}`;
  const fixedVs = `#version 300 es
layout(location=0) in vec3 aPos;
layout(location=1) in vec2 aUv;
layout(location=2) in vec4 aColor;
uniform mat4 uWorld;
uniform mat4 uView;
uniform mat4 uProj;
out vec2 vUv;
out vec4 vColor;
out float vFogZ;
void main() {
  vec4 world = uWorld * vec4(aPos, 1.0);
  vec4 view = uView * world;
  gl_Position = uProj * view;
  gl_Position.z = gl_Position.z * 2.0 - gl_Position.w;
  vFogZ = view.z;
  vUv = aUv;
  vColor = aColor;
}`;
  const fixedFs = `#version 300 es
precision mediump float;
uniform sampler2D uTex;
uniform vec4 uMaterial;
uniform float uLighting;
uniform float uFog;
uniform float uFogStart;
uniform float uFogEnd;
uniform vec3 uFogColor;
in vec2 vUv;
in vec4 vColor;
in float vFogZ;
out vec4 o;
void main() {
  vec4 tex = texture(uTex, vUv);
  vec4 col = tex * vColor;
  if (uLighting > 0.5) col *= uMaterial;
  if (uFog > 0.5) {
    float f = clamp((uFogEnd - vFogZ) / max(1.0, uFogEnd - uFogStart), 0.0, 1.0);
    col.rgb = mix(uFogColor, col.rgb, f);
  }
  o = col;
}`;
  spriteProg = program(spriteVs, spriteFs);
  fixedProg = program(fixedVs, fixedFs);
  for (const name of ["Filter", "Blur", "Warp"]) {
    const vs = shaderSources && shaderSources[name + ".vert"];
    const fs = shaderSources && shaderSources[name + ".frag"];
    if (!vs || !fs) continue;
    const prog = program(vs, fs);
    effectRecs.set(name, {
      prog,
      uScreen: gl.getUniformLocation(prog, "uScreen"),
      uFlip: gl.getUniformLocation(prog, "uFlipV"),
      uTex: gl.getUniformLocation(prog, "uTex"),
      uBlur: gl.getUniformLocation(prog, "BlurLevel"),
      uX: gl.getUniformLocation(prog, "X"),
      uY: gl.getUniformLocation(prog, "Y"),
      uTime: gl.getUniformLocation(prog, "Time"),
      uRadius: gl.getUniformLocation(prog, "Radius"),
      uColorKey: gl.getUniformLocation(prog, "ColorKey")
    });
  }
  uScreen = gl.getUniformLocation(spriteProg, "uScreen");
  uFlip = gl.getUniformLocation(spriteProg, "uFlipV");
  uTex = gl.getUniformLocation(spriteProg, "uTex");
  fuWorld = gl.getUniformLocation(fixedProg, "uWorld");
  fuView = gl.getUniformLocation(fixedProg, "uView");
  fuProj = gl.getUniformLocation(fixedProg, "uProj");
  fuMaterial = gl.getUniformLocation(fixedProg, "uMaterial");
  fuLighting = gl.getUniformLocation(fixedProg, "uLighting");
  fuFog = gl.getUniformLocation(fixedProg, "uFog");
  fuFogStart = gl.getUniformLocation(fixedProg, "uFogStart");
  fuFogEnd = gl.getUniformLocation(fixedProg, "uFogEnd");
  fuFogColor = gl.getUniformLocation(fixedProg, "uFogColor");
  fuTex = gl.getUniformLocation(fixedProg, "uTex");
  spriteBuf = gl.createBuffer();
  fixedBuf = gl.createBuffer();
  const white = new Uint8Array([255, 255, 255, 255]);
  whiteId = makeTexture(1, 1, white, false);
  gl.activeTexture(gl.TEXTURE0);
  gl.enable(gl.BLEND);
  gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
  gl.clearColor(0, 0, 0, 1);
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
  return {
    clear: glClear,
    createTexture: glCreateTexture,
    createRenderTarget: glCreateRenderTarget,
    updateTexture: glUpdateTexture,
    deleteTexture: glDeleteTexture,
    bindTarget: glBindTarget,
    setViewport: glSetViewport,
    setBlend: glSetBlend,
    setDepth: glSetDepth,
    setCull: glSetCull,
    drawSprites: glDrawSprites,
    drawEffect: glDrawEffect,
    drawFixed: glDrawFixed,
    beginImage: glBeginImage,
    imageState: glImageState,
    finishImage: glFinishImage,
    imageWidth: glImageWidth,
    imageHeight: glImageHeight,
    rasterText: glRasterText,
    rasterTextBox: glRasterTextBox,
    paintText: glPaintText
  };
}

export function glClear(r, g, b, a, color, depth) {
  let bits = 0;
  if (color) {
    gl.clearColor(r, g, b, a);
    bits |= gl.COLOR_BUFFER_BIT;
  }
  if (depth) bits |= gl.DEPTH_BUFFER_BIT;
  if (bits) gl.clear(bits);
}

export function glCreateTexture(width, height, pixels) {
  return makeTexture(width | 0, height | 0, pixels, false);
}

export function glBeginImage(bytes) {
  const id = imageJobId++;
  const job = { ready: false, failed: false, decoded: false, width: 0, height: 0, glId: 0, bitmap: null };
  imageJobs.set(id, job);
  const blob = new Blob([bytes]);
  createImageBitmap(blob).then((bitmap) => {
    job.bitmap = bitmap;
    job.width = bitmap.width;
    job.height = bitmap.height;
    job.decoded = true;
  }).catch(() => {
    job.failed = true;
  });
  return id;
}

export function glImageState(id) {
  const job = imageJobs.get(id | 0);
  if (!job || job.failed) return -1;
  if (job.ready) return 2;
  if (job.decoded) return 1;
  return 0;
}

export function glFinishImage(id) {
  const job = imageJobs.get(id | 0);
  if (!job || !job.bitmap) return 0;
  const bitmap = job.bitmap;
  const tex = gl.createTexture();
  gl.bindTexture(gl.TEXTURE_2D, tex);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
  gl.pixelStorei(gl.UNPACK_ALIGNMENT, 1);
  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, bitmap);
  const glId = nextId++;
  textures.set(glId, { tex, width: bitmap.width, height: bitmap.height, renderTarget: false });
  bitmap.close();
  job.bitmap = null;
  job.glId = glId;
  job.ready = true;
  job.decoded = false;
  return glId;
}

export function glImageWidth(id) {
  const job = imageJobs.get(id | 0);
  return job ? job.width : 0;
}

export function glImageHeight(id) {
  const job = imageJobs.get(id | 0);
  return job ? job.height : 0;
}

export function glCreateRenderTarget(width, height) {
  return makeTexture(width | 0, height | 0, null, true);
}

export function glUpdateTexture(id, width, height, pixels) {
  const rec = textures.get(id | 0);
  if (!rec) return;
  gl.bindTexture(gl.TEXTURE_2D, rec.tex);
  gl.pixelStorei(gl.UNPACK_ALIGNMENT, 1);
  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width | 0, height | 0, 0, gl.RGBA, gl.UNSIGNED_BYTE, pixels);
  rec.width = width | 0;
  rec.height = height | 0;
}

export function glDeleteTexture(id) {
  const rec = textures.get(id | 0);
  if (!rec) return;
  gl.deleteTexture(rec.tex);
  const fb = framebuffers.get(id | 0);
  if (fb) gl.deleteFramebuffer(fb);
  textures.delete(id | 0);
  framebuffers.delete(id | 0);
}

export function glBindTarget(id) {
  const fb = id ? framebuffers.get(id | 0) : null;
  gl.bindFramebuffer(gl.FRAMEBUFFER, fb || null);
}

export function glSetViewport(x, y, width, height, canvasWidth, canvasHeight) {
  viewX = x | 0;
  viewY = y | 0;
  viewW = Math.max(1, width | 0);
  viewH = Math.max(1, height | 0);
  fbW = Math.max(1, canvasWidth | 0);
  fbH = Math.max(1, canvasHeight | 0);
  gl.viewport(viewX, fbH - viewY - viewH, viewW, viewH);
}

export function glSetBlend(enable, src, dst, equation) {
  if (enable) gl.enable(gl.BLEND);
  else gl.disable(gl.BLEND);
  const eq = equation | 0;
  let mode = gl.FUNC_ADD;
  if (eq === 2) mode = gl.FUNC_SUBTRACT;
  else if (eq === 3) mode = gl.FUNC_REVERSE_SUBTRACT;
  else if (eq === 4) mode = gl.MIN;
  else if (eq === 5) mode = gl.MAX;
  gl.blendEquation(mode);
  gl.blendFunc(blendFactor(src | 0), blendFactor(dst | 0));
}

export function glSetDepth(enable, write) {
  if (enable) {
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);
  } else {
    gl.disable(gl.DEPTH_TEST);
  }
  gl.depthMask(write !== false);
}

export function glSetCull(mode) {
  const m = mode | 0;
  if (m === 2 || m === 3) {
    gl.enable(gl.CULL_FACE);
    gl.cullFace(m === 2 ? gl.FRONT : gl.BACK);
  } else {
    gl.disable(gl.CULL_FACE);
  }
}

function bindTex(id) {
  const rec = textures.get(id | 0) || textures.get(whiteId);
  gl.bindTexture(gl.TEXTURE_2D, rec.tex);
}

function asF32(data) {
  if (!data) return new Float32Array(0);
  if (data instanceof Float32Array) return data;
  const u8 = data instanceof Uint8Array ? data : new Uint8Array(data);
  const count = u8.byteLength >> 2;
  if ((u8.byteOffset & 3) === 0) return new Float32Array(u8.buffer, u8.byteOffset, count);
  const copy = new Float32Array(count);
  new Uint8Array(copy.buffer).set(u8.subarray(0, count * 4));
  return copy;
}

let activeProg = 0;
let spriteReady = false;
let fixedReady = false;

function prepSprite() {
  if (spriteReady && activeProg === 1) return;
  activeProg = 1;
  spriteReady = true;
  fixedReady = false;
  gl.useProgram(spriteProg);
  gl.bindBuffer(gl.ARRAY_BUFFER, spriteBuf);
  const stride = 32;
  gl.enableVertexAttribArray(0);
  gl.enableVertexAttribArray(1);
  gl.enableVertexAttribArray(2);
  gl.vertexAttribPointer(0, 2, gl.FLOAT, false, stride, 0);
  gl.vertexAttribPointer(1, 2, gl.FLOAT, false, stride, 8);
  gl.vertexAttribPointer(2, 4, gl.FLOAT, false, stride, 16);
  gl.uniform1i(uTex, 0);
}

function prepFixed() {
  if (fixedReady && activeProg === 2) return;
  activeProg = 2;
  fixedReady = true;
  spriteReady = false;
  gl.useProgram(fixedProg);
  gl.bindBuffer(gl.ARRAY_BUFFER, fixedBuf);
  const stride = 36;
  gl.enableVertexAttribArray(0);
  gl.enableVertexAttribArray(1);
  gl.enableVertexAttribArray(2);
  gl.vertexAttribPointer(0, 3, gl.FLOAT, false, stride, 0);
  gl.vertexAttribPointer(1, 2, gl.FLOAT, false, stride, 12);
  gl.vertexAttribPointer(2, 4, gl.FLOAT, false, stride, 20);
  gl.uniform1i(fuTex, 0);
}

function prepEffect(rec) {
  activeProg = 3;
  spriteReady = false;
  fixedReady = false;
  gl.useProgram(rec.prog);
  gl.bindBuffer(gl.ARRAY_BUFFER, spriteBuf);
  const stride = 32;
  gl.enableVertexAttribArray(0);
  gl.enableVertexAttribArray(1);
  gl.enableVertexAttribArray(2);
  gl.vertexAttribPointer(0, 2, gl.FLOAT, false, stride, 0);
  gl.vertexAttribPointer(1, 2, gl.FLOAT, false, stride, 8);
  gl.vertexAttribPointer(2, 4, gl.FLOAT, false, stride, 16);
  gl.uniform1i(rec.uTex, 0);
}

export function glDrawEffect(name, textureId, flipV, raw, blurLevel, x, y, time, radius, colorKey) {
  const rec = effectRecs.get(name);
  if (!rec) {
    glDrawSprites(textureId, flipV, raw);
    return;
  }
  const data = asF32(raw);
  if (data.length < 8) return;
  prepEffect(rec);
  gl.bufferData(gl.ARRAY_BUFFER, data, gl.DYNAMIC_DRAW);
  gl.uniform2f(rec.uScreen, viewW, viewH);
  gl.uniform1f(rec.uFlip, flipV);
  if (rec.uBlur) gl.uniform1f(rec.uBlur, blurLevel);
  if (rec.uX) gl.uniform1f(rec.uX, x);
  if (rec.uY) gl.uniform1f(rec.uY, y);
  if (rec.uTime) gl.uniform1f(rec.uTime, time);
  if (rec.uRadius) gl.uniform1f(rec.uRadius, radius);
  if (rec.uColorKey) gl.uniform1i(rec.uColorKey, colorKey | 0);
  bindTex(textureId);
  const bound = textures.get(textureId | 0) || textures.get(whiteId);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.MIRRORED_REPEAT);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.MIRRORED_REPEAT);
  gl.drawArrays(gl.TRIANGLES, 0, (data.length / 8) | 0);
  gl.bindTexture(gl.TEXTURE_2D, bound.tex);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
}

export function glDrawSprites(textureId, flipV, raw) {
  const data = asF32(raw);
  if (data.length < 8) return;
  prepSprite();
  gl.bufferData(gl.ARRAY_BUFFER, data, gl.DYNAMIC_DRAW);
  gl.uniform2f(uScreen, viewW, viewH);
  gl.uniform1f(uFlip, flipV);
  bindTex(textureId);
  gl.drawArrays(gl.TRIANGLES, 0, (data.length / 8) | 0);
}

export function glDrawFixed(textureId, raw, worldRaw, viewRaw, projRaw, mr, mg, mb, ma, lighting, fog, fogStart, fogEnd, fr, fg, fb) {
  const data = asF32(raw);
  const world = asF32(worldRaw);
  const view = asF32(viewRaw);
  const proj = asF32(projRaw);
  if (data.length < 9) return;
  prepFixed();
  gl.bufferData(gl.ARRAY_BUFFER, data, gl.DYNAMIC_DRAW);
  gl.uniformMatrix4fv(fuWorld, false, world);
  gl.uniformMatrix4fv(fuView, false, view);
  gl.uniformMatrix4fv(fuProj, false, proj);
  gl.uniform4f(fuMaterial, mr, mg, mb, ma);
  gl.uniform1f(fuLighting, lighting);
  gl.uniform1f(fuFog, fog);
  gl.uniform1f(fuFogStart, fogStart);
  gl.uniform1f(fuFogEnd, fogEnd);
  gl.uniform3f(fuFogColor, fr, fg, fb);
  bindTex(textureId);
  gl.drawArrays(gl.TRIANGLES, 0, (data.length / 9) | 0);
}

let textCanvas;
let textCtx;

function textContext(width, height) {
  const w = Math.max(1, width | 0);
  const h = Math.max(1, height | 0);
  if (!textCanvas) {
    textCanvas = document.createElement("canvas");
    textCtx = textCanvas.getContext("2d", { willReadFrequently: true });
  }
  if (textCanvas.width !== w) textCanvas.width = w;
  if (textCanvas.height !== h) textCanvas.height = h;
  else textCtx.clearRect(0, 0, w, h);
  return textCtx;
}

function paintText(text, width, height, sizePx, bold, italic, argb, family) {
  const ctx = textContext(width, height);
  const px = sizePx || 16;
  const face = family && family !== "GenericSansSerif" ? family : "sans-serif";
  ctx.font = `${italic ? "italic " : ""}${bold ? "bold " : ""}${px}px "${face}", "Microsoft YaHei", "SimSun", "Noto Sans SC", sans-serif`;
  let r = 255, g = 255, b = 255, a = 255;
  if (argb !== undefined && argb !== null) {
    const n = argb | 0;
    a = (n >>> 24) & 255;
    r = (n >>> 16) & 255;
    g = (n >>> 8) & 255;
    b = n & 255;
  }
  ctx.fillStyle = `rgba(${r},${g},${b},${(a / 255).toFixed(3)})`;
  ctx.textBaseline = "alphabetic";
  const lines = String(text || "").replace(/\r\n/g, "\n").replace(/\r/g, "\n").split("\n");
  const step = px * 1.35;
  const metrics = ctx.measureText(lines.join(""));
  const ascent = Math.max(metrics.actualBoundingBoxAscent || 0, metrics.fontBoundingBoxAscent || 0, px);
  const baseline = Math.ceil(ascent) + 2;
  for (let i = 0; i < lines.length; i++) {
    ctx.fillText(lines[i], 0, baseline + i * step);
  }
  return new Uint8Array(ctx.getImageData(0, 0, textCanvas.width, textCanvas.height).data);
}

export function glPaintText(id, text, width, height, sizePx, bold, italic, argb, family) {
  const rec = textures.get(id | 0);
  if (!rec) return;
  const pixels = paintText(text, width, height, sizePx, bold, italic, argb, family);
  const w = textCanvas.width;
  const h = textCanvas.height;
  gl.bindTexture(gl.TEXTURE_2D, rec.tex);
  gl.pixelStorei(gl.UNPACK_ALIGNMENT, 1);
  if (rec.width === w && rec.height === h) {
    gl.texSubImage2D(gl.TEXTURE_2D, 0, 0, 0, w, h, gl.RGBA, gl.UNSIGNED_BYTE, pixels);
  } else {
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, w, h, 0, gl.RGBA, gl.UNSIGNED_BYTE, pixels);
    rec.width = w;
    rec.height = h;
  }
}

export function glRasterText(text, width, height, sizePx, bold, italic) {
  return paintText(text, width, height, sizePx, bold, italic);
}

export function glRasterTextBox(text, width, height, sizePx, bold, italic, argb, family) {
  return paintText(text, width, height, sizePx, bold, italic, argb, family);
}
