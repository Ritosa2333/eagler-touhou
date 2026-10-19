// Local build-plan acceptance with synthetic source/SDK markers. No compiler or
// game data is executed. Covers all registered normal/multiplayer source owners.
import assert from "node:assert/strict";
import { mkdir, mkdtemp, rm, writeFile, readdir } from "node:fs/promises";
import { tmpdir } from "node:os";
import { dirname, resolve } from "node:path";
import { loadRuntimeBuildConfig } from "../lib/runtime-build-profiles.mjs";
import { WORKSPACE_REPOSITORIES } from "../lib/workspace-layout.mjs";
import { loadSourceLayout } from "../lib/source-layout.mjs";
import { planRuntimeSourceBuild } from "../lib/runtime-source-plan.mjs";

const root = await mkdtemp(resolve(tmpdir(), "eagler-runtime-plans-"));
async function put(path, text = "fixture") {
  await mkdir(dirname(path), { recursive: true });
  await writeFile(path, text);
}
try {
  const web = resolve(root, "launcher/web");
  await mkdir(web, { recursive: true });
  await put(resolve(root, "config/workspace.json"), JSON.stringify({ schema: "eagler-touhou/workspace-layout/1",
    repositories: { ...WORKSPACE_REPOSITORIES, launcher: "launcher/web" } }));
  const layout = loadSourceLayout(web, { environment: {} }), config = await loadRuntimeBuildConfig();
  const sdk = resolve(root, "sdk with spaces");
  await put(resolve(sdk, "install/emscripten/emcc.py"));
  await put(resolve(sdk, "install/emscripten/emscripten-version.txt"), "6.0.9-git\n");
  await put(resolve(sdk, "install/emscripten/cmake/Modules/Platform/Emscripten.cmake"));
  await put(resolve(layout.path("common"), "include/eagler/input/DialogueGesture.hpp"));
  await put(resolve(layout.path("common"), "include/eagler/audio/WebSfx.hpp"));
  await put(resolve(layout.path("common"), "include/eagler/netplay/SessionChannel.hpp"));
  await put(resolve(layout.path("common"), "include/eagler/text/RuntimeFontRaster.hpp"));
  await put(resolve(layout.path("dependencies"), "unifont-15.1.05/unifont-15.1.05.otf"));
  const context = { layout, config, environment: {} };
  const plans = [];
  for (const [game, entry] of Object.entries(config.games)) {
    for (const [variant, definition] of Object.entries(entry.variants)) {
      if (!["normal", "multiplayer"].includes(variant)) continue;
      const source = layout.path(definition.workspaceRepository ?? entry.workspaceRepository);
      const recipe = { ...entry.sourceBuild, ...definition.sourceBuild };
      if (!recipe.kind) {
        // Prebuilt-only products do not yet declare a source builder.
        await assert.rejects(() => planRuntimeSourceBuild({ game, variant, emsdk: sdk }, context),
          /source build is not declared/);
        continue;
      }
      await put(resolve(source, recipe.kind === "cmake" ? "CMakeLists.txt" : recipe.build[0]));
      const before = await readdir(source);
      const plan = await planRuntimeSourceBuild({ game, variant, emsdk: sdk }, context);
      assert.deepEqual(await readdir(source), before, "planning must not create build output");
      assert.equal(plan.common, layout.path("common"), "a monorepo resolves its declared common directory");
      assert.equal(plan.environment.EMSDK, sdk);
      assert.equal(plan.environment.TH09_EMSDK, sdk);
      assert.equal(plan.environment.TH15_EMSDK, sdk);
      assert.equal(plan.environment.EM_CONFIG, resolve(sdk, ".emscripten"));
      assert.equal(plan.toolchain.version, "6.0.9-git");
      assert.equal(plan.steps[0].cwd, source);
      if (recipe.kind === "cmake") {
        assert(plan.steps[0].args.includes("-DTH_EXTERNAL_ASSETS=ON"));
        assert(plan.steps[0].args.includes(`-DEAGLER_COMMON_ROOT=${layout.path("common")}`));
        assert(plan.steps[0].args.includes(`-B`));
      } else assert.equal(plan.steps[0].args[0], resolve(source, recipe.build[0]));
      plans.push(plan);
    }
  }
  assert.equal(plans.length, 17);
  assert.equal(plans.find(plan => plan.game === "th10" && plan.variant === "multiplayer").source, layout.path("th10mp"));
  assert(plans.find(plan => plan.game === "th11" && plan.variant === "multiplayer").steps[0].args.includes("--multiplayer"));
  const th15 = plans.find(plan => plan.game === "th15" && plan.variant === "multiplayer");
  assert.equal(th15.source, layout.path("th15mp"));
  assert(th15.steps[0].args.includes("--multiplayer"));
  assert.equal(th15.environment.EAGLER_COMMON_NETPLAY_ROOT, layout.path("common"));
  const th20 = plans.find(plan => plan.game === "th20");
  assert.equal(th20.features.thcrap, false);
  assert.equal(th20.features.thprac, false);
  const pinned = resolve(layout.path("th09"), "third_party/eagler-common");
  await put(resolve(pinned, "include/eagler/input/DialogueGesture.hpp"));
  assert.equal((await planRuntimeSourceBuild({ game: "th09", emsdk: sdk }, context)).common, layout.path("common"),
    "a stale per-title copy must not override the single workspace common source");
  const overridden = await planRuntimeSourceBuild({ game: "th09", emsdk: sdk, common: layout.path("common"), jobs: 2, python: "custom-python" }, context);
  assert.equal(overridden.common, layout.path("common"));
  assert.equal(overridden.environment.TH_PYTHON, "custom-python");
  assert.equal(overridden.environment.TH_BUILD_JOBS, "2");
  const incompleteCommon=resolve(root,"incomplete-font-common");
  await put(resolve(incompleteCommon,"include/eagler/input/DialogueGesture.hpp"));
  await assert.rejects(()=>planRuntimeSourceBuild({game:"th15",emsdk:sdk,common:incompleteCommon},context),/RuntimeFontRaster.hpp/);
  await assert.rejects(() => planRuntimeSourceBuild({ game: "th09", emsdk: resolve(root, "missing-sdk") }, context), /Emscripten compiler not found/);
  await assert.rejects(() => planRuntimeSourceBuild({ game: "th09" }, context), /specify --emsdk/);
  await assert.rejects(() => planRuntimeSourceBuild({ game: "th09", emsdk: sdk, emscriptenVersion: "6.0.5" }, context), /version mismatch/);
  for (const version of ['6.0.9', '6.0.9-git', '"6.0.9"', '"6.0.9-git"']) {
    await put(resolve(sdk, "install/emscripten/emscripten-version.txt"), version+'\n');
    const plan=await planRuntimeSourceBuild({game:"th09",emsdk:sdk,emscriptenVersion:"6.0.9"},context);
    assert.equal(plan.toolchain.version, version.replaceAll('"',''));
    await assert.rejects(()=>planRuntimeSourceBuild({game:"th09",emsdk:sdk,emscriptenVersion:"6.0.8"},context),/version mismatch/);
  }
  await assert.rejects(() => planRuntimeSourceBuild({ game: "th09", emsdk: sdk, common: resolve(root, "missing-common") }, context), /common dependency is missing/);
  await assert.rejects(() => planRuntimeSourceBuild({ game: "th15", variant: "multiplayer", emsdk: sdk, thprac: true }, context), /fixed to false/);
  await assert.rejects(() => planRuntimeSourceBuild({ game: "th09", emsdk: sdk, jobs: 0 }, context), /jobs must be/);
  await assert.rejects(() => planRuntimeSourceBuild({ game: "th20", variant: "multiplayer", emsdk: sdk }, context), /unknown Runtime build/);
} finally {
  assert.equal(dirname(root), resolve(tmpdir()));
  await rm(root, { recursive: true, force: true });
}
console.log("Runtime source plans: PASS (17 variants, shared dependencies, explicit tools, invalid inputs)");
