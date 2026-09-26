import { readFile } from "node:fs/promises";
import { resolve } from "node:path";
import { pathToFileURL } from "node:url";
import { writeFileAtomic } from "./atomic-file.mjs";

const FEATURE_STYLE_STARTS = Object.freeze([
  line => line.startsWith(".touch-direct-surface{"),
  line => line === "/* Replay manager: localized, separated actions, tonal rows. */",
  line => line === "/* Game viewport position editor: reuse the touch-layout theme, but make the",
  line => line.startsWith(".tools.mp-mode{"),
  line => line.startsWith(".mp-settings-room-drawer{"),
  line => line.startsWith(".main.mp-room-open .game,.main.mp-room-open .tools{"),
  line => line === "/* R is an ordinary cross-game control: below ESC and before the optional thprac group. */",
]);
const FEATURE_STYLE_ENDS = Object.freeze([
  line => line === "/* Top bar and rounded surfaces; preserve the existing neutral palette. */",
  line => line === "/* Desktop reduced motion follows the mobile lightweight selection cue. */",
  line => line === "/* Shared Multiplayer product surface. It deliberately inherits the launcher's",
  line => line === "/* Edge drawers: First-use Notice owns the right edge; Site Notice owns the left. */",
  line => line === "/* Shared Multiplayer mobile-first room shell. */",
  line => line === "/* Persistent game category dock; reserve space so it never covers the tools. */",
]);

const PRODUCT_CARD_MARKER = "<!-- product-cards:generated-from-product-catalog -->";

function escapeHtml(value) {
  return String(value)
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;");
}

function gameTitleMarkup(title) {
  const value = String(title);
  if (value.startsWith("東方") && value.length > 2) {
    return `<span>東方</span><wbr><span>${escapeHtml(value.slice(2))}</span>`;
  }
  return `<span>${escapeHtml(value)}</span>`;
}

function generateProductCards({ PRODUCT_GAMES, PRODUCT_IDS, gameIdForProduct, isMultiplayerProductId }) {
  const cards = PRODUCT_IDS.map((productId, index) => {
    const game = gameIdForProduct(productId);
    const product = PRODUCT_GAMES[game];
    if (!product) throw new Error(`product ${productId} resolves to unknown game ${game}`);
    const multiplayer = isMultiplayerProductId(productId);
    const classes = ["game", `game-${game}`];
    if (multiplayer) classes.push(`game-${productId}`, "game-multiplayer");
    const artwork = `assets/${product.cardArtwork}`;
    const presentation = product.cardPresentation || null;
    const presentationStyle = presentation
      ? ` style="--art-position:${presentation.positionPercent}%;--card-art-brightness:${presentation.artBrightness};--card-art-saturation:${presentation.artSaturation};--card-glow-brightness:${presentation.glowBrightness};--card-glow-saturation:${presentation.glowSaturation}"` : "";
    const primaryImageHints = index === 0 ? ' width="640" height="480" fetchpriority="high"' : "";
    const productAttribute = multiplayer ? ` data-product="${escapeHtml(productId)}"` : "";
    const badge = multiplayer ? '\n        <span class="mp-card-mark" aria-hidden="true">MULTIPLAYER</span>' : "";
    const number = `${escapeHtml(product.number)}${multiplayer ? '<span class="no-mp">MP</span>' : ""}`;
    const titleBadge = multiplayer ? ' <span class="mp-game-title-badge" aria-hidden="true">MULTIPLAYER</span>' : "";
    const railSubtitle = multiplayer ? "Multiplayer" : escapeHtml(product.subtitle);
    return `      <a class="${classes.join(" ")}" data-game="${escapeHtml(game)}"${productAttribute} href="?game=${escapeHtml(productId)}"${presentationStyle}>
        <span class="card-art" aria-hidden="true"><img class="card-art-image" src="${escapeHtml(artwork)}" alt=""${primaryImageHints}><span class="card-art-shade"></span></span>
        <span class="card-glow" aria-hidden="true"><img class="card-art-image" src="${escapeHtml(artwork)}" alt=""></span>${badge}
        <div class="no"><span class="no-label">${number}</span></div>
        <div class="game-copy"><h2><span class="game-title">${gameTitleMarkup(product.title)}</span>${titleBadge}</h2><small>${escapeHtml(product.subtitle)}</small>
        </div>
        <span class="rail-title"><strong>${escapeHtml(product.title)}</strong> ~ ${railSubtitle}</span>
      </a>`;
  });
  return `<div class="game-library">${[false, true].map(multiplayer => {
    const id = multiplayer ? "multiplayer" : "singleplayer";
    const label = multiplayer ? "联机" : "单机";
    const shelfProducts = PRODUCT_IDS.filter(productId => isMultiplayerProductId(productId) === multiplayer);
    return `<section class="game-shelf" data-shelf="${id}" aria-labelledby="${id}Heading">
      <header class="shelf-heading">
        <h2 id="${id}Heading" data-i18n="library.${id}">${label}</h2>
        <span class="shelf-caption" data-i18n="library.${id}Hint">${multiplayer ? "与朋友一起，进入幻想乡" : "选择一部作品，开始你的旅程"}</span>
      </header>
      <div class="game-rail" id="${id}Rail" role="group" aria-labelledby="${id}Heading">${cards.filter((_, index) => isMultiplayerProductId(PRODUCT_IDS[index]) === multiplayer).join("\n")}</div>
      <div class="shelf-minimap">
        <div class="minimap-dock">${shelfProducts.map(productId => {
          const product = PRODUCT_GAMES[gameIdForProduct(productId)];
          return `<button class="minimap-toggle" type="button" data-minimap-preview="${escapeHtml(productId)}" aria-label="${escapeHtml(product.title)}" aria-controls="${id}Rail" aria-describedby="${id}MinimapLabel"><span class="minimap-index" aria-hidden="true">${escapeHtml(product.number)}</span></button>`;
        }).join("")}</div><span class="sr-only" id="${id}MinimapLabel" data-i18n="library.holdNav">长按选作</span>
      </div>
    </section>`;
  }).join("\n")}</div>`;
}

function splitFeatureStyles(source) {
  const shell = [];
  const features = [];
  let feature = false;
  let starts = 0;
  let ends = 0;
  for (const line of source.split(/\r?\n/)) {
    if (!feature && FEATURE_STYLE_STARTS.some(match => match(line))) {
      feature = true;
      starts++;
    } else if (feature && FEATURE_STYLE_ENDS.some(match => match(line))) {
      feature = false;
      ends++;
    }
    (feature ? features : shell).push(line);
  }
  // The last feature section intentionally runs to EOF (the in-game restart
  // control). Other boundaries must remain paired so a harmless stylesheet
  // edit cannot silently move the whole shell into the deferred file.
  if (starts !== FEATURE_STYLE_STARTS.length || ends !== FEATURE_STYLE_ENDS.length || !feature) {
    throw new Error(`Launcher CSS split markers are stale (starts=${starts}, ends=${ends}, finalFeature=${feature})`);
  }
  return Object.freeze({ shell: shell.join("\n"), features: features.join("\n") });
}

// Maintainer-only tools are loaded only when TypeScript sources are present.
// Self-host distributions consume the already generated artifacts.
export async function optimizeLauncher({ project, buildRoot, optimizedRoot }) {
  const { build } = await import("esbuild");
  const { parse, serialize } = await import("parse5");
  const buildAtomic = async options => {
    const result = await build({ ...options, write: false });
    for (const output of result.outputFiles || []) await writeFileAtomic(output.path, output.contents);
  };
  // The schema has a classic-worker build as well as its ordinary ESM build.
  // Self-host distributions ship this artifact; operators never need esbuild.
  await buildAtomic({ entryPoints: [resolve(project, "src/contracts/runtime-generations.mts")],
    outfile: resolve(buildRoot, "assets/contracts/runtime-generations-worker.js"),
    bundle: true, format: "iife", globalName: "EaglerRuntimeGenerations", target: "es2022", logLevel: "warning" });
  await buildAtomic({
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
  const splitStyles = splitFeatureStyles(styles);
  await buildAtomic({ stdin: { contents: `${splitFonts}\n${splitStyles.shell}`, resolveDir: resolve(project, "public"), sourcefile: "styles.css", loader: "css" },
    outfile: resolve(optimizedRoot, "styles.css"), bundle: true, minify: true,
    external: ["*.woff2", "*.webp", "*.png", "*.svg", "*.jpg"], logLevel: "warning" });
  await buildAtomic({ stdin: { contents: splitStyles.features, resolveDir: resolve(project, "public"), sourcefile: "features.css", loader: "css" },
    outfile: resolve(optimizedRoot, "features.css"), bundle: true, minify: true,
    external: ["*.woff2", "*.webp", "*.png", "*.svg", "*.jpg"], logLevel: "warning" });
  await buildAtomic({ entryPoints: [resolve(project, "public/touch-guide.css")], outfile: resolve(optimizedRoot, "touch-guide.css"),
    bundle: true, minify: true, external: ["*.woff2", "*.webp", "*.png", "*.svg", "*.jpg"], logLevel: "warning" });
  const { UI_MESSAGES } = await import(pathToFileURL(resolve(buildRoot, "assets/launcher/i18n.mjs")).href);
  const productCatalog = await import(pathToFileURL(resolve(buildRoot, "assets/contracts/product-catalog.mjs")).href);
  const authoredSource = await readFile(resolve(project, "public/index.html"), "utf8");
  if (!authoredSource.includes(PRODUCT_CARD_MARKER)) {
    throw new Error("Launcher product-card generation marker is missing");
  }
  const source = authoredSource.replace(PRODUCT_CARD_MARKER, generateProductCards(productCatalog));
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
    await writeFileAtomic(resolve(optimizedRoot, locale === "en" ? "en.html" : "index.html"), serialize(document));
  }
}
