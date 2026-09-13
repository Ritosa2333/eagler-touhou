export type MarkdownParser = (source: string) => string;

let markdownParserLoad: Promise<MarkdownParser> | null = null;

function globalMarkdownParser(): MarkdownParser | null {
  const dependencies = globalThis as typeof globalThis & {
    marked?: { parse: (markdown: string, options?: Readonly<Record<string, unknown>>) => string | Promise<string> };
    DOMPurify?: { sanitize: (html: string, options?: Readonly<Record<string, unknown>>) => string };
  };
  const parser = dependencies.marked;
  const purifier = dependencies.DOMPurify;
  if (!parser || !purifier) return null;
  return source => {
    const html = parser.parse(source, { async: false, gfm: true });
    if (typeof html !== "string") throw new Error("Markdown parser unexpectedly returned an asynchronous result");
    return purifier.sanitize(html, {
      USE_PROFILES: { html: true },
      FORBID_TAGS: ["style"],
      FORBID_ATTR: ["style"],
    });
  };
}

async function loadScript(documentObj: Document, relativeUrl: string, label: string): Promise<void> {
  const script = documentObj.createElement("script");
  script.src = new URL(relativeUrl, import.meta.url).href;
  await new Promise<void>((resolvePromise, reject) => {
    script.addEventListener("load", () => resolvePromise(), { once: true });
    script.addEventListener("error", () => reject(new Error(`${label} download failed`)), { once: true });
    (documentObj.head || documentObj.documentElement).append(script);
  });
}

export async function loadMarkdownParser(documentObj: Document): Promise<MarkdownParser> {
  const existing = globalMarkdownParser();
  if (existing) return existing;
  if (!markdownParserLoad) {
    markdownParserLoad = (async () => {
      await Promise.all([
        loadScript(documentObj, "../../vendor/marked.umd.js", "Markdown parser"),
        loadScript(documentObj, "../../vendor/purify.min.js", "Markdown sanitizer"),
      ]);
      const loaded = globalMarkdownParser();
      if (!loaded) throw new Error("Markdown parser or sanitizer failed to initialize");
      return loaded;
    })().catch(error => {
      markdownParserLoad = null;
      throw error;
    });
  }
  return markdownParserLoad;
}

export function parseTrustedMarkdown(
  documentObj: Document,
  source: string,
  parseMarkdown: MarkdownParser,
): HTMLDivElement {
  const parsed = documentObj.createElement("div");
  parsed.innerHTML = parseMarkdown(source);
  const base = new URL(documentObj.baseURI || "https://launcher.invalid/");
  for (const link of Array.from(parsed.querySelectorAll("a"))) {
    let resolved: URL;
    try { resolved = new URL(link.getAttribute("href") || "", base); } catch { continue; }
    if (resolved.protocol !== "http:" && resolved.protocol !== "https:") {
      link.removeAttribute("href");
      continue;
    }
    if ((link.textContent || "").trim() !== (link.getAttribute("href") || "").trim()) {
      link.classList.add("markdown-link-labeled");
    }
    if (resolved.origin !== base.origin) {
      link.target = "_blank";
      link.rel = "noopener noreferrer";
    }
  }
  return parsed;
}
