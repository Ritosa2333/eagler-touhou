import importlib.util
import json
import os
import pathlib
import shutil
import subprocess
import tempfile
import unittest


spec = importlib.util.spec_from_file_location("check_history", pathlib.Path(__file__).with_name("check-history.py"))
history = importlib.util.module_from_spec(spec)
spec.loader.exec_module(history)


class SourceHistoryTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix="eagler-history-gate-")
        self.addCleanup(self.temporary.cleanup)
        root = pathlib.Path(self.temporary.name)
        self.source = root / "upstream"
        self.repo = root / "monorepo"
        for directory in (self.source, self.repo):
            directory.mkdir()
            self.git(directory, "init", "-q", "-b", "main")
            self.git(directory, "config", "user.name", "History Test")
            self.git(directory, "config", "user.email", "history@example.invalid")
        (self.source / "README.md").write_text("original source\n", encoding="utf-8")
        self.git(self.source, "add", "README.md")
        self.git(self.source, "commit", "-qm", "original")
        self.upstream = self.git(self.source, "rev-parse", "HEAD")

        self.write("docs/migration/import-sources.json", {"imports": []})
        self.write("docs/migration/move-sources-20261018.json", {"sources": []})
        self.write("launcher/config/workspace.json", {"repositories": {"th12": "ports/th12"}})
        self.git(self.repo, "add", ".")
        self.git(self.repo, "commit", "-qm", "base")
        self.base = self.git(self.repo, "rev-parse", "HEAD")
        self.git(self.repo, "subtree", "add", "--prefix=ports/th12", str(self.source), "main", "-m", "join")
        self.join = self.git(self.repo, "rev-parse", "HEAD")
        self.record = {
            "schema": "eagler-touhou/source-import/1",
            "repository": "https://github.com/example/th12",
            "branch": "main",
            "commit": self.upstream,
            "destination": "ports/th12",
            "currentMode": "git-subtree",
            "subtree": {"upstreamCommit": self.upstream, "initialJoin": self.join,
                        "initialSplit": self.upstream, "latestJoin": self.join,
                        "currentSplit": self.upstream},
        }
        self.write("docs/migration/th12-eagler-20261020.json", self.record)
        self.git(self.repo, "add", ".")
        self.git(self.repo, "commit", "-qm", "record source")

    @staticmethod
    def git(directory, *arguments):
        result = subprocess.run(["git", "-C", str(directory), *arguments], capture_output=True,
                                text=True, encoding="utf-8")
        if result.returncode:
            raise AssertionError(result.stderr)
        return result.stdout.strip()

    def write(self, name, value):
        path = self.repo / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(json.dumps(value), encoding="utf-8")

    def test_real_subtree_join_keeps_original_ancestor_and_export_tree(self):
        result = history.verify(history.Repository(self.repo), base=self.base)
        self.assertEqual(result["subtrees"]["ports/th12"], self.upstream)
        self.assertEqual(result["sourceAnchors"], 0)

    def test_multiplayer_source_record_runs_standard_export_and_ancestry_checks(self):
        self.write("launcher/config/workspace.json", {"repositories": {"th12": "ports/th12", "th15mp": "ports/th15-mp"}})
        self.git(self.repo, "add", ".")
        self.git(self.repo, "commit", "-qm", "register multiplayer directory")
        self.git(self.repo, "subtree", "add", "--prefix=ports/th15-mp", str(self.source), "main", "-m", "join multiplayer source")
        join = self.git(self.repo, "rev-parse", "HEAD")
        record = json.loads(json.dumps(self.record))
        record["destination"] = "ports/th15-mp"
        record["subtree"]["initialJoin"] = record["subtree"]["latestJoin"] = join
        self.write("docs/migration/th15mp-eagler-20261026.json", record)
        (self.repo / "ports/th15-mp/README.md").write_text("multiplayer frame restoration\n", encoding="utf-8")
        self.git(self.repo, "add", ".")
        self.git(self.repo, "commit", "-qm", "adapt multiplayer")
        result = history.verify(history.Repository(self.repo), base=self.base)
        split = result["subtrees"]["ports/th15-mp"]
        self.assertTrue(history.Repository(self.repo).ancestor(self.upstream, split))
        self.assertEqual(self.git(self.repo, "rev-parse", "HEAD:ports/th15-mp"), self.git(self.repo, "rev-parse", split + "^{tree}"))
        record["subtree"]["initialSplit"] = self.base
        self.write("docs/migration/th15mp-eagler-20261026.json", record)
        self.git(self.repo, "add", ".")
        self.git(self.repo, "commit", "-qm", "corrupt multiplayer source record")
        with self.assertRaisesRegex(RuntimeError, "is not a parent"):
            history.verify(history.Repository(self.repo), base=self.base)

    def test_new_port_commit_exports_ahead_of_original(self):
        (self.repo / "ports/th12/README.md").write_text("adapted source\n", encoding="utf-8")
        self.git(self.repo, "add", "ports/th12/README.md")
        self.git(self.repo, "commit", "-qm", "adapt port")
        result = history.verify(history.Repository(self.repo), base=self.base)
        split = result["subtrees"]["ports/th12"]
        self.assertNotEqual(split, self.upstream)
        self.assertTrue(history.Repository(self.repo).ancestor(self.upstream, split))
        self.assertEqual(self.git(self.repo, "rev-parse", "HEAD:ports/th12"),
                         self.git(self.repo, "rev-parse", f"{split}^{{tree}}"))

    @unittest.skipIf(os.name == "nt" or not shutil.which("bash"), "POSIX Bash execution")
    def test_subtree_helper_uses_bash_independently_of_its_sh_shebang(self):
        helpers = self.repo / "helpers"
        helpers.mkdir()
        (helpers / "git-subtree").write_text(
            '#!/bin/sh\n[ -n "$BASH_VERSION" ] || exit 91\n'
            '[ "$GIT_EXEC_PATH" = "${PATH%%:*}" ] || exit 92\n'
            'printf "%s\\n" "$*"\n',
            encoding="utf-8",
        )

        class FixtureRepository(history.Repository):
            def git(self, *arguments):
                if arguments == ("--exec-path",):
                    return str(helpers)
                return super().git(*arguments)

        self.assertEqual(
            FixtureRepository(self.repo).git("subtree", "split", "--prefix=ports/th12"),
            "split --prefix=ports/th12",
        )

    def test_invalid_join_record_cannot_claim_another_source(self):
        self.record["subtree"]["initialSplit"] = self.base
        self.write("docs/migration/th12-eagler-20261020.json", self.record)
        self.git(self.repo, "add", ".")
        self.git(self.repo, "commit", "-qm", "incorrect provenance")
        with self.assertRaisesRegex(RuntimeError, "different subtree split"):
            history.verify(history.Repository(self.repo), base=self.base)

    def test_squash_tree_does_not_preserve_source_history(self):
        tree = self.git(self.repo, "rev-parse", "HEAD^{tree}")
        squashed = subprocess.run(
            ["git", "-C", str(self.repo), "commit-tree", tree, "-p", self.base],
            input="squashed content\n", capture_output=True, text=True, encoding="utf-8", check=True,
        ).stdout.strip()
        with self.assertRaisesRegex(RuntimeError, "not an ancestor"):
            history.verify(history.Repository(self.repo), head=squashed, base=self.base)

    def test_rebase_without_merges_loses_subtree_join(self):
        self.git(self.repo, "switch", "-q", "-c", "new-base", self.base)
        (self.repo / "base-change.txt").write_text("new base\n", encoding="utf-8")
        self.git(self.repo, "add", "base-change.txt")
        self.git(self.repo, "commit", "-qm", "new base")
        self.git(self.repo, "switch", "-q", "main")
        self.git(self.repo, "rebase", "--onto", "new-base", self.base)
        with self.assertRaisesRegex(RuntimeError, "not an ancestor"):
            history.verify(history.Repository(self.repo), base=self.base)

    def test_merge_valid_history_repairs_squash_without_changing_tree(self):
        valid = self.git(self.repo, "rev-parse", "HEAD")
        tree = self.git(self.repo, "rev-parse", "HEAD^{tree}")
        squashed = subprocess.run(
            ["git", "-C", str(self.repo), "commit-tree", tree, "-p", self.base],
            input="squashed content\n", capture_output=True, text=True, encoding="utf-8", check=True,
        ).stdout.strip()
        self.git(self.repo, "switch", "-q", "--detach", squashed)
        self.git(self.repo, "merge", "-s", "ours", "-m", "restore source ancestry", valid)
        self.assertEqual(self.git(self.repo, "rev-parse", "HEAD^{tree}"), tree)
        history.verify(history.Repository(self.repo), base=self.base)

    def test_new_port_requires_retained_source_record(self):
        (self.repo / "ports/th13").mkdir()
        (self.repo / "ports/th13/README.md").write_text("new game\n", encoding="utf-8")
        self.git(self.repo, "add", ".")
        self.git(self.repo, "commit", "-qm", "unregistered port")
        with self.assertRaisesRegex(RuntimeError, "ports/th13"):
            history.verify(history.Repository(self.repo), base=self.base)

    def test_new_port_cannot_claim_a_new_legacy_import(self):
        self.write("docs/migration/import-sources.json", {
            "imports": [{"target": "runtimes/th13", "head": self.upstream}],
        })
        (self.repo / "ports/th13").mkdir()
        (self.repo / "ports/th13/README.md").write_text("new game\n", encoding="utf-8")
        self.git(self.repo, "add", ".")
        self.git(self.repo, "commit", "-qm", "false legacy import")
        with self.assertRaisesRegex(RuntimeError, "ports/th13"):
            history.verify(history.Repository(self.repo), base=self.base)

    def test_existing_subtree_record_cannot_be_removed(self):
        recorded = self.git(self.repo, "rev-parse", "HEAD")
        (self.repo / "docs/migration/th12-eagler-20261020.json").unlink()
        self.git(self.repo, "add", ".")
        self.git(self.repo, "commit", "-qm", "remove source record")
        with self.assertRaisesRegex(RuntimeError, "Retained Git subtree source records changed"):
            history.verify(history.Repository(self.repo), base=recorded)

    def test_existing_import_record_cannot_be_removed(self):
        record = {"target": "runtimes/th12", "head": self.upstream}
        self.write("docs/migration/import-sources.json", {"imports": [record]})
        self.git(self.repo, "add", ".")
        self.git(self.repo, "commit", "-qm", "record old import")
        recorded = self.git(self.repo, "rev-parse", "HEAD")
        self.write("docs/migration/import-sources.json", {"imports": []})
        self.git(self.repo, "add", ".")
        self.git(self.repo, "commit", "-qm", "remove old import")
        with self.assertRaisesRegex(RuntimeError, "Retained source records changed"):
            history.verify(history.Repository(self.repo), base=recorded)


if __name__ == "__main__":
    unittest.main()
