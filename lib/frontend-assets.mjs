import { readFile } from "node:fs/promises";
import { resolve } from "node:path";

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

/** One canonical production CSS/font partition shared by both UI entries. */
export function launcherStyleSources(authoredStyles, splitFonts) {
  const styles = authoredStyles
    .replace(/@font-face\{font-family:"ET Chill Round";font-style:normal;font-weight:(?:400|700);font-display:swap;src:url\("assets\/fonts\/chill-round-gothic-site-(?:medium|bold)\.woff2"\) format\("woff2"\)\}\r?\n/g, "");
  const splitStyles = splitFeatureStyles(styles);
  return Object.freeze({shell: `${splitFonts}\n${splitStyles.shell}`, features: splitStyles.features});
}

/** Build the original standalone CSS URLs without importing either UI shell. */
export async function buildLauncherStyleOutputs({ project, outputDirectory }) {
  const { build } = await import("esbuild");
  const authoredStyles = await readFile(resolve(project, "public/styles.css"), "utf8");
  const splitFonts = await readFile(resolve(project, "public/ui-fonts.css"), "utf8");
  const sources = launcherStyleSources(authoredStyles, splitFonts);
  const outputs = [];
  for (const [key, filename] of [["shell", "styles.css"], ["features", "features.css"]]) {
    const result = await build({
      stdin: { contents: sources[key], resolveDir: resolve(project, "public"), sourcefile: filename, loader: "css" },
      outfile: resolve(outputDirectory, filename), bundle: true, minify: true, write: false,
      external: ["*.woff2", "*.webp", "*.png", "*.svg", "*.jpg"], logLevel: "warning",
    });
    outputs.push(...result.outputFiles);
  }
  const touch = await build({ entryPoints: [resolve(project, "public/touch-guide.css")], outfile: resolve(outputDirectory, "touch-guide.css"),
    bundle: true, minify: true, write: false, external: ["*.woff2", "*.webp", "*.png", "*.svg", "*.jpg"], logLevel: "warning" });
  outputs.push(...touch.outputFiles);
  return outputs;
}

/** The unchanged classic-worker contract can be compiled independently of
 * the React UI graph. Self-host distributions still ship its compiled artifact. */
export async function buildRuntimeGenerationWorkerSource({ project }) {
  const { build } = await import("esbuild");
  const result = await build({absWorkingDir: project, entryPoints: [resolve(project, "src/contracts/runtime-generations.mts")],
    bundle: true, format: "iife", globalName: "EaglerRuntimeGenerations", target: "es2022", logLevel: "warning", write: false});
  return result.outputFiles[0].text;
}
