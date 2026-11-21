// RGL Web runtime -- 键盘 / 鼠标事件层（1c-3）
//
// 引擎用 Windows Virtual-Key 码（launch.lua:114 的 KEY 表）。浏览器给的是
// KeyboardEvent.code（物理键名，与布局无关）和 event.key。这里做一次映射，
// 之后全部投递到 WASM 内的 C 状态数组，Lua 侧热路径不再跨 JS 边界。
//
// 映射必须覆盖游戏默认键位（launch.lua:271）：
//   up/down/left/right = 0x26/0x28/0x25/0x27, slow = SHIFT(0xA0),
//   shoot = Z(0x5A), spell = X(0x58), special = C(0x43),
//   pass = SPACE(0x20), change = LCTRL(0xA2)
//   keysys.menu = ESCAPE(0x1B), snapshot = HOME(0x24)
// 以及设置界面「改键」时 GetLastKey() 要返回的 VK 值。

/** code → Windows VK。数组项 [vk, aliasVk?]，alias 用于引擎同时置通用码的键。 */
const CODE_TO_VK = {
  // 字母
  KeyA: 0x41, KeyB: 0x42, KeyC: 0x43, KeyD: 0x44, KeyE: 0x45,
  KeyF: 0x46, KeyG: 0x47, KeyH: 0x48, KeyI: 0x49, KeyJ: 0x4A,
  KeyK: 0x4B, KeyL: 0x4C, KeyM: 0x4D, KeyN: 0x4E, KeyO: 0x4F,
  KeyP: 0x50, KeyQ: 0x51, KeyR: 0x52, KeyS: 0x53, KeyT: 0x54,
  KeyU: 0x55, KeyV: 0x56, KeyW: 0x57, KeyX: 0x58, KeyY: 0x59,
  KeyZ: 0x5A,
  // 主键盘数字
  Digit0: 0x30, Digit1: 0x31, Digit2: 0x32, Digit3: 0x33, Digit4: 0x34,
  Digit5: 0x35, Digit6: 0x36, Digit7: 0x37, Digit8: 0x38, Digit9: 0x39,
  // 小键盘
  Numpad0: 0x60, Numpad1: 0x61, Numpad2: 0x62, Numpad3: 0x63, Numpad4: 0x64,
  Numpad5: 0x65, Numpad6: 0x66, Numpad7: 0x67, Numpad8: 0x68, Numpad9: 0x69,
  NumpadMultiply: 0x6A, NumpadAdd: 0x6B, NumpadSubtract: 0x6D,
  NumpadDecimal: 0x6E, NumpadDivide: 0x6F,
  // 方向 / 导航
  ArrowLeft: 0x25, ArrowUp: 0x26, ArrowRight: 0x27, ArrowDown: 0x28,
  PageUp: 0x21, PageDown: 0x22, End: 0x23, Home: 0x24,
  Insert: 0x2D, Delete: 0x2E,
  // 基本键
  Backspace: 0x08, Tab: 0x09, Enter: 0x0D, Space: 0x20, Escape: 0x1B,
  Pause: 0x13, CapsLock: 0x14, NumLock: 0x90, ScrollLock: 0x91,
  // 注意 NumpadEnter：引擎把它从 VK_RETURN(0x0D) 翻译成 VK_NUMPADENTER，
  // 游戏 KEY 表里 NUMPADENTER = 0xE8，GetLastKey 应返回 0xE8。
  NumpadEnter: 0xE8,
  // 功能键
  F1: 0x70, F2: 0x71, F3: 0x72, F4: 0x73, F5: 0x74, F6: 0x75,
  F7: 0x76, F8: 0x77, F9: 0x78, F10: 0x79, F11: 0x7A, F12: 0x7B,
  // OEM（引擎用 VK_OEM_*；游戏 KEY.GRAVE=0xC0、MINUS=0xBD）
  Backquote: 0xC0, Minus: 0xBD, Equal: 0xBB,
  BracketLeft: 0xDB, BracketRight: 0xDD, Backslash: 0xDC,
  Semicolon: 0xBA, Quote: 0xDE, Comma: 0xBC, Period: 0xBE, Slash: 0xBF,
  // 修饰键：直接给翻译后的左/右码；C 侧 apply_vk 会补上通用码 0x10/0x11/0x12
  ShiftLeft: 0xA0, ShiftRight: 0xA1,
  ControlLeft: 0xA2, ControlRight: 0xA3,
  AltLeft: 0xA4, AltRight: 0xA5,
  MetaLeft: 0x5B, MetaRight: 0x5C, ContextMenu: 0x5D,
};

/** 老浏览器没有 code 时用 key / keyCode 兜底。
 *
 * 1c-21：中文输入法激活时会向页面派发「合成噪音事件」——`code` 为空、
 * `key` 为 'Unidentified'、`keyCode` 是 16/17（VK_SHIFT/VK_CONTROL 通用码）
 * 甚至 229。这些事件没有对应的 keyup（IME 自己吞了），一旦进 held 就永久
 * 卡住：实测截图 held=[17,90,160,16,39] 里 16/17 就是这类噪音，映射表根本
 * 不会产物通用码。所以：
 *   ① code 与 key 都是空壳 → 返回 0，整条事件丢弃（不 add / 不投递）
 *   ② keyCode 兜底不再接受修饰键通用码与 229（左右码由 code 给出才可信）
 */
function vkFromEvent(e) {
  if (e.code && CODE_TO_VK[e.code] !== undefined) return CODE_TO_VK[e.code];
  const k = e.key;
  if (k && k !== 'Unidentified' && k !== 'Process') {
    if (k.length === 1) {
      const c = k.toUpperCase();
      if (c >= 'A' && c <= 'Z') return c.charCodeAt(0);
      if (c >= '0' && c <= '9') return c.charCodeAt(0);
      const punct = { '`': 0xC0, '-': 0xBD, '=': 0xBB, '[': 0xDB, ']': 0xDD,
        '\\': 0xDC, ';': 0xBA, "'": 0xDE, ',': 0xBC, '.': 0xBE, '/': 0xBF,
        ' ': 0x20 };
      if (punct[k] !== undefined) return punct[k];
    }
    const named = {
      ArrowLeft: 0x25, ArrowUp: 0x26, ArrowRight: 0x27, ArrowDown: 0x28,
      Backspace: 0x08, Tab: 0x09, Enter: 0x0D, Escape: 0x1B,
      Home: 0x24, End: 0x23, PageUp: 0x21, PageDown: 0x22,
      Insert: 0x2D, Delete: 0x2E, Shift: 0xA0, Control: 0xA2, Alt: 0xA4,
    };
    if (named[k] !== undefined) return named[k];
    if (/^F([1-9]|1[0-2])$/.test(k)) return 0x6F + parseInt(k.slice(1), 10);
  }
  /* keyCode 在 Windows 上恰好就是 VK 码，但只在事件有明确 code/key 时可信。
     排除 229（IME 合成）、0x10..0x12（修饰键通用码，无法分辨左右）、
     0xA0..0xA5（修饰键应由 code 判定）。 */
  const kc = e.keyCode;
  if (kc > 0 && kc < 256 && kc !== 229 && !(kc >= 0x10 && kc <= 0x12) &&
      !(kc >= 0xA0 && kc <= 0xA5)) return kc;
  return 0;
}

/** 1d-6：IME 接管期间的键盘事件形状。
 *
 *  这类事件没有可信的配对 keyup（输入法自己吞掉），所以它们**本身就是
 *  「键盘所有权正在交接」的确定性信号**，而不是需要猜时长的静默。
 *  1c-25 的硬结论是不存在的信号不能拿来兜底；这里相反，用的是真实到达
 *  的事件形状。一旦出现就整表清空，别让旧 held 里的方向键续命。
 */
const isImeEvent = (e) => e.isComposing === true ||
  e.key === 'Process' || e.key === 'Unidentified' || e.keyCode === 229 ||
  (!e.code && !e.key);

/** 焦点落在可编辑元素上 = 键盘输入已经不归游戏画面所有。 */
const isEditable = (n) => !!n && (n.tagName === 'INPUT' ||
  n.tagName === 'TEXTAREA' || n.tagName === 'SELECT' || n.isContentEditable);

/**
 * 把 DOM 鼠标坐标换算成画布内部坐标。
 * canvas 的 CSS 尺寸与 width/height 属性可能不同（响应式缩放）。
 *
 * 普通态：canvas 用 aspect-ratio:16/9 把自己框成 16:9，getBoundingClientRect()
 * 返回的就是真实显示区域，直接按元素尺寸换算即可。
 *
 * 全屏态：#rgl-stage:fullscreen canvas 用了 object-fit:contain，元素撑满整屏、
 * 游戏走「等比 letterbox 居中」—— 这时 getBoundingClientRect() 返回的是整块屏，
 * 而游戏只画在其中的居中子矩形里。若不做 letterbox 反算，clientX/Y 被当成整块
 * buffer 映射，越偏离中心错位越大（实测：全屏下指针与游戏内位置对不上）。
 *
 * 这里复刻 object-fit:contain：当元素宽高比与 buffer 不符时，向内收成居中的
 * 内容矩形再换算。元素宽高比 == buffer 宽高比时退化为恒等映射（普通态/同比例
 * 全屏都成立）。只有 object-fit 为 contain/scale-down 才启用（fill/cover 不在此列）。
 * y 保持「向下」，GetMousePosition 在 C 侧按引擎语义翻转。
 */
function canvasPoint(canvas, clientX, clientY) {
  const r = canvas.getBoundingClientRect();
  let left = r.left, top = r.top, w = r.width, h = r.height;
  let fit = 'fill';
  try { if (typeof getComputedStyle === 'function') fit = getComputedStyle(canvas).objectFit; } catch { /* 无头环境 */ }
  if (fit === 'contain' || fit === 'scale-down') {
    const bufAspect = canvas.width / canvas.height;   // buffer 宽高比（恒定）
    const elemAspect = w / h;                         // 元素显示宽高比（全屏=屏幕）
    if (elemAspect > bufAspect) {                     // 左右黑边（pillarbox）
      const cw = h * bufAspect;
      left += (w - cw) / 2;
      w = cw;
    } else if (elemAspect < bufAspect) {              // 上下黑边（letterbox）
      const ch = w / bufAspect;
      top += (h - ch) / 2;
      h = ch;
    }
  }
  const sx = w > 0 ? canvas.width / w : 1;
  const sy = h > 0 ? canvas.height / h : 1;
  return {
    x: (clientX - left) * sx,
    y: (clientY - top) * sy,
  };
}

/**
 * 挂载输入监听。返回可卸载 / 调试注入的句柄。
 * @param {object} opts { ccall, canvas, preventDefault }
 */
export function attachInput({ ccall, canvas, preventDefault = true }) {
  const key = (vk, down) => ccall('rgl_host_key', null, ['number', 'number'], [vk, down]);
  const char = (s) => ccall('rgl_host_char', null, ['string'], [s]);
  const mouse = (x, y, buttons, wheel) =>
    ccall('rgl_host_mouse', null, ['number', 'number', 'number', 'number'],
      [x, y, buttons, wheel]);
  const reset = () => ccall('rgl_host_input_reset', null, [], []);
  const setCanvas = () =>
    ccall('rgl_host_canvas', null, ['number', 'number'], [canvas.width, canvas.height]);

  /* ── 输入所有权模型（1c-22，对齐 eagler-touhou 的输入契约）───────────
   *
   * 参考 eagler-touhou 的两份结论（archive/th08-keyboard-ownership、
   * archive/th15-input-latency）：
   *
   *  ① **不缓冲输入**。DOM 事件直接写键盘表，逻辑帧内轮询读取；输入在
   *     「真正推进的那一帧」被采样一次，补帧不会重复采样一次性输入。
   *     我们已经是这个形状（事件即时写 g_key，begin_frame 只搬边沿）。
   *
   *  ② **所有权 + 接受/拒绝对称**。他们的原话更狠：「被映射层拒绝的 DOWN
   *     曾进入原始键盘路径，后续 UP 被映射层接收，原始路径因而一直保留按下
   *     状态」——卡键全部来自 DOWN 和 UP 走了**不同**的路径。我们把这条
   *     落成硬不变量：**任何被拒绝或丢弃的 DOWN，必须让该 vk 处于释放态**；
   *     释放永远执行，不受 enabled / 映射结果影响。
   *
   *  ③ **纪元交接**。失焦、改键、映射撤下时先释放旧路径留下的按键，再
   *     交接后续事件；失焦期间被浏览器排队的**孤儿 repeat** 不能凭空复活
   *     按键（他们的回归专门测「失焦后的旧重复事件」）。
   *
   *  ④ **不用时间猜松开（1c-25 定论）**。1c-22 到 1c-24 我都试图用「静默
   *     多久算松开」当 keyup 丢失的兜底，全部失败，原因是**前提根本不成立**：
   *     自动重复不是「按住就会有」的心跳。
   *
   *     1c-24 的取证日志（`web/build/input_log.jsonl`，真机 68 秒 160 次按下 /
   *     160 次松开）把三件事同时钉死了：
   *
   *       a) **keyup 一次都没丢**。每个 vk 的 d/u 数量完全相等，两次被淘汰的
   *          键随后都收到了真实 keyup（Z 迟到 2.3s、Shift 迟到 1.55s）—— 它们
   *          物理上一直是按住的，是**我们的判据把按键杀了**。
   *       b) **自动重复只属于「最后按下的那个键」**。Z 的 repeat 在 10867 停住，
   *          正好是 11044~11091 按下 ↑/Shift/← 的瞬间；把方向键松开后 Z 也
   *          不会恢复重复。所以同时按住「射击 + 方向」时，射击键必然静默 ——
   *          这正是「按住射击键一会就没了」。
   *       c) **字母键的重复还可能被输入法整个吃掉**。有一次 Z 按住 46 秒，
   *          `beats` 全程 0（方向键同时在正常重复）。
   *
   *     结论：静默在「按住但被别的键抢了重复」和「真的松手了」之间**零信息**，
   *     任何窗口都只能误杀。所以时间淘汰整段删除，释放只认三个来源：
   *     keyup、失焦/可见性/组合输入清空、修饰键的 getModifierState 硬权威。
   *     真出现幽灵（点过系统弹窗之类）就靠 `clearAll()` 的入口收场，HUD 上
   *     写着怎么触发。宁可留幽灵，也不误杀长按。
   */
  const held = new Map();
  /* 1d-26：**repeat 所有者** = 最后一次「真按下（非 repeat）」的那个键。
     操作系统的自动重复只属于它 —— 这条事实写在下面注释 ④ 里已经很久了，
     但 1d-24 的清理逻辑忘了用它，于是自己咬了自己（见 pruneGhosts）。 */
  let repOwner = 0;
  let clears = 0;            // 清空次数（失焦/可见性/组合输入/禁用）
  let lastClear = null;      // 最近一次清空的现场 {why, n}
  let orphanRepeats = 0;     // 没有配对 DOWN 的 repeat（失焦遗留/输入法噪音）
  let imeNoise = 0;          // IME 合成噪音事件计数（>0 说明输入法在干扰）
  let sawRepeat = false;     // 本会话是否见过 repeat（纯诊断，不参与任何判定）
  let repEvents = 0;         // 收到的 repeat keydown 总数（诊断用）

  /* ── 输入取证日志（1c-24）────────────────────────────────────────────
   * 「按住键会不会掉」只能真机复现，靠人转述永远差一层。这里把事件流攒成
   * JSONL：非重复的 down/up 逐条记，repeat 按每键 250ms 采样，另外每秒打
   * 一条心跳快照（每个 held 键的年龄 / 收到过几次 repeat / 最大重复间隔）。
   * 页面每秒 POST 一次到 serve.py 的 /__rgl_log，直接落盘
   * web/build/input_log.jsonl；发不出去（跑在别的静态服务上）就退回
   * localStorage，HUD 上有「导出输入日志」按钮。?inputlog=0 可关。
   */
  const LOG_MAX = 5000;
  const logBuf = [];
  let logSent = 0, logFailed = 0, logDropped = 0;
  const lastRepLog = new Map();   // vk -> 上次记 repeat 采样日志的时刻
  let lastHbAt = 0;
  let lastHbClears = 0;
  const LOG_ENABLED = (() => {
    try { return new URLSearchParams(location.search).get('inputlog') !== '0'; }
    catch { return true; }
  })();
  const logPush = (o) => {
    if (!LOG_ENABLED) return;
    o.t = Math.round(performance.now());
    logBuf.push(o);
    if (logBuf.length > LOG_MAX) { logBuf.shift(); logDropped++; }
    /* 1d-28：把排查价值高的事件（clr / ghost / orphan / stall / ime）同步
       转发到统一运行日志，玩家点「导出日志」就能拿到完整现场。诊断代码
       绝不许影响输入：整套调用包在 try 里，且 logger 不存在时直接短路。 */
    try { globalThis.__rglLogger?.ingest?.('input', o); } catch { /* ignore */ }
  };
  /* 会话头：UA / 分辨率 / 页面地址。真机日志必须能对上「是哪台机器、什么
     缩放、走的哪个端口」，否则读日志的人还得回头问。 */
  try {
    logPush({ k: 'sess', at: Date.now(),
      ua: String(navigator.userAgent || '').slice(0, 140),
      dpr: window.devicePixelRatio || 1,
      cw: canvas.width, ch: canvas.height,
      href: String(location.href).slice(0, 200) });
  } catch { /* node 单测里没有 navigator/location */ }

  const heldAdd = (vk, repeat) => {
    const t = performance.now();
    const cur = held.get(vk);
    if (cur) {
      const gap = t - cur.t;
      if (repeat) {
        cur.rep = 1;
        cur.beats++;
        const g = Math.round(gap);
        cur.lastGap = g;
        /* 1d-24：只统计**第 2 条起**的间隔。第一条是操作系统的「重复延迟」
           （Windows 默认 ~500ms），不是周期，计进去会把阈值撑到 2 秒。
           同时记录 max / min 两份：判定用 min（见 ghostLimit 的注释）。 */
        if (cur.beats >= 2) {
          if (g > cur.maxGap) cur.maxGap = g;
          if (!cur.minGap || g < cur.minGap) cur.minGap = g;
        }
      }
      cur.t = t;
    } else {
      held.set(vk, { t, rep: repeat ? 1 : 0, manual: false,
        beats: repeat ? 1 : 0, maxGap: 0, minGap: 0, lastGap: 0 });
    }
  };
  const heldKeys = () => [...held.keys()];
  /* 每个键活了多久（HUD / 日志用）：真按住时年龄随 repeat 归零，幽灵单调增长 */
  const heldAges = () => {
    const now = performance.now();
    return heldKeys().map((vk) => {
      const st = held.get(vk);
      return { vk, age: Math.round(now - st.t), rep: st.rep, manual: st.manual,
        beats: st.beats, maxGap: st.maxGap };
    });
  };

  /* 唯一的「释放」入口：所有路径（keyup / 拒绝 DOWN / 清空 / 纪元交接）
     都走这里，保证 DOWN 与 UP 的所有权一致。 */
  const releaseKey = (vk, why = 'up') => {
    const had = held.delete(vk);
    key(vk, 0);              // 幂等：C 侧本来就是 0 也无害
    if (repOwner === vk) repOwner = 0;   // 所有者走了，自动重复随之结束
    if (isMod(vk)) syncModifiers();
    if (had && why !== 'up') logPush({ k: 'rel', vk, why });
    return had;
  };

  /* ── 1d-24：幽灵方向键的**自动**清理 ───────────────────────────────
   *
   * 清酒反馈原话：「我已经松开了右键，但自机还是会持续往右移动，且伴随
   * 吞键」。这就是「丢 keyup」。它为什么反复出现、为什么以前修不掉，值得
   * 写清楚，因为下次再有人想改这里会踩回同一个坑：
   *
   * 输入链路是四层，**每一层都是状态对账、不靠事件**：
   *
   *     JS held  →  C g_key  →  KeyState  →  player.__*_flag
   *        ↑rgl_host_key_sync   ↑GetInput     ↑updateKeyState 的边沿
   *
   * 每层都会忠实地下传上一层。所以：**只要 held 里那个键还在，四层全都
   * 认为玩家还按着**，自机就一直走。反过来，held 一干净，下一帧四层
   * 一起归零，`__*_counting` 也被 `up` 事件重置 —— 整条链自己就恢复了。
   *
   * 也就是说：1d-16 那个 input_guard、1c-18 的对账、IME / 失焦 / F9 兜底，
   * **全都以 held 为权威**。held 脏了它们一起跟着脏。而 held 变脏只有一条
   * 路：**浏览器根本没派发 keyup**。在那种情况下，前端拿不到任何「方向键
   * 已松开」的独立信号（修饰键有 `getModifierState`，方向键没有等价物）。
   *
   * 以前唯一的出口是「玩家自己按 F9 或切一次窗口」。玩家当然不会那么做，
   * 于是幽灵一直留到下次按下同一个键 —— 这就是「反复出现好几次」的真相：
   * 不是修得不彻底，是**根本没有自动出口**。
   *
   * 1d-24 给出自动出口，依据是操作系统自己的 repeat 流：
   *
   *   一个键只要收到过 repeat，就证明「按住时会自动重复」。真按住时它
   *   必须持续到货；**repeat 流突然中断，只可能是 keyup 丢了**。
   *
   * 这条判据不会误杀长按：① 只有 `rep === 1`（证明过自己会重复）的键参与；
   * ② 阈值取该键**自己实测**的 repeat 周期（maxGap）的倍数，自适应不同
   * 键盘 / 系统设置；③ 真长按时每条 repeat 都刷新时间戳，永远够不到阈值。
   *
   * 唯一的代价是丢 keyup 后还会多走 ~300ms。用 300ms 换「永不卡死」。
   * 误判（其实还按着，却被清掉）由下面「孤儿 repeat 定向收养」接回来。
   *
   * ── 1d-26：上面这句「不会误杀」是错的，代价被低估了一个数量级 ────────
   *
   * 清酒反馈：「我明明在移动，按键也没松开，自机却停下来了，过了一会才能
   * 移动。」复现路径只有三步：
   *
   *     按住 → （方向键拿到 repeat，rep=1）→ 按一下 Z 射击 / Shift 低速 /
   *     再按另一个方向 ⇒ **repeat 所有权转移** ⇒ 方向键的 repeat 流静默
   *     ⇒ 220~800ms 后方向键被判丢 keyup 而释放 ⇒ 自机停住。
   *
   * 而「自动重复只属于最后按下的那个键」这条事实，本文档注释 ④（1c-24 的
   * 真机取证）早就钉死了，还附了证据：「Z 的 repeat 在 10867 停住，正好是
   * 11044~11091 按下 ↑/Shift/← 的瞬间」。1d-24 用静默做判据时把它绕过去了，
   * 于是把 1c-25 已经判过死刑的东西又复活了一遍。
   *
   * 1d-26 的修正（两条，缺一不可）：
   *
   *   ① **只有 repeat 所有者可以被静默判死**。所有权转移必须发生在新 keydown
   *      上 —— 所有者静默、且期间没有任何别的 keydown 到货，才说明「没有人
   *      抢走重复」，静默这时才真的等于 keyup 丢了。非所有者静默零信息。
   *   ② **只对方向键做静默判死**。字母键（Z 射击）的 repeat 会被输入法整个
   *      吃掉（同一份取证日志 c 项：Z 按住 46 秒 beats 全程 0），对它做静默
   *      判定等于随机掐断射击。方向键不会被输入法碰，是这个判据唯一安全的
   *      适用范围；而漂移这种「必须自动修」的症状也只会出现在方向键上。
   *
   * 剩下的死角（接受，不再为它加判据）：按住方向键 + 按住 Z，然后松开方向
   * 键而 keyup 丢失 —— 方向键此时不是所有者，不会被自动清。按一次方向键
   * 或碰一下窗口即可恢复。**宁可留这种窄边界，也不误杀长按**（1c-25 的结论
   * 在这里依然成立，只是适用范围要按所有权收窄）。
   *
   * ── 1d-67：这段判据整段默认退役。真机日志给出了决定性反证 ─────────────
   *
   * `rgl-log-2026-11-18T10-00-39-381Z.txt`（清酒在 127.0.0.1:8765 的后半局）
   * 三次自动清理，全部长这样：
   *
   *   ghost vk=40 age=223 limit=220 maxGap=33 beats=6 owner=1 raf=0
   *   ghost vk=39 age=221 limit=220 maxGap=45 beats=8 owner=1 raf=0
   *   ghost vk=37 age=222 limit=220 maxGap=33 beats=3 owner=1 raf=0
   *
   * 读法：`owner=1` = 它正是 OS 自动重复的唯一所有者，`beats/maxGap` 说明
   * 这条键此前一直在按时喂 repeat；`raf=0` = 这一拍**所有**键都没有 repeat
   * 到货（不是只有它静默）。也就是说：玩家仍物理按住 → 系统级重复整体暂停
   * → 没有 keyup、没有 blur、没有任何 clearAll（日志该时段无 clr）→ 我们
   * 凭「静默 220ms」把它释放了。释放后 `orphan=0 / clearAdoptions=0`：
   * 它再也没收到 repeat，收养通道一次都没被触发 ⇒ 玩家只能松开重按。
   * 这正是「按住左键走位，过一会自机不再往左」的完整因果链。
   *
   * 为什么修了好几轮都没修好：1c-25 用真机 68 秒取证已经判过「静默零信息」，
   * 1d-24 为治漂移又把它复活，1d-26 收窄到 owner+方向键，1d-27 加卡顿保护，
   * 1d-42 再放宽到「有 repeat 到货就清非 owner」。每一轮都在给同一个错误
   * 前提打补丁：把「repeat 没来」当成「玩家松手」的证据。它不是。
   *
   * eagler-touhou 的做法（ports/th08/web/sdl-runtime/directory-keyboard.mjs）：
   * 按 `code` 建 owner Map，只有真实 keyup 或生命周期边界（blur /
   * visibilitychange / pagehide / 映射交接）才清；并且明确拒绝无主 repeat
   * —— `if (event.repeat && !existing) return false`，绝不让旧 repeat 撤销
   * 生命周期清理。它**完全不用时间猜松手**。我们照这条落地。
   *
   * 取舍（必须写清）：关掉后，浏览器真的吞掉 keyup 且没有失焦信号时，幽灵
   * 方向键会留到玩家按 F9 / 点一下画面 / 切一次窗口。这是 1c-25 就选定的
   * 「宁可留窄边界幽灵，也不误杀长按」——漂移只会让自机多走一下，误杀则会
   * 让玩家在走位中途彻底失去控制且必须松手重按，后者严重得多。
   *
   * 判据代码保留、默认关闭：`ghostPrune = true` 可显式打开，历史探针
   * verify_1d24_ghost.js 靠它在 A/B 对照里复现旧行为，不做删表处理。
   */
  let ghostPruned = 0;
  let ghostEnabled = false;   // 1d-67：静默判死默认退役，只留诊断开关
  /* 阈值三件套。**基准取 minGap 而不是 maxGap** —— 这是实测踩出来的：
     系统 repeat 周期只会「抖动变大」（主线程卡顿、GC、渲染慢、setTimeout
     被推迟都会把某一两个间隔拉长），**从不变小**。用 maxGap 当基准时，只要
     有两次抖动（无头环境实测 maxGap 到过 247ms），阈值就被推到 1.2 秒，
     等于检测失效；而 minGap 始终贴着系统的真实重复周期。
     GHOST_MAX_MS 是硬天花板：一条持续 repeat 的键 800ms 内一条事件都没来，
     不可能是正常长按。万一真的误判，下一条 repeat 会被「定向收养」接回去。 */
  const GHOST_MIN_MS = 220;      // 绝对下限：真机 repeat 周期 ~33ms，6 条都没来才算丢
  const GHOST_MAX_MS = 800;      // 硬天花板：抖动再大也不放宽
  const GHOST_GAP_K = 3;         // 相对该键实测周期（minGap）的倍数
  const GHOST_GAP_ADD = 90;      // 固定余量
  /* 1d-26：允许被静默判死的键。方向键是唯一安全集合 —— 输入法不碰它们，
     而漂移这种必须自愈的症状也只在它们身上出现。 */
  const GHOST_SAFE = new Set([0x25, 0x26, 0x27, 0x28]);   // ← ↑ → ↓
  /* 1d-27：**主线程卡顿保护**。pruneGhosts 判的是「repeat 流静默」，可主线程
     一旦被贴图解码 / GC / 后台预热占住 200~400ms，键盘事件也一起堵在队列里 ——
     这时的静默**零信息**，却被当成 keyup 丢了 ⇒ 方向键被误判松开 ⇒ 自机停
     一下、下一条 repeat 才接回来。于是「卡一下」被放大成「走位顿一下」。
     两拍 syncHeld 间隔超过 STALL_GUARD_MS 就跳过本轮判定，并把所有 held 的
     时间戳刷新到现在（别把卡顿时长记成按键年龄）。1c-25 的结论在这里同样
     成立：宁可晚一点自愈幽灵，也绝不误杀长按。 */
  const STALL_GUARD_MS = 180;
  let lastSyncAt = 0, stallSkips = 0;
  /* 刚被清掉的键：给「孤儿 repeat 定向收养」用。只有出现在这里的 vk 才允许
     被一条没有配对 DOWN 的 repeat 重新接上 —— 队列残留的旧 repeat 不在表里。 */
  const prunedRecent = new Map();
  /* 1d-28：**clearAll 之后的自动恢复通道**。
     整表清空（blur / focus / IME / Alt / 无法映射的 keyup / 失焦对账…）会
     把玩家仍物理按住的键一起清掉。此后这个键只可能收到 repeat，而 repeat
     因为没有配对 DOWN 会被「孤儿判据」一律拒绝 ⇒ 必须松开重按才恢复。
     真机症状正是「按着上键走位，突然停住，那瞬间按什么键都没用」。

     ── 1d-29：这条通道的资格判据是错的，也是「只能往一个方向」的真因 ────
     旧代码只给 `st.rep >= 1`（证明过自己会 repeat）的键登记资格，TTL 400ms。
     但操作系统的自动重复**只属于最后按下的那个键**（1c-24 取证）。玩家
     交替按 ← / ↑ / Shift 时，先按下的 ← 一旦被后按的键夺走 repeat，就永远
     不会再收到 repeat ⇒ st.rep 停留在 0；这时只要来一次 clearAll（Shift
     触发输入法中英切换的 imeKeyUp、失焦、F9…），← 就**既没有资格、也没有
     repeat**，彻底失联。而 ↑ 是重复所有者，几十毫秒内就被接回 —— 表现正是
     「某刻只能往上」。另外 400ms 也短于 Windows 最长重复延迟（~1s）。

     新判据（三条，缺一不可）：
       ① **清空时 held 里的键全部登记**，不看 st.rep —— 仍然按住与否，
          和「以前有没有 repeat 过」无关；
       ② TTL 拉到 3s，覆盖最长的系统重复延迟；
       ③ **收养时把 repOwner 设成这个键**。一条 repeat 到货本身就证明
          「此刻是它在重复」，所有权必须跟着走：这样它既不会被「非所有者
          不判死」的规则保护成永久幽灵，也会在 repeat 停止后按常规被
          pruneGhosts 清掉（失焦积压的旧 repeat 最多骗一个周期就自愈）。

     仍然保留的红线：**没有任何 clearAll 记录、也没被 prune 过的纯孤儿
     repeat 继续拒绝**（verify_1d24_ghost 的 D1）—— 那是失焦积压的形状，
     不是「玩家还按着」。 */
  const CLEAR_ADOPT_TTL_MS = 3000;
  const clearRecent = new Map();
  let clearAdoptions = 0;
  /* 1d-42 task#5：本 rAF 内收到的 repeat 计数。pruneGhosts 只在「玩家确实在交互」
     （有 repeat 到货）时才放宽去清非 owner 方向键，避免误杀。每 rAF 在 syncHeld 重置。 */
  let repeatSeenRAF = 0;
  const ghostLimit = (st) => {
    const base = (st && (st.minGap || st.maxGap)) || 0;
    return Math.min(GHOST_MAX_MS,
      Math.max(GHOST_MIN_MS, base * GHOST_GAP_K + GHOST_GAP_ADD));
  };

  /* 每 rAF 调一次（在 syncHeld 里）。返回这次清掉的键数，方便诊断。 */
  const pruneGhosts = () => {
    if (!ghostEnabled || held.size === 0) return 0;
    const now = performance.now();
    let n = 0;
    for (const [vk, st] of held) {
      /* manual = 探针/脚本注入的按住，不该被时间判死（否则自动化测试里的
         「长按」会被莫名松开）。rep=0 = 还没证明过会重复，不参与判定。 */
      if (st.manual || !st.rep) continue;
      /* 1d-26 ②：只有方向键适用（字母键的 repeat 会被输入法吃掉）。 */
      if (!GHOST_SAFE.has(vk)) continue;
      /* 1d-42 task#5（根因2）：原本只有 repeat 所有者(vk===repOwner)才被静默判死。
         但「非所有者方向键」一旦 keyup 丢失（焦点切换 / 输入法吞 keyup）就会永久
         卡在 held 里、永不恢复 —— 正是「按着方向键移动、突然整组键失效 / 单向漂移」
         的来源。放宽：当本 rAF 确有 repeat 到货（repeatSeenRAF>0，证明玩家正在交互）
         时，任何老化超过 ghostLimit 的方向键都判死，无论是不是所有者。理由：玩家正在
         主动敲别的键却收不到这个键的 repeat ⇒ 它极大概率是丢失 keyup 的幽灵，而非仍
         被按住（仍被按住的键通常由 OS 持续喂 repeat，会一直是 owner 或刚被接回）。
         空闲（repeatSeenRAF==0）时仍只判所有者，避免把「交替按住 ←/↑」误杀。 */
      if (vk !== repOwner && repeatSeenRAF <= 0) continue;
      const age = now - st.t;
      const limit = ghostLimit(st);
      if (age <= limit) continue;
      prunedRecent.set(vk, now);
      logPush({ k: 'ghost', vk, age: Math.round(age), limit: Math.round(limit),
        maxGap: st.maxGap, beats: st.beats, owner: vk === repOwner ? 1 : 0,
        raf: repeatSeenRAF });
      releaseKey(vk, 'ghost');     // 内部 delete held + key(vk,0)，四层随之归零
      n++;
    }
    if (n) ghostPruned += n;
    /* 表不清会无限增长；1.5 秒足够覆盖「误杀 → 下一条 repeat 到达」的间隔。 */
    for (const [vk, t] of prunedRecent) {
      if (now - t > 1500) prunedRecent.delete(vk);
    }
    return n;
  };

  let mouseButtons = 0;
  let wheelPending = 0;
  let mouseX = 0, mouseY = 0;
  let mouseDirty = false;
  let enabled = true;
  let keyEvents = 0;
  /* 1d-23：「输入边沿」与「DOM 事件数」必须分开。
   *
   * shell.mjs 用 `edges` 决定要不要提前推进一个逻辑帧（1d-7 的输入延迟优化）。
   * 以前 edges 直接等于 keyEvents，而**浏览器的自动重复 keydown 也是
   * keydown** —— 玩家按住方向键走位时，Chrome 以 ~30Hz 连续发 repeat，
   * 于是「提前帧」被 repeat 反复触发：
   *
   *   · 逻辑帧间隔在 8.3ms / 16.7ms / 25ms 之间乱跳（原本严格 16.7ms）；
   *   · accum 长期为负，帧节奏完全由操作系统的键盘重复率驱动。
   *
   * 表现就是「自机走起来一顿一顿、不跟手」，而且帧率越高越明显。
   * repeat 不是玩家新做的动作，它不携带任何新信息，绝不能算边沿。
   * 只有「非 repeat 的 keydown」和「任意 keyup」才是真边沿。 */
  let edgeEvents = 0;
  const listeners = [];
  const timers = [];

  const on = (target, type, fn, opts) => {
    target.addEventListener(type, fn, opts);
    listeners.push([target, type, fn, opts]);
  };

  const flushMouse = () => {
    if (wheelPending === 0 && !mouseDirty) return;
    mouse(mouseX, mouseY, mouseButtons, wheelPending);
    wheelPending = 0;
    mouseDirty = false;
  };

  function syncCanvas() { setCanvas(); }

  /* C 侧 apply_vk 在处理 VK_LSHIFT/RSHIFT 时会顺带写通用码 0x10，但它
     看不到「另一个 Shift 还按着」：左右各按一个、松开其中一个就会把
     通用码误清成 0。这里用 JS 的 held 集合回写一次真实状态。 */
  const MOD_PAIRS = [
    [0x10, 0xA0, 0xA1],   // VK_SHIFT
    [0x11, 0xA2, 0xA3],   // VK_CONTROL
    [0x12, 0xA4, 0xA5],   // VK_MENU
  ];
  const isMod = (v) => MOD_PAIRS.some((p) => p[1] === v || p[2] === v);
  const syncModifiers = () => {
    for (const [generic, left, right] of MOD_PAIRS) {
      key(generic, (held.has(left) || held.has(right)) ? 1 : 0);
    }
  };

  /* 1c-20 修饰键硬权威：keyup 可能被整个吞掉（微软拼音把单独 Shift 的
     keyup 吃掉做中英切换是实锤场景），held 一旦脏了，1c-18 的对账只会
     忠实地把「卡住」同步进 C 侧 —— 对账救不了脏权威。
     但浏览器给了一条独立信道：**任何**键盘/鼠标事件的
     e.getModifierState('Shift'/'Control'/'Alt') 都反映此刻修饰键的真实
     物理状态，与事件是不是那个键发的无关。玩家玩 STG 时一直在按 Z/X
     射击、动鼠标 —— 只要任何一个输入事件到来，卡住的 Shift 最多一个
     事件内被纠正，不再依赖「松开时恰好有一个 keyup 活下来」。
     只做「held 有但物理没按」方向的清除：物理按着但 held 没有（无法
     分辨左右）时不凭空补，避免幽灵按住。
     1c-21 追加：通用码 0x10/0x11/0x12 也要一起清 —— IME 噪音事件曾把
     它们塞进 held（截图 held=[17,90,160,16,39]），只清左右码会留下 16/17。 */
  const MOD_CHECK = [
    ['Shift', 0xA0, 0xA1, 0x10],
    ['Control', 0xA2, 0xA3, 0x11],
    ['Alt', 0xA4, 0xA5, 0x12],
  ];
  const validateModifiers = (e) => {
    if (!e || !e.getModifierState) return;
    for (const [name, left, right, generic] of MOD_CHECK) {
      const physically = e.getModifierState(name);
      if (physically) continue;
      let removed = false;
      for (const vk of [left, right, generic]) {
        if (held.has(vk)) { held.delete(vk); key(vk, 0); removed = true; }
      }
      if (removed) syncModifiers();
    }
  };

  /* 捕获阶段监听：放在 window 的 capture 阶段，任何 DOM 覆盖层/输入框的
     stopPropagation 都拦不住 —— 这是「DOWN 与 UP 必须走同一条路」的前提。
     以前用冒泡，一旦某层 UI 吞了 keyup，键就永久卡住。 */
  on(window, 'keydown', (e) => {
    /* IME 合成期（isComposing 为真）的按键属于文字输入，不该驱动游戏；
       这类事件也没有配对的 keyup，进 held 就是永久卡键。
       顺带记一笔噪音计数：HUD 上能看到「输入法正在干扰」。 */
    if (e.isComposing || e.code === '' || e.key === 'Unidentified' ||
        e.key === 'Process' || e.keyCode === 229) {
      imeNoise++;
    }
    /* 1d-6：明确进入组合输入 = 键盘所有权交给 IME，此后的按住状态全不
       可信，且 IME 吞掉的 keyup 不会补发。按纪元交接整表清空，别让旧
       held 里的方向键续命成幽灵。注意**只对 isComposing 这么做**：
       key='Process'/keyCode=229 的 DOWN 在 IME 常驻时每一按都有，若也
       清空会让玩家在输入法下完全动不了；那类事件仍照常投递。 */
    if (e.isComposing) {
      if (held.size) clearAll('imeComposing');
      logPush({ k: 'ime', code: e.code, key: e.key, kc: e.keyCode });
      return;
    }
    validateModifiers(e);          // 卡键自愈不依赖本事件被正确映射
    const vk = vkFromEvent(e);
    /* ── 1d-29：「谁有焦点谁拿键盘」（task#2 / task#3）─────────────────
     * 焦点落在 INPUT / TEXTAREA / SELECT / contentEditable 上时，这段键盘
     * 属于**文字输入**，不是游戏操作：
     *
     *   ① 不投递给游戏 —— 否则在调试台输入框里敲数字会同时驱动自机；
     *   ② **不 preventDefault** —— 这是「只能输入、不能按退格」的真因：
     *      下面那段 `vk === 0x08` 是无条件拦掉的，输入框里的 Backspace
     *      （还有 Tab / 方向键 / 空格）全被游戏吃掉，浏览器删不掉字符。
     *
     * 有了这一条，调试台就**不需要**再 `input.enabled = false` 关总闸，
     * 面板开着也能正常玩（F8 之后没法操作游戏，也是这么修掉的）。
     * 被拒绝的 DOWN 依然必须让该 vk 处于释放态（1c-22 的硬不变量）。 */
    if (isEditable(e.target)) {
      if (vk) releaseKey(vk, 'typing');
      return;
    }
    if (!vk) {
      logPush({ k: 'nomap', code: e.code, key: e.key, kc: e.keyCode,
        rep: e.repeat ? 1 : 0 });
      return;
    }
    /* 手动自救键：真机万一还有幽灵方向键，按 F9 立刻整表清空。
       launch.lua:236 只定义了 KEY.F9 = 0x78，全项目没有一处使用，
       所以不占用游戏功能；也不会投递给游戏。 */
    if (vk === 0x78) { clearAll('manual'); return; }
    /* 1d-26：按住 Alt / Win(Cmd) 时，操作系统会把这些组合键的后续 keyup 全部
       截走（Alt+方向键在 Chrome 里是前进/后退，Win+方向键是窗口贴边），页面
       永远收不到 —— 这才是「幽灵方向键」最稳定的来源之一。
       这是**确定的上下文切换**，不是 1c-25 明令禁止的静默超时：按下修饰键的
       那一刻就把整表清空，不猜。AltGr 的签名是 Ctrl+Alt（欧洲键盘要打 @ 之类
       字符），不能误伤，所以排出掉 ctrlKey 那种形状。 */
    if (!e.repeat && (e.metaKey || (e.altKey && !e.ctrlKey))) {
      clearAll('altMeta');
      logPush({ k: 'altMeta', code: e.code, key: e.key, alt: 1 });
      return;
    }
    /* repeat 只做诊断，不参与任何判定（见注释 ④：重复只属于最后按下的那个
       键，字母键还可能被输入法整个吃掉，静默零信息）。 */
    if (e.repeat) { sawRepeat = true; repEvents++; repeatSeenRAF++; }
    else logPush({ k: 'd', vk, code: e.code, key: e.key, kc: e.keyCode });
    /* Ctrl+F5 / Ctrl+Shift+R 是浏览器「强制刷新」，必须真正生效。
       以前 F5 被统一 preventDefault，而游戏脚本又把 F5 绑成调试开关
       （显示鼠标坐标），于是按 Ctrl+F5 只是切了游戏内调试、根本没重新
       拉 WASM —— 重建之后还在跑旧模块。这里整件事交给浏览器，不投递
       给游戏、也不阻止默认行为。注意：拒绝交付 ⟹ 必须保证释放态。 */
    if ((e.ctrlKey || e.metaKey) &&
        (vk === 0x74 || (e.shiftKey && vk === 0x52))) { releaseKey(vk); return; }
    // 阻止方向键滚动页面、Tab 切焦点、空格滚屏、F 键触发浏览器功能
    if (preventDefault && (vk >= 0x21 && vk <= 0x28 || vk === 0x09 ||
        vk === 0x20 || vk === 0x08 || (vk >= 0x70 && vk <= 0x7B))) {
      e.preventDefault();
    }
    /* 接受/拒绝对称：输入被禁用时不交付 DOWN，但必须确保该 vk 处于释放态，
       否则「DOWN 被拒、UP 接受」就会留下永久按住（eagler-touhou 的原始
       卡键就是这个形状）。 */
    if (!enabled) { releaseKey(vk); return; }

    /* 孤儿 repeat：没有配对 DOWN 的自动重复。
       原本一律拒绝，理由是「失焦期间排队的旧 repeat 会凭空按住一个没人按的
       方向键」。那条理由仍然成立，但代价被低估了：pruneGhosts 判丢 keyup 时
       会释放按键，若玩家其实还按着，**接下来每一条 repeat 都会被拒绝** ⇒
       长按永久断掉，比幽灵更难接受（自机在走位中途突然停住）。
       1d-24 改成**定向收养**：只有刚被 prune 掉的键（prunedRecent 里有记录，
       1.5 秒内）才允许被一条无配对 DOWN 的 repeat 重新接上。队列残留的旧
       repeat 不在表里，仍然拒绝 —— 两头的行为都保住了。 */
    if (e.repeat && !held.has(vk)) {
      orphanRepeats++;
      const now = performance.now();
      const t = prunedRecent.get(vk);
      const tc = clearRecent.get(vk);
      let adopted = false;
      if (t && now - t < 1500) {
        prunedRecent.delete(vk);
        logPush({ k: 'orphan', vk, code: e.code, adopt: 1 });
        adopted = true;
      } else if (tc && now - tc < CLEAR_ADOPT_TTL_MS) {
        /* 1d-28/1d-29：clearAll 误清掉的长按键，靠这一条 repeat 立刻接回来。
           TTL 与登记判据见 clearRecent 的注释块。 */
        clearRecent.delete(vk);
        clearAdoptions++;
        logPush({ k: 'orphan', vk, code: e.code, adoptClear: 1 });
        adopted = true;
      } else {
        logPush({ k: 'orphan', vk, code: e.code });
        return;
      }
      /* 1d-29 ③：一条 repeat 到货 = 此刻它在重复 ⇒ 所有权转移给它。
         不这么做的话，被收养的键永远「不是所有者」，pruneGhosts 不会判死它，
         失焦积压的旧 repeat 就能把它变成永久幽灵。 */
      if (adopted) repOwner = vk;
    }

    /* 每次 keydown 都重投递按住状态（幂等）。以前 repeat 整段被跳过：首次
       DOWN 一旦被输入法或焦点切换吞掉，这个键就再也进不了按住状态。 */
    heldAdd(vk, !!e.repeat);
    key(vk, 1);
    /* 1d-26：记下 repeat 所有者。操作系统的自动重复只属于最后按下的那个键，
       之后的 repeat 都是它的；按下新键即转移（旧键的 repeat 永久停止，即使
       新键随后松开也不恢复 —— 1c-24 的取证日志实测）。pruneGhosts 只对这个
       键做静默判死。 */
    if (!e.repeat) repOwner = vk;
    if (isMod(vk)) syncModifiers();
    keyEvents++;
    /* 只有真实按键算边沿（repeat 是操作系统重复，不是玩家动作）。 */
    if (!e.repeat) edgeEvents++;
    /* repeat 采样（每键最多 250ms 一条）：真机上要能看出「这个键的自动重复
       到底来没来、间隔多大」。gap 是该键实测的两次 repeat 间隔。 */
    if (e.repeat) {
      const nowRep = performance.now();
      const prevLog = lastRepLog.get(vk) || 0;
      if (nowRep - prevLog >= 250) {
        lastRepLog.set(vk, nowRep);
        const st = held.get(vk);
        logPush({ k: 'rep', vk, gap: st.lastGap, beats: st.beats,
          maxGap: st.maxGap, code: e.code });
      }
    }
    // GetLastChar：只把新按下时的可打印单字符投递，自动重复不灌字符
    if (!e.repeat && e.key && e.key.length === 1) char(e.key);
  }, true);

  /* 1c-18 卡键对账：JS 的 held 集合是输入的唯一权威，每个 rAF 调一次。
     keyup 只要被任何一条路径吞掉（输入法吃掉 Shift 的 keyup、enabled 切换
     窗口期、异常焦点切换），C 侧 g_key 就卡在 1 —— 表现为「没按 Shift 却
     持续低速」「没按方向键却单向漂移」，直到用户再按一次同一个键。
     与其逐条堵路径，不如每帧把 held 传给 C 侧 rgl_input_sync_keys 对账，
     一个 rAF 内必然恢复。typical held 只有 0~6 个键，ccall 开销可忽略。 */
  /* 1d-6：键盘所有权的「状态查询」兜底。blur/focus/composition 事件在某些
     场景（系统弹窗、IME 候选窗、点进 file input 后焦点留在别处）不会可靠
     派发；窗口一旦失去焦点，浏览器不会补发 keyup，held 里的方向键就变成
     永久幽灵（verify_drift.js 实测松键后 30 帧仍滑 134px）。
     hasFocus() / activeElement 是**状态**，不是「静默多久算松开」的超时，
     所以不会误杀长按（1c-25 硬结论）。用边沿触发避免每帧 reset()。
     无头浏览器 document.hasFocus() 恒 false：只在 true→false 那一跳清一次，
     之后不再干预，探针仍能正常注入按键。 */
  let hadFocus = true, foreignFocus = false, loseFocusAt = -1;
  /* 1d-42 task#5（根因1）：原逻辑在「有焦点 → 失焦」那一跳就 clearAll，会整表
     清空玩家正按住的键。焦点一旦因 iframe 抢焦点 / 浏览器弹窗 / 文件选择框而
     *反复*闪动，每次跳变都清一次 ⇒ 表现就是「按着方向键移动、突然整组键失效」。
     改为：失焦必须**持续**超过阈值才算真失焦（容错 200ms 内的抖动，约 12 帧），
     闪动在阈值内自愈、不误清；真正的窗口切换失焦远超该阈值，照常清理。
     无头环境 hasFocus() 恒 false ⇒ 进入后约 200ms 清一次，之后 hadFocus=false
     不再重复清，探针靠 clearRecent 收养通道接回，行为不变。 */
  const NOFOCUS_CLEAR_MS = 200;
  const syncHeld = () => {
    repeatSeenRAF = 0; // 本 rAF 内若收到 repeat 由 keydown 累加，pruneGhosts 用它判活
    let focusNow = true;
    try {
      if (typeof document.hasFocus === 'function') focusNow = document.hasFocus();
    } catch { /* 老浏览器：当作有焦点 */ }
    const nowF = performance.now();
    if (!focusNow) {
      if (loseFocusAt < 0) loseFocusAt = nowF;
      else if (nowF - loseFocusAt >= NOFOCUS_CLEAR_MS && hadFocus) {
        clearAll('nofocus');
        hadFocus = false;
      }
    } else {
      loseFocusAt = -1;
      hadFocus = true;
    }
    const foreign = isEditable(document.activeElement);
    if (foreign && !foreignFocus) clearAll('foreignFocus');
    foreignFocus = foreign;

    /* 1d-24：先把「repeat 流已中断」的幽灵键清掉，再拿 held 去对账。
       顺序不能反 —— 先对账就等于把幽灵又忠实地下传给 C 侧一遍。
       1d-27：但主线程刚卡过就别判 —— 那不是「repeat 断了」，是事件被堵住。 */
    const now0 = performance.now();
    const gap = lastSyncAt > 0 ? now0 - lastSyncAt : 0;
    const stalled = gap > STALL_GUARD_MS;
    lastSyncAt = now0;
    if (stalled) {
      stallSkips++;
      for (const st of held.values()) st.t = now0;
      /* 1d-28：主线程卡顿是真机「走位一顿」的关键线索，必须留痕。 */
      logPush({ k: 'stall', gap: Math.round(gap), held: heldKeys() });
    } else {
      pruneGhosts();
    }

    const arr = heldKeys();
    ccall('rgl_host_key_sync', null, ['number', 'array'], [arr.length, arr]);
    /* 每秒一条心跳快照：此刻每个 held 键的存活年龄、收到过几次 repeat、
       两次 repeat 的最大间隔。真机日志里能直接看出「这个键到底有没有自动
       重复」，不用再靠猜。没有按住的键时不记（否则一分钟就是 60 条空行）。 */
    const now = performance.now();
    if (now - lastHbAt >= 1000) {
      lastHbAt = now;
      const hb = heldAges();
      if (hb.length || clears !== lastHbClears) {
        lastHbClears = clears;
        logPush({ k: 'hb', saw: sawRepeat ? 1 : 0, reps: repEvents,
          clears, held: hb });
      }
    }
  };

  /* 日志外送：每秒 POST 到 serve.py 的 /__rgl_log 落盘。失败（页面跑在别的
     静态服务上）就攒进 localStorage，HUD 的「导出输入日志」按钮能把它抠出来。 */
  /* 1d-28：CF 玩家端没有 /__rgl_log 端点。以前每秒仍会 POST 一次，CF 把
     未知路径软回退成 index.html(200) ⇒ 走 catch ⇒ 把每条事件反复写进
     localStorage 直到 400KB 截断。纯浪费（还可能在卡顿那一下再压一发 IO）。
     现在只在本地开发服务器上发；玩家端的事件已经由 logPush 同步转发到
     统一运行日志（环形缓冲 + 「导出日志」按钮），不需要第二条通道。 */
  const LOG_POST_HOST = (() => {
    try { return /^(localhost|127\.0\.0\.1|\[::1\])$/.test(location.hostname); }
    catch { return false; }
  })();
  const flushLog = () => {
    if (!logBuf.length || !LOG_POST_HOST) return;
    const batch = logBuf.splice(0, logBuf.length);
    const body = batch.map((o) => JSON.stringify(o)).join('\n') + '\n';
    fetch('__rgl_log', { method: 'POST', headers: { 'Content-Type': 'text/plain' }, body })
      .then((r) => {
        /* 只认 204：某些静态服务会把不存在的路径软回退成 index.html（200），
           那样 r.ok 也是 true，日志就静默丢了。 */
        if (r.status !== 204) throw new Error('http ' + r.status);
        logSent += batch.length;
      })
      .catch(() => {
        logFailed += batch.length;
        try {
          const prev = localStorage.getItem('rgl.inputlog') || '';
          localStorage.setItem('rgl.inputlog', (prev + body).slice(-400000));
        } catch { /* 隐私模式：丢掉，HUD 上看 logFailed */ }
      });
  };
  const logTimer = setInterval(flushLog, 1000);
  timers.push(() => clearInterval(logTimer));

  /* 释放永远执行，不受 enabled 影响 —— 「DOWN 被拒、UP 接受」是卡键的
     经典形状，这里从结构上消掉它。映射失败也先按修饰键状态校验一次。 */
  on(window, 'keyup', (e) => {
    validateModifiers(e);
    /* 1d-6：IME 形状 / 无法映射的 keyup 说明「用户松了某个键，但输入法
       把身份吃掉了」。这类事件拿不到 vk，无法精确释放；此时唯一安全的
       做法是整表清空 —— 用户本来就在松手，清空不会打断按住的操作，
       却能立刻断掉 held 里的幽灵方向键。以前这里只 return，于是丢一次
       keyup 就永久漂移（verify_drift.js 实测 30 帧滑 134px）。
       这是**事件驱动**，不是 1c-25 明令禁止的静默超时。 */
    /* 1e-XX（X 键卡住根因）：IME / 无法映射的 keyup 优先用 **e.code（物理键码，
       输入法不改它）** 精确释放那一个键，再走整表清空且**不收养**。

       旧逻辑无条件 clearAll('imeKeyUp')（收养所有 held），于是中文输入法驻留时
       按 X：keydown 干净进了 held，keyup 被吞成 Process/229 → clearAll 把 X 记进
       clearRecent → 紧随的残留 repeat 命中收养通道把 X 接回 → X 是字母键、不在
       GHOST_SAFE 且 ghostPrune 默认关 → 永不自愈。真机复现：DOWN=true →
       IME-KEYUP=false → ORPHAN-REPEAT=true → +600ms 仍 true。表现即用户所见
       「按一次 X 后按键显示里 X 一直亮」。

       修法两步（缺一不可）：① 能从 code 定位到 vk 就 releaseKey(vk) —— 该键此刻
       已明确松开，必须处于释放态；② clearAll 用 noAdopt=true —— 这次清表身份
       不可信，不能被残留 repeat 复活任何键。方向键走位自愈（1d-28/29）不受影响：
       那条路径由 blur/focus/imeComposing 触发，仍照常收养。 */
    const codeVk = (e.code && CODE_TO_VK[e.code] !== undefined) ? CODE_TO_VK[e.code] : 0;
    if (isImeEvent(e)) {
      imeNoise++;
      if (codeVk) releaseKey(codeVk, 'imeKeyUp');
      clearAll('imeKeyUp', true);
      logPush({ k: 'imeUp', code: e.code, key: e.key, kc: e.keyCode,
        exactVk: codeVk || undefined });
      return;
    }
    const vk = vkFromEvent(e);
    if (!vk) {
      logPush({ k: 'unomap', code: e.code, key: e.key, kc: e.keyCode });
      clearAll('unomapKeyUp', true);
      return;
    }
    logPush({ k: 'u', vk, code: e.code });
    releaseKey(vk);
    keyEvents++;
    edgeEvents++;
  }, true);

  /* 方向键没有 getModifierState 等价物，但 pointer 事件同样带修饰键
     状态 —— 玩家移动鼠标/点击时也会触发修饰键校验。 */
  const pointerValidate = (e) => { if (enabled) validateModifiers(e); };

  on(canvas, 'pointermove', (e) => {
    if (!enabled) return;
    pointerValidate(e);
    const p = canvasPoint(canvas, e.clientX, e.clientY);
    mouseX = p.x; mouseY = p.y;
    mouseButtons = e.buttons & 31;
    mouseDirty = true;
    flushMouse();
  });

  on(canvas, 'pointerdown', (e) => {
    if (!enabled) return;
    pointerValidate(e);
    const p = canvasPoint(canvas, e.clientX, e.clientY);
    mouseX = p.x; mouseY = p.y;
    mouseButtons = (e.buttons & 31) | (1 << e.button);
    mouseDirty = true;
    flushMouse();
    if (canvas.setPointerCapture) { try { canvas.setPointerCapture(e.pointerId); } catch { } }
  });

  on(canvas, 'pointerup', (e) => {
    if (!enabled) return;
    const p = canvasPoint(canvas, e.clientX, e.clientY);
    mouseX = p.x; mouseY = p.y;
    mouseButtons = e.buttons & 31;
    mouseDirty = true;
    flushMouse();
  });

  on(canvas, 'wheel', (e) => {
    if (!enabled) return;
    if (preventDefault) e.preventDefault();
    /* 引擎 GetMouseWheelDelta 返回 WHEEL_DELTA(120) 的倍数，符号遵循
       Win32 WM_MOUSEWHEEL：**向前滚（远离用户）= 正**、向后滚 = 负。
       浏览器的 deltaY 正好相反：向下滚 deltaY > 0。所以这里必须取反——
       以前写成 d > 0 ? 120 : -120，滚轮方向与原生整体反着走
       （角色列表/图鉴/成就列表都是反向滚，看着像「滚轮回退」）。 */
    const d = e.deltaMode === 1 ? e.deltaY * 40 : e.deltaY;  // 行→像素
    wheelPending += d === 0 ? 0 : (d > 0 ? -120 : 120);
    mouseDirty = true;
    flushMouse();
  }, { passive: false });

  on(canvas, 'contextmenu', (e) => { if (preventDefault) e.preventDefault(); });

  const clearAll = (why = 'blur', noAdopt = false) => {
    const n = held.size;
    const now = performance.now();
    /* 1d-28/1d-29：清空前把**所有仍按着的键**记进收养表。清空后它们只可能
       收到 repeat，没有这条通道就要松开重按才能恢复（真机「走位突然停住、
       按什么键都没用」）。1d-29 去掉了「st.rep 才登记」的限制 —— 交替按键
       时非最后按下的键根本收不到 repeat，正是「只能往一个方向」的来源。
       manual 是探针注入的按住，不收养。

       1e-XX（X 键卡住）：noAdopt=true 用于「某个键已明确松开、但事件被
       输入法吞成 IME 形状」的 clearAll。此刻 held 里的键身份不可信，若照常
       登记收养，紧随其后的残留 repeat 会把刚松开的键（如 X=spell）接回；
       字母键不属 GHOST_SAFE、ghostPrune 默认关，接回后永不自愈 —— 现象就是
       「按一次 X 后按键显示里 X 一直亮」。这种 clearAll 一律不收养：玩家若
       真还按着别的键，输入法退出后会有新 DOWN 重新建立。 */
    const adopt = [];
    /* 顺手清掉过期条目：clearAll 一次最多写几个键，但一局能触发几十次，
       不回收的话这张表会跟着会话无限长（虽然判据本身不会复活幽灵）。 */
    for (const [vk, t] of clearRecent) if (now - t >= CLEAR_ADOPT_TTL_MS) clearRecent.delete(vk);
    if (!noAdopt) {
      for (const [vk, st] of held) {
        if (!st.manual) { clearRecent.set(vk, now); adopt.push(vk); }
      }
    }
    held.clear();
    mouseButtons = 0;
    wheelPending = 0;
    reset();                   /* 纪元交接：把 C 侧键表整体清零，再让后续事件重新建立 */
    clears++;
    lastClear = { why, n, adopt };
    logPush({ k: 'clr', why, n, adopt });
  };
  /* 失焦 = 键盘所有权交出（系统弹窗、alt-tab、点地址栏、输入法候选窗）。
     这期间 keyup 不会补发，所以必须整表清空 —— 这是现在唯一的「清幽灵」
     通道，也是为什么可以放心删掉时间淘汰。 */
  on(window, 'blur', () => clearAll('blur'));
  on(window, 'focus', () => clearAll('focus'));
  on(window, 'pagehide', () => clearAll('pagehide'));
  on(document, 'visibilitychange', () => { if (document.hidden) clearAll('hidden'); });

  /* ── 输入法治理（1c-23）────────────────────────────────────────────
   * 中文输入法（哪怕切到它的英文模式）驻在前台时，Chrome 会把按键先交给
   * IME，代价是：keydown 的 code 变空壳 / keyCode=229、**自动重复整个
   * 消失**、keyup 被吞。这三件事分别对应前几轮的「幽灵按键」「按住不动
   * 却被误判松开」「松手后还滑一段」。
   *
   * 为什么 IME 会驻留：游戏自己的「输入昵称」界面（字母表）用方向键选字，
   * 但玩家习惯先用输入法打中文，IME 上下文一旦建立就会一直跟着这个标签。
   * 游戏页面不需要 IME，所以把焦点落到 canvas（非可编辑元素 → IME 不再
   * 接管文本输入）。
   *
   * 注意：canvas.focus() 只触发元素自己的 focus（不冒泡），不会撞上上面
   * 那个 window 'focus' → clearAll。
   */
  try { if (!canvas.hasAttribute('tabindex')) canvas.tabIndex = -1; } catch { /* 老浏览器 */ }
  const focusCanvas = () => {
    try { canvas.focus({ preventScroll: true }); }
    catch { try { canvas.focus(); } catch { /* ignore */ } }
  };
  on(canvas, 'pointerdown', focusCanvas);
  /* 组合开始 = 键盘所有权交给输入法。这期间浏览器不会发 repeat，keyup 也
     可能被吞，此后的按住状态全都不可信 —— 按「纪元交接」原则整表清空。
     之后 focusCanvas 把所有权抢回来。注意不 blur 可编辑元素：万一以后有真正
     的文本输入（昵称/存档名），blur 会把玩家正在打的字扔掉。 */
  on(document, 'compositionstart', () => { imeNoise++; clearAll('comp'); });
  on(document, 'compositionend', () => { clearAll('compend'); focusCanvas(); });
  /* index.html 里的「导入资源包 / 导入存档」是 file input：选完文件焦点
     会留在它身上，IME 就此常驻。用完立刻把焦点还给 canvas。 */
  on(document, 'focusin', (e) => {
    const t = e.target;
    if (isEditable(t)) t.addEventListener('change', focusCanvas, { once: true });
  });
  /* 开局就把焦点放到 canvas 上：玩家不需要先点一下画面，IME 从第一帧起
     就没有可编辑目标可接管。用 queueMicrotask 让 shell 把首帧画完再说。 */
  queueMicrotask(focusCanvas);

  syncCanvas();

  return {
    /** 调试/自动化：直接注入一次按键（down + up）或按住状态。
        会同步维护 held —— 对账后「不经过 held 的按住」会被当成卡键清掉。 */
    injectKey(vk, down = true) {
      if (down) {
        heldAdd(vk, false);
        held.get(vk).manual = true;   // 探针注入：标记来源，日志里区分
        key(vk, 1);
      } else releaseKey(vk);
      if (isMod(vk)) syncModifiers();
      keyEvents++;
      edgeEvents++;
      return true;
    },
    /** 调试：注入一次「按下并松开」，配合 rgl_input_begin_frame 使用 */
    tap(vk) { this.injectKey(vk, true); this.injectKey(vk, false); return true; },
    injectMouse(x, y, buttons = 0, wheel = 0) { mouse(x, y, buttons, wheel); },
    syncCanvas,
    syncHeld,
    clearAll,
    /** 取出完整日志文本（已落 localStorage 的尾部 + 还没发出去的）。
        HUD 的「导出输入日志」按钮和探针都用它。 */
    exportLog() {
      let tail = '';
      try { tail = localStorage.getItem('rgl.inputlog') || ''; } catch { /* 隐私模式 */ }
      const pending = logBuf.map((o) => JSON.stringify(o)).join('\n');
      return tail + (pending ? pending + '\n' : '');
    },
    /** 立刻发一次（探针/导出前调用，避免等 1s 定时器） */
    flushLog,
    detach() {
      for (const [t, ty, fn, o] of listeners) t.removeEventListener(ty, fn, o);
      listeners.length = 0;
      for (const fn of timers) { try { fn(); } catch { /* ignore */ } }
      timers.length = 0;
    },
    get diag() {
      return { enabled, keyEvents, edges: edgeEvents, held: heldKeys(),
        heldAges: heldAges(), repOwner,
        pruned: ghostPruned, clears, lastClear, orphan: orphanRepeats, imeNoise,
        clearAdoptions, stallSkips, stallGuardMs: STALL_GUARD_MS,
        /* 1d-67：日志快照必须能一眼看出静默判死是开是关。真机导出里若
           pruned>0 且 ghostPrune=false，说明是历史实例或探针显式打开过。 */
        ghostPrune: ghostEnabled,
        sawRepeat, repEvents,
        log: { on: LOG_ENABLED, sent: logSent, failed: logFailed,
          pending: logBuf.length, dropped: logDropped },
        mouseX, mouseY, mouseButtons,
        wheelPending, canvas: [canvas.width, canvas.height] };
    },
    /** 真实输入边沿数（非 repeat 的 keydown + 全部 keyup）。宿主用它判断
        「这一拍来了新输入」以决定是否提前推进一个逻辑帧；**自动重复不算**，
        否则按住方向键时帧节奏会被操作系统的重复率带跑（1d-23）。 */
    get edges() { return edgeEvents; },
    set enabled(v) { enabled = !!v; if (!enabled) clearAll('disable'); },
    get enabled() { return enabled; },
    /** 幽灵键静默判死总开关（1d-24 引入，1d-67 默认关闭）。
        开 = 旧行为（repeat 静默 220ms 就释放，可能误杀长按）；
        关 = 现行默认（只认 keyup / 失焦 / 组合输入 / 修饰键硬权威）。
        verify_1d24_ghost.js 会显式打开做 A/B，所以这个开关必须保留。 */
    set ghostPrune(v) {
      ghostEnabled = !!v;
      if (!ghostEnabled) prunedRecent.clear();
    },
    get ghostPrune() { return ghostEnabled; },
    /** 当前每个 held 键算出来的清理阈值（毫秒），排查「为什么没清 / 清早了」用。
        1d-26 起多带两个字段：`owner`（是不是 repeat 所有者，只有它能被静默
        判死）、`safe`（是不是方向键，只有它对输入法免疫）—— 「为什么这个键
        一直没被清」要能一眼答。 */
    ghostLimits() {
      const now = performance.now();
      return heldKeys().map((vk) => {
        const st = held.get(vk);
        return { vk, age: Math.round(now - st.t), rep: st.rep,
          manual: st.manual, beats: st.beats, minGap: st.minGap,
          maxGap: st.maxGap, limit: Math.round(ghostLimit(st)),
          owner: vk === repOwner ? 1 : 0, safe: GHOST_SAFE.has(vk) ? 1 : 0 };
      });
    },
  };
}
