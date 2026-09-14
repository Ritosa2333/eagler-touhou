import { mkdir, readFile, writeFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";
import { pathToFileURL } from "node:url";

// Maintainer-only tools are loaded only when TypeScript sources are present.
// Self-host distributions consume the already generated artifacts.
export async function optimizeLauncher({ project, buildRoot, optimizedRoot }) {
  const { build } = await import("esbuild");
  const { parse, serialize } = await import("parse5");
  await mkdir(optimizedRoot, { recursive: true });
  await build({
    entryPoints: { "assets/launcher/app": resolve(buildRoot, "assets/launcher/app.mjs") },
    outdir: optimizedRoot, bundle: true, splitting: true, format: "esm",
    platform: "browser", target: "es2022", charset: "utf8",
    outExtension: { ".js": ".mjs" }, chunkNames: "assets/launcher/[name]-[hash]",
    minify: true,
    legalComments: "inline", logLevel: "warning",
  });
  const authoredStyles = await readFile(resolve(project, "public/styles.css"), "utf8");
  const splitFonts = await readFile(resolve(project, "public/ui-fonts.css"), "utf8");
  const styles = authoredStyles
    .replace(/@font-face\{font-family:"ET Chill Round";font-style:normal;font-weight:(?:400|700);font-display:swap;src:url\("assets\/fonts\/chill-round-gothic-site-(?:medium|bold)\.woff2"\) format\("woff2"\)\}\r?\n/g, "");
  await build({ stdin: { contents: `${splitFonts}\n${styles}`, resolveDir: resolve(project, "public"), sourcefile: "styles.css", loader: "css" },
    outfile: resolve(optimizedRoot, "styles.css"), bundle: true, minify: true,
    external: ["*.woff2", "*.webp", "*.png", "*.svg", "*.jpg"], logLevel: "warning" });
  await build({ entryPoints: [resolve(project, "public/touch-guide.css")], outfile: resolve(optimizedRoot, "touch-guide.css"),
    bundle: true, minify: true, external: ["*.woff2", "*.webp", "*.png", "*.svg", "*.jpg"], logLevel: "warning" });
  const { UI_MESSAGES } = await import(pathToFileURL(resolve(buildRoot, "assets/launcher/i18n.mjs")).href);
  const source = await readFile(resolve(project, "public/index.html"), "utf8");
  for (const locale of ["zh-CN", "en"]) {
    const document = parse(source);
    const translate = node => {
      const attribute = name => node.attrs?.find(attr => attr.name === name);
      if (node.tagName === "html") {
        attribute("lang").value = locale;
        attribute("data-ui-locale").value = locale;
      }
      const key = attribute("data-i18n")?.value;
      if (key) {
        if (!UI_MESSAGES[locale][key]) throw new Error(`Missing ${locale} translation: ${key}`);
        node.childNodes = [{ nodeName: "#text", value: UI_MESSAGES[locale][key], parentNode: node }];
      }
      for (const name of ["content", "aria-label", "title", "placeholder", "alt"]) {
        const translation = attribute(`data-i18n-${name}`)?.value;
        if (translation && attribute(name)) attribute(name).value = UI_MESSAGES[locale][translation];
      }
      if (attribute("id")?.value === "uiLanguageLink") {
        attribute("href").value = locale === "en" ? "./" : "en.html";
        attribute("hreflang").value = locale === "en" ? "zh-CN" : "en";
        node.childNodes = [{ nodeName: "#text", value: locale === "en" ? "简体中文" : "English", parentNode: node }];
      }
      if (node.tagName === "option" && node.parentNode?.attrs?.some(attr => attr.name === "id" && attr.value === "uiLanguageSelect")) {
        node.attrs = node.attrs.filter(attr => attr.name !== "selected");
        if (attribute("value")?.value === locale) node.attrs.push({ name: "selected", value: "" });
      }
      for (const child of node.childNodes || []) translate(child);
      if (node.content) translate(node.content);
    };
    translate(document);
    await writeFile(resolve(optimizedRoot, locale === "en" ? "en.html" : "index.html"), serialize(document));
  }
}
