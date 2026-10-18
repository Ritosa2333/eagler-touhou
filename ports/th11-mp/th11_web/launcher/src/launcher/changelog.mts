export const CHANGELOG_FILE = "CHANGELOG.txt";
export const CHANGELOG_SEEN_STORAGE_KEY = "eagler-touhou-changelog-seen-v2";

export type ChangelogLoadResult =
  | Readonly<{ kind: "available"; contentId: string }>
  | Readonly<{ kind: "empty" }>
  | Readonly<{ kind: "error"; error: unknown }>;

type ChangelogStorage = Pick<Storage, "getItem" | "setItem">;

export function normalizeChangelogText(source: string): string {
  return String(source).replace(/^\uFEFF/, "").replace(/\r\n?/g, "\n").trim();
}

// Content-change fingerprint only, not a security checksum. The normalized
// length plus FNV-1a gives the browser a compact release-note identity without
// requiring maintainers to edit a second JavaScript version constant.
export function changelogContentIdentity(source: string): string {
  const text = normalizeChangelogText(source);
  let hash = 0x811c9dc5;
  for (let index = 0; index < text.length; index++) {
    hash ^= text.charCodeAt(index);
    hash = Math.imul(hash, 0x01000193);
  }
  return `fnv1a32-${text.length.toString(36)}-${(hash >>> 0).toString(16).padStart(8, "0")}`;
}

function appendLinkedText(documentObj: Document, target: HTMLElement, text: string): void {
  const pattern = /https?:\/\/[^\s<>"']+/gi;
  let offset = 0;
  for (const match of text.matchAll(pattern)) {
    const index = match.index ?? 0;
    if (index > offset) target.append(documentObj.createTextNode(text.slice(offset, index)));
    const link = documentObj.createElement("a");
    link.href = match[0];
    link.target = "_blank";
    link.rel = "noopener noreferrer";
    link.textContent = match[0];
    target.append(link);
    offset = index + match[0].length;
  }
  if (offset < text.length) target.append(documentObj.createTextNode(text.slice(offset)));
}

export function renderChangelogText(documentObj: Document, target: HTMLElement, source: string): void {
  target.replaceChildren();
  const list = documentObj.createElement("div");
  list.className = "changelog-list";
  target.append(list);
  let entry: HTMLElement | null = null;
  let bullets: HTMLUListElement | null = null;
  const ensureEntry = () => {
    if (entry) return entry;
    entry = documentObj.createElement("section");
    entry.className = "changelog-item";
    list.append(entry);
    return entry;
  };

  for (const rawLine of normalizeChangelogText(source).split("\n")) {
    const line = rawLine.trim();
    if (!line || /^=+$/.test(line)) { bullets = null; continue; }
    if (line === "EAGLER TOUHOU CHANGELOG") continue;
    if (/^-{8,}$/.test(line)) { entry = null; bullets = null; continue; }
    const dated = line.match(/^\[([^\]]+)\]\s*(.*)$/);
    if (dated) {
      entry = documentObj.createElement("section");
      entry.className = "changelog-item";
      const heading = documentObj.createElement("h2");
      const date = documentObj.createElement("span");
      date.className = "changelog-date";
      date.textContent = dated[1];
      heading.append(date);
      if (dated[2]) heading.append(documentObj.createTextNode(` ${dated[2]}`));
      entry.append(heading);
      list.append(entry);
      bullets = null;
      continue;
    }
    if (line.startsWith("## ")) {
      const heading = documentObj.createElement("h3");
      heading.textContent = line.slice(3).trim();
      ensureEntry().append(heading);
      bullets = null;
      continue;
    }
    if (line.startsWith("- ")) {
      if (!bullets) {
        bullets = documentObj.createElement("ul");
        ensureEntry().append(bullets);
      }
      const item = documentObj.createElement("li");
      appendLinkedText(documentObj, item, line.slice(2));
      bullets.append(item);
      continue;
    }
    bullets = null;
    const paragraph = documentObj.createElement("p");
    appendLinkedText(documentObj, paragraph, line);
    ensureEntry().append(paragraph);
  }
}

export interface ChangelogControllerOptions {
  documentObj?: Document;
  storage?: ChangelogStorage | null;
  fetchImpl?: typeof fetch;
  emptyText?: () => string;
  readFailureText?: (error: unknown) => string;
  canShowAutomatically?: () => boolean;
}

function defaultStorage(): ChangelogStorage | null {
  try { return globalThis.localStorage ?? null; }
  catch { return null; }
}

function isDialogElement(value: HTMLElement): value is HTMLDialogElement {
  return "showModal" in value && typeof value.showModal === "function" &&
    "close" in value && typeof value.close === "function" &&
    "open" in value && typeof value.open === "boolean";
}

export function createChangelogController(options: ChangelogControllerOptions = {}) {
  const documentObj = options.documentObj ?? globalThis.document;
  const storage = options.storage === undefined ? defaultStorage() : options.storage;
  const fetchImpl = options.fetchImpl ?? globalThis.fetch;
  const emptyText = options.emptyText ?? (() => "暂无更新日志。");
  const readFailureText = options.readFailureText ?? (error => {
    const message = error instanceof Error ? error.message : String(error);
    return `CHANGELOG.txt 读取失败：${message}。请刷新页面后重试。`;
  });
  if (!documentObj || typeof fetchImpl !== "function") throw new Error("Changelog requires a browser document and fetch implementation");

  const find = (id: string): HTMLElement => {
    const value = documentObj.getElementById(id);
    if (!value) throw new Error(`Changelog element is missing: #${id}`);
    return value;
  };
  const findDialog = (id: string): HTMLDialogElement => {
    const value = find(id);
    if (!isDialogElement(value)) throw new Error(`Changelog element must be a dialog: #${id}`);
    return value;
  };
  const dialog = findDialog("changelogDialog");
  const target = find("changelogText");
  let cached: ChangelogLoadResult | null = null;

  function renderStatus(text: string, className: string): void {
    target.replaceChildren();
    const paragraph = documentObj.createElement("p");
    paragraph.className = className;
    paragraph.textContent = text;
    target.append(paragraph);
  }

  async function load(): Promise<ChangelogLoadResult> {
    if (cached?.kind === "available" || cached?.kind === "empty") return cached;
    try {
      const response = await fetchImpl(CHANGELOG_FILE, { cache: "no-store" });
      if (!response.ok) throw new Error(`HTTP ${response.status}`);
      const text = normalizeChangelogText(await response.text());
      if (!text) {
        cached = Object.freeze({ kind: "empty" });
        renderStatus(emptyText(), "changelog-empty");
        return cached;
      }
      renderChangelogText(documentObj, target, text);
      cached = Object.freeze({ kind: "available", contentId: changelogContentIdentity(text) });
      return cached;
    } catch (error) {
      renderStatus(readFailureText(error), "changelog-error");
      return Object.freeze({ kind: "error", error });
    }
  }

  function markSeen(result: ChangelogLoadResult): void {
    if (result.kind !== "available") return;
    try { storage?.setItem(CHANGELOG_SEEN_STORAGE_KEY, result.contentId); } catch {}
  }

  async function showManual(): Promise<ChangelogLoadResult> {
    const result = await load();
    if (result.kind === "empty") renderStatus(emptyText(), "changelog-empty");
    dialog.showModal();
    markSeen(result);
    return result;
  }

  async function maybeShowAutomatically(): Promise<boolean> {
    const result = await load();
    if (result.kind !== "available") return false;
    let seen = "";
    try { seen = storage?.getItem(CHANGELOG_SEEN_STORAGE_KEY) || ""; } catch {}
    if (seen === result.contentId) return false;
    // A slow response must not place a modal over an already running game.
    if (options.canShowAutomatically && !options.canShowAutomatically()) return false;
    dialog.showModal();
    markSeen(result);
    return true;
  }

  function close(): void {
    if (dialog.open) dialog.close();
  }

  return Object.freeze({ load, showManual, maybeShowAutomatically, close });
}
