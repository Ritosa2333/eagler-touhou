// Translate the existing TH03 controls to js-dos/GLFW. No player bindings,
// rapid-fire or charge behavior is changed here.
const named = {
  Space:32, Escape:256, Enter:257, Tab:258, Backspace:259,
  Insert:260, Delete:261, ArrowRight:262, ArrowLeft:263, ArrowDown:264, ArrowUp:265,
  PageUp:266, PageDown:267, Home:268, End:269, CapsLock:280, ScrollLock:281, NumLock:282,
  PrintScreen:283, Pause:284, NumpadDecimal:330, NumpadDivide:331,
  NumpadMultiply:332, NumpadSubtract:333, NumpadAdd:334, NumpadEnter:335, NumpadEqual:336,
  ShiftLeft:340, ControlLeft:341, AltLeft:342, MetaLeft:343,
  ShiftRight:344, ControlRight:345, AltRight:346, MetaRight:347, ContextMenu:348,
  Quote:39, Comma:44, Minus:45, Period:46, Slash:47, Semicolon:59, Equal:61,
  BracketLeft:91, Backslash:92, BracketRight:93, Backquote:96,
};
export function dosboxKeyCode(code) {
  if (Object.hasOwn(named, code)) return named[code];
  if (/^Key[A-Z]$/.test(code)) return code.charCodeAt(3);
  if (/^Digit[0-9]$/.test(code)) return code.charCodeAt(5);
  if (/^Numpad[0-9]$/.test(code)) return 320 + Number(code.slice(6));
  if (/^F(?:[1-9]|1[0-9]|2[0-5])$/.test(code)) return 289 + Number(code.slice(1));
  return undefined;
}
