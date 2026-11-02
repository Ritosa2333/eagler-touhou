"""Project imported title histories into independent Git refs.

Original repository commits stay unchanged. Monorepo commits are projected by
their owned directory tree, retaining author/date, relevant merge parents and
a Monorepo-Source trailer. No checkout, staging, push or merge is performed.
"""
import argparse
import json
import pathlib
import re
import subprocess


def projection_message(message):
    # A directory projection is not a monorepo subtree connection. Replaying
    # its join trailers registers the same mainline twice in git subtree's
    # cache after write-back. Provenance below always refers to this projection.
    return re.sub(rb"(?m)^(?:git-subtree-[^:\r\n]+|Monorepo-Source|Source-Prefix):[^\r\n]*(?:\r?\n|$)", b"", message).rstrip()


class Git:
    def __init__(self, repository):
        self.repository = str(pathlib.Path(repository).resolve())
        self.reader = subprocess.Popen(
            ["git", "-C", self.repository, "cat-file", "--batch"],
            stdin=subprocess.PIPE, stdout=subprocess.PIPE,
        )
        self.objects = {}
        self.trees = {}

    def run(self, *args, data=None):
        return subprocess.check_output(
            ["git", "-C", self.repository, *args], input=data,
        ).decode("utf-8").strip()

    def ancestor(self, older, newer):
        result = subprocess.run(
            ["git", "-C", self.repository, "merge-base", "--is-ancestor", older, newer],
            stdout=subprocess.DEVNULL, stderr=subprocess.PIPE,
        )
        if result.returncode not in (0, 1):
            raise RuntimeError(result.stderr.decode("utf-8"))
        return result.returncode == 0

    def object(self, identity):
        if identity not in self.objects:
            self.reader.stdin.write((identity + "\n").encode("ascii"))
            self.reader.stdin.flush()
            header = self.reader.stdout.readline().split()
            if len(header) != 3:
                raise RuntimeError("Missing Git object: " + identity)
            content = self.reader.stdout.read(int(header[2]))
            if self.reader.stdout.read(1) != b"\n":
                raise RuntimeError("Invalid Git object stream")
            self.objects[identity] = (header[1], content)
        return self.objects[identity]

    def commit(self, identity):
        kind, content = self.object(identity)
        if kind != b"commit":
            raise RuntimeError("Expected commit: " + identity)
        header, _, message = content.partition(b"\n\n")
        fields = header.splitlines()
        return fields[0].split()[1].decode("ascii"), fields, message

    def subtree(self, identity, path):
        for name in path.split("/"):
            if identity not in self.trees:
                kind, content = self.object(identity)
                if kind != b"tree":
                    return None
                entries = {}
                index = 0
                while index < len(content):
                    space = content.index(b" ", index)
                    nul = content.index(b"\0", space)
                    entries[content[space + 1:nul].decode("utf-8")] = (
                        content[index:space], content[nul + 1:nul + 21].hex(),
                    )
                    index = nul + 21
                self.trees[identity] = entries
            entry = self.trees[identity].get(name)
            if entry is None or entry[0] not in (b"40000", b"040000"):
                return None
            identity = entry[1]
        return identity

    def close(self):
        self.reader.stdin.close()
        self.reader.stdout.close()
        self.reader.wait()

    def replace_subtree(self, tree, path, replacement):
        # Populate the parsed tree without altering the index or worktree.
        self.subtree(tree, path)
        parts = path.split("/")
        name = parts[0]
        entries = dict(self.trees[tree])
        if name not in entries or entries[name][0] not in (b"40000", b"040000"):
            raise RuntimeError("Write-back target is not a directory: " + path)
        child = replacement if len(parts) == 1 else self.replace_subtree(entries[name][1], "/".join(parts[1:]), replacement)
        entries[name] = (b"40000", child)
        records = []
        for entry, (mode, identity) in entries.items():
            kind = b"tree" if mode in (b"40000", b"040000") else b"commit" if mode == b"160000" else b"blob"
            records.append(mode + b" " + kind + b" " + identity.encode("ascii") + b"\t" + entry.encode("utf-8") + b"\0")
        return self.run("mktree", "-z", data=b"".join(records))


def project(git, head, game, write_refs=False, track_normal=False):
    if track_normal and not write_refs:
        raise RuntimeError("Tracking requires --write-refs")
    if git.run("rev-parse", "--show-object-format") != "sha1":
        raise RuntimeError("This projector currently supports SHA-1 repositories")
    head = git.run("rev-parse", "--verify", head + "^{commit}")
    load = lambda path: json.loads(git.run("show", head + ":" + path))
    imports = load("docs/migration/import-sources.json")["imports"]
    moves = load("docs/migration/move-sources-20261018.json")["sources"]
    layout = load("launcher/config/workspace.json")["repositories"]
    subtree_sources = {}
    for name in git.run("ls-tree", "--name-only", head + ":docs/migration").splitlines():
        if not re.fullmatch(r"th[0-9]+(?:mp|-mp)?-eagler-[0-9]+\.json", name):
            continue
        record = load("docs/migration/" + name)
        if record.get("currentMode") == "git-subtree":
            prefix = record["destination"]
            if prefix in subtree_sources:
                raise RuntimeError("Duplicate subtree source for " + prefix)
            subtree_sources[prefix] = record
    titles = [game, game + "mp"]
    if any(title not in layout for title in titles):
        raise RuntimeError("Game requires separate normal and multiplayer directories")
    roots = {row["head"] for row in imports + moves if row.get("target") != "launcher"}
    lines = git.run("rev-list", "--reverse", "--topo-order", "--parents", head).splitlines()
    # Track actual subtree merge sources too, so subsequent write-back merges
    # can advance the projected fork relationship through real Git ancestry.
    joins = {}
    for line in lines:
        identity = line.split()[0]
        _, _, message = git.commit(identity)
        trailers = {}
        for part in message.splitlines():
            key, separator, value = part.partition(b": ")
            if separator:
                trailers[key] = value
        directory = trailers.get(b"git-subtree-dir", b"").decode("utf-8")
        split = trailers.get(b"git-subtree-split", b"").decode("ascii")
        if directory and split and git.ancestor(split, head):
            roots.add(split)
            joins.setdefault(directory.rstrip("/"), set()).add(split)
    foreign = set(git.run("rev-list", *sorted(roots)).splitlines())
    results = {}
    for title in titles:
        prefix = layout[title]
        legacy = prefix.replace("ports/", "runtimes/", 1)
        initial = next((row for row in imports if row["target"] == legacy), None)
        subtree_source = subtree_sources.get(prefix) if initial is None else None
        subtree_imports = {}
        if initial is None:
            if subtree_source is None:
                raise RuntimeError("No imported repository anchor for " + prefix)
            source = subtree_source["subtree"]
            for join, split in ((source["initialJoin"], source["initialSplit"]), (source["latestJoin"], source["currentSplit"])):
                if not git.ancestor(join, head) or split not in git.run("rev-list", "--parents", "-n", "1", join).split()[1:]:
                    raise RuntimeError("Invalid subtree source parents for " + prefix)
                _, _, body = git.commit(join)
                if ("git-subtree-dir: " + prefix).encode() not in body.splitlines() or ("git-subtree-split: " + split).encode() not in body.splitlines():
                    raise RuntimeError("Invalid subtree source trailers for " + prefix)
                if git.run("rev-parse", join + ":" + prefix) != git.commit(split)[0]:
                    raise RuntimeError("Subtree snapshot differs from original source tree")
                subtree_imports[join] = split
            if not git.ancestor(source["upstreamCommit"], source["initialSplit"]):
                raise RuntimeError("Subtree split lost its upstream ancestor")
            if not git.ancestor(source["initialSplit"], source["currentSplit"]):
                raise RuntimeError("Subtree split lost its initial source ancestor")
            initial = {"head": source["initialSplit"], "importCommit": source["initialJoin"]}
            latest = source["currentSplit"]
        else:
            latest = next((row["head"] for row in moves if row["target"] == prefix), initial["head"])
        original_roots = {initial["head"], latest}
        join_tips = joins.get(prefix, set()) | joins.get(legacy, set())
        source_roots = original_roots | join_tips
        if any(not git.ancestor(origin, head) for origin in source_roots):
            raise RuntimeError("Source anchor is not an ancestor of selected HEAD")
        imported_tree = git.run("rev-parse", initial["importCommit"] + ":" + (prefix if subtree_source else legacy))
        if imported_tree != git.commit(initial["head"])[0]:
            raise RuntimeError("Imported snapshot differs from original source tree")
        # A later import may contain normal-side ancestors. Recognize its tip
        # without retroactively treating earlier normal imports as MP imports.
        owned = set(git.run("rev-list", *sorted(original_roots)).splitlines()) | join_tips
        shallow_path = pathlib.Path(git.run("rev-parse", "--git-path", "shallow"))
        if not shallow_path.is_absolute():
            shallow_path = pathlib.Path(git.repository) / shallow_path
        boundaries = set(shallow_path.read_text().splitlines()) if shallow_path.exists() else set()
        if owned & boundaries:
            raise RuntimeError("Imported history is shallow for " + title + "; complete original repository history before projection")
        mapping = {identity: identity for identity in owned}
        trees = {}
        created = []
        namespace = set()
        empty_tree = git.run("mktree", data=b"")

        def tree_of(identity):
            if identity not in trees:
                trees[identity] = git.commit(identity)[0]
            return trees[identity]

        for line in lines:
            identity, *parents = line.split()
            if identity in owned:
                continue
            if identity in foreign:
                mapping[identity] = None
                continue
            if identity in subtree_imports:
                # The validated export already contains pre-join directory
                # commits. Reusing it preserves the original parent chain
                # instead of creating duplicate projections beside it.
                mapping[identity] = subtree_imports[identity]
                namespace.add(identity)
                continue
            tree, fields, message = git.commit(identity)
            subtree = git.subtree(tree, prefix) or git.subtree(tree, legacy)
            projected_parents = list(dict.fromkeys(mapping[parent] for parent in parents if mapping.get(parent)))
            if subtree is None:
                if not projected_parents or not any(parent in namespace for parent in parents):
                    mapping[identity] = None
                    continue
                subtree = empty_tree
            namespace.add(identity)
            if not projected_parents:
                # A staged directory copy can precede its source join. Its
                # first authoritative export is the validated source split;
                # only that pre-join interval may lack projected ancestry.
                if subtree_source and git.ancestor(identity, initial["importCommit"]):
                    mapping[identity] = None
                    namespace.discard(identity)
                    continue
                raise RuntimeError("Unanchored directory history: " + identity + " " + prefix)
            projected_parents = [parent for parent in projected_parents if not any(
                parent != other and git.ancestor(parent, other) for other in projected_parents
            )]
            if len(projected_parents) == 1 and tree_of(projected_parents[0]) == subtree:
                mapping[identity] = projected_parents[0]
                continue
            header = [b"tree " + subtree.encode("ascii")]
            header += [b"parent " + parent.encode("ascii") for parent in projected_parents]
            header += [field for field in fields if field.startswith((b"author ", b"committer ", b"encoding "))]
            body = projection_message(message) + b"\n\nMonorepo-Source: " + identity.encode("ascii")
            body += b"\nSource-Prefix: " + prefix.encode("utf-8") + b"\n"
            projected = git.run("hash-object", "-t", "commit", "-w", "--stdin", data=b"\n".join(header) + b"\n\n" + body)
            mapping[identity] = projected
            trees[projected] = subtree
            created.append({"source": identity, "projected": projected})
        result = mapping[head]
        if tree_of(result) != git.run("rev-parse", head + ":" + prefix):
            raise RuntimeError("Projected tree differs from source directory")
        ref = "refs/heads/port-history/" + title
        results[title] = {"head": result, "ref": ref, "initialAncestor": git.ancestor(initial["head"], result),
                          "latestAncestor": git.ancestor(latest, result), "treeMatches": True, "mapping": created}
    if track_normal:
        branch = "port-history/" + game + "mp"
        expected = {"remote": ".", "merge": results[game]["ref"]}
        for key, value in expected.items():
            configured = subprocess.run(["git", "-C", git.repository, "config", "--get-all", "branch." + branch + "." + key], capture_output=True)
            if configured.returncode not in (0, 1):
                raise RuntimeError("Cannot read tracking configuration")
            if configured.stdout and configured.stdout.decode().splitlines() != [value]:
                raise RuntimeError("Existing upstream differs for " + branch)
    # Validate both sides before updating any ref. Existing refs only advance.
    if write_refs:
        updates = []
        checked_out = git.run("worktree", "list", "--porcelain")
        for result in results.values():
            ref = result["ref"]
            if "branch " + ref + "\n" in checked_out + "\n":
                raise RuntimeError("Projection ref is checked out: " + ref)
            previous = subprocess.run(["git", "-C", git.repository, "rev-parse", "--verify", ref], capture_output=True)
            if previous.returncode == 0:
                old = previous.stdout.decode().strip()
                if not git.ancestor(old, result["head"]):
                    raise RuntimeError("Projection would rewrite existing history: " + ref)
            else:
                old = "0" * 40
            updates.append("update " + ref + " " + result["head"] + " " + old)
        git.run("update-ref", "--stdin", data=("start\n" + "\n".join(updates) + "\nprepare\ncommit\n").encode())
    if track_normal:
        git.run("config", "branch." + branch + ".remote", ".")
        git.run("config", "branch." + branch + ".merge", results[game]["ref"])
    a, b = (results[title]["head"] for title in titles)
    base = subprocess.run(["git", "-C", git.repository, "merge-base", "--all", a, b], capture_output=True)
    if base.returncode not in (0, 1):
        raise RuntimeError(base.stderr.decode())
    bases = base.stdout.decode().splitlines()
    return {"sourceHead": head, "game": game, "results": results,
            "mergeBases": bases,
            "aheadBehind": git.run("rev-list", "--left-right", "--count", a + "..." + b) if bases else None}


def prepare_writeback(git, projection, merged_ref, create_commit=False, message="同步多人历史"):
    game = projection["game"]
    head = projection["sourceHead"]
    merged = git.run("rev-parse", "--verify", merged_ref + "^{commit}")
    # Only a reviewed merge that includes both current projected tips can
    # advance the complete fork relationship.
    for result in projection["results"].values():
        if not git.ancestor(result["head"], merged):
            raise RuntimeError("Merged branch does not include current projection: " + result["ref"])
    layout = json.loads(git.run("show", head + ":launcher/config/workspace.json"))["repositories"]
    prefix = layout[game + "mp"]
    merged_tree = git.commit(merged)[0]
    tree = git.replace_subtree(git.commit(head)[0], prefix, merged_tree)
    if git.subtree(tree, prefix) != merged_tree:
        raise RuntimeError("Write-back content verification failed")
    changed = [path for path in git.run("diff-tree", "--no-commit-id", "--name-only", "-z", "-r", head + "^{tree}", tree).split("\0") if path]
    if any(not path.startswith(prefix + "/") for path in changed):
        raise RuntimeError("Write-back changed a different directory")
    if not message.strip() or "\n" in message or "\r" in message:
        raise RuntimeError("Commit title must be one non-empty line")
    body = message + "\n\ngit-subtree-dir: " + prefix + "\ngit-subtree-mainline: " + head + "\ngit-subtree-split: " + merged + "\n"
    body += "Port-History-Writeback: 1\n"
    report = {"sourceHead": head, "game": game, "target": prefix, "merged": merged,
              "tree": tree, "parents": [head, merged], "message": body, "changed": changed}
    if create_commit:
        report["candidateCommit"] = git.run("-c", "commit.gpgsign=false", "commit-tree", tree, "-p", head, "-p", merged, data=body.encode("utf-8"))
    return report


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", required=True)
    parser.add_argument("--game", required=True)
    parser.add_argument("--head", default="HEAD")
    parser.add_argument("--write-refs", action="store_true")
    parser.add_argument("--track-normal", action="store_true", help="Set MP projection upstream to the normal projection")
    parser.add_argument("--report", type=pathlib.Path)
    parser.add_argument("--prepare-writeback", metavar="MERGED_REF")
    parser.add_argument("--create-commit", action="store_true")
    parser.add_argument("--message", default="同步多人历史")
    args = parser.parse_args()
    if args.track_normal and not args.write_refs:
        parser.error("--track-normal requires --write-refs")
    if args.create_commit and not args.prepare_writeback:
        parser.error("--create-commit requires --prepare-writeback")
    repository = Git(args.repo)
    try:
        report = project(repository, args.head, args.game, args.write_refs, args.track_normal)
        if args.prepare_writeback:
            report = prepare_writeback(repository, report, args.prepare_writeback, args.create_commit, args.message)
        output = json.dumps(report, ensure_ascii=False, indent=2) + "\n"
        if args.report:
            args.report.write_text(output, encoding="utf-8")
        summary = dict(report)
        if "results" in report:
            summary["results"] = {key: {field: value for field, value in result.items() if field != "mapping"}
                                  for key, result in report["results"].items()}
        print(json.dumps(summary, ensure_ascii=False, indent=2))
    finally:
        repository.close()
