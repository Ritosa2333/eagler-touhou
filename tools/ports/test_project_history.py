import importlib.util
import json
import pathlib
import subprocess
import tempfile
import unittest

spec = importlib.util.spec_from_file_location("project_history", pathlib.Path(__file__).with_name("project-history.py"))
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


class ProjectionTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix="eagler-projection-test-")
        self.repository = pathlib.Path(self.temporary.name)
        self.run_git("init", "-q", "-b", "main")
        self.run_git("config", "user.name", "Test")
        self.run_git("config", "user.email", "test@example.invalid")
        self.reader = module.Git(self.repository)
        self.addCleanup(self.temporary.cleanup)
        self.addCleanup(self.reader.close)
        self.base_files = {"value.txt": "start\nmid\nend\n"}
        self.base = self.commit(self.base_files, [], "base")
        normal_files = {**self.base_files, "normal.txt": "normal feature\n"}
        mp_files = {**self.base_files, "mp.txt": "MP session\n", "名称.txt": "多人专属文件\n"}
        self.normal = self.commit(normal_files, [self.base], "normal")
        self.mp = self.commit(mp_files, [self.base], "multiplayer")
        self.launcher = self.commit({"launcher.txt": "launcher\n"}, [], "launcher")
        files = {"launcher.txt": "launcher\n", **{"runtimes/th10/" + path: value for path, value in normal_files.items()}}
        normal_import = self.commit(files, [self.launcher, self.normal], "import normal")
        files.update({"runtimes/th10-mp/" + path: value for path, value in mp_files.items()})
        mp_import = self.commit(files, [normal_import, self.mp], "import multiplayer")
        imports = {"imports": [
            {"target": "runtimes/th10", "head": self.normal, "importCommit": normal_import},
            {"target": "runtimes/th10-mp", "head": self.mp, "importCommit": mp_import},
        ]}
        moves = {"sources": [{"target": "ports/th10", "head": self.normal}, {"target": "ports/th10-mp", "head": self.mp}]}
        self.files = {path.replace("runtimes/", "ports/"): value for path, value in files.items()}
        self.files.update({
            "docs/migration/import-sources.json": json.dumps(imports),
            "docs/migration/move-sources-20261018.json": json.dumps(moves),
            "launcher/config/workspace.json": json.dumps({"repositories": {"th10": "ports/th10", "th10mp": "ports/th10-mp"}}),
        })
        self.migration = self.commit(self.files, [mp_import], "move directories")
        self.files["ports/th10/value.txt"] = "start\nbig\nend\n"
        self.files["ports/th10-mp/value.txt"] = "start\nbig\nend\n"
        self.files["launcher.txt"] = "global launcher change\n"
        self.files["common/shared.txt"] = "shared runtime change\n"
        self.global_change = self.commit(self.files, [self.migration], "change every directory")
        self.run_git("update-ref", "refs/heads/main", self.global_change)

    def run_git(self, *args, data=None):
        return subprocess.check_output(["git", "-C", str(self.repository), *args], input=data)

    def tree(self, files):
        children = {}
        for path, value in files.items():
            first, separator, rest = path.partition("/")
            if separator:
                children.setdefault(first, {})[rest] = value
            else:
                children[first] = value
        entries = []
        for name, value in children.items():
            if isinstance(value, dict):
                mode, kind, identity = "040000", "tree", self.tree(value)
            else:
                identity = self.run_git("hash-object", "-w", "--stdin", data=value.encode() if isinstance(value, str) else value).decode().strip()
                mode, kind = "100644", "blob"
            entries.append(f"{mode} {kind} {identity}\t{name}\n")
        return self.run_git("mktree", data="".join(entries).encode()).decode().strip()

    def commit(self, files, parents, message):
        tree = self.tree(files)
        return self.run_git("commit-tree", tree, *[arg for parent in parents for arg in ("-p", parent)], data=(message + "\n").encode()).decode().strip()

    def project(self, head="HEAD", write=False):
        return module.project(self.reader, head, "th10", write)

    def test_original_ancestry_and_global_changes_remain_separate(self):
        first = self.project()
        a, b = (first["results"][title] for title in ("th10", "th10mp"))
        self.assertEqual(first["mergeBases"], [self.base])
        self.assertTrue(self.reader.ancestor(self.normal, a["head"]))
        self.assertTrue(self.reader.ancestor(self.mp, b["head"]))
        self.assertFalse(self.reader.ancestor(self.mp, a["head"]))
        self.assertFalse(self.reader.ancestor(self.launcher, a["head"]))
        for result in (a, b):
            self.assertTrue(result["treeMatches"])
            self.assertIn(self.global_change, [entry["source"] for entry in result["mapping"]])
        self.assertNotEqual(a["head"], b["head"])
        self.assertEqual(self.project()["results"], first["results"])

    def test_native_merge_writeback_and_next_update(self):
        report = self.project(write=True)
        a, b = (report["results"][title]["head"] for title in ("th10", "th10mp"))
        merged_tree = self.run_git("merge-tree", "--write-tree", b, a).decode().splitlines()[0]
        merged = self.run_git("commit-tree", merged_tree, "-p", b, "-p", a, data=b"sync upstream\n").decode().strip()
        self.assertTrue(self.reader.ancestor(a, merged))
        self.assertTrue(self.reader.ancestor(b, merged))
        for path in list(self.files):
            if path.startswith("ports/th10-mp/"):
                del self.files[path]
        names = self.run_git("ls-tree", "-r", "--name-only", "-z", merged_tree).decode().strip("\0").split("\0")
        for name in names:
            self.files["ports/th10-mp/" + name] = self.run_git("show", merged_tree + ":" + name)
        prepared = module.prepare_writeback(self.reader, report, merged)
        self.assertNotIn("candidateCommit", prepared)
        self.assertEqual(self.run_git("rev-parse", "HEAD").decode().strip(), self.global_change)
        self.assertEqual(self.reader.subtree(prepared["tree"], "ports/th10-mp"), merged_tree)
        self.assertTrue(all(path.startswith("ports/th10-mp/") for path in prepared["changed"]))
        prepared = module.prepare_writeback(self.reader, report, merged, create_commit=True)
        written = prepared["candidateCommit"]
        self.run_git("read-tree", "HEAD")
        self.run_git("checkout-index", "--all")
        self.run_git("update-index", "--refresh")
        self.run_git("merge", "--ff-only", written)
        after = self.project(write=True)
        self.assertEqual(after["results"]["th10"]["head"], a)
        self.assertEqual(after["results"]["th10mp"]["head"], merged)
        self.assertEqual(int(after["aheadBehind"].split()[0]), 0)
        self.files["ports/th10/new.txt"] = "next upstream change\n"
        newer = self.commit(self.files, [written], "new upstream commit")
        self.run_git("update-ref", "refs/heads/main", newer)
        next_report = self.project(write=True)
        self.assertEqual(int(next_report["aheadBehind"].split()[0]), 1)
        self.assertEqual(next_report["results"]["th10mp"]["head"], merged)

    def test_incomplete_merge_cannot_advance_the_fork(self):
        report = self.project()
        with self.assertRaisesRegex(RuntimeError, "does not include current projection"):
            module.prepare_writeback(self.reader, report, report["results"]["th10mp"]["head"])

    def test_native_tracking_and_upstream_conflict(self):
        report = module.project(self.reader, "HEAD", "th10", True, True)
        upstream = self.run_git("rev-parse", "port-history/th10mp@{upstream}").decode().strip()
        self.assertEqual(upstream, report["results"]["th10"]["head"])
        status = self.run_git("for-each-ref", "--format=%(upstream:track)", "refs/heads/port-history/th10mp").decode().strip()
        self.assertIn("ahead", status)
        self.assertIn("behind", status)
        self.run_git("config", "branch.port-history/th10mp.merge", "refs/heads/main")
        with self.assertRaisesRegex(RuntimeError, "Existing upstream differs"):
            module.project(self.reader, "HEAD", "th10", True, True)

    def test_existing_projection_refs_cannot_rewind(self):
        before = self.project(write=True)
        with self.assertRaisesRegex(RuntimeError, "rewrite existing history"):
            self.project(self.migration, write=True)
        for result in before["results"].values():
            self.assertEqual(self.run_git("rev-parse", result["ref"]).decode().strip(), result["head"])


if __name__ == "__main__":
    unittest.main()
