import { execFileSync } from "node:child_process";
import { existsSync } from "node:fs";
import { readdir, stat, readFile } from "node:fs/promises";
import { extname, relative, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const args = process.argv.slice(2);
const directoryArgument = args.find(value => value.startsWith("--directory="));
const workspaceAudit = args.includes("--workspace");
if (args.some(value => value !== "--workspace" && !value.startsWith("--directory=")) ||
    args.filter(value => value.startsWith("--directory=")).length > 1 ||
    (directoryArgument && workspaceAudit)) {
  throw new Error("usage: node scripts/audit-publication.mjs [--workspace | --directory=SOURCE_EXPORT]");
}
const project = directoryArgument ? resolve(directoryArgument.slice("--directory=".length))
  : resolve(fileURLToPath(new URL("..", import.meta.url)));
const forbiddenExtensions = new Set([".dat", ".data", ".wav", ".ogg", ".mid", ".midi", ".rpy", ".ttc"]);
// Authored quick-chat voice clips are the only wav the repository publishes.
// They are reviewed, small, and ship with the Launcher; every other wav stays
// forbidden so retail music/data cannot be committed by accident.
const quickChatVoiceClipAssets = new Set([
  "1", "request-life", "request-power", "share-resources", "follow-me", "spread-out",
  "stop-fire", "bomb-me", "bomb-you", "leaving", "last-game", "last", "thanks", "xxzj",
].map(id => `assets/quick-chat/${id}.wav`));
const publicAssets = new Set([
  "assets/launcher-background.webp",
  ...quickChatVoiceClipAssets,
  "assets/donation.webp",
  "assets/touch-rotate-landscape.webp",
  "assets/notice-bilibili.svg",
  "assets/notice-touhou-cloud.png",
  "assets/notice-github.svg", "assets/notice-qq.svg",
  // Repository-owned Phosphor room controls; the MIT notice ships with them.
  "assets/room-caret-left.svg", "assets/room-caret-right.svg",
  "assets/room-chart-bar.svg", "assets/room-copy.svg",
  "assets/room-gear-six.svg", "assets/room-sliders-horizontal.svg",
  "assets/room-users.svg", "assets/room-icons-LICENSE.txt",
  // Player/Replay controls from the same reviewed Phosphor collection.
  "assets/room-user.svg", "assets/player-pause.svg", "assets/player-play.svg",
  "assets/player-live.svg", "assets/player-chat.svg", "assets/replay-arrow-left.svg",
  "assets/replay-arrow-counter-clockwise.svg", "assets/replay-file-text.svg",
  "assets/replay-folder.svg", "assets/replay-upload-simple.svg", "assets/replay-dots-three.svg",
  "assets/LICENSE-Phosphor.txt", "assets/phosphor-ui-icons.json",
  "assets/fonts/touhou98.woff2",
  "assets/fonts/unifont-site.woff2",
  "assets/fonts/OFL-Unifont.txt",
  "assets/fonts/yatra-one-latin.woff2", "assets/fonts/chill-round-gothic-site-medium.woff2", "assets/fonts/chill-round-gothic-site-bold.woff2", "assets/fonts/chill-round-gothic-site-heavy.woff2",
  "assets/fonts/chill-round-gothic-site-medium-critical.woff2", "assets/fonts/chill-round-gothic-site-medium-deferred.woff2",
  "assets/fonts/chill-round-gothic-site-bold-critical.woff2", "assets/fonts/chill-round-gothic-site-bold-deferred.woff2",
  "assets/fonts/OFL-YatraOne.txt", "assets/fonts/OFL-ChillRoundGothic.txt",
  "assets/fonts/NotoSansCJKsc-Regular.otf", "assets/fonts/OFL-NotoSansCJK.txt"
]);
const legacyHostGeneratedOriginalAssets = new Set([
  // Legacy/intermediate names remain forbidden too. They are useful as a
  // guard against accidentally copying extraction staging back into source.
  "assets/th06-title00.jpg",
  "assets/th07-title00.jpg",
  "assets/th08-title00.png",
  "assets/pwa/icon-192.png",
  "assets/pwa/icon-512.png",
  "assets/pwa/icon-maskable-512.png",
  "assets/pwa/apple-touch-icon.png",
]);
function isHostGeneratedOriginalAsset(path) {
  // Card artwork is host-generated from retail content for every formal title.
  // Keep this rule product-neutral so adding a title does not require teaching
  // the publication safety audit another game id.
  return /^assets\/th\d{2}-card\.webp$/i.test(path) || legacyHostGeneratedOriginalAssets.has(path);
}
const failures = [];
let workspaceChecked = null;
function isGitWorkingTree(root) {
  try {
    return execFileSync('git', ['-C', root, 'rev-parse', '--is-inside-work-tree'],
      {encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe']}).trim() === 'true';
  } catch { return false; }
}
if (workspaceAudit) {
  const {loadSourceLayout,projectConfigurationPath}=await import('../lib/source-layout.mjs');
  const {loadReviewedPublicationAssets}=await import('../lib/reviewed-publication-assets.mjs');
  const layout=loadSourceLayout(project),root=layout.root;
  const WORKSPACE_REPOSITORIES=layout.repositories,workspacePath=(...args)=>layout.path(...args);
  const reviewed=await loadReviewedPublicationAssets(root,resolve(root,'docs/migration/reviewed-source-assets.json'));
  // Source publication must also run in a clean checkout before contracts or
  // frontend dependencies have been generated.
  const config=JSON.parse(await readFile(projectConfigurationPath('runtime-builds.json',project),'utf8'));
  if(config.schema!=='eagler-touhou/runtime-builds/1'||!config.games)throw Error('Invalid Runtime build ownership');
  const gameOwners = [...new Set(Object.values(config.games).flatMap(entry =>
    Object.values(entry.variants).map(variant => variant.workspaceRepository ?? entry.workspaceRepository)))];
  gameOwners.push('common');
  workspaceChecked = [
    WORKSPACE_REPOSITORIES.launcher,
    ...gameOwners.map(owner => `${WORKSPACE_REPOSITORIES[owner]} tracked files`),
  ];
  for (const owner of gameOwners) {
    const repo = WORKSPACE_REPOSITORIES[owner];
    const root = workspacePath(owner);
    if (!isGitWorkingTree(root)) throw new Error(`workspace publication audit requires a Git-owned source component: ${repo}`);
    const tracked = execFileSync("git", ["-C", root, "ls-files", "-z"], { encoding: "utf8" }).split("\0").filter(Boolean);
    for (const file of tracked) {
      if (forbiddenExtensions.has(extname(file).toLowerCase())) failures.push(`${repo}/${file}`);
      else if (/(^|\/)assets(?:-ogg)?\//i.test(file)&&!await reviewed(`${repo}/${file}`)) failures.push(`${repo}/${file} (unreviewed or changed source asset/evidence)`);
    }
  }
}
async function inspect(path) {
  const rel = relative(project, path).replaceAll("\\", "/");
  const publicRel = rel.startsWith("public/") ? rel.slice("public/".length) : rel;
  if (isHostGeneratedOriginalAsset(publicRel)) failures.push(`eagler-touhou/${rel} (original-game-derived host asset must not be source-published)`);
  else if (publicRel.startsWith("assets/") && !publicAssets.has(publicRel)) failures.push(`eagler-touhou/${rel} (unreviewed public asset)`);
  if (forbiddenExtensions.has(extname(rel).toLowerCase()) && !quickChatVoiceClipAssets.has(publicRel)) failures.push(`eagler-touhou/${rel}`);
  if ((await stat(path)).size > 50 * 1024 * 1024) failures.push(`eagler-touhou/${rel} (>50 MiB)`);
}
async function walk(directory) {
  for (const entry of await readdir(directory, { withFileTypes: true })) {
    if (["node_modules", ".cache", ".npm-cache", ".deploy-python", "design", "screenshots"].includes(entry.name)) continue;
    const path = resolve(directory, entry.name);
    if (entry.isDirectory()) await walk(path);
    else await inspect(path);
  }
}
if (!directoryArgument && isGitWorkingTree(project)) {
  // Source publication candidates are tracked + nonignored new source, not
  // private local validation outputs. Force-added ignored assets remain tracked
  // and therefore still fail. A concrete export is audited with --directory.
  const files = new Set(execFileSync("git", ["-C", project, "ls-files", "--cached", "--others", "--exclude-standard", "-z"],
    {encoding:"utf8"}).split("\0").filter(Boolean));
  for (const asset of publicAssets) {
    const sourcePath = `public/${asset}`;
    if (existsSync(resolve(project, sourcePath))) files.add(sourcePath);
  }
  for (const file of files) if (existsSync(resolve(project, file)) && (await stat(resolve(project, file))).isFile()) {
    await inspect(resolve(project, file));
  }
} else await walk(project);
if (failures.length) throw new Error(`publication contains private/generated resources:\n${failures.join("\n")}`);
console.log(JSON.stringify({ safe: true, scope: directoryArgument ? "explicit-source-export" : "git-source-candidates",
  checked: workspaceChecked || ["eagler-touhou"] }));
