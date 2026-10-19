// Exercise the upstream zlib configure side effect with isolated Git/CMake
// fixtures. The direct-source control reproduces the rename; staging keeps the
// tracked source in place and excludes ignored files from the build tree.
import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { mkdir, mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { tmpdir } from "node:os";

const workspace = resolve(fileURLToPath(new URL("../..", import.meta.url)));
const helper = resolve(workspace, "common/cmake/EaglerStageSdlImage.cmake");
const tempRoot = await mkdtemp(resolve(tmpdir(), "eagler-cmake-stage-"));
const cmake = process.env.EAGLER_TEST_CMAKE ?? "cmake";

async function put(path, content) {
  await mkdir(dirname(path), { recursive: true });
  await writeFile(path, content);
}

function run(command, args, cwd) {
  return execFileSync(command, args, { cwd, encoding: "utf8", windowsHide: true, stdio: ["ignore", "pipe", "pipe"] });
}

function git(cwd, ...args) {
  run("git", args, cwd);
}

async function moduleRepo(directory, files) {
  await mkdir(directory, { recursive: true });
  git(directory, "init", "--quiet");
  git(directory, "config", "user.name", "CMake stage test");
  git(directory, "config", "user.email", "cmake-stage@example.invalid");
  for (const [name, contents] of Object.entries(files)) await put(resolve(directory, name), contents);
  git(directory, "add", "--all");
  git(directory, "commit", "--quiet", "-m", "fixture source");
}

async function fixture(name) {
  const source = resolve(tempRoot, name, "SDL_image");
  await moduleRepo(source, {
    "CMakeLists.txt": `cmake_minimum_required(VERSION 3.20)\nproject(SDL_image_fixture NONE)\nadd_subdirectory(external/zlib external/zlib-build)\n`,
    "cmake/config.cmake": "set(SDLIMAGE_FIXTURE 1)\n",
    "include/SDL3_image/SDL_image.h": "#pragma once\n",
    "src/IMG_stb.c": "int sdl_image_fixture;\n",
    "VisualC/external/optional.dll": "unused platform payload\n",
  });
  await moduleRepo(resolve(source, "external/zlib"), {
    ".gitignore": "ignored-build-input.txt\n",
    "CMakeLists.txt": `cmake_minimum_required(VERSION 3.20)\nproject(zlib_fixture NONE)\nif(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR AND EXISTS \"${"\${CMAKE_CURRENT_SOURCE_DIR}"}/zconf.h\")\n  file(RENAME \"${"\${CMAKE_CURRENT_SOURCE_DIR}"}/zconf.h\" \"${"\${CMAKE_CURRENT_SOURCE_DIR}"}/zconf.h.included\")\nendif()\n`,
    "zconf.h": "/* pinned generated-header template */\n",
    "zlib.h": "/* zlib fixture */\n",
  });
  await moduleRepo(resolve(source, "external/libpng"), {
    "CMakeLists.txt": "cmake_minimum_required(VERSION 3.20)\nproject(libpng_fixture NONE)\n",
    "png.h": "/* libpng fixture */\n",
  });
  await put(resolve(source, "external/zlib/ignored-build-input.txt"), "must not be staged\n");
  return source;
}

try {
  run(cmake, ["--version"], tempRoot);

  const directSource = await fixture("direct");
  const directProject = resolve(tempRoot, "direct-project");
  await put(resolve(directProject, "CMakeLists.txt"), `cmake_minimum_required(VERSION 3.20)\nproject(direct_control NONE)\nadd_subdirectory(\"${"\${FIXTURE_SDLIMAGE}"}\" \"${"\${CMAKE_BINARY_DIR}"}/SDL_image\")\n`);
  run(cmake, ["-S", directProject, "-B", resolve(tempRoot, "direct-build"), `-DFIXTURE_SDLIMAGE=${directSource}`], tempRoot);
  assert.equal(await readFile(resolve(directSource, "external/zlib/zconf.h.included"), "utf8"),
    "/* pinned generated-header template */\n", "direct CMake configuration should reproduce SDL_image's source-tree rename");
  await assert.rejects(readFile(resolve(directSource, "external/zlib/zconf.h")), { code: "ENOENT" });

  const stagedSource = await fixture("staged");
  const stagedProject = resolve(tempRoot, "staged-project");
  await put(resolve(stagedProject, "CMakeLists.txt"), `cmake_minimum_required(VERSION 3.20)\nproject(staged_control NONE)\ninclude(\"${helper.replaceAll("\\", "/")}\")\neagler_stage_sdl_image(STAGED_SOURCE \"${"\${FIXTURE_SDLIMAGE}"}\")\nfile(WRITE \"${"\${CMAKE_BINARY_DIR}"}/stage-path.txt\" \"${"\${STAGED_SOURCE}"}\")\nadd_subdirectory(\"${"\${STAGED_SOURCE}"}\" \"${"\${CMAKE_BINARY_DIR}"}/SDL_image\")\n`);
  const stagedBuild = resolve(tempRoot, "staged-build");
  const configure = () => run(cmake, ["-S", stagedProject, "-B", stagedBuild, `-DFIXTURE_SDLIMAGE=${stagedSource}`], tempRoot);
  configure();
  const stage = (await readFile(resolve(stagedBuild, "stage-path.txt"), "utf8")).trim();
  assert.equal(await readFile(resolve(stagedSource, "external/zlib/zconf.h"), "utf8"),
    "/* pinned generated-header template */\n", "staging must preserve the pinned source header");
  await assert.rejects(readFile(resolve(stagedSource, "external/zlib/zconf.h.included")), { code: "ENOENT" });
  await assert.rejects(readFile(resolve(stage, "external/zlib/zconf.h")), { code: "ENOENT" },
    "the upstream-generated header is intentionally omitted from the staged source");
  await assert.rejects(readFile(resolve(stage, "external/zlib/zconf.h.included")), { code: "ENOENT" });
  await assert.rejects(readFile(resolve(stage, "external/zlib/ignored-build-input.txt")), { code: "ENOENT" },
    "ignored files must not enter the staged dependency closure");
  await assert.rejects(readFile(resolve(stage, "VisualC/external/optional.dll")), { code: "ENOENT" },
    "unused platform payloads must not be duplicated into the CMake stage");
  assert.equal(await readFile(resolve(stage, "src/IMG_stb.c"), "utf8"), "int sdl_image_fixture;\n");

  configure();
  assert.equal((await readFile(resolve(stagedBuild, "stage-path.txt"), "utf8")).trim(), stage,
    "unchanged source inputs should reuse the same content-addressed stage");
  assert.equal(await readFile(resolve(stagedSource, "external/zlib/zconf.h"), "utf8"),
    "/* pinned generated-header template */\n", "reconfiguration must leave the source header unchanged");

  await put(resolve(stagedSource, "src/IMG_stb.c"), "int changed_fixture_source;\n");
  configure();
  const changedStage = (await readFile(resolve(stagedBuild, "stage-path.txt"), "utf8")).trim();
  assert.notEqual(changedStage, stage, "changed tracked bytes must select a fresh stage");
  assert.equal(await readFile(resolve(changedStage, "src/IMG_stb.c"), "utf8"), "int changed_fixture_source;\n");

  await put(resolve(stagedSource, "src/IMG_added.c"), "int added_fixture_source;\n");
  git(stagedSource, "add", "src/IMG_added.c");
  configure();
  const addedStage = (await readFile(resolve(stagedBuild, "stage-path.txt"), "utf8")).trim();
  assert.notEqual(addedStage, changedStage, "adding a tracked input must select a fresh stage");
  assert.equal(await readFile(resolve(addedStage, "src/IMG_added.c"), "utf8"), "int added_fixture_source;\n");

  git(stagedSource, "rm", "--quiet", "--force", "src/IMG_added.c");
  configure();
  const removedStage = (await readFile(resolve(stagedBuild, "stage-path.txt"), "utf8")).trim();
  assert.notEqual(removedStage, addedStage, "removing a tracked input must select a fresh stage");
  await assert.rejects(readFile(resolve(removedStage, "src/IMG_added.c")), { code: "ENOENT" });
  assert.equal(await readFile(resolve(stagedSource, "external/zlib/zconf.h"), "utf8"),
    "/* pinned generated-header template */\n", "changed-input restaging must preserve the pinned source header");
  console.log("CMake SDL_image staging: PASS (source mutation negative control, tracked closure, isolated configure)");
} finally {
  assert.equal(dirname(tempRoot), resolve(tmpdir()), "test cleanup must remain inside the OS temp directory");
  await rm(tempRoot, { recursive: true, force: true });
}
