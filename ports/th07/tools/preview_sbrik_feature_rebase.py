#!/usr/bin/env python3
"""Preview a three-way rebase of sbrik TH07 multiplayer onto Eagler TH07.

This is intentionally read-only.  The two upstream refs live in this repo's
object database under src/th07/, while the Eagler tree keeps the same game
sources directly under src/.  For every file changed by the multiplayer
feature, run the equivalent of:

    merge(current Eagler, sbrik pre-netplay, sbrik latest multiplayer)

and report whether Git can resolve it without conflict.
"""

from __future__ import annotations

import difflib
import pathlib
import subprocess
import tempfile


BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"
UPSTREAM = "refs/remotes/mp-source-sbrik/main"
UPSTREAM_PREFIX = "src/th07/"


def git(*args: str, text: bool = True) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        ["git", *args],
        check=False,
        text=text,
        encoding="utf-8" if text else None,
        errors="replace" if text else None,
        capture_output=True,
    )


def show(ref: str, path: str) -> str | None:
    result = git("show", f"{ref}:{path}")
    return result.stdout if result.returncode == 0 else None


def changed_paths() -> list[str]:
    result = git("diff", "--name-only", f"{BASE}..{UPSTREAM}", "--", "src/th07")
    result.check_returncode()
    return [line.strip() for line in result.stdout.splitlines() if line.strip()]


def similarity(a: str, b: str) -> float:
    return difflib.SequenceMatcher(
        None,
        a.replace("\r\n", "\n").splitlines(),
        b.replace("\r\n", "\n").splitlines(),
        autojunk=True,
    ).ratio()


def merge_preview(ours: str, base: str, theirs: str) -> tuple[int, int]:
    with tempfile.TemporaryDirectory(dir=".") as tmp:
        root = pathlib.Path(tmp)
        ours_path = root / "ours"
        base_path = root / "base"
        theirs_path = root / "theirs"
        ours_path.write_text(ours, encoding="utf-8", newline="")
        base_path.write_text(base, encoding="utf-8", newline="")
        theirs_path.write_text(theirs, encoding="utf-8", newline="")
        result = git(
            "merge-file",
            "-p",
            str(ours_path),
            str(base_path),
            str(theirs_path),
        )
        conflicts = result.stdout.count("<<<<<<<")
        return result.returncode, conflicts


def main() -> int:
    rows: list[tuple[str, str, float | None, int | None]] = []
    for upstream_path in changed_paths():
        relative = upstream_path.removeprefix(UPSTREAM_PREFIX)
        local_path = pathlib.Path("src") / relative
        base = show(BASE, upstream_path)
        theirs = show(UPSTREAM, upstream_path)
        if theirs is None:
            rows.append((relative, "upstream-deleted", None, None))
            continue
        if base is None:
            status = "new-upstream" if not local_path.exists() else "new-upstream/local-exists"
            rows.append((relative, status, None, None))
            continue
        if not local_path.exists():
            rows.append((relative, "local-missing", None, None))
            continue

        ours = local_path.read_text(encoding="utf-8", errors="replace")
        ratio = similarity(base, ours)
        returncode, conflicts = merge_preview(ours, base, theirs)
        status = "clean" if returncode == 0 and conflicts == 0 else "conflict"
        rows.append((relative, status, ratio, conflicts))

    print(f"base={BASE}")
    print(f"upstream={UPSTREAM}")
    print("file\tstatus\tbase/local similarity\tconflicts")
    for path, status, ratio, conflicts in rows:
        ratio_text = "-" if ratio is None else f"{ratio:.4f}"
        conflict_text = "-" if conflicts is None else str(conflicts)
        print(f"{path}\t{status}\t{ratio_text}\t{conflict_text}")

    clean = sum(1 for _, status, _, _ in rows if status == "clean")
    conflict = sum(1 for _, status, _, _ in rows if status == "conflict")
    new = sum(1 for _, status, _, _ in rows if status.startswith("new-upstream"))
    print(f"summary clean={clean} conflict={conflict} new={new} total={len(rows)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
