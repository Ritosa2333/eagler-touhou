import { readFile, writeFile } from "node:fs/promises";
import { resolve } from "node:path";

export function normalizeSiteUrl(value) {
  if (!value) return null;
  const url = new URL(value);
  if (!/^https?:$/.test(url.protocol) || url.username || url.password || url.search || url.hash || !url.pathname.endsWith("/")) {
    throw new Error("site URL must be an http(s) directory URL without credentials, query or fragment");
  }
  return url.href;
}
const escape = value => String(value).replaceAll("&", "&amp;").replaceAll('"', "&quot;").replaceAll("<", "&lt;").replaceAll(">", "&gt;");
const variants = [["zh-CN", ""], ["en", "en.html"]];

// Public origin belongs to the deployment, never to the reusable launcher.
// Call before building the App Shell and deployment file identities.
export async function writeSiteMetadata(root, siteUrl) {
  const base = normalizeSiteUrl(siteUrl);
  const paths = ["index.html", "en.html", "faq.html", "about.html"];
  for (const path of paths) {
    const filename = resolve(root, path);
    let html = await readFile(filename, "utf8");
    const canonical = base ? new URL(path === "index.html" ? "" : path, base).href : null;
    const alternates = base && ["index.html", "en.html"].includes(path)
      ? [...variants, ["x-default", ""]].map(([locale, url]) => `  <link rel="alternate" hreflang="${locale}" href="${escape(new URL(url, base).href)}">`).join("\n") : "";
    const metadata = `<!-- site-metadata:start -->${canonical ? `\n  <link rel="canonical" href="${escape(canonical)}">\n${alternates}\n  ` : ""}<!-- site-metadata:end -->`;
    html = /<!-- site-metadata:start -->[\s\S]*?<!-- site-metadata:end -->/.test(html)
      ? html.replace(/<!-- site-metadata:start -->[\s\S]*?<!-- site-metadata:end -->/, metadata)
      : html.replace("</head>", `  ${metadata}\n</head>`);
    await writeFile(filename, html);
  }
  await writeFile(resolve(root, "robots.txt"), `User-agent: *\nAllow: /\n${base ? `\nSitemap: ${new URL("sitemap.xml", base).href}\n` : ""}`);
  const urls = base ? paths.map(path => {
    const href = new URL(path === "index.html" ? "" : path, base).href;
    const alternates = ["index.html", "en.html"].includes(path)
      ? [...variants, ["x-default", ""]].map(([locale, url]) => `<xhtml:link rel="alternate" hreflang="${locale}" href="${escape(new URL(url, base).href)}"/>`).join("") : "";
    return `  <url><loc>${escape(href)}</loc>${alternates}</url>`;
  }).join("\n") : "";
  await writeFile(resolve(root, "sitemap.xml"), `<?xml version="1.0" encoding="UTF-8"?>\n<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9" xmlns:xhtml="http://www.w3.org/1999/xhtml">\n${urls}\n</urlset>\n`);
  return [...paths, "robots.txt", "sitemap.xml"];
}
