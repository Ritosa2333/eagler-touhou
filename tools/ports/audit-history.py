"""Compare directory history patches without changing refs or sync progress."""
import argparse
from collections import Counter, defaultdict
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import re
import subprocess


def git(repo, *args, data=None):
    return subprocess.check_output(["git", "-C", str(repo), *args], input=data)


def read_commit(repo, identity):
    metadata = git(repo, "show", "-s", "--format=%P%x00%s%x00%aI%x00%cI%x00%B", identity).decode("utf-8").split("\0", 4)
    patch = git(repo, "show", "--no-color", "--format=", "--no-ext-diff", "--no-textconv", "--no-renames", "--binary", "--full-index", identity)
    raw = git(repo, "diff-tree", "--no-commit-id", "--raw", "--no-abbrev", "-z", "--root", "--no-renames", "-r", identity).rstrip(b"\0").split(b"\0")
    changes = []
    if raw != [b""]:
        for index in range(0, len(raw), 2):
            old_mode, mode, before, after, status = raw[index].decode().lstrip(":").split()
            changes.append({"path": raw[index+1].decode("utf-8"), "mode": mode, "after": after,
                            "before": before, "oldMode": old_mode, "status": status})
    paths = [change["path"] for change in changes]
    blocks = re.split(rb"(?m)(?=^diff --git )", patch)
    blocks = [block for block in blocks if block.startswith(b"diff --git ")]
    if len(blocks) != len(paths):
        raise RuntimeError("Patch/file count differs: " + identity)
    source = re.search(r"^Monorepo-Source: ([0-9a-f]{40})$", metadata[4], re.M)
    return {"commit": identity, "subject": metadata[1], "source": source.group(1) if source else None,
            "authorDate": metadata[2], "committerDate": metadata[3],
            "files": paths, "changes": changes, "patch": patch, "blocks": blocks}


def read_reference_commit(repo, identity):
    # Older publication commits may include complete source/data snapshots.
    # Pipe their whole patches through Git rather than retaining and duplicating
    # those snapshots for every per-file and per-hunk fingerprint.
    metadata = git(repo, "show", "-s", "--format=%s%x00%aI%x00%cI%x00%B", identity).decode("utf-8").split("\0", 3)
    source = re.search(r"^Monorepo-Source: ([0-9a-f]{40})$", metadata[3], re.M)
    producer = subprocess.Popen(["git", "-C", str(repo), "show", "--no-color", "--no-ext-diff", "--no-textconv", "--no-renames", "--binary", "--full-index", identity], stdout=subprocess.PIPE)
    try:
        output = subprocess.check_output(["git", "-C", str(repo), "patch-id", "--stable"], stdin=producer.stdout)
    finally:
        producer.stdout.close()
        code = producer.wait()
    if code:
        raise RuntimeError("Cannot read normal reference commit: " + identity)
    return {"commit": identity, "subject": metadata[0], "authorDate": metadata[1], "committerDate": metadata[2],
            "source": source.group(1) if source else None, "patchId": output.split()[0].decode() if output.strip() else None,
            "filePatchIds": {}, "hunkPatchIds": {}}


class Snapshot:
    def __init__(self, repo, head):
        self.repo, self.head, self.objects = repo, head, {}
        self.paths = {}
        for row in git(repo, "ls-tree", "-r", "-z", head).rstrip(b"\0").split(b"\0"):
            metadata, path = row.split(b"\t", 1)
            mode, _, identity = metadata.decode().split()
            self.paths[path.decode("utf-8")] = (mode, identity)
        self.reader = subprocess.Popen(["git", "-C", str(repo), "cat-file", "--batch"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)

    def content(self, identity):
        if identity not in self.objects:
            self.reader.stdin.write(identity.encode() + b"\n")
            self.reader.stdin.flush()
            header = self.reader.stdout.readline().split()
            if len(header) != 3:
                raise RuntimeError("Missing snapshot object: " + identity)
            self.objects[identity] = self.reader.stdout.read(int(header[2]))
            if self.reader.stdout.read(1) != b"\n":
                raise RuntimeError("Invalid snapshot object stream")
        return self.objects[identity]

    def close(self):
        self.reader.stdin.close()
        self.reader.stdout.close()
        self.reader.wait()


def canonical_path(path, game):
    prefix = game + "_web/"
    return "web/" + path[len(prefix):] if path.startswith(prefix) else path


def change_segments(block):
    groups = {"+": [], "-": []}
    active, lines, inside = None, [], False

    def flush():
        if active and any(line.strip() for line in lines):
            groups[active].append("\n".join(line.strip() for line in lines if line.strip()))

    for line in block.decode("utf-8", errors="replace").splitlines():
        if line.startswith("@@ "):
            flush()
            active, lines, inside = None, [], True
        elif inside:
            kind = line[:1]
            if kind != active:
                flush()
                active, lines = (kind if kind in groups else None), []
            if active:
                lines.append(line[1:])
    flush()
    return groups


def current_state(records, snapshot, game):
    results = {}
    for record in records:
        rows = []
        destinations = {canonical_path(change["path"], game) for change in record["changes"] if change["mode"] != "000000"}
        for change, block in zip(record["changes"], record["blocks"]):
            path = canonical_path(change["path"], game)
            current = snapshot.paths.get(path)
            row = {"path": change["path"], "currentPath": path}
            if change["mode"] == "000000":
                status = "directory-move" if path in destinations else "deletion-present" if current is None else "deleted-path-retained"
            elif current == (change["mode"], change["after"]):
                status = "postimage-identical"
            elif change["mode"] == "160000":
                status = "historical-dependency-pin" if current is None else "dependency-pin-differs"
            elif current is None:
                status = "path-absent"
            elif current[0] != change["mode"]:
                status = "file-mode-differs"
            else:
                content = snapshot.content(current[1])
                if b"\0" in content or b"GIT binary patch" in block:
                    status = "binary-differs"
                else:
                    normalized = "\n".join(line.strip() for line in content.decode("utf-8", errors="replace").splitlines() if line.strip())
                    groups = change_segments(block)
                    added = [group for group in groups["+"] if group not in normalized]
                    # Short syntax fragments occur throughout a file; preserve
                    # substantial removal evidence for a reviewer instead.
                    removed = [group for group in groups["-"] if len(group) >= 16 and re.search(r"[\w]", group) and group in normalized]
                    row.update({"addedSegments": len(groups["+"]), "missingAddedSegments": len(added),
                                "removedSegmentsStillPresent": len(removed),
                                "missingSamples": [text[:200] for text in added[:3]],
                                "removedSamples": [text[:200] for text in removed[:3]]})
                    status = "text-effect-present" if not added and not removed else "text-differs"
            row["state"] = status
            rows.append(row)
        results[record["commit"]] = {"targetHead": snapshot.head, "counts": dict(Counter(row["state"] for row in rows)), "files": rows}
    return results


def fingerprints(repo, records):
    # Git's stable patch IDs ignore whitespace and hunk line offsets. Keeping
    # paths in file keys prevents identical changes in unrelated files matching.
    stream = []
    lookup = {}
    buffered = 0

    def flush():
        nonlocal buffered
        if not stream:
            return
        output = git(repo, "patch-id", "--stable", data=b"".join(stream))
        for line in output.decode().splitlines():
            patch_id, token = line.split()
            record, kind, path = lookup[token]
            if kind == "commit":
                record["patchId"] = patch_id
            elif kind == "file":
                record["filePatchIds"][path] = patch_id
            else:
                record["hunkPatchIds"][path[0]][path[1]] = patch_id
        stream.clear()
        lookup.clear()
        buffered = 0

    for record in records:
        record["filePatchIds"] = {}
        record["patchId"] = None
        chunks = [("commit", None, record["patch"])]
        chunks += [("file", path, patch) for path, patch in zip(record["files"], record["blocks"])]
        record["hunkPatchIds"] = {}
        for path, block in zip(record["files"], record["blocks"]):
            positions = list(re.finditer(rb"(?m)^@@ .*@@[^\n]*\n", block))
            if positions:
                header = block[:positions[0].start()]
                record["hunkPatchIds"][path] = [None] * len(positions)
                for index, position in enumerate(positions):
                    end = positions[index+1].start() if index+1 < len(positions) else len(block)
                    chunks.append(("hunk", (path, index), header + block[position.start():end]))
        for kind, path, patch in chunks:
            token = hashlib.sha1((record["commit"] + "\0" + kind + "\0" + str(path)).encode()).hexdigest()
            lookup[token] = (record, kind, path)
            stream.append(b"commit " + token.encode() + b"\n" + patch + b"\n")
            buffered += len(stream[-1])
            if buffered >= 4 * 1024 * 1024:
                flush()
    flush()


def classify(records, other):
    patches, files, sources, subjects = (defaultdict(list) for _ in range(4))
    hunks = defaultdict(set)
    for record in other:
        if record["patchId"]:
            patches[record["patchId"]].append(record["commit"])
        for path, patch in record["filePatchIds"].items():
            files[(path, patch)].append(record["commit"])
        for path, identities in record["hunkPatchIds"].items():
            for identity in identities:
                if identity:
                    hunks[(path, identity)].add(record["commit"])
        if record["source"]:
            sources[record["source"]].append(record["commit"])
        subjects[record["subject"]].append(record["commit"])
    results = []
    for record in records:
        exact = patches.get(record["patchId"], [])
        coverage = {path: files[(path, patch)] for path, patch in record["filePatchIds"].items() if (path, patch) in files}
        same_source = sources.get(record["source"], []) if record["source"] else []
        same_subject = subjects.get(record["subject"], [])
        hunk_matches = {path: [{"hunk": index + 1, "commits": sorted(hunks[(path, identity)])}
                              for index, identity in enumerate(identities) if (path, identity) in hunks]
                        for path, identities in record["hunkPatchIds"].items() if path not in coverage}
        hunk_matches = {path: matches for path, matches in hunk_matches.items() if matches}
        hunk_total = sum(len(identities) for identities in record["hunkPatchIds"].values())
        hunk_count = sum(len(matches) for matches in hunk_matches.values())
        hunk_count += sum(len(record["hunkPatchIds"].get(path, [])) for path in coverage)
        if not record["files"]:
            status = "metadata-only"
        elif exact:
            status = "patch-equivalent"
        elif coverage and len(coverage) == len(record["files"]):
            status = "file-patches-covered"
        elif coverage:
            status = "partial-file-patches"
        elif hunk_count:
            status = "partial-hunk-patches"
        elif same_source or same_subject:
            status = "related-candidate"
        else:
            status = "unmatched"
        results.append({key: record[key] for key in ("commit", "subject", "source", "authorDate", "committerDate", "files", "patchId")} |
                       {"evidence": status, "equivalentCommits": exact, "fileMatches": coverage,
                        "sameMonorepoSource": same_source, "sameSubject": same_subject,
                        "hunkMatches": hunk_matches, "matchedHunks": hunk_count, "totalHunks": hunk_total,
                        "unmatchedFiles": [path for path in record["files"] if path not in coverage]})
    return results


def audit(repo, game):
    refs = ["port-history/" + game, "port-history/" + game + "mp"]
    heads = [git(repo, "rev-parse", ref + "^{commit}").decode().strip() for ref in refs]
    shallow = Path(git(repo, "rev-parse", "--git-path", "shallow").decode().strip())
    if not shallow.is_absolute():
        shallow = repo / shallow
    if shallow.exists():
        reachable = set(git(repo, "rev-list", *heads).decode().splitlines())
        if reachable & set(shallow.read_text().splitlines()):
            raise RuntimeError("Complete selected histories before reconciliation: " + game)
    bases = git(repo, "merge-base", "--all", *heads).decode().splitlines()
    counts = [int(n) for n in git(repo, "rev-list", "--left-right", "--count", heads[0] + "..." + heads[1]).split()]
    sides = []
    for index in range(2):
        identities = git(repo, "rev-list", "--reverse", "--topo-order", "--no-merges", heads[index], "^" + heads[1-index]).decode().splitlines()
        with ThreadPoolExecutor(max_workers=6) as executor:
            sides.append(list(executor.map(lambda identity: read_commit(repo, identity), identities)))
    # MP may have copied a change that is now in the normal side's common
    # ancestry. Search the complete normal history, not just its ahead range.
    normal_ids = {record["commit"] for record in sides[0]}
    earlier_ids = [identity for identity in git(repo, "rev-list", "--no-merges", heads[0]).decode().splitlines() if identity not in normal_ids]
    with ThreadPoolExecutor(max_workers=6) as executor:
        earlier = list(executor.map(lambda identity: read_reference_commit(repo, identity), earlier_ids))
    fingerprints(repo, sides[0] + sides[1])
    normal, mp = classify(sides[0], sides[1]), classify(sides[1], sides[0] + earlier)
    snapshot = Snapshot(repo, heads[1])
    try:
        states = current_state(sides[0], snapshot, game)
        for row in normal:
            row["currentState"] = states[row["commit"]]
    finally:
        snapshot.close()
    return {"game": game, "heads": dict(zip(("normal", "mp"), heads)), "mergeBases": bases,
            "graphExclusiveCounts": dict(zip(("normal", "mp"), counts)),
            "evidenceCounts": {"normal": dict(Counter(row["evidence"] for row in normal)),
                               "mp": dict(Counter(row["evidence"] for row in mp))},
            "normalReferenceHistory": [{key: record[key] for key in ("commit", "subject", "authorDate", "committerDate")} for record in sides[0] + earlier],
            "merges": read_merges(repo, dict(zip(("normal", "mp"), heads))),
            "normal": normal, "mp": mp}


def read_merges(repo, heads):
    normal_history = set(git(repo, "rev-list", heads["normal"]).decode().splitlines())
    rows = []
    for side, other in (("normal", "mp"), ("mp", "normal")):
        identities = git(repo, "rev-list", "--min-parents=2", heads[side], "^" + heads[other]).decode().splitlines()
        for identity in identities:
            parents, subject = git(repo, "show", "-s", "--format=%P%x00%s", identity).decode().strip().split("\0", 1)
            paths = git(repo, "show", "--no-color", "--remerge-diff", "--format=", "--name-only", identity).decode().splitlines()
            rows.append({"commit": identity, "side": side, "parents": parents.split(), "subject": subject,
                         "resolutionPaths": [path for path in paths if path.strip()],
                         "normalParents": [parent for parent in parents.split() if parent in normal_history]})
    return rows


LABELS = {"patch-equivalent": "整提交补丁等价", "file-patches-covered": "各文件补丁均有对应",
          "partial-file-patches": "部分文件补丁对应", "partial-hunk-patches": "部分修改块对应",
          "related-candidate": "共同来源或同题候选", "unmatched": "未匹配补丁", "metadata-only": "无文件差异的历史记录"}

REVIEW_LABELS = {"received": "等价接收", "adapted": "适配接收", "parallel": "同次维护",
                 "superseded": "共有目录接替", "metadata": "历史记录", "partial": "部分接收", "pending": "待同步"}

MP_LABELS = {"normal-equivalent": "普通版补丁等价", "normal-correspondence": "普通版内容对应",
             "paired-maintenance": "同次维护", "mp-implementation": "多人实现",
             "mp-runtime-work": "MP 侧运行时改进", "mp-validation": "多人验证与文档",
             "shared-infrastructure": "共有设施与交付", "mixed-integration": "混合集成"}


def attach_decisions(repo, report, decisions):
    by_game = {game["game"]: game for game in decisions["games"]}
    for game in report["games"]:
        reviewed = by_game.get(game["game"])
        if reviewed is None:
            continue
        normal_history = {row["commit"] for row in game["normalReferenceHistory"]}
        mp_history = set(git(repo, "rev-list", game["heads"]["mp"]).decode().splitlines())
        records = {}
        for record in reviewed["records"]:
            if record["normalCommit"] in records:
                raise RuntimeError("Duplicate review record: " + record["normalCommit"])
            if record["normalCommit"] not in normal_history:
                raise RuntimeError("Review source is outside normal history: " + record["normalCommit"])
            if record["status"] not in REVIEW_LABELS:
                raise RuntimeError("Unknown review status: " + record["status"])
            if any(identity not in mp_history for identity in record["correspondingMpCommits"]):
                raise RuntimeError("Review counterpart is outside MP history")
            if (record["status"] in ("pending", "partial")) != bool(record["pendingScopes"]):
                raise RuntimeError("Pending review must state its remaining scopes")
            if record["pendingScopes"]:
                if not record.get("pendingEvidence"):
                    raise RuntimeError("Pending review must include source-delta evidence")
                for proof in record["pendingEvidence"]:
                    patch = git(repo, "show", "--no-color", "--format=", record["normalCommit"], "--", proof["sourcePath"]).decode("utf-8")
                    added = "\n".join(line[1:] for line in patch.splitlines() if line.startswith("+") and not line.startswith("+++"))
                    if any(token not in added for token in proof["addedContains"]):
                        raise RuntimeError("Pending scope is not in the source delta: " + record["normalCommit"])
                    target = subprocess.run(["git", "-C", str(repo), "show", reviewed["reviewedHeads"]["mp"] + ":" + proof["targetPath"]], capture_output=True)
                    if proof.get("targetPathAbsent"):
                        if target.returncode == 0:
                            raise RuntimeError("Pending target path is already present")
                    elif target.returncode or any(token in target.stdout.decode("utf-8") for token in proof["targetMissing"]):
                        raise RuntimeError("Pending scope target evidence differs")
            records[record["normalCommit"]] = record
        current = reviewed["reviewedHeads"] == game["heads"]
        for row in game["normal"]:
            if row["commit"] in records:
                row["review"] = records[row["commit"]] | {"snapshotMatches": current}
        game["reviewSummary"] = {"snapshotMatches": current,
                                 "reviewed": sum("review" in row for row in game["normal"]),
                                 "total": len(game["normal"]),
                                 "counts": dict(Counter(row["review"]["status"] for row in game["normal"] if "review" in row))}
        mp_records = {}
        complete_normal_history = set(git(repo, "rev-list", game["heads"]["normal"]).decode().splitlines())
        observed_mp = {row["commit"]: row for row in game["mp"]}
        for record in reviewed.get("mpRecords", []):
            identity = record["mpCommit"]
            if identity in mp_records or identity not in mp_history:
                raise RuntimeError("Duplicate or unreachable MP review")
            if record["classification"] not in MP_LABELS:
                raise RuntimeError("Unknown MP review classification")
            if any(source not in complete_normal_history for source in record["normalCounterparts"]):
                raise RuntimeError("MP counterpart is outside normal history")
            if identity in observed_mp:
                row = observed_mp[identity]
                if not set(record["scopeFiles"]).issubset(row["files"]) or record["changedFileCount"] != len(row["files"]):
                    raise RuntimeError("MP review source scope differs")
                if record["classification"] == "normal-equivalent" and not set(record["normalCounterparts"]) & set(row["equivalentCommits"]):
                    raise RuntimeError("MP equivalent review lacks patch evidence")
            mp_records[identity] = record
        for row in game["mp"]:
            if row["commit"] in mp_records:
                row["review"] = mp_records[row["commit"]] | {"snapshotMatches": current}
        game["mpReviewSummary"] = {"snapshotMatches": current,
                                   "reviewed": sum("review" in row for row in game["mp"]), "total": len(game["mp"]),
                                   "counts": dict(Counter(row["review"]["classification"] for row in game["mp"] if "review" in row))}
        merge_reviews = {(row["side"], row["commit"]): row for row in reviewed.get("mergeRecords", [])}
        if len(merge_reviews) != len(reviewed.get("mergeRecords", [])):
            raise RuntimeError("Duplicate merge review")
        for row in game.get("merges", []):
            key = (row["side"], row["commit"])
            if key in merge_reviews:
                record = merge_reviews[key]
                if record["parents"] != row["parents"] or record["resolutionPaths"] != row["resolutionPaths"]:
                    raise RuntimeError("Merge review parents or resolution scope differs")
                row["review"] = {"summary": record["summary"], "snapshotMatches": current}
        game["mergeReviewSummary"] = {"snapshotMatches": current,
                                      "reviewed": sum("review" in row for row in game.get("merges", [])),
                                      "total": len(game.get("merges", []))}


def markdown(report):
    lines = ["# 普通版与 MP 提交对账", "",
             "对账固定两侧历史分支快照，为独有提交建立补丁、文件和共同来源的对应证据。原提交与 Git 祖先关系保持原样。", "",
             "补丁等价沿用 Git stable patch-id 的口径，忽略空白和行号偏移。普通版完整历史参与整提交补丁比较；两侧独有提交进一步按相同路径比较文件与修改块，定位拆分、合并迁入。共同来源与同标题用于定位适配提交。", "",
             "对应关系记录两侧内容联系，迁入方向结合来源说明与上下文核对。JSON 保留作者与提交时间，供查看反向迁入和提交重建。无文件差异的历史记录单列。", "",
             "历史补丁对应结合当前源码核验保留情况、后续撤销与玩法适配。逐项决定保存处理范围，Git 祖先关系由实际合并维护。表内逐项列出两侧独有的非合并提交；MP 的来源比较覆盖普通版完整历史。", "",
             "| 作品 | 普通版独有 | MP 独有 | 普通版补丁等价 | MP 补丁等价 |", "| --- | ---: | ---: | ---: | ---: |"]
    for result in report["games"]:
        counts, evidence = result["graphExclusiveCounts"], result["evidenceCounts"]
        lines.append(f'| {result["game"]} | {counts["normal"]} | {counts["mp"]} | {evidence["normal"].get("patch-equivalent", 0)} | {evidence["mp"].get("patch-equivalent", 0)} |')
    if any("reviewSummary" in game for game in report["games"]):
        lines += ["", "## 逐项结论与待同步范围", "",
                  "结论按固定源码快照保存在 [对账决定](port-sync-decisions.json)。同次维护表示普通版和 MP 来自同一个全目录提交，按各自模式维护内容。快照变化后，历史结论保留并标明需要复核；补丁对应与当前文本证据继续独立展示。", "",
                  "| 作品 | 已核对非合并提交 | 快照一致 | 结论 |", "| --- | ---: | --- | --- |"]
        for game in report["games"]:
            review = game.get("reviewSummary")
            if review:
                counts = ", ".join(REVIEW_LABELS[key] + " " + str(value) for key, value in review["counts"].items())
                lines.append(f'| {game["game"]} | {review["reviewed"]}/{review["total"]} | {"是" if review["snapshotMatches"] else "已变化"} | {counts} |')
        lines += ["", "| 作品 | MP 来源逐项核对 | 合并节点核对 |", "| --- | ---: | ---: |"]
        for game in report["games"]:
            mp_review, merge_review = game.get("mpReviewSummary"), game.get("mergeReviewSummary")
            if mp_review and merge_review:
                lines.append(f'| {game["game"]} | {mp_review["reviewed"]}/{mp_review["total"]} | {merge_review["reviewed"]}/{merge_review["total"]} |')
        lines += ["", "| 作品 | 来源提交 | 明确待同步范围 |", "| --- | --- | --- |"]
        for game in report["games"]:
            for row in game["normal"]:
                for scope in row.get("review", {}).get("pendingScopes", []):
                    lines.append(f'| {game["game"]} | `{row["commit"][:10]}` | {scope.replace("|", ",")} |')
    for result in report["games"]:
        lines += ["", "## " + result["game"], "", "普通版快照：`" + result["heads"]["normal"] + "`。MP 快照：`" + result["heads"]["mp"] + "`。", ""]
        # All normal-side changes are listed; MP-only changes are included when
        # a normal counterpart was found. JSON retains every unmatched MP row.
        for side, heading in [("normal", "普通版独有提交"), ("mp", "MP 独有提交中找到对应的修改")]:
            lines += ["### " + heading, "", "| 提交 | 内容 | 对应证据 | 对侧提交 | 文件补丁覆盖 | 修改块覆盖 |", "| --- | --- | --- | --- | --- | --- | --- |"]
            for row in result[side]:
                if side == "mp" and row["evidence"] == "unmatched":
                    continue
                others = set(row["equivalentCommits"] + row["sameMonorepoSource"] + row["sameSubject"])
                others.update(commit for commits in row["fileMatches"].values() for commit in commits)
                others.update(commit for matches in row["hunkMatches"].values() for match in matches for commit in match["commits"])
                others.update(row.get("review", {}).get("correspondingMpCommits", []))
                counterparts = " | ".join("`" + value[:10] + "`" for value in sorted(others)) or "无补丁对应"
                subject = row["subject"].replace("|", "\\|").replace("\n", " ")
                # Use commas inside table cells so commit links do not create columns.
                counterparts = counterparts.replace(" | ", ", ")
                lines.append(f'| `{row["commit"][:10]}` | {subject} | {LABELS[row["evidence"]]} | {counterparts} | {len(row["fileMatches"])}/{len(row["files"])} | {row["matchedHunks"]}/{row["totalHunks"]} |')
            lines.append("")
            if side == "normal":
                lines += ["| 提交 | 核对结论 | 当前源码与处理范围 |", "| --- | --- | --- |"]
                for row in result[side]:
                    review = row.get("review")
                    if review:
                        label = REVIEW_LABELS[review["status"]] + ("" if review["snapshotMatches"] else " | 历史快照")
                        lines.append(f'| `{row["commit"][:10]}` | {label.replace("|", ",")} | {review["summary"].replace("|", ",")} |')
                lines.append("")
        if any("review" in row for row in result["mp"]):
            lines += ["### MP 逐项来源记录", "",
                      "按普通版补丁、文件交集、生产函数范围与合并图记录。多人实现表示本次修改服务多人功能，MP 侧运行时改进表示在 MP 历史中维护的通用代码；迁入方向沿原始证据和时间保留。", "",
                      "| 提交 | 内容 | 来源分类 | 普通版对应 | 核对范围 |", "| --- | --- | --- | --- | --- |"]
            for row in result["mp"]:
                review = row.get("review")
                if review:
                    counterparts = ", ".join("`" + identity[:10] + "`" for identity in review["normalCounterparts"]) or "按 MP 修改范围记录"
                    label = MP_LABELS[review["classification"]] + ("" if review["snapshotMatches"] else " | 历史快照")
                    lines.append(f'| `{row["commit"][:10]}` | {row["subject"].replace("|", ",")} | {label.replace("|", ",")} | {counterparts} | {review["summary"].replace("|", ",")} |')
            lines.append("")
    reviewed_merges = [(game, row) for game in report["games"] for row in game.get("merges", []) if "review" in row]
    if reviewed_merges:
        lines += ["## 合并节点", "", "合并父链保持原提交，冲突解决范围通过 Git remerge-diff 对照。完整路径保存在 JSON 中。", "",
                  "| 作品 | 侧 | 合并提交 | 父节点 | 冲突解决文件数 | 核对范围 |", "| --- | --- | --- | --- | ---: | --- |"]
        for game, row in reviewed_merges:
            parents = ", ".join("`" + parent[:10] + "`" for parent in row["parents"])
            lines.append(f'| {game["game"]} | {row["side"]} | `{row["commit"][:10]}` | {parents} | {len(row["resolutionPaths"])} | {row["review"]["summary"].replace("|", ",")} |')
        lines.append("")
    backports = [(game, row) for game in report["games"] for row in game["mp"] if row.get("review", {}).get("runtimeBackportCandidate")]
    if backports:
        lines += ["## 可向普通版与同架构作品评估的改进", "", "运行时改进按原提交追溯，迁入时结合目标作品的数值、输入和资源所有权检查。", "",
                  "| 作品 | MP 提交 | 改进范围 |", "| --- | --- | --- |"]
        for game, row in backports:
            lines.append(f'| {game["game"]} | `{row["commit"][:10]}` | {row["review"]["summary"].replace("|", ",")} |')
    return "\n".join(lines) + "\n"


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", default=".", type=Path)
    parser.add_argument("--game", action="append", choices=("th08", "th10", "th11", "th15"))
    parser.add_argument("--report", type=Path, required=True)
    parser.add_argument("--markdown", type=Path)
    parser.add_argument("--decisions", type=Path, default=Path("docs/history/port-sync-decisions.json"))
    args = parser.parse_args()
    report = {"schema": "eagler-touhou/history-reconciliation/1", "evidenceOnly": True,
              "games": [audit(args.repo.resolve(), game) for game in (args.game or ["th08", "th10", "th11", "th15"])]}
    decisions_path = args.decisions if args.decisions.is_absolute() else args.repo / args.decisions
    if decisions_path.exists():
        attach_decisions(args.repo, report, json.loads(decisions_path.read_text(encoding="utf-8")))
    args.report.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    if args.markdown:
        args.markdown.write_text(markdown(report), encoding="utf-8")
    print(json.dumps([{key: result[key] for key in ("game", "graphExclusiveCounts", "evidenceCounts", "reviewSummary", "mpReviewSummary", "mergeReviewSummary") if key in result} for result in report["games"]], ensure_ascii=False, indent=2))
