// Setting.INI seed for the launcher configure message. Desktop-only keys stay
// at the browser defaults; volumes, UnlockFPS, optional key names, and font
// are merged into an existing file so Mode.Clear is kept.

export const DEFAULT_INI = "[DirectX]\r\nFullWindow=0\r\nAntiAlias=0\r\nVerticalSync=0\r\n[DirectInput]\r\nUseDirectInput=0\r\n[Mode]\r\nClear=no\r\nSingleThreaded=1\r\nFixedFPS=0\r\nUnlockFPS=0\r\nDelayTolerance=10\r\nWindowSize=0\r\nPresentInterval=1\r\n[Key]\r\nLeft=Left\r\nRight=Right\r\nUp=Up\r\nDown=Down\r\nSlow=ShiftKey\r\nShoot=Z\r\nBomb=X\r\n[JoyStick]\r\nSkip=0\r\nPause=0\r\nSlow=0\r\nShoot=0\r\nBomb=0\r\nC=0\r\n[Volume]\r\nBGMVolume=80\r\nSEVolume=80\r\n[Game]\r\nFontType=Arial\r\n";

const LAUNCHER_KEYS = { Left: "Left", Right: "Right", Up: "Up", Down: "Down", Slow: "ShiftKey", Shoot: "Z", Bomb: "X" };

function linesOf(text) {
  return String(text || "").replace(/\r\n/g, "\n").replace(/\r/g, "\n").split("\n");
}

function sectionAt(lines, index) {
  const text = lines[index].trim();
  if (text.startsWith("[") && text.endsWith("]")) return text.slice(1, -1);
  return "";
}

export function setIniValue(text, section, key, value) {
  const lines = linesOf(text);
  const header = "[" + section + "]";
  let sectionLine = -1;
  let keyLine = -1;
  let nextSection = lines.length;
  for (let i = 0; i < lines.length; i++) {
    const name = sectionAt(lines, i);
    if (!name) continue;
    if (sectionLine >= 0 && keyLine < 0) {
      nextSection = i;
      break;
    }
    if (name.toLowerCase() === section.toLowerCase()) sectionLine = i;
  }
  if (sectionLine >= 0) {
    const end = nextSection;
    for (let i = sectionLine + 1; i < end; i++) {
      const trimmed = lines[i].trim();
      if (trimmed.toLowerCase().startsWith(key.toLowerCase() + "=")) {
        keyLine = i;
        break;
      }
    }
  }
  const row = key + "=" + value;
  if (keyLine >= 0) lines[keyLine] = row;
  else if (sectionLine >= 0) lines.splice(sectionLine + 1, 0, row);
  else lines.push(header, row);
  return lines.filter((line, index) => line.length > 0 || index < lines.length - 1).join("\r\n").replace(/\r\n$/, "") + "\r\n";
}

function volume(value) {
  if (typeof value !== "number" || !Number.isFinite(value)) return null;
  return Math.max(0, Math.min(100, Math.round(value)));
}

function keyName(value) {
  const text = String(value || "");
  return /^[A-Za-z][A-Za-z0-9]{0,31}$/.test(text) ? text : "";
}

export function mergeSettingIni(existing, options, hosted = false) {
  const source = existing && String(existing).includes("[Mode]") ? String(existing) : DEFAULT_INI;
  const bag = options || {};
  let text = source;
  if (hosted) {
    // The launcher delivers remapped, canonical keys and owns frame pacing.
    text = setIniValue(text, "DirectInput", "UseDirectInput", "0");
    text = setIniValue(text, "Mode", "PresentInterval", "1");
  }
  const bgm = volume(bag.bgmVolume);
  const se = volume(bag.sfxVolume);
  if (bgm != null) text = setIniValue(text, "Volume", "BGMVolume", String(bgm));
  if (se != null) text = setIniValue(text, "Volume", "SEVolume", String(se));
  if (bag.limitPresentationTo60 === false) text = setIniValue(text, "Mode", "UnlockFPS", "1");
  else if (bag.limitPresentationTo60 === true) text = setIniValue(text, "Mode", "UnlockFPS", "0");
  const keys = hosted ? LAUNCHER_KEYS : bag.keys && typeof bag.keys === "object" ? bag.keys : null;
  if (keys) {
    for (const field of Object.keys(LAUNCHER_KEYS)) {
      const name = keyName(keys[field]);
      if (name) text = setIniValue(text, "Key", field, name);
    }
  }
  const font = typeof bag.fontType === "string" ? bag.fontType.trim() : "";
  if (font && font.length <= 32 && !/[\r\n=[\]]/.test(font)) {
    text = setIniValue(text, "Game", "FontType", font);
  }
  return text;
}

export function decodeIni(value) {
  if (typeof value !== "string" || !value) return "";
  const binary = atob(value);
  const bytes = new Uint8Array(binary.length);
  for (let i = 0; i < binary.length; i++) bytes[i] = binary.charCodeAt(i);
  return new TextDecoder().decode(bytes);
}

export async function seedSettingIni(runtime) {
  if (!runtime || typeof runtime.writeUserFile !== "function") return "";
  const existing = decodeIni(runtime.saves && runtime.saves["Setting.INI"]);
  const text = mergeSettingIni(existing, runtime.options || {}, runtime.captureKeys === false);
  await runtime.writeUserFile("Setting.INI", new TextEncoder().encode(text));
  return text;
}
