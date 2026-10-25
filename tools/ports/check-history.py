"""Verify retained source ancestry and exported port subtree trees."""

import argparse
import json
import os
import pathlib
import re
import shutil
import subprocess


SHA = re.compile(r"[0-9a-f]{40}\Z")
DIRECTORY = re.compile(r"ports/[a-z0-9]+(?:-mp)?\Z")


class Repository:
    def __init__(self, path):
        self.path = pathlib.Path(path).resolve()

    def git(self, *arguments):
        command = ["git", "-C", str(self.path), *arguments]
        if arguments[:1] == ("subtree",) and os.name != "nt":
            helper = pathlib.Path(self.git("--exec-path")) / "git-subtree"
            bash = shutil.which("bash")
            # Run Git's own subtree implementation with Bash. Debian's /bin/sh
            # caps nested functions at 1000, below imported source parent chains.
            if bash and helper.is_file():
                command = [bash, str(helper), *arguments[1:]]
        result = subprocess.run(
            command, cwd=self.path,
            capture_output=True, text=True, encoding="utf-8",
        )
        if result.returncode:
            raise RuntimeError(f"git {' '.join(arguments)}: {result.stderr.strip()}")
        return result.stdout.strip()

    def ancestor(self, older, newer):
        result = subprocess.run(
            ["git", "-C", str(self.path), "merge-base", "--is-ancestor", older, newer],
            capture_output=True,
        )
        if result.returncode not in (0, 1):
            raise RuntimeError(result.stderr.decode("utf-8", errors="replace"))
        return result.returncode == 0

    def document(self, head, path):
        return json.loads(self.git("show", f"{head}:{path}"))

    def directories(self, head):
        if not self.git("ls-tree", "-d", "--name-only", head, "ports"):
            return set()
        return {f"ports/{name}" for name in self.git("ls-tree", "-d", "--name-only", f"{head}:ports").splitlines()}


def sha(value, label):
    if not isinstance(value, str) or not SHA.fullmatch(value):
        raise RuntimeError(f"Invalid {label}: {value}")
    return value


def verify_join(repo, head, directory, join, split):
    sha(join, "join commit")
    sha(split, "split commit")
    if not repo.ancestor(join, head):
        raise RuntimeError(f"{directory}: join {join} is not an ancestor of {head}")
    parents = repo.git("rev-list", "--parents", "-n", "1", join).split()
    if split not in parents[1:]:
        raise RuntimeError(f"{directory}: split {split} is not a parent of join {join}")
    message = repo.git("show", "-s", "--format=%B", join)
    if f"git-subtree-dir: {directory}" not in message.splitlines():
        raise RuntimeError(f"{directory}: join {join} has a different subtree directory")
    if f"git-subtree-split: {split}" not in message.splitlines():
        raise RuntimeError(f"{directory}: join {join} has a different subtree split")
    if repo.git("rev-parse", f"{join}:{directory}") != repo.git("rev-parse", f"{split}^{{tree}}"):
        raise RuntimeError(f"{directory}: join {join} has a different source tree")


def verify_subtree(repo, head, record, workspace):
    directory = record["destination"]
    if not DIRECTORY.fullmatch(directory) or directory not in workspace.values():
        raise RuntimeError(f"Unregistered subtree directory: {directory}")
    if record["currentMode"] != "git-subtree":
        raise RuntimeError(f"{directory}: source record is not a Git subtree")
    upstream = sha(record["subtree"]["upstreamCommit"], "upstream commit")
    if record["commit"] != upstream:
        raise RuntimeError(f"{directory}: original source and subtree upstream differ")
    joins = record["subtree"]
    verify_join(repo, head, directory, joins["initialJoin"], joins["initialSplit"])
    verify_join(repo, head, directory, joins["latestJoin"], joins["currentSplit"])
    changes = repo.git("rev-list", "--max-count=1", f"{joins['latestJoin']}..{head}", "--", directory)
    split = (repo.git("subtree", "split", f"--prefix={directory}", "--quiet", head)
             if changes else joins["currentSplit"])
    if not repo.ancestor(upstream, split):
        raise RuntimeError(f"{directory}: original upstream {upstream} is not an ancestor of exported history")
    if not repo.ancestor(joins["currentSplit"], split):
        raise RuntimeError(f"{directory}: exported history no longer contains the recorded split")
    if repo.git("rev-parse", f"{head}:{directory}") != repo.git("rev-parse", f"{split}^{{tree}}"):
        raise RuntimeError(f"{directory}: exported tree differs from the current directory")
    return split


def subtree_records(repo, head):
    records = []
    migration = repo.git("ls-tree", "--name-only", f"{head}:docs/migration").splitlines()
    for name in migration:
        if not re.fullmatch(r"th[0-9]+-eagler-[0-9]+\.json", name):
            continue
        record = repo.document(head, f"docs/migration/{name}")
        if record.get("currentMode") == "git-subtree":
            records.append(record)
    if len({record["destination"] for record in records}) != len(records):
        raise RuntimeError("Duplicate Git subtree source records")
    return records


def original_identity(record):
    subtree = record["subtree"]
    return (record["destination"], record["commit"], subtree["upstreamCommit"],
            subtree["initialJoin"], subtree["initialSplit"])


def verify(repo, head="HEAD", base=None):
    head = repo.git("rev-parse", "--verify", f"{head}^{{commit}}")
    workspace = repo.document(head, "launcher/config/workspace.json")["repositories"]
    imports = repo.document(head, "docs/migration/import-sources.json")["imports"]
    moves = repo.document(head, "docs/migration/move-sources-20261018.json")["sources"]
    for entry in imports:
        target = entry["target"]
        if target == "launcher":
            continue
        original = sha(entry["head"], f"{target} source")
        if not repo.ancestor(original, head):
            raise RuntimeError(f"{target}: original source is not an ancestor of {head}")
    for entry in moves:
        target = entry["target"]
        if target.startswith("ports/") and not repo.ancestor(sha(entry["head"], f"{target} move"), head):
            raise RuntimeError(f"{target}: move source is not an ancestor of {head}")

    records = subtree_records(repo, head)
    splits = {}
    for record in records:
        directory = record["destination"]
        splits[directory] = verify_subtree(repo, head, record, workspace)

    if base and set(base) != {"0"}:
        base = repo.git("rev-parse", "--verify", f"{base}^{{commit}}")
        base_imports = repo.document(base, "docs/migration/import-sources.json")["imports"]
        for path, field in (("docs/migration/import-sources.json", "imports"),
                            ("docs/migration/move-sources-20261018.json", "sources")):
            original = repo.document(base, path)[field]
            current = repo.document(head, path)[field]
            if any(entry not in current for entry in original):
                raise RuntimeError(f"Retained source records changed: {path}")
        existing_subtrees = {original_identity(record) for record in subtree_records(repo, base)}
        if not existing_subtrees.issubset({original_identity(record) for record in records}):
            raise RuntimeError("Retained Git subtree source records changed")
        added = repo.directories(head) - repo.directories(base)
        known = {entry["target"].replace("runtimes/", "ports/", 1)
                 for entry in base_imports if entry["target"] != "launcher"}
        known.update(record["destination"] for record in records)
        missing = sorted(added - known)
        if missing:
            raise RuntimeError("New port directories need retained source history: " + ", ".join(missing))
    return {"sourceAnchors": len(imports) - sum(row["target"] == "launcher" for row in imports),
            "subtrees": splits}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", default=pathlib.Path(__file__).resolve().parents[2])
    parser.add_argument("--head", default="HEAD")
    parser.add_argument("--base")
    args = parser.parse_args()
    print(json.dumps(verify(Repository(args.repo), args.head, args.base), ensure_ascii=False, sort_keys=True))


if __name__ == "__main__":
    main()
