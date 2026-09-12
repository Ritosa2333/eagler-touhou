import { loadMarkdownParser, parseTrustedMarkdown, type MarkdownParser } from "./markdown.mjs";

export const CHANGELOG_FILE = "CHANGELOG.md";
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

export function renderChangelogMarkdown(
  documentObj: Document,
  target: HTMLElement,
  source: string,
  parseMarkdown: MarkdownParser,
): void {
  target.replaceChildren();
  const list = documentObj.createElement("div");
  list.className = "changelog-list";
  target.append(list);

  // CHANGELOG.md is repository-controlled content. Marked supplies the full
  // Markdown grammar; this adapter only applies the Launcher's visual grouping.
  const parsed = parseTrustedMarkdown(documentObj, normalizeChangelogText(source), parseMarkdown);
  let entry: HTMLElement | null = null;
  for (const node of Array.from(parsed.childNodes)) {
    const tagName = "tagName" in node ? String((node as Element).tagName).toUpperCase() : "";
    if (tagName === "H1") continue;
    if (tagName === "H2" || !entry) {
      entry = documentObj.createElement("section");
      entry.className = "changelog-item";
      list.append(entry);
    }
    entry.append(node);
  }
}

export interface ChangelogControllerOptions {
  documentObj?: Document;
  storage?: ChangelogStorage | null;
  fetchImpl?: typeof fetch;
  emptyText?: () => string;
  readFailureText?: (error: unknown) => string;
  parseMarkdown?: MarkdownParser;
  matchMediaImpl?: (query: string) => Pick<MediaQueryList, "matches">;
  setTimeoutImpl?: (callback: () => void, delay: number) => number;
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
    return `CHANGELOG.md 读取失败：${message}。请刷新页面后重试。`;
  });
  const parseMarkdown = options.parseMarkdown;
  const matchMediaImpl = options.matchMediaImpl ?? (query => globalThis.matchMedia?.(query) ?? { matches: false });
  const setTimeoutImpl = options.setTimeoutImpl ?? ((callback, delay) => globalThis.setTimeout(callback, delay));
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
  let closing = false;
  let closeGeneration = 0;

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
      renderChangelogMarkdown(documentObj, target, text, parseMarkdown ?? await loadMarkdownParser(documentObj));
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
    ++closeGeneration;
    closing = false;
    dialog.classList.remove("closing");
    if (!dialog.open) dialog.showModal();
    markSeen(result);
    return result;
  }

  async function maybeShowAutomatically(): Promise<boolean> {
    const result = await load();
    if (result.kind !== "available") return false;
    let seen = "";
    try { seen = storage?.getItem(CHANGELOG_SEEN_STORAGE_KEY) || ""; } catch {}
    if (seen === result.contentId) return false;
    ++closeGeneration;
    closing = false;
    dialog.classList.remove("closing");
    if (!dialog.open) dialog.showModal();
    markSeen(result);
    return true;
  }

  function close(): void {
    if (!dialog.open || closing) return;
    const generation = ++closeGeneration;
    if (matchMediaImpl("(prefers-reduced-motion: reduce)").matches) { dialog.close(); return; }
    closing = true;
    dialog.classList.add("closing");
    setTimeoutImpl(() => {
      if (generation !== closeGeneration) return;
      if (dialog.open) dialog.close();
      dialog.classList.remove("closing");
      closing = false;
    }, 220);
  }

  dialog.addEventListener("cancel", event => { event.preventDefault(); close(); });
  dialog.addEventListener("click", event => { if (event.target === dialog) close(); });

  return Object.freeze({ load, showManual, maybeShowAutomatically, close, isOpen: () => dialog.open });
}
