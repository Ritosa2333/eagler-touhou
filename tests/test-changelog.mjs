import assert from "node:assert/strict";
import {
  CHANGELOG_SEEN_STORAGE_KEY,
  changelogContentIdentity,
  createChangelogController,
  normalizeChangelogText,
} from "../.cache/build/browser/assets/launcher/changelog.mjs";

assert.equal(normalizeChangelogText("\uFEFFA\r\nB\r\n"), "A\nB");
assert.equal(changelogContentIdentity("A\r\nB"), changelogContentIdentity("A\nB"),
  "line-ending differences must not create a new changelog release");
assert.notEqual(changelogContentIdentity("A"), changelogContentIdentity("B"),
  "changed release-note content must create a new identity");

class FakeElement {
  constructor(id = "") { this.id = id; }
  className = "";
  textContent = "";
  children = [];
  childNodes = [];
  open = false;
  showCount = 0;
  closeCount = 0;
  classList = { values: new Set(), add: (...names) => names.forEach(name => this.classList.values.add(name)), remove: (...names) => names.forEach(name => this.classList.values.delete(name)) };
  listeners = new Map();
  append(...values) { this.children.push(...values); this.childNodes.push(...values); }
  replaceChildren(...values) { this.children = [...values]; this.childNodes = [...values]; }
  querySelectorAll() { return []; }
  addEventListener(type, callback) { this.listeners.set(type, [...(this.listeners.get(type) || []), callback]); }
  showModal() { this.open = true; this.showCount++; }
  close() { this.open = false; this.closeCount++; }
}

class FakeDocument {
  constructor() {
    this.elements = new Map([
      ["changelogDialog", new FakeElement("changelogDialog")],
      ["changelogText", new FakeElement("changelogText")],
    ]);
  }
  getElementById(id) { return this.elements.get(id) || null; }
  createElement() { return new FakeElement(); }
  createTextNode(text) { return { nodeType: 3, textContent: text }; }
}

function storageFrom(values = new Map()) {
  return {
    values,
    getItem: key => values.has(key) ? values.get(key) : null,
    setItem: (key, value) => values.set(key, String(value)),
  };
}

function response(text, status = 200) {
  return { ok: status >= 200 && status < 300, status, text: async () => text };
}

globalThis.marked = { parse: () => "<h2>2026-09-06</h2><p>Item</p>" };
const releaseText = "# EAGLER TOUHOU CHANGELOG\n\n## 2026-09-06\n\n- Item";
const sharedStorage = storageFrom();
const firstDocument = new FakeDocument();
const first = createChangelogController({
  documentObj: firstDocument,
  storage: sharedStorage,
  fetchImpl: async () => response(releaseText),
  matchMediaImpl: () => ({ matches: true }),
});
assert.equal(await first.maybeShowAutomatically(), true, "new non-empty content must auto-show once");
assert.equal(firstDocument.getElementById("changelogDialog").showCount, 1);
const firstIdentity = changelogContentIdentity(releaseText);
assert.equal(sharedStorage.values.get(CHANGELOG_SEEN_STORAGE_KEY), firstIdentity);
first.close();
assert.equal(firstDocument.getElementById("changelogDialog").closeCount, 1, "reduced-motion close finishes immediately");

const secondDocument = new FakeDocument();
const second = createChangelogController({
  documentObj: secondDocument,
  storage: sharedStorage,
  fetchImpl: async () => response(releaseText),
});
assert.equal(await second.maybeShowAutomatically(), false, "already-seen content must not auto-show again");
assert.equal(secondDocument.getElementById("changelogDialog").showCount, 0);

const changedText = `${releaseText}\n- New item`;
const changedDocument = new FakeDocument();
const changed = createChangelogController({
  documentObj: changedDocument,
  storage: sharedStorage,
  fetchImpl: async () => response(changedText),
});
assert.equal(await changed.maybeShowAutomatically(), true,
  "changing only CHANGELOG.md content must create a new auto-show release");
assert.equal(sharedStorage.values.get(CHANGELOG_SEEN_STORAGE_KEY), changelogContentIdentity(changedText));

const emptyDocument = new FakeDocument();
const empty = createChangelogController({
  documentObj: emptyDocument,
  storage: storageFrom(),
  fetchImpl: async () => response("\n\r\n"),
  emptyText: () => "EMPTY",
});
assert.equal(await empty.maybeShowAutomatically(), false, "empty packaged changelog is a valid non-interrupting state");
assert.equal(emptyDocument.getElementById("changelogDialog").showCount, 0);
const emptyResult = await empty.showManual();
assert.equal(emptyResult.kind, "empty");
assert.equal(emptyDocument.getElementById("changelogDialog").showCount, 1,
  "manual changelog entry remains usable when release content is empty");
assert.equal(emptyDocument.getElementById("changelogText").children[0]?.textContent, "EMPTY");

const reopenDocument = new FakeDocument();
let delayedClose;
const reopen = createChangelogController({
  documentObj: reopenDocument,
  storage: storageFrom(),
  fetchImpl: async () => response(releaseText),
  setTimeoutImpl: callback => { delayedClose = callback; return 1; },
});
await reopen.showManual();
reopen.close();
await reopen.showManual();
delayedClose();
assert.equal(reopenDocument.getElementById("changelogDialog").open, true,
  "reopening during the close animation must cancel the stale close completion");

const failedDocument = new FakeDocument();
const failed = createChangelogController({
  documentObj: failedDocument,
  storage: storageFrom(),
  fetchImpl: async () => { throw new Error("offline"); },
  readFailureText: error => `FAILED:${error.message}`,
});
assert.equal(await failed.maybeShowAutomatically(), false, "fetch failure must never interrupt Launcher startup");
assert.equal(failedDocument.getElementById("changelogDialog").showCount, 0);
const failedResult = await failed.showManual();
assert.equal(failedResult.kind, "error");
assert.equal(failedDocument.getElementById("changelogDialog").showCount, 1);
assert.equal(failedDocument.getElementById("changelogText").children[0]?.textContent, "FAILED:offline");

console.log(JSON.stringify({ changelog: "PASS", identity: "content-derived", empty: "valid", autoShow: "once-per-content" }));
