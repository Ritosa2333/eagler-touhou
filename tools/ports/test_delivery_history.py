import hashlib
import importlib.util
import json
import pathlib
import subprocess
import tempfile
import unittest

spec = importlib.util.spec_from_file_location("history", pathlib.Path(__file__).with_name("check-history.py"))
history = importlib.util.module_from_spec(spec)
spec.loader.exec_module(history)


class DeliveryHistoryTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = pathlib.Path(self.temp.name)
        self.repo = history.Repository(self.root)
        self.git("init", "-q")
        self.git("config", "user.name", "Test")
        self.git("config", "user.email", "test@example.invalid")
        self.digest = hashlib.sha256(b"original source").hexdigest()
        self.manifest = b'{"game.cpp":"' + self.digest.encode() + b'"}\n'
        self.write("archive/th17-20261030/FILE-SHA256.json", self.manifest)
        self.write("ports/th17/game.cpp", b"adapted source")
        self.record = {"schema": "eagler-touhou/source-snapshot/1", "sourceKind": "development-delivery-without-git",
                       "sourceCommit": None, "destination": "ports/th17",
                       "sourceManifest": {"path": "archive/th17-20261030/FILE-SHA256.json", "sha256": hashlib.sha256(self.manifest).hexdigest()},
                       "files": [{"source": "game.cpp", "destination": "ports/th17/game.cpp", "sha256": self.digest}]}

    def git(self, *args):
        return subprocess.check_output(["git", "-C", str(self.root), *args], stderr=subprocess.PIPE)

    def write(self, name, data):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(data)

    def check(self):
        self.write("docs/migration/th17-eagler-20261030.json", json.dumps(self.record).encode())
        self.git("add", ".")
        self.git("commit", "-qm", "delivery")
        return history.delivery_snapshots(self.repo, "HEAD", {"th17": "ports/th17"})

    def test_adapted_source_keeps_original_delivery_identity(self):
        self.assertEqual(self.check()[0]["files"][0]["sha256"], self.digest)

    def test_changed_original_manifest_is_rejected(self):
        self.write("archive/th17-20261030/FILE-SHA256.json", b"{}")
        with self.assertRaisesRegex(RuntimeError, "manifest changed"):
            self.check()

    def test_checksum_claim_must_match_original_manifest(self):
        self.record["files"][0]["sha256"] = "a" * 64
        with self.assertRaisesRegex(RuntimeError, "checksum mismatch"):
            self.check()

    def test_delivery_cannot_invent_upstream_ancestry(self):
        self.record["sourceCommit"] = "a" * 40
        with self.assertRaisesRegex(RuntimeError, "cannot claim Git ancestry"):
            self.check()

    def test_destination_cannot_escape_port_or_archive(self):
        self.record["files"][0]["destination"] = "launcher/game.cpp"
        with self.assertRaisesRegex(RuntimeError, "escaped"):
            self.check()

    def test_unicode_archive_names_use_unquoted_git_paths(self):
        destination = "archive/th17-20261030/中文记录.md"
        self.write(destination, b"original record")
        self.record["files"][0]["destination"] = destination
        self.assertEqual(self.check()[0]["files"][0]["destination"], destination)

    def register_base(self):
        paths = {"docs/migration/import-sources.json": {"imports": []},
                 "docs/migration/move-sources-20261018.json": {"sources": []},
                 "launcher/config/workspace.json": {"repositories": {"th17": "ports/th17"}}}
        for path, data in paths.items():
            self.write(path, json.dumps(data).encode())
        self.git("add", "--", *paths)
        self.git("commit", "-qm", "base")
        return self.git("rev-parse", "HEAD").decode().strip()

    def test_new_port_is_registered_by_delivery_snapshot(self):
        base = self.register_base()
        self.check()
        self.assertEqual(history.verify(self.repo, base=base)["deliverySnapshots"], ["ports/th17"])

    def test_replacing_retained_delivery_identity_is_rejected(self):
        self.register_base()
        self.check()
        base = self.git("rev-parse", "HEAD").decode().strip()
        self.record["source"] = "different-delivery"
        self.check()
        with self.assertRaisesRegex(RuntimeError, "Retained delivery"):
            history.verify(self.repo, base=base)


if __name__ == "__main__":
    unittest.main()
