export type MarkdownParser = (source: string) => string;

let markdownParserLoad: Promise<MarkdownParser> | null = null;

function globalMarkdownParser(): MarkdownParser | null {
  const parser = (globalThis as typeof globalThis & {
    marked?: { parse: (markdown: string, options?: Readonly<Record<string, unknown>>) => string | Promise<string> };
  }).marked;
  if (!parser) return null;
  return source => {
    const html = parser.parse(source, { async: false, gfm: true });
    if (typeof html !== "string") throw new Error("Markdown parser unexpectedly returned an asynchronous result");
    return html;
  };
}

export async function loadMarkdownParser(documentObj: Document): Promise<MarkdownParser> {
  const existing = globalMarkdownParser();
  if (existing) return existing;
  if (!markdownParserLoad) {
    markdownParserLoad = (async () => {
      const script = documentObj.createElement("script");
      script.src = new URL("../../vendor/marked.umd.js", import.meta.url).href;
      await new Promise<void>((resolvePromise, reject) => {
        script.addEventListener("load", () => resolvePromise(), { once: true });
        script.addEventListener("error", () => reject(new Error("Markdown parser download failed")), { once: true });
        (documentObj.head || documentObj.documentElement).append(script);
      });
      const loaded = globalMarkdownParser();
      if (!loaded) throw new Error("Markdown parser failed to initialize");
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
