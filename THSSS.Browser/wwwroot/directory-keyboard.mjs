const codes = {
  ArrowLeft: "Left",
  ArrowRight: "Right",
  ArrowUp: "Up",
  ArrowDown: "Down",
  ShiftLeft: "ShiftKey",
  ShiftRight: "ShiftKey",
  ControlLeft: "ControlKey",
  ControlRight: "ControlKey",
  AltLeft: "Menu",
  AltRight: "Menu",
  Escape: "Escape",
  Backspace: "Back",
  F1: "F1",
  F2: "F2",
  F3: "F3",
  Enter: "Return",
  NumpadEnter: "Return",
  Space: "Space",
  Minus: "OemMinus",
  Equal: "Oemplus",
  F8: "F8",
  KeyA: "A",
  KeyB: "B",
  KeyC: "C",
  KeyD: "D",
  KeyE: "E",
  KeyF: "F",
  KeyG: "G",
  KeyH: "H",
  KeyI: "I",
  KeyK: "K",
  KeyL: "L",
  KeyM: "M",
  KeyN: "N",
  KeyO: "O",
  KeyP: "P",
  KeyQ: "Q",
  KeyR: "R",
  KeyS: "S",
  KeyU: "U",
  KeyV: "V",
  KeyW: "W",
  KeyX: "X",
  KeyZ: "Z",
  Digit0: "D0",
  Digit1: "D1",
  Digit2: "D2",
  Digit3: "D3",
  Digit4: "D4",
  Digit5: "D5",
  Digit6: "D6",
  Digit7: "D7",
  Digit8: "D8",
  Digit9: "D9",
};

export function keyNameFromEvent(message) {
  if (message && codes[message.code]) return codes[message.code];
  const key = String(message && message.key || "");
  if (key === "Shift") return "ShiftKey";
  if (key === "Control") return "ControlKey";
  if (key === "Alt") return "Menu";
  if (key === "Escape") return "Escape";
  if (key === "Backspace") return "Back";
  if (key === "F1" || key === "F2" || key === "F3") return key;
  if (key === "Enter") return "Return";
  if (key === " ") return "Space";
  if (key === "ArrowLeft") return "Left";
  if (key === "ArrowRight") return "Right";
  if (key === "ArrowUp") return "Up";
  if (key === "ArrowDown") return "Down";
  if (key === "-" || key === "_") return "OemMinus";
  if (key === "=" || key === "+") return "Oemplus";
  if (/^[a-zA-Z]$/.test(key)) return key.toUpperCase();
  if (/^[0-9]$/.test(key)) return "D" + key;
  return "";
}
