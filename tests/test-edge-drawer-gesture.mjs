import assert from "node:assert/strict";
import { createEdgeDrawerGesture } from "../.cache/build/browser/assets/launcher/edge-drawer-gesture.mjs";

class FakeClassList {
  values = new Set();
  contains(name) { return this.values.has(name); }
}

class FakeSurface {
  constructor() { this.listeners = new Map(); }
  body = { classList: new FakeClassList() };
  documentElement = { clientWidth: 400 };
  addEventListener(type, callback) {
    const callbacks = this.listeners.get(type) || [];
    callbacks.push(callback);
    this.listeners.set(type, callbacks);
  }
  removeEventListener(type, callback) {
    this.listeners.set(type, (this.listeners.get(type) || []).filter(value => value !== callback));
  }
  dispatch(type, values) {
    const event = { isPrimary: true, button: 0, pointerId: 1, preventDefault() {}, ...values };
    for (const callback of this.listeners.get(type) || []) callback(event);
  }
}

function exercise(side) {
  const documentObj = new FakeSurface();
  const drawer = { contains: target => target === drawer };
  let open = false;
  let opens = 0;
  let closes = 0;
  const controller = createEdgeDrawerGesture({
    documentObj,
    windowObj: { innerWidth: 400 },
    side,
    drawer,
    isOpen: () => open,
    open: () => { open = true; opens++; },
    close: () => { open = false; closes++; },
  });
  const openingStart = side === "left" ? 3 : 397;
  const openingEnd = side === "left" ? 80 : 320;
  documentObj.dispatch("pointerdown", { clientX: openingStart, clientY: 100, target: {} });
  documentObj.dispatch("pointermove", { clientX: openingEnd, clientY: 102, target: {} });
  documentObj.dispatch("pointerup", { clientX: openingEnd, clientY: 102, target: {} });
  assert.equal(opens, 1, `${side} edge must reveal its drawer`);

  const closingEnd = side === "left" ? 20 : 380;
  documentObj.dispatch("pointerdown", { clientX: 90, clientY: 100, target: drawer });
  documentObj.dispatch("pointermove", { clientX: closingEnd, clientY: 101, target: drawer });
  documentObj.dispatch("pointerup", { clientX: closingEnd, clientY: 101, target: drawer });
  assert.equal(closes, 1, `${side} drawer must retract toward its owning edge`);

  documentObj.dispatch("pointerdown", { clientX: 200, clientY: 100, target: {} });
  documentObj.dispatch("pointermove", { clientX: openingEnd, clientY: 100, target: {} });
  documentObj.dispatch("pointerup", { clientX: openingEnd, clientY: 100, target: {} });
  assert.equal(opens, 1, "a gesture away from the closed edge must be ignored");
  controller.destroy();
}

exercise("left");
exercise("right");
console.log(JSON.stringify({ edgeDrawers: "PASS", sides: ["left", "right"], gestures: ["reveal", "retract"] }));
