import assert from "node:assert/strict";
import { createMultiplayerGuideController, MULTIPLAYER_GUIDE_FILE } from "../.cache/build/browser/assets/launcher/multiplayer-guide.mjs";

class FakeElement {
  constructor(id = "") { this.id = id; }
  className = "";
  textContent = "";
  innerHTML = "";
  childNodes = [];
  open = false;
  classList = { values: new Set(), add: (...names) => names.forEach(name => this.classList.values.add(name)), remove: (...names) => names.forEach(name => this.classList.values.delete(name)) };
  listeners = new Map();
  append(...values) { this.childNodes.push(...values); }
  replaceChildren(...values) { this.childNodes = [...values]; }
  querySelector() { return null; }
  querySelectorAll() { return []; }
  addEventListener(type, callback) { this.listeners.set(type, [...(this.listeners.get(type) || []), callback]); }
  showModal() { this.open = true; }
  close() { this.open = false; }
}

class FakeDocument {
  baseURI = "https://launcher.invalid/";
  constructor() {
    this.elements = new Map([
      ["mpGuideDialog", new FakeElement("mpGuideDialog")],
      ["mpGuideContent", new FakeElement("mpGuideContent")],
      ["mpGuideClose", new FakeElement("mpGuideClose")],
    ]);
  }
  getElementById(id) { return this.elements.get(id) || null; }
  createElement() { return new FakeElement(); }
}

let requested = "";
const documentObj = new FakeDocument();
const guide = createMultiplayerGuideController({
  documentObj,
  fetchImpl: async path => {
    requested = path;
    return { ok: true, status: 200, text: async () => "<h2>1. Boss 生命值缩放</h2>" };
  },
  matchMediaImpl: () => ({ matches: true }),
});
await guide.show();
assert.equal(requested, MULTIPLAYER_GUIDE_FILE);
assert.equal(documentObj.getElementById("mpGuideDialog").open, true);
assert.equal(documentObj.getElementById("mpGuideContent").innerHTML, "<h2>1. Boss 生命值缩放</h2>");
guide.close();
assert.equal(documentObj.getElementById("mpGuideDialog").open, false);

const failedDocument = new FakeDocument();
const failed = createMultiplayerGuideController({
  documentObj: failedDocument,
  fetchImpl: async () => { throw new Error("offline"); },
  readFailureText: error => `FAILED:${error.message}`,
});
await failed.show();
assert.equal(failedDocument.getElementById("mpGuideContent").childNodes[0].textContent, "FAILED:offline");

console.log(JSON.stringify({ multiplayerGuide: "PASS", source: MULTIPLAYER_GUIDE_FILE }));
