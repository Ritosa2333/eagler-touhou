import { readFileSync, writeFileSync } from "node:fs";
import { resolve } from "node:path";
import { createHash } from "node:crypto";
import { execFileSync } from "node:child_process";

const [checkout, ...options] = process.argv.slice(2);
if (!checkout) throw Error("Usage: node portable/generate-thprac.mjs <thprac_purple> [--check]");
const read = (path) => readFileSync(resolve(checkout, path), "utf8").replace(/^\uFEFF/, "").replaceAll("\r\n", "\n");
const dir = "thprac/src/thprac/";
const source = read(dir + "thprac_th17.cpp");
const definitions = JSON.parse(read(dir + "thprac_games_def.json"));
const revision = execFileSync("git", ["-C", checkout, "log", "-1", "--format=%h", "--", dir + "thprac_th17.cpp", dir + "thprac_games_def.json"], { encoding: "utf8" }).trim();
const digest = createHash("sha256").update(source).digest("hex");
const version = Array.from({ length: 4 }, (_, i) => read(dir + "thprac_version.h").match(new RegExp("#define THPRAC_VERSION_" + i + " (\\d+)"))[1]).join(".");
const banner = "// Generated from rueee/thprac master_2, " + revision + " (MIT).\n// TH17 source SHA-256 " + digest + "\n";
function between(first, last) {
    const start = source.indexOf(first);
    const end = source.indexOf(last, start + first.length);
    if (start < 0 || end < 0) throw Error("Upstream source boundary changed: " + first);
    return source.slice(start, end);
}
let patches = between("    void ECLStdExec(", "    __declspec(noinline) void THSectionPatch()")
    .replaceAll("__declspec(noinline) ", "")
    .replaceAll("THPrac::TH17::", "")
    .replaceAll("th_sections_t", "i32")
    .replaceAll("thPracParam.dlg", "thPracParam.dialogue")
    .replaceAll("thPracParam.keepSpellDrops", "thPracParam.keep_drops")
    .replaceAll("thPracParam.keikiFinalPhaseOfs", "thPracParam.keiki_offsets")
    .replace("void THPatchSP(ECLHelper& ecl)\n    {\n        uint32_t spell_id = *(int32_t*)(0x4b5a14);", "void THPatchSP(ECLHelper& ecl, i32 spell_id)\n    {")
    .replace("GetMemContent(DIFFICULTY)", "difficulty")
    .replace("else\n            start = ecl.GetPos();", "else\n            start = ecl.GetPos();\n        ecl.Jump(start, dest);");
if (/GetMem|__declspec|0x4b5a14|THPrac::/.test(patches)) throw Error("Unmapped TH17 platform dependency");
const glossary = Object.assign({}, ...Object.values(definitions).map((game) => game.glossary));
const groups = Object.assign({}, ...Object.values(definitions).map((game) => game.groups));
const entries = Object.entries(definitions.th17.sections);
const cases = (body) => [...body.matchAll(/case (TH17_\w+):/g)].map((match) => match[1]);
const dialogue = cases(between("        bool SectionHasDlg(", "        bool SectionHasSpellDrops("));
const dropsBody = between("        bool SectionHasSpellDrops(", "        void SectionWidget(");
const drops = new Set(cases(dropsBody));
for (const [, first, last] of dropsBody.matchAll(/section >= (\w+) && section <= (\w+)/g)) {
    const keys = entries.map(([key]) => key);
    for (const key of keys.slice(keys.indexOf(first), keys.indexOf(last) + 1)) drops.add(key);
}
const tuple = (labels) => "{" + labels.map((label) => JSON.stringify(label)).join(", ") + "}";
const names = (value, rank) => {
    const label = Object.entries(value).find(([key]) => key.startsWith("!") && key.includes("ENHLX"[rank]))?.[1];
    if (label === undefined) return tuple(["", "", ""]);
    if (Array.isArray(label)) return tuple(label);
    if (!glossary[label]) throw Error("Unknown TH17 glossary label " + label);
    return tuple(glossary[label]);
};
let catalog = "#pragma once\nnamespace th17 {\nenum PracticeSection {\n";
catalog += entries.map(([key], index) => "    " + key + " = " + (index + 1) + ",\n").join("");
catalog += "};\nstruct PracticeSectionInfo {\n    int id, stage, group, order, bgm;\n    bool spell, dialogue, drops;\n    const char* names[5][3];\n};\ninline constexpr PracticeSectionInfo practice_sections[] = {\n";
for (const [key, value] of entries) {
    const [stage, group, order] = value.appearance;
    catalog += "    {" + [key, stage, group, order, value.bgm || 0, !!value.spell, dialogue.includes(key), drops.has(key)].join(", ") + ", {\n        ";
    catalog += Array.from({ length: 5 }, (_, rank) => names(value, rank)).join(", ") + "}},\n";
}
catalog += "};\ninline constexpr const char* practice_version = " + JSON.stringify(version) + ";\n";
for (const key of ["TH17_GOAST_SELECT", "TH17_GOAST_SELECT_TOKENS", "TH_SPELL_PHASE2", "TH17_SPELL_5PHASE", "TH_WARP_SELECT", "IGI_DIFF"]) {
    if (!groups[key]) throw Error("Unknown practice group " + key);
    catalog += "inline constexpr const char* practice_" + key + "[][3] = {\n" +
        groups[key].map((label) => "    " + tuple(glossary[label] || label) + ",\n").join("") + "};\n";
}
catalog += "}\n";
const license = read("LICENCE");
for (const [name, content] of Object.entries({
    "PracticePatches.inc": banner + patches,
    "PracticeCatalog.hpp": banner + catalog,
    "PracticeLicense.hpp": banner + "#pragma once\nnamespace th17 {\ninline constexpr char practice_license[] = R\"MIT(" + license + ")MIT\";\n}\n",
})) {
    const file = resolve(import.meta.dirname, "../web/cpp/game", name);
    if (options.includes("--check")) {
        if (readFileSync(file, "utf8").replaceAll("\r\n", "\n") !== content) throw Error(name + " differs from upstream");
    } else {
        writeFileSync(file, content);
    }
}
