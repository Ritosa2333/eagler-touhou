// th19 keyboard map: JS KeyboardEvent.code → SDL3 scancode (SDL_Scancode values
// are identical between SDL2/SDL3). The C++ th19_key(code,down) indexes its
// injected-key table by SDL scancode.
export const scanCodes={
  "Escape":41,"Enter":40,"NumpadEnter":88,"Tab":43,"Backspace":42,"Space":44,
  "ArrowUp":82,"ArrowDown":81,"ArrowLeft":80,"ArrowRight":79,
  "KeyJ":13,"KeyK":14,"KeyL":15,"KeyZ":29,"KeyX":27,"KeyC":6,"KeyA":4,"KeyS":22,"KeyD":7,"KeyQ":20,"KeyW":26,"KeyE":8,"KeyR":21,
  "ShiftLeft":225,"ShiftRight":229,"ControlLeft":224,"ControlRight":228,"AltLeft":226,"AltRight":230,
  "Digit1":30,"Digit2":31,"Digit3":32,"Digit4":33,"Digit5":34,"Digit6":35,"Digit7":36,"Digit8":37,"Digit9":38,"Digit0":39,
  "Home":74,"End":77,"PageUp":75,"PageDown":78,"Insert":73,"Delete":76,
  "F1":58,"F2":59,"F3":60,"F4":61,"F5":62,"F6":63,"F7":64,"F8":65,"F9":66,"F10":67,"F11":68,"F12":69,
  "Pause":72,"PrintScreen":70,"ScrollLock":71,
};
