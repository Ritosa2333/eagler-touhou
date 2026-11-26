const confirmationRequired = (1 << 5) | (1 << 7) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12);
const clockImports = new Set(['_emscripten_get_now', '_emscripten_date_now']);
const graphicsCalls = /^(?:ActiveTexture|Bind(?:Buffer|Framebuffer|Sampler|Texture|VertexArray)|Blend(?:Color|Equation|EquationSeparate|Func|FuncSeparate)|BlitFramebuffer|Buffer(?:Sub)?Data|Clear(?:Buffer[fiuv]+|Color|Depthf|Stencil)?|ColorMask|CullFace|DepthFunc|DepthMask|DepthRangef|Disable(?:VertexAttribArray)?|Draw(?:Arrays(?:Instanced)?|Elements(?:Instanced)?|RangeElements)|Enable(?:VertexAttribArray)?|Flush|FrontFace|GetError|PixelStorei|Scissor|Stencil(?:Func|Mask|Op)(?:Separate)?|SamplerParameter[fi]|TexParameter[fi]|Uniform[1234][fi]v?|UniformMatrix[234]fv|UseProgram|VertexAttrib(?:Divisor|Pointer)|Viewport)$/;

export function supportsRollback() {
  // A core WebAssembly module with two linear memories. Older browsers use the exact-input engine.
  return WebAssembly.validate(Uint8Array.of(0, 97, 115, 109, 1, 0, 0, 0, 5, 5, 2, 0, 1, 0, 1));
}

export function createRollbackPreview() {
  const blocked = {};
  let phase = '', activeJournal = null, blockedImport = '', clock = 0;
  const observation = {presented: 0, fences: 0, corrections: 0, captureMs: 0, restoreMs: 0, stepMs: 0, heapBytes: 0, savedBytes: 0};
  const shown = new Map();

  function guardImports(imports) {
    for (const [key, fn] of Object.entries(imports)) {
      if (typeof fn !== 'function') continue;
      const name = fn.name;
      const graphics = name.startsWith('_emscripten_gl') && graphicsCalls.test(name.slice(14));
      imports[key] = function (...args) {
        if (phase) {
          if (clockImports.has(name)) return clock;
          if (name === "_emscripten_has_asyncify") return fn();
          if (phase === 'present' && name === '_emscripten_get_canvas_element_size') {
            activeJournal.mark(args[1], 4);
            activeJournal.mark(args[2], 4);
            return fn(...args);
          }
          if (phase !== 'present' || !graphics) {
            blockedImport = name || key;
            throw blocked;
          }
        }
        return fn.apply(this, args);
      };
    }
  }

  function render(game, frame, future, now, present = true) {
    if (!future.length || future.some(inputs => inputs.some(bits => bits & confirmationRequired))) return false;
    const heap = game.HEAPU8;
    const journal = game.thibpJournal;
    if (!heap?.length || !journal) return false;
    clock = now;
    blockedImport = '';
    observation.heapBytes = heap.length;
    let timestamp = performance.now();
    journal.begin();
    activeJournal = journal;
    observation.captureMs = performance.now() - timestamp;
    timestamp = performance.now();
    const stack = game.ph3StackSave();
    try {
      phase = 'simulate';
      if (!game._thibp_coop_can_preview()) return false;
      for (let offset = 0; offset < future.length; offset++) {
        if (game._thibp_coop_preview(frame + offset, ...future[offset]) !== 1) return false;
      }
      if (present) {
        phase = 'present';
        if (game._thibp_coop_preview_draw() !== 1) return false;
      }
      for (let offset = 0; offset < future.length; offset++) shown.set(frame + offset, future[offset]);
      observation.presented++;
      return true;
    } catch (error) {
      if (error !== blocked && !journal.failed()) throw error;
      observation.fences++;
      observation.blockedImport = blockedImport || 'journal limit';
      return false;
    } finally {
      phase = '';
      observation.stepMs = performance.now() - timestamp;
      timestamp = performance.now();
      observation.savedBytes = journal.restore();
      activeJournal = null;
      game.ph3StackRestore(stack);
      observation.restoreMs = performance.now() - timestamp;
    }
  }

  function resolve(frame, input) {
    const forecast = shown.get(frame);
    if (forecast?.some((value, seat) => value !== input[seat])) observation.corrections++;
    shown.delete(frame);
  }

  function close() {
    shown.clear();
  }

  return {guardImports, render, resolve, close, observation};
}
