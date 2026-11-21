// BRO 专属：把 launcher 的网页键盘改键（KeyboardEvent.code 别名）翻译成
// 游戏本体 setting.keys / keysys 需要的 Windows VK 数字。
//
// 为什么放在这里：launcher 自己的改键走 bridge 注入，BRO runtime 用自己的
// CODE_TO_VK 直接监听 DOM 事件，两套互相不认识。网页设置是权威，所以这里在
// configure 阶段把「网页当前生效的别名」折算成 BRO 游戏键位字段，经
// eagler_adapter → BRO_web_apply_settings 落到 setting.keys[name]。
//
// 语义：aliases[物理 code] = 游戏逻辑 code。BRO 游戏逻辑键就是默认 VK 对应的
// code（ArrowUp/KeyZ...）。取「映射到目标逻辑键的那个物理键」作为新的绑定；
// 未改键时回落 BRO 出厂默认值。keysys.repslow / snapshot 在网页改键里没有对应
// 动作，保持游戏默认，不写入。

const CODE_TO_VK: Readonly<Record<string, number>> = Object.freeze({
  KeyA: 0x41, KeyB: 0x42, KeyC: 0x43, KeyD: 0x44, KeyE: 0x45, KeyF: 0x46,
  KeyG: 0x47, KeyH: 0x48, KeyI: 0x49, KeyJ: 0x4A, KeyK: 0x4B, KeyL: 0x4C,
  KeyM: 0x4D, KeyN: 0x4E, KeyO: 0x4F, KeyP: 0x50, KeyQ: 0x51, KeyR: 0x52,
  KeyS: 0x53, KeyT: 0x54, KeyU: 0x55, KeyV: 0x56, KeyW: 0x57, KeyX: 0x58,
  KeyY: 0x59, KeyZ: 0x5A,
  Digit0: 0x30, Digit1: 0x31, Digit2: 0x32, Digit3: 0x33, Digit4: 0x34,
  Digit5: 0x35, Digit6: 0x36, Digit7: 0x37, Digit8: 0x38, Digit9: 0x39,
  Numpad0: 0x60, Numpad1: 0x61, Numpad2: 0x62, Numpad3: 0x63, Numpad4: 0x64,
  Numpad5: 0x65, Numpad6: 0x66, Numpad7: 0x67, Numpad8: 0x68, Numpad9: 0x69,
  NumpadMultiply: 0x6A, NumpadAdd: 0x6B, NumpadSubtract: 0x6D,
  NumpadDecimal: 0x6E, NumpadDivide: 0x6F,
  ArrowLeft: 0x25, ArrowUp: 0x26, ArrowRight: 0x27, ArrowDown: 0x28,
  PageUp: 0x21, PageDown: 0x22, End: 0x23, Home: 0x24,
  Insert: 0x2D, Delete: 0x2E,
  Backspace: 0x08, Tab: 0x09, Enter: 0x0D, Space: 0x20, Escape: 0x1B,
  Pause: 0x13, CapsLock: 0x14, NumLock: 0x90, ScrollLock: 0x91,
  NumpadEnter: 0xE8,
  F1: 0x70, F2: 0x71, F3: 0x72, F4: 0x73, F5: 0x74, F6: 0x75,
  F7: 0x76, F8: 0x77, F9: 0x78, F10: 0x79, F11: 0x7A, F12: 0x7B,
  Backquote: 0xC0, Minus: 0xBD, Equal: 0xBB,
  BracketLeft: 0xDB, BracketRight: 0xDD, Backslash: 0xDC,
  Semicolon: 0xBA, Quote: 0xDE, Comma: 0xBC, Period: 0xBE, Slash: 0xBF,
  ShiftLeft: 0xA0, ShiftRight: 0xA1,
  ControlLeft: 0xA2, ControlRight: 0xA3,
  AltLeft: 0xA4, AltRight: 0xA5,
});

/** BRO 出厂键位（launch.lua default_setting）。 */
export const BRO_DEFAULT_KEY_BINDINGS = Object.freeze({
  keys: Object.freeze({ up: 0x26, down: 0x28, left: 0x25, right: 0x27,
    slow: 0xA0, shoot: 0x5A, spell: 0x58, special: 0x43 }),
  keysys: Object.freeze({ repfast: 0xA2, menu: 0x1B, retry: 0x52 }),
});

/** 每个 BRO 动作对应的「网页逻辑目标码」，与 keyboardActionCodes 一致。 */
const ACTION_TARGETS: Readonly<Record<string, string>> = Object.freeze({
  'keys.up': 'ArrowUp', 'keys.down': 'ArrowDown',
  'keys.left': 'ArrowLeft', 'keys.right': 'ArrowRight',
  'keys.slow': 'ShiftLeft', 'keys.shoot': 'KeyZ',
  'keys.spell': 'KeyX', 'keys.special': 'KeyC',
  'keysys.repfast': 'ControlLeft', 'keysys.menu': 'Escape',
  'keysys.retry': 'KeyR',
});

export interface BroKeyBindings {
  keys: Record<string, number>;
  keysys: Record<string, number>;
}

/**
 * 由网页当前别名表折算 BRO 键位。返回完整可下发对象（含未改键的默认值），
 * 便于 configure 每次都把网页状态完整覆盖到游戏本体。
 */
export function broKeyBindingsFromAliases(aliases: Readonly<Record<string, string>> | null | undefined): BroKeyBindings {
  const keys: Record<string, number> = {...BRO_DEFAULT_KEY_BINDINGS.keys};
  const keysys: Record<string, number> = {...BRO_DEFAULT_KEY_BINDINGS.keysys};
  const source = aliases && typeof aliases === 'object' ? aliases : {};
  for (const [path, target] of Object.entries(ACTION_TARGETS)) {
    const [group, name] = path.split('.') as ['keys' | 'keysys', string];
    // 保留 Object.keys 的稳定顺序：多个物理键映射到同一逻辑键时取第一个。
    const physical = Object.keys(source).find((code) => source[code] === target);
    if (!physical) continue;                       // 未改键 → 用 BRO 默认
    const vk = CODE_TO_VK[physical];
    if (vk) (group === 'keys' ? keys : keysys)[name] = vk;
  }
  return {keys, keysys};
}
