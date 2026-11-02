#!/usr/bin/env node
import { readFile, mkdir } from "node:fs/promises";
import { resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { marked, Renderer } from "marked";
import { writeFileAtomic } from "../lib/atomic-file.mjs";
import {localizeCompatibilityHtml, loadCompatibilityTranslations, compatibilityThemeHtml} from '../lib/localized-compatibility.mjs';
import {acquireBuildLock} from '../lib/build-lock.mjs';
import {parseFragment, serialize} from 'parse5';

const defaultProject = resolve(fileURLToPath(new URL("..", import.meta.url)));

export async function buildContentPages({project = defaultProject, check = false, quiet = false} = {}) {
  project = resolve(project);
  const faqSourcePath = resolve(project, "docs", "FAQ.md");
  const firstUseNoticeSourcePath = resolve(project, "content", "FIRST_USE_NOTICE.md");
  const firstUseNoticeOutputPath = resolve(project, "public", "content", "FIRST_USE_NOTICE.html");
  const multiplayerSourcePath = resolve(project, "content", "MULTIPLAYER.md");
  const multiplayerOutputPath = resolve(project, "public", "content", "MULTIPLAYER.html");

  function escapeAttribute(value) {
    return String(value)
      .replaceAll("&", "&amp;")
      .replaceAll('"', "&quot;")
      .replaceAll("<", "&lt;")
      .replaceAll(">", "&gt;");
  }

  function escapeHtml(value) {
    return String(value)
      .replaceAll("&", "&amp;")
      .replaceAll("<", "&lt;")
      .replaceAll(">", "&gt;");
  }

  function safeContentHref(value) {
    const href = String(value || "").trim();
    if (!href) return "";
    try {
      const resolved = new URL(href, "https://eagler.invalid/");
      return resolved.protocol === "http:" || resolved.protocol === "https:" ? href : "";
    } catch {
      return "";
    }
  }

  const renderer = new Renderer();
  renderer.link = function link(token) {
    const href = safeContentHref(token.href);
    if (!href) return this.parser.parseInline(token.tokens);
    const title = token.title ? ` title="${escapeAttribute(token.title)}"` : "";
    const external = /^https?:\/\//i.test(href) ? ' target="_blank" rel="noopener"' : "";
    return `<a href="${escapeAttribute(href)}"${title}${external}>${this.parser.parseInline(token.tokens)}</a>`;
  };
  renderer.image = function image(token) {
    const href = safeContentHref(token.href);
    if (!href) return escapeHtml(token.text || "");
    const title = token.title ? ` title="${escapeAttribute(token.title)}"` : "";
    return `<img src="${escapeAttribute(href)}" alt="${escapeAttribute(token.text || "")}"${title} loading="lazy">`;
  };
  // Browser-facing Markdown is compiled into trusted static HTML. Raw authored
  // HTML remains text rather than becoming a second, unsanitized markup channel.
  renderer.html = token => escapeHtml(token.text || "");

  function normalizeMarkdown(source) {
    return String(source).replace(/^\uFEFF/, "").replace(/\r\n?/g, "\n").trim();
  }

  function lexMarkdown(source) {
    return marked.lexer(normalizeMarkdown(source), { gfm: true });
  }

  function withoutLeadingTitle(tokens) {
    const result = [...tokens];
    if (result[0]?.type === "heading" && result[0].depth === 1) result.shift();
    while (result[0]?.type === "space") result.shift();
    return result;
  }

  function renderTokens(tokens) {
    return marked.parser(tokens, { renderer }).trim();
  }

  function renderFirstUseNoticeFragment(source) {
    if (!normalizeMarkdown(source)) return "";
    const tokens = withoutLeadingTitle(lexMarkdown(source));
    const groups = [];
    let current = [];
    for (const token of tokens) {
      if (token.type === "heading" && token.depth === 2 && current.length) {
        groups.push(current);
        current = [];
      }
      current.push(token);
    }
    if (current.length) groups.push(current);
    const sections = groups
      .map(group => `<section class="first-use-notice-item">\n${renderTokens(group)}\n</section>`)
      .join("\n");
    return `<div class="first-use-notice-list">\n${sections}\n</div>\n`;
  }

  function renderMultiplayerFragment(source) {
    if (!normalizeMarkdown(source)) return "";
    const content = renderTokens(withoutLeadingTitle(lexMarkdown(source)));
    return content ? `${content}\n` : "";
  }

  await mkdir(resolve(project, '.cache'), {recursive: true});
  const release = await acquireBuildLock(resolve(project, '.cache/content-pages.lock'), {timeout: 180_000});
  try {
  const faqMarkdown = normalizeMarkdown(await readFile(faqSourcePath, "utf8"));
  const content = marked.parse(faqMarkdown, { async: false, gfm: true, renderer });
  if (typeof content !== "string" || !content.includes("<h1>常见问题</h1>")) {
    throw new Error("docs/FAQ.md must contain the level-one heading '常见问题'");
  }
  const emphasisStack = [];
  for (const match of content.matchAll(/<(\/)?(strong|em)>/gi)) {
    const tag = match[2].toLowerCase();
    if (!match[1]) {
      if (emphasisStack.includes(tag)) {
        throw new Error(`docs/FAQ.md produced nested <${tag}> markup; add spacing around adjacent emphasis`);
      }
      emphasisStack.push(tag);
    } else if (emphasisStack.pop() !== tag) {
      throw new Error("docs/FAQ.md produced mismatched emphasis markup");
    }
  }


  const firstUseNoticeHtml = renderFirstUseNoticeFragment(await readFile(firstUseNoticeSourcePath, "utf8"));
  const multiplayerHtml = renderMultiplayerFragment(await readFile(multiplayerSourcePath, "utf8"));

  async function emit(path, body, staleMessage) {
    if (path.endsWith('.html') && /<head\b/i.test(body) && !/<meta\s+name=["']darkreader-lock["']/i.test(body)) {
      body = body.replace(/(<head\b[^>]*>)/i, '$1\n  <meta name="darkreader-lock">');
    }
    let current = null;
    try {current = await readFile(path, 'utf8');} catch (error) {if (error.code !== 'ENOENT') throw error;}
    if (check) {
      if (current !== body) throw new Error(staleMessage);
      return;
    }
    if (current !== body) await writeFileAtomic(path, body, "utf8");
  }

  await emit(firstUseNoticeOutputPath, firstUseNoticeHtml, "public/content/FIRST_USE_NOTICE.html is stale; run npm run build:content");
  await emit(multiplayerOutputPath, multiplayerHtml, "public/content/MULTIPLAYER.html is stale; run npm run build:content");
  // The authored documents also supply the in-app Chinese messages.
  const catalogPath = resolve(project, 'messages/zh-CN.json');
  const catalog = JSON.parse(await readFile(catalogPath, 'utf8'));
  const keys = JSON.parse(await readFile(resolve(project, 'messages/keys.json'), 'utf8'));
  const aboutBody = parseFragment(await readFile(resolve(project, 'content/ABOUT.html'), 'utf8'));
  for (const [key, html] of Object.entries({'content.faq': content, 'content.firstUse': firstUseNoticeHtml, 'content.multiplayer': multiplayerHtml, 'content.about': serialize(aboutBody)})) catalog[keys[key].id] = html.trim();
  await emit(catalogPath, JSON.stringify(catalog, null, 2) + '\n', 'Chinese document messages are stale; run bun run build:content');
  const migrationTokens=await readFile(resolve(project,'app/ui/tokens.css'),'utf8');
  const compatibilityPath = resolve(project, 'public/compatibility.html');
  const documentControls = await readFile(resolve(project, 'app/ui/document-controls.css'), 'utf8');
  const compatibilitySource = compatibilityThemeHtml(await readFile(compatibilityPath, 'utf8'), migrationTokens, documentControls);
  await emit(compatibilityPath, compatibilitySource, 'Compatibility theme is stale; run npm run build:content');
  const compatibilityText = await loadCompatibilityTranslations();
  for (const locale of ['en', 'ja']) {
    const path = resolve(project, `public/compatibility.${locale}.html`);
    await emit(path, localizeCompatibilityHtml(compatibilitySource, locale, compatibilityText), `${path} is stale; run npm run build:content`);
  }
  const brandModule=await readFile(resolve(project,'lib/site-brand.mjs'),'utf8');
  await emit(resolve(project,'public/assets/site-brand.mjs'),brandModule+'\ninitializeStaticSiteBrand(document,import.meta.url);\n',"Site brand initializer is stale; run npm run build:content");
  const migrationPath=resolve(project,'public/migrate.html');
  const brandFallback=(await readFile(resolve(project,'public/assets/brand/face-F02B.svg'))).toString('base64');
  const migration=(await readFile(migrationPath,'utf8'))
    .replace(/<style id="siteTheme">[\s\S]*?<\/style>/,()=>'<style id="siteTheme">'+migrationTokens+'</style>')
    .replace(/<script id="siteBrand">[\s\S]*?<\/script>/,()=>'<script id="siteBrand">(function(){'+brandModule.replace(/^export /gm,'')+'\ninitializeStaticSiteBrand(document,new URL("assets/site-brand.mjs",document.baseURI).href);})();</script>')
    .replace(/<img[^>]*data-site-brand[^>]*>/,image=>image.replace(/src="[^"]*"/,'src="data:image/svg+xml;base64,'+brandFallback+'"'));
  await emit(migrationPath,migration,"Migration theme is stale; run npm run build:content");
  if (!quiet) console.log(`Content pages: ${check ? "PASS" : "built"} (FAQ + First-use Notice + Multiplayer Guide + Compatibility)`);
  } finally {release();}
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  const args = process.argv.slice(2);
  if (args.some(value => value !== '--check' && value !== '--quiet')) {
    throw new Error('usage: node scripts/build-content-pages.mjs [--check] [--quiet]');
  }
  await buildContentPages({check: args.includes('--check'), quiet: args.includes('--quiet')});
}
